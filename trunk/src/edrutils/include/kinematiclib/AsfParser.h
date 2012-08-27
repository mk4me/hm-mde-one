#ifndef HEADER_GUARD_KINEMATICMODEL__ASFPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__ASFPARSER_H__

namespace kinematic
{
    class Skeleton;

    /*! 
     * \brief dzięki tej klasie możliwe jest wczytanie plików *.asf
     */
    class AsfParser
    {
    public:
        /** Inteligentny wskaźnik do klasy */
        typedef boost::shared_ptr<AsfParser> Ptr;

    public:
        AsfParser();
        virtual ~AsfParser();

    public:
        /*!  Wczytanie pliku
            \param filename Nazwa pliku do wczytania */
        void parse(SkeletalModelPtr model, const std::string& filename);
        /*! zapis wczytanego modelu
            \param filename Nazwa pliku do wczytania */
        void save(const SkeletalModelPtr model, const std::string& filename);

        //! Niektóre parsery wymagają, aby rotacje roota byly w kolejności XYZ
        //! jeśli flaga jest ustawiona, to taka kolejność będzie wymuszona przy zapisie
        //! \param flag czy wymuszać kolejność rotacji XYZ przy zapisie
        void setForceXYZRoot(bool flag) {forceRootXYZ = flag;}

    protected:
        /** Dzieli wejściowy plik na poszczegolne sekcje. 
            \param asf lancuch z całym plikiem asf*/
        bool splitAsf(const std::string& asf);
        /** Parsuje sekcje z jednostkami */
        bool parseUnits(const std::string& units);
        /** Parsuje sekcje z wszystkimi stawami */
        bool parseJoints(const std::string& bones);
        /** Parsuje pojedynczy staw */
        bool parseSingleJoint(const std::string& singleBone, Joint& bone);
        /** Parsuje główną kość*/
        bool parseRoot(const std::string& root, Skeleton& skeleton);
        /** Tworzy hierarchię kości */
        bool parseHierarchy(const std::string& hierarchy, Skeleton& skeleton);
        /** Usuwa komentarze z całego pliku asf. */
        std::string removeComments(const std::string& txt);
        /** Zapisuje sekcje root do pliku asf*/
        void saveRoot(std::ostream& out) const;
        /** Zapisuje sekcje units do pliku asf*/
        void saveUnits(std::ostream& out) const;
        /** Zapisuje wszystkie kości do pliku asf*/
        void saveBones(std::ostream& out) const;
        /** Zapisuje pojedyncza kość*/
        void saveSingleBone(std::ostream& out, const Joint& bone) const;
        /** Zapisuje hierarchie do pliku asf*/
        void saveHierarchy(std::ostream& out) const;
        /** Zwraca odpowiednia sekcje, na podstawie tokenu.
            Sekcje sa tworzone w pierwszym etapie parsowania.*/
        std::string* getSectionContainter(const std::string& token);
        /** Parsuje linijke z limitami dla stopnia swobody. 
            Dopisuje wynik do wektora limitValues*/
        void parseLimit(const std::string& token, std::vector<double>& limitValues ); 
		//! Odpowiada za zapis kości
		//! \param out strumień wyjściowy
		//! \param joint zapisywana hierarchia
		void saveBoneInHierarchy(std::ostream& out, kinematic::JointConstPtr joint) const;
        

    protected:
        std::string                     version;        //!< wersja wg której został zapisany plik
        std::string                     name;           //!< nazwa zapisana w pliku
        std::string                     documentation;  //!< komentarz do modelu
        std::string                     units;          //!< sekcja z jednostkami
        std::string                     root;           //!< sekcja z rootem
        std::string                     bonedata;       //!< sekcja z wszystkimi kości
        std::string                     hierarchy;      //!< sekcja z hierarchia
        std::string                     unknown;        //!< nieznane / nieobslugiwane tokeny
        std::vector<DegreeOfFreedom::Channel> channels; //!< kanały zapisane w sekcji root

    private:
        SkeletalModelPtr               model;          //!< przechowuje model ze szkieletem
        bool                             forceRootXYZ;   //!< wymuszenie kolejności rotacji roota (zapewnia zgodność z niektórymi parserami)
        int                              idCounter;      //!< wymusza tworzenie, identyfikatorów kości od nowa
       
     };
}

#endif
