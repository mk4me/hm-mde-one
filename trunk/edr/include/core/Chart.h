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

namespace osgText 
{
    typedef osg::ref_ptr<Text> TextPtr;
} // namespace osgText

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
    
    //! Labelki dla osi.
    std::vector< osgText::TextPtr > labelsY;
    //! Labelki dla osi x.
    std::vector< osgText::TextPtr > labelsX;

    osgText::TextPtr pointer;

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
    osg::Vec4 labelColor;

    //!
	int gridDensity;
    //! Prototyp dla tekstów.
    osg::ref_ptr<osgText::Text> textPrototype;

public:
	Chart(float x, float y, float width, float height);
	~Chart();

    //! \return
    int getActiveSerieIndex() const;
    //! \param currentSerie
    void setActiveSerie(int currentSerie);
    //!
    const core::ChartSerie* getActiveSerie() const;

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
    //! \return
    osg::Vec4 getLabelColor() const;
    //! \param labelColor
    void setLabelColor(osg::Vec4 labelColor);
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
    void refreshLite();

private:
    void refreshAxis(float z);
    void refreshGrid(float z);
    void refreshLabels(float z);
    void refreshPointer(float z);

    //! Pomocnicza metoda do odœwie¿ania labelek.
    void refreshLabels(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max, float x0, float x1, float y0, float y1, float z, osgText::Text::AlignmentType align );
    bool prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name);
    bool prepareText(osgText::TextPtr& text, bool condition, const char* name);
    osg::ref_ptr<osgText::Text> createText(const osg::Vec3& pos, float size, const std::string& label);
};


#endif