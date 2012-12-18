#include "DFModelRunnerImpl.h"
#include <dflib/LoggerHelpers.h>
#include "MRNode.h"
#include "MRPin.h"
#include "MRConnection.h"
#include <dflib/IModel.h>
#include <dflib/IDFPin.h>
#include <dflib/IDFNode.h>
#include <dflib/MRModelLogicVerifier.h>
#include <dflib/MRModelPinRequirementsVerifier.h>
#include <dflib/DFModelRunner.h>
#include <boost/bind.hpp>

DefaultThreadFactory df::DFModelRunner::DFModelRunnerImpl::defaultThreadFactory = DefaultThreadFactory();

df::DFModelRunner::DFModelRunnerImpl::DFModelRunnerImpl()
{
	dataflowFinisher.setRunner(this);
}

df::DFModelRunner::DFModelRunnerImpl::INodeRunner::INodeRunner(DFModelRunnerImpl * runner, df::IDFLogger * logger) : runner_(runner), logger_(logger)
{

}

void df::DFModelRunner::DFModelRunnerImpl::INodeRunner::run()
{
	bool next = true;
	bool error = false;
	std::string errorMessage;

	while(error == false && next == true){
		try{
			next = dataflow();
		}catch(std::exception & e){
			error = true;
			errorMessage = std::string(e.what());
		}catch(...){
			error = true;
			errorMessage = "Unknown node runner error";
		}
	}

	if(error == true){
		runner_->markFailure(errorMessage);
	}
}

df::DFModelRunner::DFModelRunnerImpl::SourceNodeRunner::SourceNodeRunner(IMRSourceNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{

}

const bool df::DFModelRunner::DFModelRunnerImpl::SourceNodeRunner::dataflow()
{
	node_->lockSrcProcessing();

	if(runner_->dataflowFinished() == true)
	{
		return false;
	}

	node_->tryPause();

	runner_->tryPause();

	node_->process();

	runner_->sourceFinished();

	runner_->waitForAllSources();

	return true;
}

df::DFModelRunner::DFModelRunnerImpl::SinkNodeRunner::SinkNodeRunner(IMRSinkNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{
	node->lockSnkProcessing();
}

const bool df::DFModelRunner::DFModelRunnerImpl::SinkNodeRunner::dataflow()
{
	node_->lockSnkProcessing();

	if(runner_->dataflowFinished() == true)
	{
		return false;
	}

	node_->tryPause();

	runner_->tryPause();

	node_->process();

	runner_->nonSourceFinished();

	return true;
}

df::DFModelRunner::DFModelRunnerImpl::ProcessingNodeRunner::ProcessingNodeRunner(IMRProcessingNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger) : INodeRunner(runner, logger), node_(node)
{
	node->lockSnkProcessing();
}

const bool df::DFModelRunner::DFModelRunnerImpl::ProcessingNodeRunner::dataflow()
{
	node_->lockSnkProcessing();
	node_->lockSrcProcessing();

	if(runner_->dataflowFinished() == true)
	{
		return false;
	}

	node_->tryPause();

	runner_->tryPause();

	node_->process();

	runner_->nonSourceFinished();

	return true;
}

df::DFModelRunner::DFModelRunnerImpl::DataflowFinisher::DataflowFinisher() : runner(nullptr), factory(nullptr)
{

}

void df::DFModelRunner::DFModelRunnerImpl::DataflowFinisher::setRunner(DFModelRunnerImpl * runner)
{
	this->runner = runner;
}


void df::DFModelRunner::DFModelRunnerImpl::DataflowFinisher::setThreadFactory(utils::IThreadFactory * factory)
{
	this->factory = factory;
}

df::DFModelRunner::DFModelRunnerImpl::DataflowFinisher::~DataflowFinisher()
{

}

void df::DFModelRunner::DFModelRunnerImpl::DataflowFinisher::run()
{
	runner->finishDataflow();
}

//! \return Czy Ÿród³a maj¹ jeszcze dane
const bool df::DFModelRunner::DFModelRunnerImpl::sourcesHaveMore() const
{
	bool ret = true;

	for(auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		if((*it).dfSource->empty() == true)
		{
			ret = false;
			break;
		}
	}

	return ret;
}

void df::DFModelRunner::DFModelRunnerImpl::increaseDataToProcess()
{
	StrictScopedLock lock(dataToProcessSync);
	++dataToProcess;
}

void df::DFModelRunner::DFModelRunnerImpl::decreaseDataToProcess()
{
	StrictScopedLock lock(dataToProcessSync);
	--dataToProcess;
}

//! Meldujemy zakoñczenie pracy elementu nie bêd¹cego Ÿród³em
void df::DFModelRunner::DFModelRunnerImpl::waitForAllSources()
{
	StrictScopedLock lock(sourcesWait);
}

void df::DFModelRunner::DFModelRunnerImpl::sourceFinished()
{
	StrictScopedLock lock(sourcesSync);

	if(sourcesFinished_ == 0){
		increaseDataToProcess();
		sourcesWait.lock();
	}

	++sourcesFinished_;

	if(sourcesFinished_ == sources_.size())
	{
		if(sourcesHaveMore() == true){
			sourcesFinished_ = 0;
			sourcesWait.unlock();
		}else{
			//Ÿród³a skoñczy³y przetwarzaæ - czekamy na innych ¿eby zakoñczyæ dataflow
			sourcesEmpty_ = true;
			if(dataToProcess == 0){
				stopDataflow();
			}
		}
	}
}

void df::DFModelRunner::DFModelRunnerImpl::nonSourceFinished()
{
	StrictScopedLock lock(nonSourcesSync);
	++nonSourcesFinished_;

	if(nonSourcesFinished_ == nonSourceElements)
	{
		nonSourcesFinished_ = 0;
		decreaseDataToProcess();

		if(sourcesEmpty_ == true && dataToProcess == 0){
			stopDataflow();
		}		
	}
}

void df::DFModelRunner::DFModelRunnerImpl::stopDataflow()
{
	dataflowFinished_ = true;

	//unlock all nodes to kill corresponding threads (runners)!!
	//when all are dead dataflow will finish seamlessly processing by cleaning up and reseting

	if(paused_ == true){
		paused_ = false;
		dataflowPauseSync.unlock();
	}

	for(auto it = pausedNodes.begin(); it != pausedNodes.end(); ++it)
	{
		it->second->resume();
	}

	for(auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		(*it).node->unlockSrcProcessing();
	}

	sourcesWait.unlock();

	for(auto it = sinks_.begin(); it != sinks_.end(); ++it)
	{
		(*it)->unlockSnkProcessing();
	}

	for(auto it = processors_.begin(); it != processors_.end(); ++it)
	{
		(*it)->unlockSnkProcessing();
	}
}

void df::DFModelRunner::DFModelRunnerImpl::finishDataflow()
{
	try{
		for(auto it = threads_.begin(); it != threads_.end(); ++it)
		{
			(*it)->join();
		}

		cleanUpDataflow();

	}catch(std::exception & e){
		markFailure(e.what());
	}catch(...){
		markFailure("Unknown error while finishing dataflow");
	}
}

const bool df::DFModelRunner::DFModelRunnerImpl::dataflowFinished() const
{
	return dataflowFinished_;
}

const bool df::DFModelRunner::DFModelRunnerImpl::verifyModel(df::IModelReader * reader, MRModelInterfaceVerifier::ModelVerificationData & interfaceVerifier)
{
	MRModelLogicVerifier::ModelVerificationData logicVerify;
	MRModelPinRequirementsVerifier::UnsatisfiedPins pinsVerify;

	bool logicOK = MRModelLogicVerifier::verifyModelLogic(reader, logicVerify);
	bool pinsOK = MRModelPinRequirementsVerifier::verifyModelPinRequirements(reader, pinsVerify);
	bool interfaceOK = MRModelInterfaceVerifier::verifyModelInterface(reader, interfaceVerifier);

	return (logicOK && pinsOK && interfaceOK);
}

const bool df::DFModelRunner::DFModelRunnerImpl::verifyModel(const df::IModelReader * reader)
{
	MRModelInterfaceVerifier::ModelVerificationData interfaceVerifier;
	return verifyModel(const_cast<df::IModelReader*>(reader), interfaceVerifier);
}

void df::DFModelRunner::DFModelRunnerImpl::tryPause()
{
	StrictScopedLock lock(dataflowPauseSync);
}

void df::DFModelRunner::DFModelRunnerImpl::start(df::IModelReader * model, df::IDFLogger * logger, utils::IThreadFactory * tFactory)
{
	MRModelInterfaceVerifier::ModelVerificationData interfaceVerification;
	if(verifyModel(model, interfaceVerification) == false){
		std::string message("Model verification failed:");

		if(interfaceVerification.incompatibleNodes.empty() == false){
			message += "Some nodes do not implement required functionality - IDFSource, IDFSink or IDFProcessor";
		}

		if(interfaceVerification.incompatiblePins.empty() == false){
			message += "Some pins do not implement required functionality - IDFInput or IDFOutput";
		}

		throw df::ModelVerificationException(message.c_str());
	}

	logger_ = logger;

	if(tFactory == nullptr){
		tFactory = &defaultThreadFactory;
	}
	
	dataflowFinisher.setThreadFactory(tFactory);

	try{
		resetDataflowStatus();
		wrapModelElements(model, interfaceVerification, tFactory);
		resetDataflowElements();
	}catch(std::exception & e){
		cleanUpDataflow();
		throw ModelRunInitializationException(e.what());
	}catch(...){
		cleanUpDataflow();
		throw ModelRunInitializationException("Unknown error while initializing dataflow");
	}

	try{
		startDataflow();
	}catch(std::exception & e){
		throw ModelStartException(e.what());
	}catch(...){
		throw ModelStartException("Unknown error while starting dataflow");
	}
}

void df::DFModelRunner::DFModelRunnerImpl::stop()
{

}

void df::DFModelRunner::DFModelRunnerImpl::pause()
{
	paused_ = true;
	dataflowPauseSync.lock();
}

void df::DFModelRunner::DFModelRunnerImpl::resume()
{
	paused_ = false;
	dataflowPauseSync.unlock();
}

const bool df::DFModelRunner::DFModelRunnerImpl::paused() const
{
	return paused_;
}

void df::DFModelRunner::DFModelRunnerImpl::join()
{
	dataflowFinisher.wait();
	if(failure_ == true){
		std::string message = "Error(s) while model running: ";

		for(auto it = failureMessages_.begin(); it != failureMessages_.end(); ++it){
			message += *it + " | ";
		}

		throw ModelRunningException(message.c_str());
	}
}

void df::DFModelRunner::DFModelRunnerImpl::wrapModelElements(df::IModelReader * model, const MRModelInterfaceVerifier::ModelVerificationData & modelElements, utils::IThreadFactory * tFactory)
{
	InputPinsMapping inputMapping;
	OutputPinsMapping outputMapping;

	for(auto it = modelElements.sourceVerification.begin(); it != modelElements.sourceVerification.end(); ++it)
	{
		wrapSourceNode(*it, tFactory, outputMapping);
	}

	for(auto it = modelElements.sinkVerification.begin(); it != modelElements.sinkVerification.end(); ++it)
	{
		wrapSinkNode(*it, tFactory, inputMapping);
	}

	for(auto it = modelElements.processorVerification.begin(); it != modelElements.processorVerification.end(); ++it)
	{
		wrapProcessorNode(*it, tFactory, inputMapping, outputMapping);
	}

	for(unsigned int i = 0; i < model->connectionsSize(); ++i)
	{
		auto c = model->connection(i);
		auto src = outputMapping[c->source()];
		auto dest = inputMapping[c->destination()];
		auto mrconnection = new MRConnection(src, dest, c);

		src->addConnection(mrconnection);
		dest->addConnection(mrconnection);

	}

	nonSourceElements = sinks_.size() + processors_.size();	
}


void df::DFModelRunner::DFModelRunnerImpl::wrapSourceNode(const MRModelInterfaceVerifier::SourceVerificationData & sourceData, utils::IThreadFactory * tFactory,
	OutputPinsMapping & outputMapping)
{
	SourceNodeWrapData srcWrapData;
	srcWrapData.node = new MRSourceNode(sourceData.node, sourceData.source);
	srcWrapData.dfSource = sourceData.source;
	wrapOutputPins(srcWrapData.node, sourceData.outputVerication, outputMapping);

	auto runner = new SourceNodeRunner(srcWrapData.node, this, logger_);
	nodeRunners_.push_back(runner);
	
	threads_.push_back(tFactory->createThread(boost::bind(&INodeRunner::run,runner)));

	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(sourceData.node);

	if(loggable != nullptr){
		auto loggerHelper = new df::SourceLoggerHelper(sourceData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	sources_.push_back(srcWrapData);
}

void df::DFModelRunner::DFModelRunnerImpl::wrapSinkNode(const MRModelInterfaceVerifier::SinkVerificationData & sinkData, utils::IThreadFactory * tFactory,
	InputPinsMapping & inputMapping)
{
	auto node = new MRSinkNode(sinkData.node, sinkData.sink);
	wrapInputPins(node, sinkData.inputVerication, inputMapping);
	
	auto runner = new SinkNodeRunner(node, this, logger_);
	nodeRunners_.push_back(runner);

	threads_.push_back(tFactory->createThread(boost::bind(&INodeRunner::run, runner)));

	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(sinkData.node);

	if(loggable != nullptr){
		auto loggerHelper = new df::SinkLoggerHelper(sinkData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	sinks_.push_back(node);
}

void df::DFModelRunner::DFModelRunnerImpl::wrapProcessorNode(const MRModelInterfaceVerifier::ProcessorVerificationData & processorData, utils::IThreadFactory * tFactory,
	InputPinsMapping & inputMapping, OutputPinsMapping & outputMapping)
{
	auto node = new MRProcessingNode(processorData.node, processorData.processor);
	wrapInputPins(node, processorData.inputVerication, inputMapping);
	wrapOutputPins(node, processorData.outputVerication, outputMapping);

	INodeRunner * runner = nullptr;

	if(processorData.node->outputsConnected() == 0){
		runner = new SinkNodeRunner(node, this, logger_);
	}else{
		runner = new ProcessingNodeRunner(node, this, logger_);
	}

	nodeRunners_.push_back(runner);
	threads_.push_back(tFactory->createThread(boost::bind(&INodeRunner::run, runner)));



	df::IDFLoggable * loggable = dynamic_cast<df::IDFLoggable*>(processorData.node);

	if(loggable != nullptr){
		auto loggerHelper = new df::ProcessingLoggerHelper(processorData.node, logger_);
		loggable->initLogger(loggerHelper);
		loggerHelpers_.push_back(loggerHelper);
	}

	processors_.push_back(node);
}

void df::DFModelRunner::DFModelRunnerImpl::wrapInputPins(IMRSinkNode * sink, const MRModelInterfaceVerifier::InputVerification & inputData, InputPinsMapping & inputMapping)
{
	for(auto it = inputData.begin(); it != inputData.end(); ++it)
	{
		auto p = new MRInputPin(sink, (*it).first, (*it).second);
		sink->addInputPin(p);
		inputMapping[(*it).first] = p;
	}
}

void df::DFModelRunner::DFModelRunnerImpl::wrapOutputPins(IMRSourceNode * source, const MRModelInterfaceVerifier::OutputVerification & outputData, OutputPinsMapping & inputMapping)
{
	for(auto it = outputData.begin(); it != outputData.end(); ++it)
	{
		auto p = new MROutputPin(source, (*it).first, (*it).second);
		source->addOutputPin(p);
		inputMapping[(*it).first] = p;
	}
}

void df::DFModelRunner::DFModelRunnerImpl::resetDataflowStatus()
{
	finishedRunners = 0;
	dataToProcess = 0;
	nonSourceElements = 0;
	sourcesFinished_ = 0;
	nonSourcesFinished_ = 0;
	sourcesEmpty_ = false;
	failure_ = false;
	dataflowFinished_ = false;
	paused_ = false;
}

void df::DFModelRunner::DFModelRunnerImpl::resetDataflowElements()
{
	for(auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		(*it).node->reset();
	}

	for(auto it = sinks_.begin(); it != sinks_.end(); ++it)
	{
		(*it)->reset();
	}

	for(auto it = processors_.begin(); it != processors_.end(); ++it)
	{
		(*it)->reset();
	}
}

void df::DFModelRunner::DFModelRunnerImpl::startDataflow()
{
	for(auto it = threads_.begin(); it != threads_.end(); ++it)
	{
		(*it)->start();
	}

	dataflowFinisher.start();
}

void df::DFModelRunner::DFModelRunnerImpl::cleanUpDataflow()
{
	for(auto it = sources_.begin(); it != sources_.end(); ++it)
	{
		delete (*it).node;
	}

	for(auto it = sinks_.begin(); it != sinks_.end(); ++it)
	{
		delete *it;
	}

	for(auto it = processors_.begin(); it != processors_.end(); ++it)
	{
		delete *it;
	}

	for(auto it = loggerHelpers_.begin(); it != loggerHelpers_.end(); ++it)
	{
		delete *it;
	}

	for(auto it = threads_.begin(); it != threads_.end(); ++it)
	{
		delete *it;
	}

	NodesMapping().swap(nodesMapping);
	NodesMapping().swap(pausedNodes);

	WrapedSources().swap(sources_);
	WrapedSinks().swap(sinks_);
	WrapedProcessors().swap(processors_);
	LoggerHelpers().swap(loggerHelpers_);
	Threads().swap(threads_);

	resetDataflowStatus();

	sourcesEmpty_ = false;
	dataflowFinished_ = false;	

	logger_ = nullptr;
}

void df::DFModelRunner::DFModelRunnerImpl::markFailure(const std::string & message)
{
	StrictScopedLock lock(failureMessageSync);
	failureMessages_.push_back(message);
	if(failure_ == false)
	{
		failure_ = true;
		stopDataflow();
	}
}

void df::DFModelRunner::DFModelRunnerImpl::pause(df::INode * node)
{
	auto it = pausedNodes.find(node);
	it->second->pause();
}

void df::DFModelRunner::DFModelRunnerImpl::resume(df::INode * node)
{
	auto it = pausedNodes.find(node);
	it->second->resume();
}

const bool df::DFModelRunner::DFModelRunnerImpl::paused(df::INode * node)
{
	return pausedNodes.find(node) != pausedNodes.end();
}