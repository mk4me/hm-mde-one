#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETALMODEL_H__

#include <map>
#include <vector>
#include <KinematicModelLibrary/Joint.h>
#include <KinematicModelLibrary/Skeleton.h>

namespace kinematic 
{
    struct Bone;
    class Skeleton;

    /// \brief  Klasa zawiera wspolna reprezentacje animacji dla parserow acclaim i biovision. 
    class SkeletalModel 
    {
    public:
        enum Angle {
            degrees,
            radians
        };

        /// \brief  inteligentny wskaznik do klasy
        typedef boost::shared_ptr<SkeletalModel> Ptr;
        /// \brief  mapa nazwa kosci -> kosc
        typedef std::map<std::string, JointPtr> JointMap;
        /// \brief  mapa identyfikator kosci -> kosc
        typedef std::map<int, JointPtr> JointIdMap;

        /// \brief  Struktura zawiera wartosci kanalow dla
        /// 	    konkretnej kosci i konkretnego kanalu
        struct singleBoneState
        {
            std::string name; //!< nazwa kosci 
            /// \brief  wartosci kanalow dla tej kosci (ich liczba powinna byc rowna stopniom swobody kosci)
            std::vector<double> channelValues; 
        };

        /// \brief  struktura przechowuje wartosci kanalow dla wszystkich kosci w pojedynczej klatce animacji
        struct singleFrame
        {
            int frameNo; //!< numer klatki animacji
            std::vector<singleBoneState> bonesData; //!< wartosci kanalow
        };

    public:
        /// \brief  konstruktor wraz z domyslnymi parametrami
        SkeletalModel() :
            version("1.10"),         
            name("unknown"),            
            documentation(""), 
            length(1.0),
            mass(1.0),
            angle(degrees),
            frameTime(0.033333333333)
        {
        }

    public:
        /** Zwraca kosc na podstawie nazwy */
        JointPtr getJointByName(const std::string& name) {
            JointMap::iterator it = jointsMap.find(name);
            JointPtr nullJoint;
            return it != jointsMap.end() ? it->second : nullJoint;
        }
        /** Zwraca kosc na podstawie identyfikatora */
        JointPtr getJointByID(int id) { return jointsIds[id]; }
        /** Zwraca szkielet modelu */
        Skeleton& getSkeleton() { return skeleton; }
        /** Zwraca referencje do mapy : nazwa kosci -> kosc*/
        JointMap& getJointMap() { return jointsMap; }
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
        std::vector<singleFrame>& getFrames() { return frames; }
        void setFrameTime(double frameTime) { this->frameTime = frameTime; }
        double getFrameTime() { return frameTime; }
        void RecreateMaps();

    protected:
        void Recreate(JointPtr joint);

    protected:
        std::string                  version;        //!< wersja wg ktorej zostal zapisany plik
        std::string                  name;           //!< nazwa zapisana w pliku
        std::string                  documentation;  //!< komentarz do modelu
        double                       length;         //!< globalny modyfikator dlugosci kosci
        double                       mass;           //!< globalny modyfikator masy
        Angle                        angle;          //!< okresla, czy dane sa w stopniach czy radianach
        JointMap                      jointsMap;       //!< mapa nazwa kosci -> kosc
        JointIdMap                    jointsIds;       //!< mapa identyfikator kosci -> kosc
        Skeleton                     skeleton;       //!< szkielet z koscmi
        std::vector<singleFrame>     frames;         //!< kolejka z wszystkimi ramkami
        double                       frameTime;      //!< czas pomiedzy klatkami
    };

    ///// \brief  Wydzielony interfejs dla parserow
    //class IParser
    //{
    //public:
    //    virtual void parse(SkeletalModel::Ptr model, const std::string& filename) = 0;
    //    virtual void save(const SkeletalModel::Ptr model, const std::string& filename) = 0;
    //};
}
#endif