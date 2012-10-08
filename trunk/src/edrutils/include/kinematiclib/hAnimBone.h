/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:24
	filename: 	hAnimBone.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__HANIMBONE_H__
#define HEADER_GUARD_KINEMATIC__HANIMBONE_H__

namespace kinematic {

class hAnimJoint;
typedef boost::shared_ptr<hAnimJoint> hAnimJointPtr;
typedef boost::weak_ptr<hAnimJoint> hAnimJointWeak;


/// \brief Klasa reprezentuje segment standardu h-anim
/// Klasa nie jest powiązana z danymi, pomaga jedynie budować hierarchie
class hAnimBone : boost::noncopyable
{
private:
	//! nazwa kości (powinna być taka jak w h-anim 1.1)
	std::string name; 
	//! do jednej kości moze być podłączone wiele stawów                          
	std::vector<hAnimJointPtr> childrenJoints;  
	//! staw, z którego wychodzi kość     
	hAnimJointWeak parentJoint;                 
	
public:
	//! \return nazwa kości
	const std::string& getName() const { return name; }
	//! ustawia nazwę danej kości
	//! \param val nowa nazwa
	void setName(const std::string& val) { name = val; }
	//! zwraca tablice ze stawami, które kończą kość (segment)
	std::vector<hAnimJointPtr>& getChildrenJoints() { return childrenJoints; }
	//! pobiera staw h-anim, który jest rodzicem danej kości
	kinematic::hAnimJointWeak getParentJoint() const { return parentJoint; }
	//! ustawia rodzica kości
	//! \param val staw h-anim
	void setParentJoint(kinematic::hAnimJointWeak val) { parentJoint = val; }
};
typedef boost::shared_ptr<hAnimBone> hAnimBonePtr;
typedef boost::weak_ptr<hAnimBone> hAnimBoneWeak;

}

#endif
