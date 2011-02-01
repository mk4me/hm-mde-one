#ifndef FACTORY_H
#define FACTORY_H

#include <core/IFactory.h>
#include <map>


class IModel;
class IC3DModel;

typedef std::map<std::string, IModel*> ModelResources;
typedef std::map<std::string, IC3DModel*> C3DModelResources;

/******************************************************************************************/

/*Metoda Factory dzia�a na zasadzie kontenera obiekt�w kt�re posiada nasza aplikacja      */
/*Chodzi oczywi�cie o obiekty wykorzystywane w celu wizualizacji danych z motion          */
/*capture czyli obiekt�w typu Model i C3DModel.                                           */
/*Dzia�anie klasy polega na tworzeniu b�d� udostepnianiu isnstancji tych modeli           */
/*dane s� zapisywane w mapie. kluczem jest �cie�ka do szkieletu i animacji - je�li        */
/*danego modelu nie ma w mapie zostanie utworzony nowy model wykorzystuj�c                */
/*�cie�ki z podanej lokalizacji                                                           */

/******************************************************************************************/
class Factor: public IFactor
{
public:
    Factor()
    { std::cout << "Utworzono Factor" << std::endl; }

    virtual ~Factor()
    { std::cout << "Usunieto Factor"  << std::endl; }

    virtual void Access()
    { std::cout << "Udostepiono Factor" << std::endl; }

    virtual IModel* CreateModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList);
    virtual IC3DModel* CreateC3DModel(std::string pathToC3DModel);

    virtual IModel* GetModel(std::string pathToMesh, std::string pathToASF, std::vector<std::string> pathToAMCList); // modelPath = meshPath + asfPath + amcPath(0)
    virtual IC3DModel* GetC3DModel(std::string c3dObjectPath);

    virtual void Clear();

private:

    ModelResources m_ModelMap;
    C3DModelResources m_C3DModelMap;
};



#endif //FACTORY_H