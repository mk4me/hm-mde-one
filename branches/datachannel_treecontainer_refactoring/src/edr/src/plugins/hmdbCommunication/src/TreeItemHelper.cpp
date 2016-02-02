#include <plugins/hmdbCommunication/TreeItemHelper.h>
#include <QtWidgets/QHBoxLayout>
#include <corelib/IVisualizerManager.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/c3d/EventSerieBase.h>
#include <utils/Debug.h>
#include <plugins/kinematic/Wrappers.h>
#include <plugins/hmmlib/EMGFilter.h>
#include <loglib/Exceptions.h>
#include <boost/lexical_cast.hpp>
#include <datachannellib/Wrappers.h>

using namespace core;

void Multiserie3D::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
	std::vector<Visualizer::Serie*> tmpSeries;

	try{
		if (motion->hasObject(typeid(c3dlib::MarkerCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::MarkerCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::MarkerCollection), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - markers");
			}
		}
		if (motion->hasObject(typeid(SkeletonWithStates), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(SkeletonWithStates), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(SkeletonWithStates), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - joints");
			}
		}
		if (motion->hasObject(typeid(c3dlib::GRFCollection), false)) {
			ConstVariantsList m;
			motion->getObjects(m, typeid(c3dlib::GRFCollection), false);
			if(m.front()->getRawPtr() != nullptr){
				auto s = visualizer->createSerie(typeid(c3dlib::GRFCollection), m.front());
				s->innerSerie()->setName(path.toStdString());
				tmpSeries.push_back(s);
			}else{
				throw loglib::runtime_error("Empty object - grfs");
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

void JointsItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
	if (skeletonWithStates && skeletonWithStates->getRawPtr() != nullptr) {
		auto s = visualizer->createSerie(typeid(SkeletonWithStates), skeletonWithStates);
		s->innerSerie()->setName(path.toStdString());
        series.push_back(s);
    } else {
        throw loglib::runtime_error("Empty object - joints");
    }
}

std::vector<utils::TypeInfo> JointsItemHelper::getTypeInfos() const
{
    std::vector<utils::TypeInfo> ret;
	ret.push_back(typeid(SkeletonWithStates));
    return ret;
}

JointsItemHelper::JointsItemHelper(core::VariantConstPtr skeletonWithStates) :
skeletonWithStates(skeletonWithStates)
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
        throw loglib::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
		c3dlib::ScalarChannelReaderInterfaceConstPtr scalar = wrapper->get();

		auto df = scalar->feature<dataaccessor::IDescriptorFeature>();

		if (df != nullptr){

			title += df->name();
			title += " [";
			title += df->valueUnit();
			title += "]";

		}
		else{

		}

		chart->setTitle(QString(title.c_str()));

    }

    return visualizer;
}


void NewChartItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
    auto serie = visualizer->createSerie(wrapper);
	serie->innerSerie()->setName(path.toStdString());
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
        throw loglib::runtime_error("Wrong visualizer type!");
    } else {
        std::string title;
		c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

		auto df = vectorChannel->feature<dataaccessor::IDescriptorFeature>();

		if (df != nullptr){

			title += df->name();
			title += " [";
			title += df->valueUnit();
			title += "]";

		}
		else{

		}
        
        chart->setTitle(QString(title.c_str()));
    }
    return visualizer;
}

void NewVector3ItemHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
	c3dlib::VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

	using T = c3dlib::VectorChannelReaderInterfaceConstPtr;

	auto t = utils::supports_pointer_operator<T>::value;

	//chyba ok

	//ok
	auto r0 = std::is_class<T>::value;

	auto r1 = dataaccessor::has_data_description<typename utils::pointed_type<T>::type>::value;

	auto r2 = dataaccessor::is_valid_discrete_accessor<typename utils::pointed_type<T>::type>::value;

	auto r23 = std::is_convertible<T, dataaccessor::ExtractedDiscreteConstPtr<typename utils::pointed_type<T>::type>>::value;

	auto r3 = dataaccessor::is_valid_discrete_accessor_ptr<T>::value;

	//decltype(typename std::enable_if<dataaccessor::is_valid_discrete_accessor_ptr<c3dlib::VectorChannelReaderInterfaceConstPtr>::value>::type) ret;

	c3dlib::ScalarChannelReaderInterfacePtr x(dataaccessor::StaticVector<0>::wrap(vectorChannel));
	c3dlib::ScalarChannelReaderInterfacePtr y(dataaccessor::StaticVector<1>::wrap(vectorChannel));
	c3dlib::ScalarChannelReaderInterfacePtr z(dataaccessor::StaticVector<2>::wrap(vectorChannel));

    core::VariantPtr wrapperX = core::Variant::wrap(x);
    core::VariantPtr wrapperY = core::Variant::wrap(y);
    core::VariantPtr wrapperZ = core::Variant::wrap(z);    

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

	auto serieX = visualizer->createSerie(wrapperX);
	serieX->innerSerie()->setName(xAxisName.isEmpty() == false ? xAxisName.toStdString() : ("X_" + suffix));
	auto serieY = visualizer->createSerie(wrapperY);
	serieY->innerSerie()->setName(yAxisName.isEmpty() == false ? yAxisName.toStdString() : ("Y_" + suffix));
	auto serieZ = visualizer->createSerie(wrapperZ);
	serieZ->innerSerie()->setName(zAxisName.isEmpty() == false ? zAxisName.toStdString() : ("Z_" + suffix));

	INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());
	INewChartSerie* chartSerieY = dynamic_cast<INewChartSerie*>(serieY->innerSerie());
	INewChartSerie* chartSerieZ = dynamic_cast<INewChartSerie*>(serieZ->innerSerie());

    chartSerieX->setColor(QColor(255, 0, 0));
    chartSerieY->setColor(QColor(0, 255, 0));
    chartSerieZ->setColor(QColor(0, 0, 255));

    if (events) {
		EventSerieBase* eventSerieX = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
		EventSerieBase* eventSerieY = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
		EventSerieBase* eventSerieZ = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
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

void NewMultiserieHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<Visualizer::Serie*>& series )
{
    int count = wrappers.size();
    for (int i = 0; i < count; ++i) {
        auto wrapper = wrappers[i].wrapper;
		std::string source;
		wrapper->getMetadata("core/source", source);
		auto serieX = visualizer->createSerie(wrapper);
		serieX->innerSerie()->setName(source);
        if (wrappers[i].events) {
			EventSerieBase * eventSerie = dynamic_cast<EventSerieBase*>(serieX->innerSerie());
            eventSerie->setEvents(wrappers[i].events);
        }
		INewChartSerie* chartSerieX = dynamic_cast<INewChartSerie*>(serieX->innerSerie());

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
        throw loglib::runtime_error("Wrong visualizer type!");
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


void EMGFilterHelper::createSeries( const VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::Serie*>& series )
{
	c3dlib::ScalarChannelReaderInterfaceConstPtr channel = wrapper->get();

    auto absTest = AbsMeanValueChannel::wrap(channel);

    //utils::shared_ptr<ScalarModifier> integratorChannel(new ScalarModifier(absTest, ScalarChannelIntegrator()));
	//utils::shared_ptr<c3dlib::ScalarModifier> integratorChannel(new c3dlib::ScalarModifier(absTest, RMSModifier()));

	auto wrapperX = core::Variant::wrap<c3dlib::ScalarChannelReaderInterface>(RMSChannel::create(*absTest));
	//wrapperX->set(utils::dynamic_pointer_cast<c3dlib::ScalarChannelReaderInterface>(integratorChannel));
    wrapperX->copyMetadata(*wrapper);
    visualizer->getOrCreateWidget();

    std::string name("UNKNOWN");
    wrapperX->getMetadata("core/name", name);    
	auto s = visualizer->createSerie(typeid(c3dlib::ScalarChannelReaderInterface), wrapperX);

	EventSerieBase* chartSerie = dynamic_cast<EventSerieBase*>(s->innerSerie());
    if (events && chartSerie) {
        chartSerie->setEvents(events);
    }
	s->innerSerie()->setName(name);
    series.push_back(s);

}
