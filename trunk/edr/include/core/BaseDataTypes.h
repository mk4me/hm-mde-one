#ifndef BASE_DATA_TYPES_H
#define BASE_DATA_TYPES_H

#include <stdint.h>
#include <string>
#include <ostream>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

//! Typ unikalnego identyfikatora
typedef boost::uuids::uuid UniqueID;

//! Klasa generuj�ca unikalne identyfikatory dla zadanych ci�g�w znak�w
//! zapewnia jednoznaczne przyporz�dkowanie ci�g znak�w <-> id,
//! dla tego samego ci�gu znak�w zwraca ten sam id. Wymagane jest wi�c dostarczanie z zewn�trz
//! unikalnych ci�g�w znak�w - polecamy jaki� GUID generator (np. z Visual Studio)
//! Ta klasa i definicja typu dla identyfikatora normalizuj� r�ne mo�liwe podej�cia do unikalnych identyfikator�w w aplikacji
class UID 
{
public:
//! Statyczna metoda
//! \param uuid Ci�g znak�w dla kt�rego generujemy unikalny identyfikator
//! \return Unikalny identyfikator
static UniqueID GenerateUniqueID(const std::string& uuid) 
{
    boost::uuids::string_generator gen;
    boost::uuids::uuid id = gen(uuid);
    return id;
}
};

#endif //BASE_SERVICE_H