#ifndef CUSTOM_POSITION_ATTITUDE_TRANSFORM_H
#define CUSTOM_POSITION_ATTITUDE_TRANSFORM_H

#include <osg/PositionAttitudeTransform>


//////////////////////////////////////////////////////////////////////////
// adds to PositionAttitudeTransform field id
class CCustomPositionAttitudeTransform : public osg::PositionAttitudeTransform
{
		// id
		int m_id;

	public:

		// set id
		void setId(int id)	{ m_id = id; }

		// get id
		int  getId()		{ return m_id; }
	
		// c - tor
		CCustomPositionAttitudeTransform();

	protected:

		// d - tor
		virtual ~CCustomPositionAttitudeTransform();
};

#endif