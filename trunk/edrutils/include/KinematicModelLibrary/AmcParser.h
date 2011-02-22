#ifndef HEADER_GUARD_KINEMATICMODEL__AMCPARSER_H__
#define HEADER_GUARD_KINEMATICMODEL__AMCPARSER_H__

#include "SkeletalModel.h"
namespace hmAnimation
{
    /// \brief Klasa realizuje zapis i odczyt plikow *.amc
    class AmcParser
    {
    public:
        /// \brief  Konstruktor parsera. 
        AmcParser();

    public:
        /// \brief  Parsuje podany plik. 
        /// \param  model reprezentacja wewnetrzna animacji
        /// \param  filename  Nazwa pliku do parsowania. 
        void parse(SkeletalModel::Ptr model, const std::string& filename);
        /// \brief  Zapisuje dane z modelu do pliku *.amc
        /// \param  model reprezentacja wewnetrzna animacji
        /// \param  filename nazwa pliku do ktorego zostana zapisane dane
        void save (const SkeletalModel::Ptr model, const std::string& filename);

    private:
        bool forceRootXYZ; //!< wymuszenie kolejnosci rotacji roota (zapewnia zgodnosc z niektorymi parserami)
    };
}

#endif