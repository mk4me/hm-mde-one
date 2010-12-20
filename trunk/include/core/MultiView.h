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

    //! Adapter widget�w. Pozwala na wykonania abstrakcyjnych czynno�ci na
    //! widgetach nie znaj�c przy tym ich typ�w.
    class WidgetAdapter : public osg::Referenced
    {
    public:
        //!
        virtual ~WidgetAdapter() {}
        //! Klonowanie. Trzeba pami�ta�, �e klasa ta u�ywana jest jako prototyp i w zwi�zku
        //! przed przypisaniem do itema trzeba go skopiowa�.
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

    //! Item kt�ry mo�na umie�ci� na p�aszczy�nie.
    class Item : public osg::Referenced
    {
    public:
        //! \return Etykieta skojarzona z itemem.
        virtual const std::string& getName() const = 0;

        //! \param x Koordynat x na p�aszczy�nie.
        //! \param y Koordynat y na p�aszczy�nie.
        //! \param w Szeroko�� na p�aszczy�nie.
        //! \param h Wysoko�� na p�aszczy�nie.
        virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
            osgWidget::point_type w, osgWidget::point_type h) = 0;
        //! \param position Pozycja na p�aszcznie.
        //! \param size Rozmiar na p�aszczy�nie.
        void setLocation(osgWidget::XYCoord position, osgWidget::XYCoord size)
        {
            setLocation(position.x(), position.y(), size.x(), size.y());
        }
        //! \param location Wymiary na p�aszczy�nie.
        void setLocation(osgWidget::Quad location)
        {
            setLocation(location.x(), location.y(), location.z(), location.w());
        }

        //! \return Wsp�czynnik proporcji.
        virtual osgWidget::point_type getAspectRatio() = 0;
    };

    //! Item, kt�ry mo�na dodatkowo ustawi� jako bie��cy.
    class PreviewItem : public Item
    {
    public:
        //! \param selected
        virtual void setSelected(bool selected) = 0;
    };

private:
    //! Wska�nik na item.
    typedef osg::ref_ptr<Item> ItemPtr;
    //! Wska�nik na item.
    typedef osg::ref_ptr<PreviewItem> PreviewItemPtr;

    //! Wpis wewn�trznie u�ywany.
    struct Entry
    {
        //! Item (miniaturka).
        ItemPtr item;
        //! Preview (du�y widok).
        PreviewItemPtr preview;
        //! Widget.
        osg::ref_ptr<osgWidget::Widget> widget;
        //! Adapter widgeta.
        osg::ref_ptr<WidgetAdapter> adapter;
        //! Czy item jest zaznaczony?
        bool toggled;
        //! Okienko s�u��ce do trzymania wsp�czynnika proporcji.
        osg::ref_ptr<osgUI::AspectRatioKeeper> keeper;
    };

    //! W�a�ciwa lista item�w.
    typedef std::vector< Entry > Items;

    //! Lista item�w.
    Items items;
    //! Grid z miniaturkami.
    osg::ref_ptr<osgUI::Grid> thumbnails;
    //! Szeroko�� (b�d� wysoko��) panelu z miniaturkami.
    osgWidget::point_type thumbnailsPaneWidth;
    //! Bie��cy item
    ItemPtr selectedItem;

    //! Szablon dla miniaturek.
    osg::ref_ptr<osgWidget::Widget> thumbnailTemplate;
    //! Szablon adaptera dla miniaturek.
    osg::ref_ptr<WidgetAdapter> adapterTemplate;
    //! Wsp�rz�dne lewego dolnego rogu.
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
    bool addItem(Item* item, PreviewItem* preview = NULL);
    //! Usuwa item z kolekcji.
    void removeItem(Item* item);
    //! Usuwa wszystkie itemy.
    void removeAllItems();

    void restoreRequiredChildren();
    //! \return Wybrany element.
    std::pair<Item*, PreviewItem*> getSelected();
    //! \return Wybrany element.
    std::pair<const Item*, const PreviewItem*> getSelected() const;
    //! \param item Wybrana miniaturka.
    void setSelected(Item* item);
    //! \return Indeks wybranego itema.
    unsigned getSelectedIndex() const;
    //! \param idx Indeks wybranego itema.
    void setSelectedByIndex(unsigned idx);
    //! \return Liczba item�w.
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
    //! Od�wie�a roz�o�enie element�w.
    void refreshLayout();

private:
    //! Callback wywo�ywany gdy wybierze si� jaki� item.
    bool onItemClicked(osgWidget::Event& ev);

    void setEntrySelected( Entry &entry, bool selected );
    //! \param item
    Items::iterator getIterator(const Item* item);
    //! \param item
    Items::iterator getIterator(const osgWidget::Widget* widget);
    //! \param item
    Items::const_iterator getIterator(const Item* item) const;
    //! \param item
    Items::const_iterator getIterator(const osgWidget::Widget* widget) const;

    Items::iterator& checked(Items::iterator& it);
    Items::const_iterator& checked(Items::const_iterator& it) const;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_MULTIVIEW_H__