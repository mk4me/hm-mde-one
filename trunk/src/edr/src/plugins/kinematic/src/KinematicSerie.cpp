#include "PCH.h"
#include "KinematicSerie.h"

KinematicSerieBase::KinematicSerieBase() :
	matrixTransform(new osg::MatrixTransform())	
{

}	

KinematicSerieBase::~KinematicSerieBase()
{

}

KinematicSerieBase::MatrixTransformPtr KinematicSerieBase::getMatrixTransformNode()
{
	return matrixTransform;
}

void KinematicSerieBase::resetTransform()
{
	matrixTransform->setMatrix(osg::Matrix());
	innerResetTransform();
}

const osg::Vec3 KinematicSerieBase::pivotPoint() const
{
	return matrixTransform->getMatrix().getTrans();
}

KinematicTimeSerie::KinematicTimeSerie() : time(-1.0)
{

}

void KinematicTimeSerie::resetTransform()
{
	KinematicSerieBase::resetTransform();
    // ustawienie czasu (powinno odświeżyć)
    setLocalTime(getTime());
}

void KinematicTimeSerie::setTime( double val )
{
    time = val; 
    // ustawienie czasu w klasie pochodnej
    setLocalTime(val);
}

void KinematicTimeSerie::setEvents( EventsCollectionConstPtr val )
{
    events = val;
}
