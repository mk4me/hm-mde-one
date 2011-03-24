/********************************************************************
	created:  2010/05/20
	created:  20:5:2010   17:55
	filename: TimelineViewOSG.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINEVIEWOSG_H__
#define __HEADER_GUARD__TIMELINEVIEWOSG_H__

typedef timeline::Model Model;

/**
 *	Timeline zaimplementowany w OSG
 *  Uwaga: kod tej klasy jest ekstremalnie roboczy.
 */
class TimelineViewOSG : public timeline::View<Model, utils::PtrPolicyRaw>, public osgWidget::WindowManager
{
public:
  //! Szerokoœæ guzików.
  static const float BUTTON_WIDTH;
  //! Wysokoœæ guzików.
  static const float BUTTON_HEIGHT;
  //!
  static const std::string DATA_ROOT;

private:
  //!
  osg::ref_ptr<osgWidget::Box> labelTime;
  //!
  osg::ref_ptr<osgWidget::Box> labelTimeScale;
  //! Kontroler.
  osg::ref_ptr<timeline::Controller> controller;
  //! Kursor nawigacyjny.
  osg::ref_ptr<osgui::HorizontalScrollBar> navigationBar;
  //!
  osg::ref_ptr<osgui::HorizontalScrollBar> timescaleBar;
  //!
  osg::ref_ptr<osgui::Potentiometer> potentiometer;
  //!
  bool supressCallbacks;

public:
  //! \param view
  //! \param width
  //! \param height
  //! \param mask
  //! \param flags
  TimelineViewOSG(osgViewer::View * view, float width, float height, unsigned int mask, unsigned int flags);
  //! Aktualizacja poprzez wzór obserwatora.
  //! \param subject
  virtual void update(const Model * subject);
  //! 
  //! \param controller
  void setController(osg::ref_ptr<timeline::Controller> controller);

private:
  //!
  osgWidget::Widget* createMargin();
  //!
  osgWidget::Widget* createStreamBar();
  //!
  osgWidget::Box* createNavigationBox();
  //!
  osgWidget::Widget* createButton(const std::string& name);
  //!
  osgWidget::Box* createLabelBox();

  //!
  osgui::HorizontalScrollBar * createSlider(const std::string& name);
  //!
  osgWidget::Widget * embed(osgWidget::Window * window, const std::string& name = "");

  //! Klikniêcie w przycisk play.
  bool onButtonClicked(osgWidget::Event& event);

  //! Zmiana wartoœci potencjometru.
  bool onPotentiometerChanged(osgui::Event& event);
  //! Zmiana wartoœci slidera.
  bool onSliderValueChanged(osgui::Event& event);
  //! Proœba o odœwie¿enie tooltipa.
  bool onSliderUpdateTooltip(osgui::Event& event);

  //!
  std::string formatTime(double seconds);
  //!
  std::string formatTimeScale(double scale);
};

#endif  // __HEADER_GUARD__TIMELINEVIEWOSG_H__
