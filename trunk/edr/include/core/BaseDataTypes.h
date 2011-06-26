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

//! Klasa generuj¹ca unikalne identyfikatory dla zadanych ci¹gów znaków
//! zapewnia jednoznaczne przyporz¹dkowanie ci¹g znaków <-> id,
//! dla tego samego ci¹gu znaków zwraca ten sam id. Wymagane jest wiêc dostarczanie z zewn¹trz
//! unikalnych ci¹gów znaków - polecamy jakiœ GUID generator (np. z Visual Studio)
//! Ta klasa i definicja typu dla identyfikatora normalizuj¹ ró¿ne mo¿liwe podejœcia do unikalnych identyfikatorów w aplikacji
class UID 
{
public:
//! Statyczna metoda
//! \param uuid Ci¹g znaków dla którego generujemy unikalny identyfikator
//! \return Unikalny identyfikator
static UniqueID GenerateUniqueID(const std::string& uuid) 
{
    boost::uuids::string_generator gen;
    boost::uuids::uuid id = gen(uuid);
    return id;
}
};

#endif //BASE_SERVICE_H