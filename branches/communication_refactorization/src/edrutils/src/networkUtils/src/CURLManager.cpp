#include <networkUtils/CURLManager.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <set>
#include <list>

using namespace networkUtils;
using namespace utils;

class CURLManager::CURLManagerImpl
{
private:
	//! ScopedLock
	typedef threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> ScopedLock;
	//! Zbi�r uchwyt�w
	typedef std::set<CURL*> CURLsSet;

public:
	CURLManagerImpl(CURLM * multi, CURLManager * manager) : multi(multi),
		manager(manager), finish_(false)
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
		finish();
		pause(false);
		condVar.wakeOne();
	}

	void addRequest(CURL * curl)
	{
		ScopedLock lock(sync);

		toAddCurlsSet.insert(curl);
		toRemoveCurlsSet.erase(curl);

		condVar.wakeOne();
	}

	void removeRequest(CURL * curl)
	{
		ScopedLock lock(sync);

		toAddCurlsSet.erase(curl);
		toRemoveCurlsSet.insert(curl);
	}

	void pause(const bool pause)
	{
		ScopedLock lock(sync);
		if (pause_ != pause){
			pause_ = pause;
			if (pause_ == true){
				pauseSync.lock();
			}
			else{
				pauseSync.unlock();
			}
		}
	}

	const bool paused() const
	{
		ScopedLock lock(sync);
		return pause_;
	}

	void finish()
	{
		finish_ = true;
	}

	const bool finished() const
	{
		return finish_;
	}

	void process()
	{
		while (finish_ == false){
			{
				// pr�bujemy pausowa�
				ScopedLock lock(pauseSync);
			}

			//ilo�� przetwarzanych transfer�w
			int running_handles = 0;

			{
				//w�a�ciwe przetwarzanie
				ScopedLock lock(sync);

				CURLMcode ret = CURLM_OK;

				//najpierw pr�bujemy usuwa�
				for (auto it = toRemoveCurlsSet.begin(); it != toRemoveCurlsSet.end(); ++it){
					auto curl = *it;
					if (curl_multi_remove_handle(multi, curl) == CURLM_OK){
						manager->onRemoveRequest(curl);
					}
				}

				//czuszcz� kolejk� do usuni�cia
				CURLsSet().swap(toRemoveCurlsSet);

				//teraz pr�bujemy dodawa�
				for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
					auto curl = *it;
					if (curl_multi_add_handle(multi, curl) == CURLM_OK){
						manager->onAddRequest(curl);
					}
				}

				//czuszcz� kolejk� do dodania
				CURLsSet().swap(toAddCurlsSet);

				// tak d�ugo jak curl potrzebuje pozwalam mu operowa� na danych
				while ((ret = curl_multi_perform(multi, &running_handles)) == CURLM_CALL_MULTI_PERFORM);

				// curl zako�czy� przetwarzanie dost�pnych danych - mamy chwil� na ich przetworzenie
				if (ret != CURLM_OK){
					// wewn�trzny/krytyczny error przy obs�udze zlece� - ko�cz� dalsze przetwarzanie
					break;
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
						// jaki jest stan uchwytu
						switch (info->data.result){
						case CURLE_OK:
							manager->onFinishRequest(info->easy_handle);
							break;
						case CURLE_ABORTED_BY_CALLBACK:
							manager->onCancelRequest(info->easy_handle);
							break;
						default:
							manager->onErrorRequest(info->easy_handle);
							break;
						}
						toRemove.push_back(info->easy_handle);
					}
				}

				//usuwamy zako�czone
				for (auto it = toRemove.begin(); it != toRemove.end(); ++it){
					curl_multi_remove_handle(multi, *it);
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
		}
	}

private:
	//! Czas oczekiwania na dane [ms]
	int waitTime;
	//! Obiekt realizuj�cy pauze przetwarzania
	threadingUtils::RecursiveSyncPolicy pauseSync;
	//! Obiekt realizuj�cy czekanie na nowe uchwyty
	threadingUtils::StrictSyncPolicy waitSync;
	//!
	threadingUtils::ConditionVariable condVar;
	//! Informacja czy mamy pauze
	bool pause_;
	//! Obiekt do synchronizacji stanu obiektu
	mutable threadingUtils::RecursiveSyncPolicy sync;
	//! Uchwyt do interfejsu multi realizujacego po��czenia
	CURLM * multi;
	//! Manager dla kt�rego funkcjonalno�c realizujemy
	CURLManager * manager;
	//! Czy ko�czy� przetwarzanie
	volatile bool finish_;
	//! Zbi�r uchwyt�w do dodania
	CURLsSet toAddCurlsSet;
	//! Zbi�r uchwyt�w do usuni�cia
	CURLsSet toRemoveCurlsSet;
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

void CURLManager::addRequest(CURL * curl)
{
	impl->addRequest(curl);
}

void CURLManager::removeRequest(CURL * curl)
{
	impl->removeRequest(curl);
}

void CURLManager::pause(const bool paused)
{
	impl->pause(paused);
}

const bool CURLManager::paused() const
{
	return impl->paused();
}

void CURLManager::finalize()
{
	impl->finish();
}

const bool CURLManager::finalized() const
{
	return impl->finished();
}

void CURLManager::processThread()
{
	impl->process();
}