#ifndef HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATIC__KINEMATICMODEL_H__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Vec3>
#include <osg/Vec4>
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
    ENABLE_PRIVATE_TESTS

    JointAnglesCollection(void);
	virtual ~JointAnglesCollection() {}

public:
    //! \return głęboka kopia obiektu
	virtual JointAnglesCollection* clone() const;
	
public: 
    //! Ustawia dane z parsera, w tym miejscu tworzony jest pełny szkielet h-anim, robiona jest normalizacja danych
    //! \param skeletalModel struktura szkieletu
    //! \param skeletalData dane szkieletu
	void setSkeletal(kinematic::SkeletalModelConstPtr skeletalModel, kinematic::SkeletalDataConstPtr skeletalData, 
		SkeletonMappingSchemePtr mapping = SkeletonMappingScheme::createDefault());
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


class JointAnglesCollectionStream
{
public:
	//! Konstruktor domyślny
	JointAnglesCollectionStream();
	//! Destruktor
	~JointAnglesCollectionStream();

public: 
	//! Ustawia dane z parsera, w tym miejscu tworzony jest pełny szkielet h-anim, robiona jest normalizacja danych
	//! \param skeletalModel struktura szkieletu
	//! \param skeletalData dane szkieletu
	void setSkeletal(kinematic::SkeletalModelConstPtr skeletalModel, kinematic::SkeletalDataStreamPtr skeletalDataStream);
	//! Ustawia dane z parsera, w tym miejscu tworzony jest pełny szkielet h-anim, robiona jest normalizacja danych
	//! \param skeletalModel struktura szkieletu w postaci pełnego szkieletu h-anim
	//! \param rootPositions kolekcja danych pozycji korzenia szkieletu
	//! \param channels kolekcja z danymi w postaci DataChannel
	void setSkeletal(kinematic::hAnimSkeletonPtr skeletalModel, kinematic::SkeletalDataStreamPtr skeletalDataStream);
	//! \brief zwraca szkielet zgodny z h-anim
	const kinematic::hAnimSkeletonPtr & getHAnimSkeleton() const { return haSkeleton; }
	//! \return długość przez którą należy pomnożyc aby uzyskać początkowe długości kości
	double getLengthRatio() const { return lengthRatio; }
	//! ustawia długość przez którą należy pomnożyc aby uzyskać początkowe długości kości
	void setLengthRatio(double val) { lengthRatio = val; }
	//! zwraca pozycję roota
	//! \param time czas, dla którego ma być zwrócona pozycja
	//! \return pozycja  
	osg::Vec3 getRootPosition() const;

	const std::vector<osg::Vec3> getRotations() const;

private:
	/// \brief  Na podstawie danych z parsera tworzy tablicę z kwaternionami
	void createQuaternionRepresentation(kinematic::SkeletalModelConstPtr& skeletalModel, kinematic::SkeletalDataStreamPtr skeletalDataStream);

private:
	//! pełny szkielet h-anim                          
	kinematic::hAnimSkeletonPtr haSkeleton;
	//! Strumień z danymi dla szkieletu
	kinematic::SkeletalDataStreamPtr skeletalDataStream;
	//! długość przez którą należy pomnożyc aby uzyskać początkowe długości kości            
	double lengthRatio;
	//! czy obiekt jest już zainicjalizowany
	bool initialized;
};

typedef boost::shared_ptr<JointAnglesCollection> JointAnglesCollectionPtr;
typedef boost::shared_ptr<const kinematic::JointAnglesCollection> JointAnglesCollectionConstPtr;
}

#endif
