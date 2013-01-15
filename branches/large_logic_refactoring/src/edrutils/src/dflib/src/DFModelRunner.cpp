#include <dflib/DFModelRunner.h>
#include "DFModelRunnerImpl.h"

namespace df {

	DFModelRunner::DFModelRunner() : privImpl(new DFModelRunnerImpl())
	{

	}

	DFModelRunner::~DFModelRunner()
	{
		delete privImpl;
	}

	void DFModelRunner::start(IModelReader * model, IDFLogger * logger, utils::IThreadFactory * tFactory)
	{
		privImpl->start(model, logger, tFactory);
	}

	void DFModelRunner::stop()
	{
		privImpl->stop();
	}

	const bool DFModelRunner::paused() const
	{
		return privImpl->paused();
	}

	void DFModelRunner::pause()
	{
		privImpl->pause();
	}

	void DFModelRunner::resume()
	{
		privImpl->resume();
	}

	void DFModelRunner::pause(INode * node)
	{

	}

	void DFModelRunner::resume(INode * node)
	{

	}

	void DFModelRunner::join()
	{
		privImpl->join();
	}

	const bool DFModelRunner::verifyModel(const IModelReader * reader)
	{
		return DFModelRunnerImpl::verifyModel(reader);
	}

}