#ifndef FACTORY_H
#define FACTORY_H

#include <core/Singleton.h>
#include <map>


class IModel;
class IC3DModel;

typedef std::map<std::string, IModel*> ModelResources;
typedef std::map<std::string, IC3DModel*> C3DModelResources;

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

    virtual IModel* CreateModel(std::string pathToMesh, std::string pathToASF, std::string pathToAMC);
    virtual IC3DModel* CreateC3DModel(std::string pathToC3DModel);

    virtual IModel* GetModel(std::string pathToMesh, std::string pathToASF, std::string pathToAMC); // modelPath = meshPath + asfPath + amcPath(0)
    virtual IC3DModel* GetC3DModel(std::string c3dObjectPath);

protected:

    ModelResources m_ModelMap;
    C3DModelResources m_C3DModelMap;
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