#include "CorePCH.h"
#include <core/MultiViewCameraItem.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

MultiViewCameraItem::MultiViewCameraItem( osg::Camera* camera, osg::View* view ) : 
camera(camera), view(view), aspectRatio(1), childIdx(-1)
{
    UTILS_ASSERT(camera);
}

MultiViewCameraItem::MultiViewCameraItem( osg::Camera* camera, osgWidget::point_type aspectRatio /*= 1*/ ) :
camera(camera), aspectRatio(aspectRatio), childIdx(-1)
{
    UTILS_ASSERT(camera);
}

void MultiViewCameraItem::setSelected( bool selected )
{
    if ( selected ) {
        // dodajemy z powrotem do rodzica
        UTILS_ASSERT(parent.valid());
        UTILS_ASSERT(childIdx != -1);
        if ( parent->asSwitch() ) {
            parent->asSwitch()->setValue(childIdx, true);
        } else {
            parent->insertChild(childIdx, camera);
        }
        childIdx = -1;
        parent = NULL;

    } else {
        // czy to slave?
        if ( osg::View* slaveView = camera->getView() ) {
            if ( unsigned slaveIdx = slaveView->findSlaveIndexForCamera(camera.get()) ) {
                UTILS_ASSERT(false, "Not supported yet");
            }
        }
        // wyznaczamy rodzica, indeks oraz usuwamy
        UTILS_ASSERT(camera->getNumParents() == 1);
        parent = camera->getParent(0);
        childIdx = parent->getChildIndex(camera.get());
        if ( parent->asSwitch() ) {
            parent->asSwitch()->setValue(childIdx, false);
        } else {
            parent->removeChild(childIdx);
        }
    }


    //         if ( isSlave ) {
    //             UTILS_ASSERT(view.valid());
    //             if ( selected ) {
    //                 // dodajemy slave'a
    //                 view->addSlave( camera, slaveData._projectionOffset, slaveData._viewOffset, slaveData._useMastersSceneData );
    //             } else {
    //                 // kopiujemy dane i usuwamy
    //                 unsigned slaveIdx = view->findSlaveIndexForCamera(camera);
    //                 slaveData = view->getSlave(slaveIdx);
    //                 view->removeSlave(slaveIdx);
    //             }
    //         } else {
    //             UTILS_ASSERT(parent.valid());
    //             if ( parent->asSwitch() ) {
    //                 // w switchu mo¿na ³atwo w³¹czaæ/wy³¹czaæ
    //                 parent->asSwitch()->setValue(idx, selected);
    //             } else {
    //                 // z grupê trzeba niestety rêcznie kontrolowaæ
    //                 if ( selected ) {
    //                     parent->insertChild(idx, camera);
    //                 } else {
    //                     parent->removeChild(idx);
    //                 }
    //             }
    //         }
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