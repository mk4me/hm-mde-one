#ifndef HEADER_GUARD_VICONFORMATS__VSKPARSER_H__
#define HEADER_GUARD_VICONFORMATS__VSKPARSER_H__

#include <string>
#include <vector>
#include <viconformatslib/Stick.h>
#include <viconformatslib/Marker.h>
#include <utils/SmartPtr.h>

namespace vicon
{
	//! Dane wczytane z pliku vsk
	struct Vsk {
		std::vector<Stick> sticks;
		std::vector<Marker> markers;
	};
	DEFINE_SMART_POINTERS(Vsk);

	/// \brief Klasa realizuje odczyt plików *.vsk
	class VskParser
	{
	public:
		/// \brief  Parsuje podany plik.
		/// \param  filename  Nazwa pliku do parsowania.
		static void parse(const std::string& filename, Vsk& vsk);
	};
}

#endif
