#ifndef HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
#define HEADER_GUARD_acclaimMODEL__AMCPARSER_H__

#include <acclaimformatslib/MotionData.h>
#include <acclaimformatslib/Skeleton.h>

namespace acclaim
{
    /// \brief Klasa realizuje zapis i odczyt plików *.amc
    class AmcParser
    {
    public:
        /// \brief  Konstruktor parsera. 
        AmcParser();

    public:
        /// \brief  Parsuje podany plik. 
        /// \param  model reprezentacja wewnętrzna animacji
        /// \param  filename  Nazwa pliku do parsowania. 
        void parse(MotionData & data, const std::string& filename);
        /// \brief  Zapisuje dane z modelu do pliku *.amc
        /// \param  model reprezentacja wewnętrzna animacji
        /// \param  filename nazwa pliku do którego zostaną zapisane dane
		void save(const Skeleton & model, const MotionData & data, const std::string& filename);
        /// \return czy jest wymuszenie kolejności rotacji roota (zapewnia zgodność z niektórymi parserami)
        bool isForceRootXYZ() { return forceRootXYZ; }
        /// \brief ustawia wymuszenie kolejności rotacji roota (zapewnia zgodność z niektórymi parserami)
        void setForceRootXYZ(bool value) { forceRootXYZ = value; }

    private:
        bool forceRootXYZ; //!< wymuszenie kolejności rotacji roota (zapewnia zgodność z niektórymi parserami)
    };
}

#endif // HEADER_GUARD_acclaimMODEL__AMCPARSER_H__
