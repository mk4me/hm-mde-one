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
#include <plugins/c3d/C3DCollections.h>

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
class VisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct State {
        State() : position(), color(), isEffector(false), visible(true), name("UNKNOWN") {}
        osg::Vec3 position;  //!< pozycja w globalnym ukladzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint / marker jest lisciem w hierarchii
        bool visible;        //!< czy joint / marker powinien byæ widoczny
        std::string name;    //!< nazwa jointa / markera
    };

    //! struktura zawiera jedno polaczenie pomiedzy stawami lub markerami
    struct Connection {
        Connection() : index1(-1), index2(-1), color(), visible(true) {}
        //! indeks pierwszego stawu/markera (zgodny z przekazywana tablica State)
        int index1;         
        //! indeks drugiego stawu/markera (zgodny z przekazywana tablica State)
        int index2;
        //! kolor po³¹czenia (RGBA <0,1>)
        osg::Vec4 color;
        bool visible;
    };
            
public:
    VisualizationScheme();
    virtual ~VisualizationScheme() {}

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

    virtual osg::Vec3 getCurrentPosition() const = 0;

    
    virtual double getDuration() const = 0;

    //! akcesory do zwracanych danych
    const std::vector<State> &getStates() const { return states; }
    const std::vector<Connection> &getConnections() const { return connections; }

	virtual bool hasData() const = 0;


protected:
    //! stany markerow dla aktualnego czasu
    std::vector<State> states;
    std::vector<Connection> connections;
    //! aktualny czas
    double normalizedTime; 

protected:
    virtual void update(double time) = 0;
};

typedef core::shared_ptr<VisualizationScheme> VisualizationSchemePtr;
typedef core::weak_ptr<VisualizationScheme> VisualizationSchemeWeak;
typedef core::shared_ptr<const VisualizationScheme> VisualizationSchemeConstPtr;


class MarkersVisualizationScheme : public VisualizationScheme
{
public:
    MarkerCollectionConstPtr getMarkers() const { return markers; }
    virtual double getDuration() const 
    {
        if (markers) {
            return markers->getLength();
        }
        UTILS_ASSERT(false);
        return 0.0;
    }

    virtual bool hasData() const { return markers.get(); }

    void setMarkers(MarkerCollectionConstPtr val);
    void setMarkersDataFromVsk(kinematic::VskParserConstPtr vsk);
    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    osg::Vec3 getRootPosition(double time) const;

protected:
    virtual void update( double time )
    {
        updateMarkers(time);
    }

private:
    //! odswiezenie informacji o markerach
    void updateMarkers(double time);


private:
    MarkerCollectionConstPtr markers;
    osg::Vec3 currentPosition;
};
typedef core::shared_ptr<MarkersVisualizationScheme> MarkersVisualizationSchemePtr;
typedef core::shared_ptr<const MarkersVisualizationScheme> MarkersVisualizationSchemeConstPtr;

class SkeletalVisualizationScheme : public VisualizationScheme
{
public:

    kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
    void setJoints(kinematic::JointAnglesCollectionConstPtr val);

    virtual int getNumFrames() const { 
        UTILS_ASSERT(joints);  
        return joints->getNumPointsPerChannel(); 
    }
    double getFrameTime() const {
        UTILS_ASSERT(joints); 
        return static_cast<double>(joints->getLength()) / joints->getNumPointsPerChannel(); 
    }

    virtual double getDuration() const  { 
        if (joints) {
            return static_cast<double>(joints->getLength()); 
        } 
        UTILS_ASSERT(false);
        return 0.0;
    }

    virtual bool hasData() const { return joints.get(); }

    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    osg::Vec3 getRootPosition(double time) 
    {
        return joints->getRootPosition(time);
    }

protected:
    virtual void update( double time )
    {
        updateJointTransforms(time);
        currentPosition = getRootPosition(time);

        //int count = states.size();
        //for (int i = 0; i < count; i++) {
        //    states[i].position -= currentPosition;
        //}
    }

private:
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! obliczenie poczatkowych transformacji
    //void computeBindPoses(KinematicModelPtr model);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);

    void createSkeletonConnections(kinematic::hAnimJointPtr joint);

private:
    //! ulatwia ineksowanie jointow
    std::map<kinematic::hAnimJointPtr, int> visJoints;
	kinematic::JointAnglesCollectionConstPtr joints;
    //! pomocne przy zapelnianiu kolekcji jointow
    int counterHelper;
    osg::Vec3 currentPosition;
};
typedef core::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef core::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif
