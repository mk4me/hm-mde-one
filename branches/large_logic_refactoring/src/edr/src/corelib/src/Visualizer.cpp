#include "CoreLibPCH.h"
#include <utils/SynchronizationPolicies.h>

using namespace core;
using namespace plugin;

class Visualizer::VisualizerHelper : public IDataManagerReader::IObjectObserver
{
public:
	VisualizerHelper(Visualizer * visualizer) : visualizer_(visualizer), liveObserve(false) {}

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
	Visualizer * visualizer_;
	//! Czy live observe jest aktywne
	bool liveObserve; 
	std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataSeriesToObserve;
	std::map<ObjectWrapperConstPtr, std::list<Visualizer::VisualizerSerie*>> dataToUpdate;
};

Visualizer::VisualizerSerie::VisualizerSerie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase)
	: visualizer_(visualizer), serie_(serieBase), timeAvareSerieFeatures_(dynamic_cast<IVisualizer::ITimeAvareSerieFeatures*>(serieBase))
{
	timeEditableSerieFeatures_ = (timeAvareSerieFeatures_ != nullptr ? dynamic_cast<IVisualizer::ITimeEditableSerieFeatures*>(serieBase) : nullptr);
}

Visualizer::VisualizerSerie::~VisualizerSerie()
{

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

Visualizer::Visualizer( const plugin::IVisualizer* proto, IDataManagerReader * dmr, IVisualizerManager * visManager ) :
	visualizer_(proto->create()),
    widget(nullptr),
	dmr(dmr), visManager(visManager),
	activeSerie(nullptr)
{
	init();
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
	visualizer_(visualizer.visualizer_->create()), 
	widget(nullptr),
	dmr(visualizer.dmr), visManager(visualizer.visManager)
{
	init();

	setLiveObserveActive(visualizer.isLiveObserveActive());
}

void Visualizer::init()
{
	{
		scoped_ptr<QIcon> i(visualizer_->createIcon());
		icon = *i;
	}

	TypeInfoList types;
	visualizer_->getSupportedTypes(types);
	supportedTypes.insert(types.begin(),types.end());

	visualizerHelper_.reset(new VisualizerHelper(this));
	dmr->addObserver(visualizerHelper_);
	visManager->registerVisualizer(this, visualizer_.get());
}

Visualizer::~Visualizer()
{
	dmr->removeObserver(visualizerHelper_);
	visManager->unregisterVisualizer(this);
	destroyAllSeries();
}

QWidget* Visualizer::getOrCreateWidget()
{
    if (!widget) {
		//PLUGIN_LOG_DEBUG("Creating Visualizer " << visualizer_->getName() <<  " widget");
		widget = visualizer_->createWidget();
        UTILS_ASSERT(widget, "Nie udało się stworzyć widgeta.");
    }
    return widget;
}

const QIcon Visualizer::getIcon() const
{
	return icon;
}

QWidget* Visualizer::getWidget()
{
    return widget;
}

const std::string Visualizer::getName() const
{
	return visualizer_->getName();
}

core::UniqueID Visualizer::getID() const
{
	return visualizer_->getID();
}

const std::string  Visualizer::getDescription() const
{
	return visualizer_->getDescription();
}

Visualizer * Visualizer::create() const
{
	return new Visualizer(*this);
}

const int Visualizer::getMaxSeries() const
{
	return visualizer_->getMaxDataSeries();
}

void Visualizer::getSupportedTypes(TypeInfoSet & supportedTypes) const
{
	TypeInfoList list;
	visualizer_->getSupportedTypes(list);
	supportedTypes.insert(list.begin(), list.end());
}

Visualizer::VisualizerSerie * Visualizer::createSerie(const TypeInfo & requestedType, const ObjectWrapperConstPtr & data)
{
	VisualizerSerie * serie = nullptr;
    auto s = visualizer_->createSerie(requestedType, data);
	if(s != nullptr){
		serie = new VisualizerSerie(this, s);
		dataSeries.push_back(serie);
		visualizerHelper_->addSerieToObserve(serie);
		notifyChange(serie, ADD_SERIE);
	}

	return serie;
}

Visualizer::VisualizerSerie * Visualizer::createSerie(VisualizerSerie * serie)
{
	VisualizerSerie * retserie = nullptr;
	if(serie->visualizer_ == this){
		auto s = visualizer_->createSerie(serie->serie());
		if(s != nullptr){
			retserie = new VisualizerSerie(this, s);
			dataSeries.push_back(retserie);
			visualizerHelper_->addSerieToObserve(serie);
			notifyChange(serie, ADD_SERIE);
		}
	}

	return retserie;
}

void Visualizer::destroySerie(VisualizerSerie * serie)
{
	if(serie->visualizer_ == this){
		notifyChange(serie, REMOVE_SERIE);
		dataSeries.remove(serie);
		visualizerHelper_->removeSerieToObserve(serie);
		delete serie;
	}
}

void Visualizer::notifyChange(VisualizerSerie * serie, SerieModyfication modyfication)
{
	for(auto it = observers_.begin(); it != observers_.end(); ++it){
		(*it)->update(serie, modyfication);
	}
}

void Visualizer::destroyAllSeries()
{
    while(dataSeries.empty() == false){
		destroySerie(*(dataSeries.begin()));
    }

    DataSeries().swap(dataSeries);
}

void Visualizer::addObserver(IVisualizerObserver * observer)
{
	observers_.push_back(observer);
}

void Visualizer::removeObserver(IVisualizerObserver * observer)
{
	observers_.remove(observer);
}

void Visualizer::onScreenshotTrigger()
{
	if(widget != nullptr){
		emit screenshotTaken(visualizer_->takeScreenshot());
	}
}

void Visualizer::addDataSource(VisualizerDataSourcePtr dataSource)
{
	visualizerHelper_->addDataSource(dataSource);
}

void Visualizer::removeDataSource(VisualizerDataSourcePtr dataSource)
{
	visualizerHelper_->removeDataSource(dataSource);
}

const int Visualizer::getNumDataSources() const
{
	return visualizerHelper_->getNumDataSources();
}

Visualizer::VisualizerDataSourcePtr Visualizer::getDataSource(int idx)
{
	return visualizerHelper_->getDataSource(idx);
}

void Visualizer::getData(const TypeInfo & type, ConstObjectsList & objects, bool exact)
{
	visualizerHelper_->getData(type, objects, exact);
}

void Visualizer::setLiveObserveActive(bool active)
{
	visualizerHelper_->setLiveObserveActive(active);
}

const bool Visualizer::isLiveObserveActive() const
{
	return visualizerHelper_->isLiveObserveActive();
}

const int Visualizer::getNumSeries() const
{
	return dataSeries.size();
}

Visualizer::VisualizerSerie * Visualizer::getSerie(int idx)
{
	auto it = dataSeries.begin();
	std::advance(it, idx);
	return *it;
}

const int Visualizer::serieIdx(VisualizerSerie * serie) const
{
	auto it = std::find(dataSeries.begin(), dataSeries.end(), serie);
	if(it == dataSeries.end()){
		return -1;
	}

	return std::distance(dataSeries.begin(), it);
}

void Visualizer::setActiveSerie(VisualizerSerie * serie)
{
	if(serie == nullptr){
		visualizer_->setActiveSerie(nullptr);
	}else{
		visualizer_->setActiveSerie(serie->serie());
	}

	activeSerie = serie;
}

const Visualizer::VisualizerSerie * Visualizer::getActiveSerie() const
{
	return activeSerie;
}

Visualizer::VisualizerSerie * Visualizer::getActiveSerie()
{
	return activeSerie;
}