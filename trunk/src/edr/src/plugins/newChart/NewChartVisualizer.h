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

#include "NewChartSerie.h"
#include "NewChartState.h"

class NewChartVisualizer : public QObject, public core::IVisualizer, public boost::enable_shared_from_this<NewChartVisualizer>
{
    friend class NewChartSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
    NewChartVisualizer();

	virtual ~NewChartVisualizer()                              
    {
    }
    
public:
      QwtPlot* getPlot();

      const NewChartSerie* tryGetCurrentSerie() const {
          if (currentSerie >= 0 && currentSerie < series.size()) {
              return series[currentSerie];
          }

          return nullptr;
      }

      virtual IVisualizer* createClone() const
      {
          return new NewChartVisualizer();
      }

      virtual const std::string& getName() const
      {
          static std::string name = "NewChartVisualizer";
          return name;
      }

      core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr& data, const std::string& name);

      core::IVisualizer::SerieBase *createSerie(const core::IVisualizer::SerieBase * serie)
      {
          return nullptr;
      }

      virtual void removeSerie(core::IVisualizer::SerieBase *serie);

      virtual QWidget* createWidget(std::vector<QObject*>& actions);

      virtual QIcon* createIcon()
      {
          return nullptr;
      }

      virtual int getMaxDataSeries(void) const
      {
          return -1;
      }

      virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info);

      virtual void setUp(core::IObjectSource* source) 
      {
      }

      virtual void update(double deltaTime)
      {
          if (currentSerie >= 0 && currentSerie < series.size()) {
             
             qwtMarker->setVisible(true);
             NewChartSerie* serie = series[currentSerie];
             float x = serie->getTime();
             float y = serie->getCurrentValue();
             qwtMarker->setXValue(x);
             qwtMarker->setYValue(y);
             qwtMarker->setLabel(QString("Time: %1, Value: %2").arg(x).arg(y));
          } else {
             qwtMarker->setVisible(false);
          }
      }
      virtual void reset()
      {

      }

      void setTitle( const QString& title )
      {
          qwtPlot->setTitle(title);
      }

      void setManipulation(bool val);

      bool isShowLegend() const { return showLegend; }
      void setShowLegend(bool val)
      {
          if (qwtPlot) {
              // todo - sprawdzic wyciek...
              qwtPlot->insertLegend( val ? new QwtLegend() : nullptr, QwtPlot::RightLegend );
          }
          showLegend = val;
      }

      bool eventFilter( QObject *object, QEvent *event );

private:
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);
    void onSerieSelected(QwtPlotItem*);
    void onStateAction();
//signals:
//     void activeSeriePicked(int idx);
     
private:
      QwtPlot* qwtPlot;
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
};
typedef core::shared_ptr<NewChartVisualizer> NewChartVisualizerPtr;
typedef core::shared_ptr<const NewChartVisualizer> NewChartVisualizerConstPtr;
    
#endif  