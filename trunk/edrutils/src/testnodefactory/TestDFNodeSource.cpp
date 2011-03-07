#include <testnodefactory/TestDFNodeSource.h>


TestDFNodeSource::TestDFNodeSource(const std::string & nodeName) : DFNode(nodeName), DFSourceNode(nodeName), TestDFNodeProcessing(nodeName), availableData(0)
{
}


TestDFNodeSource::~TestDFNodeSource(void)
{
}

bool TestDFNodeSource::addInPin(dflm::PinPtr inPin){
	return DFSourceNode::addInPin(inPin);
}

bool TestDFNodeSource::checkInputPins() const{
	return true;
}

bool TestDFNodeSource::hasMoreData() const{
	if(availableData < 10){
		return true;
	}

	return false;
}

void TestDFNodeSource::process(){
	TestDFNodeProcessing::process();
	availableData++;
}
