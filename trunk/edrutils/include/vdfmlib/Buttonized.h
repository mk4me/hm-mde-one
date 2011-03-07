/********************************************************************
        created:  2010/06/10
        created:  10:6:2010   10:36
        filename: Button.h
        author:   Piotr Gwiazdowski
        
        purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__BUTTON_H__
#define __HEADER_GUARD__BUTTON_H__

#include <osgWidget/Widget>
#include <osgWidget/Label>
#include <boost/type_traits.hpp>
#include <boost/assert.hpp>
#include <vdfmlib/macroHacks.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

template <class Base, class StylePolicy = DefaultStylePolicy>
class Buttonized : public Base, public StylePolicy
{
    BOOST_STATIC_ASSERT((boost::is_base_of<osgWidget::Widget, Base>::value));

public:
    META_Widget(osgUI, Buttonized);

private:
    //! Czy jest wci�ni�ty?
    bool pushed;
    //! Czy mysz jest nad widgetem?
    bool hovered;
    //! Czy mo�na wciska�?
    bool toggleEnabled;
    //! Czy wci�ni�ty?
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
    //! Od�wie�a wygl�d przycisku.
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
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
    {}
};

class DefaultStylePolicy
{
private:
    //! Kolor normalny.
    osgWidget::Color normalColor;
    //! Kolor przy najechaniu.
    osgWidget::Color hoverColor;
    //! Kolor podczas klikni�cia.
    osgWidget::Color pushedColor;
    //! Kolor podczas klikni�cia.
    osgWidget::Color toggleColor;
public:
    DefaultStylePolicy()
    {}
    DefaultStylePolicy(const DefaultStylePolicy& policy, const osg::CopyOp& copyop) :
        normalColor(policy.normalColor),
        hoverColor(policy.hoverColor),
        pushedColor(policy.pushedColor),
        toggleColor(policy.toggleColor)
    {}

		virtual ~DefaultStylePolicy(){}

public:
    //! \param hooverColor
    void setAllColors(osgWidget::Color color) 
    { 
        normalColor = hoverColor = pushedColor = toggleColor = color;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getHoverColor() const
    { 
        return hoverColor;
    }
    //! \param hooverColor
    void setHoverColor(osgWidget::Color hoverColor) 
    { 
        this->hoverColor = hoverColor;
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

    //! Metoda do implementacji przez klas� pochodn�. Powinna wywo�ywa� metod� applyStyle.
    virtual void onStyleChanged() = 0;
    //! 
    //! \param widget
    //! \param toggled
    //! \param pushed
    //! \param hoovered
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
    {
        if ( toggled ) {
            widget->setColor(toggleColor);
        } else {
            widget->setColor(normalColor);
        }

        if ( hovered ) {
            osgWidget::Color src = pushed ? pushedColor : hoverColor;
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
    osgWidget::Color hoverColor;
    //! Kolor podczas klikni�cia.
    osgWidget::Color pushedColor;
    //! Kolor podczas klikni�cia.
    osgWidget::Color toggleColor;
public:
    LabelStylePolicy()
    {}
    LabelStylePolicy(const LabelStylePolicy& policy, const osg::CopyOp& copyop) :
    DefaultStylePolicy(policy),
    normalColor(policy.normalColor),
    hoverColor(policy.hoverColor),
    pushedColor(policy.pushedColor),
    toggleColor(policy.toggleColor)
    {}

	virtual ~LabelStylePolicy(){}

public:
    //! \param hooverColor
    void setLabelAllColors(osgWidget::Color color) 
    { 
        normalColor = hoverColor = pushedColor = toggleColor = color;
        onStyleChanged();
    }
    //! \return
    osgWidget::Color getLabelHoverColor() const
    { 
        return hoverColor;
    }
    //! \param hooverColor
    void setLabelHoverColor(osgWidget::Color hoverColor) 
    { 
        this->hoverColor = hoverColor;
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

    //! Metoda do implementacji przez klas� pochodn�. Powinna wywo�ywa� metod� applyStyle.
    virtual void onStyleChanged() = 0;
    //! 
    //! \param widget
    //! \param toggled
    //! \param pushed
    //! \param hoovered
    void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hovered)
    {
        DefaultStylePolicy::applyStyle(widget, toggled, pushed, hovered);
        osgWidget::Label* label = dynamic_cast<osgWidget::Label*>(widget);
        BOOST_ASSERT(label);
        if ( toggled ) {
            label->setFontColor(toggleColor);
        } else {
            label->setFontColor(normalColor);
        }
        if ( hovered ) {
            if ( pushed ) {
                label->setFontColor( pushedColor );
            } else {
                label->setFontColor( hoverColor );
            }
        }
    }
};


class TextStylePolicy
{
private:
	//! normalny styl
	std::string normalStyle;
	//! hoovered styl
	std::string hoverStyle;
	//! pushed styl
	std::string pushedStyle;
	//! toggle styl
	std::string toggleStyle;

public:
	TextStylePolicy()
	{}
	TextStylePolicy(const TextStylePolicy& policy, const osg::CopyOp& copyop) :
	normalStyle(policy.normalStyle),
		hoverStyle(policy.hoverStyle),
		pushedStyle(policy.pushedStyle),
		toggleStyle(policy.toggleStyle)
	{}

	virtual ~TextStylePolicy() {}

public:
	//! \param hooverColor
	void setAllStyles(const std::string & allStyle) 
	{
		normalStyle = hoverStyle = pushedStyle = toggleStyle = allStyle;
		onStyleChanged();
	}
	//! \return
	const std::string & getHoverStyle() const
	{ 
		return hoverStyle;
	}
	//! \param hooverColor
	void setHoverStyle(const std::string & hoverStyle) 
	{	
		this->hoverStyle = hoverStyle;
		onStyleChanged();
	}
	//! \return
	const std::string & getPushedStyle() const
	{ 
		return pushedStyle;
	}
	//! \param clickedColor
	void setPushedStyle(const std::string & pushedStyle) 
	{ 
		this->pushedStyle = pushedStyle;
		onStyleChanged();
	}
	//! \return
	const std::string & getNormalStyle() const
	{ 
		return normalStyle;
	}
	//! \param normalColor
	void setNormalStyle(const std::string &  normalStyle) 
	{ 
		this->normalStyle = normalStyle;
		onStyleChanged();
	}
	//! \return
	const std::string & getToggleColor() const
	{ 
		return toggleStyle;
	}
	//! \param toggleColor
	void setToggleStyle(const std::string &  toggleStyle) 
	{ 
		this->toggleStyle = toggleStyle;
		onStyleChanged();
	}

protected:

	//! Metoda do implementacji przez klas� pochodn�. Powinna wywo�ywa� metod� applyStyle.
	virtual void onStyleChanged() = 0;
	//! 
	//! \param widget
	//! \param toggled
	//! \param pushed
	//! \param hoovered
	void applyStyle(osgWidget::Widget* widget, bool toggled, bool pushed, bool hoovered)
	{
		if ( toggled ) {
			widget->setStyle(toggleStyle);
		} else {
			widget->setStyle(normalStyle);
		}

		if ( hoovered ) {
			widget->setStyle(pushed ? pushedStyle : hoverStyle);
		}

		if(widget->getWindowManager() != 0 && widget->getWindowManager()->getStyleManager() != 0){
			widget->getWindowManager()->getStyleManager()->applyStyles(widget);
		}
	}
};

class IBorderStylePolicy {
public:
	virtual osgWidget::Color getBorderNormalColor() const { return osgWidget::Color(); };
	virtual void setBorderNormalColor(osgWidget::Color borderNormalColor) {};
	virtual osgWidget::Color getBorderHooverColor() const { return osgWidget::Color(); };
	virtual void setBorderHooverColor(osgWidget::Color borderHooverColor) {};
	virtual osgWidget::Color getBorderPushedColor() const { return osgWidget::Color(); };
	virtual void setBorderPushedColor(osgWidget::Color borderPushedColor) {};
	virtual osgWidget::Color getBorderToggleColor() const { return osgWidget::Color(); };
	virtual void setBorderToggleColor(osgWidget::Color borderToggleColor) {}
	virtual void setBorderAllColors(osgWidget::Color color) {
		setBorderNormalColor(color);
		setBorderHooverColor(color);
		setBorderPushedColor(color);
		setBorderToggleColor(color);
	};

};

template <class Base>
class BorderStylePolicy : public Base, public IBorderStylePolicy
{
private:
	//! Kolor normalny.
	osgWidget::Color borderNormalColor;
	//! Kolor przy najechaniu.
	osgWidget::Color borderHooverColor;
	//! Kolor podczas kliknięcia.
	osgWidget::Color borderPushedColor;
	//! Kolor podczas kliknięcia.
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

}
#endif