#ifndef SCENE_SERVICE_H 
#define SCENE_SERVICE_H

#include <vector>

#include "Model.h"
#include "Animation.h"
#include "GlobalServicesIDs.h"


namespace MODEL_FLAG
{
    enum VIEW
    {
        NONE      = 0,     // 0b00000000
        MODEL 	  = 1,	   // 0b00000001	
        WIREFRAME = 1 << 1,// 0b00000010	
        BONE      = 1 << 2,// 0b00000100	
        MATERIAL  = 1 << 3,// 0b00001000	

        // do przeniesienia w inny enum
    };
}

// TODO
// tymczasowa klas - dalem to tu tylko by miec
// szybki dostep do zaladowanych modeli

//////////////////////////////////////////////////////////////////////////
// scene service
class ObjectService: public IBaseService
{
	M_DECLARE_CLASS(); 

protected:

	CModel* _model;

    int _viewModelFlag;
    osg::PrimitiveSet::Mode _primitiveModeFlag;

public:

    inline void setViewModelFlag(int flag);
    inline void addVieModelFlag(MODEL_FLAG::VIEW flag);
    inline void removeVieModelFlag(MODEL_FLAG::VIEW flag);

    inline void setPrimitiveModeFlag(osg::PrimitiveSet::Mode flag);
    inline int getViewModelFlag();
    inline int getPrimitiveModeFlag();


	// clear all
	void clearAll();

	// add model
	void addModel(CModel* model);

    void SetModel(CModel* model) {  addModel(model); };

	// get model 
	CModel* getModel()	{ return _model; }

	// update
	virtual AsyncResult OnTick(double delta); 

	// 
	virtual AsyncResult OnAdded(); 

	// c - tor
	ObjectService(void);

	// d - tor
	virtual ~ObjectService(void);
};

#include "ObjectService.inl"

#endif