#include "CorePCH.h"
#include <stdexcept>
#include <core/OsgWidgetGridView.h>
#include <core/LabeledView.h>
#undef min
#undef max

#include <boost/shared_ptr.hpp>
#include <core/Placeable.h>
#include <boost/bind.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void func(float x, float y, float w, float h)
{
    
}

float foo()
{
    return 666.0f;
}

osgWidget::Widget* bar()
{
    throw std::runtime_error("");
}

template<class T> osg::ref_ptr<T> refptr_from( osg::observer_ptr<T> & wpt )
{
    return osg::ref_ptr<T>( wpt ); // throws on wpt.expired()
} 

struct OsgCallbackBase2 : public osgUI::OsgCallbackBase
{
    osg::observer_ptr<osgWidget::Widget> widget;
    
    OsgCallbackBase2(osgWidget::Widget* widget = NULL) : osgUI::OsgCallbackBase(widget), widget(widget) {}
    OsgCallbackBase2(const OsgCallbackBase2& callback) : osgUI::OsgCallbackBase(callback), widget(callback.widget) {}


    void operator()(float x, float y, float w, float h)
    {

    }
};

OsgWidgetGrid::OsgWidgetGrid( const std::string& name ) 
:   osgUI::LabeledView<>(name)
{

    //osg::Referenced

    //osgWidget::Widget

//     typedef boost::function<void (float, float, float, float)> Callback;
//     typedef osgUI::Placeable< osgWidget::Widget, OsgCallbackBase2 > Placeable;
// 
//     osgWidget::Widget* dest = new osgWidget::Widget("dupa");
//     osg::ref_ptr<osgWidget::Widget> ptr = dest;
// 
//     Placeable * pb = new Placeable("bleee");
//     pb->setPlacementCallback( OsgCallbackBase2(dest) );
// 
//     pb->positioned();

//     
//     pb->setPlaceableCallback(   
//         boost::bind( &osgWidget::Widget::setDimensions,
//         boost::bind( &osgUI::observer_ptr_to_ref_ptr<osgWidget::Widget>, osg::observer_ptr<osgWidget::Widget>(dest)),
//         _1,
//         _2,
//         _3,
//         _4,
//         10.0f)
//     );
//     pb->setDimensions(0,0, 10, 10);
//     pb->positioned();


// 
//     Placeable* placeable = new Placeable ();
// 
//     if ( cb ) {
//         cb();
//     }
//     cb = foo;
//     float f = cb();
// 
//     
// 
//     osgWidget::Widget* test = new osgWidget::Widget("TEST");
//     osg::ref_ptr<osgWidget::Widget> tptr = test;
//     osg::observer_ptr<osgWidget::Widget> wptr = tptr;
//     cb = boost::bind( &osgWidget::Widget::getX, boost::bind(refptr_from<osgWidget::Widget>, wptr) );
//     f = cb();

//     typedef osgUI::Placeable<osgWidget::Widget, Callback> Placeable;
//     Placeable blee("dupa");
//     blee.setCallback( Callback(func) );
//     

    model.setAspectRatio(1.8);
    model.setNumItems(0);

    getBackground()->setColor(0,0,0,0);

    getOverlay()->setBorderWidth(2);
    getOverlay()->setBorderAllColors(osgWidget::Color(1,1,1,1));

    getLabel()->setLabel( name );
    getLabel()->setEventMask( osgWidget::EVENT_MASK_MOUSE_DRAG );
    getLabel()->addCallback( new osgWidget::Callback(&OsgWidgetGrid::onDrag, this, osgWidget::EVENT_MOUSE_DRAG) );
}

void OsgWidgetGrid::setSeletedItem( Item* item )
{
    ItemPtr ptr = ItemPtr(item);
    if ( getIterator(ptr) == items.end() ) {
        throw std::runtime_error("Item not added.");
    } else {
        if ( selectedItem != item ) {
            selectedItem = item;
            if ( ItemSelectedPtr locked = itemSelected.lock() ) {
                (*locked)(item);
            }
        }
    }
}

void OsgWidgetGrid::addItem( Item* item )
{
    ItemPtr ptr = ItemPtr(item);
    if ( getIterator(ptr) == items.end() ) {
        // dodanie itema
        items.push_back(ptr);

        typedef osgUI::Buttonized< osgUI::Borderized<osgWidget::Widget>, osgUI::BorderStylePolicy<osgUI::DefaultStylePolicy> > Overlay;
        typedef boost::function< void(osgWidget::Widget*, float, float, float, float) > Callback;
        typedef osgUI::Placeable< Overlay, Callback > PlaceableOverlay;

        // tworzenie okna
        osgUI::LabeledView<PlaceableOverlay>* view = new osgUI::LabeledView<PlaceableOverlay>("Item");
        view->getBackground()->setColor(0,0,0,0);
        view->setEventMask( 0 );
        view->getOverlay()->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
        view->getLabel()->setLabel( item->getName() );
        view->getLabel()->setColor(0.5,0.5,0.5,0.7);
        view->getOverlay()->setPlacementCallback( boost::bind( 
            &Item::setLocation,
            boost::bind( &osgUI::observer_ptr_to_ref_ptr<Item>, osg::observer_ptr<Item>(item)),
            _2, _3, _4, _5
            )
        );



      
        // 
        osgWidget::Window::EmbeddedWindow* widget = view->embed();
        widget->setColor(0,0,0,0);
        borders.push_back(widget);
        
       
        addWidget(widget);

        model.setNumItems( model.getNumItems() +1 );
    } else {
        throw std::runtime_error("Item already added.");
    }
}

void OsgWidgetGrid::removeItem( Item* item )
{
    ItemPtr ptr = ItemPtr(item);
    Items::iterator found = getIterator(ptr);
    if ( found == items.end() ) {
        throw std::runtime_error("Item not added.");
    } else {
        // usuniêcie itema oraz ramki
        Items::difference_type i = std::distance(items.begin(), found);
        items.erase(found);
        borders.erase( borders.begin() + i );
        model.setNumItems( model.getNumItems() - 1);
    }
}

void OsgWidgetGrid::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
    osgUI::LabeledView<>::_resizeImplementation(diffWidth, diffHeight);

    float width = getWidth() + diffWidth;
    float height = getHeight() + diffHeight;
   

    float margin = getOverlay()->getBorderWidth();
    width -= 2*margin;
    height -= 2*margin + (isLabelVisible() ? getLabel()->getHeight() : 0);

    model.setSize(width, height);

    for ( Items::size_type i = 0; i < items.size(); ++i ) {
        GridModel::Item location = model.getItem( static_cast<int>(i) );
        //items[i]->setLocation(margin+location.x, margin+location.y, location.width, location.height);
        borders[i]->setDimensions(margin+location.x, margin+location.y, location.width, location.height);
    }
}

// osgWidget::Window::Sizes OsgWidgetGrid::_getWidthImplementation() const
// {
//     osgUI::LabeledView* view = dynamic_cast<osgUI::LabeledView*>(frame->getWindow());
//     return osgWidget::Window::Sizes(
//         view->getWidth(),
//         view->getOverlay()->getBorderWidth() * 2
//         );
// }
// 
// osgWidget::Window::Sizes OsgWidgetGrid::_getHeightImplementation() const
// {
//     osgUI::LabeledView* view = dynamic_cast<osgUI::LabeledView*>(frame->getWindow());
//     return osgWidget::Window::Sizes(
//         view->getHeight(),
//         view->getOverlay()->getBorderWidth() * 2 + (view->isLabelVisible() ? view->getLabel()->getHeight() : 0)
//         );
// }

void OsgWidgetGrid::removeAllItems()
{
    model.setNumItems(0);
    items.clear();
    borders.clear();
}

bool OsgWidgetGrid::onDrag( osgWidget::Event& ev )
{
    if ( ev.getWindowManager()->isLeftMouseButtonDown() ) {
        addOrigin( ev.x, ev.y );
        if ( getX() < 0 ) {
            setX(0);
        } else if ( getX() + getWidth() > getWindowManager()->getWidth() ) {
            setX( getWindowManager()->getWidth() - getWidth() );
        } 
        if ( getY() < 0 ) {
            setY(0);
        } else if ( getY() + getHeight() > getWindowManager()->getHeight() ) {
            setY( getWindowManager()->getHeight() - getHeight() );
        }
        update();
        return true;
    } else if ( ev.getWindowManager()->isMiddleMouseButtonDown() ) {
        float dx = std::min( getWindowManager()->getWidth() - getX() - getWidth(), ev.x );
        float dy = std::min( getWindowManager()->getHeight() - getY() - getHeight(), ev.y );
        resizeAdd( dx, dy );
        update();
        return true;
    }
    return false;
}
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
