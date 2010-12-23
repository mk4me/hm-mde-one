/********************************************************************
	created:  2010/11/12
	created:  12:11:2010   16:29
	filename: StreamOsgWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__StreamOsgWidget_H__
#define __HEADER_GUARD__StreamOsgWidget_H__

#include <string>
#include <osg/ref_ptr>
#include <osg/Uniform>
#include <osgWidget/Widget>
#include <plugins/video/core/PixelFormat.h>

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Widget dostowany do wymagañ strumieni video. Obs³uguje niestandardowe
 *  formaty pikseli.
 */
class StreamOsgWidget : public osgWidget::Widget
{
    //! Program u¿ywany w przypadku konwersji yuv w karcie graficznej.
    osg::ref_ptr<osg::Program> yuvProgram;
    //! Shader u¿ywany dla texture_rectów.
    osg::ref_ptr<osg::Shader> yuvTextureRectShader;
    //! Shader u¿ywany dla texture2D.
    osg::ref_ptr<osg::Shader> yuvTexture2DShader;
    //! Format pikseli.
    video::PixelFormat format;

public: 
    //! \param name Nazwa widgetu.
    StreamOsgWidget(const std::string& name);
    //! 
    virtual ~StreamOsgWidget();

public:
    //! \return
    video::PixelFormat getPixelFormat() const;
    //! \param format
    void setPixelFormat(video::PixelFormat format);
    //! \return Czy u¿ywany jest texture_rectangle
    bool isUsingTextureRectangle() const;
    //! \return
    osg::Shader* getYuvTextureRectShader() const;
    //! \param yuvTextureRectShader
    void setYuvTextureRectShader(osg::Shader* yuvTextureRectShader);
    //! \return
    osg::Shader* getYuvTexture2DShader() const;
    //! \param yuvTexture2DShader
    void setYuvTexture2DShader(osg::Shader* yuvTexture2DShader);
    //!
    void refreshShaders();

private:
    //! \param useTextureRect
    //! \param format
    void refreshShaders(bool useTextureRect, video::PixelFormat format);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////




#endif  // __HEADER_GUARD__StreamOsgWidget_H__