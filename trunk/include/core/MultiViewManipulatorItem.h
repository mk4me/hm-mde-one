/********************************************************************
	created:  2010/12/13
	created:  13:12:2010   14:24
	filename: MultiViewManipulatorItem.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__
#define __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__

#include <osg/Camera>
#include <osgViewer/Viewer>
#include <osgGA/CameraManipulator>
#include <core/MultiView.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

//! Item powinien byæ u¿ywany wy³¹cznie jako preview. Jego dzia³anie polega
//! na kontrolowaniu manipulatorów kamer.
class MultiViewManipulatorItem : public core::MultiView::PreviewItem
{
private:
    //! Kamera z miniaturka.
    osg::ref_ptr<osg::Camera> thumbnailCamera;
    //! Wspó³czynnik proporcji.
    osg::observer_ptr<osgViewer::View> view;
    //! Manipulator kamery.
    osg::ref_ptr<osgGA::CameraManipulator> manipulator;

public:
    //! \param camera
    //! \param aspectRatio
    //! \param view
    //! \param manipulator
    MultiViewManipulatorItem(osg::Camera* thumbnailCamera, osgViewer::View* view, osgGA::CameraManipulator* manipulator);

    //! \return Nazwa widgetu.
    virtual const std::string& getName() const;

    //! \return Wspó³czynnik proporcji.
    virtual osgWidget::point_type getAspectRatio();

    //! Ustawia/usuwa manipulator viewera oraz przenosi transformacjê z/do kamery.
    virtual void setSelected(bool selected);

    //! Tutaj nic siê nie dzieje - pracujemy na g³ównej kamerze viewera, która
    //! zawsze zajmuje pe³ny ekran.
    virtual void setLocation(osgWidget::point_type x, osgWidget::point_type y,
        osgWidget::point_type w, osgWidget::point_type h)
    {}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__MULTIVIEWMANIPULATORITEM_H__