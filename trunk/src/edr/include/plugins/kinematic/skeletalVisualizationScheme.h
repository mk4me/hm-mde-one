#ifndef HEADER_GUARD__SKELETALVISAULIZATIONSCHEME_H__
#define HEADER_GUARD__SKELETALVISAULIZATIONSCHEME_H__

#include <map>
#include <utils/Utils.h>
#include <utils/Debug.h>
UTILS_PUSH_WARNINGS
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Node>
UTILS_POP_WARNINGS

#include <core/SmartPtr.h>
#include <core/PluginCommon.h>
#include <boost/utility.hpp>
#include <boost/function.hpp>
#include <kinematiclib/VskParser.h>
#include <kinematiclib/JointAnglesCollection.h>
#include <plugins/c3d/C3DChannels.h>

#ifdef max
    #undef max
#endif

#ifdef min
    #undef min
#endif

//! Pomocnicza klasa do obslugi plikow vsk. Likwiduje koniecznosc ich wielokrotnego parsowania
class Vsk
{
public:
	enum MarkersCount
	{
        MarkersCount16 = 16,
		MarkersCount39 = 39,
		MarkersCount53 = 53
	};

public:
	static kinematic::VskParserConstPtr get(MarkersCount count) 
	{
		switch(count) 
		{
        case MarkersCount16:										
            if (!Count16->isLoaded()) {	
                Count16->parse(core::getResourceString("trial/CZD.vsk"));		
            } 																
            return Count16;
		case MarkersCount39:										
			if (!Count39->isLoaded()) {	
				Count39->parse(core::getResourceString("trial/M39.vsk"));		
			} 																
			return Count39;

		case MarkersCount53:										
			if (!Count53->isLoaded()) {											
				Count53->parse(core::getResourceString("trial/M53.vsk"));		
			} 																
			return Count53;

		default:
			throw std::runtime_error("Wrong VSK scheme was requested");
		}
	}

private:
    static kinematic::VskParserPtr Count16;
	static kinematic::VskParserPtr Count39;
	static kinematic::VskParserPtr Count53;
};



class ISchemeDrawer;
enum DataToDraw;
typedef core::shared_ptr<ISchemeDrawer> ISchemeDrawerPtr;
//! klasa stanowi polaczenie miedzy reprezentacja wewnetrzna, a kontrolka odrysowywujaca szkielet
class SkeletalVisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct JointState {
        JointState() : position(), color(), isEffector(false), visible(true), name("UNKNOWN") {}
        osg::Vec3 position;  //!< pozycja w globalnym ukladzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint jest lisciem w hierarchii
        bool visible;        //!< czy joint powinien byæ widoczny
        std::string name;    //!< nazwa jointa
    };

    //! struktura zawiera jedno polaczenie pomiedzy stawami lub markerami
    struct Connection {
        Connection() : index1(-1), index2(-1), color(), visible(true) {}
        //! indeks pierwszego stawu/markera (zgodny z przekazywana tablica JointState)
        int index1;         
        //! indeks drugiego stawu/markera (zgodny z przekazywana tablica JointState)
        int index2;
        //! kolor po³¹czenia (RGBA <0,1>)
        osg::Vec4 color;
        bool visible;
    };
            
public:
    SkeletalVisualizationScheme();

//public:
//    //! Metoda tworzy schemat wizualizacji, zachowujac przy okazji slaby wskaznik do niego
//    static core::shared_ptr<SkeletalVisualizationScheme> create();

public: // akcesory
    double getNormalizedTime() const { return normalizedTime; }
    void setNormalizedTime(double val);

    void setTime(double time) {
        //TODO
        //Trzeba to zweryfikowaæ!!
        setNormalizedTime(std::max(0.0, std::min(1.0, time / getDuration())));
    }

    double getTime() const {
        return getNormalizedTime() * getDuration();
    }

    const osg::Vec3& getCurrentPosition() const { return currentPosition; }

    int getNumFrames() const { 
        UTILS_ASSERT(joints);  
        return joints->getNumPointsPerChannel(); 
    }
    double getFrameTime() const {
        UTILS_ASSERT(joints); 
        return static_cast<double>(joints->getLength()) / joints->getNumPointsPerChannel(); 
    }
    double getDuration() const { 
        UTILS_ASSERT(joints || markers);
        if (joints) {
            return static_cast<double>(joints->getLength()); 
        } else if (markers) {
            return markers->getLength();
        }
        UTILS_ASSERT(false);
        return 0.0;
    }

    //! akcesory do zwracanych danych
    const std::vector<JointState> &getMarkersStates() const { return markersStates; }
    const std::vector<JointState> &getJointStates() const { return jointMarkersStates; }
    const std::vector<Connection> &getJointConnections() const { return jointConnections; }
    const std::vector<Connection> &getMarkerConnections() const { return markerConnections; }

    const std::vector<JointState> &getStates(DataToDraw toDraw) const;
    const std::vector<Connection> &getConnections(DataToDraw toDraw) const;

	bool hasData() const { return markers || joints; }
	bool hasMarkers() const { return markers.get() != nullptr; }
	bool hasJoints() const { return joints.get() != nullptr; }

	MarkerCollectionConstPtr getMarkers() const { return markers; }
	kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }

	void setMarkers(MarkerCollectionConstPtr val);
	void setJoints(kinematic::JointAnglesCollectionConstPtr val);
    void setMarkersDataFromVsk(kinematic::VskParserConstPtr vsk);

        
private:
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! odswiezenie informacji o markerach
    void updateMarkers(double time);
    //! obliczenie poczatkowych transformacji
    //void computeBindPoses(KinematicModelPtr model);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);

    void createSkeletonConnections(kinematic::hAnimJointPtr joint);

private:
    //! aktualny czas
    double normalizedTime; 
    //! pomocne przy zapelnianiu kolekcji jointow
    int counterHelper;
    //! stany markerow dla aktualnego czasu
    std::vector<JointState> markersStates;
    //! stany stawow dla aktualnego czasu
    std::vector<JointState> jointMarkersStates;
    //! ulatwia ineksowanie jointow
    std::map<kinematic::hAnimJointPtr, int> visJoints;
    //! zawiera informacje o polaczeniach miedzy stawami
    std::vector<Connection> jointConnections;
    std::vector<Connection> markerConnections;
	MarkerCollectionConstPtr markers;
	kinematic::JointAnglesCollectionConstPtr joints;
    //! slaby wskaznik do this
    //boost::weak_ptr<SkeletalVisualizationScheme> weak;
    osg::Vec3 currentPosition;
};

typedef core::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef core::weak_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemeWeak;
typedef core::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif
