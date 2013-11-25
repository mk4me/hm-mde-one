#ifndef __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
#define __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__

#include <corelib/IParser.h>
#include <corelib/IDataManagerReader.h>

//! klasa wykorzystuje bibliotekę kinematiclib w celu wczytania danych animacji szkieletowej
class KinematicParser : public plugin::IParser, public plugin::ISourceParserCapabilities
{
	UNIQUE_ID("{AE673002-AE76-4E79-983E-ABDB15713E74}")
	CLASS_DESCRIPTION("Kinematic parser", "Kinematic parser")
private:
    //! wrapper, do którego trafią wczytane dane
    core::ObjectWrapperPtr skeletonData;

public:
    KinematicParser();
    virtual ~KinematicParser();

public:
    //! metoda odpowiada za wczytanie pliku z animacją szkieletową
    //! \param path ścieżka do pliku 
    virtual void parse(const std::string & source);
    //! tworzy pusty obiekt parsera
    virtual plugin::IParser* create() const;
    //! zwraca wspierane rozszerzenia plików (amc, bvh)
    //! \param extensions kolecja, do której trafią wspierane rozszerzenia
    virtual void acceptedExpressions(Expressions & expressions) const;
    //! \param objects kolekcja z object wrapperem (skeletonData)
    virtual void getObjects(core::Objects& objects);
};

#endif  // __HEADER_GUARD_KINEMATIC__KINEMATICPARSER_H__
