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
  border[BorderLeft] = new osgWidget::Widget("borderLeft", 1, 1);
  border[BorderLeft]->setColor(1, 1, 1, 1);
  border[BorderLeft]->setLayer(osgWidget::Widget::LAYER_MIDDLE, 1);
  border[BorderTop] = osg::clone(border[BorderLeft].get(), "borderTop", osg::CopyOp::DEEP_COPY_ALL);
  border[BorderRight] = osg::clone(border[BorderLeft].get(), "borderRight", osg::CopyOp::DEEP_COPY_ALL);
  border[BorderBottom] = osg::clone(border[BorderLeft].get(), "borderBottom", osg::CopyOp::DEEP_COPY_ALL);

  // dodajemy widgety
  addWidget(rect, 0, 0);
  addWidget(label, 0, 0);
  addWidget(border[BorderLeft], 0, 0);
  addWidget(border[BorderTop], 0, 0);
  addWidget(border[BorderRight], 0, 0);
  addWidget(border[BorderBottom], 0, 0);
}

ImageCanvas::ImageCanvas( const ImageCanvas& canvas, const osg::CopyOp& copyop ):
  osgWidget::Canvas(canvas, copyop),
  keepImageRatio(canvas.keepImageRatio)
{
  rect = getByName("image");
  label = dynamic_cast<osgWidget::Label*>(getByName("label"));
  border[BorderLeft] = getByName("borderLeft");
  border[BorderTop] = getByName("borderTop");
  border[BorderRight] = getByName("borderRight");
  border[BorderBottom] = getByName("borderBottom");

  // ustawiamy zmienn¹ image
  osg::StateSet* ss = rect->getStateSet();
  if (ss) {
    osg::Texture * texture = dynamic_cast<osg::Texture*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
    if ( texture ) {
      image = texture->getImage(0);
    }
  }
}

ImageCanvas::~ImageCanvas()
{

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
    for (size_t i = 0; i < utils::length(border); ++i) {
        border[i]->setColor(color);
    }
}

void ImageCanvas::_resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
{
  osgWidget::Canvas::_resizeImplementation(diffWidth, diffHeight);
  // aktualizacja rozmiaru prostok¹ta z obrazkiem
  float height = getHeight() + diffHeight;
  float width = getWidth() + diffWidth;

  if ( getKeepImageRatio() ) {
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
  border[BorderLeft]->setSize(rect->getPadLeft(), height);
  border[BorderLeft]->setOrigin(0, 0);
  border[BorderTop]->setSize(width, rect->getPadTop());
  border[BorderTop]->setOrigin(0, height - border[BorderTop]->getHeight());
  border[BorderRight]->setSize(rect->getPadRight(), height);
  border[BorderRight]->setOrigin(width - border[BorderRight]->getWidth(), 0);
  border[BorderBottom]->setSize(width, rect->getPadBottom());
  border[BorderBottom]->setOrigin(0, 0);
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
  if ( value && !getShowLabel() ) {
    addWidget(label, 0, 0);
  } else if ( !value && getShowLabel() ) {
    removeWidget(label);
  }
}

void ImageCanvas::setShowBorder( bool value )
{
    if (value && !getShowLabel() ) {
        for (size_t i = 0; i < utils::length(border); ++i) {
            addWidget(border[i], 0, 0);
        }
    } else {
        for (size_t i = 0; i < utils::length(border); ++i) {
            removeWidget(border[i]);
        }
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