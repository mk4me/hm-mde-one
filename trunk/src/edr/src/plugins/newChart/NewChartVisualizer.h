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

#include "NewChartSerie.h"
#include "NewChartPicker.h"

class NewChartVisualizer : public QObject, public core::IVisualizer
{
    friend class NewChartSerie;
    Q_OBJECT;
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
    NewChartVisualizer() : showLegend(true), currentSerie(-1) {}
	virtual ~NewChartVisualizer() 
    {
    }
    
public:
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
          return 1;
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

      bool isShowLegend() const { return showLegend; }
      void setShowLegend(bool val)
      {
          if (qwtPlot) {
              // todo - sprawdzic wyciek...
              qwtPlot->insertLegend( val ? new QwtLegend() : nullptr, QwtPlot::RightLegend );
          }
          showLegend = val;
      }

private:
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);
    void onSerieSelected(QwtPlotItem*);

//signals:
//     void activeSeriePicked(int idx);
     
private:
      core::shared_ptr<QwtPlot> qwtPlot;
      QwtPlotMarker* qwtMarker;
      core::shared_ptr<QwtPlotGrid> grid;
      core::shared_ptr<QwtPlotZoomer> zoomer;
      Scales plotScales;
      QComboBox* activeSerieCombo;
      std::vector<NewChartSerie*> series;
      bool showLegend;
      NewChartPicker* picker;
      int currentSerie;
};
    
#endif  