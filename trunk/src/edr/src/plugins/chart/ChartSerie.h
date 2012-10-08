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

//! Klasa bazowa dla serii danych wykresów.
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
    //! Zerujący konstruktor.
    ChartSerie();

    //! Pusty polimorficzny destruktor.
    virtual ~ChartSerie()
    {}

public:
    //! Generuje wykres na nowo.
    virtual void refresh() = 0;
    //! \return Zakres wartości dla osi X.
    virtual std::pair<float, float> getXRange() const = 0;
    //! \return Zakres wartości dla osi Y.
    virtual std::pair<float, float> getYRange() const = 0;
    //! \return Jednostka dla osi X.
    virtual const std::string& getXUnit() const = 0;
    //! \return Jednostka dla osi Y.
    virtual const std::string& getYUnit() const = 0;
    //!
    virtual float getValue() const = 0;
    //!
    virtual float getTime() const = 0;
    //!
    virtual bool isActive() const = 0;
    //!
    virtual void setActive(bool active) = 0;

public:
    //! \return Długość zakresu dla X.
    inline float getXLength() const
    {
        auto range = getXRange();
        return range.second - range.first;
    }

    //! \return Długość zakresu dla Y.
    inline float getYLength() const
    {
        auto range = getYRange();
        return range.second - range.first;
    }

    //! \return Maksymalna liczba wierzchołków per jednostka miary. Domyślnie 1.
    inline float getVerticesPerUnit() const
    { 
        return verticesPerUnit;
    }
    //! \return Maksymalna liczba wierzchołków per jednostka miary. Domyślnie 1.
    void setVerticesPerUnit(float verticesPerUnit);

    //! \return Czy wykres ma się generować na nowo przy każdej zmianie property? Domyślnie false.
    inline bool isAutoRefresh() const
    { 
        return autoRefresh;
    }
    //! \return Czy wykres ma się generować na nowo przy każdej zmianie property? Domyślnie false.
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

    //! \return Współrzędna z wykresu. Domyślnie 0.
    inline float getZ() const
    { 
        return z;
    }
    //! \return Współrzędna z wykresu. Domyślnie 0.
    void setZ( float z );

    //! \return Kolor wykresu.
    osg::Vec4 getColor() const
    {
        return color;
    }

    //! \param color Kolor wykresu.
    void setColor(const osg::Vec4& color);
    
protected:
    //! Tę metodę powinny wywoływać implementacje po modyfikacji którejś
    //! z propert.
    inline void tryRefresh()
    {
        if ( autoRefresh ) {
            refresh();
        }
    }
};

typedef osg::ref_ptr<ChartSerie> ChartSeriePtr;


#endif  // __HEADER_GUARD_CORE__CHARTSERIE_H__
