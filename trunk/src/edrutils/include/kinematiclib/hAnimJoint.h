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
	Joint joint;                               //!< staw
	hAnimBoneWeak parentBone;                  //!< kość, z której wychodzi staw
	std::vector<hAnimBonePtr> childrenBones;   //!< do jednego stawu moze być podłączone wiele kości
	bool active;                               //!< określa, czy staw jest aktywny (jeśli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
	osg::Vec3 localShift;                      //!< przesunięcie względem rodzica
	osg::Quat childParentRotation;             //!< rotacja względem rodzica
	hAnimJointWeak activeParent;               //!< najbliższy, aktywny parent
	std::vector<hAnimJointPtr> activeJointChildren; //!< najbliższe aktywne childy
	
public:
	/// \brief  Domyslny konstruktor, wszystkie stawy sa nieaktywne, dopiero podanie danych z parsera moze zmienic ten stan
	hAnimJoint(JointPtr joint) :
	  active(false)
	  {
		  // dla bezpieczeństwa zerowane sa dane o hierarchi
		  // szkielet h-anim powinien być stworzony od zera!
		  this->joint.parent.reset();
		  this->joint.children.clear();
		  Joint::copyContent(*joint, this->joint);
	  }

	  /// \brief domyślny konstruktor
	  hAnimJoint() :
	  active(false)
	  {

	  }

public: 
    //! ustawia nazwę stawu
	void setName(const std::string& name) { joint.name = name; }
    //! \return nazwa stawu
	const std::string& getName(void) const { return joint.name; }
    //! \return kość rodzica
	const hAnimBoneWeak& getParentBone(void) const { return(parentBone);		};
    //! ustawia kość rodzica
	void setParentBone(const hAnimBoneWeak& _parentBone) { parentBone = _parentBone;	};
    //! \return kolekcja z podlegającymi kośćmi
	std::vector<hAnimBonePtr>& getChildrenBones(void) { return(childrenBones);			};
    //! \return kolekcja z podlegającymi kośćmi
	const std::vector<hAnimBonePtr>& getChildrenBones(void) const { return(childrenBones);			};
    //! \return kolekcja z najbliższymi, aktywnymi i podległymi stawami
	std::vector<hAnimJointPtr>& getActiveJointChildren() { return activeJointChildren; }
    //! \return najbliższy aktywny rodzic
	kinematic::hAnimJointWeak getActiveParent() const { return activeParent; }
    //! ustawia najbliższego, aktywnego rodzica
	void setActiveParent(kinematic::hAnimJointWeak val) { activeParent = val; }
    //! \return przesunięcie względem rodzica
	osg::Vec3 getLocalShift() const { return localShift; }
    //! ustawia przesunięcie względem rodzica
	void setLocalShift(osg::Vec3 val) { localShift = val; }
    //! \return rotacja względem rodzica
	osg::Quat getChildParentRotation() const { return childParentRotation; }
    //! ustawia rotację względem rodzica
	void setChildParentRotation(osg::Quat val) { childParentRotation = val; }
    //! \return czy staw jest aktywny (jeśli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
	bool isActive() const { return this->active; }
    //! ustawia aktywność stawu
	void setActive(bool val) { this->active = val; }
    //! \return kierunek kości w globalnym układzie odniesienia
	osg::Vec3d getDirection() const { return joint.direction; }
    //! ustawia kierunek kości w globalnym układzie odniesienia
	void setDirection(osg::Vec3d val) { joint.direction = val; }
    //! \return długość kości
	double getLength() const { return joint.length; }
    //! ustawia długość kości
	void setLength(double val) { joint.length = val; }
    //! \return początkowa orientacja dla kości
	osg::Vec3d getAxis() const { return joint.axis; }
    //! ustawia początkową orientację dla kości
	void setAxis(osg::Vec3d val) { joint.axis = val; }
    //! \return kolejność rotacji
	Axis::Order getOrder() const { return joint.order; }
    //! ustawia kolejność rotacji
	void setOrder(Axis::Order val) { joint.order = val; }
    //! \return stopnie swobody dla kości (razem z limitami)
	std::vector<DegreeOfFreedom> getDofs() const { return joint.dofs; }
    //! ustawia stopnie swobody dla kości (razem z limitami)
	void setDofs(std::vector<DegreeOfFreedom> val) { joint.dofs = val; }
    //! \return masa ciała związana z tą kością
	double getBodymass() const { return joint.bodymass; }
    //! ustawia mesę ciała związaną z tą kością
	void setBodymass(double val) { joint.bodymass = val; }
    //! \return pozycja środka cieżkości względem kości
	double getCofmass() const { return joint.cofmass; }
    //! ustawia pozycję środka cieżkości względem kości
	void setCofmass(double val) { joint.cofmass = val; }
	//! ustawia staw hAnim na podstawie stawu wczytanego z pliku
	//! \param j wczytany staw
	void setJoint(const Joint& j) {
		auto jp = this->joint.parent;
		auto jc = this->joint.children;
		//this->joint = j;
		Joint::copyContent(j, this->joint);
		this->joint.parent = jp;
		this->joint.children = jc;
	}
	//! kopiuje zawartość stawu
	//! \param source staw źródłowy
	//! \param destination staw docelowy
	static void copyContent(const hAnimJoint& source, hAnimJoint& destination)
	{
		destination.setJoint(source.joint);
	}
};
}
#endif
