#include "../PCH.h"
#include <vidlib/osg/VideoImageStreamSizeOptimizer.h>
#include <boost/foreach.hpp>
#include <vidlib/osg/VideoImageStreamSizeOptimizer.h>
#include <osg/Uniform>

#undef  min
#undef  max

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

VideoImageStreamSizeOptimizer::VideoImageStreamSizeOptimizer(osg::Uniform* imageSize /*= NULL*/) : 
imageSize( imageSize ? imageSize : new osg::Uniform() )
{
}

void VideoImageStreamSizeOptimizer::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    refreshSize();
    traverse(node,nv);
}

void VideoImageStreamSizeOptimizer::refreshSize()
{
    UTILS_ASSERT(image);
    int prevWidth = image->s();
    int prevHeight = image->t();
    // okreœlenie maksymalnej szerokoœci
    float maxWidth = 0;
    for ( size_t i = 0; i < clients.size(); ++i ) {
        Client* client = clients[i];
        if ( client->isValid() ) {
            maxWidth = std::max(client->getDesiredImageSize().x(), maxWidth);
        }
    }
    
    // TODO: przenieœæ do metody wirtualnej?
    if ( vidlib::VideoImageStream* videoImageStream = dynamic_cast<vidlib::VideoImageStream*>(image.get()) ) {
        // aktualizacja wymiarów
        videoImageStream->setMaxWidth( int(ceil(maxWidth)) );
    }

    // aktualizacja rozmiaru
    if ( prevWidth != image->s() ) {
        imageSize->set( osg::Vec2(image->s(), image->t()) );
        BOOST_FOREACH(Client* client, clients) {
            if ( client->isValid() ) {
                client->onImageResized(this, prevWidth, prevHeight);
            }
        }
    }
}

void VideoImageStreamSizeOptimizer::setImage( osg::Image* image )
{
    this->image = image;
    if ( image ) {
        imageSize->set( osg::Vec2(image->s(), image->t()) );
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
