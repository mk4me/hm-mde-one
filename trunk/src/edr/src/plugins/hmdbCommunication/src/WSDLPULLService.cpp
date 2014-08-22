#include "CommunicationPCH.h"
#include "WSDLPULLService.h"
#include <wsdlparser/WsdlInvoker.h>

using namespace hmdbCommunication;

template<typename Invoker>
class WSDLPULLServiceImpl : public networkUtils::IWSDLService
{
public:
	WSDLPULLServiceImpl(CURL * curl) : invoker_(curl)
	{

	}

	virtual ~WSDLPULLServiceImpl()
	{

	}

	void setOperation(const std::string & operation)
	{
		impl->setOperation(operation);
	}

	void setValue(const std::string & name, const std::string & value)
	{
		impl->setValue(name, value);
	}

	void invoke(const bool process = false)
	{
		impl->invoke(process);
	}

	const std::string xmlResponse() const
	{
		return impl->xmlResponse();
	}

	const void * getValue(const std::string & name) const
	{
		return impl->getValue(name);
	}

private:
	//! Invoker Wsdlpull`a
	Invoker invoker_;
};