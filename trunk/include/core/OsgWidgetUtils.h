/********************************************************************
	created:  2010/12/22
	created:  22:12:2010   17:01
	filename: OsgWidgetUtils.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__OSGWIDGETUTILS_H__
#define __HEADER_GUARD__OSGWIDGETUTILS_H__

#include <osg/StateSet>
#include <osg/Image>
#include <osg/TextureRectangle>
#include <osg/Texture>
#include <osgWidget/Widget>


////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

inline void flipTexCoordsVertically(osg::Image* image, osgWidget::Widget* widget)
{
    // czy trzeba zrobiæ flipa?
    osgWidget::TexCoord temp = widget->getTexCoord(osgWidget::Widget::UPPER_LEFT);
    widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_LEFT), osgWidget::Widget::UPPER_LEFT);
    widget->setTexCoord(temp, osgWidget::Widget::LOWER_LEFT);
    temp = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
    widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_RIGHT), osgWidget::Widget::UPPER_RIGHT);
    widget->setTexCoord(temp, osgWidget::Widget::LOWER_RIGHT);
}

inline void correctTexCoords(osg::Image* image, osgWidget::Widget* widget)
{
    switch ( image->getOrigin() ) {
        case osg::Image::TOP_LEFT:
            flipTexCoordsVertically(image, widget);
            break;
        case osg::Image::BOTTOM_LEFT:
            break;
        default:
            UTILS_ASSERT(false, "Not implemented.");
            break;
    }
}

inline osg::Texture* getTexture(osg::StateSet* state)
{
    if (state) {
        return dynamic_cast<osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    }
    return NULL;
}

inline const osg::Texture* getTexture(const osg::StateSet* state)
{
    if (state) {
        return dynamic_cast<const osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    }
    return NULL;
}

inline osg::TextureRectangle* getTextureRect(osg::StateSet* state)
{
    return dynamic_cast<osg::TextureRectangle*>(getTexture(state));
}

inline const osg::TextureRectangle* getTextureRect(const osg::StateSet* state)
{
    return dynamic_cast<const osg::TextureRectangle*>(getTexture(state));
}

inline osg::Texture* getTexture(osgWidget::Widget* widget)
{
    if (widget) {
        return getTexture(widget->getStateSet());
    }
    return NULL;
}

inline const osg::Texture* getTexture(const osgWidget::Widget* widget)
{
    if (widget) {
        return getTexture(widget->getStateSet());
    }
    return NULL;
}

inline osg::TextureRectangle* getTextureRect(osgWidget::Widget* widget)
{
    return dynamic_cast<osg::TextureRectangle*>(getTexture(widget));
}

inline const osg::TextureRectangle* getTextureRect(const osgWidget::Widget* widget)
{
    return dynamic_cast<const osg::TextureRectangle*>(getTexture(widget));
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__OSGWIDGETUTILS_H__