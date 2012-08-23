#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

//! klasa wykorzystuje bibliotekê kinematiclib w celu wczytania danych animacji szkieletowej
class KinematicParser : public core::IParser
{
    UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}", "Kinematic parser");
private:
    //! wrapper, do którego trafi¹ wczytane dane
    core::ObjectWrapperPtr skeletonData;

public:
    KinematicParser();
    virtual ~KinematicParser();

public:
    //! metoda odpowiada za wczytanie pliku z animacj¹ szkieletow¹
    //! \param path œcie¿ka do pliku 
    virtual void parseFile(const core::Filesystem::Path& path);
    //! tworzy pusty obiekt parsera
    virtual core::IParser* create();
    //! zwraca wspierane rozszerzenia plikow (amc, bvh)
    //! \param extensions kolecja, do której trafi¹ wspierane rozszerzenia
    virtual void getSupportedExtensions(Extensions & extensions) const;
    //! \param objects kolekcja z object wrapperem (skeletonData)
    virtual void getObjects(core::Objects& objects);
};

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__