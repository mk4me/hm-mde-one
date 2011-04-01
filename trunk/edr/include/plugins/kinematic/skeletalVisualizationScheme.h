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

#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <kinematiclib/KinematicModel.h>

//#include "uniqueCollection.h"

class ISchemeDrawer;
typedef boost::shared_ptr<ISchemeDrawer> ISchemeDrawerPtr;
//! klasa stanowi polaczenie miedzy reprezentacja wewnetrzna, a kontrolka odrysowywujaca szkielet
class SkeletalVisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct JointState {
        osg::Vec3 position;  //!< pozycja w globalnym ukladzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint jest lisciem w hierarchii
    };

    //! struktura zawiera jedno polaczenie pomiedzy stawami lub markerami
    struct Connection {
        //! indeks pierwszego stawu/markera (zgodny z przekazywana tablica JointState)
        int index1;         
        //! indeks drugiego stawu/markera (zgodny z przekazywana tablica JointState)
        int index2;
        //! kolor po��czenia (RGBA <0,1>)
        osg::Vec4 color;
    };
            
private:
    SkeletalVisualizationScheme();

public:
    //! Metoda tworzy schemat wizualizacji, zachowujac przy okazji slaby wskaznik do niego
    static boost::shared_ptr<SkeletalVisualizationScheme> create();

public: // akcesory
    double getNormalizedTime() const { return normalizedTime; }
    void setNormalizedTime(double val);

    void setTime(double time) {
        setNormalizedTime(time / getDuration());
    }

    double getTime() const {
        return getNormalizedTime() * getDuration();
    }

    int getNumFrames() const { 
        UTILS_ASSERT(kinematicModel);  
        return kinematicModel->getNumFrames(); 
    }
    double getFrameTime() const { 
        UTILS_ASSERT(kinematicModel); 
        return kinematicModel->getFrameTime(); 
    }
    double getDuration() const { 
        UTILS_ASSERT(kinematicModel);
        if (kinematicModel->hasSkeleton()) {
            return getNumFrames() * getFrameTime(); 
        } else if (kinematicModel->hasMarkers()) {
            return kinematicModel->getMarkersData()->getDuration();
        }
        UTILS_ASSERT(false);
        return 0.0;
    }

    //! akcesory do zwracanych danych
    const std::vector<JointState>  &getMarkersStates() const { return markersStates; }
    const std::vector<JointState>  &getJointStates() const { return jointMarkersStates; }
    const std::vector<Connection>  &getJointConnections() const  { return jointConnections; }

    //model kinematyczny
    kinematic::KinematicModelConstPtr getKinematicModel() const { return kinematicModel; }
    void setKinematicModel(kinematic::KinematicModelConstPtr val);

    ////! ustawia schemat odrysowywania modelu
    //void setSchemeDrawer(ISchemeDrawerPtr drawer);
    //ISchemeDrawerPtr getSchemeDrawer() const { return schemeDrawer; }
        
private:
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! odswiezenie informacji o markerach
    void updateMarkers(double time);
    //! obliczenie poczatkowych transformacji
    void computeBindPoses(kinematic::KinematicModelPtr model);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    void updateJointTransforms(const std::map<std::string, osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);
    void updateWorldTransforms(osg::Vec3 worldPosition, osg::Quat worldRotation);
    void updateSkinnedTransforms();

    void createSkeletonConnections(kinematic::JointPtr joint);

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
    std::map<kinematic::JointPtr, int> visJoints;
    //! zawiera informacje o polaczeniach miedzy stawami
    std::vector<Connection> jointConnections;
    ////! aktualny wizualizator schematu
    //ISchemeDrawerPtr schemeDrawer;
    //! model kinematyczny z danymi
    kinematic::KinematicModelConstPtr kinematicModel;
    //! slaby wskaznik do this
    boost::weak_ptr<SkeletalVisualizationScheme> weak;
};

typedef boost::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef boost::weak_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemeWeak;
typedef boost::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif