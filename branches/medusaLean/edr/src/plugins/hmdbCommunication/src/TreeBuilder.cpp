#include <corelib/IDataManagerReader.h>
#include <corelib/DataAccessors.h>
#include <plugins/subject/IMotion.h>
#include <corelib/HierarchyItem.h>
#include <corelib/HierarchyDataItem.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include "TreeBuilder.h"
#include <plugins/hmdbCommunication/TreeItemHelper.h>
//#include <plugins/kinematic/Wrappers.h>
//#include <plugins/video/Wrappers.h>
#include <plugins/subject/ISubject.h>
#include <plugins/hmdbCommunication/IPatient.h>
#include <corelib/PluginCommon.h>
#include <objectwrapperlib/ObjectWrapper.h>
#include <boost/lexical_cast.hpp>
#include <dataaccessorlib/Wrappers.h>

using namespace PluginSubject;
//core::IHierarchyItemPtr TreeBuilder::createTree(const QString& rootItemName, const core::ConstVariantsList& sessions)
//{
//    SubjectHierarchyFilterPtr nullFilter;
//    return TreeBuilder::createTree(rootItemName, sessions, nullFilter);
//}
//
//
//core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject, const SubjectHierarchyFilterPtr & dataFilter )
//{
//    core::ConstVariantsList sessions;
//    subject->getSessions(sessions);
//    return createTree(rootItemName, sessions, dataFilter);
//}
//
//
//core::IHierarchyItemPtr TreeBuilder::createTree( const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject )
//{
//    return TreeBuilder::createTree(rootItemName, subject, SubjectHierarchyFilterPtr());
//}
//
//
//
//core::IHierarchyItemPtr  TreeBuilder::createTree(const QString& rootItemName, const core::ConstVariantsList& sessions, const SubjectHierarchyFilterPtr & dataFilter)
//{
//    core::IHierarchyItemPtr rootItem(new core::HierarchyItem(rootItemName, QString()));
//    core::ConstVariantsList filteredSessions;
//    if(dataFilter != nullptr){
//        dataFilter->filterSessions(sessions, filteredSessions);
//    }else {
//        filteredSessions = sessions;
//    }
//
//    for (auto it = filteredSessions.crbegin(); it != filteredSessions.crend(); ++it) {
//        
//        std::string emgConfigName;
//        (*it)->getMetadata("EMGConf", emgConfigName);
//        
//        core::ConstVariantsList filteredMotions;
//
//        if(dataFilter != nullptr){
//            dataFilter->filterSessionMotions(*it, filteredMotions);
//        }else {
//            PluginSubject::SessionConstPtr s = (*it)->get();
//            s->getMotions(filteredMotions);
//        }
//
//        for(auto motionOW : filteredMotions) {	
//
//            PluginSubject::MotionConstPtr motion = motionOW->get();
//           
//            QString label(QString::fromUtf8(motion->getLocalName().c_str()));
//
//            std::string metaLabel;
//            if(motionOW->getMetadata("label", metaLabel)){
//                label = QString::fromStdString(metaLabel);
//            }
//
//            QString desc = createDescription(motion);
//
//            core::IHierarchyItemPtr motionItem(new core::HierarchyItem(label, desc));
//            rootItem->appendChild(motionItem);
//
//
//        }
//    }
//
//    return rootItem;
//}
//
//template <class Channel, class Collection>
//core::IHierarchyItemPtr TreeBuilder::createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
//{
//    typedef typename utils::ObjectWrapperTraits<Collection>::Ptr CollectionPtr;
//	typedef typename utils::ObjectWrapperTraits<Collection>::ConstPtr CollectionConstPtr;
//    //typedef typename Collection::ChannelType Channel;
//    QString desc = createDescription(motion);
//
//    core::ConstVariantsList collection;
//	motion->getObjects(collection, typeid(Collection), false);
//    core::IHierarchyItemPtr rootItem (new core::HierarchyItem(rootName, desc, rootIcon));
//    CollectionConstPtr m = collection.front()->get();
//    tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
//    return rootItem;
//}
//
//template <class Channel, class CollectionPtr>
//void TreeBuilder::tryAddVectorToTree( const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, core::IHierarchyItemPtr parentItem, bool createContainerItem /*= true */ )
//{
//    QString desc = createDescription(motion);
//    if (collection) {
//        std::vector<core::VariantConstPtr> wrappers;
//        for (int i = 0; i < collection->getNumAccessors(); ++i) {
//            auto wrapper = core::Variant::create<Channel>();
//            wrapper->set(utils::const_pointer_cast<Channel>(utils::dynamic_pointer_cast<const Channel>(collection->getAccessor(i))));
//
//            static int number = 0;
//            std::string();
//            wrapper->setMetadata("name", "Serie_" + boost::lexical_cast<std::string>(number));
//            wrapper->setMetadata("source", "TreeBuilder");
//            wrappers.push_back(wrapper);
//        }
//        core::IHierarchyItemPtr collectionItem;
//        if (createContainerItem) {
//            collectionItem = core::IHierarchyItemPtr(new core::HierarchyItem(QString::fromStdString(name), desc));
//            parentItem->appendChild(collectionItem);
//        } else {
//            collectionItem = parentItem;
//        }
//        int count = wrappers.size();
//
//		c3dlib::EventsCollectionConstPtr events = getEvents(motion);
//        
//        for (int i = 0; i < count; ++i) {
//			c3dlib::VectorChannelReaderInterfaceConstPtr c = wrappers[i]->get();
//            std::string channelName = c->getOrCreateFeature<dataaccessor::DescriptorFeature>()->name();
//            std::list<core::HierarchyHelperPtr> helpers;
//            NewVector3ItemHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i], events));
//            push_not_null(helpers, channelHelper);
//            push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 0));
//            push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 1));
//            push_not_null(helpers, allTFromSession(channelName, motion->getUnpackedSession(), 2));
//            push_not_null(helpers, createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Left));
//            push_not_null(helpers, createNormalized(wrappers[i], motion, c3dlib::C3DParser::IEvent::Right));
//            push_not_null(helpers, createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Left));
//            push_not_null(helpers, createNormalizedFromAll(channelName, motion->getUnpackedSession(), c3dlib::C3DParser::IEvent::Right));
//            core::IHierarchyItemPtr channelItem (new core::HierarchyDataItem(wrappers[i], childIcon, QString::fromStdString(c->getOrCreateFeature<dataaccessor::DescriptorFeature>()->name()), desc, helpers));
//            collectionItem->appendChild(channelItem);
//        }
//    }
//}
//
//
//QString TreeBuilder::createDescription( PluginSubject::MotionConstPtr motion )
//{
//    QString text;
//    PluginSubject::SessionConstPtr session = motion->getUnpackedSession();
//    PluginSubject::SubjectConstPtr subject = session->getUnpackedSubject();
//    
//    text += QObject::tr("Motion: ") + QString::fromStdString(motion->getLocalName()) + "\n";
//    text += QObject::tr("Session: ") + QString::fromStdString(session->getLocalName()) + "\n";
//    text += QObject::tr("Subject: ") + QString::fromStdString(subject->getName()) + "\n";
//       
//    try {
//
//		std::string groupName, groupID;
//
//		if(motion->getSession()->getMetadata("groupName", groupName) == true) {
//			text += QObject::tr("Owner: %1").arg(groupName.c_str()); 
//			if (motion->getSession()->getMetadata("groupID", groupID) == true) {
//				text += QString("(%1)").arg(groupID.c_str());
//			}
//			text += "\n";
//		}
//
//		std::string date;
//		if(motion->getSession()->getMetadata("data", date) == true) {
//			text += QObject::tr("Date: %1\n").arg(date.c_str());
//		}
//    } catch (...) {
//        PLUGIN_LOG_WARNING("Problem with summary window metadata");
//    }
//    
//	if (session->hasObject(typeid(hmdbCommunication::IPatient), false)) {
//		core::ConstVariantsList patients;
//		session->getObjects(patients, typeid(hmdbCommunication::IPatient), false);
//        hmdbCommunication::PatientConstPtr patient = patients.front()->get();
//        text += QObject::tr("Patient: ") + QString::fromStdString(patient->getName()) + " " + QString::fromStdString(patient->getSurname()) + "\n";
//    }
//
//	if (session->hasObject(typeid(hmdbCommunication::AntropometricData), false)) {
//		core::ConstVariantsList antropo;
//		session->getObjects(antropo, typeid(hmdbCommunication::AntropometricData), false);
//        utils::shared_ptr<const hmdbCommunication::AntropometricData> antro = antropo.front()->get();
//        text += " ";
//        text += QObject::tr("Weight: ") + QString("%1 ").arg(antro->bodyMass.first) + QString::fromStdString(antro->bodyMass.second) + "\n";
//    }
//    return text;
//}
//
//core::IHierarchyItemPtr MotionPerspective::getPerspective( PluginSubject::SubjectPtr subject )
//{
//    if (hasValidData(subject)) {
//        return TreeBuilder::createTree("SUB", subject);
//    }
//
//    return core::IHierarchyItemPtr();
//}
//
//bool MotionPerspective::hasValidData(PluginSubject::SubjectPtr subject)
//{
//    std::set<utils::TypeInfo> types;
//    types.insert(typeid(c3dlib::C3DEventsCollection));
//    types.insert(typeid(c3dlib::EMGChannel));
//    types.insert(typeid(c3dlib::GRFCollection));
//    types.insert(typeid(c3dlib::ForceCollection));
//    types.insert(typeid(c3dlib::MomentCollection));
//    types.insert(typeid(c3dlib::PowerCollection));
//    types.insert(typeid(c3dlib::MarkerCollection));
//    types.insert(typeid(SkeletonWithStates));
//    types.insert(typeid(c3dlib::AngleCollection));
//    types.insert(typeid(VideoChannel));
//    types.insert(typeid(c3dlib::EMGCollection));
//    types.insert(typeid(c3dlib::GRFChannel));
//
//    core::ConstVariantsList sessions;
//    subject->getSessions(sessions);
//    for (auto it = sessions.crbegin(); it != sessions.crend(); ++it) {
//        core::ConstVariantsList motions;
//        PluginSubject::SessionConstPtr s = (*it)->get();
//        s->getMotions(motions);
//
//        for(auto motionOW : motions) {	
//
//            PluginSubject::MotionConstPtr motion = motionOW->get();
//            for (auto it = types.begin(); it != types.end(); ++it) {
//                if (motion->hasObject(*it, false)) {
//                    return true;
//                }
//            }
//        }
//    }
//
//    return false;
//}
