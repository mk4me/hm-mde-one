#include "CorePCH.h"
#include <core/OsgWidgetBorder.h>
#include <core/LabeledView.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

OsgWidgetBorder::OsgWidgetBorder()
{
    // tworzymy labelkê
    label = new osgWidget::Label("label", "NONE");
    label->setColor(osgWidget::Color(0.5, 0.5, 0.5, 0.5));
    label->setFontColor(osgWidget::Color(1, 1, 1, 1));
    label->setAlignHorizontal(osgWidget::Widget::HA_CENTER);
    label->setAlignVertical(osgWidget::Widget::VA_TOP);
    label->setHeight(10);
    label->setLayer(osgWidget::Widget::LAYER_HIGH, 10);


    osgUI::LabeledView<>* view = new osgUI::LabeledView<>("VIEW");
    view->getBackground()->setColor(0,0,0,0);
    view->getLabel()->setColor(1,1,1,0.3);
    view->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
    view->getOverlay()->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
    //view->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
    overlay = view->embed();
    overlay->setColor(0,0,0,0);

//     overlay = new Background("overlay");
//     overlay->setBorderWidth(1);
//     overlay->setHooverColor( osgWidget::Color(1,1,1,0.5) );
//     //overlay->setPushedColor( osgWidget::Color(1,0,0,0.5) );
//     //overlay->setBorderColor( osgWidget::Color(1,1,1,1) );
//     //overlay->setToggleColor( osgWidget::Color(0,1,0,0.5));
//     overlay->setLayer(osgWidget::Widget::LAYER_HIGH);
//     
//     overlay->setToggleEnabled(true);
// 
//     overlay->setBorderNormalColor( osgWidget::Color(1,1,1,1) );
//     overlay->setBorderHooverColor( osgWidget::Color(1,0,0,0.5) );
//     overlay->setBorderPushedColor( osgWidget::Color(1,0,0,1) );
//     overlay->setBorderToggleColor( osgWidget::Color(0,1,0,1) );
    
    
    
    overlay->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
}

void OsgWidgetBorder::setBorderColor( osgWidget::Color color )
{
    //overlay->setBorderColor(color);
}

void OsgWidgetBorder::setBorderColor( osgWidget::color_type r, osgWidget::color_type g, osgWidget::color_type b, osgWidget::color_type a )
{
    setBorderColor(osgWidget::Color(r, g, b, a));
}
osgWidget::Color OsgWidgetBorder::getBorderColor() const
{
    return osgWidget::Color(); //overlay->getBorderColor();
}

void OsgWidgetBorder::setLocation( float x, float y, float width, float height, float thickness /*= 1*/ )
{
    label->setSize( width - 2*thickness, label->getHeight() );
    label->setOrigin( x + thickness, y + height - label->getHeight() - thickness );

    //overlay->setBorderWidth(thickness);
    overlay->setSize(width, height);
    overlay->setOrigin(x, y);
}

bool OsgWidgetBorder::getShowBorder() const
{
    UTILS_ASSERT(parent);
    const osgWidget::Window* realParent = overlay->getParent();
    UTILS_ASSERT(!realParent || realParent == parent, "Invalid parent.");
    return realParent == parent;
}

void OsgWidgetBorder::setShowLabel( bool value )
{
    UTILS_ASSERT(parent);
    if (!getShowLabel() ) {
        if ( value ) { 
            //parent->addWidget(label);
        }
    } else if ( !value ) { 
        //parent->removeWidget(label);
    }
}

void OsgWidgetBorder::setShowBorder( bool value )
{
    UTILS_ASSERT(parent);
    if (!getShowBorder()) {
        if ( value ) {
            parent->addWidget(overlay);
        }
    } else if ( !value ) {
        parent->removeWidget(overlay);
    }
}

bool OsgWidgetBorder::getShowLabel()
{
    UTILS_ASSERT(parent, "Parent not set.");
    const osgWidget::Window* realParent = label->getParent();
    UTILS_ASSERT(!realParent || realParent == parent, "Invalid parent.");
    return realParent == parent;
}

osgWidget::Window* OsgWidgetBorder::getParent()
{
    return parent;
}

const osgWidget::Window* OsgWidgetBorder::getParent() const
{
    return parent;
}

void OsgWidgetBorder::setParent( osgWidget::Window* parent )
{
    // TODO: asercje!
    this->parent = parent;
}

osgWidget::Label* OsgWidgetBorder::getLabel()
{
    return label.get();
}

const osgWidget::Label* OsgWidgetBorder::getLabel() const
{
    return label.get();
}

void OsgWidgetBorder::onClone( const OsgWidgetBorder& proto )
{   
    UTILS_ASSERT(parent);
    label = dynamic_cast<osgWidget::Label*>(parent->getByName(proto.label->getName()));
    //overlay = dynamic_cast<Background*>(parent->getByName(proto.overlay->getName()));
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
