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
#include "TreeItemHelper.h"

//! Klasa s³u¿y do budowania drzewa danych w analizach
class TreeBuilder
{
public:
    //! Tworzy drzewo na podstawie dostarczonych sesji
    //! \param rootItemName nazwa korzenia
    //! \param sessions dostarczone sesje
    //! \return stworzony element
    static QTreeWidgetItem* createTree(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions);
    //! Tworzy drzewo na podstawie dostarczony sesji
    //! \param rootItemName nazwa korzenia
    //! \param sessions dostarczone sesje
    //! \param dataFilter obiekt filtruj¹cy sesje
    //! \return stworzony element
    static QTreeWidgetItem* createTree(const QString& rootItemName, const std::vector<PluginSubject::SessionConstPtr>& sessions, const PluginSubject::DataFilterPtr & dataFilter);
    //! Tworzy ga³¹Ÿ dla markerów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createMarkersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla stawów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createJointsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla nagrañ video
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createVideoBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla danych grf
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createGRFBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla danych emg
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createEMGBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla 
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createForcesBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla momentów
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createMomentsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! Tworzy ga³¹Ÿ dla mocy
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    static QTreeWidgetItem* createPowersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );

    //! \return ikona dla ga³êzi z markerami
    static const QIcon& getMarkersIcon( )      { static QIcon icon(QString::fromUtf8(":/resources/icons/markerSmall.png")); return icon; }
    //! \return ikona dla ga³êzi ze stawami
    static const QIcon& getJointsIcon( )       { static QIcon icon(QString::fromUtf8(":/resources/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla ga³êzi z nagraniami
    static const QIcon& getVideoIcon( )        { static QIcon icon(QString::fromUtf8(":/resources/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla ga³êzi z danymi grf
    static const QIcon& getGRFIcon( )          { static QIcon icon(QString::fromUtf8(":/resources/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla ga³êzi z danymi emg
    static const QIcon& getEMGIcon( )          { static QIcon icon(QString::fromUtf8(":/resources/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla ga³êzi z si³ami
    static const QIcon& getForcesIcon( )       { static QIcon icon(QString::fromUtf8(":/resources/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla ga³êzi z momentami
    static const QIcon& getMomentsIcon( )      { static QIcon icon(QString::fromUtf8(":/resources/icons/momentSmall.png")); return icon; }
    //! \return ikona dla ga³êzi z mocami
    static const QIcon& getPowersIcon( )       { static QIcon icon(QString::fromUtf8(":/resources/icons/powerSmall.png"));  return icon; }

    //! \return ikona dla g³ównej ga³êzi z markerami
    static const QIcon& getRootMarkersIcon( )  { static QIcon icon(QString::fromUtf8(":/resources/icons/markerSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi ze stawami
    static const QIcon& getRootJointsIcon( )   { static QIcon icon(QString::fromUtf8(":/resources/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla g³ównej ga³êzi z nagraniami
    static const QIcon& getRootVideoIcon( )    { static QIcon icon(QString::fromUtf8(":/resources/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla g³ównej ga³êzi z danymi grf
    static const QIcon& getRootGRFIcon( )      { static QIcon icon(QString::fromUtf8(":/resources/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla g³ównej ga³êzi z danymi emg
    static const QIcon& getRootEMGIcon( )      { static QIcon icon(QString::fromUtf8(":/resources/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla g³ównej ga³êzi z si³ami
    static const QIcon& getRootForcesIcon( )   { static QIcon icon(QString::fromUtf8(":/resources/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi z momentami
    static const QIcon& getRootMomentsIcon( )  { static QIcon icon(QString::fromUtf8(":/resources/icons/momentSmall.png")); return icon; }
    //! \return ikona dla g³ównej ga³êzi z mocami
    static const QIcon& getRootPowersIcon( )   { static QIcon icon(QString::fromUtf8(":/resources/icons/powerSmall.png"));  return icon; }

    //! tworzy element drzewa z dowolnej kolekcji kana³ów danych
    //! \param motion próba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych elementów
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla liœci
    template <class Collection>
    static QTreeWidgetItem* createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon )
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
    //! dodanie do drzewa elementów z podpiêtymi danymi w postaci wektorów
    //! \param motion próba pomiarowa
    //! \param collection kolekcja zawieraj¹ca wektory
    //! \param name nazwa tworzonego elementu
    //! \param childIcon ikona dla liœci
    //! \param parentItem rodzic dla tworzonych elementów
    //! \param createContainerItem czy stworzyæ element pomiêdzy rodzicem a liœæmi
    template <class Channel, class CollectionPtr>
    static void tryAddVectorToTree(const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon, QTreeWidgetItem* parentItem, bool createContainerItem = true )
    {
        if (collection) {
            std::vector<core::ObjectWrapperConstPtr> wrappers;
            for (int i = 0; i < collection->getNumChannels(); ++i) {
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
            for (int i = 0; i < count; ++i) {
                VectorChannelConstPtr c = wrappers[i]->get();
                TreeItemHelperPtr channelHelper(new NewVector3ItemHelper(wrappers[i]));
                channelHelper->setMotion(motion);

                HmmTreeItem* channelItem = new HmmTreeItem(channelHelper);
                channelItem->setIcon(0, childIcon);
                channelItem->setItemAndHelperText(c->getName().c_str());
                collectionItem->addChild(channelItem);
            }
        }
    }

private:
    //! prywatny konstruktor, klasa statyczna
    TreeBuilder();
    //!
    ~TreeBuilder();
};

#endif
