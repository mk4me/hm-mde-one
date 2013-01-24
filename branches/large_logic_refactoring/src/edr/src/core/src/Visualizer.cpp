#include "CorePCH.h"
#include <QtGui/QAction>
#include "Visualizer.h"
#include <core/IDataManagerReader.h>
#include <core/ObjectWrapper.h>
#include <core/CoreAction.h>

using namespace core;
using namespace coreUI;
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

Visualizer::Visualizer( const plugin::IVisualizer* proto ) :
	visualizer_(proto->create()),
    widget(nullptr)
{
	init();
}

Visualizer::Visualizer( const Visualizer& visualizer ) :
	visualizer_(visualizer.visualizer_->create()), 
	widget(nullptr)
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
	getMemoryDataManager()->addObserver(visualizerHelper_);
	
	getVisualizerManager()->registerForUpdate(visualizer_.get());
}

Visualizer::~Visualizer()
{
	getVisualizerManager()->unregisterForUpdate(visualizer_.get());
	destroyAllSeries();
}

QWidget* Visualizer::getOrCreateWidget()
{
    if (!widget) {
		PLUGIN_LOG_DEBUG("Creating Visualizer " << visualizer_->getName() <<  " widget");
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

const QString Visualizer::getName() const
{
	return QString::fromUtf8(visualizer_->getName().c_str());
}

const int Visualizer::getMaxSeries() const
{
	return visualizer_->getMaxDataSeries();
}

const TypeInfoSet & Visualizer::getSupportedTypes() const
{
	return supportedTypes;
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

//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
BrowserVisualizer::BrowserVisualizer( const plugin::IVisualizer* proto ) : Visualizer(proto)
{

}

//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta.
BrowserVisualizer::BrowserVisualizer( const BrowserVisualizer& visualizer ) : Visualizer(visualizer)
{

}

void BrowserVisualizer::getData(const TypeInfo & type, ConstObjectsList & objects) const
{
	if(getSupportedTypes().find(type) != getSupportedTypes().end()){
		getDataManagerReader()->getObjects(objects, type, false);
	}
}

//! \param impl Implementacja wizualizatora. Obiekt przejmowany na własność.
DataManagebleVisualizer::DataManagebleVisualizer(const plugin::IVisualizer* proto ) : Visualizer(proto)
{

}
//! Konstuktor kopiujący - głęboka kopia. Nie kopiuje widgeta.
DataManagebleVisualizer::DataManagebleVisualizer( const DataManagebleVisualizer& visualizer ) : Visualizer(visualizer)
{

}

void DataManagebleVisualizer::getData(const TypeInfo & type, ConstObjectsList & objects) const
{
	auto it = data_.find(type);
	if(it != data_.end()){
		objects.insert(objects.end(), it->second.begin(), it->second.end());
	}
}

void DataManagebleVisualizer::addData(ObjectWrapperCollection & data)
{
	if(getSupportedTypes().find(data.getTypeInfo()) != getSupportedTypes().end()){
		data_[data.getTypeInfo()].insert(data.begin(), data.end());
	}
}

void DataManagebleVisualizer::removeData(const TypeInfo & type)
{
	data_.erase(type);
}

void DataManagebleVisualizer::removeData(const TypeInfo & type, ObjectWrapperConstPtr data)
{
	auto it = data_.find(type);
	if(it != data_.end()){
		it->second.erase(data);
	}
}