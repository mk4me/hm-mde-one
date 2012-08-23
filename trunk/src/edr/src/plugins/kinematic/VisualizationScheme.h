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

//! Pomocnicza klasa do obs³ugi plików vsk. Likwiduje koniecznoœæ ich wielokrotnego parsowania
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
	//! dziêki tej metodzie mo¿na pobraæ przeparsowany plik vsk z obs³ugiwanych schematów. W przypadku niepowodzenia rzucany jest wyj¹tek
	//! \param count dostêpny schemat
    //! \return Parser z wczytan¹ struktur¹
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
typedef core::shared_ptr<ISchemeDrawer> ISchemeDrawerPtr;

//! klasa stanowi po³¹czenie miêdzy reprezentacj¹ wewnêtrzn¹, a kontrolk¹ odrysowywuj¹c¹ szkielet
class VisualizationScheme
{
public:
    //! stan pojedynczego markera / jointa
    struct State {
        State() : position(), color(), isEffector(false), visible(true), name("UNKNOWN") {}
        osg::Vec3 position;  //!< pozycja w globalnym uk³adzie odniesienia
        osg::Vec4 color;     //!< kolor w formacie RGBA <0,1>
        bool isEffector;     //!< czy joint / marker jest liœciem w hierarchii
        bool visible;        //!< czy joint / marker powinien byæ widoczny
        std::string name;    //!< nazwa jointa / markera
    };

    //! struktura zawiera jedno po³¹czenie pomiêdzy stawami lub markerami
    struct Connection {
        Connection() : index1(-1), index2(-1), color(), visible(true), length(-1.0f) {}
        //! indeks pierwszego stawu/markera (zgodny z przekazywan¹ tablica State)
        int index1;
        //! indeks drugiego stawu/markera (zgodny z przekazywan¹ tablica State)
        int index2;
        //! kolor po³¹czenia (RGBA <0,1>)
        osg::Vec4 color;
        //! czy po³¹czenie jest widoczne
        bool visible;
        //! d³ugoœæ po³¹czenia
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
    //! \return aktualny œrodek geometryczny
    virtual osg::Vec3 getCurrentPosition() const = 0;
    //! \return czas trwania animacji
    virtual double getDuration() const = 0;
    //! \return kolekcja ze stanami obiektów (pozycja, widocznoœæ itp.)
    const std::vector<State> &getStates() const { return states; }
    //! \return kolekcja ze stanami po³¹czeñ miêdzy obiektami
    const std::vector<Connection> &getConnections() const { return connections; }
    //! \return czy schemat przechowuje ju¿ jakieœ dane?
	virtual bool hasData() const = 0;
    //! usawia jednolity kolor dla wszystkich obiektów
    //! \param color ustawiany kolor (RGBA 0-1)
    void setGlobalStatesColor(const osg::Vec4& color);

protected:
    //! stany markerów dla aktualnego czasu
    std::vector<State> states;
    //! po³¹czenia miêdzy markerami
    std::vector<Connection> connections;
    //! aktualny czas
    double normalizedTime;

protected:
    //! Uaktualnienie schematu
    //! \param time czas, do którego trzeba uaktualniæ
    virtual void update(double time) = 0;
};

typedef core::shared_ptr<VisualizationScheme> VisualizationSchemePtr;
typedef core::weak_ptr<VisualizationScheme> VisualizationSchemeWeak;
typedef core::shared_ptr<const VisualizationScheme> VisualizationSchemeConstPtr;
#endif
