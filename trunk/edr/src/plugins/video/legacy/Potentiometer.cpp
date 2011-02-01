#include "VideoPCH.h"
#undef M_PI
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <osg/TextureRectangle>
#include "Potentiometer.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

void Potentiometer::Updater::update( osg::NodeVisitor* nv , osg::Drawable* geom )
{
  // aktualizacja czasu
  const osg::FrameStamp * f = nv->getFrameStamp();
  float delta = f->getSimulationTime() - previous;
  previous = f->getSimulationTime();

  // aktualizacja potencjometru
  Potentiometer * potentiometer = dynamic_cast<Potentiometer*>(geom);
  if ( potentiometer ) {
    potentiometer->update(delta);
  } else {
    UTILS_ASSERT(false);
  }
}

Potentiometer::Potentiometer(const std::string& name /*= ""*/,
  osgWidget::point_type width /*= 0*/, osgWidget::point_type height /*= 0*/ ):
  osgWidget::Widget(name, width, height),
  PotentiometerModel(),
  textureTileWidth(0.5f),
  sensetivity(1.0f),
  prevValue(0.0f)
{
  setUpdateCallback(new Updater());
  setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
}

Potentiometer::Potentiometer( const Potentiometer& src, const osg::CopyOp& copyop ) :
  osgWidget::Widget(src, copyop),
  PotentiometerModel(src),
  pushTime(src.pushTime),
  pushDelta(src.pushDelta),
  textureTileWidth(src.textureTileWidth),
  sensetivity(src.sensetivity),
  prevValue(src.prevValue)
{
  setUpdateCallback(new Updater());
}

bool Potentiometer::mouseDrag( double x, double y, const osgWidget::WindowManager* wm )
{
  pushDelta += osgWidget::XYCoord(x, y);

  // obracamy zgodnie z ruchem myszki
  x /= getWidth();
  x *= osg::PI * 2;
  //setValue( getValue() + x );

  return false;
}

bool Potentiometer::mousePush( double x, double y, const osgWidget::WindowManager* wm )
{
  pushTime.setStartTick();
  pushDelta.set(0, 0);
  setIsPushed(true);
  return false;
}

bool Potentiometer::mouseRelease( double x, double y, const osgWidget::WindowManager* wm )
{
  if ( isPushed() ) {
    notifyDrag(pushDelta);
  }
  setIsPushed(false);
  return false;
}

bool Potentiometer::mouseLeave( double x, double y, const osgWidget::WindowManager* wm )
{
  if ( isPushed() ) {
    notifyDrag(pushDelta);
  }
  setIsPushed(false);
  return false;
}

void Potentiometer::notifyDrag( osgWidget::XYCoord delta )
{
  float deltaX = delta.x();
  // deltaX /= pushTime.time_s();
  deltaX /= getWidth() / 2.0f;
  deltaX /= pushTime.time_s();
  addMoment( deltaX * sensetivity );
}

void Potentiometer::update( double deltaTime )
{
  PotentiometerModel::update(deltaTime);

  // okre�lenie stanu
  bool absoulteCoords = false;
  osg::Image * image = NULL;
  if ( osg::StateSet* ss = getStateSet() ) {
    // czy mamy tekstur�?
    if (osg::Texture * texture = dynamic_cast<osg::Texture*>(ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE))) {
      image = texture->getImage(0);
      // czy mamy absolutne wsp�rz�dne?
      if ( dynamic_cast<osg::TextureRectangle*>(texture) ) {
        absoulteCoords = true;
      }
    }
  }

#if 0
  // przesuni�cie tekstury
  float delta = getDelta();
  // maksymalna warto��
  float maxS = (absoulteCoords && image) ? image->s() : 1.0f;
  float prevS = getTexCoord(LL).x();
  float s = prevS - delta;

  while ( prevS <= maxS - textureTileWidth && s > maxS - textureTileWidth ) {
    // wracamy do pocz�tku
    s -= maxS - textureTileWidth;
  }
  while ( prevS >= 0.0f && s < 0.0f ) {
    // wracamy do ko�ca
    s += maxS - textureTileWidth;
  }
#else
  // warto�� w radianach...
  float value = -getValue();
  // ... t�umaczona na znormalizowan�
  value /= 2 * M_PI;

  // maksymalna wsp�rz�dna s lewej kraw�dzi
  float maxS = ((absoulteCoords && image) ? image->s() : 1.0f) - textureTileWidth;
  // wsp�rz�dna tekstury jako reszta z dzielenia
  // pami�ta� o odwr�ceniu warto�ci!
  float s = fmodf(value * maxS, maxS);
  if ( s < 0 ) {
    s += maxS;
  }
#endif
  setTexCoordS(s, LL);
  setTexCoordS(s, UL);
  setTexCoordS(s + textureTileWidth, LR);
  setTexCoordS(s + textureTileWidth, UR);
}

void Potentiometer::notify()
{
  if ( getValue() != prevValue ) {
    prevValue = getValue();
    fireEvent(EventIDValueChanged, this, getValue());
  }
  PotentiometerModel::notify();
}
////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////
