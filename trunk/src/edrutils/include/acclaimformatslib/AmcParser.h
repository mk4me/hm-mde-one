#ifndef HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
#define HEADER_GUARD_acclaimMODEL__AMCPARSER_H__

#include <acclaimformatslib/MotionData.h>

namespace acclaim
{
    /// \brief Klasa realizuje zapis i odczyt plików *.amc
    class AmcParser
    {
    public:
        /// \brief  Parsuje podany plik. 
        /// \param  model reprezentacja wewnętrzna animacji
        /// \param  filename  Nazwa pliku do parsowania. 
		static void parse(MotionData & data, std::istream& stream);
        /// \brief  Zapisuje dane z modelu do pliku *.amc
        /// \param  model reprezentacja wewnętrzna animacji
        /// \param  filename nazwa pliku do którego zostaną zapisane dane
		static void serialize(const MotionData & data,
			std::ostream& stream);
    };
}

#endif // HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
