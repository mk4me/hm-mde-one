#ifndef HEADER_GUARD_KINEMATICMODEL__KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATICMODEL__KINEMATICMODEL_H__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/Vec3>
#include <tinyxml.h>
#include <utils/Debug.h>
#include <kinematiclib/SkeletalParsers.h>
#include <kinematiclib/KinematicModel.h>
namespace kinematic {

class hAnimJoint;
typedef boost::shared_ptr<hAnimJoint> hAnimJointPtr;
typedef boost::weak_ptr<hAnimJoint> hAnimJointWeak;

/// \brief Struktura reprezentuje kosc standardu h-anim
struct hAnimBone {
    //typedef boost::shared_ptr<hAnimBone> Ptr;   
    //typedef boost::weak_ptr<hAnimBone> Weak;    
    std::string name;                           //!< nazwa kosci (powinna byc taka jak w h-anim 1.1)
    std::vector<JointPtr> childrenJoints;       //!< do jednej kosci moze byc podlaczone wiele stawow
    hAnimJointWeak parentJoint;                      //!< staw, z ktorego wychodzi kosc
};

typedef boost::shared_ptr<hAnimBone> hAnimBonePtr;
typedef boost::weak_ptr<hAnimBone> hAnimBoneWeak;

/// \brief Klasa reprezentuje staw standardu h-anim
class hAnimJoint : public Joint {
    friend class KinematicModel;
public:
    //typedef boost::shared_ptr<hAnimJoint> Ptr;
    //typedef boost::weak_ptr<hAnimJoint> Weak;
    hAnimBoneWeak parentBone;                  //!< kosc, z ktorej wychodzi staw
    std::vector<hAnimBonePtr> childrenBones;   //!< do jednego stawu moze byc podlaczone wiele kosci
                                                 
public:
    /// \brief  Domyslny konstruktor, wszystkie stawy sa nieaktywne, dopiero podanie danych z parsera moze zmienic ten stan
    hAnimJoint() : Joint(), active(false) {}

public: // akcesory
    void setName(const std::string& name)                   { this->name = name; }
    const std::string& getName(void) const                  { return name; }
    const hAnimBoneWeak& getParentBone(void) const		    { return(parentBone);		};
    void setParentBone(const hAnimBoneWeak& _parentBone)	{ parentBone = _parentBone;	};
    const std::vector<hAnimBonePtr>& GetChildrenBones(void) const				{ return(childrenBones);			};
    void SetChildrenBones(const std::vector<hAnimBonePtr>& _childrenBones)	{ childrenBones = _childrenBones;	};
    const hAnimJointPtr getRoot();

    osg::Vec3 getLocalShift() const { return localShift; }
    void setLocalShift(osg::Vec3 val) { localShift = val; }
    osg::Quat getChildParentRotation() const { return childParentRotation; }
    void setChildParentRotation(osg::Quat val) { childParentRotation = val; }
    
    bool isActive() { return this->active; }

private:
    bool active;                                 //!< okresla, czy staw jest aktywny (jesli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
    osg::Vec3 localShift;
    osg::Quat childParentRotation;
};

//! Schemat mapowania kosci
class SkeletonMappingScheme {
public:
    //! typ pojedynczego schematu mapowania
    typedef std::map<std::string, std::string> mappingDict;
    //! statyczna metoda wczytuje schematy z pliku XML
    //! \param filename nazwa pliku do wczytania
    //! \return kolekcja schematow mapowania
    static std::vector<SkeletonMappingScheme> LoadFromXML(const std::string& filename);
    //! \return zwraca slownik mapowania
    const mappingDict& getMappingDictionary() { return mappingDictionary; }

private:
    SkeletonMappingScheme(mappingDict mapping) {
        this->mappingDictionary = mapping;
      }

private:
    mappingDict mappingDictionary;
    static void readNode(TiXmlNode* node, std::vector<SkeletonMappingScheme>& result);
    static mappingDict readMappingNode(TiXmlNode* node);
};


class IMarkerSet
{
public:
    // hack
    void setScale(double scale) const { this->scale = scale; } 
    virtual int getMarkersCount() const = 0;
    virtual osg::Vec3 getPosition(int markerNo, double normalizedTime) const = 0;

protected:
    double getScale() const { return scale; }
private: 
    mutable double scale;
};

typedef boost::shared_ptr<IMarkerSet> IMarkerSetPtr;
typedef boost::shared_ptr<const IMarkerSet> IMarkerSetConstPtr;


/// \brief  klasa reprezentuje szkielet h-anim
class hAnimSkeleton : public Skeleton {
public:
    typedef boost::shared_ptr<hAnimSkeleton> Ptr;

    hAnimJointPtr getHAnimRoot() const { return boost::dynamic_pointer_cast<hAnimJoint>(getRoot()); }
    //void setRoot(hAnimJointPtr root) { this->root = root; }
};


/// \brief  Klasa pomocnicza, dostarcza typow i metod staycznych do obslugi szkieletu h-anim
class HAnimHelper
{
public:
    enum LevelOfArticulation 
    {
        _0_HumanoidRootOnly,
        _1_TypicalLowEndRealTime3DHierarchy,
        _2_BodyWithSimplifiedSpine,
        _3_Full_HAnim_Hierarchy
    };

public:
    static LevelOfArticulation GetLevelOfArticulation(SkeletalModelPtr skeletalModel);
};


/// \brief  Klasa dostarcza reprezentacje wewnetrzna szkieletu.
/// \details Zawiera znormalizowane dane : rotacje jako kwaterniony, dlugosc kosci z zakresu <0,1>, nazewnictwo h-anim
class KinematicModel
{
public:

    /// \brief  Domyslny konstruktor, stara sie wczytac plik 'dictionary.txt' (slownik z roznymi nazwami kosci i ich mapowaniem do h-anim)
    KinematicModel(void);

    /// \brief  Konstruktor, ktoremu trzeba wskazaz plik ze slownikiem 
    /// \param  dictionaryFile plik ze slownikiem z roznymi nazwami kosci i ich mapowaniem do h-anim
    KinematicModel(const std::string& dictionaryFilename);

public:
    /// \brief  macierz MxN z kwaternionami 
    /// \details pierwszy indeks - nr klatki, drugi indeks - nr stawu
    typedef std::vector< std::vector<osg::Quat> > quatTable;

public:
    /// \brief  Zwraca surowe dane z parsera
    kinematic::SkeletalModelPtr getSkeletalData() { return skeletalModel; }
    //! Tworzy dane zgodne z parserami na podstawie reprezentacji wewnetrznej
    kinematic::SkeletalModelPtr createSkeletalData() const;
    /// \brief  Wczytuje slownik z nazwami do mapowania. 
    /// \param  filename   Plik ze slownikiem. 
    void loadMappingDictionary(const std::string& filename);
    /// \brief  Ustawia dane z parsera
    /// \details W tym miejscu tworzony jest pe³ny szkielet h-anim, robiona jest normalizacja danych
    /// \param  skeletalModel   The skeletal model. 
    void setSkeletalData(kinematic::SkeletalModelPtr skeletalModel);
    //! \brief zwraca szkielet zgodny z h-anim
    kinematic::hAnimSkeleton::Ptr getHAnimSkeleton() const { return haSkeleton; }
    const std::map<std::string, kinematic::hAnimJointPtr>& getJoints() const { return joints; }
    // akcesory do markerow
    IMarkerSetConstPtr getMarkersData() const { return markers;}
    void setMarkersData(IMarkerSetConstPtr data) const { markers = data; markers->setScale(lengthRatio); }

    double getFrameTime() const { 
        UTILS_ASSERT(skeletalModel); 
        return skeletalModel->getFrameTime();
    }
    int getNumFrames() const {
         UTILS_ASSERT(skeletalModel); 
         return skeletalModel->getFrames().size(); 
    }
    /// \brief  Zwraca dane o rotacji dla wszystkich jointow dla konkretnej ramki
    /// \param  time znormalizowany czas (0 - 1) , clampowanie dla wartosci spoza zakresu
    /// \return Tablica z kwaternionami opisujacymi rotacje jointow.
    const std::map<std::string, osg::Quat>& getQuaternionRotation(double time) const;

    osg::Quat getQuaternionRotationByName(const std::string& name, double time) const;

    /// \brief  Zwraca dane o rotacji dla wszystkich jointow dla konkretnej ramki
    /// \param  frameNo numer klatki, dla ktorej maja byc zwrocone dane
    /// \return Tablica z kwaternionami opisujacymi rotacje jointow. 
    const std::map<std::string, osg::Quat>& getQuaternionRotation(int frameNo) const;

    osg::Vec3 getRootPosition(int frame) const;
    osg::Vec3 getRootPosition(double time) const;


    // reprezentacje dla innych typow
    template <typename T>
    std::vector<T> getRotation(double time);
    template <typename T>
    std::vector<T> getRotation(int frameNo);

    // specjalizacje
    template<> std::vector<osg::Quat> getRotation(double time);
    template<> std::vector<osg::Vec3d> getRotation(double time);
    template<> std::vector<osg::Matrixd> getRotation(double time);
    /// \brief  Pobiera joint z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
    /// \param  name    Nazwa pobieranego stawu 
    /// \return Wyszukany staw, jesli takiego nie ma to rzucany jest wyjatek
    hAnimJointPtr getJointByName(const std::string& name);
    /// \brief  Pobiera kosc z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
    /// \param  name    Nazwa pobieranej kosci
    /// \return Wyszukana kosc, jesli takiej nie ma to rzucany jest wyjatek
    hAnimBonePtr getBoneByName(const std::string& name);
    /// \brief  Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
    /// \param  joint   Aktywowany staw. 
    /// \param  active  true - staw staje sie aktywny , false - staw staje sie nieaktywny 
    void activateJoint(hAnimJointPtr joint, bool active);
    //! Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
    //! \param jointName nazwa aktywowanego stawu
    //! \param active  true - staw staje sie aktywny , false - staw staje sie nieaktywny 
    void activateJoint(const std::string& jointName, bool active);

private:
    /// \brief  Tworzy szkielet h-anim
    /// \param  loa Poziom szczegolowosci. 
    hAnimSkeleton::Ptr createHAnim(/*HAnimHelper::LevelOfArticulation loa*/);
    /// \brief  Metoda mapuje podana nazwe kosci na nomentklature h-anim 
    /// \param  given  nazwa kosci lub stawu. 
    /// \return nazwa w nomenklaturze h-anim
    std::string mapJointName(const std::string& given);
    /// \brief  Wyszukuje kosc i staw na podstawie podanych nazw i zmienia referencje do wskaznikow.
    /// \details Pomocnicze przy tworzeniu drzewa h-anim 
    /// \param  newJointName       Nazwa stawu do wyszukania. 
    /// \param  newBoneName        Nazwa kosci do wyszukania. 
    /// \param [out] parentJoint Referencja do wskaznika, tutaj laduje znaleziony staw. 
    /// \param [out] parentBone  Referencja do wskaznika, tutaj laduje znaleziona kosc. 
    void backJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
    /// \brief  Tworzy kosc i staw na podstawie podanych nazw i zmienia referencje do wskaznikow.
    /// \details Pomocnicze przy tworzeniu drzewa h-anim 
    /// \param  newJointName       Nazwa nowego stawu. 
    /// \param  newBoneName        Nazwa nowej kosci.
    /// \param [in,out] parentJoint Referencja do wskaznika, tutaj laduje tworzony staw. 
    /// \param [in,out] parentBone  Referencja do wskaznika, tutaj laduje tworzona kosc.
    void createJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
    /// \brief  Executes the skeleton mapping operation. 
    /// \param  skeletalModel   The skeletal model. 
    void doSkeletonMapping(SkeletalModelPtr skeletalModel);
    /// \brief  Na podstawie danych z parsera tworzy tablice z kwaternionami
    void createQuaternionRepresentation();
    /// \brief  Wyszukuje i zwraca dlugosc najdluzszej z kosci w szkielecie
    /// \param  skeleton  Przeszukiwany szkielet
    /// \return Dlugosc najdluzszej z kosci. 
    double getMaxBoneLength(const Skeleton& skeleton) const;
    /// \brief  Gets a maximum length.
    /// \param  joint       The joint. 
    /// \param  maxLength   Length of the maximum. 
    /// \return The maximum length. 
    double getMaxLength(JointPtr joint, double maxLength) const;
    //! Tworzenie kopii roota
    //! \param origin
    JointPtr cloneRootWithActivated(const JointPtr origin) const;
    //! Zmienia nazwy stawow w danych (po mapowaniu)
    //! \param oldName stara nazwa stawu
    //! \param newName nowa, zmapowana nazwa stawu
    void changeNameInFrames(const std::string& oldName, const std::string& newName);
    //! Sprawdza polaczenie pomiedzy stawami
    //! \param parent
    //! \param middle
    //! \param child
    bool checkLink(hAnimJointPtr parent, hAnimJointPtr middle, hAnimJointPtr child);

    osg::Vec3 vectorRotation(osg::Vec3 v, double rX, double rY, double rZ);
    osg::Quat rotationParentChild(JointPtr parent, JointPtr child);
    osg::Quat createRotation(const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order);

private:
    // hack
    mutable IMarkerSetConstPtr markers;                                 //!< dane z markerami
    kinematic::SkeletalModelPtr skeletalModel;                          //!< dane z parsera acclaim / biovision
    kinematic::hAnimSkeleton::Ptr haSkeleton;                           //!< pelny szkielet h-anim
    std::vector< std::map<std::string, osg::Quat> > quaternionRepresentation;     //!< tablica z kwaternionami dla kazdego ze stawow
    std::map<std::string, hAnimJointPtr> joints;                        //!< mapa wszystkich jointow h-anim
    std::map<std::string, hAnimBonePtr> bones;                          //!< mapa wszystkich kosci h-anim
    std::map<std::string, std::string> jointMappingDictionary;          //!< mapa dzieki ktorej mapuje sie do nomenklatury h-anim
    std::map<std::string, std::string> boneMappingDictionary;           //!< mapa dzieki ktorej mapuje sie do nomenklatury h-anim
    double lengthRatio;                                                 //!< dlugosc przez ktora nalezy pomnozyc aby uzyskac poczatkowe dlugosci kosci 
    std::vector<SkeletalModel::singleFrame> frames;                     //!< zbior wszystkich klatek animacji
    std::vector<SkeletonMappingScheme> mappingSchemes;                  //!< slowniki ze sposobem mapowania
    std::vector<osg::Vec3> rootPositions;
}; 

typedef boost::shared_ptr<KinematicModel> KinematicModelPtr;
typedef boost::shared_ptr<const kinematic::KinematicModel> KinematicModelConstPtr;
}

#endif