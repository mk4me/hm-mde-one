#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>

// Prostrza wersja rozbudowanego Singletona (pominięto odwoływania poprzez adresy pamięci)
// pozwalająca na dziedziczenie - co daje mnustwo korzysci - możliwe jest ponadto stworzenie dla niego interfejsu
// co w naszym przyapdku - ładowania Servisów z dllek jest bardzo przydatne - mogąc wykorzystać ten singleton
// jako fabryki - Fabryka jest wyszczególnieniem, uogólnienia jakim jest klasa Singleton 

class Singleton
{
public:
    Singleton()
    { std::cout << "Utworzono Singleton" << std::endl; }

    virtual ~Singleton()
    { std::cout << "Usunieto Singleton"  << std::endl; }

    virtual void Access()
    { std::cout << "Udostepiono Singleton" << std::endl; }

    static Singleton* GetObj()
    { return m_pObj; }

    static void SetObj(Singleton* pObj)
    { m_pObj = pObj; }

protected:
    static Singleton* m_pObj;
};

Singleton* Singleton::m_pObj;

inline Singleton* Base()
{
    assert(Singleton::GetObj());
    return Singleton::GetObj();
}

#endif //SINGLETON_H
