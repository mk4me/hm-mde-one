#ifndef ANIMATION_SERVICE_H
#define ANIMATION_SERVICE_H

#include <map>
#include <vector>
#include <string>

#include "Model.h"
#include "Animation.h"
#include "GlobalServicesIDs.h"
#include "SimpleFunctors.h"

namespace osgViewer
{
    class Scene; 
}

// na razie obslugiwana jest zgodnie z zalozeniem tylko jedna
// animacja!

//////////////////////////////////////////////////////////////////////////
// animation service
class AnimationService: public IBaseService
{
		M_DECLARE_CLASS(); 

		ISimpleOneArgFunctor<double>*					_animation;

		// functions called every frame
		std::vector<ISimpleOneArgFunctor<double>*>		_functionsToCall;

		// functions to remove from caller
		std::vector<
			std::vector<ISimpleOneArgFunctor<double>*>::
				iterator>								_functionsToRemove;

		// functions called when animation is being unregistered
		std::vector<ISimpleNoArgFunctor*>				_functionsToCallWhenAnimationStopped;

		std::string										_selectedAnimatonName;

		// clear all
		void clearAll();
	protected: 
	
		// scene
		osgViewer::Scene* _scene; 

	public:

		// set act selected animation name
		void setSelectedAnimationName(std::string& name) { _selectedAnimatonName = name; } 

		// returns name of act selected animation
		std::string& getSelectedAnimationName() { return _selectedAnimatonName; }

		// add function to caller
		void registerAnimation	(CAnimation* object, void (CAnimation::*fun)(double));

		// remove function from caller
		bool unregisterAnimation();

		// adds function to caller - called on every animation update
		template<class T>
		void registerFunction(T* object, void (T::*fun)(double))
		{
			_functionsToCall.push_back(new CSimpleOneArgFunctor<T, double>(object, fun));
		}

		// remove function from caller
		template<class T>
		bool unregisterFunction(T* object, void (T::*fun)(double))
		{
			// TODO:
			// na razie zreb - zanim nie zaczne tego uzywac..
			CSimpleOneArgFunctor<T, double> f(object, fun);
			for (std::vector<ISimpleOneArgFunctor<double>*>::iterator i = _functionsToCall.begin(); i != _functionsToCall.end(); ++i)
			{
				CSimpleOneArgFunctor<T, double>* pf = dynamic_cast<CSimpleOneArgFunctor<T, double>*>(*i);

				if (pf && ((*pf) == f))
				{
					_functionsToRemove.push_back(i);
					return true;
				}
			}
			return false;
		}

		// adds function to caller - called on stop of animation
		template<class T>
		void registerOnStopFunction(T* object, void (T::*fun)())
		{
			_functionsToCallWhenAnimationStopped.push_back(new CSimpleNoArgFunctor<T>(object, fun));
		}

		// notify stop
		void notifyStop();

		// clear caller
		void clearCaller();

		// set animated scene
		void SetScene(osgViewer::Scene* scene); 

		// returns act animation
		CAnimation* getAnimation();

		// update
		virtual AsyncResult OnTick(double delta); 
		// 
		virtual AsyncResult OnAdded(); 

		// c - tor
		AnimationService(void);
		// d - tor
		virtual ~AnimationService(void);
};

#endif //ANIMATION_SERVICE_H