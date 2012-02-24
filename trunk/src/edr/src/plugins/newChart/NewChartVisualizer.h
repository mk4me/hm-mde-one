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
#include <core/IVisualizer.h>
#include <core/IObjectSource.h>
#include <plugins/c3d/C3DChannels.h>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_text.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_marker.h>
#include <qwt/qwt_series_data.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_picker_machine.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_plot_panner.h>
#include <qwt/qwt_plot_magnifier.h>
#include <qwt/qwt_scale_draw.h>

#include "NewChartSerie.h"
#include "NewChartState.h"
#include "NewChartPicker.h"
#include "NewChartEvents.h"

class StatsTable;
class NewChartLegend;
class PercentScaleDraw;

class NewChartVisualizer : public QObject, public core::IVisualizer, public boost::enable_shared_from_this<NewChartVisualizer>
{
    friend class NewChartSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
    NewChartVisualizer();
	virtual ~NewChartVisualizer();
    
public:
      QwtPlot* getPlot();
      const NewChartSerie* tryGetCurrentSerie() const;

      virtual IVisualizer* createClone() const;
      
      virtual const std::string& getName() const;
      
      core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr& data, const std::string& name);

      core::IVisualizer::SerieBase *createSerie(const core::IVisualizer::SerieBase * serie);
      
      virtual void removeSerie(core::IVisualizer::SerieBase *serie);

      virtual QWidget* createWidget(core::IActionsGroupManager * manager);

      virtual QIcon* createIcon();
      
      virtual int getMaxDataSeries(void) const;

      virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info);

      virtual void setUp(core::IObjectSource* source);
      virtual void update(double deltaTime);
      virtual void reset();

      void setTitle( const QString& title ) { qwtPlot->setTitle(title); }
      void setManipulation(bool val);
      bool isShowLegend() const { return showLegend; }
      void setShowLegend(bool val);
      bool eventFilter( QObject *object, QEvent *event );

      bool isEventMode() const { return context != C3DEventsCollection::Context::General; }

      virtual QPixmap print() const 
      {
          return QPixmap::grabWidget(qwtPlot);
      }
      
private:
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);
      void recreateScales();
      void setEvents(NewChartSerie* serie, EventsCollectionConstPtr val );
      void rescale(float t1, float t2);
      void recreateStats(ScalarChannelStatsConstPtr stats = ScalarChannelStatsConstPtr());
      void refreshSerieLayers();
      void refreshSpinBoxes();
      void setScale(bool scaleToActive, bool eventMode);
      void setGlobalScales(bool scaleToActive);
      void adjustOffsetStep(QDoubleSpinBox* spinBox, QwtPlot::Axis axis);

private slots:
      void onComboDestroy(QObject * obj);
      void setNormalized(bool normalized);
      void setActiveSerie(int idx);
      void onSerieSelected(QwtPlotItem*);
      void onSerieSelected(QwtPlotItem* dataSerie, bool on, int idx);
      void onSerieVisible(const QwtPlotItem* dataSerie, bool visible);
      void onStateAction();
      void onEventContext(int);
      void showStatistics(bool visible);
      bool timeInsideEvent();
      void scaleToActiveSerie(bool);
      void onShiftX(double d);
      void onShiftY(double d);
      void onScaleX(double d);
      void onScaleY(double d);

private:
      QwtPlot* qwtPlot;
      NewChartLegend* legend;
      QwtPlotMarker* qwtMarker;
      core::shared_ptr<QwtPlotGrid> grid;
      core::shared_ptr<QwtPlotZoomer> zoomer;
      Scales plotScales;
      QComboBox* activeSerieCombo;
      std::vector<NewChartSerie*> series;
      std::map<QAction*, NewChartStatePtr> statesMap;
      bool showLegend;
      NewChartStatePtr currentState;
      int currentSerie;
      QwtPlotPanner* plotPanner;
      QwtPlotMagnifier* plotMagnifier;
      EventsPlotItem* eventsItem;
      StatsTable* statsTable;
      bool eventsVisible;
      bool scaleToActive;
      PercentScaleDraw* percentDraw;

      QWidget* eventsContextWidget;
      QComboBox * eventsMenu;

      QDoubleSpinBox* shiftSpinX;
      QDoubleSpinBox* shiftSpinY;
      QDoubleSpinBox* scaleSpinX;
      QDoubleSpinBox* scaleSpinY;

      C3DEventsCollection::Context context;
      std::map<NewChartSerie*, EventsHelperPtr> eventsHelpers;
      EventsHelper::SegmentConstPtr oldSegment;
      NewChartPickerPtr picker;
};
typedef core::shared_ptr<NewChartVisualizer> NewChartVisualizerPtr;
typedef core::shared_ptr<const NewChartVisualizer> NewChartVisualizerConstPtr;


    
#endif  