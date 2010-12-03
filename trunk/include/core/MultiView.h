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
#include <boost/tuple/tuple.hpp>
#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgGA/GUIEventHandler>
#include <core/Grid.h>
#include <core/Buttonized.h>
#include <core/Borderized.h>
#include <core/LabeledView.h>
#include <core/EmbeddedWindow.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class MultiView : public osgWidget::WindowManager
{
public:

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

    //! Typ overlay'a nad miniaturk�.
    typedef osgUI::Buttonized< osgUI::Borderized<osgWidget::Label>, osgUI::BorderStylePolicy<osgUI::LabelStylePolicy> > OverlayButton;
    //! Typ okienka z miniaturk�.
    typedef osgUI::LabeledView< OverlayButton > OverlayWindow;
    //! Typ miniaturki.
    typedef osgUI::Embedded< OverlayWindow > Overlay;

private:
    //! Wska�nik na item.
    typedef osg::ref_ptr<Item> ItemPtr;
    //! Wska�nik na item.
    typedef osg::ref_ptr<PreviewItem> PreviewItemPtr;

    //! Wpis wewn�trznie u�ywany.
    struct Entry
    {
        ItemPtr item;
        PreviewItemPtr preview;
        osg::ref_ptr<OverlayButton> widget;
    };

    //! W�a�ciwa lista item�w.
    typedef std::vector< Entry > Items;

    //! Lista item�w.
    Items items;
    //! Grid z miniaturkami.
    osg::ref_ptr<osgUI::Grid> thumbnails;
    //! Bie��cy item
    ItemPtr selectedItem;
    //! Szablon dla miniaturek.
    osg::ref_ptr<Overlay> thumbnailTemplate;
    //! Szablon dla podgl�d�w. Na razie nieu�ywany.
    osg::ref_ptr<Overlay> viewTemplate;
    //! Szeroko�� (b�d� wysoko��) panelu z miniaturkami.
    osgWidget::point_type thumbnailsPaneWidth;


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

    //! Od�wie�a roz�o�enie element�w.
    void refreshLayout();

    //! \return Prototyp 
    const Overlay* getThumbnailTemplate() const
    { 
        return thumbnailTemplate;
    }
    //! \return
    Overlay* getThumbnailTemplate()
    { 
        return thumbnailTemplate;
    }
    //! \return Prototyp 
    const Overlay* getViewTemplate() const
    { 
        return viewTemplate;
    }
    //! \return
    Overlay* getViewTemplate()
    { 
        return viewTemplate;
    }
    //! \return
    osgWidget::point_type getThumbnailsPaneWidth() const
    { 
        return thumbnailsPaneWidth;
    }
    //! \param thumbnailsPaneWidth
    void setThumbnailsPaneWidth(osgWidget::point_type thumbnailsPaneWidth);

private:
    //! 
    //! \param item
    Items::iterator getIterator(const Item* item);
    //! \param item
    Items::iterator getIterator(const osgWidget::Widget* widget);

    //! Callback wywo�ywany gdy wybierze si� jaki� item.
    bool onItemClicked(osgWidget::Event& ev);
};


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__CORE_MULTIVIEW_H__