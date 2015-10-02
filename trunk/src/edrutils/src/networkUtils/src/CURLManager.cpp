#include <networkUtils/CURLManager.h>
#include <mutex>
#include <condition_variable>
#include <map>
#include <set>
#include <utils/Utils.h>
#include <utils/Debug.h>

using namespace networkUtils;
using namespace utils;

class CURLManager::CURLManagerImpl
{
private:
	//! ScopedLock
	typedef std::lock_guard<std::mutex> ScopedLock;
	//! Zbi�r uchwyt�w
	typedef std::set<CURLPtr> CURLsSet;
	//! Mapa uchwyt�w i obiekt�w na kt�rych czekamy
	typedef std::map<CURLPtr, std::promise<CURLcode>> CURLsWaitMap;

private:

	void unlockAndRemove(CURLsWaitMap::iterator it, CURLcode curlCode)
	{		
		it->second.set_value(curlCode);
		currentCurls.erase(it);		
	}

	void innerRemove()
	{
		for (auto & curl : toRemoveCurlsSet){
			auto IT = currentCurls.find(curl);
			const auto ret = curl_multi_remove_handle(multi.get(), curl.get());
			if (ret == CURLM_OK){			
				manager->onCancelRequest(curl);			
			}
			else{
				std::stringstream ss;
				ss << "Internal multi curl error code: " << ret;
				manager->onErrorRequest(curl, ss.str());
			}

			unlockAndRemove(IT, CURLE_ABORTED_BY_CALLBACK);
		}

		//czuszcz� kolejk� do usuni�cia
		CURLsSet().swap(toRemoveCurlsSet);
	}

	bool changeToContinue() const
	{
		return toAddCurlsSet.empty() == false
			|| currentCurls.empty() == false
			|| toRemoveCurlsSet.empty() == false
			|| finalize_ == true;
	}

public:
	CURLManagerImpl(CURLMPtr multi, CURLManager * manager) : multi(multi),
		manager(manager), finalize_(false), finalized_(false)
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
		
	}

	void finalize()
	{
		ScopedLock lock(sync);
		finalize_ = true;
		condVar.notify_one();
	}

	void run()
	{
		while (finalized_ == false){
			try{
				process();
			}
			catch (...){

			}
		}
	}

	std::future<CURLcode> addRequest(CURLPtr curl)
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

	void removeRequest(CURLPtr curl)
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
				manager->onRemoveRequest(curl);
				IT->second.set_value(CURLE_OK);
				toAddCurlsSet.erase(IT);
			}
		}
		else{
			// do usuniecia - anulowania
			toRemoveCurlsSet.insert(curl);
		}
		
		condVar.notify_one();
	}

	void updateHandles()
	{
		if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
		{
			ScopedLock lock(sync);
			if (toRemoveCurlsSet.empty() == false || toAddCurlsSet.empty() == false)
			{
				//najpierw pr�bujemy usuwa�
				innerRemove();

				if (finalize_ == false){

					//teraz pr�bujemy dodawa�
					for (auto it = toAddCurlsSet.begin(); it != toAddCurlsSet.end(); ++it){
						auto curl = it->first;
						if (curl_multi_add_handle(multi.get(), curl.get()) == CURLM_OK){
							manager->onAddRequest(curl);
							currentCurls.insert(std::move(*it));
						}
					}

					//czuszcz� kolejk� do dodania
					CURLsWaitMap().swap(toAddCurlsSet);
				}
			}
		}	
	}

	void process()
	{
		{
			ScopedLock lock(sync);
			if (finalize_ == true){

				finalized_ = true;

				for (auto it = currentCurls.begin(); it != currentCurls.end(); ++it){
					toRemoveCurlsSet.insert(it->first);
				}

				innerRemove();

				for (auto & t : toAddCurlsSet)
				{
					manager->onRemoveRequest(t.first);
					t.second.set_value(CURLE_OK);
				}

				//czuszcz� kolejk� do dodania
				CURLsWaitMap().swap(toAddCurlsSet);				

				return;
			}
		}

		//ilo�� przetwarzanych transfer�w
		int runningHandles = 0;
		{
			updateHandles();

			CURLMcode ret = CURLM_OK;

			// tak d�ugo jak curl potrzebuje pozwalam mu operowa� na danych
			while ((finalize_ == false) && ((ret = curl_multi_perform(multi.get(), &runningHandles)) == CURLM_CALL_MULTI_PERFORM))
			{
				updateHandles();
			};

			// curl zako�czy� przetwarzanie dost�pnych danych - mamy chwil� na ich przetworzenie
			if (ret != CURLM_OK && ret != CURLM_CALL_MULTI_PERFORM){
				// wewn�trzny/krytyczny error przy obs�udze zlece� - ko�cz� dalsze przetwarzanie
				//TODO - abort aktualnych uchwyt�w je�li nie zako�czy�y poprawnie swoich dzia�a� i lecimy dalej
				//return false;
			}

			// informujemy �e ju� przetworzyli�my dane uchwyt�w
			manager->onProcess();

			// teraz sprawdzamy status uchwyt�w
			int messagesInfo = 0;
			CURLMsg * info = nullptr;

			//rezultaty zako�czonych transfer�w
			std::vector<std::pair<CURLsWaitMap::iterator, CURLcode>> results;

			while ((info = curl_multi_info_read(multi.get(), &messagesInfo)) != nullptr) {
				// czy uchwyt sko�czy� ��dane operacje
				if (info->msg == CURLMSG_DONE){
					
					auto it = currentCurls.find(utils::dummyWrap(info->easy_handle));

					results.push_back({ it, info->data.result });

					// jaki jest stan uchwytu
					switch (info->data.result){
					case CURLE_OK:
						manager->onFinishRequest(it->first);
						break;
					case CURLE_ABORTED_BY_CALLBACK:
						manager->onCancelRequest(it->first);
						break;
					default:
						{
							std::string e(curl_easy_strerror(info->data.result));
							manager->onErrorRequest(it->first, e);
						}
						break;					
					}					
				}
			}

			//usuwamy zako�czone
			for (const auto & res : results){				
				curl_multi_remove_handle(multi.get(), res.first->first.get());
				toRemoveCurlsSet.erase(res.first->first);
				unlockAndRemove(res.first, res.second);			
			}
		}

		if (runningHandles > 0){
			// czekamy a� co� si� zacznie dzia� z danymi uchwyt�w
			int descriptorsNumber = 0;
			while ((finalize_ == false) && (descriptorsNumber == 0)){
				curl_multi_wait(multi.get(), nullptr, 0, waitTime, &descriptorsNumber);
			}
		}
		else{
			//wait condition variable woken up when some new handles are added or removed
			std::unique_lock<std::mutex> lock(waitSync);
			// warunki dodane dla spurious wakeups - jak co� jest do zrobienia to robimy
			while (changeToContinue() == false){
				condVar.wait_for(lock, std::chrono::milliseconds(waitTime), [this]{ return changeToContinue(); });
			}
		}		
	}

private:
	//! Czy mamy ju� ko�czy� dzia�anie managera
	volatile bool finalize_;
	//! Czy mamy ju� ko�czy� dzia�anie managera
	volatile bool finalized_;
	//! Czas oczekiwania na dane [ms]
	static const int waitTime = 500;
	//! Obiekt realizuj�cy czekanie na nowe uchwyty
	std::mutex waitSync;
	//! Conditional variable
	std::condition_variable condVar;
	//! Obiekt do synchronizacji stanu obiektu
	std::mutex sync;
	//! Uchwyt do interfejsu multi realizujacego po��czenia
	CURLMPtr multi;
	//! Manager dla kt�rego funkcjonalno�c realizujemy
	CURLManager * manager;
	//! Zbi�r uchwyt�w do dodania
	CURLsWaitMap toAddCurlsSet;
	//! Zbi�r uchwyt�w do usuni�cia
	CURLsSet toRemoveCurlsSet;
	//! Aktualnie obs�ugiwane uchwyty
	CURLsWaitMap currentCurls;
};

CURLManager::CURLManager() : impl(new CURLManagerImpl(utils::shared_ptr<CURLM>(curl_multi_init()), this))
{

}

CURLManager::CURLManager(utils::shared_ptr<CURLM> multi) : impl(new CURLManagerImpl(multi, this))
{
	
}

CURLManager::~CURLManager()
{
}

std::future<CURLcode> CURLManager::addRequest(CURLPtr curl)
{
	return impl->addRequest(curl);
}

void CURLManager::removeRequest(CURLPtr curl)
{
	impl->removeRequest(curl);
}

void CURLManager::run()
{
	impl->run();
}

void CURLManager::finalize()
{
	impl->finalize();
}