#include <dfmlib/DFModel.h>
#include <dfmlib/DFSourceNode.h>
#include <dfmlib/Connection.h>
#include <algorithm>
#include <vector>
#include <boost/bind.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

DFModel::DFModel(void) : Model(), running(false), paused(false),
    finishedLeafes(0), finished(true)
{
    pauseMutex.lock();
    modelRunner.reset(new ModelRunner(this));
}


DFModel::~DFModel(void)
{
    ScopedLock lock(const_cast<DFModel*>(this)->runningMutex);
    if(modelRunner->isRunning() == true){
        modelRunner->finishProcessing();
        pauseMutex.unlock();
        modelRunner->join();
    }
}

bool DFModel::isModelChangeAllowed() const
{
    return Model::isModelChangeAllowed() == true && running == false;
}

bool DFModel::isFinished() const
{
    ScopedLock lock(const_cast<DFModel*>(this)->runningMutex);
    
    return finished;
}

bool DFModel::isRunning() const
{
    ScopedLock lock(const_cast<DFModel*>(this)->runningMutex);

    return running;
}

bool DFModel::isPaused() const
{
    ScopedLock lock(const_cast<DFModel*>(this)->runningMutex);

    return paused;
}

void DFModel::run()
{
    //ScopedLock lock(const_cast<DFModel*>(this));
    ScopedLock lock(const_cast<DFModel*>(this)->runningMutex);
    editMutex.lock();

    if(running == true){
        return;
    }

    if(isModelValid() == false){
        throw std::runtime_error("Can not run invalid model!");
    }

    running = true;
    paused = false;
    finished = false;

    pauseMutex.unlock();

    if(modelRunner->isRunning() == false){
        modelRunner->start();
    }

    notify();
}

void DFModel::pause()
{
    //ScopedLock lock(const_cast<DFModel*>(this));
    ScopedLock lock(const_cast<DFModel*>(this)->editMutex);

    if(running == false || paused == true){
        return;
    }

    paused = true;
}

void DFModel::stop()
{
    //ScopedLock lock(const_cast<DFModel*>(this));
    ScopedLock lock(const_cast<DFModel*>(this)->editMutex);

    if(running == false){
        return;
    }

    running = false;
    paused = false;
    finished = false;
}

bool DFModel::sourcesHaveMoreData() const
{
	bool ret = true;

	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); it++){
		if((*it)->hasMoreData() == false){
			ret = false;
			break;
		}
	}

	return ret;
}

bool DFModel::additionalConnectRules(const CPinPtr & src, const CPinPtr & dest) const
{
	bool ret = true;

	if( src->getParent() == dest->getParent() || dest->empty() == false || createCycle(src, dest) == true){
		ret = false;
	}

	return ret;
}

void DFModel::notifySources()
{
	for(auto it = sourceNodes.begin(); it != sourceNodes.end(); it++){
		(*it)->notify();
	}
}

bool DFModel::isNodeSupported(const NPtr & node) const
{
	if(Model::isNodeSupported(node) && DFNode::getDFNode(node) != nullptr){
		return true;
	}

	return false;
}

void DFModel::leafHasProcessedData()
{
    {
        ScopedLock lock(leavesMutex);
        ++finishedLeafes %= getLeafNodes().size();
    }
    
    //sprawdü czy wszystkie liúcie przetworzy≥y dane
	if( finishedLeafes == 0 ){
        ScopedLock _lock(const_cast<DFModel*>(this)->editMutex);
        if(running == false){
            resetPinStates();
            resetNodeStates();
            finished = true;
            editMutex.unlock();
            notify();
        }else if(paused == false){
            if(sourcesHaveMoreData() == true){
                pauseMutex.unlock();
            }else{
                finished = true;
                running = false;
                paused = false;
                editMutex.unlock();
                notify();
            }
        }
	}
}

void DFModel::resetPinStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); it++){
		DFNPtr node(DFNode::getDFNode(*it));
		if(node != 0){
			for(auto iT = node->beginIn(); iT != node->endIn(); iT++){
				resetPinState(*iT);
			}

			for(auto iT = node->beginOut(); iT != node->endOut(); iT++){
				resetPinState(*iT);
			}
		}
	}
}

void DFModel::resetPinState(const PinPtr & pin)
{
	DFPinPtr p(DFPin::getDFPin(pin));
	//only DFPins are supported
	if(p != nullptr){
		p->reset();
	}
}

void DFModel::resetNodeStates()
{
	for(auto it = getNodes().begin(); it != getNodes().end(); it++){
		DFNPtr node(DFNode::getDFNode(*it));
		if(node != nullptr){
			node->reset();
		}
	}
}

bool DFModel::additionalModelValidation() const
{
    if(getNodes().empty() == true || sourceNodes.empty() == true){
        return false;
    }	

    return true;
}

const DFModel::SourceNodes & DFModel::getSourceNodes() const
{
	return sourceNodes;
}

void DFModel::afterNodeAdd(const NPtr & node)
{
    Model::afterNodeAdd(node);

    DFSNPtr sourceNode(boost::dynamic_pointer_cast<DFSourceNode>(node));
    if(sourceNode != nullptr){
        sourceNodes.insert(sourceNode);
    }
}

void DFModel::beforeNodeRemove(const NPtr & node)
{
    Model::beforeNodeRemove(node);

    DFSNPtr sourceNode(boost::dynamic_pointer_cast<DFSourceNode>(node));
    if(sourceNode != nullptr){
        sourceNodes.erase(sourceNode);
    }
}

void DFModel::afterLeafAdd(const NPtr & node)
{
    DFNode::getDFNode(node)->onLeafProcessedCallback = DFNode::OnLeafProcessedCallback(boost::bind(&DFModel::leafHasProcessedData, this));
}

void DFModel::beforeLeafRemove(const NPtr & node)
{
    DFNode::getDFNode(node)->onLeafProcessedCallback = DFNode::OnLeafProcessedCallback();
}

DFModel::ModelRunner::ModelRunner(DFModel * model) : model(model), finish(false)
{

}

DFModel::ModelRunner::~ModelRunner()
{

}

void DFModel::ModelRunner::run()
{
    while(true){
        {
            ScopedLock lock(model->pauseMutex);
            if(finish == true){
                return;
            }

            model->notifySources();
        }        
        
        model->pauseMutex.lock();
    }
}

void DFModel::ModelRunner::finishProcessing()
{
    finish = true;
}

}