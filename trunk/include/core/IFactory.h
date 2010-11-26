#ifndef IFACTORY_H
#define IFACTORY_H

//#include <core/Singleton.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>


class IModel;
class IC3DModel;


class IFactor
{
public:
    virtual ~IFactor()
    { std::cout << "Usunieto Factor"  << std::endl; }

    virtual void Access()
    { std::cout << "Udostepiono Factor" << std::endl; }

    virtual IC3DModel* GetC3DModel(std::string c3dObjectPath) = 0;
    virtual IC3DModel* CreateC3DModel(std::string pathToC3DModel) = 0;

    virtual IModel* GetModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList) = 0; // modelPath = meshPath + asfPath + amcPath(0)
    virtual IModel* CreateModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList) = 0;
};


#endif //IFACTORY_H