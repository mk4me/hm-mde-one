#ifndef BASE_DATA_TYPES_H
#define BASE_DATA_TYPES_H

#include <stdint.h>

typedef long ClassID; 
#define NULL 0

//! Identyfikator. Jest to pewien kompromis mi�dzy zapewnieniem unikalno�ci
//! bez potrzeby definiowania sta�ych w jednym pliku a mo�liwo�ci�
//! "r�cznego" odczytu (utrudnione w przypadku GUID�w).
struct UniqueId
{
  union {
    //! Dane w postaci tablicy bajt�w.
    unsigned char data[8];
    //! Dane w postaci identyfikator�w g��wnych i pomocniczych.
    struct {
      int32_t minor;
      int32_t major;
    };
    //! Dane w postaci spakowanej.
    int64_t quad;
  };

  // Zdefiniowanie poni�szej sta�ej spowoduje dodanie kolejnego pola, kt�re
  // zawsze b�dzie wyzerowane. Jest to przydatne przy testowaniu, poniewa�
  // �atwiejszy staje si� podgl�d pola data.
#ifdef CORE_UNIQUEID_ENABLE_TERMINATOR
  struct Terminator {
    char data;
    Terminator() : data(0) {}
  } __terminator;
#endif

  //! Najwygodniej podawa� wersje w postaci czteroliterowych sta�ych,
  //! np. new UniqueId('TEST', 'STFU'). W tym wypadku pole data b�dzie
  //! w odwrotnej kolejno�ci!
  //! \param major G��wny identyfikator, np. plugina.
  //! \param minor Mniej znacz�cy identyfikator, np. us�ugi.
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