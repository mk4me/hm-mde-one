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
#include <osgText/Text>
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
    float textWidth(const Text & text);
    float textHeight(const Text & text);
} // namespace osgText

class Chart : public osg::Group
{
public:

    enum TitlePosition {
        TOP_GRID_CENTER,    //nad gridem wy�rodkowany
        TOP_CHART_CENTER,   //nad ca�ym wykresem wy�rodkowany
        BOTTOM_GRID_CENTER,
        BOTTOM_CHART_CENTER
    };

public:
    typedef std::vector<ChartSeriePtr> Series;
    typedef Series::iterator iterator;
    typedef Series::const_iterator const_iterator;

private:
    //! Serie danych wykresu.
    std::vector<ChartSeriePtr> series;
    //! Bie��ca seria.
    int activeSerieIndex;

    //! Geode.
    osg::GeodePtr geode;

    //! Ramka
    osg::GeometryPtr frame;
    //! Szeroko�� kreski ramki
    float frameWidth;
    //! Czy pokazywa� ramk�
    bool showFrame;
    //! Kolor ramki
    osg::Vec4 frameColor;

    //! Grid.
    osg::GeometryPtr grid;
    //! Szeroko�� kreski siatki.
    float dashWidth;
    //! Czy pokazywa� poziome linie?
    bool showGridX;
    //! Czy pokazywa� pionowe linie?
    bool showGridY;
    //! Kolor siatki.
    osg::Vec4 gridColor;

    //! Osie.
    osg::GeometryPtr axises;
    //! Czy pokazywa� o� X?
    bool showAxisX;
    //! Czy o� X ma by� do �rodka czy na zewn�trz?
    bool axisXInside;
    //! Czy pokazywa� o� Y?
    bool showAxisY;
    //! Czy o� Y ma by� do �rodka czy na zewn�trz?
    bool axisYInside;

    //! Kolor osi
    osg::Vec4 axisesColor;

    //! Labelki dla osi.
    std::vector< osgText::TextPtr > labelsY;
    //! Labelki dla osi x.
    std::vector< osgText::TextPtr > labelsX;
    //! Offset pomi�dzy labelkami i osiami
    float labelsToAxisesOffset;
    //! Wska�nik bie��cego czasu.
    osgText::TextPtr cursorText;
    //! Kolor kursora
    osg::Vec4 cursorColor;
    //! Wska�nik bie��cego czasu.
    osg::GeometryPtr cursor;
    //! Czy pokazywa� czas w kursorze
    bool showTimeInCursor;
    //! Czy pokazywac jednostki w kursorze
    bool showUnitsInCursor;
    //! Czy pokazywa� dodatkowe t�o kursora
    bool showCursorBackground;
    //! Kolor t�a kursora
    osg::Vec4 cursorBackgroundColor;
    //! T�o kursora
    osg::GeometryPtr cursorBackground;

    //! Jednostki osi X
    osgText::TextPtr xAxisUnitsLabel;
    //! Czy jednostki osobno i jest opis osi (czy text dodana do sceny)
    bool xUnitLabelVisible;
    //! Pokazuj jednostki osi X
    bool showXUnits;
    //! Jednostki osi Y
    osgText::TextPtr yAxisUnitsLabel;
    //! Czy jednostki osobno i jest opis osi (czy text dodana do sceny)
    bool yUnitLabelVisible;
    //! Pokazuj jednostki osi Y
    bool showYUnits;
    //! Czy jednostki osobno od warto�ci
    bool xUnitsSeparated;
    //! Czy jednostki osobno od warto�ci
    bool yUnitsSeparated;
    //! Tytu� wykresu
    osgText::TextPtr titleLabel;
    //! Czy pokazywac tytu�
    bool showTitle;
    //! Czy tutu� widoczny - klient chcia� go zobaczy� i ustawi� mu jaki� tekst (czy text dodany do sceny)
    bool titleVisible;
    //! Pozycja tytu�u
    TitlePosition titlePosition;


    //! Wsp�rz�dna bazowa wykresu.
    float z;
    //! Zakres, w ramach kt�rego mo�na pozycjonowa� elementy.
    float zRange;
    //! Wsp�rz�dna x.
    float x;
    //! Wsp�rz�dna y.
    float y;
    //! Szeroko��.
    float w;
    //! Wysoko��.
    float h;
    //! Margines.
    float margin;

    //! Pozycja grida dla wykres�w (x, y, w, h)
    osg::Vec4 gridPosition;

    //! Czy wykres wymaga ponownego odrysowania?
    bool dirty;
    //! Czy wykres sam si� od�wie�a?
    bool autoRefresh;

    //!
    bool normalized;

    //!
	int gridDensity;
    //! Prototyp dla tekst�w.
    osg::ref_ptr<osgText::Text> textPrototype;

public:
    //! Konstruktor zeruj�cy.
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

    //! Dodaje kana� do wykres.ug
    bool addSerie( const ChartSeriePtr& serie );

    //! Usuni�cie kana� z wykres.ug
    bool removeSerie( const ChartSeriePtr& serie );

    //! Usuwa wszystkie wykresy.
    void removeAllSeries();
    //! \return Liczba kana��w.
    int getNumSeries() const;

    //! \return Indeks aktywnego kana�u b�d� -1.
    int getActiveSerieIndex() const;
    //! \param currentSerie Indeks aktywnego kana�u.
    void setActiveSerie(int currentSerie);
    //! \return Aktywny kana� b�d� nullptr.
    const ChartSerie* getActiveSerie() const;

    //! \return Czy wykres wymaga od�wie�enia?
    inline bool isDirty() const
    { 
        return dirty;
    }
    //! \return Czy wykres sam si� od�wie�a przy zmianie?
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
    //! \return Obszar, gdzie mo�na rysowa�.
    osg::Vec4 getClientLocation() const;
    //! \param x Wsp�rz�dna obszaru, gdzie mo�na rysowa�.
    //! \param y Wsp�rz�dna obszaru, gdzie mo�na rysowa�.
    //! \param w Wsp�rz�dna obszaru, gdzie mo�na rysowa�.
    //! \param h Wsp�rz�dna obszaru, gdzie mo�na rysowa�.
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
    bool isShowingFrame() const;
    //! \param showFrame
    void setShowFrame(bool showFrame);
    //! \return
    bool isShowingGridX() const;
    //! \param showGridX
    void setShowGridX(bool showGridX);
    //! \return
    bool isShowingGridY() const;
    //! \param showGridY
    void setShowGridY(bool showGridY);
    //! pobiera gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    int getGridDensity() const;
    //! ustawia gestosc siatki. Ile pixeli szerokosci ma jedna kratka
    void setGridDensity(int gridDensity);
    //! \return
    osg::Vec4 getFrameColor() const;
    //! \param frameColor
    void setFrameColor(osg::Vec4 frameColor);
    //! pobiera kolor siatki w rgba
    osg::Vec4 getGridColor() const;
    //! ustawia kolor siatki w rgba
    void setGridColor(osg::Vec4 gridColor);
    //! \return
    float getFrameWidth() const;
    //! \param frameWidth
    void setFrameWidth(float frameWidth);
    //! \return
    float getGridDashWidth() const;
    //! \param dashWidth
    void setGridDashWidth(float dashWidth);

    //! \return
    bool isShowingAxisX() const;
    //! \param showAxisX
    void setShowAxisX(bool showAxisX);
    //! \return
    bool isAxisXInside() const;
    //! \param axisXInside
    void setAxisXInside(bool axisXInside);
    //! \return
    bool isShowingAxisY() const;
    //! \param showAxisY
    void setShowAxisY(bool showAxisY);
    //! \return
    bool isAxisYInside() const;
    //! \param axisXInside
    void setAxisYInside(bool axisYInside);

    //! Pobiera kolor wykresu w rgba
    osg::Vec4 getAxisesColor() const;
    //! Ustawia kolor wykresu w rgba
    void setAxisesColor(osg::Vec4 color);

    //! Pobiera kolor kursora w rgba
    osg::Vec4 getCursorColor() const;
    //! Ustawia kolor kursora w rgba
    void setCursorColor(osg::Vec4 color);

    //! \return Czy pokazywa� tytu�
    bool isShowingTitle() const;
    //! \param, showTitle Czy pokazywa� tytu�
    void setShowTitle(bool showTitle);

    //! \return Tytu� wykresu
    std::string getTitleText() const;
    //! \param titleText Tytu� wykresu
    void setTitleText(const std::string & titleText);

    int getTitleTextSize() const;
    void setTitleTextSize(int titleTextSize);

    TitlePosition getTitlePosition() const;
    void setTitlePosition(TitlePosition position);

    bool isShowingXUnitsSeparately() const;
    void setShowingXUnitsSeparately(bool unitsSeparate);

    bool isShowingYUnitsSeparately() const;
    void setShowingYUnitsSeparately(bool unitsSeparate);

    bool isShowingXUnits() const;
    void setShowingXUnits(bool showUnits);

    bool isShowingYUnits() const;
    void setShowingYUnits(bool showUnits);

    bool isShowingTimeInCursor() const;
    void setShowingTimeInCursor(bool showTime);

    bool isShowingUnitsInCursor() const;
    void setShowingUnitsInCursor(bool showUnits);

    bool isShowingCursorBackground() const;
    void setShowCursorBackground(bool showBackground);

    osg::Vec4 getCursorBackgroundColor() const;
    void setCursorBackgroundColor(osg::Vec4 color);


    //! \return Prototyp labelek.
    const osgText::Text* getLabelPrototype() const
    {
        return textPrototype.get();
    }
    //! \param prototype Prototyp labelek. Obiekt nie jest kopiowany. Gdy wprowadza si� zmiany w labelce trzeba
    //! koniecznie wywo�a� jeszcze raz setLabelPrototype, bo inaczej nie b�d� od�wie�one. Mo�na r�wnie� sklonowa�
    //! tekst w parametrze.
    void setLabelPrototype(osgText::Text* prototype);

    //! \return
    bool isNormalized() const;
    //! \param normalized
    void setNormalized(bool normalized);

    //! Od�wie�a wykres.
    void refreshAll();
    void refreshCursor();

    static float maxLabelsWidth(const std::vector< osgText::TextPtr > & labels);

private:

    void refreshAllLabelsText();
    void repositionAll();

    void rebuildLabelsText(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max);

    void refreshLabelsPositions(float currentZ);
    void refreshLabelsPositions(std::vector<osgText::TextPtr> &labels, float x0, float x1, float y0, float y1, float z, osgText::TextBase::AlignmentType alignment);


    void getGridLocation(float& x, float& y, float& w, float& h) const;
    osg::Vec4 getGridLocation() const;

    float getXAxisHeight() const;
    float getYAxisWidth() const;
    float getTextPrototypeHeight() const;

    void setDirty(bool dirty = true);

    void refreshFrame(float z);
    void refreshAxis(float z);
    void refreshGrid(float z);
    void refreshLabels(float z);
    void refreshCursor(float z, float range);
    void refreshTitle(float z);
    void refreshSeries(float & currentZ, float zStep);

    //! Pomocnicza metoda do od�wie�ania labelek.
    void refreshLabels(std::vector<osgText::TextPtr> &labels, const std::string& unit, bool condition, float min, float max, float x0, float x1, float y0, float y1, float z, osgText::Text::AlignmentType align );
    bool prepareGeometry(osg::GeometryPtr& geom, bool condition, const char* name);
    bool prepareText(osgText::TextPtr& text, bool condition, const char* name);

private:
    //! \param pos Pozycja.
    //! \param size Rozmiar.
    //! \param label Tekst.
    osg::ref_ptr<osgText::Text> createText(const osg::Vec3& pos, float size, const std::string& label);

    //! Formatuje strumie� tak, aby dobrze formatowa� liczby zmiennoprzecinkowe.
    //! \param stream
    //! \param decimalsToShow Liczba dodatkowych miejsc zmiennoprzecinkowych od pokazania. Gdy r�nica mi�dzy
    //!                       max a min jest rz�du 10^n, to wyswietlone zostan� cyfry z rz�d�w 10^n-decimalsToShow
    //! \param max
    //! \param min
    void formatStream(std::ostringstream& stream, float min, float max, int decimalsToShow = 2 );
};

#endif