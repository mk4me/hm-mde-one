#include <networkUtils/CURLManager.h>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <map>
#include <set>
#include <list>

using namespace networkUtils;
using namespace utils;


class CURLManager::WaitCurl::WaitCurlImpl
{
	friend class CURLManagerImpl;

public:
	//! Konstruktor domyslny
	WaitCurlImpl() : result_(CURLE_OK) {}
	//! Destruktor
	~WaitCurlImpl() { wait_.notify_all(); }

	void wait()
	{
		std::mutex sync;
		std::unique_lock<std::mutex> lock(sync);
		wait_.wait(lock);
	}

	//! \return Rezultat obs³ugi po³¹czenia
	const CURLcode result() const
	{
		return result_;
	}

	//! \return Rezultat obs³ugi po³¹czenia
	void setResult(const CURLcode result)
	{
		result_ = result;
	}

	const std::string error() const
	{
		return error_;
	}

	void setError(const std::string error)
	{
		error_ = error;
	}

private:
	//! Obiekt synchronizuj¹cy
	std::condition_variable wait_;
	//! Rezultat obs³ugi po³aczenia
	std::atomic<CURLcode> result_;
	//! Opis b³êdu
	std::string error_;
};

CURLManager::WaitCurl::WaitCurl() : impl(new CURLManager::WaitCurl::WaitCurlImpl)
{

}

CURLManager::WaitCurl::~WaitCurl()
{

}

void CURLManager::WaitCurl::wait()
{
	impl->wait();
}

const CURLcode CURLManager::WaitCurl::result() const
{
	return impl->result();
}

const std::string CURLManager::WaitCurl::error() const
{
	return impl->error();
}

class CURLManager::CURLManagerImpl
{
private:
	//! ScopedLock
	typedef std::lock_guard<std::recursive_mutex> ScopedLock;
	//! Zbiór uchwytów
	typedef std::set<CURL*> CURLsSet;
	//! Mapa uchwytów i obiektów na których czekamy
	typedef std::map<CURL*, CURLManager::WaitCurl*> CURLsWaitMap;

private:

	void tryUnlockAndRemove(CURL * curl)
	{
		auto IT = currentCurls.find(curl);
		if (IT != currentCurls.end()){
			if (IT->second != nullptr){
				IT->second->impl->wait_.notify_all();
			}

			currentCurls.erase(IT);
		}
		
		manager->onRemoveRequest(curl);
	}

	void innerRemove()
	{
		for (auto it = toRemoveCurlsSet.begin(); it != toRemoveCurlsSet.end(); ++it){
			auto curl = *it;
			if (curl_multi_remove_handle(multi, curl) == CURLM_OK){
				tryUnlockAndRemove(curl);
			}
		}
	}



public:
	CURLManagerImpl(CURLM * multi, CURLManager * manager, const bool releaseCurl) : multi(multi),
		manager(manager), finalize_(false), releaseCurl(releaseCurl)
	{
		if (multi == nullptr){
			throw std::runtime_error("Invalid multi handle");
		}

		if (manager == nullptr){
			throw std::runtime_error("Invalid manager");
		}
	}

	~CURLManagerImpl()
	{
		if (releaseCurl == true){
			curl_multi_cleanup(multi);
		}
	}

	void finalize()
	{
		ScopedLock lock(sync);

		for (auto it = currentCurls.begin(); it != currentCurls.end(); ++it){
			toRemoveCurlsSet.insert(it->first);
		}

		innerRemove();

		finalize_ = true;
		condVar.notify_one();
	}

	void addRequest(CURL * curl, CURLManager::WaitCurl * wait)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy jest ju¿ dodany do obs³ugi?
		auto it = currentCurls.find(curl);
		if (it == currentCurls.end()){

			bool lock = true;

			// czy ju¿ go czasem nie prubuje dodaæ?
			auto IT = toAddCurlsSet.find(curl);
			if (IT != toAddCurlsSet.end()){
				// jeszcze raz dodaje ten sam uchwyt, mo¿e inny wait?
				if (IT->second != wait){

					if (IT->second != nullptr){
						// muszê poprzedni zwolniæ
						IT->second->impl->wait_.notify_all();
					}

					IT->second = wait;
				}
				else{
					lock = false;
				}
			}
			else{
				toAddCurlsSet[curl] = wait;
			}
		}
		
		// usuwam jeœli by³ w kolejce do usuniêcia
		toRemoveCurlsSet.erase(curl);

		// budzimy przetwarzanie
		condVar.notify_one();
	}

	void removeRequest(CURL * curl)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy aktualnie przetwarzany
		auto it = currentCurls.find(curl);
		if (it == currentCurls.end()){
			// czy w kolejce do dowania
			auto IT = toAddCurlsSet.find(curl);
			if(IT != toAddCurlsSet.end()){
				// czy by³ obiekt oczekujacy
				if (IT->second != nullptr){
					// zwalniamy
					IT->second->impl->wait_.notify_all();
				}

				toAddCurlsSet.erase(IT);
			}
		}
		else{
			// do usuniecia
			toRemoveCurlsSet.insert(curl);
		}
	}

	const bool process()
	{
		if (finalize_ == true){
			return false;
		}

		//iloœæ przetwarzanych transferów
		int running_handles = 0;

		{
			//w³aœciwe przetwarzanie
			ScopedLock lock(sync);

			//najpierw próbujemy usuwaæ
			innerRemove();

			//czuszczê kolejkê do usuniêcia
			CURLsSet().swap(toRemoveCurlsSet);

			//teraz próbujemy dodawaæ
			for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
				auto curl = it->first;
				if (curl_multi_add_handle(multi, curl) == CURLM_OK){
					manager->onAddRequest(curl);
					currentCurls.insert(*it);
				}
			}

			//czuszczê kolejkê do dodania
			CURLsWaitMap().swap(toAddCurlsSet);

			CURLMcode ret = CURLM_OK;

			// tak d³ugo jak curl potrzebuje pozwalam mu operowaæ na danych
			while ((ret = curl_multi_perform(multi, &running_handles)) == CURLM_CALL_MULTI_PERFORM);

			// curl zakoñczy³ przetwarzanie dostêpnych danych - mamy chwilê na ich przetworzenie
			if (ret != CURLM_OK && ret != CURLM_CALL_MULTI_PERFORM){
				// wewnêtrzny/krytyczny error przy obs³udze zleceñ - koñczê dalsze przetwarzanie
				return false;
			}

			// informujemy ¿e ju¿ przetworzyliœmy dane uchwytów
			manager->onProcess();

			// teraz sprawdzamy status uchwytów
			int messages_info = 0;
			CURLMsg * info = nullptr;

			//lista uchwytów które zakoñczy³y dzia³anie (ok, anulowano lub b³¹d)
			std::list<CURL*> toRemove;

			while ((info = curl_multi_info_read(multi, &messages_info)) != nullptr) {
				// czy uchwyt skoñczy³ ¿¹dane operacje
				if (info->msg == CURLMSG_DONE){

					auto it = currentCurls.find(info->easy_handle);
					if (it != currentCurls.end() && it->second != nullptr){
						it->second->impl->setResult(info->data.result);
					}

					// jaki jest stan uchwytu
					switch (info->data.result){
					case CURLE_OK:
						manager->onFinishRequest(info->easy_handle);
						break;
					case CURLE_ABORTED_BY_CALLBACK:
						manager->onCancelRequest(info->easy_handle);
						break;
					default:
					{
						std::string e(curl_easy_strerror(info->data.result));
						auto it = currentCurls.find(info->easy_handle);
						if (it != currentCurls.end() && it->second != nullptr){
							it->second->impl->setError(e);
						}
						manager->onErrorRequest(info->easy_handle, e);
					}
						
						break;					
					}
					toRemove.push_back(info->easy_handle);
				}
			}

			//usuwamy zakoñczone
			for (auto it = toRemove.begin(); it != toRemove.end(); ++it){
				if (curl_multi_remove_handle(multi, *it) == CURLM_OK){
					tryUnlockAndRemove(*it);
				}
			}
		}

		if (running_handles > 0){
			// czekamy a¿ coœ siê zacznie dziaæ z danymi uchwytów
			int descriptors_number = 0;
			curl_multi_wait(multi, nullptr, 0, waitTime, &descriptors_number);
		}
		else{
			//wait condition variable woken up when some new handles are added or removed
			std::unique_lock<std::mutex> lock(waitSync);
			condVar.wait(lock);
		}

		return true;
	}

private:
	//! Czy mamy ju¿ koñczyæ dzia³anie managera
	std::atomic<bool> finalize_;
	//! Czas oczekiwania na dane [ms]
	static const int waitTime = 500;
	//! Obiekt realizuj¹cy czekanie na nowe uchwyty
	std::mutex waitSync;
	//! Conditional variable
	std::condition_variable condVar;
	//! Obiekt do synchronizacji stanu obiektu
	mutable std::recursive_mutex sync;
	//! Uchwyt do interfejsu multi realizujacego po³¹czenia
	CURLM * multi;
	//! Manager dla którego funkcjonalnoœc realizujemy
	CURLManager * manager;
	//! Zbiór uchwytów do dodania
	CURLsWaitMap toAddCurlsSet;
	//! Zbiór uchwytów do usuniêcia
	CURLsSet toRemoveCurlsSet;
	//! Aktualnie obs³ugiwane uchwyty
	CURLsWaitMap currentCurls;
	//! Czy mamy usuwac uchwyt curla
	const bool releaseCurl;
};

CURLManager::CURLManager()
{
	impl.reset(new CURLManagerImpl(curl_multi_init(), this, true));
}

CURLManager::CURLManager(CURLM * multi)
{
	impl.reset(new CURLManagerImpl(multi, this, false));
}

CURLManager::~CURLManager()
{
}

void CURLManager::addRequest(CURL * curl, WaitCurl * wait)
{
	impl->addRequest(curl, wait);
}

void CURLManager::removeRequest(CURL * curl)
{
	impl->removeRequest(curl);
}

const bool CURLManager::process()
{
	return impl->process();
}

void CURLManager::finalize()
{
	impl->finalize();
}