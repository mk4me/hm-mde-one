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

class SubjectVisualizer : public core::IVisualizer
{
	UNIQUE_ID("{1122BD8A-6056-4965-9AEA-502F99CA2433}", "Subject visualizer");

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
		  virtual void setSerieName(const std::string & name)
		  {
			  //TODO
			  //obecnie nazwy serii nie sa obslugiwane ale musimy pomyslec o tym i ewentualnie dodac!!
		  }

		  virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
		  {
			  std::string txt;
			  txt = "<b>SUBJECT VISUALIZER</b>";
			  visualizer->widget->setText(QString(txt.c_str()));
		  }

	  private:
		  SubjectVisualizer* visualizer;
	  };

	  virtual IVisualizer* createClone() const
	  {
		  return new SubjectVisualizer();
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

	  virtual void removeSerie(core::IVisualizer::SerieBase *serie)
	  {
	  }

	  virtual QWidget* createWidget(std::vector<QObject*>& actions)
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
