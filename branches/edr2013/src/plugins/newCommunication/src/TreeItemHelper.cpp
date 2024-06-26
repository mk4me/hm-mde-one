#include <plugins/newCommunication/TreeItemHelper.h>
#include <QtWidgets/QHBoxLayout>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/hmmlib/EMGFilter.h>

using namespace core;

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
	std::vector<Visualizer::VisualizerSerie*> tmpSeries;

	try{
		if (motion->hasObject(typeid(MarkerCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(MarkerCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(MarkerCollection), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw std::runtime_error("Empty object - markers");
			}
		}
		if (motion->hasObject(typeid(kinematic::JointAnglesCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(kinematic::JointAnglesCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(kinematic::JointAnglesCollection), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw std::runtime_error("Empty object - joints");
			}
		}
		if (motion->hasObject(typeid(GRFCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(GRFCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(GRFCollection), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw std::runtime_error("Empty object - grfs");
			}
		}
	}catch(...){

	}

	series.insert(series.end(), tmpSeries.begin(), tmpSeries.end());
}

VisualizerPtr Multiserie3D::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(kinematic::JointAnglesCollection), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

std::vector<utils::TypeInfo> Multiserie3D::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(kinematic::JointAnglesCollection));
    ret.push_back(typeid(GRFCollection));
    ret.push_back(typeid(MarkerCollection));
    return ret;
}

Multiserie3D::Multiserie3D(const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{

}
//
VisualizerPtr JointsItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(kinematic::JointAnglesCollection), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

void JointsItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
	ConstVariantsList m;
	motion->getObjects(m, typeid(kinematic::JointAnglesCollection), false);
    core::VariantConstPtr joints = m.front();
    if (joints && joints->getRawPtr() != nullptr) {
		auto s = visualizer->createSerie(typeid(kinematic::JointAnglesCollection), joints);
		s->serie()->setName(path.toStdString());
        series.push_back(s);
    } else {
        throw std::runtime_error("Empty object - joints");
    }
}

std::vector<utils::TypeInfo> JointsItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(kinematic::JointAnglesCollection));
    return ret;
}

JointsItemHelper::JointsItemHelper(const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{
}


VisualizerPtr NewChartItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw std::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
        ScalarChannelReaderInterfaceConstPtr scalar = wrapper->get();
        title += scalar->getName();
        title += " [";
        title += scalar->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }

    return visualizer;
}


void NewChartItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
    auto serie = visualizer->createSerie(wrapper->data()->getTypeInfo(), wrapper);
	serie->serie()->setName(path.toStdString());
    series.push_back(serie);
}

std::vector<utils::TypeInfo> NewChartItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

NewChartItemHelper::NewChartItemHelper(const core::VariantConstPtr& wrapper, const EventsCollectionConstPtr& events ) :
    WrappedItemHelper(wrapper),
    events(events)
{
}

VisualizerPtr NewVector3ItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw std::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
        VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();
        title += vectorChannel->getName();
        title += " [";
        title += vectorChannel->getValueBaseUnit();
        title += "]";
        chart->setTitle(QString(title.c_str()));
    }
    return visualizer;
}

void NewVector3ItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
    VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(vectorChannel, 0));
    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(vectorChannel, 1));
    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(vectorChannel, 2));
    core::VariantPtr wrapperX = core::Variant::create<ScalarChannelReaderInterface>();
    core::VariantPtr wrapperY = core::Variant::create<ScalarChannelReaderInterface>();
    core::VariantPtr wrapperZ = core::Variant::create<ScalarChannelReaderInterface>();
    wrapperX->set(x);
    wrapperY->set(y);
    wrapperZ->set(z);

    static int number = 0;
    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
    std::string suffix = boost::lexical_cast<std::string>(number++);
    std::string p = path.toStdString();

	wrapperX->setMetadata("core/name", "X_" + suffix);
	wrapperY->setMetadata("core/name", "Y_" + suffix);
	wrapperZ->setMetadata("core/name", "Z_" + suffix);
	wrapperX->setMetadata("core/source", p + "/X_" + suffix);
	wrapperY->setMetadata("core/source", p + "/Y_" + suffix);
	wrapperZ->setMetadata("core/source", p + "/Z_" + suffix);
    visualizer->getOrCreateWidget();

	auto serieX = visualizer->createSerie(wrapperX->data()->getTypeInfo(), wrapperX);
	serieX->serie()->setName("X_" + suffix);
	auto serieY = visualizer->createSerie(wrapperY->data()->getTypeInfo(), wrapperY);
	serieY->serie()->setName("Y_" + suffix);
	auto serieZ = visualizer->createSerie(wrapperZ->data()->getTypeInfo(), wrapperZ);
	serieZ->serie()->setName("Z_" + suffix);

    INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->serie());
    INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->serie());
    INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->serie());

    if (events) {
        chartSerieX->setEvents(events);
        chartSerieY->setEvents(events);
        chartSerieZ->setEvents(events);
    }

    chartSerieX->setColor(QColor(255, 0, 0));
    chartSerieY->setColor(QColor(0, 255, 0));
    chartSerieZ->setColor(QColor(0, 0, 255));

    series.push_back(serieX);
    series.push_back(serieY);
    series.push_back(serieZ);
}

NewVector3ItemHelper::NewVector3ItemHelper(const core::VariantConstPtr& wrapper, const EventsCollectionConstPtr& events ) :
    WrappedItemHelper(wrapper),
    events(events)
{
}

std::vector<utils::TypeInfo> NewVector3ItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

void NewMultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
    int count = wrappers.size();
    for (int i = 0; i < count; ++i) {
        auto wrapper = wrappers[i].wrapper;
		std::string source;
		wrapper->getMetadata("core/source", source);
		auto serieX = visualizer->createSerie(wrapper->data()->getTypeInfo(), wrapper);
		serieX->serie()->setName(source);
        if (wrappers[i].events) {
            EventSerieBase * eventSerie = dynamic_cast<EventSerieBase*>(serieX->serie());
            eventSerie->setEvents(wrappers[i].events);
        }
        INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->serie());

        chartSerieX->setColor(colorStrategy->getColor(chartSerieX, wrapper));
        series.push_back(serieX);
    }
}


VisualizerPtr NewMultiserieHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
    // TODO : jawnie pobrac New Chart
	manager->getVisualizerPrototypes(typeid(ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw std::runtime_error("Wrong visualizer type!");
    } else {
        chart->setTitle(title);
        //chart->setShowLegend(false);
    }
    return visualizer;
}

std::vector<utils::TypeInfo> NewMultiserieHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}

NewMultiserieHelper::NewMultiserieHelper(const ChartWithDescriptionCollection& charts ):
    wrappers(charts),
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
}

NewMultiserieHelper::NewMultiserieHelper(const std::vector<core::VariantConstPtr>& charts ):
    title(""),
    colorStrategy(new RandomMultiserieColorStrategy())
{
    UTILS_ASSERT(false);
    for (auto it = charts.begin(); it != charts.end(); ++it) {
        wrappers.push_back(ChartWithDescription(*it, EventsCollectionConstPtr(), PluginSubject::MotionConstPtr()));
    }
}



EMGFilterHelper::EMGFilterHelper( const core::VariantConstPtr& wrapper, const EventsCollectionConstPtr& events ) :
    NewChartItemHelper(wrapper, events)
{
}


void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
    ScalarChannelReaderInterfacePtr channel = wrapper->clone()->get();

    boost::shared_ptr<AbsMeanChannel<float, float>> absTest( new AbsMeanChannel<float, float>(channel));
    absTest->initialize();

    //utils::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
    utils::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, RMSModifier()));

    core::VariantPtr wrapperX = core::Variant::create<ScalarChannelReaderInterface>();
    wrapperX->set(utils::dynamic_pointer_cast<ScalarChannelReaderInterface>(integratorChannel));
    wrapperX->copyMetadata(*wrapper);
    visualizer->getOrCreateWidget();

    std::string name("UNKNOWN");
    wrapperX->getMetadata("core/name", name);    
    auto s = visualizer->createSerie(typeid(ScalarChannelReaderInterface),wrapperX);

    INewChartSerie* chartSerie = dynamic_cast<INewChartSerie*>(s->serie());
    if (events && chartSerie) {
        chartSerie->setEvents(events);
    }
    s->serie()->setName(name);
    series.push_back(s);

}
