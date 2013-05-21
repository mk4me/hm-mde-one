#include "MRPin.h"
#include "IMRNode.h"
#include "MRConnection.h"

MRInputPin::MRInputPin(IMRSink * sink, df::IInputPin * pin, df::IDFInput * dfInput) : sink_(sink), pin_(pin), dfInput_(dfInput), consumed_(false), outputsReady(0)
{

}

MRInputPin::~MRInputPin()
{

}

IMRSink * MRInputPin::sink()
{
	return sink_;
}

const IMRSink * MRInputPin::sink() const
{
	return sink_;
}

void MRInputPin::reset()
{
	ScopedLock lock(*mux);
	dfInput_->reset();
}

void MRInputPin::update()
{
	ScopedLock lock(*mux);
	consumed_ = false;
	++outputsReady;
	if(outputsReady == connections.size())
	{
		sink_->updateSnk();	
	}
}

const bool MRInputPin::updated() const
{
	ScopedLock lock(*mux);
	return outputsReady == connections.size();
}

void MRInputPin::consumeData()
{
	ScopedLock lock(*mux);	
	try{
		for(auto it = connections.begin(); it != connections.end(); ++it){
			dfInput_->copyData((*it)->source()->dfOutput());
			(*it)->source()->consumeData();
		}
		outputsReady = 0;
		consumed_ = true;
	}catch(std::exception & e){
		
	}catch(...){

	}
}

const bool MRInputPin::dataConsumed() const
{
	ScopedLock lock(*mux);
	return consumed_;
}

MRConnection * MRInputPin::connection(size_type idx)
{
	return connections[idx];
}

const MRConnection * MRInputPin::connection(size_type idx) const
{
	return connections[idx];
}

void MRInputPin::addConnection(MRConnection * connection)
{
	if(std::find(connections.begin(), connections.end(), connection) != connections.end())
	{
		throw std::exception("Connection already registered in pin");
	}

	connections.push_back(connection);
}

MRInputPin::size_type MRInputPin::size() const
{
	return connections.size();
}

const bool MRInputPin::empty() const
{
	return connections.empty();
}

df::IInputPin * MRInputPin::innerPin()
{
	return pin_;
}

const df::IInputPin * MRInputPin::innerPin() const
{
	return pin_;
}

df::IDFInput * MRInputPin::dfInput()
{
	return dfInput_;
}

const df::IDFInput * MRInputPin::dfInput() const
{
	return dfInput_;
}

MROutputPin::MROutputPin(IMRSource * source, df::IOutputPin * pin, df::IDFOutput * dfOutput) : source_(source), pin_(pin), dfOutput_(dfOutput), updated_(false), inputsConsumed(0)
{

}

MROutputPin::~MROutputPin()
{

}

IMRSource * MROutputPin::source()
{
	return source_;
}

const IMRSource * MROutputPin::source() const
{
	return source_;
}

void MROutputPin::reset()
{
	ScopedLock lock(*mux);
	dfOutput_->reset();
}

void MROutputPin::update()
{
	ScopedLock lock(*mux);
	inputsConsumed = 0;
	updated_ = true;

	for(auto it = connections.begin(); it != connections.end(); ++it)
	{
		(*it)->destination()->update();
	}
}

const bool MROutputPin::updated() const
{
	ScopedLock lock(*mux);
	return updated_;
}

void MROutputPin::consumeData()
{
	ScopedLock lock(*mux);
	++inputsConsumed;
	if(inputsConsumed == connections.size())
	{
		source_->updateSrc();
		updated_ = false;
	}
}

const bool MROutputPin::dataConsumed() const
{
	ScopedLock lock(*mux);
	return inputsConsumed == connections.size();
}

MRConnection * MROutputPin::connection(size_type idx)
{
	return connections[idx];
}

const MRConnection * MROutputPin::connection(size_type idx) const
{
	return connections[idx];
}

void MROutputPin::addConnection(MRConnection * connection)
{
	if(std::find(connections.begin(), connections.end(), connection) != connections.end())
	{
		throw std::exception("Connection already registered in pin");
	}

	connections.push_back(connection);
}

MROutputPin::size_type MROutputPin::size() const
{
	return connections.size();
}

const bool MROutputPin::empty() const
{
	return connections.empty();
}

df::IOutputPin * MROutputPin::innerPin()
{
	return pin_;
}

const df::IOutputPin * MROutputPin::innerPin() const
{
	return pin_;
}

df::IDFOutput * MROutputPin::dfOutput()
{
	return dfOutput_;
}

const df::IDFOutput * MROutputPin::dfOutput() const
{
	return dfOutput_;
}