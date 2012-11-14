#include "PCH.h"
#include "KinematicSerie.h"

KinematicSerie::KinematicSerie() :
transformNode(new osg::PositionAttitudeTransform()),
    matrixTransform(new osg::MatrixTransform()),
    time(0.0)
{
    // węzeł obsługiwany przez manipulatory jest parentem dla wezja wewnetrznego serii
    matrixTransform->addChild(transformNode);
}

void KinematicSerie::resetTransform()
{
    // zerowanie pracy manipulatorów
    MatrixTransformPtr transform = getMatrixTransformNode();
    // pobieramy podstawowa macierz (dla czasu 0)
    transform->setMatrix(getInitialMatrix());
    // ustawienie czasu (powinno odświeżyć)
    setLocalTime(getTime());
}

void KinematicSerie::setTime( double val )
{
    time = val; 
    // ustawienie czasu w klasie pochodnej
    setLocalTime(val);
}

osg::Vec3 KinematicSerie::getPivot() const
{
    // w najprostszym wariancie, pivot pobieramy od razu z macierzy
    // klasy pochodne moga to robic w inny sposób
    auto matrix = matrixTransform->getMatrix();
    auto t1 = matrix.getTrans();
    return t1;
}

void KinematicSerie::setEvents( EventsCollectionConstPtr val )
{
    events = val;
}

KinematicSerie::MatrixTransformPtr KinematicSerie::getMatrixTransformNode()
{
    return matrixTransform;
}
