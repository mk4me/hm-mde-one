#include "MRNode.h"
#include "MRNodeImpl.h"

MRSinkNode::MRSinkNode(df::ISinkNode * node, df::IDFSink * sink) : node_(node), sink_(sink)
{
	snkImpl = new MRSinkNodeImpl(this, node->inputsConnected());
}

MRSinkNode::~MRSinkNode()
{
	delete snkImpl;
}

void MRSinkNode::process()
{
	snkImpl->consumeInputs();
	sink_->consume();
}

void MRSinkNode::pause()
{
	ScopedLock lock(*mux);
	nodeImpl.pause();
}

void MRSinkNode::resume()
{
	ScopedLock lock(*mux);
	nodeImpl.resume();
}

void MRSinkNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool MRSinkNode::paused() const
{
	ScopedLock lock(*mux);
	return nodeImpl.paused();
}

void MRSinkNode::updateSnk()
{
	ScopedLock lock(*mux);
	snkImpl->updateSnk();
}

void MRSinkNode::reset()
{
	sink_->reset();
	snkImpl->resetInputs();
}

void MRSinkNode::lockSnkProcessing()
{
	snkImpl->lock();
}

void MRSinkNode::unlockSnkProcessing()
{
	snkImpl->unlock();
}

void MRSinkNode::addInputPin(MRInputPin * pin)
{
	snkImpl->addPin(pin);
}

MRInputPin * MRSinkNode::inputPin(size_type idx)
{
	return snkImpl->pin(idx);
}

const MRInputPin * MRSinkNode::inputPin(size_type idx) const
{
	return snkImpl->pin(idx);
}

const MRSinkNode::size_type MRSinkNode::inputSize() const
{
	return snkImpl->size();
}

const bool MRSinkNode::inputEmpty() const
{
	return snkImpl->empty();
}

MRSourceNode::MRSourceNode(df::ISourceNode * node, df::IDFSource * source) : node_(node), source_(source)
{
	srcImpl = new MRSourceNodeImpl(this, node->outputsConnected());
}

MRSourceNode::~MRSourceNode()
{
	delete srcImpl;
}

void MRSourceNode::process()
{
	source_->produce();
	srcImpl->updateOutputs();
}

void MRSourceNode::pause()
{
	ScopedLock lock(*mux);
	nodeImpl.pause();
}

void MRSourceNode::resume()
{
	ScopedLock lock(*mux);
	nodeImpl.resume();
}

void MRSourceNode::tryPause()
{
	nodeImpl.tryPause();
}

const bool MRSourceNode::paused() const
{
	ScopedLock lock(*mux);
	return nodeImpl.paused();
}

void MRSourceNode::updateSrc()
{
	ScopedLock lock(*mux);
	srcImpl->updateSrc();
}

void MRSourceNode::reset()
{
	ScopedLock lock(*mux);
	source_->reset();
	srcImpl->resetOutputs();
}

void MRSourceNode::lockSrcProcessing()
{
	srcImpl->lock();
}

void MRSourceNode::unlockSrcProcessing()
{
	srcImpl->unlock();
}

void MRSourceNode::addOutputPin(MROutputPin * pin)
{
	srcImpl->addPin(pin);
}

MROutputPin * MRSourceNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const MROutputPin * MRSourceNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const MRSourceNode::size_type MRSourceNode::outputSize() const
{
	return srcImpl->size();
}

const bool MRSourceNode::outputEmpty() const
{
	return srcImpl->size();
}

MRProcessingNode::MRProcessingNode(df::IProcessingNode * node, df::IDFProcessor * processor) : node_(node), processor_(processor)
{
	snkImpl = new MRSinkNodeImpl(this, node->inputsConnected());
	srcImpl = new MRSourceNodeImpl(this, node->outputsConnected());
}

MRProcessingNode::~MRProcessingNode()
{
	delete snkImpl;
	delete srcImpl;
}

void MRProcessingNode::process()
{
	snkImpl->consumeInputs();
	processor_->process();
	srcImpl->updateOutputs();
}

void MRProcessingNode::tryPause()
{
	nodeImpl.tryPause();
}

void MRProcessingNode::pause()
{
	ScopedLock lock(*mux);
	nodeImpl.pause();
}

void MRProcessingNode::resume()
{
	ScopedLock lock(*mux);
	nodeImpl.resume();
}

const bool MRProcessingNode::paused() const
{
	ScopedLock lock(*mux);
	return nodeImpl.paused();
}

void MRProcessingNode::updateSnk()
{
	ScopedLock lock(*mux);
	snkImpl->updateSnk();
}

void MRProcessingNode::updateSrc()
{
	ScopedLock lock(*mux);
	srcImpl->updateSrc();
}

void MRProcessingNode::reset()
{
	ScopedLock lock(*mux);
	processor_->reset();
	snkImpl->resetInputs();
	srcImpl->resetOutputs();
}

void MRProcessingNode::lockSnkProcessing()
{
	snkImpl->lock();
}

void MRProcessingNode::unlockSnkProcessing()
{
	snkImpl->unlock();
}

void MRProcessingNode::addInputPin(MRInputPin * pin)
{
	snkImpl->addPin(pin);
}

MRInputPin * MRProcessingNode::inputPin(size_type idx)
{	
	return snkImpl->pin(idx);
}

const MRInputPin * MRProcessingNode::inputPin(size_type idx) const
{	
	return snkImpl->pin(idx);
}

const MRProcessingNode::size_type MRProcessingNode::inputSize() const
{
	return snkImpl->size();
}

const bool MRProcessingNode::inputEmpty() const
{
	return snkImpl->empty();
}

void MRProcessingNode::lockSrcProcessing()
{
	srcImpl->lock();
}

void MRProcessingNode::unlockSrcProcessing()
{
	srcImpl->unlock();
}

void MRProcessingNode::addOutputPin(MROutputPin * pin)
{
	srcImpl->addPin(pin);
}

MROutputPin * MRProcessingNode::outputPin(size_type idx)
{
	return srcImpl->pin(idx);
}

const MROutputPin * MRProcessingNode::outputPin(size_type idx) const
{
	return srcImpl->pin(idx);
}

const MRProcessingNode::size_type MRProcessingNode::outputSize() const
{
	return srcImpl->size();
}

const bool MRProcessingNode::outputEmpty() const
{
	return srcImpl->empty();
}