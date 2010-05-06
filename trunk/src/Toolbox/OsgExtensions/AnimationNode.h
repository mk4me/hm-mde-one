#ifndef ANIMATION_NODE_H
#define ANIMATION_NODE_H


#include <osg/PositionAttitudeTransform>


//////////////////////////////////////////////////////////////////////////
// represents node holding animation - one frame
class CAnimationNode : public osg::PositionAttitudeTransform
{
		double _time;

	protected:

		// d - tor - legacy thing
		~CAnimationNode() {}

	public :

		// get time
		const double GetTime() { return _time; }

		// c - tor
		CAnimationNode(double time);
};

#endif