/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:28
	filename: 	QuaternionManipulators.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__QUATERNIONMANIPULATORS_H__
#define HEADER_GUARD_KINEMATIC__QUATERNIONMANIPULATORS_H__

namespace kinematic {

	class QuaternionManipulator
	{
	public:
		virtual bool isLower(const osg::Quat& q1, const osg::Quat& q2)
		{
			return q1 < q2;
		}
		virtual osg::Quat normalize(const osg::Quat& q, const osg::Quat& min, const osg::Quat& max) const
		{
			UTILS_ASSERT("Unable to normalize quaternion");
			throw std::runtime_error("Unable to normalize quaternion");
		}
		virtual osg::Quat interpolate(const osg::Quat& q1, const osg::Quat& q2, float t) const = 0;
	};

	class QuaternionManipulatorSlerp : public QuaternionManipulator
	{
	public:
		virtual osg::Quat interpolate( const osg::Quat& q1, const osg::Quat& q2, float t ) const
		{
			osg::Quat q;
			q.slerp(t, q1, q2);
			return q;
		}
	};

	class QuaternionManipulatorLerp : public QuaternionManipulator
	{
	public:
		virtual osg::Quat interpolate( const osg::Quat& q1, const osg::Quat& q2, float t ) const
		{
			osg::Quat q;
			q._v[0] = q1._v[0] * (1.0f - t)  + q2._v[0] * t;
			q._v[1] = q1._v[1] * (1.0f - t)  + q2._v[1] * t;
			q._v[2] = q1._v[2] * (1.0f - t)  + q2._v[2] * t;
			q._v[3] = q1._v[3] * (1.0f - t)  + q2._v[3] * t;
			return q;
		}
	};

	class QuaternionManipulatorNoInterpolation : public QuaternionManipulator
	{
	public:
		virtual osg::Quat interpolate( const osg::Quat& q1, const osg::Quat& q2, float t  ) const
		{
			return q1;
		}
	};

}

#endif
