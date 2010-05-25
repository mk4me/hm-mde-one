#ifndef SCENE_SERVICE_H 
#define SCENE_SERVICE_H

#include <vector>

#include "Model.h"
#include "Animation.h"
#include "GlobalServicesIDs.h"

// TODO
// tymczasowa klas - dalem to tu tylko by miec
// szybki dostep do zaladowanych modeli

//--------------------------------------------------------------------------------------------------
// Scene service
class ObjectService: public IBaseService
{
public: 
    // TODO: W zasadzie ten enum tu nie pasuje. Przenisc go np do Model.h (?)
    enum ViewMode
    {
        NONE      = 0,     // 0b00000000
        MODEL 	  = 1,	   // 0b00000001	
        WIREFRAME = 1 << 1,// 0b00000010	
        BONE      = 1 << 2,// 0b00000100	
        MATERIAL  = 1 << 3,// 0b00001000	
    };

public:
    ObjectService(void);
    virtual ~ObjectService(void);
    void Clear();

    void SetViewModelFlag(int flag);
    void AddVieModelFlag(ObjectService::ViewMode flag);
    void RemoveVieModelFlag(ObjectService::ViewMode flag);
    void SetPrimitiveModeFlag(osg::PrimitiveSet::Mode flag);
    int GetViewModelFlag();
    int GetPrimitiveModeFlag();

    void AddModel(Model* model);
    void SetModel(Model* model); 
    // get model 
    Model* GetModel(); 
    // update
    virtual AsyncResult OnTick(double delta); 
    // 
    virtual AsyncResult OnAdded(); 

protected:
	Model* _model;
    int _viewModelFlag;
    osg::PrimitiveSet::Mode _primitiveModeFlag;

private: 
    M_DECLARE_CLASS(); 
	
};

#endif