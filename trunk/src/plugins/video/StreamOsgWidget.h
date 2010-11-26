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
 *	Widget wsp�pracuj�cy ze strumieniem video HD. Pozwala na dostosowanie
 *  rozmiaru tekstury do swojego wymiaru.
 *  Zak�ada, �e widget wype�niony jest ca�ym obrazkiem.
 *  TODO: zaktualizowa� pod wszystkie ImageStreamy.
 */
class StreamOsgWidget : public osgWidget::Widget
{
private:

//     class Optimizer : public osg::Referenced
//     {
//         //! Zestaw widget�w, na podstawie kt�rego okre�lany b�dzie rozmiar tekstury.
//         typedef utils::Adapter< std::set< osg::observer_ptr<StreamOsgWidget> > > Streams;
//         //! Zestaw widget�w, na podstawie kt�rego okre�lany b�dzie rozmiar tekstury.
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
    //! rozmiar strumienia do bie��cego rozmiaru. Sam strumie� pobierany
    //! jest z bie��cego stanu widgeta, wi�c zawsze jest prawid�owy.
    void adjustStreamSize();
    //! Prze�adowanie metody wywo�ywanej po pozycjonowaniu widgetu. Wywo�uje
    //! adjustStreamSize
    virtual void positioned();
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////




#endif  // __HEADER_GUARD__StreamOsgWidget_H__