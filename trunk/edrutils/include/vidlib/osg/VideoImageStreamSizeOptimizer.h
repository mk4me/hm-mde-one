/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:32
	filename: VideoImageStreamSizeOptimizer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__
#define __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__

#include <vector>
#include <osg/NodeCallback>
#include <osg/Referenced>
#include <osg/Image>
#include <vidlib/Export.h>
#include "VideoImageStream.h"

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class VIDLIB_EXPORT VideoImageStreamSizeOptimizer : public osg::NodeCallback
{
public:
    class Client : public osg::Referenced
    {
    public:
        virtual ~Client() {}
        //! \return Rozmiar obrazka.
        virtual osg::Vec2 getDesiredImageSize() = 0;
        //! Reakcja na zmianê rozmiaru obrazka.
        virtual void onImageResized(VideoImageStreamSizeOptimizer* optimizer, int prevS, int prevT) = 0;
        //! \return Czy mo¿na jeszcze korzystaæ z tego callbacka?
        virtual bool isValid() = 0;
    };

private:
    //! Lista kontrolowanych widgetów.
    std::vector<osg::ref_ptr<Client> > clients;
    //! Kontrolowany strumieñ.
    osg::ref_ptr<osg::Image> image;
    //! Zmienna któr¹ mo¿na podpi¹æ do shaderów; zawsze bêdzie mia³a aktualny rozmiar.
    osg::ref_ptr<osg::Uniform> imageSize;

public:
    VideoImageStreamSizeOptimizer( osg::Uniform* imageSize = NULL);

public:
    //! \return Widgety.
    std::vector<osg::ref_ptr<Client> >& getClients()
    { 
        return clients;
    }
    //! \return Widgety.
    const std::vector<osg::ref_ptr<Client> >& getClients() const
    { 
        return clients;
    }
    //! \return
    osg::Uniform* getImageSize()
    { 
        return imageSize;
    }
    //! \return
    osg::Image* getImage()
    { 
        return image;
    }
    //!
    void setImage(osg::Image* image);

    //! Odœwie¿a rozmiar strumienia.
    void refreshSize();

// osg::NodeCallback
public:
    //! Wywo³uje refreshSize.
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__