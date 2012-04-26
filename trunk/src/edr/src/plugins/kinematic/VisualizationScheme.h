/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:45
	filename: 	VisualizationScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__VISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__VISUALIZATIONSCHEME_H__


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
        Connection() : index1(-1), index2(-1), color(), visible(true), length(-1.0f) {}
        //! indeks pierwszego stawu/markera (zgodny z przekazywana tablica State)
        int index1;         
        //! indeks drugiego stawu/markera (zgodny z przekazywana tablica State)
        int index2;
        //! kolor po³¹czenia (RGBA <0,1>)
        osg::Vec4 color;
        bool visible;

        float length;
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
#endif