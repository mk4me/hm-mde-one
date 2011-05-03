#ifndef HEADER_GUARD__STYLEEX_H__
#define HEADER_GUARD__STYLEEX_H__

#include <osgWidget/StyleManager>
#include <osgText/Text>
#include <osgDB/ReadFile>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

//! Interfejs elementow z ramka. Pozwala zarzadzac wygladem ramki
class IBorderized{
public:

    //! osgWidget::Color - nowy kolor ramki
	virtual void setBorderColor(const osgWidget::Color & borderColor) {};

    //! zwraca aktualny kolor ramki
	virtual osgWidget::Color getBorderColor() const { return osgWidget::Color(); };

    //! float - nowa grubosc ramki
	virtual void setBorderWidth(float thickness) {};

    //! zwraca aktualna grubosc ramki
	virtual float getBorderWidth() const { return 0; };
};

//! Klasa rozszerzajaca mozliwosci styli w OSG o wlasne komponenty i interfejsy GUI
class StyleEx : public osgWidget::Style
{

protected:

    //! typ wyliczeniowy opisujacy sposob dopasowania wielkosci Labelki do tekstu
    //! domyslnie po zmianach w tekscie Labelka ulega tylko rozszerzeniu, tutaj mozna ja zmniejszac
    typedef enum {VerticalFit, HorizontalFit, AllFit, NoFit} LabelFontFitMode;

private:
    class CustomStyleOptions : public osgDB::Options
    {
    public:
        CustomStyleOptions() : osgDB::Options() {}

        CustomStyleOptions(const std::string& str)
            : osgDB::Options(str) {}

        CustomStyleOptions(const osgDB::Options& options,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            : osgDB::Options(options, copyop) {}


        META_Object(osgui,CustomStyleOptions);

        ~CustomStyleOptions() {}
    };

public:
	META_Object(osgWidget, StyleEx);

	//! std::string - nazwa stylu, powinna byc unikalna w StyleManager
    //! std::string - definicja stylu w ooparciu o mozliwosci klasyStyle i rozszerzen klasy StyleEx
	StyleEx (const std::string& styleName = "", const std::string& styleDef = "") : osgWidget::Style(styleName, styleDef) {
        static bool initialized = false;
        if(initialized == false){
            initialized = true;
            //customStyleOptions = new CustomStyleOptions(*osgDB::Registry::instance()->getOptions());
            customStyleOptions->setObjectCacheHint(osgDB::Options::CACHE_IMAGES);
        }

    };

	StyleEx (const Style& style, const osg::CopyOp& co) : osgWidget::Style(style, co){};

	virtual bool applyStyle(osgWidget::Widget* w, osgWidget::Reader r);
	virtual bool applyStyle(osgWidget::Label* label, osgWidget::Reader r);
	virtual bool applyStyle(osgWidget::Input* input, osgWidget::Reader r);
	virtual bool applyStyle (osgWidget::Window* window, osgWidget::Reader r);

protected:

    //! std::string - wyodrebniony z definicji stylu ciag znakow opisujacy sposob dopasowania Labelki do tekstu
	static LabelFontFitMode strToFontFit(const std::string & fontFit);

    //! std::string - wyodrebniony z definicji stylu ciag znakow opisujacy sposob wyrownania tekstu w Labelce
	static osgText::Text::AlignmentType strToFontAligment(const std::string & fontAlign);

    //! funkcja obslugujaca nowe elementy styli i nowe interfejsy GUI w OSG dla Widgetow
	virtual bool handleExWidgets(osgWidget::Widget* w, osgWidget::Reader r);

    //! funkcja obslugujaca nowe elementy styli i nowe interfejsy GUI w OSG dla Windows
	virtual bool handleExWindows(osgWidget::Window* window, osgWidget::Reader r);

private:

    static osg::ref_ptr<CustomStyleOptions> customStyleOptions;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__STYLEEX_H__

