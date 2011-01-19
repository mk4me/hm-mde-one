/********************************************************************
	created:  2010/07/13
	created:  13:7:2010   22:04
	filename: ImageCanvas.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__IMAGECANVAS_H__
#define __HEADER_GUARD__IMAGECANVAS_H__

#include <osg/Image>
#include <osgWidget/Widget>
#include <osgWidget/Canvas>
#include <osgWidget/Label>
#include <core/Enumerators.h>
#include <core/OsgWidgetBorder.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class ImageCanvas : public osgWidget::Canvas
{
private:
  //! Obrazek.
  osg::ref_ptr<osg::Image> image;
  //! Prostok¹t zawieraj¹cy obrazek.
  osg::ref_ptr<osgWidget::Widget> rect;
  //! Czy rozmiar ma odpowiadaæ rozmiarowi obrazka?
  bool keepImageRatio;
  //!
  core::OsgWidgetBorder border;

public:
  META_Object(osgUI, ImageCanvas)

  //! Konstruktor domyœlny.
  //! \param Nazwa kontrolki.
  ImageCanvas(const std::string& name = "");

  //! Konstruktor kopiuj¹cy.
  //! \param canvas
  //! \param copyop
  ImageCanvas(const ImageCanvas& canvas, const osg::CopyOp& copyop);

  virtual ~ImageCanvas();

  //! \param maxWidth
  //! \param maxHeight
  //! \return Rozmiar (przewa¿nie mniejszy) zgodny z proporcjami obrazka.
  osgWidget::XYCoord getRationedSize(float maxWidth, float maxHeight);


  //! \return
  core::OsgWidgetBorder* getBorder()
  { 
      return &border;
  }
  //! \return
  osgWidget::Label * getLabel()
  {
    return border.getLabel();
  }
  //! \return
  osgWidget::Widget * getRect()
  {
    return rect.get();
  }
  //! \return
  const osgWidget::Label * getLabel() const
  {
    return border.getLabel();
  }
  //! \return
  const osgWidget::Widget * getRect() const
  {
    return rect.get();
  }
  //! \return
  inline const osg::Image* getImage() const
  { 
    return image.get();
  }
  //! \return
  inline osg::Image* getImage()
  { 
    return image.get();
  }
  //! \param image
  void setImage(osg::Image* image, bool useTextureRect = false);

  //! Ustawia wspó³rzêdne tekstury.
  //! \param normCoords Znormalizowane wspó³rzêdne.
  void setTexCoord(osgWidget::Quad normCoords = osgWidget::Quad(0,0,1,1) );
  
  //! \return
  inline bool getKeepImageRatio() const
  { 
    return keepImageRatio;
  }
  //! \param keepImageRatio
  void setKeepImageRatio(bool keepImageRatio);

protected:
  //! Prze³adowanie metody rozmieszczaj¹cej elementy.
  //! \param diffWidth
  //! \param diffHeight
  virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight );
  //! Prze³adowanie metody obliczaj¹cej rozmiar.
  virtual osgWidget::Window::Sizes _getWidthImplementation() const;
  //! Prze³adowanie metody obliczaj¹cej rozmiar.
  virtual osgWidget::Window::Sizes _getHeightImplementation() const;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__IMAGECANVAS_H__