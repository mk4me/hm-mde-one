#include "stdafx.h"
#include <acclaimlib/DegreeOfFreedom.h>
#include <acclaimlib/Joint.h>
#include <acclaimlib/Skeleton.h>
#include <acclaimlib/SkeletalModel.h>

using namespace std;

namespace acclaim {

Skeleton::Skeleton():
    position(0.0, 0.0, 0.0),
    orientation(0.0, 0.0, 0.0),
    axisOrder(Axis::XYZ),
    dataOrder(Axis::XYZ),
    rootName("root")
{

}

osg::Quat SkeletonUtils::getQuatFromEuler( double heading, double attitude, double bank)
{
	double c1 = cos(heading/2);   
	double s1 = sin(heading/2);   
	double c2 = cos(attitude/2);   
	double s2 = sin(attitude/2);   
	double c3 = cos(bank/2);   
	double s3 = sin(bank/2);
	double c1c2 = c1*c2;  
	double s1s2 = s1*s2;  

	double w =c1c2*c3 - s1s2*s3;
	double x =c1c2*s3 + s1s2*c3;
	double y =s1*c2*c3 + c1*s2*s3;

	double z =c1*s2*c3 - s1*c2*s3;

	return osg::Quat(x, y, z, w);
}

osg::Quat SkeletonUtils::getQuatFromEuler( const osg::Vec3& euler )
{
	return getQuatFromEuler(euler[0], euler[1], euler[2]);
}

osg::Vec3 SkeletonUtils::getEulerFromQuat(const osg::Quat& q )
{
	double heading, attitude, bank;

	double limit = 0.499999;
	double sqx = q.x()*q.x();   
	double sqy = q.y()*q.y();   
	double sqz = q.z()*q.z();

	double t = q.x()*q.y() + q.z()*q.w();
	// gimbal lock ?
	if (t > limit) {
		heading = 2 * atan2(q.x(),q.w());
		attitude = osg::PI_2;
		bank = 0;
	} else if (t < -limit) {
		heading = -2 * atan2(q.x(),q.w());
		attitude = - osg::PI_2;
		bank = 0;
	} else {
		heading = atan2(2*q.y()*q.w()-2*q.x()*q.z() , 1 - 2*sqy - 2*sqz);
		attitude = asin(2*t);
		bank = atan2(2*q.x()*q.w()-2*q.y()*q.z() , 1 - 2*sqx - 2*sqz);
	}

	return osg::Vec3(heading, attitude, bank);
}

double SkeletonUtils::getMaxBoneLength(const Skeleton& skeleton) 
{
	JointConstPtr root = skeleton.getRoot();
	return getMaxLength(root, -1.0);
}

double SkeletonUtils::getMaxLength(const JointConstPtr & joint, double maxLength)
{
	maxLength = std::max(maxLength, joint->length);
	for (int i = joint->children.size() - 1; i >= 0; --i) {
		maxLength = std::max(maxLength, getMaxLength(joint->children[i], maxLength));
	}
	return maxLength;
}

osg::Vec3 SkeletonUtils::vectorRotation( osg::Vec3 v, double rX, double rY, double rZ )
{
	osg::Quat rotz; rotz.makeRotate(rZ, 0,0,1);
	osg::Quat roty; roty.makeRotate(rY, 0,1,0);
	osg::Quat rotx; rotx.makeRotate(rX, 1,0,0);
	v = rotz * v;
	v = roty * v;
	v = rotx * v;
	return v;
}

osg::Quat SkeletonUtils::rotationParentChild(hAnimJointPtr parent, hAnimJointPtr child)
{
	double mul  = osg::DegreesToRadians(1.0);
	osg::Matrix matParZ; matParZ.makeRotate(-parent->getAxis()[2] * mul, 0.0, 0.0, 1.0);
	osg::Matrix matParY; matParY.makeRotate(-parent->getAxis()[1] * mul, 0.0, 1.0, 0.0);
	osg::Matrix matParX; matParX.makeRotate(-parent->getAxis()[0] * mul, 1.0, 0.0, 0.0);

	osg::Matrix matPar = matParZ * matParY * matParX;
	osg::Matrix matChiZ; matChiZ.makeRotate(child->getAxis()[2] * mul, 0.0, 0.0, 1.0);
	osg::Matrix matChiY; matChiY.makeRotate(child->getAxis()[1] * mul, 0.0, 1.0, 0.0);
	osg::Matrix matChiX; matChiX.makeRotate(child->getAxis()[0] * mul, 1.0, 0.0, 0.0);

	osg::Matrix matChi = matChiX * matChiY * matChiZ;
	osg::Matrix resM = matChi * matPar ;
	osg::Quat res; 
	res.set(resM);
	return res;
}

osg::Quat SkeletonUtils::createRotation( const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order )
{
	switch (order) 
	{
	case Axis::UnknownAxisOrder:
	case Axis::XYZ: return rX * rY * rZ;
	case Axis::XZY: return rX * rZ * rY;
	case Axis::YXZ: return rY * rX * rZ;
	case Axis::YZX: return rY * rZ * rX;
	case Axis::ZXY: return rZ * rX * rY;
	case Axis::ZYX: return rZ * rY * rX;
	default:
		UTILS_ASSERT(false);
		osg::Quat q;
		return q;
	}
}

} // acclaim
