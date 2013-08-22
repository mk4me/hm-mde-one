/********************************************************************
	created:	2011/10/12
	created:	12:10:2011   12:28
	filename: 	TreeBuilder.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEWCOMMUNICATION__TREEBUILDER_H__
#define HEADER_GUARD_NEWCOMMUNICATION__TREEBUILDER_H__

#include <plugins/subject/Types.h>
#include <plugins/subject/SubjectDataFilters.h>
#include <corelib/IHierarchyItem.h>
#include <QtGui/QIcon>
#include <corelib/HierarchyHelper.h>

class TreeBuilder
{
public:  
    //! Tworzy drzewo na podstawie dostarczony sesji
    //! \param rootItemName nazwa korzenia
    //! \param sessions dostarczone sesje
    //! \param dataFilter obiekt filtruj�cy sesje
    //! \return stworzony element
    static core::IHierarchyItemPtr  createTree(const QString& rootItemName, const core::ConstObjectsList& sessions, const SubjectHierarchyFilterPtr & dataFilter);

    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const core::ConstObjectsList& sessions);

    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject);
    static core::IHierarchyItemPtr createTree(const QString& rootItemName, const PluginSubject::SubjectConstPtr& subject, const SubjectHierarchyFilterPtr & dataFilter);
    //! Tworzy ga��� dla marker�w
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createMarkersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla staw�w
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createJointsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla nagra� video
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createVideoBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla danych grf
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createGRFBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla danych emg
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createEMGBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIconconst, const std::string & conf );
    //! Tworzy ga��� dla 
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createForcesBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla moment�w
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createMomentsBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );
    //! Tworzy ga��� dla mocy
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    static core::IHierarchyItemPtr createPowersBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon, const std::string & );

    //! \return ikona dla ga��zi z markerami
    static const QIcon& getMarkersIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
    //! \return ikona dla ga��zi ze stawami
    static const QIcon& getJointsIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla ga��zi z nagraniami
    static const QIcon& getVideoIcon( )        { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla ga��zi z danymi grf
    static const QIcon& getGRFIcon( )          { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla ga��zi z danymi emg
    static const QIcon& getEMGIcon( )          { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla ga��zi z si�ami
    static const QIcon& getForcesIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla ga��zi z momentami
    static const QIcon& getMomentsIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
    //! \return ikona dla ga��zi z mocami
    static const QIcon& getPowersIcon( )       { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }

    //! \return ikona dla g��wnej ga��zi z markerami
    static const QIcon& getRootMarkersIcon( )  { static QIcon icon(QString::fromUtf8(":/mde/icons/markerSmall.png")); return icon; }
    //! \return ikona dla g��wnej ga��zi ze stawami
    static const QIcon& getRootJointsIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/jointSmall.png"));  return icon; }
    //! \return ikona dla g��wnej ga��zi z nagraniami
    static const QIcon& getRootVideoIcon( )    { static QIcon icon(QString::fromUtf8(":/mde/icons/videoSmall.png"));  return icon; }
    //! \return ikona dla g��wnej ga��zi z danymi grf
    static const QIcon& getRootGRFIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/grfSmall.png"));    return icon; }
    //! \return ikona dla g��wnej ga��zi z danymi emg
    static const QIcon& getRootEMGIcon( )      { static QIcon icon(QString::fromUtf8(":/mde/icons/emg1Small.png"));   return icon; }
    //! \return ikona dla g��wnej ga��zi z si�ami
    static const QIcon& getRootForcesIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/forcesSmall.png")); return icon; }
    //! \return ikona dla g��wnej ga��zi z momentami
    static const QIcon& getRootMomentsIcon( )  { static QIcon icon(QString::fromUtf8(":/mde/icons/momentSmall.png")); return icon; }
    //! \return ikona dla g��wnej ga��zi z mocami
    static const QIcon& getRootPowersIcon( )   { static QIcon icon(QString::fromUtf8(":/mde/icons/powerSmall.png"));  return icon; }


private:
    //! tworzy element drzewa z dowolnej kolekcji kana��w danych
    //! \param motion pr�ba pomiarowa
    //! \param rootName nazwa dla rodzica tworzonych element�w
    //! \param rootIcon ikona rodzica
    //! \param itemIcon ikona dla li�ci
    template <class Channel, class Collection>
    static core::IHierarchyItemPtr createTBranch( const PluginSubject::MotionConstPtr & motion, const QString& rootName, const QIcon& rootIcon, const QIcon& itemIcon );
    //! dodanie do drzewa element�w z podpi�tymi danymi w postaci wektor�w
    //! \param motion pr�ba pomiarowa
    //! \param collection kolekcja zawieraj�ca wektory
    //! \param name nazwa tworzonego elementu
    //! \param childIcon ikona dla li�ci
    //! \param parentItem rodzic dla tworzonych element�w
    //! \param createContainerItem czy stworzy� element pomi�dzy rodzicem a li��mi
    template <class Channel, class CollectionPtr>
    static void tryAddVectorToTree(const PluginSubject::MotionConstPtr & motion, const CollectionPtr & collection, const std::string& name, const QIcon& childIcon,  core::IHierarchyItemPtr parentItem, bool createContainerItem = true );

    static core::HierarchyHelperPtr allTFromSession( const std::string& channelName, PluginSubject::SessionConstPtr s, int channelNo );
    static core::HierarchyHelperPtr createNormalized( utils::ObjectWrapperConstPtr wrapper, PluginSubject::MotionConstPtr motion, c3dlib::C3DParser::IEvent::Context context );
    static core::HierarchyHelperPtr createNormalizedFromAll( const std::string& channelName, PluginSubject::SessionConstPtr s, c3dlib::C3DParser::IEvent::Context context );
    static QString createDescription( PluginSubject::MotionConstPtr motion);
};


#endif
