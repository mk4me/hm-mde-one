/********************************************************************
	created:  2010/07/14
	created:  14:7:2010   10:40
	filename: Potentiometer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__POTENTIOMETER_H__
#define __HEADER_GUARD__POTENTIOMETER_H__

#include <osg/Timer>
#include <osgWidget/Widget>

// TODO: przenieœæ do nawiasów <>
#include "PotentiometerModel.h"
#include "NestedNodeCallback.h"
#include "Callback.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Kontrolka potencjometru. Po³¹czenie widoku i kontrolera.
 *  Jego dzia³anie polega na przesuwaniu wspó³rzêdnych tekstury. Wobec tego
 *  musi byæ do tego celu specjalnie przygotowana: musi sk³adaæ siê z kafelek
 *  (tiles):
 *      +----+-- ... --+----+
 *      |    |         |    |
 *      +----+-- ... --+----+
 *  ¯eby efekt obrotu móg³ byæ uzyskany pierwszy i ostatni tile musz¹ byæ
 *  jednakowe. Alternatyw¹ jest tekstura bez "zapêtlenia" oraz taki dobór
 *  parametrów, aby nie musia³o dojœæ do obrotu wymagaj¹cego zapêtlonej
 *  tekstury.
 *  Przy obrocie potencjometru o 2*PI (360 stopni) tekstura ustawia siê na
 *  ostatnim tile'u.
 */
class Potentiometer : public osgWidget::Widget, public PotentiometerModel,
  public EventSource
{
public:
  //! Lista eventów.
  enum EventID {
    //! Zmieni³a siê wartoœæ potencjometru.
    EventIDValueChanged = 1
  };

private:
  //! Aktualizator.
  struct Updater : public osg::Drawable::UpdateCallback
  {
    //! Poprzedni timestamp
    float previous;
    //! Konstruktor zeruj¹cy.
    Updater() : previous(0.0f) {}
    //! Aktualizacja.
    virtual void update(osg::NodeVisitor* nv , osg::Drawable* geom);
  };
  friend struct Updater;
  //! 
  using osgWidget::Widget::Corner;

private:
  //! Czas rozpoczêcia dotyku.
  osg::Timer pushTime;
  //! Miejsce rozpoczêcia dotyku.
  osgWidget::XYCoord pushDelta;
  //! Szerokoœæ tile'i w jednostkach tekstury.
  osgWidget::texcoord_type textureTileWidth;
  //! Parametr mówi¹cy, jak bardzo d³ugoœæ ruchu bêdzie przek³adana na si³ê
  //! przy³o¿on¹ do potencjometru.
  /** Przy³o¿ona si³a = Wektor wodz¹cy myszy * czu³oœæ / (deltaCzasowa * szerokoœæ/2) */
  float sensetivity;
  //! Poprzednia wartoœæ.
  float prevValue;

public:

  META_Object(osgUI, Potentiometer)
  //! 
  //! \param name
  //! \param width
  //! \param height
  Potentiometer(const std::string& name = "", osgWidget::point_type width = 0, 
    osgWidget::point_type height = 0);

  //! Konstruktor kopiuj¹cy.
  //! \param src
  //! \param copyop
  Potentiometer(const Potentiometer& src, const osg::CopyOp& copyop);

  //! \return
  inline float getSensetivity() const
  { 
    return sensetivity;
  }
  //! \param sensetivity
  inline void setSensetivity(float sensetivity) 
  { 
    this->sensetivity = sensetivity; 
  }
  //! \return Szerokoœæ tile'i w jednostkach tekstury.
  inline osgWidget::texcoord_type getTextureTileWidth() const
  { 
    return textureTileWidth;
  }
  //! \param textureTileWidth Szerokoœæ tile'i w jednostkach tekstury.
  inline void setTextureTileWidth(osgWidget::texcoord_type textureTileWidth) 
  { 
    this->textureTileWidth = textureTileWidth; 
  }
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mousePush(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseRelease(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseLeave(double x, double y, const osgWidget::WindowManager* wm);
  //! 
  //! \param x
  //! \param y
  //! \param wm
  virtual bool mouseDrag(double x, double y, const osgWidget::WindowManager* wm);

private:
  //! Reakcja na wykonanie ruchu mysz¹.
  //! \param delta
  void notifyDrag(osgWidget::XYCoord delta);

  //! Ukrycie metody z klasy bazowej.
  //! \param deltaTime Delta.
  void update(double deltaTime);

  //! Pomocnicza funkcja do ustawiania koordynatów tekstury.
  //! \param s
  //! \param corner
  inline void setTexCoordS(osgWidget::texcoord_type s, Corner corner)
  {
    setTexCoord( s, getTexCoord(corner).y(), corner );
  }

  //! Prze³adowanie metody z obserwatora.
  virtual void notify();

  //! Ukrycie metody bazowej.
  using PotentiometerModel::setIsPushed;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__POTENTIOMETER_H__