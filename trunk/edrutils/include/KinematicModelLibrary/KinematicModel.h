#ifndef HEADER_GUARD_KINEMATICMODEL__KINEMATICMODEL_H__
#define HEADER_GUARD_KINEMATICMODEL__KINEMATICMODEL_H__

namespace hmAnimation {

class hAnimJoint;
typedef boost::shared_ptr<hAnimJoint> JointPtr;
typedef boost::weak_ptr<hAnimJoint> JointWeak;


/// \brief Struktura reprezentuje kosc standardu h-anim
struct hAnimBone {
    typedef boost::shared_ptr<hAnimBone> Ptr;   
    typedef boost::weak_ptr<hAnimBone> Weak;    
    std::string name;                           //!< nazwa kosci (powinna byc taka jak w h-anim 1.1)
    std::vector<JointPtr> childrenJoints;       //!< do jednej kosci moze byc podlaczone wiele stawow
    JointWeak parentJoint;                      //!< staw, z ktorego wychodzi kosc
};

/// \brief Klasa reprezentuje staw standardu h-anim
class hAnimJoint : public Joint {
    friend class KinematicModel;
public:
    typedef boost::shared_ptr<hAnimJoint> Ptr;
    typedef boost::weak_ptr<hAnimJoint> Weak;
    hAnimBone::Weak parentBone;                  //!< kosc, z ktorej wychodzi staw
    std::vector<hAnimBone::Ptr> childrenBones;   //!< do jednego stawu moze byc podlaczone wiele kosci
                                                 
public:
    /// \brief  Domyslny konstruktor, wszystkie stawy sa nieaktywne, dopiero podanie danych z parsera moze zmienic ten stan
    hAnimJoint() : Joint(), active(false) {}

public: // akcesory
    void setName(const std::string& name)                   { this->name = name; }
    const std::string& getName(void) const                  { return name; }
    const hAnimBone::Weak& getParentBone(void) const		{ return(parentBone);		};
    void setParentBone(const hAnimBone::Weak& _parentBone)	{ parentBone = _parentBone;	};
    const std::vector<hAnimBone::Ptr>& GetChildrenBones(void) const				{ return(childrenBones);			};
    void SetChildrenBones(const std::vector<hAnimBone::Ptr>& _childrenBones)	{ childrenBones = _childrenBones;	};
    const hAnimJoint::Ptr getRoot();

    bool isActive() { return this->active; }

private:
    bool active;                                 //!< okresla, czy staw jest aktywny (jesli model ma jakies stawy nieaktywne, to mamy do czynienia z niekompletnym szkieletem h-anim 1.1)
};



/// \brief  klasa reprezentuje szkielet h-anim
class hAnimSkeleton : public Skeleton {
public:
    typedef boost::shared_ptr<hAnimSkeleton> Ptr;
    //void setRoot(hAnimJoint::Ptr root) { this->root = root; }
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
    static LevelOfArticulation GetLevelOfArticulation(SkeletalModel::Ptr skeletalModel);
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
    virtual ~KinematicModel(void);

public:
    /// \brief  macierz MxN z kwaternionami 
    /// \details pierwszy indeks - nr klatki, drugi indeks - nr stawu
    typedef std::vector< std::vector<osg::Quat> > quatTable;

public:
    /// \brief  Zwraca surowe dane z parsera
    hmAnimation::SkeletalModel::Ptr getSkeletalData() { return skeletalModel; }

    /// \brief  Ustawia dane z parsera
    /// \details W tym miejscu tworzony jest pe³ny szkielet h-anim, robiona jest normalizacja danych
    /// \param  skeletalModel   The skeletal model. 
    void setSkeletalData(hmAnimation::SkeletalModel::Ptr skeletalModel);

    // akcesory do markerow
    void* getMarkersData() { return markers;}
    void setMarkersData(void* data) { markers = data; }

    /// \brief  Zwraca dane o rotacji dla wszystkich jointow dla konkretnej ramki
    /// \param  time znormalizowany czas (0 - 100) , clampowanie dla wartosci spoza zakresu
    /// \return Tablica z kwaternionami opisujacymi rotacje jointow.
    std::vector<osg::Quat>& getQuaternionRotation(double time);

    /// \brief  Zwraca dane o rotacji dla wszystkich jointow dla konkretnej ramki
    /// \param  frameNo numer klatki, dla ktorej maja byc zwrocone dane
    /// \return Tablica z kwaternionami opisujacymi rotacje jointow. 
    std::vector<osg::Quat>& getQuaternionRotation(int frameNo);

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
    hAnimJoint::Ptr getJointByName(const std::string& name);
    /// \brief  Pobiera kosc z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
    /// \param  name    Nazwa pobieranej kosci
    /// \return Wyszukana kosc, jesli takiej nie ma to rzucany jest wyjatek
    hAnimBone::Ptr getBoneByName(const std::string& name);

    /// \brief  Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
    /// \param  joint   Aktywowany staw. 
    /// \param  active  true - staw staje sie aktywny , false - staw staje sie nieaktywny 
    void activateJoint(hAnimJoint::Ptr joint, bool active);

private:
    //void createHAnimSkeleton(hmAnimation::SkeletalModel::Ptr SkeletalModel);
    
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
    void backJointAndBone(std::string newJointName, std::string newBoneName, hAnimJoint::Ptr& parentJoint, hAnimBone::Ptr& parentBone);
    /// \brief  Tworzy kosc i staw na podstawie podanych nazw i zmienia referencje do wskaznikow.
    /// \details Pomocnicze przy tworzeniu drzewa h-anim 
    /// \param  newJointName       Nazwa nowego stawu. 
    /// \param  newBoneName        Nazwa nowej kosci.
    /// \param [in,out] parentJoint Referencja do wskaznika, tutaj laduje tworzony staw. 
    /// \param [in,out] parentBone  Referencja do wskaznika, tutaj laduje tworzona kosc.
    void createJointAndBone(std::string newJointName, std::string newBoneName, hAnimJoint::Ptr& parentJoint, hAnimBone::Ptr& parentBone);
    /// \brief  Wczytuje slownik z nazwami do mapowania. 
    /// \param  filename   Plik ze slownikiem. 
    void loadMappingDictionary(const std::string& filename);
    /// \brief  Executes the skeleton mapping operation. 
    /// \param  skeletalModel   The skeletal model. 
    void doSkeletonMapping(SkeletalModel::Ptr skeletalModel);
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
    double getMaxLength(Joint::Ptr joint, double maxLength) const;

private:
    void* markers;                                                      //!< dane z markerami
    hmAnimation::SkeletalModel::Ptr skeletalModel;                      //!< dane z parsera acclaim / bvh
    hmAnimation::hAnimSkeleton::Ptr haSkeleton;                         //!< pelny szkielet h-anim
    std::vector< std::vector<osg::Quat> > quaternionRepresentation;     //!< tablica z kwaternionami dla kazdego ze stawow
    std::map<std::string, hAnimJoint::Ptr> joints;                      //!< mapa wszystkich jointow h-anim
    std::map<std::string, hAnimBone::Ptr> bones;                        //!< mapa wszystkich kosci h-anim
    std::map<std::string, std::string> jointMappingDictionary;          //!< mapa dzieki ktorej mapuje sie do nomenklatury h-anim
    std::map<std::string, std::string> boneMappingDictionary;           //!< mapa dzieki ktorej mapuje sie do nomenklatury h-anim
    double lengthRatio;                                                 //!< w zasadzie dlugos 
};
}

#endif