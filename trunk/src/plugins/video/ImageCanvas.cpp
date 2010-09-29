#include "VideoPCH.h"
#include <osg/TextureRectangle>
#include "ImageCanvas.h"

#undef min
#undef max


////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

ImageCanvas::ImageCanvas( const std::string& name/*= ""*/ ) :
  osgWidget::Canvas(name),
  keepImageRatio(false)
{
  // t³o przeŸroczyste
  getBackground()->setColor(0, 0, 0, 0);

  // tworzymy labelkê
  label = new osgWidget::Label("label", "NONE");
  label->setColor(osgWidget::Color(0.5, 0.5, 0.5, 0.5));
  label->setFontColor(osgWidget::Color(1, 1, 1, 1));
  label->setAlignHorizontal(osgWidget::Widget::HA_CENTER);
  label->setAlignVertical(osgWidget::Widget::VA_TOP);
  label->setPadding(1);
  label->setHeight(10);
  label->setLayer(osgWidget::Widget::LAYER_HIGH);

  // tworzymy widget
  rect = new osgWidget::Widget("image", 64, 64);
  rect->setPadding(1);
  rect->setLayer(osgWidget::Widget::LAYER_MIDDLE);
  rect->setColor(1.0, 1.0, 1.0, 1.0);
  rect->setEventMask(osgWidget::EVENT_MOUSE_SCROLL);

  // tworzymy ramkê
  borderLeft = new osgWidget::Widget("borderLeft", 1, 1);
  borderLeft->setColor(1, 1, 1, 1);
  borderLeft->setLayer(osgWidget::Widget::LAYER_MIDDLE, 1);
  borderTop = osg::clone(borderLeft.get(), "borderTop", osg::CopyOp::DEEP_COPY_ALL);
  borderRight = osg::clone(borderLeft.get(), "borderRight", osg::CopyOp::DEEP_COPY_ALL);
  borderBottom = osg::clone(borderLeft.get(), "borderBottom", osg::CopyOp::DEEP_COPY_ALL);

  // dodajemy widgety
  addWidget(rect, 0, 0);
  addWidget(label, 0, 0);
  addWidget(borderLeft, 0, 0);
  addWidget(borderTop, 0, 0);
  addWidget(borderRight, 0, 0);
  addWidget(borderBottom, 0, 0);
}

ImageCanvas::ImageCanvas( const ImageCanvas& canvas, const osg::CopyOp& copyop ):
  osgWidget::Canvas(canvas, copyop),
  keepImageRatio(canvas.keepImageRatio)
{
  rect = getByName("image");
  label = dynamic_cast<osgWidget::Label*>(getByName("label"));
  borderLeft = getByName("borderLeft");
  borderTop = getByName("borderTop");
  borderRight = getByName("borderRight");
  borderBottom = getByName("borderBottom");

  // ustawiamy zmienn¹ image
  osg::StateSet* ss = rect->getStateSet();
  if (ss) {
    osg::Texture * texture = dynamic_cast<osg::Texture*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    if ( texture ) {
      image = texture->getImage(0);
    }
  }
}

osgWidget::XYCoord ImageCanvas::getRationedSize( float maxWidth, float maxHeight )
{
  float rectRatio = maxWidth / maxHeight;
  float imageRatio = (image->s()* image->getPixelAspectRatio()) / image->t() ;
  if ( imageRatio > rectRatio ) {
    maxWidth = ceil(maxWidth);
    maxHeight = ceil(maxWidth / imageRatio);
  } else {
    maxHeight = ceil(maxHeight);
    maxWidth = ceil(maxHeight * imageRatio);
  }
  return osgWidget::XYCoord(maxWidth, maxHeight);
}

void ImageCanvas::setImage( osg::Image* image, bool useTextureRect /*= false*/ )
{
  this->image = image;
  if ( image ) {
    osg::Texture * texture = NULL;
    if ( useTextureRect ) {
      texture = new osg::TextureRectangle();
      texture->setImage(0, image);
    } else {
      // tworzymy teksturkê
      texture = new osg::Texture2D();
      texture->setImage(0, image);
      texture->setResizeNonPowerOfTwoHint(false);
    }
    // ustawiamy teksturkê
    rect->setTexture(texture, false, false);
    // ustawiamy wspó³rzêdne
    setTexCoord();
    label->setLabel(image->getFileName());
  } else {
    rect->setTexture(NULL);
    label->setLabel("NONE");
  }
}

void ImageCanvas::setBorderColor( osgWidget::Color color )
{
  borderBottom->setColor(color);
  borderTop->setColor(color);
  borderRight->setColor(color);
  borderBottom->setColor(color);
}

void ImageCanvas::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
  osgWidget::Canvas::_resizeImplementation(diffWidth, diffHeight);
  // aktualizacja rozmiaru prostok¹ta z obrazkiem
  float height = getHeight() + diffHeight;
  float width = getWidth() + diffWidth;

  if ( isKeepingImageRatio() ) {
    osgWidget::XYCoord size = getRationedSize(width, height);
    width = size.x();
    height = size.y();
  }

  // ustawienie rozmiaru obrazka
  rect->setSize(width - rect->getPadHorizontal(), height - rect->getPadVertical());
  // pozycja
  rect->setOrigin( rect->getPadLeft(), rect->getPadBottom() );
  // aktualizacja pozycji labelki
  label->setSize( width - label->getPadHorizontal(), label->getHeight() );
  label->setOrigin( label->getPadLeft(), height - label->getHeight() - label->getPadTop() );

  // resize ramki
  borderLeft->setSize(rect->getPadLeft(), height);
  borderLeft->setOrigin(0, 0);
  borderTop->setSize(width, rect->getPadTop());
  borderTop->setOrigin(0, height - borderTop->getHeight());
  borderRight->setSize(rect->getPadRight(), height);
  borderRight->setOrigin(width - borderRight->getWidth(), 0);
  borderBottom->setSize(width, rect->getPadBottom());
  borderBottom->setOrigin(0, 0);
}

osgWidget::Window::Sizes ImageCanvas::_getWidthImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = rect->getWidthTotal();
  result.minimum = rect->getPadHorizontal();
  result.minimum = std::max(0.0f, result.minimum);
  result.current = std::max(result.current, result.minimum);
  return result;
}

osgWidget::Window::Sizes ImageCanvas::_getHeightImplementation() const
{
  osgWidget::Window::Sizes result;
  result.current = rect->getHeightTotal();
  result.minimum = rect->getPadVertical();
  result.minimum = std::max(0.0f, result.minimum);
  result.current = std::max(result.current, result.minimum);
  return result;
}

void ImageCanvas::setShowLabel( bool value )
{
  if ( value && !isShowingLabel() ) {
    addWidget(label, 0, 0);
  } else if ( !value && isShowingLabel() ) {
    removeWidget(label);
  }
}

void ImageCanvas::setKeepImageRatio( bool keepImageRatio )
{
  this->keepImageRatio = keepImageRatio;
  if ( keepImageRatio ) {
    resizeAdd();
  }
}

void ImageCanvas::setTexCoord(osgWidget::Quad normCoords /*= osgWidget::Quad(0,0,1,1)*/ )
{
  // ustawiamy zmienn¹ image
  osg::StateSet* ss = rect->getStateSet();
  bool useTextureRect = ss && dynamic_cast<osg::TextureRectangle*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
  bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
  double left, right, top, bottom;
  double width, height;
  if ( useTextureRect ) {
    width = image->s();
    height = image->t();
  } else {
    width = 1.0;
    height = 1.0;
  }

  bottom = (flip ? normCoords.w() : normCoords.y()) * height;
  top = (flip ? normCoords.y() : normCoords.w()) * height;
  left = normCoords.x() * width;
  right = normCoords.z() * width;

  rect->setTexCoord(left, top, osgWidget::Widget::UPPER_LEFT);
  rect->setTexCoord(left, bottom, osgWidget::Widget::LOWER_LEFT);
  rect->setTexCoord(right, top, osgWidget::Widget::UPPER_RIGHT);
  rect->setTexCoord(right, bottom, osgWidget::Widget::LOWER_RIGHT);
}

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////

#if 0

class ImageCanvas : public osgWidget::Canvas
{
private:
  osg::ref_ptr<osgWidget::Widget> rect;
  osg::ref_ptr<osgWidget::Label> label;

  osg::ref_ptr<osgWidget::Widget> borderLeft;
  osg::ref_ptr<osgWidget::Widget> borderTop;
  osg::ref_ptr<osgWidget::Widget> borderRight;
  osg::ref_ptr<osgWidget::Widget> borderBottom;
  const int buttonSize;
  osg::ref_ptr<osg::Image> image;

  //osg::ref_ptr<Button> incAlpha;
  //osg::ref_ptr<Button> decAlpha;
  //osg::ref_ptr<Button> decSize;
  //osg::ref_ptr<Button> incSize;
  //float scale;

public:

  ImageCanvas(osg::Image * image) : buttonSize(10), image(image)
  {
    getBackground()->setColor(0, 0, 0, 0);

    // tworzymy teksturkê
    osg::Texture* texture = new osg::Texture2D();
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;
    texture->setImage(0, image);
    texture->setResizeNonPowerOfTwoHint(false);

    // tworzymy widget
    rect = new osgWidget::Widget("Image", 64, 64);
    rect->setTexture(texture, false, false);
    rect->setPadding(1);
    double bottom = flip ? 1.0 : 0.0;
    double top = flip ? 0.0 : 1.0;
    rect->setTexCoord(0.0, top, osgWidget::Widget::UPPER_LEFT);
    rect->setTexCoord(0.0, bottom, osgWidget::Widget::LOWER_LEFT);
    rect->setTexCoord(1.0, top, osgWidget::Widget::UPPER_RIGHT);
    rect->setTexCoord(1.0, bottom, osgWidget::Widget::LOWER_RIGHT);
    rect->setLayer(osgWidget::Widget::LAYER_MIDDLE);
    rect->setColor(1.0, 1.0, 1.0, 1.0);
    rect->setEventMask(osgWidget::EVENT_MOUSE_SCROLL);
    //rect->addCallback(new osgWidget::Callback(&ImageCanvas::onMouseScroll, this, osgWidget::EVENT_MOUSE_SCROLL));

    // tworzymy labelkê
    label = new osgWidget::Label("Label", image->getFileName());
    label->setColor(osgWidget::Color(0.5, 0.5, 0.5, 0.5));
    label->setFontColor(osgWidget::Color(1, 1, 1, 1));
    label->setAlignHorizontal(osgWidget::Widget::HA_CENTER);
    label->setAlignVertical(osgWidget::Widget::VA_TOP);
    label->setPadding(1);
    label->setHeight(10);
    label->setLayer(osgWidget::Widget::LAYER_HIGH);

    //// guziki
    //scale = 1.0f;
    //incAlpha = new Button("incAlpha", osgWidget::Color(0,1,1,1), buttonSize, buttonSize);
    //incAlpha->setClickedColor(osgWidget::Color(1,0,0,1));
    //incAlpha->setHooverColor(osgWidget::Color(1,1,0,1));
    //incAlpha->setLayer(osgWidget::Widget::LAYER_HIGH, 1);
    //incAlpha->addCallback( new osgWidget::Callback(&ImageCanvas::onButtonPressed, this, osgWidget::EVENT_MOUSE_PUSH) );
    //decAlpha = osg::clone(incAlpha.get(), "decAlpha", osg::CopyOp::DEEP_COPY_ALL);
    //decAlpha->setNormalColor(osgWidget::Color(0, 0, 1, 1));
    //decSize = osg::clone(incAlpha.get(), "decSize", osg::CopyOp::DEEP_COPY_ALL);
    //decSize->setNormalColor(osgWidget::Color(0, 1, 0, 1));
    //incSize = osg::clone(incAlpha.get(), "incSize", osg::CopyOp::DEEP_COPY_ALL);
    //incSize->setNormalColor(osgWidget::Color(1, 0, 1, 1));

    // dodajemy ramkê
    borderLeft = new osgWidget::Widget("borderLeft", 1, 1);
    borderLeft->setColor(1, 1, 1, 1);
    borderLeft->setLayer(osgWidget::Widget::LAYER_MIDDLE, 1);
    borderTop = osg::clone(borderLeft.get(), "boderTop", osg::CopyOp::DEEP_COPY_ALL);
    borderRight = osg::clone(borderLeft.get(), "borderRight", osg::CopyOp::DEEP_COPY_ALL);
    borderBottom = osg::clone(borderLeft.get(), "borderBottom", osg::CopyOp::DEEP_COPY_ALL);

    // dodajemy widgety
    addWidget(rect, 0, 0);
    addWidget(label, 0, 0);
    addWidget(borderLeft, 0, 0);
    addWidget(borderTop, 0, 0);
    addWidget(borderRight, 0, 0);
    addWidget(borderBottom, 0, 0);
    /*addWidget(incAlpha, 0, 0);
    addWidget(decAlpha, 0, 0);
    addWidget(decSize, 0, 0);
    addWidget(incSize, 0, 0);*/
  }

  //!
  void adjustToRectSize()
  {
    resize(rect->getWidthTotal(), rect->getHeightTotal());
  }

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

  //!
  void setBorderColor(osgWidget::color_type r, osgWidget::color_type g, osgWidget::color_type b, osgWidget::color_type a)
  {
    setBorderColor(osgWidget::Color(r, g, b, a));
  }
  //!
  void setBorderColor(osgWidget::Color color)
  {
    borderBottom->setColor(color);
    borderTop->setColor(color);
    borderRight->setColor(color);
    borderBottom->setColor(color);
  }

  //! Zmienia rozmiar kontrolki tak, aby zachowa³a proporcje zgodne z proporcjami obrazka.
  //! \param maxWidth Maksymalna mo¿liwa szerokoœæ.
  //! \param maxHeight Maksymalna mo¿liwa wysokoœæ.
  void resizeKeepImageRatio(float maxWidth, float maxHeight)
  {
    float rectRatio = maxWidth / maxHeight;
    float imageRatio = (image->s()* image->getPixelAspectRatio()) / image->t() ;
    if ( imageRatio > rectRatio ) {
      maxWidth = ceil(maxWidth);
      maxHeight = ceil(maxWidth / imageRatio);
    } else {
      maxHeight = ceil(maxHeight);
      maxWidth = ceil(maxHeight * imageRatio);
    }
    resize(maxWidth, maxHeight);
  }

//   //! \return
//   inline float getScale() const
//   {
//     return scale;
//   }
//   //! \param scale
//   inline void setScale(float scale)
//   {
//     changeScale(scale - this->scale);
//   }

protected:

  //! Prze³adowanie metody rozmieszczaj¹cej elementy.
  //! \param diffWidth
  //! \param diffHeight
  void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
  {
    osgWidget::Canvas::_resizeImplementation(diffWidth, diffHeight);
    // aktualizacja rozmiaru prostok¹ta z obrazkiem
    double height = getHeight() + diffHeight;
    double width = getWidth() + diffWidth;
    rect->setSize(width - rect->getPadHorizontal(), height - rect->getPadVertical());
    rect->setOrigin( rect->getPadLeft(), rect->getPadBottom() );
    // aktualizacja pozycji labelki
    label->setSize( width - label->getPadHorizontal(), label->getHeight() );
    label->setOrigin( label->getPadLeft(), height - label->getHeight() - label->getPadTop() );

    // resize ramki
    borderLeft->setSize(rect->getPadLeft(), height);
    borderLeft->setOrigin(0, 0);
    borderTop->setSize(width, rect->getPadTop());
    borderTop->setOrigin(0, height - borderTop->getHeight());
    borderRight->setSize(rect->getPadRight(), height);
    borderRight->setOrigin(width - borderRight->getWidth(), 0);
    borderBottom->setSize(width, rect->getPadBottom());
    borderBottom->setOrigin(0, 0);

    // rozmieszczenie guzików
    //decAlpha->setOrigin( width - rect->getPadRight() - 4 * buttonSize, height - rect->getPadTop() - buttonSize );
    //incAlpha->setOrigin( width - rect->getPadRight() - 3 * buttonSize, height - rect->getPadTop() - buttonSize );
    //decSize->setOrigin( width - rect->getPadRight() - 2 * buttonSize, height - rect->getPadTop() - buttonSize );
    //incSize->setOrigin( width - rect->getPadRight() - buttonSize, height - rect->getPadTop() - buttonSize );
  }

  //! Prze³adowanie metody obliczaj¹cej rozmiar.
  osgWidget::Window::Sizes _getWidthImplementation() const
  {
    osgWidget::Window::Sizes result;
    result.current = rect->getWidthTotal();
    result.minimum = rect->getPadHorizontal();
    result.minimum = std::max(0.0f, result.minimum);
    result.current = std::max(result.current, result.minimum);
    return result;
  }

  //! Prze³adowanie metody obliczaj¹cej rozmiar.
  osgWidget::Window::Sizes _getHeightImplementation() const
  {
    osgWidget::Window::Sizes result;
    result.current = rect->getHeightTotal();
    result.minimum = rect->getPadVertical();
    result.minimum = std::max(0.0f, result.minimum);
    result.current = std::max(result.current, result.minimum);
    return result;
  }

  //! Reakcja na naciœniêcie guzika.
  //! \param event
  //bool onButtonPressed(osgWidget::Event& event)
  //{
  //  if ( event.getWidget() == incAlpha ) {
  //    changeAlpha(+0.1f);
  //  } else if ( event.getWidget() == decAlpha ) {
  //    changeAlpha(-0.1f);
  //  } else if ( event.getWidget() == decSize) {
  //    changeScale(-0.1f);
  //  } else if ( event.getWidget() == incSize) {
  //    changeScale(+0.1f);
  //  }
  //  return true;
  //}
};

#endif
