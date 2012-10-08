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

/// \brief Klasa realizuje odczyt i zapis plików *.bvh
class BvhParser
{
public:
    BvhParser();
    virtual ~BvhParser();

public:
    /// \brief  Zapisuje dane z modelu do pliku *.asf
    /// \param  model reprezentacja wewnętrzna animacji
    /// \param  filename nazwa pliku, do którego zostaną zapisane dane
    virtual void save(SkeletalModelConstPtr model, SkeletalDataConstPtr data, const std::string& filename );
    /// \brief  Parsuje podany plik. 
    /// \param  model reprezentacja wewnętrzna animacji
    /// \param  filename  Nazwa pliku do parsowania. 
    virtual void parse(SkeletalModelPtr model, SkeletalDataPtr data, const std::string& filename );

private:			
	//! jeśli true, to wymuszony zostaje format kanałów (3 RZ,RX,RY lub 6 TX,TY,TZ,RZ,RX,RY)
    bool forceZXY; 	
	//! jeśli ustawione, to w pliku zostaną wstawione tabulatory, spacje w przeciwnym razie
    bool tabs;     

private:
    /// \brief  Wczytuje pojedynczego jointa z pliku. Wywoływane rekurencyjnie
    /// \param [in] in strumień wejściowy. 
    /// \param [in,out] jointList Lista, do której trafia kolejność wystapienia jointow. 
    void readSingleJoint(SkeletalModelPtr model, std::istream& in, std::list<std::string>& jointList);
    /// \brief  Zwraca nazwę kanału (w formacie biovision, np. xPosition)
    /// \param  channel kanał, dla którego zostanie zwrócona nazwa. 
    /// \return The channel name. 
    std::string getChannelName(DegreeOfFreedom::Channel channel) const;
    /// \brief  Pobiera wartości kanałów dla konkretnej kości i konkretnej klatki. 
    /// \param  frameNo     Nr klatki animacji 
    /// \param  boneName    Nazwa kości
    /// \return Stan kości, z wartościami kanałów. 
	SkeletalData::singleJointStatePtr getSingleJointState(SkeletalDataConstPtr data,int frameNo, std::string boneName) const;
    /// \brief  Zapisuje pojedynczy staw
	/// \param model				Model ze szkieletem
    /// \param [in,out] out         strumień wyjsciowy
    /// \param  bone                Kość do zapisu 
    /// \param  parent				Kość rodzica (przydatne przy sprawdzaniu i usuwaniu dummy bones).
    /// \param  lvl                 Poziom rekurencji (do wciec w pliku) 
    /// \param [in,out] jointList   Do listy trafiaja w odpowiedniej kolejności nazwy zapisywanych kości 
    void saveJoint(SkeletalModelConstPtr model, std::ostream& out, JointConstPtr bone, JointConstPtr parent, int lvl, std::list<std::string>& jointList) const;
    /// \brief  Zapisuje kolejność zapisu kanałów dla jointa 
    /// \param [in]        strumień wyjsciowy. 
    /// \param  bone       Kość, dla której będzie zapisana hierarchia. 
    /// \param  lvl        Poziom rekurencji (do wciec w pliku) 
    void saveChannelsInHierarchy(std::ostream& out, JointConstPtr bone, int lvl) const;
    /// \brief  Czy podane stopnie swobody zawieraja tez dotyczace przesuniecia
    /// \param [in] dofs    Wektor ze stopniami swobody 
    /// \return true jeśli znaleziono kanał z transformacja. 
    bool hasTranslation(const std::vector<DegreeOfFreedom>& dofs);
    /// \brief  Metoda tworzy kości z jointow. Możliwe, ze zmieni się hierarchia (dojda elementy - dummy bones)
	void setBones(SkeletalModelPtr model);
    /// \brief  Wywoływane z setBones. Bierze udzial w tworzeniu nowej hierarchi (kości zamiast joinow)
    /// \param  newBone Nowo stworzona kość w hierarchii. 
    void HandleBone(SkeletalModelPtr model, JointPtr newBone);
    /// \brief  Zwraca kolejność rotacji.
    /// \param  bone    Kość, dla której ma być zwrócona kolejność. 
    /// \return Kolejność rotacji
    Axis::Order getAxisOrder(JointPtr bone) const;
    /// \brief  Zwraca stringa zawierającego podana liczbę spacji 
    std::string space(int no) const;
    /// \brief  Zwraca stringa zawierającego liczbę spacji lub tabulatorów uzależniona od poziomu rekurencji
    std::string spaceL(int lvl) const;

private:				
	//! dzięki tej zmiennej mozna nadać unikatowe identyfikatory dla kości 
    int boneCounter;			    
	//! stos, umozliwia popranwe ustalenie zależności rodzic - dziecko (w rekurencji)
    std::vector<std::string> tempParentVector;			
	//! pomocne przy wczytywaniu i tworzeniu hierarchii
    SkeletalModel::JointIdMap tempBonesID;       
	//! pomocne przy wczytywaniu i tworzeniu hierarchii
    SkeletalModel::JointMap tempBonesMap;      
};
}

#endif
