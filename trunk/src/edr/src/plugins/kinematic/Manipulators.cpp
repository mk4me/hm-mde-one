#include "PCH.h"
#include "Manipulators.h"

void DraggerContainer::traverse( osg::NodeVisitor& nv )
{
    if ( _dragger.valid() )
    {
        if ( _active && nv.getVisitorType()==osg::NodeVisitor::CULL_VISITOR )
        {
            osgUtil::CullVisitor* cv = static_cast<osgUtil::CullVisitor*>(&nv);

            float pixelSize = cv->pixelSize(_dragger->getBound().center(), 0.48f);
            if ( pixelSize!=_draggerSize )
            {
                float pixelScale = pixelSize>0.0f ? _draggerSize/pixelSize : 1.0f;
                osg::Vec3d scaleFactor(pixelScale, pixelScale, pixelScale);

                osg::Vec3 trans = _dragger->getMatrix().getTrans();
                _dragger->setMatrix( osg::Matrix::scale(scaleFactor) * osg::Matrix::translate(trans) );
            }
        }
    }
    osg::Group::traverse(nv);
}

void DraggerContainer::setDragger( osgManipulator::Dragger* dragger )
{
    _dragger = dragger;
    if ( !containsNode(dragger) ) addChild( dragger );
}

DraggerContainer::DraggerContainer( const DraggerContainer& copy, const osg::CopyOp& copyop/*=osg::CopyOp::SHALLOW_COPY */ )
    :   osg::Group(copy, copyop),
    _dragger(copy._dragger), _draggerSize(copy._draggerSize), _active(copy._active)
{}

void DraggerContainer::setDraggerPivot( const osg::Vec3& pivot )
{
    //float scale = 0.20f;//scene->getBound().radius() * 1.6;
    //_dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) *  osg::Matrix::translate(pivot));
    _dragger->setMatrix(osg::Matrix::translate(pivot));
}

osg::Node* Manipulators::addDraggerToScene(osg::PositionAttitudeTransform* scene, Manipulators::DraggerType type, bool fixedSizeInScreen)
{
    scene->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    osg::MatrixTransform* selection = new osg::MatrixTransform;
    selection->addChild(scene);
    //osg::PositionAttitudeTransform* selection = scene;
    osgManipulator::Dragger* dragger = createDragger(type);
    

    osg::Group* root = new osg::Group;
    root->addChild(selection);

    if ( fixedSizeInScreen ) {
        DraggerContainer* draggerContainer = new DraggerContainer;
        draggerContainer->setDragger( dragger );
        root->addChild(draggerContainer);
    } else {
        root->addChild(dragger);
    }

    dragger->setMatrix(osg::Matrix::translate(scene->getBound().center()));

    dragger->addTransformUpdating(selection);

    // we want the dragger to handle it's own events automatically
    dragger->setHandleEvents(true);

    // if we don't set an activation key or mod mask then any mouse click on
    // the dragger will activate it, however if do define either of ActivationModKeyMask or
    // and ActivationKeyEvent then you'll have to press either than mod key or the specified key to
    // be able to activate the dragger when you mouse click on it.  Please note the follow allows
    // activation if either the ctrl key or the 'a' key is pressed and held down.
    //dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);
    //dragger->setActivationKeyEvent('b');
    dragger->setDraggerActive(false);
    return root;
}

osgManipulator::Dragger* Manipulators::createDragger( Manipulators::DraggerType type )
{
    osgManipulator::Dragger* dragger = nullptr;
    switch (type) {
        case TabPlane:          
            dragger = new osgManipulator::TabPlaneDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case TabPlaneTrackball: 
            dragger = new osgManipulator::TabPlaneTrackballDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case TabBoxTrackball:   
            dragger = new osgManipulator::TabBoxTrackballDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case Trackball:         
            dragger = new osgManipulator::TrackballDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case Translate1D:       
            dragger = new osgManipulator::Translate1DDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case Translate2D:       
            dragger = new osgManipulator::Translate2DDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case TranslateAxis:     
            dragger = new osgManipulator::TranslateAxisDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        case TabBox:                
            dragger = new osgManipulator::TabBoxDragger(); 
            dragger->setupDefaultGeometry(); 
            return dragger;

        default:
            UTILS_ASSERT(false);
    }
    
    return dragger;
}

void Manipulators::connect( osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer, const osg::Vec3& pivot )
{
    //scene->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);

    //osg::MatrixTransform* selection = new osg::MatrixTransform;
    //selection->addChild(scene);
    ////osg::PositionAttitudeTransform* selection = scene;
    //osgManipulator::Dragger* dragger = createDragger(name);


    //osg::Group* root = new osg::Group;
    //root->addChild(selection);

    //if ( fixedSizeInScreen ) {
    //    DraggerContainer* draggerContainer = new DraggerContainer;
    //    draggerContainer->setDragger( dragger );
    //    root->addChild(draggerContainer);
    //} else {
    //    root->addChild(dragger);
    //}
    parent->addChild(draggerContainer);
    osgManipulator::Dragger* dragger = draggerContainer->getDragger();
    //float scale = 0.20f;//scene->getBound().radius() * 1.6;
    //dragger->setMatrix(osg::Matrix::scale(scale, scale, scale) *  osg::Matrix::translate(pivot));
    draggerContainer->setDraggerPivot(pivot);

    dragger->addTransformUpdating(child);

    // we want the dragger to handle it's own events automatically
    dragger->setHandleEvents(true);
    
    // if we don't set an activation key or mod mask then any mouse click on
    // the dragger will activate it, however if do define either of ActivationModKeyMask or
    // and ActivationKeyEvent then you'll have to press either than mod key or the specified key to
    // be able to activate the dragger when you mouse click on it.  Please note the follow allows
    // activation if either the ctrl key or the 'a' key is pressed and held down.
    //dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);
    //dragger->setActivationKeyEvent('b');
    //dragger->setDraggerActive(false);
}

void Manipulators::disconnect( osg::PositionAttitudeTransform* parent, osg::MatrixTransform* child, DraggerContainer* draggerContainer )
{
    osgManipulator::Dragger* dragger = draggerContainer->getDragger();
    dragger->removeTransformUpdating(child);
    parent->removeChild(draggerContainer);
}

DraggerContainer* Manipulators::creatreDraggerContainer( Manipulators::DraggerType type )
{
    DraggerContainer* draggerContainer = new DraggerContainer;
    osgManipulator::Dragger* dragger = createDragger(type);
    draggerContainer->setDragger( dragger );
    return draggerContainer;
}


