/********************************************************************
	created:  2011/02/22
	created:  22:2:2011   8:35
	filename: ChartSerie.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__CHARTSERIE_H__
#define __HEADER_GUARD_CORE__CHARTSERIE_H__

#include <string>
#include <utility>
#include <osg/Geometry>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Klasa bazowa dla serii danych wykres�w.
class ChartSerie : public osg::Geometry
{
protected:
    float x;
    float y;
    float w;
    float h;
    float z;
    bool autoRefresh;
    float verticesPerUnit;
    osg::Vec4 color;

public:
    //! Zeruj�cy konstruktor.
    ChartSerie();

    //! Pusty polimorficzny destruktor.
    virtual ~ChartSerie()
    {}

public:
    //! Generuje wykres na nowo.
    virtual void refresh() = 0;
    //! \return Zakres warto�ci dla osi X.
    virtual std::pair<float, float> getXRange() const = 0;
    //! \return Zakres warto�ci dla osi Y.
    virtual std::pair<float, float> getYRange() const = 0;
    //! \return Jednostka dla osi X.
    virtual const std::string& getXUnit() const = 0;
    //! \return Jednostka dla osi Y.
    virtual const std::string& getYUnit() const = 0;
    //!
    virtual float getValue() const = 0;
    //!
    virtual float getTime() const = 0;

public:
    //! \return D�ugo�� zakresu dla X.
    inline float getXLength() const
    {
        auto range = getXRange();
        return range.second - range.first;
    }

    //! \return D�ugo�� zakresu dla Y.
    inline float getYLength() const
    {
        auto range = getYRange();
        return range.second - range.first;
    }

    //! \return Maksymalna liczba wierzcho�k�w per jednostka miary. Domy�lnie 1.
    inline float getVerticesPerUnit() const
    { 
        return verticesPerUnit;
    }
    //! \return Maksymalna liczba wierzcho�k�w per jednostka miary. Domy�lnie 1.
    void setVerticesPerUnit(float verticesPerUnit);

    //! \return Czy wykres ma si� generowa� na nowo przy ka�dej zmianie property? Domy�lnie false.
    inline bool isAutoRefresh() const
    { 
        return autoRefresh;
    }
    //! \return Czy wykres ma si� generowa� na nowo przy ka�dej zmianie property? Domy�lnie false.
    inline void setAutoRefresh(bool autoRefresh) 
    { 
        this->autoRefresh = autoRefresh; 
    }

    //! \param x
    //! \param y
    //! \param w
    //! \param h
    void setLocation(float x, float y, float w, float h);

    //! \param location
    //! \param size
    void setLocation(osg::Vec2 location, osg::Vec2 size);

    //! \return Pozycja wykresu.
    inline osg::Vec2 getPosition() const
    {
        return osg::Vec2(x, y);
    }
    //! \return Rozmiar wykresu.
    inline osg::Vec2 getSize() const
    {
        return osg::Vec2(w, h);
    }

    //! \return Wsp�rz�dna z wykresu. Domy�lnie 0.
    inline float getZ() const
    { 
        return z;
    }
    //! \return Wsp�rz�dna z wykresu. Domy�lnie 0.
    void setZ( float z );

    //! \return Kolor wykresu.
    osg::Vec4 getColor() const
    {
        return color;
    }

    //! \param color Kolor wykresu.
    void setColor(const osg::Vec4& color);
    
protected:
    //! T� metod� powinny wywo�ywa� implementacje po modyfikacji kt�rej�
    //! z propert.
    inline void tryRefresh()
    {
        if ( autoRefresh ) {
            refresh();
        }
    }
};

typedef osg::ref_ptr<ChartSerie> ChartSeriePtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD_CORE__CHARTSERIE_H__