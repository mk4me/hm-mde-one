#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <core/IParser.h>
#include <core/IDataManager.h>

//! klasa wykorzystuje biblioteke kinematiclib w celu wczytania danych animacji szkieletowej
class KinematicParser : public core::IParser
{
    UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}", "Kinematic parser");
private:
    //! wrapper do ktorego trafia wczytane dane
    core::ObjectWrapperPtr skeletonData;

public:
    KinematicParser();
    virtual ~KinematicParser();

public:
    //! 
    //! \param path 
    virtual void parseFile(const core::Filesystem::Path& path);
    //! tworzy pusta obiekt parsera
    virtual core::IParser* create();
    //! zwraca wspierane rozszerzenia plikow (amc, bvh)
    //! \param extensions kolecja do ktorej trafia wspierane rozszerzenia
    virtual void getSupportedExtensions(Extensions & extensions) const;
    //! \param objects kolekcja z object wrapperem (skeletonData)
    virtual void getObjects(core::Objects& objects);
};

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__