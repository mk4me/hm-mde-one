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

//! Klasa wzorowana na przykladzie z osg, pozwala zachowac staly rozmiar manipulatora niezaleznie od odleglosci od kamery
class DraggerContainer : public osg::Group
{
public:
    //! ustawienie domyslnych parametrow
    DraggerContainer() : _draggerSize(100.0f), _active(true) {}
    //! Konstruktor kopiujacy (w tylu osg)
    //! \param copy 
    //! \param copyop 
    DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY );
    
public:
    META_Node( osgManipulator, DraggerContainer );
    //! Ustawienie manipulatora, ktory ma zachowac jednakowy rozmiar
    //! \param dragger 
    void setDragger( osgManipulator::Dragger* dragger );
    //! \return ustawiony manipulator, ktory ma zachowac jednakowy rozmiar
    osgManipulator::Dragger* getDragger() { return _dragger.get(); }
    //! \return ustawiony manipulator, ktory ma zachowac jednakowy rozmiar
    const osgManipulator::Dragger* getDragger() const { return _dragger.get(); }
    //! nowy rozmiar manipulaotra
    //! \param size 
    void setDraggerSize( float size ) { _draggerSize = size; }
    //! 
    float getDraggerSize() const { return _draggerSize; }
    //! 
    //! \param b 
    void setActive( bool b ) { _active = b; }
    //! 
    bool getActive() const { return _active; }
    //! Metoda wykonuje glowna prace, dostosowywuje rozmiar do faktycznej rozdzielczosci okna, w ktorym odbywa sie renderowanie
    //! \param nv 
    void traverse( osg::NodeVisitor& nv );
    //! 
    //! \param pivot 
    void setDraggerPivot( const osg::Vec3& pivot );
protected:
    //! opakowywany manipulator
    osg::ref_ptr<osgManipulator::Dragger> _dragger;
    //! staly rozmiar manipulatora
    float _draggerSize;
    //! czy aktualnie uzywany
    bool _active;
};
typedef osg::ref_ptr<DraggerContainer> DraggerContainerPtr;

//! Klasa sluzaca do obslugi manipulatorow
class Manipulators
{
public:
    //! rodzaje dostepnych manipulatorow
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

    //! Podlaczenie manipulatora miedzy parenta i childa
    //! \param parent do niego zostanie podpiety manipulator
    //! \param child obiekt, na ktorym bedzie dzialal manipulator
    //! \param draggerContainer opakowany manipulator (staly rozmiar)
    //! \param pivot punkt zaczepienie manipulatora
    static void connect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot );
    //! Rozlaczenie manipulaotra
    //! \param parent od niego trzeba sie odpiac
    //! \param child obiekt, ktory przestaje byc manipulowany
    //! \param draggerContainer odpinany manipulaotr
    static void disconnect(osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer);
    //! Metoda fabrykujaca manipulatory
    //! \param type typ manipulaotra
    //! \return stworzony obiekt
    static osgManipulator::Dragger* createDragger(DraggerType type);
    //! Metoda fabrykujaca manipulatory o stalym rozmiarze
    //! \param type  typ manipulaotra
    //! \return stworzony obiekt
    static DraggerContainer* creatreDraggerContainer(DraggerType type);
    //! Metoda tworzy nowy manipulator i podpina go pod wezel, ktory chcemy manipulowac
    //! \param scene wezel, ktory bedzie manipulowany
    //! \param type typ manipulatora
    //! \param fixedSizeInScreen czy manipulator ma miec staly rozmiar
    //! \return wezel zawierajacy manipulator
    static osg::Node* addDraggerToScene(osg::PositionAttitudeTransform* scene, DraggerType type, bool fixedSizeInScreen);
};
#endif
