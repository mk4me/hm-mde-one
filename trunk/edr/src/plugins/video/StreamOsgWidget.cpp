#include "VideoPCH.h"
#include "StreamOsgWidget.h"
#include <vidlib/osg/VideoImageStream.h>
#include <core/ILog.h>


////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

StreamOsgWidget::StreamOsgWidget( const std::string& name ) : 
osgWidget::Widget(name),
yuvProgram(new osg::Program),
format(vidlib::PixelFormatBGRA)
{
    getOrCreateStateSet()->setAttribute(yuvProgram);
}

StreamOsgWidget::~StreamOsgWidget()
{
}

osg::Shader* StreamOsgWidget::getYuvTextureRectShader() const
{
    return yuvTextureRectShader;
}

void StreamOsgWidget::setYuvTextureRectShader( osg::Shader* yuvTextureRectShader )
{
    if ( this->yuvTextureRectShader != yuvTextureRectShader ) {
        if ( this->yuvTextureRectShader ) {
            yuvProgram->removeShader(this->yuvTextureRectShader);
        }
        if ( this->yuvTextureRectShader = yuvTextureRectShader ) {
            yuvProgram->addShader(this->yuvTextureRectShader);
        }
    }
}

osg::Shader* StreamOsgWidget::getYuvTexture2DShader() const
{
    return yuvTexture2DShader;
}

void StreamOsgWidget::setYuvTexture2DShader( osg::Shader* yuvTexture2DShader )
{
    if ( this->yuvTexture2DShader != yuvTexture2DShader ) {
        if ( this->yuvTexture2DShader ) {
            yuvProgram->removeShader(this->yuvTexture2DShader);
        }
        if ( this->yuvTexture2DShader = yuvTexture2DShader ) {
            yuvProgram->addShader(this->yuvTexture2DShader);
        }
    }
}

bool StreamOsgWidget::isUsingTextureRectangle() const
{
    if ( const osg::StateSet* state = getStateSet() ) {
        return dynamic_cast<const osg::TextureRectangle*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE)) != NULL;
    } else {
        return false;
    }
}

void StreamOsgWidget::refreshShaders( bool useTextureRect, vidlib::PixelFormat format )
{
    if ( format == vidlib::PixelFormatYV12 ) {
        // usuniêcie shaderów z programu
        while ( yuvProgram->getNumShaders() ) {
            yuvProgram->removeShader( yuvProgram->getShader(0) );
        }
        if ( useTextureRect ) {
            if (yuvTextureRectShader) {
                yuvProgram->addShader(yuvTextureRectShader);
            } else {
                LOG_ERROR("Missing yuv texture_rect shader. As a result image may be incorrect.");        
            }
        } else {
            if (yuvTexture2DShader) {
                yuvProgram->addShader(yuvTexture2DShader);
            } else {
                LOG_ERROR("Missing yuv texture2D shader. As a result image may be incorrect.");
            }
        }
        // TODO: sprawdziæ czy wielokrotnie nie dodajemy
        getOrCreateStateSet()->setAttribute(yuvProgram);
    } else if ( format == vidlib::PixelFormatARGB || format == vidlib::PixelFormatBGRA || format == vidlib::PixelFormatRGB24 ) {
        // usuwamy shadera, poniewaz mo¿na odrysowaæ wprost
        if ( osg::StateSet* state = getOrCreateStateSet() ) {
            state->removeAttribute(yuvProgram);
        }
    } else {
        LOG_ERROR("Nieobs³ugiwany format: "<<utils::Enum<vidlib::PixelFormat>::getName(format));
    }
}

void StreamOsgWidget::refreshShaders()
{
    refreshShaders( isUsingTextureRectangle(), format );
}

void StreamOsgWidget::setPixelFormat( vidlib::PixelFormat format )
{
    this->format = format;
    refreshShaders();
}

vidlib::PixelFormat StreamOsgWidget::getPixelFormat() const
{
    return format;
}


// 
// void StreamOsgWidget::__deprecated__positioned()
// {
//     osgWidget::Widget::positioned();
//     __deprecated__adjustStreamSize();
// }
// 
// void StreamOsgWidget::__deprecated__adjustStreamSize()
// {
//     osg::Texture* texture = NULL;
//     osg::Image* image = NULL;
//     vmOSGPlugin::VideoImageStream* stream = NULL;
//     osg::StateSet* state = NULL;
// 
//     // pobranie danych
//     if (state = getStateSet()) {
//         texture = dynamic_cast<osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
//         if ( texture ) {
//             image = texture->getImage(0);
//             stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
//         }
//     }
// 
//     // aktualizacja rozmiaru i uvek
//     if ( stream ) {
//         if ( dynamic_cast<osg::TextureRectangle*>(texture) ) {
//             // pobranie i ewentualna normalizacja wspó³rzêdnych
//             osgWidget::TexCoord coords[4];
//             for (unsigned i = 0; i < utils::length(coords); ++i) {
//                 coords[i] = getTexCoord(static_cast<osgWidget::Widget::Corner>(i));
//                 coords[i].x() *= 1.0f/image->s();
//                 coords[i].y() *= 1.0f/image->t();
//             }
// 
//             // aby niepotrzebnie nie tworzyæ gigantycznych tekstur
//             stream->setMaxWidth( getWidth() );
// 
//             // transformacja do bezwzglêdnych wspó³rzêdnych
//             for (unsigned i = 0; i < utils::length(coords); ++i) {
//                 coords[i].x() *= image->s();
//                 coords[i].y() *= image->t();
//                 setTexCoord( coords[i], static_cast<osgWidget::Widget::Corner>(i) );
//             }
//         } else {
//             // wspó³rzêdne wzglêdne, nie trzeba nic robiæ dodatkowo
//             stream->setMaxWidth( getWidth() );
//         }
//     }
// 
//     // aktualizacja zmiennej trzymaj¹cej rozmiar tekstury
//     if ( __deprecated__textureSize ) {
//         __deprecated__textureSize->set(image->s(), image->t());
//     }
// }
// 
// osg::Uniform* StreamOsgWidget::__deprecated__getTextureSize()
// {
//     return __deprecated__textureSize;
// }
// 
// void StreamOsgWidget::__deprecated__setTextureSize( osg::Uniform* textureSize )
// {
//     if ( this->__deprecated__textureSize != textureSize ) {
//         if ( this->__deprecated__textureSize ) {
//             getOrCreateStateSet()->removeUniform(this->__deprecated__textureSize);
//         }
//         if ( this->__deprecated__textureSize = textureSize ) {
//             getOrCreateStateSet()->addUniform(textureSize);
//         }
//     }
// }
// void StreamOsgWidget::setUseTextureRect( bool useTextureRect )
// {
// //     this->useTextureRect = useTextureRect;
// //     refreshShaders(useTextureRect, getPixelFormat());
// }
// vidlib::PixelFormat StreamOsgWidget::getPixelFormat() const
// {
//     if (const vmOSGPlugin::VideoImageStream* stream = getStream(getStateSet())) {
//         return stream->getTargetFormat();
//     } else {
//         return vidlib::PixelFormatUndefined;
//     }
// }

// void StreamOsgWidget::setPixelFormat( vidlib::PixelFormat format )
// {
//     if (vmOSGPlugin::VideoImageStream* stream = getStream(getStateSet())) {
//         stream->setTargetFormat(format);
//         refreshShaders(true, format);
//     } else {
//         LOG_ERROR("Couldn't set desired format.");
//     }
// }


////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

