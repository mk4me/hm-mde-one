#ifndef HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__BVHPARSER_H__

#include <list>
#include <kinematiclib/Skeleton.h>
#include <kinematiclib/SkeletalData.h>
#include <kinematiclib/SkeletalModel.h>
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
    virtual void save(SkeletalModelConstPtr model, SkeletalDataConstPtr data, const std::string& filename );
    /// \brief  Parsuje podany plik. 
    /// \param  model reprezentacja wewnetrzna animacji
    /// \param  filename  Nazwa pliku do parsowania. 
    virtual void parse(SkeletalModelPtr model, SkeletalDataPtr data, const std::string& filename );

private:			
	//! jesli true to wymuszony zostaje format kanalow (3 RZ,RX,RY lub 6 TX,TY,TZ,RZ,RX,RY)
    bool forceZXY; 	
	//! jesli ustawione to w pliku zostana wstawione tabulatory, spacje w przeciwnym razie
    bool tabs;     

private:
    /// \brief  Wczytuje pojedynczego jointa z pliku. Wywolywane rekurencyjnie
    /// \param [in] in Strumien wejsciowy. 
    /// \param [in,out] jointList Lista, do ktorej trafia kolejnosc wystapienia jointow. 
    void readSingleJoint(SkeletalModelPtr model, std::istream& in, std::list<std::string>& jointList);
    /// \brief  Zwraca nazwe kanalu (w formacie biovision, np. xPosition)
    /// \param  channel Kanal, dla ktorego zostanie zwrocona nazwa. 
    /// \return The channel name. 
    std::string getChannelName(DegreeOfFreedom::Channel channel) const;
    /// \brief  Pobiera wartosci kanalow dla konkretnej kosci i konkretnej klatki. 
    /// \param  frameNo     Nr klatki animacji 
    /// \param  boneName    Nazwa kosci
    /// \return Stan kosci, z wartosciami kanalow. 
	SkeletalData::singleJointStatePtr getSingleJointState(SkeletalDataConstPtr data,int frameNo, std::string boneName) const;
    /// \brief  Zapisuje pojedynczy staw
	/// \param model				Model ze szkieletem
    /// \param [in,out] out         Strumien wyjsciowy
    /// \param  bone                Kosc do zapisu 
    /// \param  parent				Kosc rodzica (przydatne przy sprawdzaniu i usuwaniu dummy bones).
    /// \param  lvl                 Poziom rekurencji (do wciec w pliku) 
    /// \param [in,out] jointList   Do listy trafiaja w odpowiedniej kolejnosci nazwy zapisywanych kosci 
    void saveJoint(SkeletalModelConstPtr model, std::ostream& out, JointConstPtr bone, JointConstPtr parent, int lvl, std::list<std::string>& jointList) const;
    /// \brief  Zapisuje kolejnosc zapisu kanalow dla jointa 
    /// \param [in]        Strumien wyjsciowy. 
    /// \param  bone       Kosc, dla ktorej bedzie zapisana hierarchia. 
    /// \param  lvl        Poziom rekurencji (do wciec w pliku) 
    void saveChannelsInHierarchy(std::ostream& out, JointConstPtr bone, int lvl) const;
    /// \brief  Czy podane stopnie swobody zawieraja tez dotyczace przesuniecia
    /// \param [in] dofs    Wektor ze stopniami swobody 
    /// \return true jesli znaleziono kanal z transformacja. 
    bool hasTranslation(const std::vector<DegreeOfFreedom>& dofs);
    /// \brief  Metoda tworzy kosci z jointow. Mozliwe, ze zmieni sie hierarchia (dojda elementy - dummy bones)
	void setBones(SkeletalModelPtr model);
    /// \brief  Wywolywane z setBones. Bierze udzial w tworzeniu nowej hierarchi (kosci zamiast joinow)
    /// \param  newBone Nowo stworzona kosc w hierarchii. 
    void HandleBone(SkeletalModelPtr model, JointPtr newBone);
    /// \brief  Zwraca kolejnosc rotacji.
    /// \param  bone    Kosc, dla ktorej ma byc zwrocona kolejnosc. 
    /// \return Kolejnosc rotacji
    Axis::Order getAxisOrder(JointPtr bone) const;
    /// \brief  Zwraca stringa zawierajacego podana liczbe spacji 
    std::string space(int no) const;
    /// \brief  Zwraca stringa zawierajacego liczbe spacji lub tabulatorow uzalezniona od poziomu rekurencji
    std::string spaceL(int lvl) const;

private:				
	//! dzieki tej zmiennej mozna nadac unikatowe identyfikatory dla kosci 
    int boneCounter;			    
	//! stos, umozliwia popranwe ustalenie zaleznosci rodzic - dziecko (w rekurencji)
    std::vector<std::string> tempParentVector;			
	//! pomocne przy wczytywaniu i tworzeniu hierarchii
    SkeletalModel::JointIdMap tempBonesID;       
	//! pomocne przy wczytywaniu i tworzeniu hierarchii
    SkeletalModel::JointMap tempBonesMap;      
};
}

#endif