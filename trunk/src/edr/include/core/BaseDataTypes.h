﻿#ifndef BASE_DATA_TYPES_H
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

//! Klasa generująca unikalne identyfikatory dla żądanych ciągów znaków
//! zapewnia jednoznaczne przyporządkowanie ciąg znaków <-> id,
//! dla tego samego ciągu znaków zwraca ten sam id. Wymagane jest więc dostarczanie z zewnątrz
//! unikalnych ciągów znaków - polecamy jakiś GUID generator (np. z Visual Studio)
//! Ta klasa i definicja typu dla identyfikatora normalizują różne możliwe podejścia do unikalnych identyfikatorów w aplikacji
class UID 
{
public:
//! Statyczna metoda
//! \param uuid Ciąg znaków dla którego generujemy unikalny identyfikator
//! \return Unikalny identyfikator
static UniqueID GenerateUniqueID(const std::string& uuid) 
{
    boost::uuids::string_generator gen;
    boost::uuids::uuid id = gen(uuid);
    return id;
}
};

#endif //BASE_SERVICE_H
