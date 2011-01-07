/********************************************************************
	created:  2010/11/24
	created:  24:11:2010   13:21
	filename: MultiView.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__CORE_MULTIVIEW_H__
#define __HEADER_GUARD__CORE_MULTIVIEW_H__

#include <vector>
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgGA/GUIEventHandler>
#include <core/Grid.h>
#include <core/AspectRatioKeeper.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class MultiView : public osgWidget::WindowManager
{
public:
    //! Wbudowane style miniaturek.
    enum Templates {
        TemplatesLabelBased,
        TemplatesLabeledViewBased
    };

    //! Adapter widgetów. Pozwala na wykonania abstrakcyjnych czynnoœci na
    //! widgetach nie znaj¹c przy tym ich typów.
    class WidgetAdapter : public osg::Referenced
    {
    public:
        //!
        virtual ~WidgetAdapter() {}
        //! Klonowanie. Trzeba pamiêtaæ, ¿e klasa ta u¿ywana jest jako prototyp i w zwi¹zku
        //! przed przypisaniem do itema trzeba go skopiowaæ.
        virtual WidgetAdapter* clone() = 0;
        //! \param widget
        virtual osgWidget::EventInterface* getEventSource(osgWidget::Widget* widget) = 0;
        //! \param widget
        //! \param label
        virtual void setLabel(osgWidget::Widget* widget, const std::string& label) {};
        //! \param widget
        //! \param toggle
        virtual void setToggle(osgWidget::Widget* widget, bool toggle) {};
        //!
        virtual osgWidget::point_type getMargin(osgWidget::Widget* widget) { return 0; }
    };

    //! Item który mo¿na umieœciæ na p³aszczyŸnie.
    class Item : public osg::Referenced
    {
    public:
        //! \return Etykieta skojarzona z itemem.
        virtual const std::string& getName() const = 0;

        //! \param x Koordynat x na p³aszczyŸnie.
        //! \param y Koordynat y na p³aszczyŸnie.
        //! \param w Szerokoœæ na p³aszczyŸnie.
        //! \param h Wysokoœæ na p³aszczyŸnie.
        virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
            osgWidget::point_type w, osgWidget::point_type h) = 0;
        //! \param position Pozycja na p³aszcznie.
        //! \param size Rozmiar na p³aszczyŸnie.
        void setLocation(osgWidget::XYCoord position, osgWidget::XYCoord size)
        {
            setLocation(position.x(), position.y(), size.x(), size.y());
        }
        //! \param location Wymiary na p³aszczyŸnie.
        void setLocation(osgWidget::Quad location)
        {
            setLocation(location.x(), location.y(), location.z(), location.w());
        }

        //! \return Wspó³czynnik proporcji.
        virtual osgWidget::point_type getAspectRatio() = 0;

        //! \param visible Czy item jest widoczny?
        virtual void setVisible(bool visible) = 0;
    };

private:
    //! WskaŸnik na item.
    typedef osg::ref_ptr<Item> ItemPtr;

    //! Wpis wewnêtrznie u¿ywany.
    struct Entry
    {
        //! Item (miniaturka).
        ItemPtr thumbnail;
        //! Preview (du¿y widok).
        ItemPtr preview;
        //! Widget.
        osg::ref_ptr<osgWidget::Widget> widget;
        //! Adapter widgeta.
        osg::ref_ptr<WidgetAdapter> adapter;
        //! Czy thumbnail jest zaznaczony?
        bool toggled;
        //! Okienko s³u¿¹ce do trzymania wspó³czynnika proporcji.
        osg::ref_ptr<osgUI::AspectRatioKeeper> keeper;
    };

    //! W³aœciwa lista itemów.
    typedef std::vector< Entry > Items;

    //! Lista itemów.
    Items items;
    //! Grid z miniaturkami.
    osg::ref_ptr<osgUI::Grid> thumbnails;
    //! Okienko z guzikami.
    osg::ref_ptr<osgWidget::Box> buttons;
    //! Szerokoœæ (b¹dŸ wysokoœæ) panelu z miniaturkami.
    osgWidget::point_type thumbnailsPaneWidth;

    //! Czy widoczny jest podgl¹d?
    bool previewVisible;

    //! Bie¿¹cy thumbnail
    ItemPtr selectedItem;

    //! Szablon dla miniaturek.
    osg::ref_ptr<osgWidget::Widget> thumbnailTemplate;
    //! Szablon adaptera dla miniaturek.
    osg::ref_ptr<WidgetAdapter> adapterTemplate;

    //!
    ItemPtr prevSelectedItem;
    //! Wspó³rzêdne lewego dolnego rogu.
    //osgWidget::XYCoord origin;
    //! Rozmiar multiwidoku;
    //osgWidget::XYCoord size;

public:
    //! \param view
    //! \param width
    //! \param height
    //! \param mask
    //! \param flags
    MultiView(osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags);

    //! Dodaje item na koniec kolekcji.
    //! \param item Item do dodania. Miniaturka.
    //! \param preview Opcjonalny item, który bêdzie pokazywany/chowany, gdy miniaturka zostanie wybrana.
    bool addItem(Item* thumbnail, Item* preview = NULL);

    //! Usuwa item z kolekcji.
    void removeItem(Item* item);
    //! Usuwa wszystkie itemy.
    void removeAllItems();

    void restoreRequiredChildren();

    //! \return Wybrany element.
    std::pair<Item*, Item*> getSelected();
    //! \return Wybrany element.
    std::pair<const Item*, const Item*> getSelected() const;

    //! \param item Wybrana miniaturka.
    void setSelected(Item* item);
    //! \return Indeks wybranego itema.
    unsigned getSelectedIndex() const;
    //! \param idx Indeks wybranego itema.
    void setSelectedByIndex(unsigned idx);
    //! \return Liczba itemów.
    unsigned getNumItems() const;

    //! \return Prototyp 
    const osgWidget::Widget* getThumbnailTemplate() const
    { 
        return thumbnailTemplate;
    }
    //! \return
    osgWidget::Widget* getThumbnailTemplate()
    { 
        return thumbnailTemplate;
    }
    //! \param thumbnail
    //! \param setLabelFunc
    //! \param setToggleFunc
    void setThumbnailTemplate(osgWidget::Widget* thumbnail, WidgetAdapter* adapter)
    {
        this->thumbnailTemplate = thumbnail;
        this->adapterTemplate = adapter;
    }

    //! \param templ
    void setThumbnailBuitinTemplate(Templates templ = TemplatesLabelBased);

    //! \param thumbnailsPaneWidth
    void setThumbnailsPaneWidth(osgWidget::point_type thumbnailsPaneWidth);
    //! Odœwie¿a roz³o¿enie elementów.
    void refreshLayout();

    //! \param visible Czy miniaturki maj¹ byæ widoczne?
    void setThumbnailsVisible(bool visible);
    //! Byk ortograficzny zamierzony (powinno byæ: areThumbnailsVisible).
    //! \return Czy miniaturki maj¹ byæ widoczne?
    bool isThumbnailsVisible() const;

    //! \param visible Czy podgl¹d jest widoczny?
    void setPreviewVisible(bool visible);
    //! \return Czy podgl¹d jest widoczny?
    bool isPreviewVisible() const;

private:

    void setSelectedByEntry(Entry* selected);

    //! Callback wywo³ywany gdy wybierze siê jakiœ item.
    bool onItemClicked(osgWidget::Event& ev);
    //! Callback wywo³ywany gdy kliknie siê w przycisk mini/maksymalizacji.
    bool onShowHideButtonClicked(osgWidget::Event& ev);

    void setEntrySelected( Entry &entry, bool selected );

    //! Pomocnicza funkcja wyszukuj¹ca.
    Items::iterator getIterator(const Item* item);
    //! Pomocnicza funkcja wyszukuj¹ca.
    Items::iterator getIterator(const osgWidget::Widget* widget);
    //! Pomocnicza funkcja wyszukuj¹ca.
    Items::const_iterator getIterator(const Item* item) const;
    //! Pomocnicza funkcja wyszukuj¹ca.
    Items::const_iterator getIterator(const osgWidget::Widget* widget) const;

    //! Pomocnicza funkcja sprawdzaj¹ca zakres iteratora.
    Items::iterator& checked(Items::iterator& it);
    //! Pomocnicza funkcja sprawdzaj¹ca zakres iteratora.
    Items::const_iterator& checked(Items::const_iterator& it) const;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_MULTIVIEW_H__