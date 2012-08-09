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

//! klasa pomocnicza, dzieki niej mozna latwo podpiac sie pod system pickingu
class PickHandler : public osgGA::GUIEventHandler
{
public:
    typedef osg::Geode* GeodePtr;
    typedef osg::ref_ptr<osg::Drawable> DrawablePtr;
    typedef boost::tuple<GeodePtr, DrawablePtr, double> PickerTuple;
    typedef std::list<PickerTuple> PickerList;
    typedef boost::function<void (const PickerList& )> HandlerFunction;

public:
    //! Konstruktor
    //! \param callback funkcja, ktora zostanie wywolana za kazdym razem, gdy kliknie sie obiekt na scenie
    PickHandler(HandlerFunction callback);

public:
    //! Obsluga eventow
    //! \param ea
    //! \param aa
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    //! Metoda realizujaca faktyczny picking
    //! \param view
    //! \param ea
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

protected:
    HandlerFunction handlerFunction;
};



#endif
