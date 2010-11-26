/*******************************************************************
	created:  2010/06/09
	created:  9:6:2010   18:04
	filename: HorizontalScrollBar.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__HORIZONTALSCROLLBAR_H__
#define __HEADER_GUARD__HORIZONTALSCROLLBAR_H__

#include <osg/ref_ptr>
#include <osgWidget/Canvas>

// TODO
// Jeœli wrzuci siê ten kod do osobnej biblioteki te includy trzeba
// bêdzie przerobiæ na bezwzglêdne (nawiasy ostre)
#include <core/Buttonized.h>
#include "NestedNodeCallback.h"
#include "Callback.h"

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

/**
 *	Poziomy pasek scroll.
 */
class HorizontalScrollBar : public osgWidget::Canvas, public EventSource
{
public:
  enum EventID {
    EventIDValueChanged = 1,
    EventIDUpdateTooltip
  };

private:
  friend struct NestedCallback<HorizontalScrollBar>;

private:
  //! Kursor.
  osg::ref_ptr<Button> cursor;
  //!
  osg::ref_ptr<osgWidget::Widget> track;
  //!
  osg::ref_ptr<osgWidget::Widget> borderLeft;
  //!
  osg::ref_ptr<osgWidget::Widget> borderRight;
  //!
  osg::ref_ptr<osgWidget::Widget> body;
  //!
  double currentValue;
  //!
  double trackExtension;
  //!
  double draggedValue;
  //!
  osg::ref_ptr<osgWidget::Window> tooltipWindow;

public:
  //! \param name
  HorizontalScrollBar(const std::string& name = "");

  //! \return
  inline osgUI::Button* getCursor()
  { 
    return cursor.get();
  }
  //! \return
  inline const osgUI::Button* getCursor() const
  { 
    return cursor.get();
  }
  //! \return
  inline const osgWidget::Widget* getBorderLeft() const
  { 
    return borderLeft.get();
  }
  //! \return
  inline osgWidget::Widget* getBorderLeft() 
  { 
    return borderLeft.get();
  }
  //! \return
  inline const osgWidget::Widget* getBorderRight() const
  { 
    return borderRight.get();
  }
  //! \return
  inline osgWidget::Widget* getBorderRight() 
  { 
    return borderRight.get();
  }
  //! \return
  inline const osgWidget::Widget* getTrack() const
  { 
    return track.get();
  }
  //! \return
  inline osgWidget::Widget* getTrack() 
  { 
    return track.get();
  }

  //! \return Znormalizowana pozycja slidera.
  inline double getValue() const
  {
    //return getCursorValue();
    return currentValue;
  }

  //! \param value Znormalizowana wartoœæ.
  void setValue(double value);

  //! \return Rozszerzenie obszaru tracka.
  inline double getTrackExtension() const
  {
    return trackExtension;
  }

  //! \param trackExtension Rozszerzenie obszaru tracka.
  inline void setTrackExtension(double trackExtension)
  {
    this->trackExtension = trackExtension;
    setValue(currentValue);
  }

  //!
  void setTrackBackground(osgWidget::Color color)
  {
    borderLeft->setColor(color);
    borderRight->setColor(color);
    track->setColor(color);
  }
  //! \return
  inline const osgWidget::Window* getTooltipWindow() const
  { 
    return tooltipWindow.get();
  }
  //! \return
  inline osgWidget::Window* getTooltipWindow()
  { 
    return tooltipWindow.get();
  }
  //! \param window
  inline void setTooltipWindow(osgWidget::Window* window) 
  { 
    this->tooltipWindow = window; 
  }

protected:
  //! Operator aktualizucj¹cy.
  //! \param node
  //! \param nv
  void operator()(osg::Node* node, osg::NodeVisitor* nv);

  //! Pomocnicza funkcja do ustawiania wartoœci.
  //! \param value Znormalizowana wartoœæ.
  void setCursorValue(double value);
  //! Pomocnicza funkcja pobierania wartoœci.
  //! \return Znormalizowana wartoœæ.
  double getCursorValue() const;
  //! Pomocnicza funkcja tworz¹ca widget t³a z domyœlnymi parametrami.
  //! \param name
  //! \param canFill
  osgWidget::Widget * createBackgroundWidget(const std::string& name, bool canFill = false);
  //! \return Ramka okreœlaj¹ca po³o¿enie, w jakim mo¿e znajdowaæ siê kursor. Zale¿y od
  //!         rozmiaru tracka oraz od property trackExtension.
  osgWidget::Quad getCursorBounds() const;

  //! Reakcja na przesuwanie.
  //! \param event
  //! \return false
  bool onScrollDrag(osgWidget::Event& event);
  //! Reakcja na przesuwanie.
  //! \param event
  //! \return false
  bool onScrollPushed(osgWidget::Event& event);

  //! Reakcja na klik.
  //! \param event
  //! \return false
  bool onTrackClicked(osgWidget::Event& event);

  //! Organizacja layoutu.
  //! \param diffWidth
  //! \param diffHeight
  virtual void _resizeImplementation(osgWidget::point_type diffWidth, osgWidget::point_type diffHeight);

  virtual osgWidget::Window::Sizes _getWidthImplementation() const;
  virtual osgWidget::Window::Sizes _getHeightImplementation() const;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__HORIZONTALSCROLLBAR_H__