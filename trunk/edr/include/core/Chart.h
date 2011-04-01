/********************************************************************
created:  2010/11/17
filename: Chart.h
author:	  Michal Szafarski

purpose:  Klasa ta zarzadza calym wykresem
*********************************************************************/
#ifndef __HEADER_GUARD__Chart_H__
#define __HEADER_GUARD__Chart_H__

#include <osg/Geode>
#include <osg/Geometry>
#include <core/C3DChannels.h>
#include <core/ChartSerie.h>

////////////////////////////////////////////////////////////////////////////////
namespace osg {
////////////////////////////////////////////////////////////////////////////////

    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

////////////////////////////////////////////////////////////////////////////////
} // namespace osg
////////////////////////////////////////////////////////////////////////////////

class Chart : public osg::Group
{
private:
    //! Serie danych wykresu.
    std::vector<core::ChartSeriePtr> series;
    //! Bie¿¹ca seria.
    int activeSerie;

    osg::GeodePtr background;
    osg::GeodePtr foreground;

    bool showGridX;
    bool showGridY;
    osg::GeometryPtr grid;

    bool showAxisX;
    bool showAxisY;
    osg::GeometryPtr axises;
    float dashWidth;
    

    float margin;

    //! Wspó³rzêdna bazowa wykresu.
    float z;
    //! Zakres, w ramach którego mo¿na pozycjonowaæ elementy.
    float zRange;
    //!
	float x;
    //!
    float y;
    //!
    float width;
    //!
    float height;
    //!
	osg::Vec4 gridColor;
    //!
	osg::Vec4 axisColor;
    //!
	int gridDensity;

public:
	Chart(float x, float y, float width, float height);
	~Chart();

    //! \return
    int getActiveSerie() const;
    //! \param currentSerie
    void setActiveSerie(int currentSerie);

    //! Dodaje kana³ do wykresu.
    void addChannel( const core::ScalarChannelConstPtr& channel, osg::Vec4 color );
    //! Usuwa wszystkie wykresy.
    void removeAllChannels();
    //! \return Liczba kana³ów.
    int getNumChannels() const;

	//! zwraca odstep ramki od wykresu
	float getMargin() const;
	//! ustawia odstep ramki od wykres
	void setMargin(float margin);
    //! \return
    bool isShowingGridX() const;
    //! \param showGridX
    void setShowGridX(bool showGridX);
    //! \return
    bool isShowingGridY() const;
    //! \param showGridY
    void setShowGridY(bool showGridY);
    //! \return
    bool isShowingAxisX() const;
    //! \param showAxisX
    void setShowAxisX(bool showAxisX);
    //! \return
    bool isShowingAxisY() const;
    //! \param showAxisY
    void setShowAxisY(bool showAxisY);

	//! pobiera gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	int getGridDensity();
	//! ustawia gestosc siatki. Ile pixeli szerokosci ma jedna kratka
	void setGridDensity(int gridDensity);
	//! pobiera kolor siatki w rgba
	osg::Vec4 getGridColor();
	//! ustawia kolor siatki w rgba
	void setGridColor(osg::Vec4 gridColor);
	//! pobiera kolor wykresu w rgba
	osg::Vec4 getAxisColor();
	//! ustawia kolor wykresu w rgba
	void setAxisColor(osg::Vec4 color);
	//! ustawia lokacje wykresu
	void setLocation(float x, float y, float width, float height);
	//! pobiera lokacje wykresu
	osg::Vec4 getLocation();

    //! \return D³ugoœæ.
    float getLength() const;

    //! \return
    float getZ() const;
    //! \param z
    void setZ(float z);
    //! \return
    float getZRange() const;
    //! \param zRange
    void setZRange(float zRange);
    //! \return
    float getDashWidth() const;
    //! \param dashWidth
    void setDashWidth(float dashWidth);

    //! Odœwie¿a wykres.
    void refresh();

private:
    void refreshAxis(float z);
    void refreshGrid(float z);
    bool prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name);
};


#endif