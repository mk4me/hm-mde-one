#include <networkUtils/CURLManager.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <boost/atomic.hpp>
#include <map>
#include <set>
#include <list>

using namespace networkUtils;
using namespace utils;


class CURLManager::WaitCurl::WaitCurlImpl
{
public:
	//! Konstruktor domyslny
	WaitCurlImpl() : result_(CURLE_OK) {}
	//! Destruktor
	~WaitCurlImpl() {}

	void wait()
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(sync_);
	}

	//! \return Rezultat obs�ugi po��czenia
	const CURLcode result() const
	{
		return result_;
	}

	//! \return Rezultat obs�ugi po��czenia
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

	void lock()
	{
		sync_.lock();
	}

	void unlock()
	{
		sync_.unlock();
	}

private:
	//! Obiekt synchronizuj�cy
	threadingUtils::StrictSyncPolicy sync_;
	//! Rezultat obs�ugi po�aczenia
	volatile CURLcode result_;
	//! Opis b��du
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
	typedef threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> ScopedLock;
	//! Zbi�r uchwyt�w
	typedef std::set<CURL*> CURLsSet;
	//! Mapa uchwyt�w i obiekt�w na kt�rych czekamy
	typedef std::map<CURL*, CURLManager::WaitCurl*> CURLsWaitMap;

private:

	void tryUnlockAndRemove(CURL * curl)
	{
		auto IT = currentCurls.find(curl);
		if (IT != currentCurls.end()){
			if (IT->second != nullptr){
				IT->second->impl->unlock();
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
	CURLManagerImpl(CURLM * multi, CURLManager * manager) : multi(multi),
		manager(manager), finalize_(false)
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
		finalize();
	}

	void finalize()
	{
		ScopedLock lock(sync);

		for (auto it = currentCurls.begin(); it != currentCurls.end(); ++it){
			toRemoveCurlsSet.insert(it->first);
		}

		innerRemove();

		condVar.wakeOne();
		finalize_ = true;
	}

	void addRequest(CURL * curl, CURLManager::WaitCurl * wait)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy jest ju� dodany do obs�ugi?
		auto it = currentCurls.find(curl);
		if (it == currentCurls.end()){

			bool lock = true;

			// czy ju� go czasem nie prubuje doda�?
			auto IT = toAddCurlsSet.find(curl);
			if (IT != toAddCurlsSet.end()){
				// jeszcze raz dodaje ten sam uchwyt, mo�e inny wait?
				if (IT->second != wait){

					if (IT->second != nullptr){
						// musz� poprzedni zwolni�
						IT->second->impl->unlock();
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

			// czy wait podano
			if ((lock == true) && (wait != nullptr))
			{
				// blokujemy czekanie
				wait->impl->lock();
			}
		}
		
		// usuwam je�li by� w kolejce do usuni�cia
		toRemoveCurlsSet.erase(curl);

		// budzimy przetwarzanie
		condVar.wakeOne();
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
				// czy by� obiekt oczekujacy
				if (IT->second != nullptr){
					// zwalniamy
					IT->second->impl->unlock();
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

		//ilo�� przetwarzanych transfer�w
		int running_handles = 0;

		{
			//w�a�ciwe przetwarzanie
			ScopedLock lock(sync);

			//najpierw pr�bujemy usuwa�
			innerRemove();

			//czuszcz� kolejk� do usuni�cia
			CURLsSet().swap(toRemoveCurlsSet);

			//teraz pr�bujemy dodawa�
			for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
				auto curl = it->first;
				if (curl_multi_add_handle(multi, curl) == CURLM_OK){
					manager->onAddRequest(curl);
					currentCurls.insert(*it);
				}
			}

			//czuszcz� kolejk� do dodania
			CURLsWaitMap().swap(toAddCurlsSet);

			CURLMcode ret = CURLM_OK;

			// tak d�ugo jak curl potrzebuje pozwalam mu operowa� na danych
			while ((ret = curl_multi_perform(multi, &running_handles)) == CURLM_CALL_MULTI_PERFORM);

			// curl zako�czy� przetwarzanie dost�pnych danych - mamy chwil� na ich przetworzenie
			if (ret != CURLM_OK && ret != CURLM_CALL_MULTI_PERFORM){
				// wewn�trzny/krytyczny error przy obs�udze zlece� - ko�cz� dalsze przetwarzanie
				return false;
			}

			// informujemy �e ju� przetworzyli�my dane uchwyt�w
			manager->onProcess();

			// teraz sprawdzamy status uchwyt�w
			int messages_info = 0;
			CURLMsg * info = nullptr;

			//lista uchwyt�w kt�re zako�czy�y dzia�anie (ok, anulowano lub b��d)
			std::list<CURL*> toRemove;

			while ((info = curl_multi_info_read(multi, &messages_info)) != nullptr) {
				// czy uchwyt sko�czy� ��dane operacje
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

			//usuwamy zako�czone
			for (auto it = toRemove.begin(); it != toRemove.end(); ++it){
				if (curl_multi_remove_handle(multi, *it) == CURLM_OK){
					tryUnlockAndRemove(*it);
				}
			}
		}

		if (running_handles > 0){
			// czekamy a� co� si� zacznie dzia� z danymi uchwyt�w
			int descriptors_number = 0;
			curl_multi_wait(multi, nullptr, 0, waitTime, &descriptors_number);
		}
		else{
			//wait condition variable woken up when some new handles are added or removed
			threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(waitSync);
			condVar.wait(&waitSync);
		}

		return true;
	}

private:

	boost::atomic<bool> finalize_;
	//! Czas oczekiwania na dane [ms]
	static const int waitTime = 500;
	//! Obiekt realizuj�cy czekanie na nowe uchwyty
	threadingUtils::StrictSyncPolicy waitSync;
	//! Conditional variable
	threadingUtils::ConditionVariable condVar;
	//! Obiekt do synchronizacji stanu obiektu
	mutable threadingUtils::RecursiveSyncPolicy sync;
	//! Uchwyt do interfejsu multi realizujacego po��czenia
	CURLM * multi;
	//! Manager dla kt�rego funkcjonalno�c realizujemy
	CURLManager * manager;
	//! Zbi�r uchwyt�w do dodania
	CURLsWaitMap toAddCurlsSet;
	//! Zbi�r uchwyt�w do usuni�cia
	CURLsSet toRemoveCurlsSet;
	//! Aktualnie obs�ugiwane uchwyty
	CURLsWaitMap currentCurls;
};

CURLManager::CURLManager()
{
	impl.reset(new CURLManagerImpl(curl_multi_init(), this));
}

CURLManager::CURLManager(CURLM * multi)
{
	impl.reset(new CURLManagerImpl(multi, this));
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