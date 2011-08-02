#ifndef HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Vec3>
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
	
//! Klasa reprezentuje kanal dla pojedynczego stawu, przechowuje rotacje wzgledne w postaci kwaternionow
typedef utils::BaseChannel<osg::Quat, float, QuaternionManipulatorSlerp> JointAngleChannel;
typedef boost::shared_ptr<JointAngleChannel> JointAngleChannelPtr;
typedef boost::shared_ptr<const JointAngleChannel> JointAngleChannelConstPtr;

/// \brief  Klasa dostarcza reprezentacje wewnetrzna szkieletu.
/// \details Zawiera znormalizowane dane : rotacje jako kwaterniony, dlugosc kosci z zakresu <0,1>, nazewnictwo h-anim
class JointAnglesCollection : public utils::DataChannelCollection<JointAngleChannel>
{
public:
    JointAnglesCollection(void);
	virtual ~JointAnglesCollection() {}

public:
	virtual JointAnglesCollection* clone() const;
	
public:
    /// \brief  Zwraca surowe dane z parsera
    const kinematic::SkeletalModelPtr & getSkeletalData() const { return skeletalModel; }
	//! Tworzy dane zgodne z parserami na podstawie reprezentacji wewnetrznej
	//kinematic::SkeletalModelPtr createSkeletalData() const;
    /// \brief  Ustawia dane z parsera
    /// \details W tym miejscu tworzony jest pe�ny szkielet h-anim, robiona jest normalizacja danych
    /// \param  skeletalModel   The skeletal model. 
    void setSkeletal(const kinematic::SkeletalModelPtr & skeletalModel, const kinematic::SkeletalDataPtr & skeletalData);
    //! \brief zwraca szkielet zgodny z h-anim
    const kinematic::hAnimSkeletonPtr & getHAnimSkeleton() const { return haSkeleton; }
	
    bool hasSkeleton() const {
        return (skeletalModel);
    }

   	int getIndex(const std::string& name) const;
	int tryGetIndex(const std::string& name) const;

    osg::Vec3 getRootPosition(int frame) const;
    osg::Vec3 getRootPosition(double time) const;

private:
    /// \brief  Na podstawie danych z parsera tworzy tablice z kwaternionami
    void createQuaternionRepresentation(const SkeletalDataPtr & skeletalData);
    /// \brief  Wyszukuje i zwraca dlugosc najdluzszej z kosci w szkielecie
    /// \param  skeleton  Przeszukiwany szkielet
    /// \return Dlugosc najdluzszej z kosci. 
    double getMaxBoneLength(const Skeleton& skeleton) const;
    /// \brief  Gets a maximum length.
    /// \param  joint       The joint. 
    /// \param  maxLength   Length of the maximum. 
    /// \return The maximum length. 
    double getMaxLength(const JointConstPtr & joint, double maxLength) const;

    osg::Vec3 vectorRotation( osg::Vec3 v, double rX, double rY, double rZ);
    osg::Quat rotationParentChild(hAnimJointPtr parent, hAnimJointPtr child);
    osg::Quat createRotation(const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order);
	
private:
	//! dane z parsera acclaim / biovision
    kinematic::SkeletalModelPtr skeletalModel;
	//! dane z parsera acclaim / biovision
	kinematic::SkeletalDataPtr skeletalData;
	//! pelny szkielet h-anim                          
    kinematic::hAnimSkeletonPtr haSkeleton;
	//! pozycje dla roota
    std::vector<osg::Vec3> rootPositions;
	//! dlugosc przez ktora nalezy pomnozyc aby uzyskac poczatkowe dlugosci kosci            
    double lengthRatio;
}; 

typedef boost::shared_ptr<JointAnglesCollection> JointAnglesCollectionPtr;
typedef boost::shared_ptr<const kinematic::JointAnglesCollection> JointAnglesCollectionConstPtr;
}

#endif