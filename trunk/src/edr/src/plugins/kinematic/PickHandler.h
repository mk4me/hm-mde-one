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

//! klasa pomocnicza, dziêki niej mo¿na ³atwo podpi¹æ siê pod system pickingu
class PickHandler : public osgGA::GUIEventHandler
{
public:
    //! wskaŸnik do wêz³a przechowywuj¹cego geometriê
    typedef osg::Geode* GeodePtr;
    //! wskaŸnik do obiektu, który mo¿e zostaæ odrysowany na scenie
    typedef osg::ref_ptr<osg::Drawable> DrawablePtr;
    //! krotka przechowuj¹ca : wêze³, jego obiekt, który zosta³ klikniêty, doleg³oœæ od obserwatora
    typedef boost::tuple<GeodePtr, DrawablePtr, double> PickerTuple;
    //! lista obiektów, które zosta³y "klikniête"
    typedef std::list<PickerTuple> PickerList;
    //! funktor realizuj¹cy jak¹œ akcjê po klikniêciu
    typedef boost::function<void (const PickerList& )> HandlerFunction;

public:
    //! Konstruktor
    //! \param callback funkcja, która zostanie wywo³ana za ka¿dym razem, gdy kliknie siê obiekt na scenie
    PickHandler(HandlerFunction callback);

public:
    //! Obsluga eventow
    //! \param ea eventy myszki, klawiatury lub okna
    //! \param aa interfejs obs³ugi akcji
    bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
    //! Metoda realizuj¹ca faktyczny picking
    //! \param view widok sceny
    //! \param ea eventy myszki, klawiatury lub okna
    virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

protected:
    //! funktor realiuzj¹cy pewn¹ akcjê po klikniêciu
    HandlerFunction handlerFunction;
};



#endif
