#ifndef HEADER_GUARD__NODE_H__
#define HEADER_GUARD__NODE_H__

#include <dfmlib/DFLMTypes.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class Node : public boost::enable_shared_from_this<Node>
{

public:

	friend class Model;

public:

	typedef std::set<PinPtr> PINS_SET;

public:

	virtual ~Node(void);

	virtual bool addInPin(PinPtr newPin);
	virtual bool addOutPin(PinPtr newPin);

	const PINS_SET & getInPins() const;
	const PINS_SET & getOutPins() const;

	bool pinExists(PinPtr pin) const;

	bool validInPinsConnection() const;
	bool validOutPinsConnection() const;
	bool validConnection() const;
	bool anyInPinConnected() const;
	bool anyOutPinConnected() const;
	bool anyPinConnected() const;

	const std::string & getNodeName() const;
	void setNodeName(const std::string & nodeName);

protected:

	static bool pinExists(const PINS_SET & pins, PinPtr pin);
	static bool anyPinConnected(const PINS_SET & pins);

	Node(const std::string & nodeName = std::string());

protected:
	PINS_SET inPins;
	PINS_SET outPins;
	std::string nodeName;
};

}

#endif
