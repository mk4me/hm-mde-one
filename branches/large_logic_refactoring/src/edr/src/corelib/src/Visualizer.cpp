#include "CoreLibPCH.h"

using namespace core;
using namespace plugin;

class Visualizer::VisualizerHelper : public IDataManagerReader::IObjectObserver
{
public:
	VisualizerHelper(Visualizer * visualizer) : visualizer_(visualizer) {}

	virtual void observe(const IDataManagerReader::ChangeList & changes)
	{
		for(auto it = changes.begin(); it != changes.end(); ++it){
			if((*it).modyfication == IDataManagerReader::UPDATE_OBJECT){
				for(auto serieIT = visualizer_->dataSeries.begin(); serieIT != visualizer_->dataSeries.end(); ++serieIT){
					if( (*serieIT)->serie()->getData() == (*it).currentValue ){
						(*serieIT)->serie()->update();
						return;
					}
				}
			}
		}
	}

private:
	Visualizer * visualizer_;
	QAction * screenshotAction_;
};

Visualizer::VisualizerSerie::VisualizerSerie(Visualizer * visualizer, plugin::IVisualizer::ISerie * serieBase, plugin::IVisualizer::ITimeSerieFeatures * timeSerieFeatures)
	: visualizer_(visualizer), serie_(serieBase), timeSerieFeatures_(timeSerieFeatures)
{

}

Visualizer::VisualizerSerie::~VisualizerSerie()
{

}

plugin::IVisualizer::ISerie * Visualizer::VisualizerSerie::serie() const
{
	return serie_;
}

plugin::IVisualizer::ITimeSerieFeatures * Visualizer::VisualizerSerie::timeSerieFeatures() const
{
	return timeSerieFeatures_;
}

Visualizer::Visualizer( const plugin::IVisualizer* proto, IDataManagerReader * dmr, IVisualizerManager * visManager ) :
	visualizer_(proto->create()),
    widget(nullptr),
	dmr(dmr), visManager(visManager)
{
	init();
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
	visualizer_(visualizer.visualizer_->create()), 
	widget(nullptr),
	dmr(visualizer.dmr), visManager(visualizer.visManager)
{
	init();
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

Visualizer::VisualizerSerie * Visualizer::createSerie(const ObjectWrapperConstPtr & data)
{
	VisualizerSerie * serie = nullptr;
    auto s = visualizer_->createSerie(data);
	if(s != nullptr){
		serie = new VisualizerSerie(this, s, dynamic_cast<IVisualizer::ITimeSerieFeatures*>(s));
		dataSeries.insert(serie);
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
			retserie = new VisualizerSerie(this, s, serie->timeSerieFeatures() != nullptr ? dynamic_cast<IVisualizer::ITimeSerieFeatures*>(s) : nullptr);
			dataSeries.insert(retserie);
			notifyChange(serie, ADD_SERIE);
		}
	}

	return retserie;
}

void Visualizer::destroySerie(VisualizerSerie * serie)
{
	if(serie->visualizer_ == this){
		notifyChange(serie, REMOVE_SERIE);
		dataSeries.erase(serie);
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

void Visualizer::getData(const TypeInfo & type, ConstObjectsList & objects)
{
	ConstObjectsList locObjects;
	for(auto it = sources_.begin(); it != sources_.end(); ++it){
		try{
			(*it)->getData(type, locObjects);
		}catch(...){

		}
	}

	ConstObjects uniqueObjects(locObjects.begin(), locObjects.end());
	objects.insert(objects.end(), uniqueObjects.begin(), uniqueObjects.end());
}