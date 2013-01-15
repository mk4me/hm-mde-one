#include "CorePCH.h"
#include <utils/Debug.h>
#include <core/MultiViewManipulatorItem.h>
#include <core/ILog.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

MultiViewManipulatorItem::MultiViewManipulatorItem( osg::Camera* thumbnailCamera, osgViewer::View* view, osgGA::CameraManipulator* manipulator ) :
thumbnailCamera(thumbnailCamera),
view(view),
manipulator(manipulator)
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

void MultiViewManipulatorItem::setVisible( bool visible )
{
    UTILS_ASSERT(view.valid());
    if ( !visible ) {
        if ( thumbnailCamera->getReferenceFrame() != osg::Transform::ABSOLUTE_RF ) {
            // wyłączenie powiązania między kamerami
            thumbnailCamera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
            thumbnailCamera->setViewMatrix( view->getCamera()->getViewMatrix() );
            thumbnailCamera->setProjectionMatrix( view->getCamera()->getProjectionMatrix() );
            view->setCameraManipulator(NULL);
        }
    } else {
        if ( thumbnailCamera->getReferenceFrame() != osg::Transform::RELATIVE_RF ) {
            // powiązanie kamery miniaturki z kamerą główną
            thumbnailCamera->setReferenceFrame( osg::Transform::RELATIVE_RF );
            thumbnailCamera->setViewMatrix( osg::Matrixd::identity() );
            thumbnailCamera->setProjectionMatrix( osg::Matrixd::identity() );
            view->setCameraManipulator(manipulator, false);
        }
    }
}



////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
