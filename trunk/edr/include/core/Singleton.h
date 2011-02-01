#ifndef SINGLETON_H
#define SINGLETON_H

#include <iostream>

// Prostrza wersja rozbudowanego Singletona (pomini�to odwo�ywania poprzez adresy pami�ci)
// pozwalaj�ca na dziedziczenie - co daje mnustwo korzysci - mo�liwe jest ponadto stworzenie dla niego interfejsu
// co w naszym przyapdku - �adowania Servis�w z dllek jest bardzo przydatne - mog�c wykorzysta� ten singleton
// jako fabryki - Fabryka jest wyszczeg�lnieniem, uog�lnienia jakim jest klasa Singleton 

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