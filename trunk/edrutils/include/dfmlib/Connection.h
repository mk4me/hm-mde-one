#ifndef HEADER_GUARD__CONNECTION_H__
#define HEADER_GUARD__CONNECTION_H__

#include <dfmlib/DFLMTypes.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class Connection
{

public:

	friend class DFModel;

public:
	Connection(PinPtr src, PinPtr dest);
	virtual ~Connection(void);

	PinPtr getSrc() const;
	PinPtr getDest() const;

	PinPtr getOther(CWPinPtr pin) const;
	std::string getConnectionName() const;

private:
	void setSrc(PinPtr src);
	void setDest(PinPtr dest);

protected:
	WPinPtr srcPin;
	WPinPtr destPin;
};

}

#endif