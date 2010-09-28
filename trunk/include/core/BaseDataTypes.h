#ifndef BASE_DATA_TYPES_H
#define BASE_DATA_TYPES_H

#include <stdint.h>

typedef long ClassID; 
#define NULL 0

//! Identyfikator. Jest to pewien kompromis miêdzy zapewnieniem unikalnoœci
//! bez potrzeby definiowania sta³ych w jednym pliku a mo¿liwoœci¹
//! "rêcznego" odczytu (utrudnione w przypadku GUIDów).
struct UniqueId
{
  union {
    //! Dane w postaci tablicy bajtów.
    unsigned char data[8];
    //! Dane w postaci identyfikatorów g³ównych i pomocniczych.
    struct {
      int32_t minor;
      int32_t major;
    };
    //! Dane w postaci spakowanej.
    int64_t quad;
  };

  // Zdefiniowanie poni¿szej sta³ej spowoduje dodanie kolejnego pola, które
  // zawsze bêdzie wyzerowane. Jest to przydatne przy testowaniu, poniewa¿
  // ³atwiejszy staje siê podgl¹d pola data.
#ifdef CORE_UNIQUEID_ENABLE_TERMINATOR
  struct Terminator {
    char data;
    Terminator() : data(0) {}
  } __terminator;
#endif

  //! Najwygodniej podawaæ wersje w postaci czteroliterowych sta³ych,
  //! np. new UniqueId('TEST', 'STFU'). W tym wypadku pole data bêdzie
  //! w odwrotnej kolejnoœci!
  //! \param major G³ówny identyfikator, np. plugina.
  //! \param minor Mniej znacz¹cy identyfikator, np. us³ugi.
  inline UniqueId(int major, int minor) : minor(minor), major(major)
  {}
  inline UniqueId() : quad(0)
  {}
  inline UniqueId(const UniqueId& rhs) : quad(rhs.quad)
  {}

  inline UniqueId& operator=(const UniqueId& rhs)
  {
    quad = rhs.quad;
    return *this;
  }
  inline bool operator==(const UniqueId& rhs) const
  {
    return quad == rhs.quad;
  }
  inline bool operator!=(const UniqueId& rhs) const
  {
    return quad != rhs.quad;
  }
  inline bool operator<(const UniqueId& rhs) const
  {
    return quad < rhs.quad;
  }
};

template <int32_t Major, int32_t Minor>
struct UniqueIdSpec : public UniqueId
{
  UniqueIdSpec() : UniqueId(Major, Minor) {}
};

#endif //BASE_SERVICE_H