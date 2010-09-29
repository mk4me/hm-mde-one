#include <osgWidget/Widget>
#include "Button.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

Button::Button( const std::string & name /*= ""*/, osgWidget::Color color /*= osgWidget::Color()*/, 
               double width /*= 0.0*/, double height /*= 0.0*/ ) : 
  osgWidget::Widget(name, width, height), 
  currentColor(&normalColor),
  normalColor(color), 
  hooverColor(color), 
  clickedColor(color), 
  pushed(false), 
  dragged(false), 
  mouseOver(false)
{
  setColor(*currentColor);
  setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
}

Button::Button( const Button& button, const osg::CopyOp& copyop ) : 
  osgWidget::Widget(button, copyop),
  normalColor(button.normalColor),
  hooverColor(button.hooverColor),
  clickedColor(button.clickedColor),
  pushed(button.pushed),
  dragged(button.dragged),
  mouseOver(button.mouseOver),
  currentColor(&normalColor)
{
  setColor(*currentColor);
}

bool Button::mousePush( double x, double y, const osgWidget::WindowManager* manager )
{
  setColor(*(currentColor = &clickedColor));
  pushed = true;
  return false;
}

bool Button::mouseRelease( double x, double y, const osgWidget::WindowManager* manager )
{
  osgWidget::XYCoord xy = localXY(x, y);
  if ( xy.x() >= 0.0 && xy.y() >= 0 && xy.x() <= getWidth() && xy.y() <= getHeight() ) {
    setColor(*(currentColor = &hooverColor));
  } else {
    setColor(*(currentColor = &normalColor));
  }
  pushed = false;
  dragged = false;
  return false;
}

bool Button::mouseEnter( double x, double y, const osgWidget::WindowManager* manager )
{
  setColor(*(currentColor = &hooverColor));
  mouseOver = true;
  return false;
}

bool Button::mouseDrag( double x, double y, const osgWidget::WindowManager* manager )
{
  dragged = true;
  return false;
}

void Button::updateColor( osgWidget::Color & ref, osgWidget::Color value )
{
  ref = value;
  if ( currentColor == &ref ) {
    setColor(*currentColor);
  }
}

bool Button::mouseLeave( double x, double y, const osgWidget::WindowManager* manager )
{
  setColor(*(currentColor = &normalColor));
  mouseOver = false;
  return false;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////