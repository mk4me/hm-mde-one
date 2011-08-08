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
#include <plugins/c3d/C3DChannels.h>
#include "ChartSerie.h"

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
public:
    typedef std::vector<ChartSeriePtr> Series;
    typedef Series::iterator iterator;
    typedef Series::const_iterator const_iterator;

private:
    //! Serie danych wykresu.
    std::vector<ChartSeriePtr> series;
    //! Bie¿¹ca seria.
    int activeSerieIndex;

    //! Geode.
    osg::GeodePtr geode;

    //! Grid.
    osg::GeometryPtr grid;
    //! Szerokoœæ kreski siatki.
    float dashWidth;
    //! Czy pokazywaæ poziome linie?
    bool showGridX;
    //! Czy pokazywaæ pionowe linie?
    bool showGridY;
    //! Kolor siatki.
    osg::Vec4 gridColor;

    //! Osie.
    osg::GeometryPtr axises;
    //! Czy pokazywaæ oœ X?
    bool showAxisX;
    //! Czy pokazywaæ oœ Y?
    bool showAxisY;
    //! Kolor osi.
    osg::Vec4 axisColor;

    //! Labelki dla osi.
    std::vector< osgText::TextPtr > labelsY;
    //! Labelki dla osi x.
    std::vector< osgText::TextPtr > labelsX;
    //! WskaŸnik bie¿¹cego czasu.
    osgText::TextPtr cursorText;
    //! WskaŸnik bie¿¹cego czasu.
    osg::GeometryPtr cursor;

    //! Wspó³rzêdna bazowa wykresu.
    float z;
    //! Zakres, w ramach którego mo¿na pozycjonowaæ elementy.
    float zRange;
    //! Wspó³rzêdna x.
    float x;
    //! Wspó³rzêdna y.
    float y;
    //! Szerokoœæ.
    float w;
    //! Wysokoœæ.
    float h;
    //! Margines.
    float margin;

    //! Czy wykres wymaga ponownego odrysowania?
    bool dirty;
    //! Czy wykres sam siê odœwie¿a?
    bool autoRefresh;

    //!
    bool normalized;

    //!
	int gridDensity;
    //! Prototyp dla tekstów.
    osg::ref_ptr<osgText::Text> textPrototype;

public:
    //! Konstruktor zeruj¹cy.
	Chart();
    //! Pusty polimorficzny destruktor.
	virtual ~Chart();

public:
    //!
    iterator begin()
    {
        return series.begin();
    }
    //!
    iterator end()
    {
        return series.end();
    }
    //!
    const_iterator begin() const
    {
        return series.begin();
    }
    //!
    const_iterator end() const
    {
        return series.end();
    }

    //! Dodaje kana³ do wykres.ug
    bool addSerie( const ChartSeriePtr& serie );

    //! Usuniêcie kana³ z wykres.ug
    bool removeSerie( const ChartSeriePtr& serie );

    //! Usuwa wszystkie wykresy.
    void removeAllSeries();
    //! \return Liczba kana³ów.
    int getNumSeries() const;

    //! \return Indeks aktywnego kana³u b¹dŸ -1.
    int getActiveSerieIndex() const;
    //! \param currentSerie Indeks aktywnego kana³u.
    void setActiveSerie(int currentSerie);
    //! \return Aktywny kana³ b¹dŸ nullptr.
    const ChartSerie* getActiveSerie() const;

    //! \return Czy wykres wymaga odœwie¿enia?
    inline bool isDirty() const
    { 
        return dirty;
    }
    //! \return Czy wykres sam siê odœwie¿a przy zmianie?
    inline bool getAutoRefresh() const
    { 
        return autoRefresh;
    }
    //! \param autoRefresh
    void setAutoRefresh(bool autoRefresh);

    //! \param x Lokalizacja wykresu.
    //! \param y Lokalizacja wykresu.
    //! \param width Lokalizacja wykresu.
    //! \param height Lokalizacja wykresu.
    void setLocation(float x, float y, float width, float height);
    //! \param loc Lokalizacja wykresu.
    void setLocation(osg::Vec4 loc);
    //! \return Lokalizacja wykresu.
    osg::Vec4 getLocation() const;
    //! \return Obszar, gdzie mo¿na rysowaæ.
    osg::Vec4 getClientLocation() const;
    //! \param x Wspó³rzêdna obszaru, gdzie mo¿na rysowaæ.
    //! \param y Wspó³rzêdna obszaru, gdzie mo¿na rysowaæ.
    //! \param w Wspó³rzêdna obszaru, gdzie mo¿na rysowaæ.
    //! \param h Wspó³rzêdna obszaru, gdzie mo¿na rysowaæ.
    void getClientLocation(float& x, float& y, float& w, float& h) const;

    //! \return
    float getZ() const;
    //! \param z
    void setZ(float z);
    //! \return
    float getZRange() const;
    //! \param zRange
    void setZRange(float zRange);

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
    //! pobiera gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    int getGridDensity();
    //! ustawia gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    void setGridDensity(int gridDensity);
    //! pobiera kolor siatki w rgba
    osg::Vec4 getGridColor();
    //! ustawia kolor siatki w rgba
    void setGridColor(osg::Vec4 gridColor);
    //! \return
    float getGridDashWidth() const;
    //! \param dashWidth
    void setGridDashWidth(float dashWidth);

    //! \return
    bool isShowingAxisX() const;
    //! \param showAxisX
    void setShowAxisX(bool showAxisX);
    //! \return
    bool isShowingAxisY() const;
    //! \param showAxisY
    void setShowAxisY(bool showAxisY);
	//! pobiera kolor wykresu w rgba
	osg::Vec4 getAxisColor();
	//! ustawia kolor wykresu w rgba
	void setAxisColor(osg::Vec4 color);

    //! \return Prototyp labelek.
    const osgText::Text* getLabelPrototype() const
    {
        return textPrototype.get();
    }
    //! \param prototype Prototyp labelek. Obiekt nie jest kopiowany. Gdy wprowadza siê zmiany w labelce trzeba
    //! koniecznie wywo³aæ jeszcze raz setLabelPrototype, bo inaczej nie bêd¹ odœwie¿one. Mo¿na równie¿ sklonowaæ
    //! tekst w parametrze.
    void setLabelPrototype(osgText::Text* prototype);

    //! \return
    bool isNormalized() const;
    //! \param normalized
    void setNormalized(bool normalized);

    //! Odœwie¿a wykres.
    void refreshAll();
    void refreshCursor();

private:
    void setDirty(bool dirty = true);

    void refreshAxis(float z);
    void refreshGrid(float z);
    void refreshLabels(float z);
    void refreshCursor(float z);

    //! Pomocnicza metoda do odœwie¿ania labelek.
    void refreshLabels(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max, float x0, float x1, float y0, float y1, float z, osgText::Text::AlignmentType align );
    bool prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name);
    bool prepareText(osgText::TextPtr& text, bool condition, const char* name);

private:
    //! \param pos Pozycja.
    //! \param size Rozmiar.
    //! \param label Tekst.
    osg::ref_ptr<osgText::Text> createText(const osg::Vec3& pos, float size, const std::string& label);

    //! Formatuje strumieñ tak, aby dobrze formatowa³ liczby zmiennoprzecinkowe.
    //! \param stream
    //! \param decimalsToShow Liczba dodatkowych miejsc zmiennoprzecinkowych od pokazania. Gdy ró¿nica miêdzy
    //!                       max a min jest rzêdu 10^n, to wyswietlone zostan¹ cyfry z rzêdów 10^n-decimalsToShow
    //! \param max
    //! \param min
    void formatStream(std::ostringstream& stream, float min, float max, int decimalsToShow = 2 );
};

#endif