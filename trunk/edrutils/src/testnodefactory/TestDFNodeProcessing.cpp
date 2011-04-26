#include <testnodefactory/TestDFNodeProcessing.h>

#include <iostream>
#include <string>

TestDFNodeProcessing::TestDFNodeProcessing(const std::string & nodeName) : DFNode(nodeName) 
{
}


TestDFNodeProcessing::~TestDFNodeProcessing(void)
{
}

void TestDFNodeProcessing::process(){

    std::cout << getNodeName().c_str() << " active - DATA PROCESSING procedure" << std::endl;

    if(checkInputPins() == true){
        std::cout << "inpute pins verified correctly - data on all connections avaiable" << std::endl;
        std::cout << "getting input data" << std::endl;

        if(isProcessingAllowed() == true){
			std::cout << "data processing allowed" << std::endl;
			std::cout << "processing data" << std::endl;
			processData();
		}

		if(isPropagatingAllowed() == true){
			std::cout << "data propagation allowed" << std::endl;
			std::cout << "propagating data" << std::endl;
			std::cout << "setting output pins data" << std::endl;
			setOutputData();
			std::cout << "updating output pins" << std::endl;
			updateOutputPins();
		}
	}
}
