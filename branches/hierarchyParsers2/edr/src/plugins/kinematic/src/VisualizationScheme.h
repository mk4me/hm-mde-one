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

#include <utils/SmartPtr.h>
#include <corelib/PluginCommon.h>
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

//! Pomocnicza klasa do obsługi plików vsk. Likwiduje konieczność ich wielokrotnego parsowania
class Vsk
{
public:
    //! Okresla przygotowane wczesniej zestawy danych
	enum MarkersCount
	{
        MarkersCount16 = 16,
		MarkersCount39 = 39,
		MarkersCount53 = 53
	};

public:
	//! dzięki tej metodzie można pobrać przeparsowany plik vsk z obsługiwanych schematów. W przypadku niepowodzenia rzucany jest wyjątek
	//! \param count dostępny schemat
    //! \return Parser z wczytaną strukturą
	static kinematic::VskParserConstPtr get(MarkersCount count);

private:
    //! przygotowany schemat dla 16 markerów
    static kinematic::VskParserPtr Count16;
    //! przygotowany schemat dla 39 markerów
	static kinematic::VskParserPtr Count39;
    //! przygotowany schemat dla 53 markerów
	static kinematic::VskParserPtr Count53;
};

class ISchemeDrawer;
typedef utils::shared_ptr<ISchemeDrawer> ISchemeDrawerPtr;

//! klasa stanowi połączenie między reprezentacją wewnętrzną, a kontrolką odrysowywującą szkielet
class VisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct State {
        State() : position(), color(), isEffector(false), visible(true), name("UNKNOWN") {}
        osg::Vec3 position;  //!< pozycja w globalnym układzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint / marker jest liściem w hierarchii
        bool visible;        //!< czy joint / marker powinien być widoczny
        std::string name;    //!< nazwa jointa / markera
    };

    //! struktura zawiera jedno połączenie pomiędzy stawami lub markerami
    struct Connection {
        Connection() : index1(-1), index2(-1), color(), visible(true), length(-1.0f) {}
        //! indeks pierwszego stawu/markera (zgodny z przekazywaną tablica State)
        int index1;
        //! indeks drugiego stawu/markera (zgodny z przekazywaną tablica State)
        int index2;
        //! kolor połączenia (RGBA <0,1>)
        osg::Vec4 color;
        //! czy połączenie jest widoczne
        bool visible;
        //! długość połączenia
        float length;
    };

public:
    //! konstruktor
    VisualizationScheme();
    //! destruktor
    virtual ~VisualizationScheme() {}

public:
    //! \return znormalizowany czas ( z zakresu 0 - 1 )
    double getNormalizedTime() const { return normalizedTime; }
    //! ustawia znormalizowany czas 
    //! \param val nowy czas( z zakresu 0 - 1 )
    void setNormalizedTime(double val);
    //! Ustawia czas, schemat przechowuje go w postaci znormalizowanej, dlatego konieczna jest konwersja
    //! \param time ustawiany czas
    void setTime(double time) {
        setNormalizedTime(std::max(0.0, std::min(1.0, time / getDuration())));
    }
    //! Pobiera czas, schemat przechowuje go w postaci znormalizowanej, dlatego konieczna jest konwersja
    double getTime() const {
        return getNormalizedTime() * getDuration();
    }
    //! \return aktualny środek geometryczny
    virtual osg::Vec3 getCurrentPosition() const = 0;
    //! \return czas trwania animacji
    virtual double getDuration() const = 0;
    //! \return kolekcja ze stanami obiektów (pozycja, widoczność itp.)
    const std::vector<State> &getStates() const { return states; }
    //! \return kolekcja ze stanami połączeń między obiektami
    const std::vector<Connection> &getConnections() const { return connections; }
    //! \return czy schemat przechowuje już jakieś dane?
	virtual bool hasData() const = 0;
    //! usawia jednolity kolor dla wszystkich obiektów
    //! \param color ustawiany kolor (RGBA 0-1)
    void setGlobalStatesColor(const osg::Vec4& color);

protected:
    //! stany markerów dla aktualnego czasu
    std::vector<State> states;
    //! połączenia między markerami
    std::vector<Connection> connections;
    //! aktualny czas
    double normalizedTime;

protected:
    //! Uaktualnienie schematu
    //! \param time czas, do którego trzeba uaktualnić
    virtual void update(double time) = 0;
};

typedef utils::shared_ptr<VisualizationScheme> VisualizationSchemePtr;
typedef utils::weak_ptr<VisualizationScheme> VisualizationSchemeWeak;
typedef utils::shared_ptr<const VisualizationScheme> VisualizationSchemeConstPtr;
#endif
