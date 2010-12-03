/********************************************************************
	created:  2010/11/16
	created:  16:11:2010   1:20
	filename: Borderized.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__BORDERIZED_H__
#define __HEADER_GUARD__BORDERIZED_H__

#include <boost/type_traits.hpp>
#include <osg/LineWidth>
#include <osgWidget/Widget>
#include <osgWidget/Window>
#include <osgWidget/WindowManager>
#include <utils/Debug.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

// template <class Base, bool Interactive = false, osg::PrimitiveSet::Mode Primitives = osg::PrimitiveSet::QUADS>
// class Borderized;

/**
 *	Klasa do opakowywania dowolnych widgetów w ramkê. Wariant QUAD generujê ramkê
 *  "do œrodka", podczas gdy LINE_STRIP po równo do œrodka i na zewn¹trz.
 */
template <class Base, osg::PrimitiveSet::Mode Primitives = osg::PrimitiveSet::QUADS>
class Borderized : public Base
{
    UTILS_STATIC_ASSERT((boost::is_base_of<osgWidget::Widget, Base>::value));

public:
    META_Object(osgUI, Borderized);

private:
    //! Typ u¿ywany do wyró¿niania wariantów.
    template <osg::PrimitiveSet::Mode M>
    struct Mode : public boost::integral_constant<osg::PrimitiveSet::Mode, M>
    {};
    //! Geometria ramki.
    osg::ref_ptr<osg::Geometry> border;
    //!
    unsigned borderIdx;
    //! Gruboœæ ramki.
    float thickness;
   
public:
    //! 
    //! \param name
    Borderized(const std::string & name = "" ) :
        Base(name)
    {
        init();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Widget
    //! \param name
    Borderized(const std::string & name, osgWidget::point_type width, osgWidget::point_type height = 0) :
    Base(name, width, height)
    {
        init();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Label
    //! \param name
    Borderized(const std::string & name, const std::string& label) :
    Base(name, label, width, height)
    {
        init();
    }

    //! 
    //! \param borderized
    //! \param copyop
    Borderized(const Borderized& borderized, const osg::CopyOp& copyop) :
        Base(borderized, copyop),
        thickness(borderized.thickness),
        borderIdx(borderized.borderIdx)
    {
        // inspirowane osgWidget::Label
        border = new osg::Geometry(*borderized.border, copyop);
    }


    //! \return Gruboœæ ramki.
    float getBorderWidth() const
    { 
        return thickness;
    }
    //! \param thickness Gruboœæ ramki.
    void setBorderWidth(float thickness) 
    { 
        this->thickness = thickness; 
        positionBorder( Mode<Primitives>() );
    }

    //! \return Kolor ramki.
    osgWidget::Color getBorderColor() const
    {
        return (*getBorderColorArray())[0];
    }
    //! \param color Kolor ramki.
    void setBorderColor(osgWidget::Color color)
    {
        (*getBorderColorArray())[0] = color;
        border->setColorArray(getBorderColorArray());
    }

public:
    //! Dodaje ramkê do parenta.
    //! \param window
    virtual void parented(osgWidget::Window* window)
    {
        Base::parented(window);
        osg::Geode* geode = window->getGeode();
        if ( borderIdx ) {
            // prawdopodobnie zostaliœmy sklonowani
            osg::Geometry* geom = dynamic_cast<osg::Geometry*>(geode->getDrawable(borderIdx));
            if ( geom ) {
                geode->setDrawable( borderIdx, border.get() );
            } else {
                borderIdx = window->addDrawableAndGetIndex(border.get());
            }
        } else {
            borderIdx = window->addDrawableAndGetIndex(border.get());
        }

        //unsigned idx = geode->getDrawableIndex(border.get());
        //if ( idx != geode->getNumDrawables() ) {
        //    borderIdx = idx;
        //} else {
        //    borderIdx = window->addDrawableAndGetIndex(border.get());
        //}
    }
    //! Usuwa ramkê z parenta.
    //! \param window
    virtual void unparented(osgWidget::Window* window)
    {
        Base::unparented(window);
        if ( borderIdx ) {
            window->getGeode()->removeDrawable(border.get());
            borderIdx = 0;
        }
    }
    //! Pozycjonuje wierzcho³ki.
    virtual void positioned()
    {
        Base::positioned();
        positionBorder( Mode<Primitives>() );
    }

protected:
    //! \return Tablica kolorów ramki.
    osgWidget::ColorArray* getBorderColorArray()
    {
        return dynamic_cast<osgWidget::ColorArray*>(border->getColorArray());
    }
    //! \return Tablica kolorów ramki.
    const osgWidget::ColorArray* getBorderColorArray() const
    {
        return dynamic_cast<osgWidget::ColorArray*>(border->getColorArray());
    }
    //! \return Tablica pozycji ramki.
    osg::Vec3Array* getBorderPositionData()
    {
        return dynamic_cast<osg::Vec3Array*>(border->getVertexArray());
    }

private:

    //! Inicjalizacja.
    void init() 
    {
        thickness = 1;
        borderIdx = 0;
        border = new osg::Geometry();
        border->setDataVariance( osg::Object::DYNAMIC );
        // stworzenie kolorów
        osg::Vec4Array* colors = new osg::Vec4Array(1);
        border->setColorArray(colors);
        border->setColorBinding(osg::Geometry::BIND_OVERALL);
        // stworzenie wierzcho³ków
        positionBorder( Mode<Primitives>() );
    }

    //! \return Pozycja lewego dolnego rogu ramki.
    osg::Vec3 getCornerPosition()
    {
        // ustalenie pozycji ramki (lewego dolnego rogu)
        float x, y, z;
        x = getX();
        y = getY();
        z = _calculateZ(getLayer() + 1);
        const osgWidget::WindowManager* wm = _getWindowManager();
        if(wm && wm->isUsingRenderBins()) {
            border->getOrCreateStateSet()->setRenderBinDetails(
                static_cast<int>(z * osgWidget::OSGWIDGET_RENDERBIN_MOD),
                "RenderBin"
                );
            z = 0.0f;
        }

        // ustawienie pozycji
        osg::Vec3 corner(x, y, z);
        return corner;
    }


    //! Tworzy ramkê w postaci LINE_STRIP
    //! \param corner
    //! \param w
    //! \param h
    //! \param 
    void positionBorder( Mode<osg::PrimitiveSet::LINE_STRIP> )
    {
        osg::Vec3 corner = getCornerPosition();
        osg::Vec3 h = osg::Vec3(0, getHeight(), 0);
        osg::Vec3 w = osg::Vec3(getWidth(), 0, 0);
        
        // inicjalizacja tablicy
        osg::Vec3Array* positions = getBorderPositionData();
        if ( !positions ) {
            positions = new osg::Vec3Array(5);
            border->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, positions->size()));
        }

        // wype³nienie danymi
        (*positions)[0] = corner + h;
        (*positions)[1] = corner;
        (*positions)[2] = corner + w;
        (*positions)[3] = corner + w + h;
        (*positions)[4] = (*positions)[0];

        // gruboœæ linii
        osg::LineWidth* lineWidth = dynamic_cast<osg::LineWidth*>(border->getOrCreateStateSet()->getAttribute(osg::StateAttribute::LINEWIDTH));
        if ( lineWidth ) {
            lineWidth->setWidth( thickness );
        } else {
            border->getOrCreateStateSet()->setAttribute(new osg::LineWidth(thickness), osg::StateAttribute::ON);
        }
        border->setVertexArray(positions);
    }

    //! Tworzy ramkê w postaci QUAD
    //! \param corner
    //! \param w
    //! \param h
    //! \param 
    void positionBorder( Mode<osg::PrimitiveSet::QUADS> )
    {
        osg::Vec3 corner = getCornerPosition();
        osg::Vec3 h = osg::Vec3(0, getHeight(), 0);
        osg::Vec3 w = osg::Vec3(getWidth(), 0, 0);

        // inicjalizacja tablicy
        osg::Vec3Array* positions = getBorderPositionData();
        if ( !positions ) {
            positions = new osg::Vec3Array(16);
            border->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, positions->size()));
        }

        osg::Vec3 ul0 = corner+h;
        osg::Vec3 ul1 = ul0 + osg::Vec3( thickness,-thickness, 0);
        osg::Vec3 ll0 = corner;
        osg::Vec3 ll1 = ll0 + osg::Vec3( thickness, thickness, 0);
        osg::Vec3 lr0 = corner+w;
        osg::Vec3 lr1 = lr0 + osg::Vec3(-thickness, thickness, 0);
        osg::Vec3 ur0 = corner+w+h;
        osg::Vec3 ur1 = ur0 + osg::Vec3(-thickness,-thickness, 0);

        (*positions)[0] = ul0;
        (*positions)[1] = ll0;
        (*positions)[2] = ll1;
        (*positions)[3] = ul1;

        (*positions)[4] = ll1;
        (*positions)[5] = ll0;
        (*positions)[6] = lr0;
        (*positions)[7] = lr1;

        (*positions)[8] = ur1;
        (*positions)[9] = lr1;
        (*positions)[10]= lr0;
        (*positions)[11]= ur0;

        (*positions)[12]= ul0;
        (*positions)[13]= ul1;
        (*positions)[14]= ur1;
        (*positions)[15]= ur0;

        border->setVertexArray(positions);
    }

    //! 
    //! \param 
    template <osg::PrimitiveSet::Mode Primitives>
    void positionBorder( Mode<Primitives> )
    {
        UTILS_STATIC_ASSERT(false);
    }
};

template <class Base>
class BorderStylePolicy : public Base
{
private:
    //! Kolor normalny.
    osgWidget::Color borderNormalColor;
    //! Kolor przy najechaniu.
    osgWidget::Color borderHooverColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color borderPushedColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color borderToggleColor;
public:    //! 
    //! \param name
    BorderStylePolicy() :
        Base()
    {}

    //! 
    //! \param borderized
    //! \param copyop
    BorderStylePolicy(const BorderStylePolicy& border, const osg::CopyOp& copyop) :
        Base(border, copyop),
        borderNormalColor(border.borderNormalColor),
        borderHooverColor(border.borderHooverColor),
        borderToggleColor(border.borderToggleColor),
        borderPushedColor(border.borderPushedColor)
    {}

public:
    //! \return
    osgWidget::Color getBorderNormalColor() const
    { 
        return borderNormalColor;
    }
    //! \param borderNormalColor
    void setBorderNormalColor(osgWidget::Color borderNormalColor) 
    { 
        this->borderNormalColor = borderNormalColor; 
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getBorderHooverColor() const
    { 
        return borderHooverColor;
    }
    //! \param borderHooverColor
    void setBorderHooverColor(osgWidget::Color borderHooverColor) 
    { 
        this->borderHooverColor = borderHooverColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getBorderPushedColor() const
    { 
        return borderPushedColor;
    }
    //! \param borderPushedColor
    void setBorderPushedColor(osgWidget::Color borderPushedColor) 
    { 
        this->borderPushedColor = borderPushedColor; 
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getBorderToggleColor() const
    { 
        return borderToggleColor;
    }
    //! \param borderToggleColor
    void setBorderToggleColor(osgWidget::Color borderToggleColor) 
    { 
        this->borderToggleColor = borderToggleColor; 
        onStyleChanged();
    }
    //!
    void setBorderAllColors(osgWidget::Color color)
    {
        borderNormalColor = borderHooverColor = borderPushedColor = borderToggleColor = color;
        onStyleChanged();
    }

protected:

    //! 
    //! \param widget
    //! \param toggled
    //! \param pushed
    //! \param hoovered
    template <class Borderized>
    void applyStyle(Borderized* widget, bool toggled, bool pushed, bool hoovered)
    {
        Base::applyStyle(widget, toggled, pushed, hoovered);
        if ( toggled ) {
            widget->setBorderColor( borderToggleColor );
        } else {
            widget->setBorderColor( borderNormalColor );
        }
        if ( hoovered ) {
            if ( pushed ) {
                widget->setBorderColor( borderPushedColor );
            } else {
                widget->setBorderColor( borderHooverColor );
            }
        }
    }
};


//
///**
// *	Szablon obramowania rozszerzony o cechy interaktywnego widgetu.
// *  TODO: rozwa¿yæ zrobienie jednej klasy opatrzonej statycznymi asercjami.
// */
//template <class Base, osg::PrimitiveSet::Mode Primitives>
//class Borderized<Base, true, Primitives> : public Borderized<Base, false, Primitives>
//{
//public:
//    typedef Borderized<Base, true, Primitives> BorderizedSpecialization;
//    META_Object(osgUI, BorderizedSpecialization);
//private:
//    //! Kolor normalny.
//    osgWidget::Color borderNormalColor;
//    //! Kolor przy najechaniu.
//    osgWidget::Color borderHooverColor;
//    //! Kolor podczas klikniêcia.
//    osgWidget::Color borderPushedColor;
//    //! Kolor podczas klikniêcia.
//    osgWidget::Color borderToggleColor;
//
//public:
//
//    //! 
//    //! \param name
//    Borderized(const std::string & name = "" ) :
//        Borderized<Base, false, Primitives>(name)
//    {}
//
//    //! 
//    //! \param borderized
//    //! \param copyop
//    Borderized(const Borderized& borderized, const osg::CopyOp& copyop) :
//        Borderized<Base, false, Primitives>(borderized, copyop),
//        borderNormalColor(borderized.borderNormalColor),
//        borderHooverColor(borderized.borderHooverColor),
//        borderToggleColor(borderized.borderToggleColor),
//        borderPushedColor(borderized.borderPushedColor)
//    {}
//
//    //! \return
//    osgWidget::Color getBorderNormalColor() const
//    { 
//        return borderNormalColor;
//    }
//    //! \param borderNormalColor
//    void setBorderNormalColor(osgWidget::Color borderNormalColor) 
//    { 
//        this->borderNormalColor = borderNormalColor; 
//        Base::refresh();
//    }
//    //! \return
//    osgWidget::Color getBorderHooverColor() const
//    { 
//        return borderHooverColor;
//    }
//    //! \param borderHooverColor
//    void setBorderHooverColor(osgWidget::Color borderHooverColor) 
//    { 
//        this->borderHooverColor = borderHooverColor;
//        Base::refresh();
//    }
//    //! \return
//    osgWidget::Color getBorderPushedColor() const
//    { 
//        return borderPushedColor;
//    }
//    //! \param borderPushedColor
//    void setBorderPushedColor(osgWidget::Color borderPushedColor) 
//    { 
//        this->borderPushedColor = borderPushedColor; 
//        Base::refresh();
//    }
//    //! \return
//    osgWidget::Color getBorderToggleColor() const
//    { 
//        return borderToggleColor;
//    }
//    //! \param borderToggleColor
//    void setBorderToggleColor(osgWidget::Color borderToggleColor) 
//    { 
//        this->borderToggleColor = borderToggleColor; 
//        Base::refresh();
//    }
//    //!
//    void setBorderAllColors(osgWidget::Color color)
//    {
//        borderNormalColor = borderHooverColor = borderPushedColor = borderToggleColor = color;
//        Base::refresh();
//    }
//
//protected:
//    //! Wywo³ywane w pierwszej kolejnoœci.
//    virtual void applyState(bool triggered)
//    {
//        Base::applyState(triggered);
//        if ( triggered ) {
//            setBorderColor( borderToggleColor );
//        } else {
//            setBorderColor( borderNormalColor );
//        }
//    }
//    //! Wywo³ywane w drugiej kolejnoœci.
//    virtual void applyHoover(bool pushed)
//    {
//        Base::applyHoover(pushed);
//        if ( pushed ) {
//            setBorderColor( borderPushedColor );
//        } else {
//            setBorderColor( borderHooverColor );
//        }
//    }
//};
//

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__BORDERIZED_H__
