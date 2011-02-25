#ifndef HEADER_GUARD_KINEMATICMODEL__ASFPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__ASFPARSER_H__

#define PARSER_VERSION "0.11"
//#define forceRootXYZ true

namespace hmAnimation
{
    class Skeleton;

    /*! 
     * \brief Dzieki tej klasie mozliwe jest wczytanie plikow *.asf
     */
    class AsfParser
    {
    public:
        /** Inteligentny wskaznik do klasy */
        typedef boost::shared_ptr<AsfParser> Ptr;

    public:
        AsfParser();
        virtual ~AsfParser();

    public:
        /*!  Wczytanie pliku
            \param filename Nazwa pliku do wczytania */
        void parse(SkeletalModel::Ptr model, const std::string& filename);
        /*! zapis wczytanego modelu
            \param filename Nazwa pliku do wczytania */
        void save(const SkeletalModel::Ptr model, const std::string& filename);

        //! Niektore parsery wymagaja, aby rotacje roota byly w kolejnosci XYZ
        //! Jesli flaga jest ustawiona, to taka kolejnosc bedzie wymuszona przy zapisie
        //! \param flag czy wymuszac kolejnosc rotacji XYZ przy zapisie
        void setForceXYZRoot(bool flag) {forceRootXYZ = flag;}

    protected:
        /** Dzieli wejsciowy plik na poszczegolne sekcje. 
            \param asf lancuch z calym plikiem asf*/
        bool splitAsf(const std::string& asf);
        /** Parsuje sekcje z jednostkami */
        bool parseUnits(const std::string& units);
        /** Parsuje sekcje z wszystkimi koscmi */
        bool parseBones(const std::string& bones);
        /** Parsuje pojedyncza kosc */
        bool parseSingleBone(const std::string& singleBone, Joint& bone);
        /** Parsuje glowna kosc*/
        bool parseRoot(const std::string& root, Skeleton& skeleton);
        /** Tworzy hierarchie kosci */
        bool parseHierarchy(const std::string& hierarchy, Skeleton& skeleton);
        /** Usuwa komentarze z calego pliku asf. */
        std::string removeComments(const std::string& txt);
        /** Zapisuje sekcje root do pliku asf*/
        void saveRoot(std::ostream& out);
        /** Zapisuje sekcje units do pliku asf*/
        void saveUnits(std::ostream& out);
        /** Zapisuje wszystkie kosci do pliku asf*/
        void saveBones(std::ostream& out);
        /** Zapisuje pojedyncza kosc*/
        void saveSingleBone(std::ostream& out, const Joint& bone);
        /** Zapisuje hierarchie do pliku asf*/
        void saveHierarchy(std::ostream& out);
        /** Zwraca odpowiednia sekcje, na podstawie tokenu.
            Sekcje sa tworzone w pierwszym etapie parsowania.*/
        std::string* getSectionContainter(const std::string& token);
        /** Parsuje linijke z limitami dla stopnia swobody. 
            Dopisuje wynik do wektora limitValues*/
        void parseLimit(const std::string& token, std::vector<double>& limitValues ); 
        

    protected:
        std::string                     version;        //!< wersja wg ktorej zostal zapisany plik
        std::string                     name;           //!< nazwa zapisana w pliku
        std::string                     documentation;  //!< komentarz do modelu
        std::string                     units;          //!< sekcja z jednostkami
        std::string                     root;           //!< sekcja z rootem
        std::string                     bonedata;       //!< sekcja z wszystkimi kosci
        std::string                     hierarchy;      //!< sekcja z hierarchia
        std::string                     unknown;        //!< nieznane / nieobslugiwane tokeny
        std::vector<DegreeOfFreedom::Channel> channels; //!< kanaly zapisane w sekcji root

    private:
        SkeletalModel::Ptr               model;          //!< przechowuje model ze szkieletem
        bool                             forceRootXYZ;   //!< wymuszenie kolejnosci rotacji roota (zapewnia zgodnosc z niektorymi parserami)
        int                              idCounter;      //!< wymusza tworzenie, identyfikatorow kosci od nowa
       
     };
}

#endif