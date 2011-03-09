#ifndef HEADER_GUARD__DFSOURCENODE_H__
#define HEADER_GUARD__DFSOURCENODE_H__

#include <dfmlib/DFNode.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFSourceNode :
	public virtual DFNode
{
public:
	virtual ~DFSourceNode(void);
	virtual bool hasMoreData() const = 0;

	virtual bool addInPin(PinPtr newPin);

protected:

	DFSourceNode(const std::string & nodeName = std::string(), bool processingAllowed = true,
		bool propagatingAllowed = true);
	virtual bool checkInputPins() const;
	void resetInputPins();
};

}

#endif