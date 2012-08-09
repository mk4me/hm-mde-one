#include "VideoPCH.h"
#include "StreamOsgWidget.h"
#include <vidlib/osg/VideoImageStream.h>
#include <core/PluginCommon.h>
#include <core/ILog.h>
#include <osg/TextureRectangle>

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

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

