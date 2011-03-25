#include <testnodefactory/TestDFPin.h>
#include <testnodefactory/TestDFNodeProcessing.h>

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

TestDFPin::TestDFPin(dflm::WDFNPtr parentNode, const std::string & pinName, bool required,
		const std::set<dflm::WPinPtr> & requiredPins, const std::set<dflm::ConnPtr> & connections)
		: DFPin(parentNode, pinName, required, requiredPins, connections){
}

TestDFPin::~TestDFPin(void)
{
}

std::string TestDFPin::getPinFullName() const{
	std::string fullName;

	boost::shared_ptr<TestDFNodeProcessing> parent(boost::dynamic_pointer_cast<TestDFNodeProcessing>(parentNode.lock()));
	if(parent != 0){
		fullName.append(parent->getNodeName());
	}else{
		fullName.append("UNKNOWN PARENT");
	}

	fullName.append(":");
	fullName.append(pinName);

	return fullName;
}

bool TestDFPin::onUpdate(){
	std::cout << "UPDATE " << getPinFullName().c_str() << std::endl;
	return true;
}

void TestDFPin::copyDataToPin(dflm::DFPinPtr pin){
	std::cout << getPinFullName().c_str() << " copies data to " << boost::dynamic_pointer_cast<TestDFPin>(pin)->getPinFullName().c_str() << std::endl;
}