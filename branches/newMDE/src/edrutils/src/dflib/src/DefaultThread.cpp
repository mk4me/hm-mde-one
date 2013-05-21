#include "DefaultThread.h"

DefaultThread::DefaultThread(const utils::IThreadFactory::Functor & func) : func_(func)
{

}

void DefaultThread::start()
{
	this->QThread::start();
}

void DefaultThread::cancel()
{
	this->QThread::terminate();
}

void DefaultThread::join()
{
	this->QThread::wait();
}

void DefaultThread::run()
{
	if(func_.empty() == false){
		func_();
	}
}