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
#include <osg/Quat>
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
public:
    ENABLE_PRIVATE_TESTS
	//friend class JointAnglesCollection;
private :
    //! staw
	//Joint joint;

	//! Nazwa jointa
	std::string name;
    //! kość, z której wychodzi staw 
	hAnimBoneWeak parentBone;                 
    //! do jednego stawu moze być podłączone wiele kości 
	std::vector<hAnimBonePtr> childrenBones;  
    //! określa, czy staw jest aktywny (jeśli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1) 
	bool active;
    //! najbliższy, aktywny parent 
	hAnimJointWeak activeParent;           
    //! najbliższe aktywne childy 
	std::vector<hAnimJointPtr> activeJointChildren;
	//! Kierunek stawu
	osg::Vec3d direction;
	//! Inicjalny kierunek stawu
	osg::Vec3d axis;
	//! Kolejnosc katow dla rotacji
	Axis::Order order;
	//! Stopnie swobody
	std::vector<DegreeOfFreedom> dofs;
	//! rotacja względem rodzica 
	osg::Quat childParentRotation;
	//! przesunięcie względem rodzica 
	osg::Vec3 localShift;
	
public:
	/// \brief  Domyslny konstruktor, wszystkie stawy sa nieaktywne, dopiero podanie danych z parsera moze zmienic ten stan
	/*hAnimJoint(JointPtr joint) :
	  active(false)
	  {
		  // dla bezpieczeństwa zerowane sa dane o hierarchi
		  // szkielet h-anim powinien być stworzony od zera!
		  this->joint.parent.reset();
		  this->joint.children.clear();
		  Joint::copyContent(*joint, this->joint);
	  }
	  */

	  /// \brief domyślny konstruktor
	  hAnimJoint() :
	  active(false)
	  {

	  }

public: 

	//! \return przesunięcie względem rodzica
	osg::Vec3 getLocalShift() const { return localShift; }
	//! ustawia przesunięcie względem rodzica
	void setLocalShift(osg::Vec3 val) { localShift = val; }

	//! \return kierunek kości w globalnym układzie odniesienia
	osg::Vec3d getDirection() const { return direction; }
	//! ustawia kierunek kości w globalnym układzie odniesienia
	void setDirection(osg::Vec3d val) { direction = val; }
	//! \return początkowa orientacja dla kości
	osg::Vec3d getAxis() const { return axis; }
	//! ustawia początkową orientację dla kości
	void setAxis(osg::Vec3d val) { axis = val; }

	//! \return rotacja względem rodzica
	osg::Quat getChildParentRotation() const { return childParentRotation; }
	//! ustawia rotację względem rodzica
	void setChildParentRotation(osg::Quat val) { childParentRotation = val; }

    //! ustawia nazwę stawu
	void setName(const std::string& name) { this->name = name; }
    //! \return nazwa stawu
	const std::string& getName() const { return name; }
    //! \return kość rodzica
	const hAnimBoneWeak& getParentBone() const { return(parentBone);		};
    //! ustawia kość rodzica
	void setParentBone(const hAnimBoneWeak& _parentBone) { parentBone = _parentBone;	};
    //! \return kolekcja z podlegającymi kośćmi
	std::vector<hAnimBonePtr>& getChildrenBones() { return(childrenBones);			};
    //! \return kolekcja z podlegającymi kośćmi
	const std::vector<hAnimBonePtr>& getChildrenBones() const { return(childrenBones);			};
    //! \return kolekcja z najbliższymi, aktywnymi i podległymi stawami
	std::vector<hAnimJointPtr>& getActiveJointChildren() { return activeJointChildren; }
    //! \return najbliższy aktywny rodzic
	kinematic::hAnimJointWeak getActiveParent() const { return activeParent; }
    //! ustawia najbliższego, aktywnego rodzica
	void setActiveParent(kinematic::hAnimJointWeak val) { activeParent = val; }
    //! \return czy staw jest aktywny (jeśli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
	bool isActive() const { return this->active; }
    //! ustawia aktywność stawu
	void setActive(bool val) { this->active = val; }
    //! \return kolejność rotacji
	Axis::Order getOrder() const { return order; }
    //! ustawia kolejność rotacji
	void setOrder(Axis::Order val) { order = val; }
    //! \return stopnie swobody dla kości (razem z limitami)
	std::vector<DegreeOfFreedom> getDofs() const { return dofs; }
    //! ustawia stopnie swobody dla kości (razem z limitami)
	void setDofs(std::vector<DegreeOfFreedom> val) { dofs = val; }
	//! ustawia staw hAnim na podstawie stawu wczytanego z pliku
	//! \param j wczytany staw
	
	//void setJoint(const Joint& j) {
		//auto jp = this->joint.parent;
		//auto jc = this->joint.children;
		//this->joint = j;
		//Joint::copyContent(j, this->joint);
		//this->joint.parent = jp;
		//this->joint.children = jc;
	//}

	//! kopiuje zawartość stawu
	//! \param source staw źródłowy
	//! \param destination staw docelowy
	static void copyContent(const hAnimJoint& source, hAnimJoint& destination)
	{
		//destination.setJoint(source.joint);
        destination.active = source.active;
        //destination.localShift = source.localShift;                   
        destination.childParentRotation = source.childParentRotation;
	}
};
}
#endif
