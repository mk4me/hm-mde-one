#include "C3DPCH.h"
#include "C3DParser.h"

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <c3dlib/C3DParser.h>
#include <corelib/Filesystem.h>
#include <plugins/c3d/C3DChannels.h>
#include "C3DParser.h"
#include "ForcePlatform.h"
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include <corelib/IVisualizerManager.h>
#include <QtGui/QWidget>
#include <QtGui/QLayout>
#include <plugins/c3d/EventSerieBase.h>
#include <corelib/HierarchyDataItem.h>
#include <kinematiclib/VskParser.h>

core::VisualizerPtr NewVector3ItemHelper::createVisualizer(core::IVisualizerManager* manager)
{
    core::IVisualizerManager::VisualizerPrototypes prototypes;
    manager->getVisualizerPrototypes(typeid(ScalarChannelReaderInterface), prototypes, true);
    core::VisualizerPtr visualizer(prototypes.front()->create());

    QWidget * visWidget = visualizer->getOrCreateWidget();
    visWidget->layout()->setContentsMargins(2, 0, 2, 2);
    /*INewChartVisualizer* chart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
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
    }*/
    return visualizer;
}

void NewVector3ItemHelper::createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series )
{
    VectorChannelReaderInterfaceConstPtr vectorChannel = wrapper->get();

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

    (*wrapperX)["core/name"] = "X_" + suffix;
    (*wrapperY)["core/name"] = "Y_" + suffix;
    (*wrapperZ)["core/name"] = "Z_" + suffix;
    (*wrapperX)["core/source"] = p + "/X_" + suffix;
    (*wrapperY)["core/source"] = p + "/Y_" + suffix;
    (*wrapperZ)["core/source"] = p + "/Z_" + suffix;
    visualizer->getOrCreateWidget();

    auto serieX = visualizer->createSerie(wrapperX->getTypeInfo(), wrapperX);
    serieX->serie()->setName("X_" + suffix);
    auto serieY = visualizer->createSerie(wrapperY->getTypeInfo(), wrapperY);
    serieY->serie()->setName("Y_" + suffix);
    auto serieZ = visualizer->createSerie(wrapperZ->getTypeInfo(), wrapperZ);
    serieZ->serie()->setName("Z_" + suffix);

    EventSerieBase* chartSerieX = dynamic_cast<EventSerieBase*>(serieX->serie());
    EventSerieBase* chartSerieY = dynamic_cast<EventSerieBase*>(serieY->serie());
    EventSerieBase* chartSerieZ = dynamic_cast<EventSerieBase*>(serieZ->serie());

    if (events) {
        chartSerieX->setEvents(events);
        chartSerieY->setEvents(events);
        chartSerieZ->setEvents(events);
    }

    //chartSerieX->setColor(QColor(255, 0, 0));
    //chartSerieY->setColor(QColor(0, 255, 0));
    //chartSerieZ->setColor(QColor(0, 0, 255));

    series.push_back(serieX);
    series.push_back(serieY);
    series.push_back(serieZ);
}

NewVector3ItemHelper::NewVector3ItemHelper(const core::ObjectWrapperConstPtr& wrapper, const EventsCollectionConstPtr& events ) :
    WrappedItemHelper(wrapper),
    events(events)
{
}

std::vector<core::TypeInfo> NewVector3ItemHelper::getTypeInfos() const
{
    std::vector<core::TypeInfo> ret;
    ret.push_back(typeid(ScalarChannelReaderInterface));
    return ret;
}


C3DParser::C3DParser()
{
    // sztywne stworzenie obiektów, zachowanie kompatybilności
	for(int i = 0; i < 4; ++i){
		GRFChannels.push_back(core::ObjectWrapper::create<GRFChannel>());
	}
	for(int i = 0; i < 16; ++i){
		EMGChannels.push_back(core::ObjectWrapper::create<EMGChannel>());
	}

    // GRF i EMG są dosepne tez poprzez standardowe kolekcje
	GRFs = core::ObjectWrapper::create<GRFCollection>();
	EMGs = core::ObjectWrapper::create<EMGCollection>();
    // reszta kolekcji już bez udziwnien
	markerChannels = core::ObjectWrapper::create<MarkerCollection>();
	forceChannels  = core::ObjectWrapper::create<ForceCollection>();
	angleChannels  = core::ObjectWrapper::create<AngleCollection>();
	momentChannels = core::ObjectWrapper::create<MomentCollection>();
	powerChannels  = core::ObjectWrapper::create<PowerCollection>();
	allEvents = core::ObjectWrapper::create<C3DEventsCollection>();
    movieDelays = core::ObjectWrapper::create<MovieDelays>();

    hierarchy = utils::make_shared<core::HierarchyItem>("TEST", "TEST");
}

C3DParser::~C3DParser()
{
}

void C3DParser::parse( const std::string & source  )
{
	core::Filesystem::Path path(source);

	parserPtr = utils::make_shared<c3dlib::C3DParser>();

    std::vector<std::string> files;
    files.push_back(path.string());
	std::string importWarnings;
    parserPtr->importFrom(files, importWarnings);

	MovieDelaysPtr delays = utils::make_shared<MovieDelays>(parserPtr->getMovieDelays());
    movieDelays->set(delays);
    // wczytanie danych analogowych
    GRFCollectionPtr grfs(new GRFCollection());
    EMGCollectionPtr e(new EMGCollection());
    if (parserPtr->getNumAnalogs() == 28)
    {
        for (int i = 0; i < 4; ++i) {
            GRFChannelPtr ptr(new GRFChannel(*parserPtr , i));
            GRFChannels[i]->set(ptr);
			(*(GRFChannels[i]))["core/name"]  = ptr->getName();
            (*(GRFChannels[i]))["core/source"] = path.string();
            grfs->addChannel(ptr);
        }

		GRFs->set(grfs);
        (*GRFs)["core/source"] = path.string();

        for (int i = 12; i < 28; ++i) {
            EMGChannelPtr ptr(new EMGChannel(*parserPtr , i));
            EMGChannels[i-12]->set(ptr);
			(*(EMGChannels[i-12]))["core/name"]  = ptr->getName();
			(*(EMGChannels[i-12]))["core/source"]  = path.string();
            e->addChannel(ptr);
        }
        
		EMGs->set(e);
        (*EMGs)["core/name"] = path.string();
    }

    // wczytanie eventów
	int count = parserPtr->getNumEvents();
    EventsCollectionPtr allEventsCollection(new C3DEventsCollection());
	for (int i = 0; i < count; ++i) {
		c3dlib::C3DParser::IEventPtr event = parserPtr->getEvent(i);
        C3DEventsCollection::EventPtr e(event->clone());
        allEventsCollection->addEvent(e);
	}
    
	allEvents->set(allEventsCollection);
	(*allEvents)["core/source"] = path.string();
    // wczytanie plików *vsk, które dostarczaja opis do markerów
    core::Filesystem::Path dir = path.parent_path();
    std::vector<std::string> vskFiles = core::Filesystem::listFiles(dir, false, ".vsk");
    kinematic::VskParserPtr vsk;
    if (vskFiles.size() == 1) {
        vsk = kinematic::VskParserPtr(new kinematic::VskParser());
        vsk->parse(*vskFiles.begin());
    }
	MarkerCollectionPtr markers(new MarkerCollection(vsk));
	ForceCollectionPtr forces(new ForceCollection);
	AngleCollectionPtr angles(new AngleCollection);
	MomentCollectionPtr moments(new MomentCollection);
	PowerCollectionPtr powers(new PowerCollection);

	powerChannels->set(powers);
	(*powerChannels)["core/source"] = path.string();

	markerChannels->set(markers);
	(*markerChannels)["core/source"] = path.string();
	forceChannels->set(forces);
	(*forceChannels)["core/source"] = path.string();
	angleChannels->set(angles);
	(*angleChannels)["core/source"] = path.string();
	momentChannels->set(moments);
	(*momentChannels)["core/source"] = path.string();

	int markersCount = parserPtr->getNumPoints();
	for (int i = 0; i < markersCount; ++i) {
		switch (parserPtr->getPoint(i)->getType()) {
			case c3dlib::C3DParser::IPoint::Angle: {
                AngleChannelPtr ptr(new AngleChannel(*parserPtr, i));
                angles->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Marker: {
                MarkerChannelPtr ptr(new MarkerChannel(*parserPtr, i));
                markers->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Force: {
                ForceChannelPtr ptr(new ForceChannel(*parserPtr, i));
                forces->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Moment: {
                MomentChannelPtr ptr(new MomentChannel(*parserPtr, i));
                moments->addChannel(ptr);
            } break;
            case c3dlib::C3DParser::IPoint::Power: {
                PowerChannelPtr ptr(new PowerChannel(*parserPtr, i));
                powers->addChannel(ptr);
            } break;
		}
	}

    try {
        IForcePlatformCollection platforms;
        auto parsedPlatforms = parserPtr->getForcePlatforms();
        int count = static_cast<int>(parsedPlatforms.size());
        for (int i = 0; i < count;  ++i) {
            ForcePlatformPtr fp(new ForcePlatform(parsedPlatforms[i]));
            // HACK ! POWINIEN BYC KANAL NA PODST. NR PLATFORMY!
            fp->setForceChannel(grfs->getGRFChannel(i == 0 ? GRFChannel::F1 : GRFChannel::F2));
            fp->setMomentChannel(grfs->getGRFChannel(i == 0 ? GRFChannel::M1 : GRFChannel::M2));
            fp->computeSteps(markers, allEventsCollection);
            platforms.push_back(fp);
        }
		grfs->setPlatforms(platforms);
	} catch(...) {}

    createTree();
}

plugin::IParser* C3DParser::create() const
{
    return new C3DParser();
}

void C3DParser::acceptedExpressions(Expressions & expressions) const
{
    ExpressionDescription expDesc;
    expDesc.description = "C3D format";

    expDesc.types.insert(typeid(GRFChannel));
    expDesc.types.insert(typeid(EMGChannel));
    expDesc.types.insert(typeid(GRFCollection));
    expDesc.types.insert(typeid(EMGCollection));
    expDesc.types.insert(typeid(MarkerCollection));
    expDesc.types.insert(typeid(ForceCollection));
    expDesc.types.insert(typeid(AngleCollection));
    expDesc.types.insert(typeid(MomentCollection));
    expDesc.types.insert(typeid(PowerCollection));
    expDesc.types.insert(typeid(C3DEventsCollection));
    expDesc.types.insert(typeid(MovieDelays));

    expressions.insert(Expressions::value_type(".*\.c3d$", expDesc));
}

plugin::IParser::ParsedObjectsPtr C3DParser::getObjects()
{
    plugin::IParser::ParsedObjectsPtr  objs = utils::make_shared<IParser::ParsedObjects>();
    objs->additionalObjects.insert(GRFChannels.begin(), GRFChannels.end());
    objs->additionalObjects.insert(EMGChannels.begin(), EMGChannels.end());
    objs->additionalObjects.insert(markerChannels);    
    objs->additionalObjects.insert(allEvents);
    objs->additionalObjects.insert(EMGs);
    objs->additionalObjects.insert(GRFs);
    objs->additionalObjects.insert(forceChannels );
    objs->additionalObjects.insert(angleChannels );
    objs->additionalObjects.insert(momentChannels);
    objs->additionalObjects.insert(powerChannels );
    objs->additionalObjects.insert(movieDelays   );
	//objects.insert(c3dMisc);

    objs->hierarchy = hierarchy;
    return objs;
}

void C3DParser::saveFile( const core::Filesystem::Path& path )
{
	if (parserPtr) {
		parserPtr->save(path.string());
	}
}

template < class CollectionPtr>
void C3DParser::tryAddVectorToTree( const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */ )
{
    typedef CollectionPtr::element_type Collection;
    typedef Collection::ChannelType Channel;
    if (collection) {
        std::vector<core::ObjectWrapperConstPtr> wrappers;
        for (int i = 0; i < collection->getNumChannels(); ++i) {
            core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();
            wrapper->set(core::const_pointer_cast<Channel>(boost::dynamic_pointer_cast<const Channel>(collection->getChannel(i))));

            static int number = 0;
            std::string();
            (*wrapper)["name"] = "Serie_" + boost::lexical_cast<std::string>(number);
            (*wrapper)["source"] = "C3DParser";
            wrappers.push_back(wrapper);
        }
        core::IHierarchyItemPtr collectionItem;
        if (createContainerItem) {
            collectionItem = core::IHierarchyItemPtr(new core::HierarchyItem(QString::fromStdString(name), QString("DESC")));
            parentItem->appendChild(collectionItem);
        } else {
            collectionItem = parentItem;
        }
        int count = wrappers.size();

        EventsCollectionConstPtr events = this->allEvents->get();

        for (int i = 0; i < count; ++i) {
            VectorChannelConstPtr c = wrappers[i]->get();
            std::string channelName = c->getName();
            std::list<core::HierarchyHelperPtr> helpers;
            NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i], events));
            helpers.push_back(channelHelper);
            core::IHierarchyItemPtr channelItem (new core::HierarchyDataItem(wrappers[i], childIcon, QString::fromStdString(c->getName()), "desc", helpers));
            collectionItem->appendChild(channelItem);
        }
    }
}

void C3DParser::createTree()
{
    core::IHierarchyItemPtr analog = utils::make_shared<core::HierarchyItem>(QString("Analog"), QString(""), QIcon());
    addCollection<GRFCollectionConstPtr>(analog, GRFs->get(), "GRF", "GRF");
    //addCollection<EMGCollectionConstPtr>(analog, EMGs->get(), "EMG", "GRF");

    core::IHierarchyItemPtr kinetic = utils::make_shared<core::HierarchyItem>(QString("Kinetic"), QString(""), QIcon());
    addCollection<ForceCollectionConstPtr>(kinetic, forceChannels->get(), "Forces", "Forces");
    addCollection<MomentCollectionConstPtr>(kinetic, momentChannels->get(), "Moments", "Moments");
    addCollection<PowerCollectionConstPtr>(kinetic, powerChannels->get(), "Powers", "Powers");

    core::IHierarchyItemPtr kinematic = utils::make_shared<core::HierarchyItem>(QString("Kinematic"), QString(""), QIcon());
    addCollection<AngleCollectionConstPtr>(kinematic, angleChannels->get(), "Angles", "Angles");
    addCollection<MarkerCollectionConstPtr>(kinematic, markerChannels->get(), "Markers", "Markers");

    hierarchy->appendChild(analog);
    hierarchy->appendChild(kinetic);
    hierarchy->appendChild(kinematic);
    /*core::IHierarchyItemPtr momentsItem = utils::make_shared<core::HierarchyItem>("Moments", "Moments");
    tryAddVectorToTree<ForceCollectionConstPtr>(forceChannels->get(), "Collection", QIcon(), momentsItem, false);
    hierarchy->appendChild(momentsItem);*/
}

template <class CollectionPtr>
void C3DParser::addCollection(core::IHierarchyItemPtr parent, const CollectionPtr& collection, const QString& name, const QString& desc)
{
    core::IHierarchyItemPtr item = utils::make_shared<core::HierarchyItem>(name, desc);
    tryAddVectorToTree(collection, "Collection", QIcon(), item, false);
    parent->appendChild(item);
}

