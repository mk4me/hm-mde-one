/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:13
	filename: 	Scales.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__SCALES_H__
#define HEADER_GUARD_NEW_CHART__SCALES_H__

#include <limits>

//! Prosta klasa, u�atwiaj�ca zarz�dza� ekstremami wykresu
class Scales
{
public:
    //! Konstruktor, zostawia obiekt niezainicjalizowany
    Scales() : 
	  xMin((std::numeric_limits<float>::max)()),
      xMax((std::numeric_limits<float>::min)()),
      yMin((std::numeric_limits<float>::max)()),
      yMax((std::numeric_limits<float>::min)()),
      initialized(false) 
    {}

    //! Konstruktor, inicjalizuje obiekt
    //! \param xMin minimalny argument na wykresie
    //! \param xMax maksymalny argument na wykresie
    //! \param yMin warto�� minimalna na wykresie
    //! \param yMax warto�� maksymalna na wykresie
    Scales(float xMin, float xMax, float yMin, float yMax) :
      xMin(xMin),
      xMax(xMax),
      yMin(yMin),
      yMax(yMax),
      initialized(true) 
    {}

public:
    //! 
    float getYMax() const { return yMax; }
    //! 
    float getYMin() const { return yMin; }
    //! 
    float getXMax() const { return xMax; }
    //! 
    float getXMin() const { return xMin; }
    //! \return czy obiekt ju� zosta� zainicjalizowany poprawnymi danymi
    bool isInitialized() const { return initialized; }

public:
    //! resetowanie ustawien, zostawia obiekt niezainicjalizowany
    void clear() { initialized = false; }
    //! Laczy obiekt z drugim obiektem tego samego typu, w razie potrzeby zmieniane s� ekstrema
    //! \param scales drugi obiekt z ekstremami
    void merge(const Scales& scales);

private:
    //! minimalny argument na wykresie
    float xMin;
    //! maksymalny argument na wykresie
    float xMax;
    //! warto�� minimalna na wykresie
    float yMin;
    //! warto�� maksymalna na wykresie
    float yMax;
    //! flaga okresla, czy obiekt ju� zosta� zainicjalizowany poprawnymi danymi
    bool initialized;
};



#endif
