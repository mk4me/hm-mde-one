#include "CorePCH.h"
#include "DataProcessor.h"
#include "DataProcessorManager.h"

using namespace core;

CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

DataProcessor::DataProcessor( plugin::IDataProcessor* impl ) : 
  InputOutputItem<plugin::IDataProcessor>(impl)
{
    DataProcessorManager::getInstance()->notifyCreated(this);
}

DataProcessor::~DataProcessor()
{
    DataProcessorManager::getInstance()->notifyDestroyed(this);
}
