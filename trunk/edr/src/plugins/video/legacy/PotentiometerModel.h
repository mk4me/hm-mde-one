/********************************************************************
	created:  2010/07/12
	created:  12:7:2010   14:27
	filename: PotentiometerModel.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__POTENTIOMETERMODEL_H__
#define __HEADER_GUARD__POTENTIOMETERMODEL_H__

#include <utils/ObserverPattern.h>

struct PotentiometerModelData
{
  //! Moment bezw³adnoœci. Zastêpuje promieñ i masê.
  /** Moment bezw³adnoœci walca to 0.5 * m * r^2 */
  float momentOfInteria;
  //! Ostatnia delta k¹tu obrotu. W radianach.
  float delta;
  //! Przy³o¿ona si³a.
  float forcedMoment;
  //! Moment tarcia powietrza.
  float airFriction;
  //! Moment tarcia dotyku.
  float clickFriction;
  //! Prêdkoœæ k¹towa.
  float angularVelocity;
  //! Czy jest dotykany?
  bool pushed;
  //! Maksymalna wartoœæ obrotu (radiany).
  float maxValue;
  //! Minimalna wartoœæ obrotu (radiany).
  float minValue;
  //! Bie¿¹ca wartoœæ (radiany).
  float value;
};

/**
 *	
 */
class PotentiometerModel : public utils::Observable<PotentiometerModel>, private PotentiometerModelData
{
public:
  //! Konstruktor zeruj¹cy.
  PotentiometerModel();
  //! Konstruktor kopiuj¹cy.
  PotentiometerModel(const PotentiometerModel& src);

  //! Aktualizacja modelu fizycznego.
  //! \param deltaTime Ile czasu up³ynê³o.
  void update(float deltaTime);

  //! \return Delta obrotu (radiany).
  inline float getDelta() const
  { 
    return delta;
  }
  //! \return
  inline float getMoment() const
  { 
    return forcedMoment;
  }
  //! \param moment
  inline void setMoment(float moment) 
  { 
    this->forcedMoment = moment; 
    notify();
  }
  //! Przyk³ada si³ê (impuls) do modelu.
  /**
   *	Stosujemy uproszczony model. Poniewa¿ zak³adadamy, ¿e potencjometr ma promieñ równy 1
   *  oraz si³e przyk³ada siê zawsze do zewnêtrzych œcianek wektorem prostopad³ym do osi symetrii
   *  forcedMoment obrotowy staje siê równy sumie przy³o¿onych si³.
   *  \param magnitude D³ugoœæ wektora si³y.
   */
  inline void addMoment(float magnitude)
  {
    this->forcedMoment += magnitude;
    notify();
  }
  //! \return
  inline bool isPushed() const
  { 
    return pushed;
  }
  //! \param isTouched
  inline void setIsPushed(bool pushed) 
  { 
    this->pushed = pushed; 
    notify();
  }
  //! \return
  inline float getMomentOfInteria() const
  { 
    return momentOfInteria;
  }
  //! \param momentOfInteria
  inline void setMomentOfInteria(float momentOfInteria) 
  { 
    this->momentOfInteria = momentOfInteria; 
    notify();
  }
  //! \return
  inline float getAirFriction() const
  { 
    return airFriction;
  }
  //! \param airFriction
  inline void setAirFriction(float airFriction) 
  { 
    this->airFriction = airFriction; 
    notify();
  }
  //! \return
  inline float getClickFriction() const
  { 
    return clickFriction;
  }
  //! \param clickFriction
  inline void setClickFriction(float clickFriction) 
  { 
    this->clickFriction = clickFriction; 
    notify();
  }
  //! \return
  inline float getMaxValue() const
  { 
    return maxValue;
  }
  //! \param maxValue
  inline void setMaxValue(float maxValue) 
  { 
    this->maxValue = maxValue; 
    notify();
  }
  //! \return
  inline float getValue() const
  { 
    return value;
  }
  //! Do ustawienia wartoœci z zewn¹rz. Zeruje model fizyczny.
  //! \param value
  inline void setValue(float value) 
  { 
    delta = value - this->value;
    this->value = value;
    notify();
  }
  //! \return
  inline float getMinValue() const
  { 
    return minValue;
  }
  //! \param minValue
  inline void setMinValue(float minValue) 
  { 
    this->minValue = minValue; 
    notify();
  }
  //! \return
  inline float getNormalizedValue() const
  { 
    return (value - minValue) / (maxValue - minValue);
  }
  //! \param value
  inline void setNormalizedValue(float value) 
  { 
    setValue((1 - value) * minValue + value * maxValue);
  }
  //! Zeruje fizykê.
  inline void zeroPhysics()
  {
    angularVelocity = 0.0f;
  }
};


#endif  // __HEADER_GUARD__POTENTIOMETERMODEL_H__