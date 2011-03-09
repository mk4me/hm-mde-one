#ifndef HEADER_GUARD__DFM_TESTPROCESSINGNODE_H__
#define HEADER_GUARD__DFM_TESTPROCESSINGNODE_H__

#include <dfmlib/DFNode.h>

class TestNodeFactory;

class TestDFNodeProcessing :
	public virtual dflm::DFNode
{
public:
	friend class TestNodeFactory;

public:
	~TestDFNodeProcessing(void);

protected:
	TestDFNodeProcessing(const std::string & nodeName);

	virtual void process();
};

#endif