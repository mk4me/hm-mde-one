#include "PCH.h"
#include <osgui/DisableableRenderer.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
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
} // namespace osgUI
////////////////////////////////////////////////////////////////////////////////
