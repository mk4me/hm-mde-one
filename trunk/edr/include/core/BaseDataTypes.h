#ifndef BASE_DATA_TYPES_H
#define BASE_DATA_TYPES_H

#include <stdint.h>
#include <string>
#include <ostream>
#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

typedef boost::uuids::uuid UniqueID;

class UID 
{
public:
static UniqueID GenerateUniqueID(const std::string& uuid) 
{
    boost::uuids::string_generator gen;
    return gen(uuid);
}
};

////! Identyfikator. Jest to pewien kompromis mi�dzy zapewnieniem unikalno�ci
////! bez potrzeby definiowania sta�ych w jednym pliku a mo�liwo�ci�
////! "r�cznego" odczytu (utrudnione w przypadku GUID�w).
//struct UniqueID
//{
//    boost::uuids::uuid uuid;
//    std::string name;
//
//    inline UniqueID(boost::uuids::uuid id, std::string description) :
//        uuid(id), 
//        name(description)
//    {}
//
//    /*inline UniqueID() : quad(0)
//    {}*/
//
//    /*inline UniqueID(const UniqueID& rhs) : quad(rhs.quad)
//    {}*/
//
//    inline UniqueID& operator=(const UniqueID& rhs)
//    {
//        quad = rhs.quad;
//        return *this;
//    }
//    inline bool operator==(const UniqueID& rhs) const
//    {
//        return quad == rhs.quad;
//    }
//    inline bool operator!=(const UniqueID& rhs) const
//    {
//        return quad != rhs.quad;
//    }
//    inline bool operator<(const UniqueID& rhs) const
//    {
//        return quad < rhs.quad;
//    }
//};
//
//inline std::ostream& operator<<(std::ostream& stream, UniqueID id)
//{
//    std::string s(id.data, id.data+8);
//    std::reverse(s.begin(), s.end());
//    return (stream << s);
//}
//
//template <int32_t Major, int32_t Minor>
//struct UniqueIdSpec : public UniqueID
//{
//    UniqueIdSpec() : UniqueID(Major, Minor) {}
//};

//
//typedef long ClassID; 
//#define NULL 0
//
////! Identyfikator. Jest to pewien kompromis mi�dzy zapewnieniem unikalno�ci
////! bez potrzeby definiowania sta�ych w jednym pliku a mo�liwo�ci�
////! "r�cznego" odczytu (utrudnione w przypadku GUID�w).
//struct UniqueID
//{
//  union {
//    //! Dane w postaci tablicy bajt�w.
//    unsigned char data[8];
//    //! Dane w postaci identyfikator�w g��wnych i pomocniczych.
//    struct {
//      int32_t minor;
//      int32_t major;
//    };
//    //! Dane w postaci spakowanej.
//    int64_t quad;
//  };
//
//  // Zdefiniowanie poni�szej sta�ej spowoduje dodanie kolejnego pola, kt�re
//  // zawsze b�dzie wyzerowane. Jest to przydatne przy testowaniu, poniewa�
//  // �atwiejszy staje si� podgl�d pola data.
//#ifdef CORE_UNIQUEID_ENABLE_TERMINATOR
//  struct Terminator {
//    char data;
//    Terminator() : data(0) {}
//  } __terminator;
//#endif
//
//  //! Najwygodniej podawa� wersje w postaci czteroliterowych sta�ych,
//  //! np. new UniqueID('TEST', 'STFU'). W tym wypadku pole data b�dzie
//  //! w odwrotnej kolejno�ci!
//  //! \param major G��wny identyfikator, np. plugina.
//  //! \param minor Mniej znacz�cy identyfikator, np. us�ugi.
//  inline UniqueID(int major, int minor) : minor(minor), major(major)
//  {}
//  inline UniqueID() : quad(0)
//  {}
//  inline UniqueID(const UniqueID& rhs) : quad(rhs.quad)
//  {}
//
//  inline UniqueID& operator=(const UniqueID& rhs)
//  {
//    quad = rhs.quad;
//    return *this;
//  }
//  inline bool operator==(const UniqueID& rhs) const
//  {
//    return quad == rhs.quad;
//  }
//  inline bool operator!=(const UniqueID& rhs) const
//  {
//    return quad != rhs.quad;
//  }
//  inline bool operator<(const UniqueID& rhs) const
//  {
//    return quad < rhs.quad;
//  }
//};
//
//inline std::ostream& operator<<(std::ostream& stream, UniqueID id)
//{
//    std::string s(id.data, id.data+8);
//    std::reverse(s.begin(), s.end());
//    return (stream << s);
//}
//
//template <int32_t Major, int32_t Minor>
//struct UniqueIdSpec : public UniqueID
//{
//  UniqueIdSpec() : UniqueID(Major, Minor) {}
//};

#endif //BASE_SERVICE_H