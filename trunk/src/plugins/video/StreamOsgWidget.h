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

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Widget wspó³pracuj¹cy ze strumieniem video HD. Pozwala na dostosowanie
 *  rozmiaru tekstury do swojego wymiaru.
 *  Zak³ada, ¿e widget wype³niony jest ca³ym obrazkiem.
 *  TODO: zaktualizowaæ pod wszystkie ImageStreamy.
 */
class StreamOsgWidget : public osgWidget::Widget
{
private:

//     class Optimizer : public osg::Referenced
//     {
//         //! Zestaw widgetów, na podstawie którego okreœlany bêdzie rozmiar tekstury.
//         typedef utils::Adapter< std::set< osg::observer_ptr<StreamOsgWidget> > > Streams;
//         //! Zestaw widgetów, na podstawie którego okreœlany bêdzie rozmiar tekstury.
//         Streams 
//     };

private:
    //! Rozmiar tekstury.
    osg::ref_ptr<osg::Uniform> textureSize;

public: 
    //! \param name
    StreamOsgWidget(const std::string& name);
    //! 
    virtual ~StreamOsgWidget();

public:
    //! \return Zmienna na rozmiar tekstury.
    osg::Uniform* getTextureSize();
    //! \param textureSize Zmienna na rozmiar tekstury.
    void setTextureSize(osg::Uniform* textureSize);
    //! Dostosowuje
    //! rozmiar strumienia do bie¿¹cego rozmiaru. Sam strumieñ pobierany
    //! jest z bie¿¹cego stanu widgeta, wiêc zawsze jest prawid³owy.
    void adjustStreamSize();
    //! Prze³adowanie metody wywo³ywanej po pozycjonowaniu widgetu. Wywo³uje
    //! adjustStreamSize
    virtual void positioned();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////




#endif  // __HEADER_GUARD__StreamOsgWidget_H__