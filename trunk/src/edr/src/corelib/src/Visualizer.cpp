#include "CoreLibPCH.h"
#include <corelib/Visualizer.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/IDataManagerReader.h>
#include <threading/SynchronizationPolicies.h>

using namespace core;
using namespace plugin;

//! Klasa implementująca funkcjonalność wizualizatora w zakresie jego logiki
class Visualizer::VisualizerImpl
{

private:
	//! Klasa pomocnicza implementacji wizualizatora przy obserwowaniu zmian danych dla serii
	class VisualizerHelper : public IDataManagerReader::IObjectObserver
	{
	public:
		//! \param visualizer Wizualizaotor który wspieramy
		VisualizerHelper(VisualizerImpl * visualizer) : visualizer_(visualizer), liveObserve(false) {}

		//! \param changes Zmiany jakie zaszły w managerze danych
		virtual void observe(const IDataManagerReader::ChangeList & changes)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			
			if(liveObserve == false){
				return;
			}

			for(auto it = changes.begin(); it != changes.end(); ++it){
				if((*it).modyfication == IDataManagerReader::UPDATE_OBJECT){
					auto dataIT = dataSeriesToObserve.find((*it).currentValue);
					if(dataIT != dataSeriesToObserve.end()){
						dataToUpdate.insert(*dataIT);
					}
				}
			}

			tryUpdate();
		}

		//! \param active Czy auto odświeżanie i obserwacja danych w menagerze danych są aktywne
		void setLiveObserveActive(bool active)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			liveObserve = active;
			tryUpdate();

		}

		//! \return Czy auto odświeżanie i obserwacja danych w menagerze danych są aktywne
		const bool isLiveObserveActive() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			return liveObserve;
		}

		//! \param serie Seria której dane będą obserwowane
		void addSerieToObserve(Visualizer::VisualizerSerie* serie)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			dataSeriesToObserve[serie->serie()->getData()].push_back(serie);
		}

		//! \param serie Seria której dane nie będą obserwowane
		void removeSerieToObserve(Visualizer::VisualizerSerie* serie)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			dataSeriesToObserve[serie->serie()->getData()].remove(serie);
			dataToUpdate[serie->serie()->getData()].remove(serie);
		}

		//! \param dataSource Nowe źródło danych na potrzeby pobierania danych dostępnych dla wizualizatorów
		void addDataSource(VisualizerDataSourcePtr dataSource)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			sources_.push_back(dataSource);
		}

		//! \param dataSource Usuwane źródło danych na potrzeby pobierania danych dostępnych dla wizualizatorów
		void removeDataSource(VisualizerDataSourcePtr dataSource)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			sources_.remove(dataSource);
		}

		//! \return Ilość źródeł danych
		const int getNumDataSources() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			return sources_.size();
		}

		//! \param idx Indeks źródła danych
		//! \return Źródło danych o zadanym indeksie
		VisualizerDataSourcePtr getDataSource(int idx)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			auto it = sources_.begin();
			std::advance(it, idx);
			return *it;
		}

		//! \param type Typ danych o które pytamy
		//! \param objects [out] Lista obiektów zadanego typu z wszystkich źródeł danych
		//! \param exact Czy dane muszą być dokłądnie zadanego typu czy mogą go wspierać
		void getData(const TypeInfo & type, ConstObjectsList & objects, bool exact)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			ConstObjectsList locObjects;
			for(auto it = sources_.begin(); it != sources_.end(); ++it){
				try{
					(*it)->getData(type, locObjects, exact);
				}catch(...){

				}
			}

			ConstObjects uniqueObjects(locObjects.begin(), locObjects.end());
			objects.insert(objects.end(), uniqueObjects.begin(), uniqueObjects.end());
		}

	private:

		//! Próbujemy odświeżać dane z serii wizualizatora jeśli nastąpiła w nich jakaś zmiana
		void tryUpdate()
		{
			for(auto dataIT = dataToUpdate.begin(); dataIT != dataToUpdate.end(); ++dataIT){
				for(auto serieIT = dataIT->second.begin(); serieIT != dataIT->second.end(); ++serieIT){
					(*serieIT)->serie()->requestUpdate();
				}
			}

			std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>>().swap(dataToUpdate);
		}

	private:
		//! Obiekt synchornizujący stan
		mutable utils::StrictSyncPolicy sync;
		//! Obiekt synchronizujący operacje na źródłach
		mutable utils::StrictSyncPolicy sourcesSync;
		//! Lista źródeł danych wizualizatora
		DataSources sources_;
		//! Prywatna implementacja wizualizatora
		VisualizerImpl * visualizer_;
		//! Czy live observe jest aktywne
		bool liveObserve; 
		//! Mapa seri wizualizatorów obserwujących dane
		std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataSeriesToObserve;
		//! Mapa danych do aktualizacji (odświeżenia) w seriach
		std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataToUpdate;
	};

	//! Zaprzyjaźniona klasa pomocnicza wizualizatora
	friend class VisualizerHelper;

private:

	//! Inicjalizacja wizualizatora na bazie dostarczonej implementacji
	void init()
	{
		{
			scoped_ptr<QIcon> i(innerVisualizer_->createIcon());
			icon = *i;
		}

		TypeInfoList types;
		innerVisualizer_->getSupportedTypes(types);
		supportedTypes.insert(types.begin(),types.end());

		visualizerHelper_.reset(new VisualizerHelper(this));
		dmr->addObserver(visualizerHelper_);
	}

public:
	
	//! \param vis Wizualizator dostarczony przez klienta, opakowany przez nasz obiekt
	//! \param dmr Manager pamięci pozwalający czytać załadowane dane
	//! \param visManager Manager wizualizatorów - informujemy o usunięci/zamknięciu naszego wizualizatora
	VisualizerImpl( plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager ) :
		visualizer_(nullptr),
		innerVisualizer_(vis),
		widget(nullptr),
		dmr(dmr), visManager(visManager),
		activeSerie(nullptr)
	{
		init();
	}
	
	//! Konstruktor kopiujący
	//! \param vis Wizualizator do skopiowania
	//TODO dodać kopiowanie serii danych!!
	VisualizerImpl( const VisualizerImpl& vis ) :
		visualizer_(nullptr),
		innerVisualizer_(vis.visualizer()->create()), 
		widget(nullptr),
		dmr(vis.dmr), visManager(vis.visManager),
		activeSerie(nullptr)
	{
		init();

		setLiveObserveActive(vis.isLiveObserveActive());
	}

	//! \param visualizer Obsługiwany wizualizator
	void setVisualizer(Visualizer * visualizer)
	{
		visualizer_ = visualizer;
		visManager->registerVisualizer(visualizer_);
	}
	
	//! \return Obsługiwany wizualizator
	plugin::IVisualizer * visualizer()
	{
		return innerVisualizer_.get();
	}

	const plugin::IVisualizer * visualizer() const
	{
		return innerVisualizer_.get();
	}

	//! Destruktor
	VisualizerImpl::~VisualizerImpl()
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		//innerVisualizer_->setActiveSerie(nullptr);
		dmr->removeObserver(visualizerHelper_);		
		visManager->unregisterVisualizer(visualizer_);	
		destroyAllSeries();
	}

	//! \return Pobiera lub tworzy widget wizualizatora do pokazania w aplikacji
	QWidget* getOrCreateWidget()
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		if (!widget) {
			//PLUGIN_LOG_DEBUG("Creating Visualizer " << innerVisualizer_->getName() <<  " widget");
			widget = innerVisualizer_->createWidget();
			UTILS_ASSERT(widget, "Nie udało się stworzyć widgeta.");
		}
		return widget;
	}

	//! \return Ikona wizualizatora
	const QIcon getIcon() const
	{
		return icon;
	}

	//! \return Widget wizualizatora do pokazania w aplikacji
	QWidget* getWidget()
	{
		return widget;
	}

	//! \return Nazwa wizualizatora
	const std::string getName() const
	{
		return innerVisualizer_->getName();
	}

	//! \return Unikalny ID wizualizatora
	core::UniqueID getID() const
	{
		return innerVisualizer_->getID();
	}

	//! \return Opis wizualizatora
	const std::string  getDescription() const
	{
		return innerVisualizer_->getDescription();
	}

	//! \return Nowa instancja wizualizatora
	Visualizer * create() const
	{
		return new Visualizer(plugin::IVisualizerPtr(innerVisualizer_->create()), dmr, visManager);
	}

	//! \return Maksumalna ilość serii danych wizualizatora
	const int getMaxSeries() const
	{
		return innerVisualizer_->getMaxDataSeries();
	}

	//! \param supportedTypes Typy wspierane przez wizualizator
	void getSupportedTypes(TypeInfoSet & supportedTypes) const
	{
		TypeInfoList list;
		innerVisualizer_->getSupportedTypes(list);
		supportedTypes.insert(list.begin(), list.end());
	}

	//! \param requestedType Typ danych oczekiwanych do wyświetlenia
	//! \param data Dane
	//! \return Seria danych wizualizatora
	Visualizer::VisualizerSerie * createSerie(const TypeInfo & requestedType, const ObjectWrapperConstPtr & data)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		VisualizerSerie * serie = nullptr;
		auto s = innerVisualizer_->createSerie(requestedType, data);
		if(s != nullptr){
			serie = new VisualizerSerie(visualizer_, s);
			dataSeries.push_back(serie);
			visualizerHelper_->addSerieToObserve(serie);
			notifyChange(serie, ADD_SERIE);
			if(dataSeries.size() == 1){
				setActiveSerie(serie);
			}
		}

		return serie;
	}

	//! \param serie Seria danych do sklonowania
	//! \return sklonowana seria danych
	Visualizer::VisualizerSerie * createSerie(VisualizerSerie * serie)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		VisualizerSerie * retserie = nullptr;
		if(serie->visualizer_ == visualizer_){
			auto s = innerVisualizer_->createSerie(serie->serie());
			if(s != nullptr){
				retserie = new VisualizerSerie(visualizer_, s);
				dataSeries.push_back(retserie);
				visualizerHelper_->addSerieToObserve(serie);
				notifyChange(serie, ADD_SERIE);
			}
		}

		return retserie;
	}

	//! \param serie Seria danych do usunięcia/zniszczenia
	void destroySerie(VisualizerSerie * serie)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		if(serie->visualizer_ == visualizer_){

			if(activeSerie == serie){

				if(dataSeries.size() - 1 > 0){

					auto idx = serieIdx(serie);
					if(idx > 0){
						setActiveSerie(getSerie(idx-1));
					}else{
						setActiveSerie(getSerie(idx+1));
					}

				}else{
					setActiveSerie(nullptr);
				}
			}

			notifyChange(serie, REMOVE_SERIE);
			dataSeries.remove(serie);
			visualizerHelper_->removeSerieToObserve(serie);
			innerVisualizer_->removeSerie(serie->serie());
			delete serie;
		}
	}

	//! \param serie Seria danych do odświeżenia
	//! \param modyfication typ modyfikacji serii
	void notifyChange(VisualizerSerie * serie, SerieModyfication modyfication)
	{
		for(auto it = observers_.begin(); it != observers_.end(); ++it){
			(*it)->update(serie, modyfication);
		}
	}

	//! Metoda niszczy wszystkie serie wizualizatora
	void destroyAllSeries()
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);

		setActiveSerie(nullptr);

		while(dataSeries.empty() == false){
			destroySerie(*(dataSeries.begin()));
		}

		DataSeries().swap(dataSeries);
	}

	//! \param observer Obiekt obserwujący wizualizator
	void addObserver(IVisualizerObserver * observer)
	{		
		observers_.push_back(observer);
	}
	//! \param observer Usuwany obiekt obserwujący wizualizator
	void removeObserver(IVisualizerObserver * observer)
	{
		observers_.remove(observer);
	}
	//! \param dataSource Nowe źródło danych wizualizatora
	void addDataSource(VisualizerDataSourcePtr dataSource)
	{
		visualizerHelper_->addDataSource(dataSource);
	}
	//! \param dataSource Usuwane źródło dancyh wizualizatora
	void removeDataSource(VisualizerDataSourcePtr dataSource)
	{
		visualizerHelper_->removeDataSource(dataSource);
	}
	//! \return Ilość źródeł danych wizualizatora
	const int getNumDataSources() const
	{
		return visualizerHelper_->getNumDataSources();
	}
	//! \param idx Indeks źródła danych wizualizatora
	//! \return Źródło danych
	Visualizer::VisualizerDataSourcePtr getDataSource(int idx)
	{
		return visualizerHelper_->getDataSource(idx);
	}

	//! \param type Typ danych
	//! \param objects [out] Dane pobrane ze źróeł danych wizualizatora dla danego typu
	//! \param exact Czy dane musza być dokłądnie  tego samego typu o który pytano
	void getData(const TypeInfo & type, ConstObjectsList & objects, bool exact)
	{
		visualizerHelper_->getData(type, objects, exact);
	}

	void setLiveObserveActive(bool active)
	{
		visualizerHelper_->setLiveObserveActive(active);
	}

	const bool isLiveObserveActive() const
	{
		return visualizerHelper_->isLiveObserveActive();
	}

	const int getNumSeries() const
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		return dataSeries.size();
	}

	Visualizer::VisualizerSerie * getSerie(int idx)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		auto it = dataSeries.begin();
		std::advance(it, idx);
		return *it;
	}

	const int serieIdx(VisualizerSerie * serie) const
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		auto it = std::find(dataSeries.begin(), dataSeries.end(), serie);
		if(it == dataSeries.end()){
			return -1;
		}

		return std::distance(dataSeries.begin(), it);
	}

	void setActiveSerie(VisualizerSerie * serie)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);

		if(serie == nullptr){
			
			innerVisualizer_->setActiveSerie(nullptr);
		}else{
			innerVisualizer_->setActiveSerie(serie->serie());
		}		

		activeSerie = serie;

		notifyChange(activeSerie, Visualizer::ACTIVE_SERIE_CHANGED);
	}

	const Visualizer::VisualizerSerie * getActiveSerie() const
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		return activeSerie;
	}

	Visualizer::VisualizerSerie * getActiveSerie()
	{		
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		return activeSerie;
	}

	void update(double deltaTime)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		innerVisualizer_->update(deltaTime);
	}

private:
	//! Obiekt obserwujący dane w MemoryDM - tylko update nas interesuje aby odświeżać serię
	//! Reszta będzie robiona w widgetach na odpowiednie akcje (czy dane są jeszcze w DM)
	utils::shared_ptr<VisualizerHelper> visualizerHelper_;
	//! Faktyczny wizualizator dla danych
	utils::shared_ptr<plugin::IVisualizer> innerVisualizer_;

	//! Faktyczny widget.
	QWidget* widget;
	//! Ikona widgeta
	QIcon icon;
	//! Wspierane typy danych przez wizualizator
	TypeInfoSet supportedTypes;
	//! Serie danych utrworzone przez użytkownika
	DataSeries dataSeries;
	//! Lista obserwujących
	std::list<IVisualizerObserver*> observers_;

	//! DataManagerReader
	IDataManagerReader * dmr;
	//! VisualizerManager
	IVisualizerManager * visManager;
	//! Aktywna seria
	VisualizerSerie * activeSerie;
	//! Obiekt synchronizujący stan
	mutable utils::RecursiveSyncPolicy sync;
	//! Obsługiwany wizualizator
	Visualizer * visualizer_;
};

Visualizer::VisualizerSerie::VisualizerSerie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase)
	: visualizer_(visualizer), serie_(serieBase)
{
	
}

Visualizer::VisualizerSerie::~VisualizerSerie()
{
	delete serie_;
}

const Visualizer * Visualizer::VisualizerSerie::visualizer() const
{
	return visualizer_;
}

Visualizer * Visualizer::VisualizerSerie::visualizer()
{
	return visualizer_;
}

const plugin::IVisualizer::ISerie * Visualizer::VisualizerSerie::serie() const
{
	return serie_;
}

plugin::IVisualizer::ISerie * Visualizer::VisualizerSerie::serie()
{
	return serie_;
}

Visualizer::Visualizer(plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager ) :
	visualizerImpl(new VisualizerImpl(vis, dmr, visManager))
{
	visualizerImpl->setVisualizer(this);
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
	visualizerImpl(new VisualizerImpl(*(visualizer.visualizerImpl)))
{
	visualizerImpl->setVisualizer(this);
}

plugin::IVisualizer * Visualizer::visualizer()
{
	return visualizerImpl->visualizer();
}

const plugin::IVisualizer * Visualizer::visualizer() const
{
	return visualizerImpl->visualizer();
}

Visualizer::~Visualizer()
{
	
}

QWidget* Visualizer::getOrCreateWidget()
{
    return visualizerImpl->getOrCreateWidget();
}

const QIcon Visualizer::getIcon() const
{
	return visualizerImpl->getIcon();
}

QWidget* Visualizer::getWidget()
{
    return visualizerImpl->getWidget();
}

const std::string Visualizer::getName() const
{
	return visualizerImpl->getName();
}

core::UniqueID Visualizer::getID() const
{
	return visualizerImpl->getID();
}

const std::string  Visualizer::getDescription() const
{
	return visualizerImpl->getDescription();
}

Visualizer * Visualizer::create() const
{
	return visualizerImpl->create();
}

const int Visualizer::getMaxSeries() const
{
	return visualizerImpl->getMaxSeries();
}

void Visualizer::getSupportedTypes(TypeInfoSet & supportedTypes) const
{
	visualizerImpl->getSupportedTypes(supportedTypes);
}

Visualizer::VisualizerSerie * Visualizer::createSerie(const TypeInfo & requestedType, const ObjectWrapperConstPtr & data)
{
	return visualizerImpl->createSerie(requestedType, data);
}

Visualizer::VisualizerSerie * Visualizer::createSerie(VisualizerSerie * serie)
{
	return visualizerImpl->createSerie(serie);
}

void Visualizer::destroySerie(VisualizerSerie * serie)
{
	visualizerImpl->destroySerie(serie);
}

void Visualizer::destroyAllSeries()
{
    visualizerImpl->destroyAllSeries();
}

void Visualizer::addObserver(IVisualizerObserver * observer)
{
	visualizerImpl->addObserver(observer);
}

void Visualizer::removeObserver(IVisualizerObserver * observer)
{
	visualizerImpl->removeObserver(observer);
}

void Visualizer::onScreenshotTrigger()
{
	if(visualizerImpl->getWidget() != nullptr){
		emit screenshotTaken(visualizerImpl->visualizer()->takeScreenshot());
	}
}

void Visualizer::addDataSource(VisualizerDataSourcePtr dataSource)
{
	visualizerImpl->addDataSource(dataSource);
}

void Visualizer::removeDataSource(VisualizerDataSourcePtr dataSource)
{
	visualizerImpl->removeDataSource(dataSource);
}

const int Visualizer::getNumDataSources() const
{
	return visualizerImpl->getNumDataSources();
}

Visualizer::VisualizerDataSourcePtr Visualizer::getDataSource(int idx)
{
	return visualizerImpl->getDataSource(idx);
}

void Visualizer::getData(const TypeInfo & type, ConstObjectsList & objects, bool exact)
{
	visualizerImpl->getData(type, objects, exact);
}

void Visualizer::setLiveObserveActive(bool active)
{
	visualizerImpl->setLiveObserveActive(active);
}

const bool Visualizer::isLiveObserveActive() const
{
	return visualizerImpl->isLiveObserveActive();
}

const int Visualizer::getNumSeries() const
{
	return visualizerImpl->getNumSeries();
}

Visualizer::VisualizerSerie * Visualizer::getSerie(int idx)
{
	return visualizerImpl->getSerie(idx);
}

const int Visualizer::serieIdx(VisualizerSerie * serie) const
{
	return visualizerImpl->serieIdx(serie);
}

void Visualizer::setActiveSerie(VisualizerSerie * serie)
{
	visualizerImpl->setActiveSerie(serie);
}

const Visualizer::VisualizerSerie * Visualizer::getActiveSerie() const
{
	return visualizerImpl->getActiveSerie();
}

Visualizer::VisualizerSerie * Visualizer::getActiveSerie()
{
	return visualizerImpl->getActiveSerie();
}

void Visualizer::update(double deltaTime)
{
	visualizerImpl->update(deltaTime);
}

VisualizerMemoryDataSource::VisualizerMemoryDataSource(core::IDataManagerReader * dmr) :dmr(dmr)
{

}

void VisualizerMemoryDataSource::getData(const TypeInfo & type, ConstObjectsList & objects, bool exact) const
{
	dmr->getObjects(objects, type, exact);
}
