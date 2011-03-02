#ifndef HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__

namespace kinematic
{

struct Bone;
class Skeleton;

/// \brief Klasa realizuje odczyt i zapis plikow *.bvh
class BvhParser
{
public:
    BvhParser();
    virtual ~BvhParser();

public:
    /// \brief  Zapisuje dane z modelu do pliku *.asf
    /// \param  model reprezentacja wewnetrzna animacji
    /// \param  filename nazwa pliku do ktorego zostana zapisane dane
    virtual void save(const SkeletalModel::Ptr model, const std::string& filename );
    /// \brief  Parsuje podany plik. 
    /// \param  model reprezentacja wewnetrzna animacji
    /// \param  filename  Nazwa pliku do parsowania. 
    virtual void parse(SkeletalModel::Ptr model, const std::string& filename );

private:
    bool forceZXY; //!< jesli true to wymuszony zostaje format kanalow (3 RZ,RX,RY lub 6 TX,TY,TZ,RZ,RX,RY)
    bool tabs;     //!< jesli ustawione to w pliku zostana wstawione tabulatory, spacje w przeciwnym razie

private:
    /// \brief  Wczytuje pojedynczego jointa z pliku. Wywolywane rekurencyjnie
    /// \param [in] in Strumien wejsciowy. 
    /// \param [in,out] jointList Lista, do ktorej trafia kolejnosc wystapienia jointow. 
    void readSingleJoint(std::istream& in, std::list<std::string>& jointList);
    /// \brief  Zwraca nazwe kanalu (w formacie biovision, np. xPosition)
    /// \param  channel Kanal, dla ktorego zostanie zwrocona nazwa. 
    /// \return The channel name. 
    std::string getChannelName(DegreeOfFreedom::Channel channel) const;
    /// \brief  Pobiera wartosci kanalow dla konkretnej kosci i konkretnej klatki. 
    /// \param  frameNo     Nr klatki animacji 
    /// \param  boneName    Nazwa kosci
    /// \return Stan kosci, z wartosciami kanalow. 
    SkeletalModel::singleBoneState getSingleBoneState(int frameNo, std::string boneName) const;
    /// \brief  Zapisuje pojedyncza kosc
    /// \param [in,out] out         Strumien wyjsciowy
    /// \param  bone                Kosc do zapisu 
    /// \param  parent				Kosc rodzica (przydatne przy sprawdzaniu i usuwaniu dummy bones).
    /// \param  lvl                 Poziom rekurencji (do wciec w pliku) 
    /// \param [in,out] jointList   Do listy trafiaja w odpowiedniej kolejnosci nazwy zapisywanych kosci 
    void saveBone(std::ostream& out, JointPtr bone, JointPtr parent, int lvl, std::list<std::string>& jointList) const;
    /// \brief  Zapisuje kolejnosc zapisu kanalow dla jointa 
    /// \param [in]        Strumien wyjsciowy. 
    /// \param  bone       Kosc, dla ktorej bedzie zapisana hierarchia. 
    /// \param  lvl        Poziom rekurencji (do wciec w pliku) 
    void saveChannelsInHierarchy(std::ostream& out, JointPtr bone, int lvl) const;
    /// \brief  Czy podane stopnie swobody zawieraja tez dotyczace przesuniecia
    /// \param [in] dofs    Wektor ze stopniami swobody 
    /// \return true jesli znaleziono kanal z transformacja. 
    bool hasTranslation(const std::vector<DegreeOfFreedom>& dofs);
    /// \brief  Metoda tworzy kosci z jointow. Mozliwe, ze zmieni sie hierarchia (dojda elementy - dummy bones)
    void setBones();
    /// \brief  Wywolywane z setBones. Bierze udzial w tworzeniu nowej hierarchi (kosci zamiast joinow)
    /// \param  newBone Nowo stworzona kosc w hierarchii. 
    /// \param  oldBone The old bone. 
    void HandleBone(JointPtr newBone);
    /// \brief  Zwraca kolejnosc rotacji.
    /// \param  bone    Kosc, dla ktorej ma byc zwrocona kolejnosc. 
    /// \return Kolejnosc rotacji
    Axis::Order getAxisOrder(JointPtr bone) const;
    /// \brief  Zwraca stringa zawierajacego podana liczbe spacji 
    std::string space(int no) const;
    /// \brief  Zwraca stringa zawierajacego liczbe spacji lub tabulatorow uzalezniona od poziomu rekurencji
    std::string spaceL(int lvl) const;

private:
    SkeletalModel::Ptr                      model;           //!< model dla ktorego nastapi zapis lub odczyta
    int                             boneCounter;     //!< dzieki tej zmiennej mozna nadac unikatowe identyfikatory dla kosci 
    std::vector<std::string>        tempParentVector;//!< stos, umozliwia popranwe ustalenie zaleznosci rodzic - dziecko (w rekurencji)
    SkeletalModel::JointIdMap                tempBonesID;     //!< pomocne przy wczytywaniu i tworzeniu hierarchii
    SkeletalModel::JointMap                  tempBonesMap;    //!< pomocne przy wczytywaniu i tworzeniu hierarchii
};
}

#endif