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
#include <osgWidget/Label>
#include <boost/type_traits.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

template <class Base, class StylePolicy = DefaultStylePolicy>
class Buttonized : public Base, public StylePolicy
{
    UTILS_STATIC_ASSERT((boost::is_base_of<osgWidget::Widget, Base>::value));

public:
    META_Object(osgUI, Buttonized);

private:
    //! Czy jest wciœniêty?
    bool pushed;
    //! Czy mysz jest nad widgetem?
    bool hovered;
    //! Czy mo¿na wciskaæ?
    bool toggleEnabled;
    //! Czy wciœniêty?
    bool toggled;

public:
    //! 
    //! \param name
    //! \param color
    //! \param width
    //! \param height
    Buttonized(const std::string & name = "") :
    Base(name),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        onStyleChanged();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Widget
    Buttonized(const std::string & name, osgWidget::point_type width, osgWidget::point_type height = 0) :
    Base(name, width, height),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        onStyleChanged();
    }

    //! Konstruktor dla obiektu o cechach osgWidget::Label
    Buttonized(const std::string & name, const std::string& label) :
    Base(name, label),
    StylePolicy(),
    pushed(false),
    hovered(false),
    toggleEnabled(false),
    toggled(false)
    {
        onStyleChanged();
    }


          
    //! 
    //! \param button
    //! \param copyop
    Buttonized(const Buttonized& button, const osg::CopyOp& copyop) :
        Base(button, copyop), StylePolicy(button, copyop),
        pushed(button.pushed),
        hovered(button.hovered),
        toggleEnabled(button.toggleEnabled),
        toggled(button.toggled)
    {
        onStyleChanged();
    }

public:
    //! 
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mousePush(double x, double y, const osgWidget::WindowManager* manager)
    {
        pushed = true;
        onStyleChanged();
        return Base::mousePush(x, y , manager);
    }
    //! 
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* manager)
    {
        pushed = false;
        toggled = toggleEnabled ? !toggled : toggled;
        onStyleChanged();
        return Base::mouseRelease(x, y , manager);
    }
    //! 
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseEnter(double x, double y, const osgWidget::WindowManager* manager)
    {
        hovered = true;
        onStyleChanged();
        return Base::mouseEnter(x, y , manager);
    }
    //! 
    //! \param x
    //! \param y
    //! \param manager
    virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* manager)
    {
        hovered = false;
        onStyleChanged();
        return Base::mouseLeave(x, y , manager);
    }

public:
    //! \return
    bool isPushed() const
    { 
        return pushed;
    }
    //! \return
    bool isHovered() const
    { 
        return hovered;
    }
    //! \return
    bool isToggleEnabled() const
    { 
        return toggleEnabled;
    }
    //! \param checkable
    void setToggleEnabled(bool triggerEnabled) 
    { 
        this->toggleEnabled = triggerEnabled;
    }
    //! \return
    bool isToggled() const
    { 
        return toggled;
    }
    //! \param triggered
    void setToggled(bool triggered) 
    { 
        this->toggled = triggered;
        onStyleChanged();
    }

protected:
    //! Odœwie¿a wygl¹d przycisku.
    void onStyleChanged()
    {
        StylePolicy::applyStyle(this, isToggled(), isPushed(), isHovered());
    }
};

class NullStylePolicy
{
    NullStylePolicy()
    {}
    NullStylePolicy(const NullStylePolicy& policy, const osg::CopyOp& copyop)
    {}
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hoovered)
    {}
};

class DefaultStylePolicy
{
private:
    //! Kolor normalny.
    osgWidget::Color normalColor;
    //! Kolor przy najechaniu.
    osgWidget::Color hooverColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color pushedColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color toggleColor;
public:
    DefaultStylePolicy()
    {}
    DefaultStylePolicy(const DefaultStylePolicy& policy, const osg::CopyOp& copyop) :
        normalColor(policy.normalColor),
        hooverColor(policy.hooverColor),
        pushedColor(policy.pushedColor),
        toggleColor(policy.toggleColor)
    {}

public:
    //! \param hooverColor
    void setAllColors(osgWidget::Color color) 
    { 
        normalColor = hooverColor = pushedColor = toggleColor = color;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getHooverColor() const
    { 
        return hooverColor;
    }
    //! \param hooverColor
    void setHooverColor(osgWidget::Color hooverColor) 
    { 
        this->hooverColor = hooverColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getPushedColor() const
    { 
        return pushedColor;
    }
    //! \param clickedColor
    void setPushedColor(osgWidget::Color clickedColor) 
    { 
        this->pushedColor = clickedColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getNormalColor() const
    { 
        return normalColor;
    }
    //! \param normalColor
    void setNormalColor(osgWidget::Color normalColor) 
    { 
        this->normalColor = normalColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getToggleColor() const
    { 
        return toggleColor;
    }
    //! \param toggleColor
    void setToggleColor(osgWidget::Color toggleColor) 
    { 
        this->toggleColor = toggleColor;
        onStyleChanged();
    }

protected:

    //! Metoda do implementacji przez klasê pochodn¹. Powinna wywo³ywaæ metodê applyStyle.
    virtual void onStyleChanged() = 0;
    //! 
    //! \param widget
    //! \param toggled
    //! \param pushed
    //! \param hoovered
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hoovered)
    {
        if ( toggled ) {
            widget->setColor(toggleColor);
        } else {
            widget->setColor(normalColor);
        }

        if ( hoovered ) {
            osgWidget::Color src = pushed ? pushedColor : hooverColor;
            float srcA = src.a();
            osgWidget::Color dst = widget->getColor();
            float dstA = dst.a();

            // robimy tutaj mieszanie alpha
            // wyprowadzone z wzoru out = srca * src + (1-srca)*dst
            float a = dstA + srcA - dstA*srcA;
            osgWidget::Color out = (src*srcA + dst*dstA - dst*srcA*dstA)/a;
            out.a() = a;

            // ustawienie koloru
            widget->setColor(out);
        }
    }
};

class LabelStylePolicy : public DefaultStylePolicy
{
private:
    //! Kolor normalny.
    osgWidget::Color normalColor;
    //! Kolor przy najechaniu.
    osgWidget::Color hooverColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color pushedColor;
    //! Kolor podczas klikniêcia.
    osgWidget::Color toggleColor;
public:
    LabelStylePolicy()
    {}
    LabelStylePolicy(const LabelStylePolicy& policy, const osg::CopyOp& copyop) :
    DefaultStylePolicy(policy),
    normalColor(policy.normalColor),
    hooverColor(policy.hooverColor),
    pushedColor(policy.pushedColor),
    toggleColor(policy.toggleColor)
    {}

public:
    //! \param hooverColor
    void setLabelAllColors(osgWidget::Color color) 
    { 
        normalColor = hooverColor = pushedColor = toggleColor = color;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getLabelHooverColor() const
    { 
        return hooverColor;
    }
    //! \param hooverColor
    void setLabelHooverColor(osgWidget::Color hooverColor) 
    { 
        this->hooverColor = hooverColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getLabelPushedColor() const
    { 
        return pushedColor;
    }
    //! \param clickedColor
    void setLabelPushedColor(osgWidget::Color clickedColor) 
    { 
        this->pushedColor = clickedColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getLabelNormalColor() const
    { 
        return normalColor;
    }
    //! \param normalColor
    void setLabelNormalColor(osgWidget::Color normalColor) 
    { 
        this->normalColor = normalColor;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getLabelToggleColor() const
    { 
        return toggleColor;
    }
    //! \param toggleColor
    void setLabelToggleColor(osgWidget::Color toggleColor) 
    { 
        this->toggleColor = toggleColor;
        onStyleChanged();
    }

protected:

    //! Metoda do implementacji przez klasê pochodn¹. Powinna wywo³ywaæ metodê applyStyle.
    virtual void onStyleChanged() = 0;
    //! 
    //! \param widget
    //! \param toggled
    //! \param pushed
    //! \param hoovered
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hoovered)
    {
        DefaultStylePolicy::applyStyle(widget, toggled, pushed, hoovered);
        osgWidget::Label* label = dynamic_cast<osgWidget::Label*>(widget);
        UTILS_ASSERT(label);
        if ( toggled ) {
            label->setFontColor(toggleColor);
        } else {
            label->setFontColor(normalColor);
        }
        if ( hoovered ) {
            if ( pushed ) {
                label->setFontColor( pushedColor );
            } else {
                label->setFontColor( hooverColor );
            }
        }
    }
};


typedef Buttonized< osgWidget::Widget > Button;


//
///**
// *	
// */
//class Button : public osgWidget::Widget
//{
//private:
//  //! Kolor normalny.
//  osgWidget::Color normalColor;
//  //! Kolor przy najechaniu.
//  osgWidget::Color hooverColor;
//  //! Kolor podczas klikniêcia.
//  osgWidget::Color pushedColor;
//  //! Kolor podczas klikniêcia.
//  osgWidget::Color toggleColor;
//
//  //! Czy jest wciœniêty?
//  bool pushed;
//  //! Czy jest przeci¹gany?
//  bool dragged;
//  //! Czy mysz jest nad widgetem?
//  bool hovered;
//  //! Czy mo¿na wciskaæ?
//  bool toggleEnabled;
//  //! Czy wciœniêty?
//  bool toggled;
//
//
//public:
//  META_Object(osgUI, Button);
//
//  //! 
//  //! \param name
//  //! \param color
//  //! \param width
//  //! \param height
//  Button(const std::string & name = "", 
//    double width = 0.0, double height = 0.0);
//  //! 
//  //! \param button
//  //! \param copyop
//  Button(const Button& button, const osg::CopyOp& copyop);
//
//  //! 
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mousePush(double x, double y, const osgWidget::WindowManager* manager);
//  //! 
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* manager);
//  //! 
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseEnter(double x, double y, const osgWidget::WindowManager* manager);
//  //! 
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* manager);
//  //! 
//  //! \param x
//  //! \param y
//  //! \param manager
//  virtual bool mouseDrag(double x, double y, const osgWidget::WindowManager* manager);
//
//  //! \return
//  osgWidget::Color getHooverColor() const
//  { 
//    return hooverColor;
//  }
//  //! \param hooverColor
//  void setHooverColor(osgWidget::Color hooverColor) 
//  { 
//    this->hooverColor = hooverColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getPushedColor() const
//  { 
//    return pushedColor;
//  }
//  //! \param clickedColor
//  void setPushedColor(osgWidget::Color clickedColor) 
//  { 
//    this->pushedColor = clickedColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getNormalColor() const
//  { 
//    return normalColor;
//  }
//  //! \param normalColor
//  void setNormalColor(osgWidget::Color normalColor) 
//  { 
//    this->normalColor = normalColor;
//    refresh();
//  }
//  //! \return
//  osgWidget::Color getToggleColor() const
//  { 
//      return toggleColor;
//  }
//  //! \param toggleColor
//  void setToggleColor(osgWidget::Color toggleColor) 
//  { 
//      this->toggleColor = toggleColor;
//      refresh();
//  }
//
//  //! \return
//  bool isPushed() const
//  { 
//    return pushed;
//  }
//  //! \return
//  bool isDragged() const
//  { 
//    return dragged;
//  }
//  //! \return
//  bool isHovered() const
//  { 
//    return hovered;
//  }
//
//  //! \return
//  bool isToggleEnabled() const
//  { 
//      return toggleEnabled;
//  }
//  //! \param checkable
//  void setToggleEnabled(bool triggerEnabled) 
//  { 
//      this->toggleEnabled = triggerEnabled;
//  }
//
//  //! \return
//  bool isToggled() const
//  { 
//      return toggled;
//  }
//  //! \param triggered
//  void setToggled(bool triggered) 
//  { 
//      this->toggled = triggered;
//      refresh();
//  }
//
//
//public:
//    virtual void parented(osgWidget::Window* window);
//    virtual void unparented(osgWidget::Window* window);
//    virtual void positioned();
//
//protected:
//    //! Odœwie¿a wygl¹d przycisku.
//    void refresh();
//    //! Wywo³ywane w pierwszej kolejnoœci.
//    virtual void applyState(bool triggered);
//    //! Wywo³ywane w drugiej kolejnoœci.
//    virtual void applyHoover(bool pushed);
//};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__BUTTON_H__