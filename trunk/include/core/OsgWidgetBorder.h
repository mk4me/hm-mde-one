/********************************************************************
	created:  2010/11/08
	created:  8:11:2010   17:03
	filename: OsgWidgetBorder.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__OSGWIDGETDECORATION_H__
#define __HEADER_GUARD__OSGWIDGETDECORATION_H__

#include <osgWidget/Widget>
#include <osgWidget/Label>
#include <osgWidget/Window>
#include <core/Enumerators.h>
#include <core/Buttonized.h>
#include <core/Borderized.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class OsgWidgetBorder : public osg::Referenced
{
private:
    //typedef osgUI::Borderize<osgUI::Button, true> Background;
   // typedef osgUI::Borderize<osgWidget::Widget> Background2;
    osg::ref_ptr<osgWidget::Label> label;
    osg::ref_ptr<osgWidget::Widget> overlay;
    osgWidget::Window* parent;

public:
    //!
    OsgWidgetBorder();

    //! 
    //! \param proto
    void onClone(const OsgWidgetBorder& proto);

    //! \return
    osgWidget::Window* getParent();
    //! \return
    const osgWidget::Window* getParent() const;
    //! \param parent
    void setParent(osgWidget::Window* parent);

    //! \param x
    //! \param y
    //! \param width
    //! \param height
    //! \param thickness
    void setLocation(float x, float y, float width, float height, float thickness = 1);

    //! 
    osgWidget::Label* getLabel();
    //! 
    const osgWidget::Label* getLabel() const;
    //! 
    bool getShowLabel();
    //! \param value
    void setShowLabel(bool value);

    //! 
    //! \param color
    void setBorderColor( osgWidget::Color color );
    //! 
    //! \param r
    //! \param g
    //! \param b
    //! \param a
    void setBorderColor(osgWidget::color_type r, osgWidget::color_type g, 
        osgWidget::color_type b, osgWidget::color_type a);
    //! \return Kolor obramowania.
    osgWidget::Color getBorderColor() const;
    //! \return Czy pokazuje obramowanie?
    bool getShowBorder() const;
    //! \param value Czy obramowanie ma byæ pokazywane?
    void setShowBorder(bool value);

private:
    bool onOverItem();

};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__OSGWIDGETDECORATION_H__