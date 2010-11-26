#include "VideoPCH.h"
#include "StreamOsgWidget.h"
#include "osg/VideoImageStream.h"

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

StreamOsgWidget::StreamOsgWidget( const std::string& name ) : osgWidget::Widget(name)
{

}

StreamOsgWidget::~StreamOsgWidget()
{

}

osg::Uniform* StreamOsgWidget::getTextureSize()
{
    return textureSize.get();
}

void StreamOsgWidget::setTextureSize( osg::Uniform* textureSize )
{
    this->textureSize = textureSize;
}

void StreamOsgWidget::positioned()
{
    osgWidget::Widget::positioned();
    adjustStreamSize();
}

void StreamOsgWidget::adjustStreamSize()
{
    osg::Texture* texture = NULL;
    osg::Image* image = NULL;
    vmOSGPlugin::VideoImageStream* stream = NULL;
    osg::StateSet* state = NULL;

    // pobranie danych
    if (state = getStateSet()) {
        texture = dynamic_cast<osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
        if ( texture ) {
            image = texture->getImage(0);
            stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
        }
    }

    // aktualizacja rozmiaru i uvek
    if ( stream ) {
        if ( dynamic_cast<osg::TextureRectangle*>(texture) ) {
            // pobranie i ewentualna normalizacja wspó³rzêdnych
            osgWidget::TexCoord coords[4];
            for (unsigned i = 0; i < utils::length(coords); ++i) {
                coords[i] = getTexCoord(static_cast<osgWidget::Widget::Corner>(i));
                coords[i].x() *= 1.0f/image->s();
                coords[i].y() *= 1.0f/image->t();
            }

            // aby niepotrzebnie nie tworzyæ gigantycznych tekstur
            stream->setMaxWidth( getWidth() );

            // transformacja do bezwzglêdnych wspó³rzêdnych
            for (unsigned i = 0; i < utils::length(coords); ++i) {
                coords[i].x() *= image->s();
                coords[i].y() *= image->t();
                setTexCoord( coords[i], static_cast<osgWidget::Widget::Corner>(i) );
            }
        } else {
            // wspó³rzêdne wzglêdne, nie trzeba nic robiæ dodatkowo
            stream->setMaxWidth( getWidth() );
        }
    }

    // aktualizacja zmiennej trzymaj¹cej rozmiar tekstury
    if ( textureSize ) {
        textureSize->set(image->s(), image->t());
    }
}
////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

