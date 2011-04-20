#include "PCH.h"
#include "uniqueCollection.h"
#include "SchemeDrawerContainer.h"

SchemeDrawerContainer::SchemeDrawerContainer() :
    nodes(new osg::Group()),
    wasInit(false)
{

}

osg::ref_ptr<osg::Node> SchemeDrawerContainer::getNode()
{
    return nodes;
}

void SchemeDrawerContainer::init( SkeletalVisualizationSchemeConstPtr scheme )
{
    OsgSchemeDrawer::init(scheme);
    wasInit = true;
    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
        drawer->init(scheme);
        nodes->addChild(drawer->getNode());
    }
}

void SchemeDrawerContainer::addDrawer( OsgSchemeDrawerPtr drawer )
{
    UTILS_ASSERT(!wasInit);
    pushBackUnique(drawers, drawer);
}

void SchemeDrawerContainer::removeDrawer( OsgSchemeDrawerPtr drawer )
{
    UTILS_ASSERT(false);
    // TODO jesli byl init to usun z kolekcji i noda
    //      jesli nie to tylko z kolekcji
    removeFromVector(drawers, drawer);
}

void SchemeDrawerContainer::deinit()
{
    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
        drawer->deinit();
    }
}

void SchemeDrawerContainer::draw()
{
    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
        drawer->draw();
    }
}

void SchemeDrawerContainer::update()
{
    BOOST_FOREACH(OsgSchemeDrawerPtr drawer, drawers) {
        drawer->update();
    }
}


