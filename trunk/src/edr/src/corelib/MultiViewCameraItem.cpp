#include "CorePCH.h"
#include <utils/Debug.h>
#include <core/MultiViewCameraItem.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

MultiViewCameraItem::MultiViewCameraItem( osg::Camera* camera, osg::Group* parent, osg::View* view ) :
camera(camera), view(view), aspectRatio(1), childIdx(-1), parent(parent)
{
    UTILS_ASSERT(camera);
    UTILS_ASSERT(parent);
}

MultiViewCameraItem::MultiViewCameraItem( osg::Camera* camera, osg::Group* parent, osgWidget::point_type aspectRatio /*= 1*/ ) :
camera(camera), aspectRatio(aspectRatio), childIdx(-1), parent(parent)
{
    UTILS_ASSERT(camera);
    UTILS_ASSERT(parent);
}

bool MultiViewCameraItem::isVisible() const
{
    if ( parent->getChildIndex(camera) != parent->getNumChildren() ) {
        // gdy mamy swticha dziecko mo¿e byæ wy³¹czone
        if ( osg::Switch* sw = parent->asSwitch() ) {
            // dziecko mo¿e byæ wy³¹czone
            return sw->getChildValue(camera);
        } else {
            // gdy jest zwyk³a grupa na pewno dzieko jest widoczne
            return true;
        }
    }
    return false;
}

void MultiViewCameraItem::setVisible( bool visible )
{
    if ( visible ) {
        if ( !isVisible() ) {
            if ( childIdx < 0 ) {
                // rozpoczyna³ jako niewidoczny
                parent->addChild(camera);
            } else {
                // wczeœniej wy³¹czyliœmy
                if ( parent->asSwitch() ) {
                    parent->asSwitch()->setValue(childIdx, true);
                } else {
                    parent->insertChild(childIdx, camera);
                }
                childIdx = -1;
            }
        }
    } else {
        if ( isVisible() ) {
            // czy to slave?
            if ( osg::View* slaveView = camera->getView() ) {
                if ( unsigned slaveIdx = slaveView->findSlaveIndexForCamera(camera.get()) ) {
                    UTILS_ASSERT(false, "Not supported yet");
                }
            }
            // pobieramy indeks dziecka
            childIdx = parent->getChildIndex(camera.get());
            if ( parent->asSwitch() ) {
                parent->asSwitch()->setValue(childIdx, false);
            } else {
                parent->removeChild(childIdx);
            }
        }
    }
}

void MultiViewCameraItem::setLocation( osgWidget::point_type x, osgWidget::point_type y, osgWidget::point_type w, osgWidget::point_type h )
{
    // korekcja macierzy projekcji
    if ( osg::Viewport* viewport = camera->getViewport() ) {
        double widthChangeRatio = double(w) / double(viewport->width());
        double heigtChangeRatio = double(h) / double(viewport->height());
        double aspectRatioChange = widthChangeRatio / heigtChangeRatio;
        if ( camera->getReferenceFrame() != osg::Transform::RELATIVE_RF ) {
            switch(camera->getProjectionResizePolicy())
            {
            case(osg::Camera::HORIZONTAL):
                camera->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange, 1.0, 1.0);
                break;
            case(osg::Camera::VERTICAL):
                camera->getProjectionMatrix() *= osg::Matrix::scale(1.0, aspectRatioChange, 1.0);
                break;
            default:
                break;
            }
        }
    }

    // ustawienie viewportu
    camera->setViewport(x, y, w, h);
}

osgWidget::point_type MultiViewCameraItem::getAspectRatio()
{
    if ( view.valid() && view->getCamera()->getViewport() ) {
        return view->getCamera()->getViewport()->aspectRatio();
    } else {
        return aspectRatio;
    }
}

void MultiViewCameraItem::setDefaultAspectRatio( osgWidget::point_type aspectRatio )
{
    this->aspectRatio = aspectRatio;
}

const std::string& MultiViewCameraItem::getName() const
{
    return camera->getName();
}


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
