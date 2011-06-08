#include <testnodefactory/TestDFPin.h>
#include <testnodefactory/TestDFNodeProcessing.h>

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

TestDFPin::TestDFPin(const std::string & pinName, bool required,
		const std::set<dflm::WPinPtr> & requiredPins)
		: DFPin(pinName, required, requiredPins){
}

TestDFPin::~TestDFPin(void)
{
}

std::string TestDFPin::getPinFullName() const{
	std::string fullName;

	boost::shared_ptr<TestDFNodeProcessing> parent(boost::dynamic_pointer_cast<TestDFNodeProcessing>(getParent()));
	if(parent != 0){
		fullName.append(parent->getName());
	}else{
		fullName.append("UNKNOWN PARENT");
	}

	fullName.append(":");
	fullName.append(getName());

	return fullName;
}

void TestDFPin::onUpdate(){
	std::cout << "UPDATE " << getPinFullName().c_str() << std::endl;
}

void TestDFPin::copyDataToPin(dflm::DFPinPtr pin){
	std::cout << getPinFullName().c_str() << " copies data to " << boost::dynamic_pointer_cast<TestDFPin>(pin)->getPinFullName().c_str() << std::endl;
}
