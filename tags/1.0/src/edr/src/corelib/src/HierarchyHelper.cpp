#include "CoreLibPCH.h"
#include <corelib/HierarchyHelper.h>
#include <corelib/IVisualizerManager.h>

using namespace core;

core::HierarchyHelper::HierarchyHelper()
{
    text = QObject::tr("Create Visualizer");
}


WrappedItemHelper::WrappedItemHelper(const ObjectWrapperConstPtr & wrapper ) :
    wrapper(wrapper)
{
}

VisualizerPtr WrappedItemHelper::createVisualizer(IVisualizerManager* manager)
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    IVisualizerManager::VisualizerPrototypes prototypes;

    manager->getVisualizerPrototypes(wrapper->getTypeInfo(), prototypes, false);
    return VisualizerPtr(prototypes.front()->create());
}

void WrappedItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    auto serie = visualizer->createSerie(wrapper->getTypeInfo(), wrapper);
	serie->serie()->setName(path.toStdString());
    series.push_back(serie);
}

std::vector<TypeInfo> WrappedItemHelper::getTypeInfos() const
{
    std::vector<TypeInfo> ret;
    ret.push_back(wrapper->getTypeInfo());
    return ret;
}

void HierarchyHelper::getSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
    createSeries(visualizer, path, series);

	//filtrujemy serie -> przechodza tylko te różne od nullptr
	auto it = series.begin();
	while(it != series.end()){
		if(*it == nullptr){
			it = series.erase(it);
		}else{
			++it;
		}
	}	
    
	if(series.empty() == false && visualizer->getActiveSerie() == nullptr){
		visualizer->setActiveSerie(series.front());
	}
}