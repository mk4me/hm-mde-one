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

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class ImageCanvas : public osgWidget::Canvas
{
private:
    //! Indeks obramowania.
    enum Border {
        BorderLeft,
        BorderTop,
        BorderRight,
        BorderBottom
    };

  //! Obrazek.
  osg::ref_ptr<osg::Image> image;
  //! Prostok�t zawieraj�cy obrazek.
  osg::ref_ptr<osgWidget::Widget> rect;
  //! Podpis.
  osg::ref_ptr<osgWidget::Label> label;
  //! Ramka.
  osg::ref_ptr<osgWidget::Widget> border[4];
  //! Czy rozmiar ma odpowiada� rozmiarowi obrazka?
  bool keepImageRatio;

public:
  META_Object(osgUI, ImageCanvas)

  //! Konstruktor domy�lny.
  //! \param Nazwa kontrolki.
  ImageCanvas(const std::string& name = "");

  //! Konstruktor kopiuj�cy.
  //! \param canvas
  //! \param copyop
  ImageCanvas(const ImageCanvas& canvas, const osg::CopyOp& copyop);

  virtual ~ImageCanvas();

  //! \param maxWidth
  //! \param maxHeight
  //! \return Rozmiar (przewa�nie mniejszy) zgodny z proporcjami obrazka.
  osgWidget::XYCoord getRationedSize(float maxWidth, float maxHeight);

  //! \return
  osgWidget::Label * getLabel()
  {
    return label.get();
  }
  //! \return
  osgWidget::Widget * getRect()
  {
    return rect.get();
  }
  //! \return
  const osgWidget::Label * getLabel() const
  {
    return label.get();
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

  //! Ustawia wsp�rz�dne tekstury.
  //! \param normCoords Znormalizowane wsp�rz�dne.
  void setTexCoord(osgWidget::Quad normCoords = osgWidget::Quad(0,0,1,1) );
  
  //! \return Kolor obramowania.
  inline osgWidget::Color getBorderColor() const
  {
    return border[BorderTop]->getColor();
  }
  //! \param r
  //! \param g
  //! \param b
  //! \param a
  inline void setBorderColor(osgWidget::color_type r, osgWidget::color_type g, 
    osgWidget::color_type b, osgWidget::color_type a)
  {
    setBorderColor(osgWidget::Color(r, g, b, a));
  }
  //! \param color Kolor obramowania.
  void setBorderColor(osgWidget::Color color);

  //! \return Czy pokazuje etykiet�?
  inline bool getShowLabel() const
  {
    return label->getParent() == this;
  }
  //! \param value Czy etykieta ma by� pokazywana?

  void setShowLabel(bool value);
  //! \return Czy pokazuje obramowanie?
  inline bool getShowBorder() const
  {
      return border[BorderTop]->getParent() == this;
  }
  //! \param value Czy obramowanie ma by� pokazywane?
  void setShowBorder(bool value);

  //! \return
  inline bool getKeepImageRatio() const
  { 
    return keepImageRatio;
  }
  //! \param keepImageRatio
  void setKeepImageRatio(bool keepImageRatio);

protected:
  //! Prze�adowanie metody rozmieszczaj�cej elementy.
  //! \param diffWidth
  //! \param diffHeight
  void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight );
  //! Prze�adowanie metody obliczaj�cej rozmiar.
  osgWidget::Window::Sizes _getWidthImplementation() const;
  //! Prze�adowanie metody obliczaj�cej rozmiar.
  osgWidget::Window::Sizes _getHeightImplementation() const;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////



#endif  // __HEADER_GUARD__IMAGECANVAS_H__