#include "CorePCH.h"
#include <core/MultiViewManipulatorItem.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

MultiViewManipulatorItem::MultiViewManipulatorItem( osg::Camera* thumbnailCamera, osgViewer::View* view, osgGA::CameraManipulator* manipulator ) :
thumbnailCamera(thumbnailCamera), view(view), manipulator(manipulator)
{
    UTILS_ASSERT(thumbnailCamera);
    UTILS_ASSERT(view);
    UTILS_ASSERT(manipulator);
}

const std::string& MultiViewManipulatorItem::getName() const
{
    return thumbnailCamera->getName();
}

osgWidget::point_type MultiViewManipulatorItem::getAspectRatio()
{
    UTILS_ASSERT(view.valid());
    if ( view->getCamera()->getViewport() ) {
        return view->getCamera()->getViewport()->aspectRatio();
    } else {
        return 1;
    }
}

void MultiViewManipulatorItem::setSelected( bool selected )
{
    UTILS_ASSERT(view.valid());
    if ( !selected ) {
        // wy³¹czenie powi¹zania miêdzy kamerami
        thumbnailCamera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        thumbnailCamera->setViewMatrix( view->getCamera()->getViewMatrix() );
        thumbnailCamera->setProjectionMatrix( view->getCamera()->getProjectionMatrix() );
        view->setCameraManipulator(NULL);
    } else {
        // powi¹zanie kamery miniaturki z kamer¹ g³ówn¹
        thumbnailCamera->setReferenceFrame( osg::Transform::RELATIVE_RF );
        thumbnailCamera->setViewMatrix( osg::Matrixd::identity() );
        thumbnailCamera->setProjectionMatrix( osg::Matrixd::identity() );
        view->setCameraManipulator(manipulator, false);
    }
}



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
