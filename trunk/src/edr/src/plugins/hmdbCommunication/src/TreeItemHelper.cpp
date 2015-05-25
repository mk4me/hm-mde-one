#include <plugins/hmdbCommunication/TreeItemHelper.h>
#include <QtWidgets/QHBoxLayout>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/hmmlib/EMGFilter.h>
#include <corelib/Exceptions.h>
#include <boost/lexical_cast.hpp>

using namespace core;

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
	std::vector<Visualizer::VisualizerSerie*> tmpSeries;

	try{
		if (motion->hasObject(typeid(c3dlib::MarkerCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::MarkerCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::MarkerCollection), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw core::runtime_error("Empty object - markers");
			}
		}
		if (motion->hasObject(typeid(SkeletonWithStates), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(SkeletonWithStates), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(SkeletonWithStates), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw core::runtime_error("Empty object - joints");
			}
		}
		if (motion->hasObject(typeid(c3dlib::GRFCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::GRFCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::GRFCollection), m.front());
				s->serie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw core::runtime_error("Empty object - grfs");
			}
		}
	}catch(...){

	}

	series.insert(series.end(), tmpSeries.begin(), tmpSeries.end());
}

VisualizerPtr Multiserie3D::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(SkeletonWithStates), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

std::vector<utils::TypeInfo> Multiserie3D::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
    ret.push_back(typeid(SkeletonWithStates));
	ret.push_back(typeid(c3dlib::GRFCollection));
	ret.push_back(typeid(c3dlib::MarkerCollection));
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
	manager->getVisualizerPrototypes(typeid(SkeletonWithStates), prototypes, true);
	return VisualizerPtr(prototypes.front()->create());
}

void JointsItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::VisualizerSerie*>& series )
{
	ConstVariantsList m;
	motion->getObjects(m, typeid(SkeletonWithStates), false);
    core::VariantConstPtr joints = m.front();
    if (joints && joints->getRawPtr() != nullptr) {
		auto s = visualizer->createSerie(typeid(SkeletonWithStates), joints);
		s->serie()->setName(path.toStdString());
        series.push_back(s);
    } else {
        throw core::runtime_error("Empty object - joints");
    }
}

std::vector<utils::TypeInfo> JointsItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(SkeletonWithStates));
    return ret;
}

JointsItemHelper::JointsItemHelper(const PluginSubject::MotionConstPtr & motion ) :
    motion(motion)
{
}


VisualizerPtr NewChartItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw core::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
		c3dlib::ScalarChannelReaderInterfaceConstPtr scalar = wrapper->get();
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
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
    return ret;
}

NewChartItemHelper::NewChartItemHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events) :
    WrappedItemHelper(wrapper),
    events(events)
{
}

VisualizerPtr NewVector3ItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
	core::IVisualizerManager::VisualizerPrototypes prototypes;
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw core::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
		c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();
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
	c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

    c3dlib::ScalarChannelReaderInterfacePtr x(new c3dlib::VectorToScalarAdaptor(vectorChannel, 0));
    c3dlib::ScalarChannelReaderInterfacePtr y(new c3dlib::VectorToScalarAdaptor(vectorChannel, 1));
    c3dlib::ScalarChannelReaderInterfacePtr z(new c3dlib::VectorToScalarAdaptor(vectorChannel, 2));
    core::VariantPtr wrapperX = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
    core::VariantPtr wrapperY = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
    core::VariantPtr wrapperZ = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
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
	serieX->serie()->setName(xAxisName.isEmpty() == false ? xAxisName.toStdString() : ("X_" + suffix));
	auto serieY = visualizer->createSerie(wrapperY->data()->getTypeInfo(), wrapperY);
	serieY->serie()->setName(yAxisName.isEmpty() == false ? yAxisName.toStdString() : ("Y_" + suffix));
	auto serieZ = visualizer->createSerie(wrapperZ->data()->getTypeInfo(), wrapperZ);
	serieZ->serie()->setName(zAxisName.isEmpty() == false ? zAxisName.toStdString() : ("Z_" + suffix));

    INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->serie());
    INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->serie());
    INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->serie());

    chartSerieX->setColor(QColor(255, 0, 0));
    chartSerieY->setColor(QColor(0, 255, 0));
    chartSerieZ->setColor(QColor(0, 0, 255));

    if (events) {
    	EventSerieBase* eventSerieX = dynamic_cast<EventSerieBase*>(serieX->serie());
    	EventSerieBase* eventSerieY = dynamic_cast<EventSerieBase*>(serieX->serie());
    	EventSerieBase* eventSerieZ = dynamic_cast<EventSerieBase*>(serieX->serie());
    	if (eventSerieX && eventSerieY && eventSerieZ) {
    		eventSerieX->setEvents(events);
    		eventSerieY->setEvents(events);
    		eventSerieZ->setEvents(events);
    	}
    }


    series.push_back(serieX);
    series.push_back(serieY);
    series.push_back(serieZ);
}

NewVector3ItemHelper::NewVector3ItemHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events,
	const QString & xAxisName, const QString & yAxisName, const QString & zAxisName) :
	WrappedItemHelper(wrapper), events(events),
	xAxisName(xAxisName), yAxisName(yAxisName), zAxisName(zAxisName)
{
}

std::vector<utils::TypeInfo> NewVector3ItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
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
	manager->getVisualizerPrototypes(typeid(c3dlib::ScalarChannelReaderInterface), prototypes, true);
	VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
    if (!chart) {
        UTILS_ASSERT(false);
        throw core::runtime_error("Wrong visualizer type!");
    } else {
        chart->setTitle(title);
        //chart->setShowLegend(false);
    }
    return visualizer;
}

std::vector<utils::TypeInfo> NewMultiserieHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(c3dlib::ScalarChannelReaderInterface));
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
		wrappers.push_back(ChartWithDescription(*it, c3dlib::EventsCollectionConstPtr(), PluginSubject::MotionConstPtr()));
    }
}



EMGFilterHelper::EMGFilterHelper(const core::VariantConstPtr& wrapper, const c3dlib::EventsCollectionConstPtr& events) :
    NewChartItemHelper(wrapper, events)
{
}


void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
	c3dlib::ScalarChannelReaderInterfacePtr channel = wrapper->clone()->get();

    utils::shared_ptr<AbsMeanChannel<float, float>> absTest( new AbsMeanChannel<float, float>(channel));
    absTest->initialize();

    //utils::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
	utils::shared_ptr<c3dlib::ScalarModifier> integratorChannel(new c3dlib::ScalarModifier(absTest, RMSModifier()));

	core::VariantPtr wrapperX = core::Variant::create<c3dlib::ScalarChannelReaderInterface>();
	wrapperX->set(utils::dynamic_pointer_cast<c3dlib::ScalarChannelReaderInterface>(integratorChannel));
    wrapperX->copyMetadata(*wrapper);
    visualizer->getOrCreateWidget();

    std::string name("UNKNOWN");
    wrapperX->getMetadata("core/name", name);    
	auto s = visualizer->createSerie(typeid(c3dlib::ScalarChannelReaderInterface), wrapperX);

    EventSerieBase* chartSerie = dynamic_cast<EventSerieBase*>(s->serie());
    if (events && chartSerie) {
        chartSerie->setEvents(events);
    }
    s->serie()->setName(name);
    series.push_back(s);

}
