/********************************************************************
	created:  2010/11/08
	created:  8:11:2010   15:12
	filename: OsgWidgetGrid.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__OSGWIDGETGRIDVIEW_H__
#define __HEADER_GUARD__OSGWIDGETGRIDVIEW_H__

#include <vector>

#include <boost/function.hpp>

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osgWidget/Canvas>
#include <osgWidget/Widget>
#include <osg/Image>

#include <core/SmartPtr.h>
#include <core/GridModel.h>
#include <core/OsgWidgetBorder.h>

#include <core/LabeledView.h>
#include <core/Grid.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class OsgWidgetGrid : public osgUI::LabeledView<>
{
public:
    
    class Item : public osg::Referenced
    {
    public:
        //! \return Wspó³czynnik proporcji. Wartoœci <= 0.0 oznaczaj¹ dowolnoœæ.
        virtual double getAspectRatio() = 0;
        //! \param location
        virtual void setLocation(osg::Vec4 location) = 0;
        //! 
        virtual osg::Vec4 getLocation() = 0;
        //! \param x
        //! \param y
        //! \param w
        //! \param h
        virtual void setLocation(float x, float y, float w, float h)
        {
            setLocation(osg::Vec4(x,y,w,h));
        }
        //! \param x
        //! \param y
        //! \param w
        //! \param h
        void getLocation(float& x, float& y, float& w, float& h)
        {
            osg::Vec4 location = getLocation();
            x = location.x();
            y = location.y();
            w = location.z();
            h = location.w();
        }
        //! 
        virtual const std::string& getName() const = 0;
    };

    //! Typ callbacka.
    typedef boost::function1<void, Item*> ItemSelected;
    typedef weak_ptr<ItemSelected> ItemSelectedWeakPtr;
    typedef shared_ptr<ItemSelected> ItemSelectedPtr;

    //! WskaŸnik na item.
    typedef osg::ref_ptr<Item> ItemPtr;
    //! Lista itemów.
    typedef std::vector<ItemPtr> Items;
    //! Iterator.
    typedef Items::iterator iterator;
    //!
    typedef osg::ref_ptr<osgWidget::Window::EmbeddedWindow> BorderPtr;
    //!
    typedef std::vector<BorderPtr> Borders;

    typedef osg::ref_ptr<osgWidget::Window::EmbeddedWindow> FramePtr;

private:
    //!
    Borders borders;
    //! Lista itemów.
    Items items;
    //! Grid z miniaturkami.
    GridModel model;
    //! Wybrany item z miniaturek.
    osg::observer_ptr<Item> selectedItem;
    //! 
    ItemSelectedWeakPtr itemSelected;
    //!
    FramePtr frame;

    osg::ref_ptr<osgUI::Grid> grid;

public: 
    //! \param view
    //! \param width
    //! \param height
    //! \param mask
    //! \param flags
    OsgWidgetGrid(const std::string& name);

    //! 
    iterator begin()
    {
        return items.begin();
    }

    //! 
    iterator end()
    {
        return items.end();
    }

    //! \return
    Item* getSelectedItem()
    { 
        return selectedItem.valid() ? selectedItem.get() : NULL;
    }

    //! \param selectedItem
    void setSeletedItem(Item* item);
    //! \param item
    void addItem(Item* item);
    //! \param item
    void removeItem(Item* item);
    //!
    void removeAllItems();

protected:
    //! Prze³adowanie metody rozmieszczaj¹cej elementy.
    //! \param diffWidth
    //! \param diffHeight
    virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight );
//     //! Prze³adowanie metody obliczaj¹cej rozmiar.
//     virtual osgWidget::Window::Sizes _getWidthImplementation() const;
//     //! Prze³adowanie metody obliczaj¹cej rozmiar.
//     virtual osgWidget::Window::Sizes _getHeightImplementation() const;

    bool onDrag(osgWidget::Event& ev);

private:
    //! \param item
    //! \return Iterator dla wybranego elementu.
    Items::iterator getIterator(const ItemPtr& item)
    {
        return std::find(items.begin(), items.end(), item);
    }

};
// 
// class MultiViewItem : public osg::Referenced
// {
//     //! \param location
//     virtual void setLocation(osg::Vec4 location) = 0;
//     //! \param x
//     //! \param y
//     //! \param w
//     //! \param h
//     virtual void setLocation(float x, float y, float w, float h)
//     {
//         setLocation(osg::Vec4(x,y,w,h));
//     }
// 
//     //! 
//     virtual osg::Vec4 getLocation() = 0;
//     //! \param x
//     //! \param y
//     //! \param w
//     //! \param h
//     void getLocation(float& x, float& y, float& w, float& h)
//     {
//         osg::Vec4d location = getLocation();
//         x = location.x();
//         y = location.y();
//         w = location.z();
//         h = location.w();
//     }
// };
// 
// class CameraMultiViewItem : public MultiViewItem
// {
// private:
//     osg::ref_ptr<osg::Camera> camera;
// 
// public:
//     //! \param location
//     virtual void setLocation(osg::Vec4 location)
//     {
//         camera->setViewport(
//             static_cast<int>(location.x()),
//             static_cast<int>(location.y()),
//             static_cast<int>(location.z()),
//             static_cast<int>(location.w())
//         );
//     }
//     //! 
//     virtual osg::Vec4 getLocation()
//     {
//         return osg::Vec4(
//             camera->getViewport()->x(),
//             camera->getViewport()->y(), 
//             camera->getViewport()->width(), 
//             camera->getViewport()->height());
//     }
// };
// 

class WidgetMultiViewItem : public OsgWidgetGrid::Item
{
protected:
    osg::ref_ptr<osgWidget::Widget> widget;

public:

    //! 
    //! \param widget
    WidgetMultiViewItem(osgWidget::Widget* widget) : widget(widget)
    {}

    //! \param location
    virtual void setLocation(osg::Vec4 location)
    {
        widget->setOrigin(location.x(), location.y());
        widget->setSize(location.z(), location.w());
    }
    
    //! 
    virtual osg::Vec4 getLocation()
    {
        osgWidget::XYCoord pos = widget->getOrigin();
        osgWidget::XYCoord size = widget->getSize();
        return osg::Vec4(pos.x(), pos.y(), size.x(), size.y());
    }

    //!
    virtual double getAspectRatio()
    {
        osg::StateSet* ss = widget->getStateSet();
        if (ss) {
            osg::Texture * texture = dynamic_cast<osg::Texture*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
            if ( texture ) {
                osg::Image* image = texture->getImage(0);
                return image->s() * image->getPixelAspectRatio() / image->t();
            }
        }
        return -1.0;
    }

    virtual const std::string& getName() const
    {
        return widget->getName();
    }
};

class WindowMultiViewItem : public OsgWidgetGrid::Item
{
protected:
    osg::ref_ptr<osgWidget::Window> window;
    osg::ref_ptr<osgWidget::Window> reference;

public:

    //! 
    //! \param widget
    WindowMultiViewItem(osgWidget::Window* window, osgWidget::Window* reference) : window(window), reference(reference)
    {}

    //! \param location
    virtual void setLocation(osg::Vec4 location)
    {
        window->setOrigin(reference->getX() + location.x(), reference->getY() + location.y());
        window->getByName("ITEM")->setSize(location.z(), location.w());
        window->resizeAdd();
    }

    //! 
    virtual osg::Vec4 getLocation()
    {
        return osg::Vec4();
    }

    //!
    virtual double getAspectRatio()
    {
        return 1;
    }

    virtual const std::string& getName() const
    {
        return window->getName();
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__OSGWIDGETGRIDVIEW_H__