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

// TODO: przenie�� do nawias�w <>
#include "PotentiometerModel.h"
#include "NestedNodeCallback.h"
#include "Callback.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Kontrolka potencjometru. Po��czenie widoku i kontrolera.
 *  Jego dzia�anie polega na przesuwaniu wsp�rz�dnych tekstury. Wobec tego
 *  musi by� do tego celu specjalnie przygotowana: musi sk�ada� si� z kafelek
 *  (tiles):
 *      +----+-- ... --+----+
 *      |    |         |    |
 *      +----+-- ... --+----+
 *  �eby efekt obrotu m�g� by� uzyskany pierwszy i ostatni tile musz� by�
 *  jednakowe. Alternatyw� jest tekstura bez "zap�tlenia" oraz taki dob�r
 *  parametr�w, aby nie musia�o doj�� do obrotu wymagaj�cego zap�tlonej
 *  tekstury.
 *  Przy obrocie potencjometru o 2*PI (360 stopni) tekstura ustawia si� na
 *  ostatnim tile'u.
 */
class Potentiometer : public osgWidget::Widget, public PotentiometerModel,
  public EventSource
{
public:
  //! Lista event�w.
  enum EventID {
    //! Zmieni�a si� warto�� potencjometru.
    EventIDValueChanged = 1
  };

private:
  //! Aktualizator.
  struct Updater : public osg::Drawable::UpdateCallback
  {
    //! Poprzedni timestamp
    float previous;
    //! Konstruktor zeruj�cy.
    Updater() : previous(0.0f) {}
    //! Aktualizacja.
    virtual void update(osg::NodeVisitor* nv , osg::Drawable* geom);
  };
  friend struct Updater;
  //! 
  using osgWidget::Widget::Corner;

private:
  //! Czas rozpocz�cia dotyku.
  osg::Timer pushTime;
  //! Miejsce rozpocz�cia dotyku.
  osgWidget::XYCoord pushDelta;
  //! Szeroko�� tile'i w jednostkach tekstury.
  osgWidget::texcoord_type textureTileWidth;
  //! Parametr m�wi�cy, jak bardzo d�ugo�� ruchu b�dzie przek�adana na si��
  //! przy�o�on� do potencjometru.
  /** Przy�o�ona si�a = Wektor wodz�cy myszy * czu�o�� / (deltaCzasowa * szeroko��/2) */
  float sensetivity;
  //! Poprzednia warto��.
  float prevValue;

public:

  META_Object(osgUI, Potentiometer)
  //! 
  //! \param name
  //! \param width
  //! \param height
  Potentiometer(const std::string& name = "", osgWidget::point_type width = 0, 
    osgWidget::point_type height = 0);

  //! Konstruktor kopiuj�cy.
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
  //! \return Szeroko�� tile'i w jednostkach tekstury.
  inline osgWidget::texcoord_type getTextureTileWidth() const
  { 
    return textureTileWidth;
  }
  //! \param textureTileWidth Szeroko�� tile'i w jednostkach tekstury.
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
  //! Reakcja na wykonanie ruchu mysz�.
  //! \param delta
  void notifyDrag(osgWidget::XYCoord delta);

  //! Ukrycie metody z klasy bazowej.
  //! \param deltaTime Delta.
  void update(double deltaTime);

  //! Pomocnicza funkcja do ustawiania koordynat�w tekstury.
  //! \param s
  //! \param corner
  inline void setTexCoordS(osgWidget::texcoord_type s, Corner corner)
  {
    setTexCoord( s, getTexCoord(corner).y(), corner );
  }

  //! Prze�adowanie metody z obserwatora.
  virtual void notify();

  //! Ukrycie metody bazowej.
  using PotentiometerModel::setIsPushed;
};


////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__POTENTIOMETER_H__