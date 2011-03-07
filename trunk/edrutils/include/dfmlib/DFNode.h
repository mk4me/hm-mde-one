#ifndef __HEADER_GUARD__DFNODE_H__
#define __HEADER_GUARD__DFNODE_H__

#include <dfmlib/Node.h>
#include <dfmlib/DFInterface.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFNode :
	public Node, public DFInterface
{
public:

	virtual ~DFNode(void);

	bool setEnable(bool enable);

	bool addInPin(PinPtr newPin);
	bool addOutPin(PinPtr newPin);

	void allowProcessing(bool allow);
	void allowPropagating(bool allow);

	bool isProcessingAllowed() const;
	bool isPropagatingAllowed() const;

	virtual void configureNode();

	static DFNPtr getDFNode(NPtr node);

protected:

	DFNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
		bool propagatingAllowed = true);

	void process();

	virtual bool checkInputPins() const;
	virtual void updateOutputPins();

	virtual void getInputData();
	virtual void setOutputData();
	virtual void processData();

protected:

	bool processingAllowed;
	bool propagatingAllowed;
};

}

#endif