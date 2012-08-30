#ifndef HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec4>
#include <tinyxml.h>
#include <utils/Debug.h>
#include <utils/DataChannelCollection.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/hAnimBone.h>
#include <kinematiclib/hAnimJoint.h>
#include <kinematiclib/SkeletalMappingScheme.h>
#include <kinematiclib/hAnimSkeleton.h>
#include <kinematiclib/QuaternionManipulators.h>
#include <kinematiclib/SkeletalModel.h>

namespace kinematic 
{
//! Klasa reprezentuje kanał dla pojedynczego stawu, przechowuje rotacje względne w postaci kwaternionów
typedef utils::Channel<osg::Quat> JointAngleChannel;

typedef boost::shared_ptr<JointAngleChannel> JointAngleChannelPtr;
typedef boost::shared_ptr<const JointAngleChannel> JointAngleChannelConstPtr;

/// \brief  Klasa dostarcza reprezentacje wewnętrzną szkieletu.
/// \details Zawiera znormalizowane dane : rotacje jako kwaterniony, długość kości z zakresu <0,1>, nazewnictwo h-anim
class JointAnglesCollection : public utils::DataChannelCollection<JointAngleChannel>
{
public:
    JointAnglesCollection(void);
	virtual ~JointAnglesCollection() {}

public:
    //! \return głęboka kopia obiektu
	virtual JointAnglesCollection* clone() const;
	
public: 
    //! Ustawia dane z parsera, w tym miejscu tworzony jest pełny szkielet h-anim, robiona jest normalizacja danych
    //! \param skeletalModel struktura szkieletu
    //! \param skeletalData dane szkieletu
    void setSkeletal(kinematic::SkeletalModelConstPtr skeletalModel, kinematic::SkeletalDataConstPtr skeletalData);
    //! Ustawia dane z parsera, w tym miejscu tworzony jest pełny szkielet h-anim, robiona jest normalizacja danych
    //! \param skeletalModel struktura szkieletu w postaci pełnego szkieletu h-anim
    //! \param rootPositions kolekcja danych pozycji korzenia szkieletu
    //! \param channels kolekcja z danymi w postaci DataChannel
    void setSkeletal(kinematic::hAnimSkeletonPtr skeletalModel, const std::vector<osg::Vec3>& rootPositions, const std::vector<JointAngleChannelPtr>& channels);
    //! \brief zwraca szkielet zgodny z h-anim
    const kinematic::hAnimSkeletonPtr & getHAnimSkeleton() const { return haSkeleton; }
	
   	//! zwraca indeks kanału lub rzuca wyjątkiem w przypadku niepowodzenia
   	//! \param name nazwa szukanego kanału
    //! \return indeks kanału
   	int getIndex(const std::string& name) const;
    //! zwraca indeks kanału lub -1 wyjątkiem w przypadku niepowodzenia
    //! \param name nazwa szukanego kanału
    //! \return indeks kanału
	int tryGetIndex(const std::string& name) const;
    //! zwraca pozycję roota
    //! \param frame klatka, dla której ma być zwrócona pozycja
    //! \return pozycja  
    osg::Vec3 getRootPosition(int frame) const;
    //! zwraca pozycję roota
    //! \param time czas, dla którego ma być zwrócona pozycja
    //! \return pozycja  
    osg::Vec3 getRootPosition(double time) const;
    //! \return zwraca kolekcję z wszystkimi pozycjami roota na przestrzeni czasu
    const std::vector<osg::Vec3>& getRootPositions() const { return rootPositions; }
    //! \return długość przez którą należy pomnożyc aby uzyskać początkowe długości kości
	double getLengthRatio() const { return lengthRatio; }
    //! ustawia długość przez którą należy pomnożyc aby uzyskać początkowe długości kości
	void setLengthRatio(double val) { lengthRatio = val; }

private:
    /// \brief  Na podstawie danych z parsera tworzy tablicę z kwaternionami
    void createQuaternionRepresentation(kinematic::SkeletalModelConstPtr& skeletalModel, SkeletalDataConstPtr & skeletalData);
    /// \brief  Wyszukuje i zwraca długość najdłuższej z kości w szkielecie
    /// \param  skeleton  Przeszukiwany szkielet
    /// \return długość najdłuższej z kości. 
    double getMaxBoneLength(const Skeleton& skeleton) const;
    /// \brief zwraca maksymalną długość kości
    /// \param  joint staw, dostarczający hierarchię 
    /// \param  maxLength dotychczasowa, najdłuższa kość
    /// \return wyliczona długość. 
    double getMaxLength(const JointConstPtr & joint, double maxLength) const;
    //! obraca wektor o kąty eulera
    //! \param v obracany wektor
    //! \param rX kąt X
    //! \param rY kąt Y
    //! \param rZ kąt Z
    //! \return obliczony wektor
    osg::Vec3 vectorRotation( osg::Vec3 v, double rX, double rY, double rZ);
    //! zwraca rotację względną pozmiędzy rodzicem a dzieckiem
    //! \param parent rodzic, dla którego liczona jest rotacja
    //! \param child dziecko, dla którego liczona jest rotacja
    //! \return kwaternion z obliczoną rotacją
    osg::Quat rotationParentChild(hAnimJointPtr parent, hAnimJointPtr child);
    //! Tworzy rotację na podstawie kątów
    //! \param rX kąt X
    //! \param rY kąt Y
    //! \param rZ kąt Z
    //! \param order kolejność kątów
    //! \return wyliczony kwaternion
    osg::Quat createRotation(const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order);
	
private:
	//! pełny szkielet h-anim                          
    kinematic::hAnimSkeletonPtr haSkeleton;
	//! pozycje dla roota
    std::vector<osg::Vec3> rootPositions;
	//! długość przez którą należy pomnożyc aby uzyskać początkowe długości kości            
    double lengthRatio;
    //! czy obiekt jest już zainicjalizowany
    bool initialized;
}; 

typedef boost::shared_ptr<JointAnglesCollection> JointAnglesCollectionPtr;
typedef boost::shared_ptr<const kinematic::JointAnglesCollection> JointAnglesCollectionConstPtr;
}

#endif
