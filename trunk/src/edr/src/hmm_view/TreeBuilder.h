/********************************************************************
	created:	2011/10/12
	created:	12:10:2011   12:28
	filename: 	TreeBuilder.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__TREEBUILDER_H__
#define HEADER_GUARD_HMM__TREEBUILDER_H__

#include <plugins/subject/Types.h>

class TreeBuilder
{
public:
    static QTreeWidgetItem* createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions);
    static QTreeWidgetItem* createTree(const QString& rootItemName, const std::vector<SessionConstPtr>& sessions, const DataFilterPtr & dataFilter);
    static QTreeWidgetItem* createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createForcesBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createMomentsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    static QTreeWidgetItem* createPowersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );

    static const QIcon& getMarkersIcon( )      { static QIcon icon(core::getResourceString("icons/markerSmall.png")); return icon; }
    static const QIcon& getJointsIcon( )       { static QIcon icon(core::getResourceString("icons/jointSmall.png"));  return icon; }
    static const QIcon& getVideoIcon( )        { static QIcon icon(core::getResourceString("icons/videoSmall.png"));  return icon; }
    static const QIcon& getGRFIcon( )          { static QIcon icon(core::getResourceString("icons/grfSmall.png"));    return icon; }
    static const QIcon& getEMGIcon( )          { static QIcon icon(core::getResourceString("icons/emg1Small.png"));   return icon; }
    static const QIcon& getForcesIcon( )       { static QIcon icon(core::getResourceString("icons/forcesSmall.png")); return icon; }
    static const QIcon& getMomentsIcon( )      { static QIcon icon(core::getResourceString("icons/momentSmall.png")); return icon; }
    static const QIcon& getPowersIcon( )       { static QIcon icon(core::getResourceString("icons/powerSmall.png"));  return icon; }

    static const QIcon& getRootMarkersIcon( )  { static QIcon icon(core::getResourceString("icons/markerSmall.png")); return icon; }
    static const QIcon& getRootJointsIcon( )   { static QIcon icon(core::getResourceString("icons/jointSmall.png"));  return icon; }
    static const QIcon& getRootVideoIcon( )    { static QIcon icon(core::getResourceString("icons/videoSmall.png"));  return icon; }
    static const QIcon& getRootGRFIcon( )      { static QIcon icon(core::getResourceString("icons/grfSmall.png"));    return icon; }
    static const QIcon& getRootEMGIcon( )      { static QIcon icon(core::getResourceString("icons/emg1Small.png"));   return icon; }
    static const QIcon& getRootForcesIcon( )   { static QIcon icon(core::getResourceString("icons/forcesSmall.png")); return icon; }
    static const QIcon& getRootMomentsIcon( )  { static QIcon icon(core::getResourceString("icons/momentSmall.png")); return icon; }
    static const QIcon& getRootPowersIcon( )   { static QIcon icon(core::getResourceString("icons/powerSmall.png"));  return icon; }

    template <class Collection>
    static QTreeWidgetItem* createTBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
    {
        typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
        typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;
        typedef typename Collection::ChannelType Channel;

        core::ObjectWrapperConstPtr collection = motion->getWrapperOfType(typeid(Collection));
        QTreeWidgetItem* rootItem = new QTreeWidgetItem();
        rootItem->setIcon(0, rootIcon);
        rootItem->setText(0, rootName);		

        CollectionConstPtr m = collection->get(); 
        tryAddVectorToTree<Channel>(motion, m, "Collection", itemIcon, rootItem, false);
        return rootItem;
    }

    template <class Channel, class CollectionPtr>
    static void tryAddVectorToTree(const MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, QTreeWidgetItem* parentItem, bool createContainerItem = true ) 
    {
        if (collection) {
            std::vector<core::ObjectWrapperConstPtr> wrappers;
            for (int i = 0; i < collection->getNumChannels(); i++) {
                core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<VectorChannel>();
                wrapper->set(core::const_pointer_cast<VectorChannel>(boost::dynamic_pointer_cast<const VectorChannel>(collection->getChannel(i))));

                static int number = 0;
                std::string name = "Serie_" + boost::lexical_cast<std::string>(number);
                wrapper->setName(name);
                wrapper->setSource(name);
                wrappers.push_back(wrapper);
            }
            QTreeWidgetItem* collectionItem;
            if (createContainerItem) {
                collectionItem = new QTreeWidgetItem();
                collectionItem->setText(0, name.c_str());	
                parentItem->addChild(collectionItem);
            } else {
                collectionItem = parentItem;
            }
            int count = wrappers.size();
            for (int i = 0; i < count; i++) {	
                VectorChannelConstPtr c = wrappers[i]->get();
                TreeItemHelper* channelItem = new NewVector3ItemHelper(wrappers[i]);
                channelItem->setIcon(0, childIcon);	
                channelItem->setText(0, c->getName().c_str());			
                channelItem->setMotion(motion);
                collectionItem->addChild(channelItem);		
            }	
        }
    }
private:
    TreeBuilder();
    ~TreeBuilder();
};

#endif
