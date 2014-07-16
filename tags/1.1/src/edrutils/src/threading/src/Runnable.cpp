#include <threading/Runnable.h>

using namespace utils;

FunctorRunnable::FunctorRunnable(const Functor & func)
	: func(func)
{

}

FunctorRunnable::~FunctorRunnable()
{

}

void FunctorRunnable::run()
{
	if(func.empty() == false){
		func();
	}
}