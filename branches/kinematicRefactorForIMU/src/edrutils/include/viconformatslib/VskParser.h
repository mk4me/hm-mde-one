#ifndef HEADER_GUARD_VICONFORMATS__VSKPARSER_H__
#define HEADER_GUARD_VICONFORMATS__VSKPARSER_H__

#include <string>
#include <vector>
#include <viconformatslib/Stick.h>
#include <viconformatslib/Marker.h>

namespace vicon
{
	/// \brief Klasa realizuje odczyt plików *.vsk
	class VskParser
	{
	public:
		/// \brief  Parsuje podany plik.
		/// \param  filename  Nazwa pliku do parsowania.
		static void parse(const std::string& filename,
			std::vector<Stick> & sticks,
			std::vector<Marker> & markers);	
	};
}

#endif
