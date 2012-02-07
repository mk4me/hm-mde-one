/********************************************************************
	created:	2011/07/21
	created:	21:7:2011   18:42
	filename: 	SubjectVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SUBJECT__SUBJECTVISUALIZER_H__
#define HEADER_GUARD_SUBJECT__SUBJECTVISUALIZER_H__

#include <string>
#include <core/IObjectSource.h>
#include <QtCore/QObject>
#include <core/IVisualizer.h>
#include <core/PluginCommon.h>
#include <core/IIdentifiable.h>
#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QLabel>
#include <core/IDataManager.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/Subject.h>

class SubjectVisualizer : public core::IVisualizer
{
	UNIQUE_ID("{9D6687BA-C4D6-438A-B6C9-02FDAA6ABDC5}", "Subject visualizer");

	std::string name;
	std::auto_ptr<QLabel> widget;

public:
	SubjectVisualizer() :
	  name("SubjectVisualizer")
	  {
	  }

	  class SubjectVisualizerSerie : public SerieBase
	  {
	  public:
		  SubjectVisualizerSerie(SubjectVisualizer * visualizer)
			  : visualizer(visualizer)
		  {

		  }

	  protected:
		  virtual void setName(const std::string & name)
		  {
			  this->name = name;
		  }

          virtual const std::string & getName() const
          {
                return name;
          }

		  virtual void setData(const core::ObjectWrapperConstPtr & data)
		  {
			  std::string txt;
			  txt = "<b>SUBJECT VISUALIZER</b>";
			  visualizer->widget->setText(QString(txt.c_str()));
		  }

          virtual const core::ObjectWrapperConstPtr & getData() const
          {
              return data;
          }

	  private:
		  SubjectVisualizer* visualizer;
          std::string name;
          core::ObjectWrapperConstPtr data;
	  };

	  virtual IVisualizer* createClone() const
	  {
		  return new SubjectVisualizer();
	  }

      virtual QPixmap print() const 
      {
          return QPixmap::grabWidget(widget.get());
      }

	  virtual const std::string& getName() const
	  {
		  static std::string name = "SubjectVisualizer";
		  return name;
	  }

	  core::IVisualizer::SerieBase *createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
	  {
		  core::IVisualizer::SerieBase * ret = new SubjectVisualizerSerie(this);
		  ret->setName(name);
		  ret->setData(data);

		  return ret;
	  }

      core::IVisualizer::SerieBase *createSerie(const core::IVisualizer::SerieBase * serie)
      {
          return nullptr;
      }

	  virtual void removeSerie(core::IVisualizer::SerieBase *serie)
	  {

	  }

	  virtual QWidget* createWidget(core::IActionsGroupManager * manager)
	  {
		  widget.reset(new QLabel(nullptr));
		  widget->setStyleSheet(  "QLabel { background: blue; }" );
		  return widget.get();
	  }

	  virtual QIcon* createIcon()
	  {
		  return nullptr;
	  }

	  virtual int getMaxDataSeries(void) const
	  {
		  return 1;
	  }

	  virtual void getInputInfo( std::vector<core::IInputDescription::InputInfo>& info)
	  {
		  core::IInputDescription::InputInfo input;
		  input.name = "subject";
		  input.type = typeid(Subject);
		  input.required = false;
		  input.modify = false;
		  info.push_back(input);
	  }

	  virtual void setUp(core::IObjectSource* source) 
	  {
	  }

	  virtual void update(double deltaTime)
	  {
	  }
	  virtual void reset()
	  {

	  }
};

typedef core::shared_ptr<SubjectVisualizer> SubjectVisualizerPtr;
typedef core::shared_ptr<const SubjectVisualizer> SubjectVisualizerConstPtr;

#endif
