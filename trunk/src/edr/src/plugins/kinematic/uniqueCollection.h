#ifndef HEADER_GUARD__UNIQUECOLLECTION_H__
#define HEADER_GUARD__UNIQUECOLLECTION_H__

#include <utils/Utils.h>
#include <utils/Enum.h>
#include <utils/Debug.h>

//! Prosta metoda, rzucajaca ostra asercja, gdy probujemy dodac obiekt, ktory juz istnieje w kolecji
//! \param vec kolecja, do ktorej dodajemy obiekt
//! \param val dodawany obiekt
template<typename T>
void pushBackUnique( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it == vec.end());
    vec.push_back(val);
}

//! Prosta metoda, rzucajaca ostra asercja, gdy probujemy usunac obiekt, ktorego nie ma w kolecji
//! \param vec kolecja, z ktorej usuwamy obiekt
//! \param val usuwany obiekt
template<typename T>
void removeFromVector( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it != vec.end());
    vec.erase(it);
}



#endif
