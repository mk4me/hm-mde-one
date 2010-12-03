/********************************************************************
	created:  2010/11/17
	created:  17:11:2010   17:16
	filename: LabeledView.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__LABELEDVIEW_H__
#define __HEADER_GUARD__LABELEDVIEW_H__

#include <osgWidget/Window>
#include <osgWidget/Label>
#include <osg/ref_ptr>
#include <core/Borderized.h>
#include <core/Buttonized.h>
//#include <osgWidget/>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

// TODO: zamieniæ na bez szablonu

template <class Overlay = Buttonized< Borderized<osgWidget::Widget>, BorderStylePolicy<DefaultStylePolicy> > >
class LabeledView : public osgWidget::Window
{
private:
    osg::ref_ptr<Overlay> overlay;
    osg::ref_ptr<osgWidget::Label> label;

public:

    META_Object(osgUI, LabeledView);

    LabeledView(const std::string& name = "")  :
    osgWidget::Window(name)
    {
        overlay = new Overlay("overlay");
        overlay->setBorderWidth(1);
        overlay->setHooverColor( osgWidget::Color(1,1,1,0.5) );
        overlay->setBorderAllColors( osgWidget::Color(1,1,1,1) );
        overlay->setBorderToggleColor( osgWidget::Color(1,0,0,1) );
        overlay->setToggleEnabled(true);
        label = new osgWidget::Label("label", name);
        label->setLayer(osgWidget::Widget::LAYER_HIGH);
        addWidget(overlay.get());
        addWidget(label.get());
    }

    LabeledView(const LabeledView& view, const osg::CopyOp& copyop) :
    osgWidget::Window(view, copyop)
    {
        overlay = dynamic_cast<Overlay*>(getByName("overlay"));
        if ( !overlay ) {
            overlay = dynamic_cast<Overlay*>(copyop(overlay));
        }
        label = dynamic_cast<osgWidget::Label*>(getByName("label"));
        if ( !label ) {
            label = dynamic_cast<osgWidget::Label*>(copyop(label));
        }
    }

    //! \return
    Overlay* getOverlay()
    {
        return overlay.get();
    }
    //! \return
    const Overlay* getOverlay() const
    {
        return overlay.get();
    }
    //!
    void setOverlay(Overlay* overlay)
    {
        if ( this->overlay ) {
            removeWidget(this->overlay);
        }
        this->overlay = overlay;
        addWidget(overlay);
    }
    //!
    osgWidget::Label* getLabel()
    {
        return label.get();
    }
    //!
    const osgWidget::Label* getLabel() const
    {
        return label.get();
    }

    bool isLabelVisible() const
    {
        return label->getParent() == this;
    }

    void setLabelVisible(bool visible)
    {
        if ( isLabelVisible() ) {
            if ( !visible ) {
                removeWidget(label);
            }
        } else if ( visible ) {
            addWidget(label);
        }
    }

protected:
    //! Prze³adowanie metody rozmieszczaj¹cej elementy.
    //! \param diffWidth
    //! \param diffHeight
    virtual void _resizeImplementation( osgWidget::point_type diffWidth, osgWidget::point_type diffHeight )
    {
        // aktualizacja rozmiaru prostokata z obrazkiem
        float height = getHeight() + diffHeight;
        float width = getWidth() + diffWidth;

        overlay->setDimensions(0, 0, width, height);
        //overlay->setOrigin(0, 0);
        //_positionWidget(overlay, width, height);

        float margin = overlay->getBorderWidth();
        label->setSize( width - 2*margin, label->getHeight() );
        label->setOrigin( margin, height - label->getHeight() - margin);
    }

    //! Prze³adowanie metody obliczaj¹cej rozmiar.
    virtual osgWidget::Window::Sizes _getWidthImplementation() const
    {
        osgWidget::Window::Sizes result;
        result.minimum = 2*getOverlay()->getBorderWidth();
        result.current = result.minimum > getOverlay()->getWidth() ? result.minimum : getOverlay()->getWidth();
        return result;
    }
    //! Prze³adowanie metody obliczaj¹cej rozmiar.
    virtual osgWidget::Window::Sizes _getHeightImplementation() const
    {
        osgWidget::Window::Sizes result;
        result.minimum = 2*getOverlay()->getBorderWidth() /*+ (isLabelVisible() ? getLabel()->getHeight() : 0.0f )*/;
        result.current = result.minimum > getOverlay()->getHeight() ? result.minimum : getOverlay()->getHeight();
        return result;
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__LABELEDVIEW_H__