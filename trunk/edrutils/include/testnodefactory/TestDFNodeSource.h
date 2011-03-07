#ifndef __HEADER_GUARD__DFM_TESTSOURCENODE_H__
#define __HEADER_GUARD__DFM_TESTSOURCENODE_H__

#include <dfmlib/DFSourceNode.h>
#include <testnodefactory/TestDFNodeProcessing.h>

class TestDFNodeSource :
	public dflm::DFSourceNode, public TestDFNodeProcessing
{
public:
	friend class TestNodeFactory;

public:
	virtual ~TestDFNodeSource(void);
	virtual bool hasMoreData() const;

	virtual bool addInPin(dflm::PinPtr inPin);

protected:
	TestDFNodeSource(const std::string & nodeName);

	virtual bool checkInputPins() const;
	virtual void process();

protected:
	unsigned int availableData;
};

#endif