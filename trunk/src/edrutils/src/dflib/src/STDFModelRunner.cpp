#include <dflib/STDFModelRunner.h>
#include "STDFModelRunnerImpl.h"

namespace df {
	STDFModelRunner::STDFModelRunner() : privImpl(new STDFModelRunnerImpl())
	{
	}

	STDFModelRunner::~STDFModelRunner()
	{
		delete privImpl;
	}

	void STDFModelRunner::start(IModelReader * model, IDFLogger * logger)
	{
		privImpl->start(model, logger);
	}

	void STDFModelRunner::stop()
	{
		privImpl->stop();
	}

	const bool STDFModelRunner::paused() const
	{
		return privImpl->paused();
	}

	void STDFModelRunner::pause()
	{
		privImpl->pause();
	}

	void STDFModelRunner::resume()
	{
		privImpl->resume();
	}

	void STDFModelRunner::pause(INode * node)
	{
	}

	void STDFModelRunner::resume(INode * node)
	{
	}


	const bool STDFModelRunner::verifyModel(const IModelReader * reader)
	{
		return STDFModelRunnerImpl::verifyModel(reader);
	}
}
