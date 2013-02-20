#include "CoreLibPCH.h"
#include <utils/SynchronizationPolicies.h>

using namespace core;
using namespace plugin;

class Visualizer::VisualizerImpl
{

private:

	class VisualizerHelper : public IDataManagerReader::IObjectObserver
	{
	public:
		VisualizerHelper(VisualizerImpl * visualizer) : visualizer_(visualizer), liveObserve(false) {}

		virtual void observe(const IDataManagerReader::ChangeList & changes)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
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

		void setLiveObserveActive(bool active)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			liveObserve = active;
			tryUpdate();

		}

		const bool isLiveObserveActive() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			return liveObserve;
		}

		void addSerieToObserve(Visualizer::VisualizerSerie* serie)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			dataSeriesToObserve[serie->serie()->getData()].push_back(serie);
		}

		void removeSerieToObserve(Visualizer::VisualizerSerie* serie)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sync);
			dataSeriesToObserve[serie->serie()->getData()].remove(serie);
			dataToUpdate[serie->serie()->getData()].remove(serie);
		}

		void addDataSource(VisualizerDataSourcePtr dataSource)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			sources_.push_back(dataSource);
		}

		void removeDataSource(VisualizerDataSourcePtr dataSource)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			sources_.remove(dataSource);
		}

		const int getNumDataSources() const
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			return sources_.size();
		}

		VisualizerDataSourcePtr getDataSource(int idx)
		{
			utils::ScopedLock<utils::StrictSyncPolicy> lock(sourcesSync);
			auto it = sources_.begin();
			std::advance(it, idx);
			return *it;
		}

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

		void tryUpdate()
		{
			if(liveObserve == true && dataToUpdate.empty() == false){
				for(auto dataIT = dataToUpdate.begin(); dataIT != dataToUpdate.end(); ++dataIT){
					for(auto serieIT = dataIT->second.begin(); serieIT != dataIT->second.end(); ++serieIT){
						(*serieIT)->serie()->update();
					}
				}

				std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>>().swap(dataToUpdate);
			}
		}

	private:
		mutable utils::StrictSyncPolicy sync;
		mutable utils::StrictSyncPolicy sourcesSync;
		//! Lista źródeł danych wizualizatora
		DataSources sources_;
		VisualizerImpl * visualizer_;
		//! Czy live observe jest aktywne
		bool liveObserve; 
		std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataSeriesToObserve;
		std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataToUpdate;
	};

	friend class VisualizerHelper;

private:

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
	
	VisualizerImpl( plugin::IVisualizerPtr vis, IDataManagerReader * dmr, IVisualizerManager * visManager ) :
		visualizer_(nullptr),
		innerVisualizer_(vis),
		widget(nullptr),
		dmr(dmr), visManager(visManager),
		activeSerie(nullptr)
	{
		init();
	}
	
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

	void setVisualizer(Visualizer * visualizer)
	{
		visualizer_ = visualizer;
		visManager->registerVisualizer(visualizer_);
	}
	
	plugin::IVisualizer * visualizer()
	{
		return innerVisualizer_.get();
	}

	const plugin::IVisualizer * visualizer() const
	{
		return innerVisualizer_.get();
	}

	VisualizerImpl::~VisualizerImpl()
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		//innerVisualizer_->setActiveSerie(nullptr);
		dmr->removeObserver(visualizerHelper_);		
		visManager->unregisterVisualizer(visualizer_);	
		destroyAllSeries();
	}

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

	const QIcon getIcon() const
	{
		return icon;
	}

	QWidget* getWidget()
	{
		return widget;
	}

	const std::string getName() const
	{
		return innerVisualizer_->getName();
	}

	core::UniqueID getID() const
	{
		return innerVisualizer_->getID();
	}

	const std::string  getDescription() const
	{
		return innerVisualizer_->getDescription();
	}

	Visualizer * create() const
	{
		return new Visualizer(plugin::IVisualizerPtr(innerVisualizer_->create()), dmr, visManager);
	}

	const int getMaxSeries() const
	{
		return innerVisualizer_->getMaxDataSeries();
	}

	void getSupportedTypes(TypeInfoSet & supportedTypes) const
	{
		TypeInfoList list;
		innerVisualizer_->getSupportedTypes(list);
		supportedTypes.insert(list.begin(), list.end());
	}

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
		}

		return serie;
	}

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

	void destroySerie(VisualizerSerie * serie)
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		if(serie->visualizer_ == visualizer_){
			notifyChange(serie, REMOVE_SERIE);
			dataSeries.remove(serie);
			visualizerHelper_->removeSerieToObserve(serie);
			innerVisualizer_->removeSerie(serie->serie());
			delete serie;
		}
	}

	void notifyChange(VisualizerSerie * serie, SerieModyfication modyfication)
	{
		for(auto it = observers_.begin(); it != observers_.end(); ++it){
			(*it)->update(serie, modyfication);
		}
	}

	void destroyAllSeries()
	{
		utils::ScopedLock<utils::RecursiveSyncPolicy> lock(sync);
		while(dataSeries.empty() == false){
			destroySerie(*(dataSeries.begin()));
		}

		DataSeries().swap(dataSeries);
	}

	void addObserver(IVisualizerObserver * observer)
	{		
		observers_.push_back(observer);
	}

	void removeObserver(IVisualizerObserver * observer)
	{
		observers_.remove(observer);
	}

	void addDataSource(VisualizerDataSourcePtr dataSource)
	{
		visualizerHelper_->addDataSource(dataSource);
	}

	void removeDataSource(VisualizerDataSourcePtr dataSource)
	{
		visualizerHelper_->removeDataSource(dataSource);
	}

	const int getNumDataSources() const
	{
		return visualizerHelper_->getNumDataSources();
	}

	Visualizer::VisualizerDataSourcePtr getDataSource(int idx)
	{
		return visualizerHelper_->getDataSource(idx);
	}

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
	shared_ptr<VisualizerHelper> visualizerHelper_;
	//! Faktyczny wizualizator dla danych
	shared_ptr<plugin::IVisualizer> innerVisualizer_;

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

	mutable utils::RecursiveSyncPolicy sync;

	Visualizer * visualizer_;
};

Visualizer::VisualizerSerie::VisualizerSerie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase)
	: visualizer_(visualizer), serie_(serieBase), timeAvareSerieFeatures_(dynamic_cast<IVisualizer::ITimeAvareSerieFeatures*>(serieBase))
{
	timeEditableSerieFeatures_ = (timeAvareSerieFeatures_ != nullptr ? dynamic_cast<IVisualizer::ITimeEditableSerieFeatures*>(serieBase) : nullptr);
}

Visualizer::VisualizerSerie::~VisualizerSerie()
{
	delete serie_;
}

Visualizer * Visualizer::VisualizerSerie::visualizer() const
{
	return visualizer_;
}

plugin::IVisualizer::ISerie * Visualizer::VisualizerSerie::serie() const
{
	return serie_;
}

plugin::IVisualizer::ITimeAvareSerieFeatures * Visualizer::VisualizerSerie::timeAvareSerieFeatures() const
{
	return timeAvareSerieFeatures_;
}

plugin::IVisualizer::ITimeEditableSerieFeatures * Visualizer::VisualizerSerie::timeEditableSerieFeatures() const
{
	return timeEditableSerieFeatures_;
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