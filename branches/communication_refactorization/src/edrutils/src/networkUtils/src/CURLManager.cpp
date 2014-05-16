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
	//! Zbiór uchwytów
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
				// próbujemy pausowaæ
				ScopedLock lock(pauseSync);
			}

			//iloœæ przetwarzanych transferów
			int running_handles = 0;

			{
				//w³aœciwe przetwarzanie
				ScopedLock lock(sync);

				CURLMcode ret = CURLM_OK;

				//najpierw próbujemy usuwaæ
				for (auto it = toRemoveCurlsSet.begin(); it != toRemoveCurlsSet.end(); ++it){
					auto curl = *it;
					if (curl_multi_remove_handle(multi, curl) == CURLM_OK){
						manager->onRemoveRequest(curl);
					}
				}

				//czuszczê kolejkê do usuniêcia
				CURLsSet().swap(toRemoveCurlsSet);

				//teraz próbujemy dodawaæ
				for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
					auto curl = *it;
					if (curl_multi_add_handle(multi, curl) == CURLM_OK){
						manager->onAddRequest(curl);
					}
				}

				//czuszczê kolejkê do dodania
				CURLsSet().swap(toAddCurlsSet);

				// tak d³ugo jak curl potrzebuje pozwalam mu operowaæ na danych
				while ((ret = curl_multi_perform(multi, &running_handles)) == CURLM_CALL_MULTI_PERFORM);

				// curl zakoñczy³ przetwarzanie dostêpnych danych - mamy chwilê na ich przetworzenie
				if (ret != CURLM_OK){
					// wewnêtrzny/krytyczny error przy obs³udze zleceñ - koñczê dalsze przetwarzanie
					break;
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

				//usuwamy zakoñczone
				for (auto it = toRemove.begin(); it != toRemove.end(); ++it){
					curl_multi_remove_handle(multi, *it);
				}
			}

			if (running_handles > 0){
				// czekamy a¿ coœ siê zacznie dziaæ z danymi uchwytów
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
	//! Obiekt realizuj¹cy pauze przetwarzania
	threadingUtils::RecursiveSyncPolicy pauseSync;
	//! Obiekt realizuj¹cy czekanie na nowe uchwyty
	threadingUtils::StrictSyncPolicy waitSync;
	//!
	threadingUtils::ConditionVariable condVar;
	//! Informacja czy mamy pauze
	bool pause_;
	//! Obiekt do synchronizacji stanu obiektu
	mutable threadingUtils::RecursiveSyncPolicy sync;
	//! Uchwyt do interfejsu multi realizujacego po³¹czenia
	CURLM * multi;
	//! Manager dla którego funkcjonalnoœc realizujemy
	CURLManager * manager;
	//! Czy koñczyæ przetwarzanie
	volatile bool finish_;
	//! Zbiór uchwytów do dodania
	CURLsSet toAddCurlsSet;
	//! Zbiór uchwytów do usuniêcia
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