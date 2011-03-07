#include "StdAfx.h"
#include <vdfmlib/osgVDFModel.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

float LinearSelectionFactory::selectionWidth = 1.0;

float LinearConnectionFactory::connectingWidth = 1.0;
float LinearConnectionFactory::connectedWidth = LinearConnectionFactory::connectingWidth;

osgWidget::XYCoord BezierConnectionFactory::dirOut = osgWidget::XYCoord(1,0);
osgWidget::XYCoord BezierConnectionFactory::dirIn = osgWidget::XYCoord(-1,0);
unsigned int BezierConnectionFactory::interpCount = 100;
float BezierConnectionFactory::coeffMul = 100;
float BezierConnectionFactory::connectingWidth = 1.0;
float BezierConnectionFactory::connectedWidth = BezierConnectionFactory::connectingWidth;

}