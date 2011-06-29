/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:32
	filename: VideoImageStreamSizeOptimizer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__
#define HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__

#include <vector>
#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/NodeCallback>
#include <osg/Referenced>
#include <osg/Image>
UTILS_POP_WARNINGS
#include <vidlib/Export.h>
#include "VideoImageStream.h"


////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////
UTILS_PUSH_WARNINGS
UTILS_DISABLE_DLL_INTERFACE_WARNING

class VIDLIB_EXPORT VideoImageStreamSizeOptimizer : public osg::NodeCallback
{
public:
    class Client : public osg::Referenced
    {
    public:
        virtual ~Client() {}
        //! \return Rozmiar obrazka.
        virtual osg::Vec2 getDesiredImageSize() = 0;
        //! Reakcja na zmian� rozmiaru obrazka.
        virtual void onImageResized(VideoImageStreamSizeOptimizer* optimizer, int prevS, int prevT) = 0;
        //! \return Czy mo�na jeszcze korzysta� z tego callbacka?
        virtual bool isValid() = 0;
    };

private:
    //! Lista kontrolowanych widget�w.
    std::vector<osg::ref_ptr<Client> > clients;
    //! Kontrolowany strumie�.
    osg::ref_ptr<osg::Image> image;
    //! Zmienna kt�r� mo�na podpi�� do shader�w; zawsze b�dzie mia�a aktualny rozmiar.
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

    //! Od�wie�a rozmiar strumienia.
    void refreshSize();

// osg::NodeCallback
public:
    //! Wywo�uje refreshSize.
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
};

UTILS_POP_WARNINGS
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAMSIZEOPTIMIZER_H__