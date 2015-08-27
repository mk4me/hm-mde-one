#include <networkUtils/CURLManager.h>
#include <mutex>
#include <condition_variable>
#include <map>
#include <set>
#include <list>

using namespace networkUtils;
using namespace utils;

class CURLManager::CURLManagerImpl
{
private:
	//! ScopedLock
	typedef std::lock_guard<std::recursive_mutex> ScopedLock;
	//! Zbi�r uchwyt�w
	typedef std::set<CURL*> CURLsSet;
	//! Mapa uchwyt�w i obiekt�w na kt�rych czekamy
	typedef std::map<CURL*, std::promise<CURLcode>> CURLsWaitMap;

private:

	void tryUnlockAndRemove(CURL * curl)
	{
		currentCurls.erase(curl);		
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

	std::future<CURLcode> addRequest(CURL * curl)
	{
		ScopedLock lock(sync);

		if (finalize_ == true){
			throw std::runtime_error("Manager already finalized");
		}

		// czy jest ju� dodany do obs�ugi?
		if (currentCurls.find(curl) != currentCurls.end() ||
			toAddCurlsSet.find(curl) != toAddCurlsSet.end()){

			throw std::runtime_error("CURL handle already managed");
		}
		
		// usuwam je�li by� w kolejce do usuni�cia
		toRemoveCurlsSet.erase(curl);
		//dodajemy do kolejki
		auto f = toAddCurlsSet[curl].get_future();
		// budzimy przetwarzanie
		condVar.notify_one();
		//zwracamy przez przeniesienie
		return std::move(f);
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
				toAddCurlsSet.erase(IT);
			}
		}
		else{
			// do usuniecia
			toRemoveCurlsSet.insert(curl);
		}
		//TODO = do weryfikacji
		condVar.notify_one();
	}

	void updateHandles()
	{
		//najpierw pr�bujemy usuwa�
		innerRemove();

		//czuszcz� kolejk� do usuni�cia
		CURLsSet().swap(toRemoveCurlsSet);

		//teraz pr�bujemy dodawa�
		for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
			auto curl = it->first;
			if (curl_multi_add_handle(multi, curl) == CURLM_OK){
				manager->onAddRequest(curl);
				currentCurls.insert(std::move(*it));
			}
		}

		//czuszcz� kolejk� do dodania
		CURLsWaitMap().swap(toAddCurlsSet);
	}

	const bool process()
	{
		if (finalize_ == true){
			return false;
		}

		//ilo�� przetwarzanych transfer�w
		int running_handles = 0;

		{
			{
				//w�a�ciwe przetwarzanie
				ScopedLock lock(sync);
				updateHandles();
			}

			CURLMcode ret = CURLM_OK;

			// tak d�ugo jak curl potrzebuje pozwalam mu operowa� na danych
			while ((ret = curl_multi_perform(multi, &running_handles)) == CURLM_CALL_MULTI_PERFORM)
			{
				//TODO - wn�trzne while jest experymentalne!!
				if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
				{
					ScopedLock lock(sync);
					if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
					{
						updateHandles();
					}
				}
			};

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

			//rezultaty zako�czonych transfer�w
			std::map<CURL*, CURLcode> results;

			while ((info = curl_multi_info_read(multi, &messages_info)) != nullptr) {
				// czy uchwyt sko�czy� ��dane operacje
				if (info->msg == CURLMSG_DONE){

					results.insert({ info->easy_handle, info->data.result });

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
							manager->onErrorRequest(info->easy_handle, e);
						}
						break;					
					}					
				}
			}

			//usuwamy zako�czone
			for (const auto & res : results){
				if (curl_multi_remove_handle(multi, res.first) == CURLM_OK){

					auto cIT = currentCurls.find(res.first);
					cIT->second.set_value(res.second);
					tryUnlockAndRemove(res.first);
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
			std::unique_lock<std::mutex> lock(waitSync);
			// TODO - warunki dodane dla spurious wakeups - jak co� jest do zrobienia to robimy
			condVar.wait(lock, [this]{ return toAddCurlsSet.empty() == false
				|| currentCurls.empty() == false
				|| toRemoveCurlsSet.empty() == false
				|| finalize_ == true; });
		}

		return true;
	}

private:
	//! Czy mamy ju� ko�czy� dzia�anie managera
	volatile bool finalize_;
	//! Czas oczekiwania na dane [ms]
	static const int waitTime = 500;
	//! Obiekt realizuj�cy czekanie na nowe uchwyty
	std::mutex waitSync;
	//! Conditional variable
	std::condition_variable condVar;
	//! Obiekt do synchronizacji stanu obiektu
	mutable std::recursive_mutex sync;
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

std::future<CURLcode> CURLManager::addRequest(CURL * curl)
{
	return impl->addRequest(curl);
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