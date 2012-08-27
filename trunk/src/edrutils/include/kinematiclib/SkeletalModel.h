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
    /// \brief  Klasa zawiera wspólną reprezentację animacji dla parserów acclaim i biovision. 
    class SkeletalModel : boost::noncopyable
    {
    public:
        enum Angle {
            degrees,
            radians
        };
        /// \brief  mapa nazwa kości -> kość
        typedef std::map<std::string, JointPtr> JointMap;
        /// \brief  mapa identyfikator kości -> kość
        typedef std::map<int, JointPtr> JointIdMap;
		
    public:
        /// \brief  konstruktor wraz z domyślnymi parametrami
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
        //! \return głęboka kopia obiektu
		virtual SkeletalModel* clone() const;
        /** Zwraca kość na podstawie nazwy */
        JointPtr getJointByName(const std::string& name);
		/** Zwraca kość na podstawie nazwy */
		JointConstPtr getJointByName(const std::string& name) const;
        /** Zwraca kość na podstawie identyfikatora */
        JointPtr getJointByID(int id) { return jointsIds[id]; }
        /** Zwraca szkielet modelu */
        Skeleton& getSkeleton() { return *skeleton; }
		const Skeleton& getSkeleton() const { return *skeleton; }
        /** Zwraca referencje do mapy : nazwa kości -> kość*/
        JointMap& getJointMap() { return jointsMap; }
		const JointMap& getJointMap() const { return jointsMap; }
        /** Zwraca referencje do mapy : identyfikator kości -> kość*/
        JointIdMap& getJointIDMap() { return jointsIds; }
        //! \return  czy dane sa w stopniach czy radianach
        Angle getAngle() const { return angle; }
        //!  określa, czy dane sa w stopniach czy radianach
        void setAngle(Angle angle) { this->angle = angle;}
        //! \return globalny modyfikator masy
        double getMass() const { return mass; }
        //! ustawia globalny modyfikator masy
        void setMass(double val) { mass = val; }
        //! \return globalny modyfikator długości kości
        double getLength() const { return length; }
        //! ustawia globalny modyfikator długości kości
        void setLength(double val) { length = val; }
        //! \return wersja, wg której został zapisany plik
        std::string getVersion() const { return version; }
        //! ustawia wersję, wg której został zapisany plik
        void setVersion(const std::string& val) { version = val; }
        //! \return nazwę, która została zapisana w pliku
        std::string getName() const { return name; }
        //! ustawia nazwa zapisana w pliku
        void setName(const std::string& val) { name = val; }
        //! \return komentarz do modelu
        std::string getDocumentation() const { return documentation; }
        //! ustawia komentarz do modelu
        void setDocumentation(const std::string& val) { documentation = val; }

	private:
        //! tworzy pomocnicze struktury, ułatwiające znalezienie stawu
		//! \param skeleton rozpatrywany szkielet
		//! \param jointMap mapa nazwa stawu -> staw
		//! \param jointIDMap mapa identyfikator stawu -> staw
		static void createMaps(JointPtr skeleton, JointMap& jointMap, JointIdMap& jointIDMap);

    protected:										 
		//! wersja wg której został zapisany plik
        std::string version;        
		//! nazwa zapisana w pliku
        std::string name;           
		//! komentarz do modelu
        std::string documentation;  
		//! globalny modyfikator długości kości
        double length;         
		//! globalny modyfikator masy
        double mass;           
		//! określa, czy dane sa w stopniach czy radianach
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
