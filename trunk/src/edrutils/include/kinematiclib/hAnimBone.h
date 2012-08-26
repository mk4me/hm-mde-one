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
/// Klasa nie jest powiazana z danymi, pomaga jedynie budowac hierarchie
class hAnimBone : boost::noncopyable
{
private:
	//! nazwa kosci (powinna byc taka jak w h-anim 1.1)
	std::string name; 
	//! do jednej kosci moze byc podlaczone wiele stawow                          
	std::vector<hAnimJointPtr> childrenJoints;  
	//! staw, z ktorego wychodzi kosc     
	hAnimJointWeak parentJoint;                 
	
public:
	//! \return nazwa kosci
	const std::string& getName() const { return name; }
	//! ustawia nazwe danej kosci
	//! \param val 
	void setName(const std::string& val) { name = val; }
	//! zwraca tablice ze stawami, ktore koncza kosc (segment)
	std::vector<hAnimJointPtr>& getChildrenJoints() { return childrenJoints; }
	//! pobiera staw h-anim, kotry jest rodzicem danej kosci
	kinematic::hAnimJointWeak getParentJoint() const { return parentJoint; }
	//! ustawia rodzica kosci
	//! \param val staw h-anim
	void setParentJoint(kinematic::hAnimJointWeak val) { parentJoint = val; }
};
typedef boost::shared_ptr<hAnimBone> hAnimBonePtr;
typedef boost::weak_ptr<hAnimBone> hAnimBoneWeak;

}

#endif
