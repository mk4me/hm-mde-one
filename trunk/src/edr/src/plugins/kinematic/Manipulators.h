/********************************************************************
	created:	2012/03/12
	created:	12:3:2012   11:17
	filename: 	Manipulators.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__MANIPULATORS_H__
#define HEADER_GUARD_KINEMATIC__MANIPULATORS_H__

#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <osgViewer/Viewer>
#include <osg/CoordinateSystemNode>
#include <osgText/Text>

#include <osgManipulator/TabBoxDragger>
#include <osgManipulator/TabBoxTrackballDragger>
#include <osgManipulator/TabPlaneDragger>
#include <osgManipulator/TabPlaneTrackballDragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Translate1DDragger>
#include <osgManipulator/Translate2DDragger>
#include <osgManipulator/TranslateAxisDragger>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>

#include <iostream>

//! Klasa wzorowana na przyk³adzie z osg, pozwala zachowaæ sta³y rozmiar manipulatora niezaleznie od odleg³oœci od kamery
class DraggerContainer : public osg::Group
{
public:
    //! ustawienie domyœlnych parametrów
    DraggerContainer() : _draggerSize(100.0f), _active(true) {}
    //! Konstruktor kopiuj¹cy (w tylu osg)
    //! \param copy kopiowany obiekt
    //! \param copyop rodzaj kopii (p³ytka, g³êboka, ... )
    DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    
public:
    META_Node( osgManipulator, DraggerContainer );
    //! Ustawienie manipulatora, który ma zachowaæ jednakowy rozmiar
    //! \param dragger 
    void setDragger( osgManipulator::Dragger* dragger );
    //! \return ustawiony manipulator, który ma zachowaæ jednakowy rozmiar
    osgManipulator::Dragger* getDragger() { return _dragger.get(); }
    //! \return ustawiony manipulator, który ma zachowaæ jednakowy rozmiar
    const osgManipulator::Dragger* getDragger() const { return _dragger.get(); }
    //! nowy rozmiar manipulaotra
    //! \param size rozmiar w jednostkach sceny
    void setDraggerSize( float size ) { _draggerSize = size; }
    //! \return pobiera rozmiar manipulatora w jednostkach sceny
    float getDraggerSize() const { return _draggerSize; }
    //! zmiana aktywnoœci manipulatora
    void setActive( bool b ) { _active = b; }
    //! \return aktywnoœæ manipulatora
    bool getActive() const { return _active; }
    //! Metoda wykonuje g³ówn¹ pracê, dostosowywuje rozmiar do faktycznej rozdzielczoœci okna, w którym odbywa siê renderowanie
    //! \param nv obiekt odwiedzaj¹cy wêze³ (zgodnie ze wzorcem projektowym)
    void traverse( osg::NodeVisitor& nv );
    //! Ustawia punkt obrotu manipulatora
    //! \param pivot nowy punkt obrotu
    void setDraggerPivot( const osg::Vec3& pivot );
protected:
    //! opakowywany manipulator
    osg::ref_ptr<osgManipulator::Dragger> _dragger;
    //! sta³y rozmiar manipulatora
    float _draggerSize;
    //! czy aktualnie u¿ywany
    bool _active;
};
typedef osg::ref_ptr<DraggerContainer> DraggerContainerPtr;

//! Klasa s³u¿¹ca do obs³ugi manipulatorów
class Manipulators
{
public:
    //! rodzaje dostêpnych manipulatorów
    enum DraggerType 
    {
        TabPlane,
        TabPlaneTrackball,
        TabBoxTrackball,
        Trackball,
        Translate1D,
        Translate2D,
        TranslateAxis,
        TabBox
    };

    //! Pod³¹czenie manipulatora miêdzy parenta i childa
    //! \param parent do niego zostanie podpiêty manipulator
    //! \param child obiekt, na którym bêdzie dzia³a³ manipulator
    //! \param draggerContainer opakowany manipulator (sta³y rozmiar)
    //! \param pivot punkt zaczepienia manipulatora
    static void connect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot );
    //! Roz³¹czenie manipulaotra
    //! \param parent od niego trzeba siê odpi¹æ
    //! \param child obiekt, który przestaje byæ manipulowany
    //! \param draggerContainer odpinany manipulaotr
    static void disconnect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer);
    //! Metoda fabrykuj¹ca manipulatory
    //! \param type typ manipulaotra
    //! \return stworzony obiekt
    static osgManipulator::Dragger* createDragger(DraggerType type);
    //! Metoda fabrykuj¹ca manipulatory o sta³ym rozmiarze
    //! \param type  typ manipulaotra
    //! \return stworzony obiekt
    static DraggerContainer* creatreDraggerContainer(DraggerType type);
    //! Metoda tworzy nowy manipulator i podpina go pod wêze³, który chcemy manipulowaæ
    //! \param scene wêze³, który bêdzie manipulowany
    //! \param type typ manipulatora
    //! \param fixedSizeInScreen czy manipulator ma mieæ sta³y rozmiar
    //! \return wêze³ zawieraj¹cy manipulator
    static osg::Node* addDraggerToScene(osg::PositionAttitudeTransform* scene, DraggerType type, bool fixedSizeInScreen);
};
#endif
