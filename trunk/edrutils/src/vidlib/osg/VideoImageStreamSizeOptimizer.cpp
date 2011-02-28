#include "../PCH.h"
#include <vidlib/osg/VideoImageStreamSizeOptimizer.h>
#include <boost/foreach.hpp>
UTILS_PUSH_WARNINGS
#include <osg/Uniform>
UTILS_POP_WARNINGS

#undef  min
#undef  max

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

typedef osg::Vec2::value_type coord_type;

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
    // okre�lenie maksymalnej szeroko�ci
    float maxWidth = 0;
    for ( size_t i = 0; i < clients.size(); ++i ) {
        Client* client = clients[i];
        if ( client->isValid() ) {
            maxWidth = std::max(client->getDesiredImageSize().x(), maxWidth);
        }
    }
    
    // TODO: przenie�� do metody wirtualnej?
    if ( vidlib::VideoImageStream* videoImageStream = dynamic_cast<vidlib::VideoImageStream*>(image.get()) ) {
        // aktualizacja wymiar�w
        videoImageStream->setMaxWidth( int(ceil(maxWidth)) );
    }

    // aktualizacja rozmiaru
    if ( prevWidth != image->s() ) {
        imageSize->set( osg::Vec2(static_cast<coord_type>(image->s()), static_cast<coord_type>(image->t())) );
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
        imageSize->set( osg::Vec2(static_cast<coord_type>(image->s()), static_cast<coord_type>(image->t())) );
    }
}

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////
