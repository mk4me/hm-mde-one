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

class NewChartVisualizer : public QObject, public core::IVisualizer
{
    Q_OBJECT;
public:
    struct Scales
    {
        Scales() : initialized(false) {}
        Scales(float xMin, float xMax, float yMin, float yMax) :
            xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), initialized(true) {}

        float xMin, xMax, yMin, yMax;
        bool initialized;
    };
    NewChartVisualizer() : markerX(0.0f) {}
	virtual ~NewChartVisualizer() {}
    UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "New Chart Visualizer");

public:
      class NewChartSerie : public TimeSerieBase
      {
      protected:
          class PointData: public QwtSyntheticPointData
          {
          public:
              PointData(ScalarChannelReaderInterfaceConstPtr reader, int size = 100):
                QwtSyntheticPointData(size),
                    reader(reader),
                    accessor(new ScalarContiniousTimeAccessor(reader))          
                {
                    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
                    //normalizedChannel.reset(new ScalarModifier(nonConstChannel, ScalarChannelNormalizer()));
                    //accessor.reset(new ScalarContiniousTimeAccessor());
                    stats.reset(new ScalarChannelStats(nonConstChannel));
                }
                virtual double y(double x) const
                {
                    if (x > 3.3f && x < 3.5f) {
                        int x = 0;
                        x = x;
                    }
                    if (x >= 0 && x < reader->getLength()) {
                        
                        return accessor->getValue(x);
                    } else {
                        return 0.0;
                    }
                }

                Scales getScales() const { return Scales(0.0f, reader->getLength(), getMin(), getMax()); }
                float getLength() const { return reader->getLength(); }
                float getMin() const { return stats->minValue(); }
                float getMax() const { return stats->maxValue(); }

          private:
              ScalarChannelReaderInterfaceConstPtr reader;
              ScalarChannelStatsPtr stats;
              boost::shared_ptr<ScalarContiniousTimeAccessor> accessor;
              //boost::shared_ptr<ScalarModifier> normalizedChannel;
          };
      public:
          NewChartSerie(NewChartVisualizer * visualizer)
              : visualizer(visualizer), active(false)
          {

          }

          virtual void setTime(float time);

          virtual float getLength() const { return pointHelper->getLength(); }

          void setColor(int r, int g, int b, int a = 255) 
          {
              UTILS_ASSERT(curve);
              QPen pen = curve->pen();
              pen.setColor(QColor(r, g, b, a));
              curve->setPen(pen);
          }

          void setWidth(int width)
          {
              UTILS_ASSERT(curve);
              QPen pen = curve->pen();
              pen.setWidth(width);
              curve->setPen(pen);
          }

          virtual void setName(const std::string & name)
          {
              this->name = name;
          }

          virtual const std::string & getName() const
          {
              return name;
          }

          virtual void setData(const core::ObjectWrapperConstPtr & data);

          virtual const core::ObjectWrapperConstPtr & getData() const
          {
              return data;
          }

          Scales getScales() const 
          { 
              UTILS_ASSERT(pointHelper);
              return pointHelper->getScales(); 
          }
          
          bool getActive() const { return active; }
          void setActive(bool val);

      private:
          NewChartVisualizer* visualizer;
          std::string name;
          core::ObjectWrapperConstPtr data;
          QwtPlotCurve* curve;
          PointData* pointHelper;
          bool active;
          
      };

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
          qwtMarker->setXValue(markerX);
          qwtMarker->setLabel(markerLabel);
      }
      virtual void reset()
      {

      }

      void setTitle( const QString& title )
      {
          qwtPlot->setTitle(title);
      }

private:
      void addPlotCurve(QwtPlotCurve* curve, const Scales& scales);

private slots:
    void setNormalized(bool normalized);
    void setActiveSerie(int idx);

signals:
     void activeSeriePicked(int idx);
     
private:
      core::shared_ptr<QwtPlot> qwtPlot;
      //core::shared_ptr<QwtPlotMarker> qwtMarker;
      QwtPlotMarker* qwtMarker;
      core::shared_ptr<QwtPlotGrid> grid;
      core::shared_ptr<QwtPlotZoomer> zoomer;
      Scales plotScales;
      float markerX;
      QString markerLabel;
      QComboBox* activeSerieCombo;
      std::vector<NewChartSerie*> series;
};
    
#endif  