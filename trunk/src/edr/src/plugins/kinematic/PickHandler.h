/********************************************************************
	created:	2012/03/09
	created:	9:3:2012   12:10
	filename: 	PickHandler.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__PICKHANDLER_H__
#define HEADER_GUARD_KINEMATIC__PICKHANDLER_H__


#include <osg/Geode>
#include <osg/Drawable>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>
#include <osgViewer/View>

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <list>

//! klasa pomocnicza, dzi�ki niej mo�na �atwo podpi�� si� pod system pickingu
class PickHandler : public osgGA::GUIEventHandler
{
public:
    //! wska�nik do w�z�a przechowywuj�cego geometri�
    typedef osg::Geode* GeodePtr;
    //! wska�nik do obiektu, kt�ry mo�e zosta� odrysowany na scenie
    typedef osg::ref_ptr<osg::Drawable> DrawablePtr;
    //! krotka przechowuj�ca : w�ze�, jego obiekt, kt�ry zosta� klikni�ty, doleg�o�� od obserwatora
    typedef boost::tuple<GeodePtr, DrawablePtr, double> PickerTuple;
    //! lista obiekt�w, kt�re zosta�y "klikni�te"
    typedef std::list<PickerTuple> PickerList;
    //! funktor realizuj�cy jak�� akcj� po klikni�ciu
    typedef boost::function<void (const PickerList& )> HandlerFunction;

public:
    //! Konstruktor
    //! \param callback funkcja, kt�ra zostanie wywo�ana za ka�dym razem, gdy kliknie si� obiekt na scenie
    PickHandler(HandlerFunction callback);

public:
    //! Obsluga eventow
    //! \param ea eventy myszki, klawiatury lub okna
    //! \param aa interfejs obs�ugi akcji
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    //! Metoda realizuj�ca faktyczny picking
    //! \param view widok sceny
    //! \param ea eventy myszki, klawiatury lub okna
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

protected:
    //! funktor realiuzj�cy pewn� akcj� po klikni�ciu
    HandlerFunction handlerFunction;
};



#endif
