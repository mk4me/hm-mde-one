#ifndef HEADER_GUARD__UNIQUECOLLECTION_H__
#define HEADER_GUARD__UNIQUECOLLECTION_H__

#include <utils/Utils.h>
#include <utils/Enum.h>
#include <utils/Debug.h>

//! Prosta metoda, rzucaj�ca ostr� asercj�, gdy pr�bujemy doda� obiekt, kt�ry ju� istnieje w kolecji
//! \param vec kolecja, do kt�rej dodajemy obiekt
//! \param val dodawany obiekt
template<typename T>
void pushBackUnique( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it == vec.end());
    vec.push_back(val);
}

//! Prosta metoda, rzucaj�ca ostr� asercj�, gdy pr�bujemy usun�� obiekt, kt�rego nie ma w kolecji
//! \param vec kolecja, z kt�rej usuwamy obiekt
//! \param val usuwany obiekt
template<typename T>
void removeFromVector( std::vector<T>& vec, T& val )
{
    auto it = std::find(vec.begin(), vec.end(), val);
    UTILS_ASSERT(it != vec.end());
    vec.erase(it);
}



#endif
