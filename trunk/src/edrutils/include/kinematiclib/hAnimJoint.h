/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:25
	filename: 	hAnimJoint.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__HANIMJOINT_H__
#define HEADER_GUARD_KINEMATIC__HANIMJOINT_H__

#include <osg/Vec3>
#include <kinematiclib/Joint.h>
#include <kinematiclib/hAnimBone.h>

namespace kinematic {

class hAnimJoint;
typedef boost::shared_ptr<hAnimJoint> hAnimJointPtr;
typedef boost::shared_ptr<const hAnimJoint> hAnimJointConstPtr;
typedef boost::weak_ptr<hAnimJoint> hAnimJointWeak;
/// \brief Klasa reprezentuje staw standardu h-anim
class hAnimJoint : boost::noncopyable 
{
	friend class JointAnglesCollection;
private :
	Joint joint;
	hAnimBoneWeak parentBone;                  //!< kosc, z ktorej wychodzi staw
	std::vector<hAnimBonePtr> childrenBones;   //!< do jednego stawu moze byc podlaczone wiele kosci
	bool active;                                 //!< okresla, czy staw jest aktywny (jesli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
	osg::Vec3 localShift;
	osg::Quat childParentRotation;
	hAnimJointWeak activeParent;
	std::vector<hAnimJointPtr> activeJointChildren;
	
public:
	/// \brief  Domyslny konstruktor, wszystkie stawy sa nieaktywne, dopiero podanie danych z parsera moze zmienic ten stan
	hAnimJoint(JointPtr joint) :
	  active(false)
	  {
		  // dla bezpieczenstwa zerowane sa dane o hierarchi
		  // szkielet h-anim powinien byc stworzony od zera!
		  this->joint.parent.reset();
		  this->joint.children.clear();
		  Joint::copyContent(*joint, this->joint);
	  }

	  /// \brief domyslny konstruktor
	  hAnimJoint() :
	  active(false)
	  {

	  }

public: // akcesory
	void setName(const std::string& name)                   { joint.name = name; }
	const std::string& getName(void) const                  { return joint.name; }
	const hAnimBoneWeak& getParentBone(void) const		    { return(parentBone);		};
	void setParentBone(const hAnimBoneWeak& _parentBone)	{ parentBone = _parentBone;	};
	std::vector<hAnimBonePtr>& getChildrenBones(void)       { return(childrenBones);			};
	const std::vector<hAnimBonePtr>& getChildrenBones(void) const { return(childrenBones);			};

	std::vector<hAnimJointPtr>& getActiveJointChildren() { return activeJointChildren; }
	kinematic::hAnimJointWeak getActiveParent() const { return activeParent; }
	void setActiveParent(kinematic::hAnimJointWeak val) { activeParent = val; }

	osg::Vec3 getLocalShift() const { return localShift; }
	void setLocalShift(osg::Vec3 val) { localShift = val; }
	osg::Quat getChildParentRotation() const { return childParentRotation; }
	void setChildParentRotation(osg::Quat val) { childParentRotation = val; }

	bool isActive() const { return this->active; }
	void setActive(bool val) { this->active = val; }
	osg::Vec3d getDirection() const { return joint.direction; }
	void setDirection(osg::Vec3d val) { joint.direction = val; }
	double getLength() const { return joint.length; }
	void setLength(double val) { joint.length = val; }
	osg::Vec3d getAxis() const { return joint.axis; }
	void setAxis(osg::Vec3d val) { joint.axis = val; }
	Axis::Order getOrder() const { return joint.order; }
	void setOrder(Axis::Order val) { joint.order = val; }
	std::vector<DegreeOfFreedom> getDofs() const { return joint.dofs; }
	void setDofs(std::vector<DegreeOfFreedom> val) { joint.dofs = val; }
	double getBodymass() const { return joint.bodymass; }
	void setBodymass(double val) { joint.bodymass = val; }
	double getCofmass() const { return joint.cofmass; }
	void setCofmass(double val) { joint.cofmass = val; }

	void setJoint(const Joint& j) {
		auto jp = this->joint.parent;
		auto jc = this->joint.children;
		//this->joint = j;
		Joint::copyContent(j, this->joint);
		this->joint.parent = jp;
		this->joint.children = jc;
	}

	static void copyContent(const hAnimJoint& source, hAnimJoint& destination)
	{
		destination.setJoint(source.joint);
	}
};
}
#endif
