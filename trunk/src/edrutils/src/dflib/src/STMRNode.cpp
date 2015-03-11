#include "STMRNode.h"
#include "STMRNodeImpl.h"

STMRSinkNode::STMRSinkNode(df::ISinkNode * node, df::IDFSink * sink) : node_(node), sink_(sink)
{
	snkImpl = new STMRSinkNodeImpl(this, node->inputsConnected());
}

STMRSinkNode::~STMRSinkNode()
{
	delete snkImpl;
}

void STMRSinkNode::process()
{
	snkImpl->consumeInputs();
	sink_->consume();
}

void STMRSinkNode::pause()
{
	nodeImpl.pause();
}

void STMRSinkNode::resume()
{
	nodeImpl.resume();
}

void STMRSinkNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool STMRSinkNode::paused() const
{
	return nodeImpl.paused();
}

void STMRSinkNode::updateSnk()
{
	snkImpl->updateSnk();
}

void STMRSinkNode::reset()
{
	sink_->reset();
	snkImpl->resetInputs();
}

void STMRSinkNode::lockSnkProcessing()
{
	snkImpl->wait();
}

void STMRSinkNode::unlockSnkProcessing()
{
	snkImpl->wakeUp();
}

void STMRSinkNode::addInputPin(STMRInputPin * pin)
{
	snkImpl->addPin(pin);
}

STMRInputPin * STMRSinkNode::inputPin(size_type idx)
{
	return snkImpl->pin(idx);
}

const STMRInputPin * STMRSinkNode::inputPin(size_type idx) const
{
	return snkImpl->pin(idx);
}

const STMRSinkNode::size_type STMRSinkNode::inputSize() const
{
	return snkImpl->size();
}

const bool STMRSinkNode::inputEmpty() const
{
	return snkImpl->empty();
}

STMRSourceNode::STMRSourceNode(df::ISourceNode * node, df::IDFSource * source) : node_(node), source_(source)
{
	srcImpl = new STMRSourceNodeImpl(this, node->outputsConnected());
}

STMRSourceNode::~STMRSourceNode()
{
	delete srcImpl;
}

void STMRSourceNode::process()
{
	source_->produce();
	srcImpl->updateOutputs();
}

void STMRSourceNode::pause()
{
	nodeImpl.pause();
}

void STMRSourceNode::resume()
{
	nodeImpl.resume();
}

void STMRSourceNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool STMRSourceNode::paused() const
{
	return nodeImpl.paused();
}

void STMRSourceNode::updateSrc()
{
	srcImpl->updateSrc();
}

void STMRSourceNode::reset()
{
	source_->reset();
	srcImpl->resetOutputs();
}

void STMRSourceNode::lockSrcProcessing()
{
	srcImpl->wait();
}

void STMRSourceNode::unlockSrcProcessing()
{
	srcImpl->wakeUp();
}

void STMRSourceNode::addOutputPin(STMROutputPin * pin)
{
	srcImpl->addPin(pin);
}

STMROutputPin * STMRSourceNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const STMROutputPin * STMRSourceNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const STMRSourceNode::size_type STMRSourceNode::outputSize() const
{
	return srcImpl->size();
}

const bool STMRSourceNode::outputEmpty() const
{
	return srcImpl->size();
}

STMRProcessingNode::STMRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor) : node_(node), processor_(processor)
{
	snkImpl = new STMRSinkNodeImpl(this, node->inputsConnected());
	srcImpl = new STMRSourceNodeImpl(this, node->outputsConnected());
}

STMRProcessingNode::~STMRProcessingNode()
{
	delete snkImpl;
	delete srcImpl;
}

void STMRProcessingNode::process()
{
	snkImpl->consumeInputs();
	processor_->process();
	srcImpl->updateOutputs();
}

void STMRProcessingNode::tryPause()
{
	nodeImpl.tryPause();
}

void STMRProcessingNode::pause()
{
	nodeImpl.pause();
}

void STMRProcessingNode::resume()
{
	nodeImpl.resume();
}

const bool STMRProcessingNode::paused() const
{
	return nodeImpl.paused();
}

void STMRProcessingNode::updateSnk()
{
	snkImpl->updateSnk();
}

void STMRProcessingNode::updateSrc()
{
	srcImpl->updateSrc();
}

void STMRProcessingNode::reset()
{
	processor_->reset();
	snkImpl->resetInputs();
	srcImpl->resetOutputs();
}

void STMRProcessingNode::lockSnkProcessing()
{
	snkImpl->wait();
}

void STMRProcessingNode::unlockSnkProcessing()
{
	snkImpl->wakeUp();
}

void STMRProcessingNode::addInputPin(STMRInputPin * pin)
{
	snkImpl->addPin(pin);
}

STMRInputPin * STMRProcessingNode::inputPin(size_type idx)
{	
	return snkImpl->pin(idx);
}

const STMRInputPin * STMRProcessingNode::inputPin(size_type idx) const
{	
	return snkImpl->pin(idx);
}

const STMRProcessingNode::size_type STMRProcessingNode::inputSize() const
{
	return snkImpl->size();
}

const bool STMRProcessingNode::inputEmpty() const
{
	return snkImpl->empty();
}

void STMRProcessingNode::lockSrcProcessing()
{
	srcImpl->wait();
}

void STMRProcessingNode::unlockSrcProcessing()
{
	srcImpl->wakeUp();
}

void STMRProcessingNode::addOutputPin(STMROutputPin * pin)
{
	srcImpl->addPin(pin);
}

STMROutputPin * STMRProcessingNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const STMROutputPin * STMRProcessingNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const STMRProcessingNode::size_type STMRProcessingNode::outputSize() const
{
	return srcImpl->size();
}

const bool STMRProcessingNode::outputEmpty() const
{
	return srcImpl->empty();
}
