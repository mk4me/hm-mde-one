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

//! Klasa wzorowana na przyk�adzie z osg, pozwala zachowa� sta�y rozmiar manipulatora niezaleznie od odleg�o�ci od kamery
class DraggerContainer : public osg::Group
{
public:
    //! ustawienie domy�lnych parametr�w
    DraggerContainer() : _draggerSize(100.0f), _active(true) {}
    //! Konstruktor kopiuj�cy (w tylu osg)
    //! \param copy kopiowany obiekt
    //! \param copyop rodzaj kopii (p�ytka, g��boka, ... )
    DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    
public:
    META_Node( osgManipulator, DraggerContainer );
    //! Ustawienie manipulatora, kt�ry ma zachowa� jednakowy rozmiar
    //! \param dragger 
    void setDragger( osgManipulator::Dragger* dragger );
    //! \return ustawiony manipulator, kt�ry ma zachowa� jednakowy rozmiar
    osgManipulator::Dragger* getDragger() { return _dragger.get(); }
    //! \return ustawiony manipulator, kt�ry ma zachowa� jednakowy rozmiar
    const osgManipulator::Dragger* getDragger() const { return _dragger.get(); }
    //! nowy rozmiar manipulaotra
    //! \param size rozmiar w jednostkach sceny
    void setDraggerSize( float size ) { _draggerSize = size; }
    //! \return pobiera rozmiar manipulatora w jednostkach sceny
    float getDraggerSize() const { return _draggerSize; }
    //! zmiana aktywno�ci manipulatora
    void setActive( bool b ) { _active = b; }
    //! \return aktywno�� manipulatora
    bool getActive() const { return _active; }
    //! Metoda wykonuje g��wn� prac�, dostosowywuje rozmiar do faktycznej rozdzielczo�ci okna, w kt�rym odbywa si� renderowanie
    //! \param nv obiekt odwiedzaj�cy w�ze� (zgodnie ze wzorcem projektowym)
    void traverse( osg::NodeVisitor& nv );
    //! Ustawia punkt obrotu manipulatora
    //! \param pivot nowy punkt obrotu
    void setDraggerPivot( const osg::Vec3& pivot );
protected:
    //! opakowywany manipulator
    osg::ref_ptr<osgManipulator::Dragger> _dragger;
    //! sta�y rozmiar manipulatora
    float _draggerSize;
    //! czy aktualnie u�ywany
    bool _active;
};
typedef osg::ref_ptr<DraggerContainer> DraggerContainerPtr;

//! Klasa s�u��ca do obs�ugi manipulator�w
class Manipulators
{
public:
    //! rodzaje dost�pnych manipulator�w
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

    //! Pod��czenie manipulatora mi�dzy parenta i childa
    //! \param parent do niego zostanie podpi�ty manipulator
    //! \param child obiekt, na kt�rym b�dzie dzia�a� manipulator
    //! \param draggerContainer opakowany manipulator (sta�y rozmiar)
    //! \param pivot punkt zaczepienia manipulatora
    static void connect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot );
    //! Roz��czenie manipulaotra
    //! \param parent od niego trzeba si� odpi��
    //! \param child obiekt, kt�ry przestaje by� manipulowany
    //! \param draggerContainer odpinany manipulaotr
    static void disconnect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer);
    //! Metoda fabrykuj�ca manipulatory
    //! \param type typ manipulaotra
    //! \return stworzony obiekt
    static osgManipulator::Dragger* createDragger(DraggerType type);
    //! Metoda fabrykuj�ca manipulatory o sta�ym rozmiarze
    //! \param type  typ manipulaotra
    //! \return stworzony obiekt
    static DraggerContainer* creatreDraggerContainer(DraggerType type);
    //! Metoda tworzy nowy manipulator i podpina go pod w�ze�, kt�ry chcemy manipulowa�
    //! \param scene w�ze�, kt�ry b�dzie manipulowany
    //! \param type typ manipulatora
    //! \param fixedSizeInScreen czy manipulator ma mie� sta�y rozmiar
    //! \return w�ze� zawieraj�cy manipulator
    static osg::Node* addDraggerToScene(osg::PositionAttitudeTransform* scene, DraggerType type, bool fixedSizeInScreen);
};
#endif
