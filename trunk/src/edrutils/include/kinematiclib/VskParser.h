#ifndef HEADER_GUARD_KINEMATICMODEL__VSKPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__VSKPARSER_H__

#include <string>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <osg/Vec3>
#include <osg/Vec4>
#include <tinyxml.h>

namespace kinematic
{
/// \brief Klasa realizuje odczyt plikow *.vsk
class VskParser
{
public:
    //! Struktura opisuje polaczenie miedzy markerami
    struct Stick {
        std::string name1;  //<! nazwa pierwszego markera
        std::string name2;  //<! nazwa drugiego markera
        osg::Vec4 color;    //<! kolor polaczenia
    };

    //! Struktura opisuje pojedynczy marker
    struct Marker {
        std::string name;   //<! nazwa markera
        double radius;      //<! promien markera
        osg::Vec4 color;    //<! kolor markera
    };

    typedef std::pair<std::vector<Stick>::const_iterator, std::vector<Stick>::const_iterator> stickIterators;
    typedef std::pair<std::vector<Marker>::const_iterator, std::vector<Marker>::const_iterator> markerIterators;
public:
    VskParser();

public:
    /// \brief  Parsuje podany plik.
    /// \param  filename  Nazwa pliku do parsowania.
    virtual void parse(const std::string& filename );

    stickIterators getSticks() const;
    markerIterators getMarkers() const;

	bool isLoaded() const { return loaded; }


private:
    //! Wczytanie polaczen miedzy markerami
    //! \param sticks Element pliku vsk zawierajacy polaczenia
    void readSticks(TiXmlElement* sticks);

    //! wczytanie danych markerow
    //! \param markers Element pliku vsk zawierajacy dane markerow
    void readMarkers(TiXmlElement* markers);

private:
    bool loaded;
    //! wczytane polaczenia
    std::vector<Stick> sticks;
    //! wczytane markery
    std::vector<Marker> markers;
};

typedef boost::shared_ptr<VskParser> VskParserPtr;
typedef boost::shared_ptr<const VskParser> VskParserConstPtr;
}

#endif
