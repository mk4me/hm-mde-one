#include "hmmPCH.h"
#include "TreeItemHelper.h"
#include "VisualizerManager.h"
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>


HmmTreeItem::HmmTreeItem( TreeItemHelperPtr helper ) :
helper(helper)
{

}

void HmmTreeItem::setItemAndHelperText( const QString& text )
{
    helper->setText(text);
    QTreeWidgetItem::setText(0, text);
}

ChildrenVisualizers::ChildrenVisualizers( PlacePolicy policy /*= Auto*/) :
    policy(policy)
{
}

TreeWrappedItemHelper::TreeWrappedItemHelper( const core::ObjectWrapperConstPtr & wrapper ) :
    wrapper(wrapper)
{
}

VisualizerPtr TreeWrappedItemHelper::createVisualizer()
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    return VisualizerManager::getInstance()->createVisualizer(wrapper->getTypeInfo());
}

void TreeWrappedItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    UTILS_ASSERT(wrapper, "Item should be initialized");
    auto serie = visualizer->createSerie(wrapper, path.toStdString());
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serie));
}

std::vector<core::TypeInfo> TreeWrappedItemHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(wrapper->getTypeInfo());
    return ret;
}

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
	std::vector<core::VisualizerTimeSeriePtr> tmpSeries;

	try{
		if (motion->hasObjectOfType(typeid(MarkerCollection))) {
			core::ObjectWrapperConstPtr m = motion->getWrapperOfType(typeid(MarkerCollection));
			if(m->getRawPtr() != nullptr){
				tmpSeries.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(m ,path.toStdString())));
			}else{
				throw std::runtime_error("Empty object - markers");
			}
		}
		if (motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection))) {
			core::ObjectWrapperConstPtr j = motion->getWrapperOfType(typeid(kinematic::JointAnglesCollection));
			if(j->getRawPtr() != nullptr){
				tmpSeries.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(j ,path.toStdString())));
			}else{
				throw std::runtime_error("Empty object - joints");
			}
		}
		if (motion->hasObjectOfType(typeid(GRFCollection))) {
			core::ObjectWrapperConstPtr g = motion->getWrapperOfType(typeid(GRFCollection));
			if(g->getRawPtr() != nullptr){
				tmpSeries.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(g ,path.toStdString())));
			}else{
				throw std::runtime_error("Empty object - grfs");
			}
		}
	}catch(...){

	}

	series.insert(series.end(), tmpSeries.begin(), tmpSeries.end());
}

VisualizerPtr Multiserie3D::createVisualizer()
{
    return VisualizerManager::getInstance()->createVisualizer(typeid(kinematic::JointAnglesCollection));
}

std::vector<core::TypeInfo> Multiserie3D::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(kinematic::JointAnglesCollection));
    ret.push_back(typeid(GRFCollection));
    ret.push_back(typeid(MarkerCollection));
    return ret;
}

Multiserie3D::Multiserie3D( const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{

}

VisualizerPtr JointsItemHelper::createVisualizer()
{
    return VisualizerManager::getInstance()->createVisualizer(typeid(kinematic::JointAnglesCollection));
}

void JointsItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    core::ObjectWrapperConstPtr joints = motion->getWrapperOfType(typeid(kinematic::JointAnglesCollection));
    if (joints && joints->getRawPtr() != nullptr) {
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(visualizer->createSerie(joints, path.toStdString())));
    } else {
        //LOG_ERROR("Empty object - joints");
        //UTILS_ASSERT(false);
        throw std::runtime_error("Empty object - joints");
    }
}

std::vector<core::TypeInfo> JointsItemHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(kinematic::JointAnglesCollection));
    return ret;
}

JointsItemHelper::JointsItemHelper( const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{
}


VisualizerPtr NewChartItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(typeid(ScalarChannelReaderInterface));
    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title;
        ScalarChannelReaderInterfacePtr scalar = wrapper->get();
        title += scalar->getName();
        title += " [";
        title += scalar->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }

    return visualizer;
}


void NewChartItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    auto serie = visualizer->createSerie(wrapper, path.toStdString());
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serie));
}

std::vector<core::TypeInfo> NewChartItemHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

NewChartItemHelper::NewChartItemHelper( const core::ObjectWrapperConstPtr& wrapper ) :
    TreeWrappedItemHelper(wrapper)
{
}

VisualizerPtr NewVector3ItemHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(typeid(ScalarChannelReaderInterface));
    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        std::string title;
        VectorChannelPtr vectorChannel = wrapper->get();
        title += vectorChannel->getName();
        title += " [";
        title += vectorChannel->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }
    return visualizer;
}

void NewVector3ItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    VectorChannelConstPtr vectorChannel = wrapper->get();

    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(vectorChannel, 0));
    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(vectorChannel, 1));
    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(vectorChannel, 2));
    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
    wrapperX->set(x);
    wrapperY->set(y);
    wrapperZ->set(z);

    static int number = 0;
    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
    std::string suffix = boost::lexical_cast<std::string>(number++);
    std::string p = path.toStdString();
    wrapperX->setName  ("X_" + suffix);
    wrapperY->setName  ("Y_" + suffix);
    wrapperZ->setName  ("Z_" + suffix);
    wrapperX->setSource(p + "/X_" + suffix);
    wrapperY->setSource(p + "/Y_" + suffix);
    wrapperZ->setSource(p + "/Z_" + suffix);
    visualizer->getOrCreateWidget();

    auto serieX = visualizer->createSerie(wrapperX, wrapperX->getName());
    auto serieY = visualizer->createSerie(wrapperY, wrapperY->getName());
    auto serieZ = visualizer->createSerie(wrapperZ, wrapperZ->getName());

    INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX.get());
    INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY.get());
    INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ.get());

    chartSerieX->setColor(QColor(255, 0, 0));
    chartSerieY->setColor(QColor(0, 255, 0));
    chartSerieZ->setColor(QColor(0, 0, 255));

    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieY));
    series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieZ));
}

NewVector3ItemHelper::NewVector3ItemHelper( const core::ObjectWrapperConstPtr& wrapper ) :
    TreeWrappedItemHelper(wrapper)
{
}

std::vector<core::TypeInfo> NewVector3ItemHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

void NewMultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
{
    int count = wrappers.size();
    for (int i = 0; i < count; ++i) {
        auto wrapper = wrappers[i].wrapper;
        auto serieX = visualizer->createSerie(wrapper, wrapper->getSource());
        if (wrappers[i].events) {
            EventSerieBasePtr eventSerie = core::dynamic_pointer_cast<EventSerieBase>(serieX);
            eventSerie->setEvents(wrappers[i].events);
        }
        INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX.get());

        chartSerieX->setColor(colorStrategy->getColor(chartSerieX, wrapper));
        series.push_back(core::dynamic_pointer_cast<core::IVisualizer::TimeSerieBase>(serieX));
    }
}


VisualizerPtr NewMultiserieHelper::createVisualizer()
{
    VisualizerPtr visualizer = VisualizerManager::getInstance()->createVisualizer(typeid(ScalarChannelReaderInterface));
    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->getImplementation());
    if (!chart) {
        UTILS_ASSERT(false);
        LOG_ERROR("Wrong visualizer type!");
    } else {
        chart->setTitle(title);
        //chart->setShowLegend(false);
    }
    return visualizer;
}

std::vector<core::TypeInfo> NewMultiserieHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

NewMultiserieHelper::NewMultiserieHelper( const ChartWithDescriptionCollection& charts ):
    wrappers(charts),
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
}

NewMultiserieHelper::NewMultiserieHelper( const std::vector<core::ObjectWrapperConstPtr>& charts ):
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
    UTILS_ASSERT(false);
    for (auto it = charts.begin(); it != charts.end(); ++it) {
        wrappers.push_back(ChartWithDescription(*it, EventsCollectionConstPtr(), PluginSubject::MotionConstPtr()));
    }
}

void TreeItemHelper::getSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::VisualizerTimeSeriePtr>& series )
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

    auto& type = typeid(C3DEventsCollection);
    if (motion && motion->hasObjectOfType(type)) {
        EventsCollectionConstPtr events = motion->getWrapperOfType(type)->get();
        BOOST_FOREACH(core::VisualizerTimeSeriePtr ptr, series) {
            EventSerieBasePtr eventPtr = boost::dynamic_pointer_cast<EventSerieBase>(ptr);
            if (eventPtr) {
                eventPtr->setEvents(events);
            }
        }
    }
}
