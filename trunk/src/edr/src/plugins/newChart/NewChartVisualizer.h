/********************************************************************
	created:	2011/11/12
	created:	12:11:2011   15:12
	filename: 	ChartVisualizer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__
#define HEADER_GUARD_NEW_CHART__CHARTVISUALIZER_H__

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_item.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include <plugins/newChart/INewChartSerie.h>

#include "NewChartSerie.h"
#include "NewChartState.h"
#include "NewChartPicker.h"
#include "NewChartEvents.h"

class StatsTable;
class NewChartLegend;
class PercentScaleDraw;

//! Wizualizator wykresów, oparty o QWT
class NewChartVisualizer : public QObject, public INewChartVisualizer
{
    friend class NewChartSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
    NewChartVisualizer();
	virtual ~NewChartVisualizer();

public:
      //! \return wykres QWT, który jest sercem wizualizatora
      QwtPlot* getPlot();
      //! \return aktualnie aktywna seria lub nullptr jeœli takiej nie ma
      const NewChartSerie* tryGetCurrentSerie() const;
      //! \return aktualnie aktywna seria lub nullptr jeœli takiej nie ma
      NewChartSerie* tryGetCurrentSerie();
      //! \return pusty obiekt wizualizatora
      virtual IVisualizer* createClone() const;
      //! \return nazwa wizualizatora
      virtual const std::string& getName() const;
      //! Tworzy obiekt typu NewChartSerie
      //! \param data ObjectWrapper z interfacem do odczytu DataChannela
      //! \param name nazwa tworzonej serii danych
      //! \return utworzona seria
      core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr& data, const std::string& name);
      // \return metoda nie jest obs³ugiwana, nullptr
      core::IVisualizer::SerieBase *createSerie(const core::IVisualizer::SerieBase * serie);
      //! Usuwa serie z wizualizatora
      //! \param serie seria do usuniecia, musi nalezec do wizualizatora i musi byæ przez niego stworzona
      virtual void removeSerie(core::IVisualizer::SerieBase *serie);
      //! Tworzy g³ówny widget wizualizatora
      //! \param manager Manager z akcjami do flexi bara
      //! \return utworzony widget
      virtual QWidget* createWidget(core::IActionsGroupManager * manager);
      //! \return ikona wizualizatora
      virtual QIcon* createIcon();
      //! \return makymalna liczba serii, -1 bo nie ma limitu
      virtual int getMaxDataSeries() const;
      //! Zwraca info o wspieranych typach (tylko ScalarChannelReaderInterface)
      //! \param info zwracana struktura z opisami
      virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info);
      //! Metoda wywo³ywana w momencie dostarczenia nowych danych do przetwarzania, nie wykorzystywana
      //! \param source
      virtual void setUp(core::IObjectSource* source);
      //! Metoda wywo³ywana cyklicznie pozwala odœwie¿yc stan wizualizatora
      //! \param deltaTime czas od ostatniego wywo³ania metody
      virtual void update(double deltaTime);
      //! metoda, na podstawie stanu wizualizatora, ustawia widoczne przedzia³y wykresu
      void setScale();
      //! resetuje stan wizualizatora, nie obs³ugiwane
      virtual void reset();
      //! ustawia sformatowwany tytul wizualizatora
      virtual void setTitle( const QString& title ) { qwtPlot->setTitle(title); }
      //! \return sformatowany tytul wizualizatora
      virtual QString getTitle() const { return qwtPlot->title().text(); }
      //! Umo¿liwia manipulowanie wykresem, przesuwanie , zblizanie
      //! \param val w³¹cza / wy³¹cza mo¿liwoœæ manipulacji
      void setManipulation(bool val);
      //! \return czy pokazywaæ legende
      bool isShowLegend() const { return showLegend; }
      //! \param val w³¹cza/wy³¹cza rysowanie legendy
      void setShowLegend(bool val);
      //! Filtrowanie eventów Qt, przekazywane do obiektu obs³uguj¹cego stan wizualizatora
      //! \param object
      //! \param event
      bool eventFilter( QObject *object, QEvent *event );
      //! \return wszystkie serie aktualnie obs³ugiwane przez wizualizator
      boost::iterator_range<std::vector<NewChartSerie*>::const_iterator> getSeries() const;
      //! \return czy wizualizator jest w trybie wizualizacji eventów
      bool isEventMode() const { return context != C3DEventsCollection::IEvent::General; }
      //! Zrzut wizualizatora do pixmapy
      virtual QPixmap print() const;

private:
      //! Dodanie krzywej do wykresu, aktualizacja podzialek
      //! \param curve dodawana krzywa
      //! \param scales ekstrema dodawanego wykresu
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);
      //! Odtwarza podzialki na podstawie widocznych krzywych
      void recreateScales();
      //! Ustawia zdarzenia C3D zwi¹zane z próba pomiarowa
      //! \param serie seria, której dotycza eventy
      //! \param val kolekcja ze zdarzeniami
      void setEvents(NewChartSerie* serie, EventsCollectionConstPtr val );
      //! Tworzy drzewo ze statystykami
      //! \param stats
      void recreateStats(ScalarChannelStatsConstPtr stats = ScalarChannelStatsConstPtr());
      //! odœwie¿a widzialne serie, przypisanie wspó³rzêdnej Z (przykrywanie)
      void refreshSerieLayers();
      //! odœwie¿a spinboxy zawieraj¹ce przesuniecie i skale aktywnej serii
      void refreshSpinBoxes();
      //! Ustawia podzialki wykresu
      //! \param scaleToActive czy skalowanie ma byæ wzgledem aktywnej serii czy caloœci
      //! \param eventMode czy jest aktywny eventMode
      void setScale(bool scaleToActive, bool eventMode);
      //! Ustawia podzialki wykresu, czesciowo na podstawie stanu wizualizatora
      //! \param scaleToActive czy skalowanie ma byæ wzgledem aktywnej serii czy caloœci
      void setGlobalScales(bool scaleToActive);
      //! Dostosowywuje krok, z którym mo¿na przesuwaæ wykres (adaptacyjne spinboxy)
      //! \param spinBox modyfikowany spinbox
      //! \param axis os, na podstawie której ma byæ obliczony nowy krok
      void adjustOffsetStep(QDoubleSpinBox* spinBox, QwtPlot::Axis axis);
      //! Czy krzywa nale¿y do jakiejs serii czy mo¿e jest krzywa dodatkowa
      //! \param curve badana krzywa
      bool isCurveFromSerie(const QwtPlotCurve* curve) const;
      //! Metoda wywo³ywana kiedy zmieni siê stan wykresów
      void plotChanged();
      //! odœwie¿a dodatkowe krzywe
      void refreshBounds();
      //! chowa lub pokazuje wszystkie etykiety
      void setLabelsVisible(bool);
      //! Tworzy kroczaca srednia dla wykresu
      //! \param startIdx
      //! \param endIdx
      //! \param inReal
      //! \param optInTimePeriod
      //! \param outBegIdx
      //! \param outNBElement
      //! \param outReal
      void simpleMovingAverage(int startIdx, int endIdx, const std::vector<float> & inReal,
          int optInTimePeriod, int & outBegIdx, int & outNBElement, std::vector<float> & outReal);
      //! tworzy ograniczenia - gorne i dolne krzywych
      //! \param startIdx
      //! \param endIdx
      //! \param inReal
      //! \param optInTimePeriod
      //! \param optInNbDevUp
      //! \param optInNbDevDn
      //! \param outBegIdx
      //! \param outNBElement
      //! \param outRealUpperBand
      //! \param outRealMiddleBand
      //! \param outRealLowerBand
      void bbands( int startIdx, int endIdx, const std::vector<float> & inReal,
          int optInTimePeriod, double optInNbDevUp, double optInNbDevDn, int & outBegIdx, int & outNBElement,
          std::vector<float> & outRealUpperBand,
          std::vector<float> & outRealMiddleBand,
          std::vector<float> & outRealLowerBand);
      //! Tworzy odchylenie standardowe
      //! \param inReal
      //! \param inMovAvg
      //! \param inMovAvgBegIdx
      //! \param inMovAvgNbElement
      //! \param timePeriod
      //! \param output
      void stddev_using_precalc_ma( const std::vector<float> & inReal,
          const std::vector<float> & inMovAvg,
          int inMovAvgBegIdx,
          int inMovAvgNbElement,
          int timePeriod,
          std::vector<float> & output);

private slots:
      //! Ustawia aktywna serie danych
      //! \param idx indeks w comboboxie
      void setActiveSerie(int idx);
      //! Wywo³ane przez picker, ustawia aktywna serie
      //! \param item krzywa zwiazana z seria
      void onSerieSelected(QwtPlotItem* item);
      //! Zaznacza aktywna seria, metoda wywo³ywana przez legende
      //! \param dataSerie krzywa zwiazana z seria
      //! \param on stan przycisku legendy (on = uczyn aktywna)
      //! \param idx niewykorzystywane
      void onSerieSelected(QwtPlotItem* dataSerie, bool on, int idx);
      //! Zmienia widocznoœæ aktywnej serii, metoda wywo³ywana przez legende
      //! \param dataSerie krzywa zwiazana z seria
      //! \param visible czy seria ma byæ widoczna czy ukryta
      void onSerieVisible(const QwtPlotItem* dataSerie, bool visible);
      //! wybrano zmiane stanu wizualizatora
      void onStateAction();
      //! zmiana kontekstu eventów, czyli sposobu rysowania wykresu
      //! \param  okresla czy wykres ma obs³ugiwac zdarzenia, a jeœli tak to czy lewe, czy prawe
      void onEventContext(int);
      //! zarz¹dza widocznoœcia statystyk
      //! \param visible pokaz / ukryj
      void showStatistics(bool visible);
      //! \return czy czas aktywnej serii jest wewn¹trz jakiegos eventu zwi¹zanego z próba pomiarowa
      bool timeInsideEvent();
      //! zmiana podzialki
      //! \param czy skalowanie powinno byæ do aktywnej serii czy do ca³ego wykresu
      void scaleToActiveSerie(bool);
      //! Zmieniono przesuniecie aktywnej serii
      //! \param d przesuniecie wzgledem osi X
      void onShiftX(double d);
      //! Zmieniono przesuniecie aktywnej serii
      //! \param d przesuniecie wzgledem osi Y
      void onShiftY(double d);
      //! Zmieniono skale aktywnej serii
      //! \param d skala wzgledem osi X
      void onScaleX(double d);
      //! Zmieniono skala aktywnej serii
      //! \param d skale wzgledem osi Y
      void onScaleY(double d);

      void showBands(bool show);
      //! Wstêgi wokó³ serii danych - zbiorcze dla wszystkich serii na wykresie
      void showDataBounds(bool show);
      //! Œrdenia danych
      void showMovingAverageCurve(bool show);
      //! Auto odœwie¿anie wstêg
      void setAutoRefreshDataBounds(bool autorefresh);
      //! Okno czasowe dla œredniej krocz¹cej
      void setMovingAverageTimeWindow(double timeWindow);

private:
    //! dodatkowa krzywa - gorne ograniczenie serii danych
    QwtPlotCurve* upperBoundCurve;
    //! dodatkowa krzywa - dolne ograniczenie serii danych
    QwtPlotCurve* lowerBoundCurve;
    //! dodatkowa krzywa - srednia kroczaca
    QwtPlotCurve* averageCurve;
    //! wektor z operacjami do wykonania na póŸniej (w update) pozwala wyeliminowaæ problemy z watkami
    std::vector< boost::function<void ()> > updateFIFO;
    //! automatyczne odœwie¿anie dodatkowych krzywych
    bool boundsAutoRefresh;
    //! dodatkowe krzywe powinny zostac odœwie¿one
    bool boundsToRefresh;
    //! zakres czasowy dla kroczacej krzywej
    double movingAverageTimeWindow;
    //! liczba punktów przypadaj¹cych na zakres czasowy (okno) krzywej kroczacej
    int pointsPerWindow;
    //! Obiekt wykresu z Qwt, serce wizualizatora
    QwtPlot* qwtPlot;
    //! Legenda, która zosta³a wzbogacona w customowe widgety
    NewChartLegend* legend;
    //! Marker obrazuj¹cy aktualna wartoœæ na wykresie
    QwtPlotMarker* qwtMarker;
    //! "kratka" na wykresie
    core::shared_ptr<QwtPlotGrid> grid;
    //! obiekt przechwuj¹cy ekstrema krzywych
    Scales plotScales;
    //! combo z lista serii danych
    QComboBox* activeSerieCombo;
    //! kolekcja z wszystkimi seriami danych
    std::vector<NewChartSerie*> series;
    //! mapa przypisuj¹ca akcje do stanu wykresu
    std::map<QAction*, NewChartStatePtr> statesMap;
    //! flaga mowiaca o tym, czy legenda jest widoczna
    bool showLegend;
    //! aktualny stan wykresu, nie mo¿e byæ nullem
    NewChartStatePtr currentState;
    //! indeks aktualnej serii
    int currentSerie;

    // TODO porzadek z manipulatorami
    //! Manipulator ca³ego wykresu
    core::shared_ptr<QwtPlotZoomer> zoomer;
    //! Manipulator ca³ego wykresu
    QwtPlotPanner* plotPanner;
    //! Manipulator ca³ego wykresu
    QwtPlotMagnifier* plotMagnifier;

    //! Tabela statystyk
    StatsTable* statsTable;
    //! czy wykres powinno siê skalowaæ do aktywnego
    bool scaleToActive;
    //! obiekt umo¿liwiaj¹cy rysowanie podzialki z procentami
    PercentScaleDraw* percentDraw;
    //! widget z menu eventów
    QWidget* eventsContextWidget;
    //! menu eventów (general, left, right)
    QComboBox * eventsMenu;
    //! akcja wywoluj¹ca stan 'picker'
    QAction* pickerAction;
    //! akcja wywoluj¹ca stan zaznaczania wartoœci na wykresie
    QAction* valueMarkerAction;
    //! akcja wywoluj¹ca stan zaznaczania roznicy wartoœci na wykresie
    QAction* vMarkerAction;
    //! akcja wywoluj¹ca stan zaznaczania roznicy argumentów na wykresie
    QAction* hMarkerAction;
    //! akcja wywoluj¹ca skalowanie do aktywnej serii
    QAction* scaleAction;
    //! akcja wywoluj¹ca pokazanie/chowanie dodatkowych krzywych
    QAction* bandsAction;
    //! spinbox z aktualnym przesunieciem w X aktywnej serii
    QDoubleSpinBox* shiftSpinX;
    //! spinbox z aktualnym przesunieciem w Y aktywnej serii
    QDoubleSpinBox* shiftSpinY;
    //! spinbox z aktualna skala w X aktywnej serii
    QDoubleSpinBox* scaleSpinX;
    //! spinbox z aktualna skala w T aktywnej serii
    QDoubleSpinBox* scaleSpinY;
    //! aktualny kontekst eventów (general, left, right)
    C3DEventsCollection::Context context;
    //! pomocnicze, do ustalenia czy zmieni³ siê aktualnie obrazowany event
    EventsHelper::SegmentConstPtr oldSegment;
    //! stan wizualizatora, picker - umo¿liwia wybieranie aktywnej serii klikaj¹c na nia
    NewChartPickerPtr picker;
    //! czas aktywnej serii
    float currentSerieTime;
    //! aktualna wartoœæ dla serii
    float currentSerieValue;
};
typedef core::shared_ptr<NewChartVisualizer> NewChartVisualizerPtr;
typedef core::shared_ptr<const NewChartVisualizer> NewChartVisualizerConstPtr;



#endif
