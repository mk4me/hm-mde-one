#ifndef HEADER_GUARD_KINEMATICMODEL__VSKPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__VSKPARSER_H__

#include <string>
#include <iterator>
#include <vector>
#include <utils/SmartPtr.h>
#include <osg/Vec3>
#include <osg/Vec4>
#include <tinyxml2.h>

namespace kinematic
{
	/// \brief Klasa realizuje odczyt plików *.vsk
	class VskParser
	{
	public:
		//! Struktura opisuje połączenie między markerami
		struct Stick {
			std::string name1;  //<! nazwa pierwszego markera
			std::string name2;  //<! nazwa drugiego markera
			osg::Vec4 color;    //<! kolor połączenia
		};

		//! Struktura opisuje pojedynczy marker
		struct Marker {
			std::string name;   //<! nazwa markera
			double radius;      //<! promień markera
			osg::Vec4 color;    //<! kolor markera
		};

		typedef std::pair<std::vector<Stick>::const_iterator, std::vector<Stick>::const_iterator> stickIterators;
		typedef std::pair<std::vector<Marker>::const_iterator, std::vector<Marker>::const_iterator> markerIterators;
	public:
		VskParser();

	public:
		/// \brief  Parsuje podany plik.
		/// \param  filename  Nazwa pliku do parsowania.
		virtual void parse(const std::string& filename);
		//! \return wczytane połączenia (para niemodyfikowalnych iteratorów)
		stickIterators getSticks() const;
		//! \return wczytane markery (para niemodyfikowalnych iteratorów)
		markerIterators getMarkers() const;
		//! \return czy plik jest już załadowany
		bool isLoaded() const { return loaded; }

	private:
		//! Wczytanie połączeń między markerami
		//! \param sticks Element pliku vsk zawierający połączenia
    void readSticks(tinyxml2::XMLElement* sticks);

		//! wczytanie danych markerow
		//! \param markers Element pliku vsk zawierający dane markerow
    void readMarkers(tinyxml2::XMLElement* markers);

	private:
		//! czy plik został wczytany
		bool loaded;
		//! wczytane połączenia
		std::vector<Stick> sticks;
		//! wczytane markery
		std::vector<Marker> markers;
	};

	typedef utils::shared_ptr<VskParser> VskParserPtr;
	typedef utils::shared_ptr<const VskParser> VskParserConstPtr;
}

#endif
