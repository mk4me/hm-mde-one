/********************************************************************
    created:  2012/12/10
    created:  10:12:2012   23:05
    filename: DFModelRunnerImpl.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DFMODELRUNNERIMPL_H__
#define HEADER_GUARD___DFMODELRUNNERIMPL_H__

#include <dflib/DFModelRunner.h>
#include <utils/SynchronizationPolicies.h>
#include <list>
#include <vector>
#include <map>
#include <utils/IThread.h>
#include <utils/IThreadFactory.h>
#include <dflib/MRModelInterfaceVerifier.h>
#include "DefaultThreadFactory.h"
#include <dflib/IDFLogger.h>
#include <QtCore/QThread>

namespace df{

	class IPin;
	class INode;

	class IInputPin;
	class IOutputPin;

	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;

	class IDFInput;
	class IDFOutput;

	class IDFSource;
	class IDFSink;
	class IDFProcessor;

	class IModelReader;
}

class MRInputPin;
class MROutputPin;


class IMRNode;
class IMRSourceNode;
class IMRSinkNode;
class IMRProcessingNode;

class df::DFModelRunner::DFModelRunnerImpl : public utils::SynchronizedT<true>
{
private:

	typedef utils::RecursiveSyncPolicy RecursiveSyncPolicy;
	typedef utils::ScopedLock<RecursiveSyncPolicy> RecursiveScopedLock;

	typedef utils::StrictSyncPolicy StrictSyncPolicy;
	typedef utils::ScopedLock<StrictSyncPolicy> StrictScopedLock;


private:

	class INodeRunner
	{
	public:

		INodeRunner(DFModelRunnerImpl * runner, df::IDFLogger * logger);

		void run();

	private:
		virtual const bool dataflow() = 0;

	protected:

		df::IDFLogger * logger_;
		DFModelRunnerImpl * runner_;

	};

	friend class INodeRunner;

	class SinkNodeRunner : public INodeRunner
	{
	public:
		SinkNodeRunner(IMRSinkNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);

	private:
		virtual const bool dataflow();

	private:
		IMRSinkNode * node_;
	};

	friend class SinkNodeRunner;

	class SourceNodeRunner : public INodeRunner
	{
	public:
		SourceNodeRunner(IMRSourceNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);

	private:
		virtual const bool dataflow();

	private:
		IMRSourceNode * node_;
	};

	friend class SourceNodeRunner;

	class ProcessingNodeRunner : public INodeRunner
	{
	public:
		ProcessingNodeRunner(IMRProcessingNode * node, DFModelRunnerImpl * runner, df::IDFLogger * logger);

	private:
		virtual const bool dataflow();

	private:
		IMRProcessingNode * node_;		
	};

	friend class ProcessingNodeRunner;

	class DataflowFinisher : public QThread
	{
	public:
		DataflowFinisher();

		void setThreadFactory(utils::IThreadFactory * factory);
		void setRunner(DFModelRunnerImpl * runner);

		virtual ~DataflowFinisher();

	protected:

		virtual void run();

	private:

		DFModelRunnerImpl * runner;
		utils::IThreadFactory * factory;
	};

	friend class DataflowFinisher;

struct SourceNodeWrapData
{
	IMRSourceNode * node;
	df::IDFSource * dfSource;
};

typedef std::vector<SourceNodeWrapData> WrapedSources;
typedef std::vector<IMRSinkNode *> WrapedSinks;
typedef std::vector<IMRProcessingNode*> WrapedProcessors;

typedef std::vector<utils::IThread*> Threads;
typedef std::list<df::IDFLoggerHelper*> LoggerHelpers;
typedef std::vector<INodeRunner*> NodeRunners;

typedef std::map<df::INode*, IMRNode *> NodesMapping;

typedef std::map<df::IInputPin*, MRInputPin*> InputPinsMapping;
typedef std::map<df::IOutputPin*, MROutputPin*> OutputPinsMapping;

private:

	void markFailure(const std::string & message);

	void cleanUpDataflow();

	void wrapSourceNode(const MRModelInterfaceVerifier::SourceVerificationData & sourceData, utils::IThreadFactory * tFactory, OutputPinsMapping & outputMapping);
	void wrapSinkNode(const MRModelInterfaceVerifier::SinkVerificationData & sinkData, utils::IThreadFactory * tFactory, InputPinsMapping & inputMapping);
	void wrapProcessorNode(const MRModelInterfaceVerifier::ProcessorVerificationData & processorData, utils::IThreadFactory * tFactory, InputPinsMapping & inputMapping, OutputPinsMapping & outputMapping);
	
	void wrapInputPins(IMRSinkNode * sink, const MRModelInterfaceVerifier::InputVerification & inputData, InputPinsMapping & inputMapping);
	void wrapOutputPins(IMRSourceNode * source, const MRModelInterfaceVerifier::OutputVerification & outputData, OutputPinsMapping & outputMapping);

	void reset();

	void resetDataflowStatus();

	void resetDataflowElements();
	void startDataflow();

	void increaseDataToProcess();
	void decreaseDataToProcess();

	//! \return Czy Ÿród³a maj¹ jeszcze dane
	const bool sourcesHaveMore() const;

	//! Meldujemy zakoñczenie pracy elementu nie bêd¹cego Ÿród³em
	void waitForAllSources();
	void sourceFinished();
	void nonSourceFinished();

	void finishDataflow();
	void stopDataflow();

	//! Koñczymy przep³yw danych - czyœcimy wszystko
	const bool dataflowFinished() const;


	//! \param reader Obiekt pozwalajacy czytaæ strukturê naszego modelu
	//! \return Czy model mo¿na przetwarzaæ
	static const bool verifyModel(df::IModelReader * reader, MRModelInterfaceVerifier::ModelVerificationData & interfaceVerifier);

	void tryPause();

	//! Metoda mapuje elementy modelu do obiektów wrappuj¹cych, które obs³uguj¹ logikê przep³ywu danych
	//! \param reader Obiekt pozwalajacy czytaæ strukturê naszego modelu
	void wrapModelElements(df::IModelReader * model, const MRModelInterfaceVerifier::ModelVerificationData & modelElements, utils::IThreadFactory * tFactory);

private:
	StrictSyncPolicy failureMessageSync;

	StrictSyncPolicy sourcesSync;
	StrictSyncPolicy sourcesWait;
	StrictSyncPolicy dataflowPauseSync;
	StrictSyncPolicy nonSourcesSync;
	StrictSyncPolicy dataToProcessSync;

	WrapedSources sources_;
	WrapedSinks sinks_;
	WrapedProcessors processors_;
	Threads threads_;
	LoggerHelpers loggerHelpers_;
	NodeRunners nodeRunners_;

	NodesMapping nodesMapping;
	NodesMapping pausedNodes;

	unsigned int finishedRunners;
	unsigned int dataToProcess;
	unsigned int nonSourceElements;
	unsigned int sourcesFinished_;
	unsigned int nonSourcesFinished_;

	volatile bool sourcesEmpty_;
	bool dataflowFinished_;

	bool failure_;
	std::list<std::string> failureMessages_;

	bool paused_;

	df::IDFLogger * logger_;

	DataflowFinisher dataflowFinisher;

	static DefaultThreadFactory defaultThreadFactory;

public:

	DFModelRunnerImpl();

	//! \param reader Obiekt pozwalajacy czytaæ strukturê naszego modelu
	//! \return Czy model mo¿na przetwarzaæ
	static const bool verifyModel(const df::IModelReader * reader);

	void start(df::IModelReader * model, df::IDFLogger * logger, utils::IThreadFactory * tFactory);
	void stop();

	void pause(df::INode * node);
	void resume(df::INode * node);
	const bool paused(df::INode * node);

	void pause();
	void resume();
	const bool paused() const;

	void join();
};

#endif	//	HEADER_GUARD___DFMODELRUNNERIMPL_H__
