#ifndef FACTORY_H
#define FACTORY_H

#include <iostream>

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



// Tworzenie klasy wywodz¹cej sie z Singleton
class Factor: public Singleton
{
public:
    Factor()
    { std::cout << "Utworzono Factor" << std::endl; }

    virtual ~Factor()
    { std::cout << "Usunieto Factor"  << std::endl; }

    virtual void Access()
    { std::cout << "Udostepiono Factor" << std::endl; }

protected:
};

inline Factor* Factory()
{
    assert(Factor::GetObj());
    return (Factor*)Factor::GetObj();
}

// Factor::SetObj(new Factor);

// odwo³anie Base->Access()
// albo Factory->Access()

#endif //FACTORY_H