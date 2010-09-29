/********************************************************************
	created:  2010/06/10
	created:  10:6:2010   10:36
	filename: Button.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__BUTTON_H__
#define __HEADER_GUARD__BUTTON_H__

#include <osgWidget/Widget>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	
 */
class Button : public osgWidget::Widget
{
private:
  //! Bie¿¹cy kolor.
  /** U¿ywany do ustawiania i sprawdzania bie¿¹cej wartoœci. */
  const osgWidget::Color * currentColor;
  //! Kolor normalny.
  osgWidget::Color normalColor;
  //! Kolor przy najechaniu.
  osgWidget::Color hooverColor;
  //! Kolor podczas klikniêcia.
  osgWidget::Color clickedColor;
  //!
  bool pushed;
  //!
  bool dragged;
  //!
  bool mouseOver;

public:
  META_Object(osgUI, Button);

  //! 
  //! \param name
  //! \param color
  //! \param width
  //! \param height
  Button(const std::string & name = "", osgWidget::Color color = osgWidget::Color(), 
    double width = 0.0, double height = 0.0);

  //! 
  //! \param button
  //! \param copyop
  Button(const Button& button, const osg::CopyOp& copyop);

  //! 
  //! \param x
  //! \param y
  //! \param manager
  virtual bool mousePush(double x, double y, const osgWidget::WindowManager* manager);

  //! 
  //! \param x
  //! \param y
  //! \param manager
  virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* manager);

  //! 
  //! \param x
  //! \param y
  //! \param manager
  virtual bool mouseEnter(double x, double y, const osgWidget::WindowManager* manager);

  //! 
  //! \param x
  //! \param y
  //! \param manager
  virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* manager);

  //! 
  //! \param x
  //! \param y
  //! \param manager
  virtual bool mouseDrag(double x, double y, const osgWidget::WindowManager* manager);

  //! \return
  inline osgWidget::Color getHooverColor() const
  { 
    return hooverColor;
  }
  //! \param hooverColor
  inline void setHooverColor(osgWidget::Color hooverColor) 
  { 
    updateColor(this->hooverColor, hooverColor); 
  }
  //! \return
  inline osgWidget::Color getClickedColor() const
  { 
    return clickedColor;
  }
  //! \param clickedColor
  inline void setClickedColor(osgWidget::Color clickedColor) 
  { 
    updateColor(this->clickedColor, clickedColor); 
  }
  //! \return
  inline osgWidget::Color getNormalColor() const
  { 
    return normalColor;
  }
  //! \param normalColor
  inline void setNormalColor(osgWidget::Color normalColor) 
  { 
    updateColor(this->normalColor, normalColor);
  }
  //! \return
  inline bool isPushed() const
  { 
    return pushed;
  }
  //! \return
  inline bool isDragged() const
  { 
    return dragged;
  }
  //! \return
  inline bool isMouseOver() const
  { 
    return mouseOver;
  }

protected:
  //! Aktualizuje kolor.
  //! \param ref Referencja do koloru.
  //! \param value Nowa wartoœæ.
  void updateColor(osgWidget::Color & ref, osgWidget::Color value);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__BUTTON_H__