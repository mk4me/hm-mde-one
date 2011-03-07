#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <stack>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/filesystem.hpp>

#include <utils/Utils.h>
UTILS_PUSH_WARNINGS
#include <osg/Node>
#include <osg/AnimationPath>
#include <osg/Config>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/ImageStream>
#include <osg/Node>
#include <osg/NodeVisitor>
#include <osg/Notify>
#include <osg/observer_ptr>
#include <osg/PositionAttitudeTransform>
#include <osg/PrimitiveSet>
#include <osg/Quat>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ShapeDrawable>
#include <osg/Timer>
#include <osg/Vec3>
#include <osg/Vec3d>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers>

#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>

#include <osgDB/ReadFile>
    UTILS_POP_WARNINGS

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtOpenGL/QtOpenGL>

#include <utils/Debug.h>
#include <utils/Enum.h>
#include "DisableableRenderer.h"

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void DisableableRenderer::operator()( osg::GraphicsContext* ctx )
{
    if ( renderEnabled ) {
        osgViewer::Renderer::operator()(ctx);
    } else {
        osgUtil::SceneView* sceneView = _sceneView[0].get();
        if ( !sceneView || _done /*|| _graphicsThreadDoesCull*/ )
            return;

        // te wywo³ania s¹ potrzebne, inaczej np. statystyki siê przewracaj¹.
        updateSceneView( sceneView );
        osgViewer::View* view = dynamic_cast<osgViewer::View*>( _camera->getView() );
        if ( view ) {
            sceneView->setFusionDistance( view->getFusionDistanceMode(), view->getFusionDistanceValue() );
        }
        sceneView->inheritCullSettings( *(sceneView->getCamera()) );
        sceneView->cull();
    }
}


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
