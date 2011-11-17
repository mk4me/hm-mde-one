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
    static QTreeWidgetItem* createMarkersBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon );
    static QTreeWidgetItem* createJointsBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon );
    static QTreeWidgetItem* createVideoBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon );
    static QTreeWidgetItem* createGRFBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon );
    static QTreeWidgetItem* createEMGBranch( const MotionConstPtr & motion, const QString& rootName, const QIcon& itemIcon );

    template <class Channel, class CollectionPtr>
    static void tryAddVectorToTree(const MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon* childIcon, QTreeWidgetItem* parentItem, bool createContainerItem = true ) 
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
                QTreeWidgetItem* channelItem = new NewVector3ItemHelper(wrappers[i]);
                if (childIcon) {
                    channelItem->setIcon(0, *childIcon);							
                }				
                channelItem->setText(0, c->getName().c_str());			
                collectionItem->addChild(channelItem);		
            }	
        }
    }
private:
    TreeBuilder();
    ~TreeBuilder();
};

#endif
