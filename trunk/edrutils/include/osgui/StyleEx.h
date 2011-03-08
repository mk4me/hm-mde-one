#ifndef __HEADER_GUARD__EXTSTYLE_H__
#define __HEADER_GUARD__EXTSTYLE_H__

#include <osgWidget/StyleManager>
#include <osgText/Text>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class IBorderized{
public:

	virtual void setBorderColor(const osgWidget::Color & borderColor) {};
	virtual osgWidget::Color getBorderColor() const { return osgWidget::Color(); };

	virtual void setBorderWidth(float thickness) {};
	virtual float getBorderWidth() const { return 0; };
};

//class IBorderized{
//public:
//
//	osgWidget::Color normalColor;
//	//! Kolor przy najechaniu.
//	osgWidget::Color hooverColor;
//	//! Kolor podczas klikni�cia.
//	osgWidget::Color pushedColor;
//	//! Kolor podczas klikni�cia.
//	osgWidget::Color toggleColor;
//
//	virtual void setNormalColor(const osgWidget::Color & normalColor) {};
//	virtual osgWidget::Color getNormalColor() const { return osgWidget::Color(); };
//
//	virtual void setHooverColor(const osgWidget::Color & normalColor) {};
//	virtual osgWidget::Color getHooColor() const { return osgWidget::Color(); };
//
//	virtual void setBorderWidth(float thickness) = 0;
//	virtual float getBorderWidth() const = 0;
//};

class StyleEx : public osgWidget::Style
{
public:
	META_Object(osgWidget, StyleEx);

	// Class and contents...
	StyleEx (const std::string& styleName = "", const std::string& styleDef = "") : osgWidget::Style(styleName, styleDef) {};
	StyleEx (const Style& style, const osg::CopyOp& co) : osgWidget::Style(style, co){};

	virtual bool applyStyle(osgWidget::Widget* w, osgWidget::Reader r);
	virtual bool applyStyle(osgWidget::Label* label, osgWidget::Reader r);
	virtual bool applyStyle(osgWidget::Input* input, osgWidget::Reader r);
	virtual bool applyStyle (osgWidget::Window* window, osgWidget::Reader r);

protected:

	typedef enum {VERTICAL_FIT, HORIZONTAL_FIT, ALL_FIT, NONE_FIT} LABEL_FONT_FIT_MODE;

protected:

	static LABEL_FONT_FIT_MODE strToFontFit(const std::string & fontFit);
	static osgText::Text::AlignmentType strToFontAligment(const std::string & fontAlign);

	virtual bool handleExWidgets(osgWidget::Widget* w, osgWidget::Reader r);
	virtual bool handleExWindows(osgWidget::Window* window, osgWidget::Reader r);
};

}

#endif

