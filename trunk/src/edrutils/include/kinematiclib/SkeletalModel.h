#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETALMODEL_H__

#include <map>
#include <vector>
#include <boost/utility.hpp>
#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletalData.h>

namespace kinematic 
{
    /// \brief  Klasa zawiera wspolna reprezentacje animacji dla parserow acclaim i biovision. 
    class SkeletalModel : boost::noncopyable
    {
    public:
        enum Angle {
            degrees,
            radians
        };
        /// \brief  mapa nazwa kosci -> kosc
        typedef std::map<std::string, JointPtr> JointMap;
        /// \brief  mapa identyfikator kosci -> kosc
        typedef std::map<int, JointPtr> JointIdMap;
		
    public:
        /// \brief  konstruktor wraz z domyslnymi parametrami
        SkeletalModel() :
            version("1.10"),         
            name("unknown"),            
            documentation(""), 
            length(1.0),
            mass(1.0),
            angle(degrees),
			skeleton(new Skeleton)
        {
        }

    public:

		virtual SkeletalModel* clone() const;

        /** Zwraca kosc na podstawie nazwy */
        JointPtr getJointByName(const std::string& name);
		/** Zwraca kosc na podstawie nazwy */
		JointConstPtr getJointByName(const std::string& name) const;
        /** Zwraca kosc na podstawie identyfikatora */
        JointPtr getJointByID(int id) { return jointsIds[id]; }
        /** Zwraca szkielet modelu */
        Skeleton& getSkeleton() { return *skeleton; }
		const Skeleton& getSkeleton() const { return *skeleton; }
        /** Zwraca referencje do mapy : nazwa kosci -> kosc*/
        JointMap& getJointMap() { return jointsMap; }
		const JointMap& getJointMap() const { return jointsMap; }
        /** Zwraca referencje do mapy : identyfikator kosci -> kosc*/
        JointIdMap& getJointIDMap() { return jointsIds; }
        // akcesory
        Angle getAngle() const { return angle; }
        void setAngle(Angle angle) { this->angle = angle;}
        double getMass() const { return mass; }
        void setMass(double val) { mass = val; }
        double getLength() const { return length; }
        void setLength(double val) { length = val; }
        std::string getVersion() const { return version; }
        void setVersion(const std::string& val) { version = val; }
        std::string getName() const { return name; }
        void setName(const std::string& val) { name = val; }
        std::string getDocumentation() const { return documentation; }
        void setDocumentation(const std::string& val) { documentation = val; }

	private:
		static void createMaps(JointPtr skeleton, JointMap& jointMap, JointIdMap& jointIDMap);

    protected:										 
		//! wersja wg ktorej zostal zapisany plik
        std::string version;        
		//! nazwa zapisana w pliku
        std::string name;           
		//! komentarz do modelu
        std::string documentation;  
		//! globalny modyfikator dlugosci kosci
        double length;         
		//! globalny modyfikator masy
        double mass;           
		//! okresla, czy dane sa w stopniach czy radianach
        Angle angle;          
		//! mapa nazwa stawu -> staw
        JointMap jointsMap;      
		//! mapa identyfikator stawu -> staw
        JointIdMap jointsIds;      
		//! szkielet ze stawami
        SkeletonPtr skeleton;     
    };
	typedef boost::shared_ptr<SkeletalModel> SkeletalModelPtr;
	typedef boost::shared_ptr<const SkeletalModel> SkeletalModelConstPtr;
}
#endif