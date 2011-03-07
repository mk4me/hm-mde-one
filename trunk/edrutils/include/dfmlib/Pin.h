#ifndef __HEADER_GUARD__PIN_H__
#define __HEADER_GUARD__PIN_H__

#include <dfmlib/DFLMTypes.h>
#include <set>
#include <boost/enable_shared_from_this.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class Pin : public boost::enable_shared_from_this<Pin>
{
public:
	typedef enum {PIN_IN, PIN_OUT, PIN_UNKNOWN} PIN_TYPE;
	typedef enum {PIN_OK, PIN_REQUIRED, PIN_INCOMPLETE, PIN_CONNECTED} PIN_STATIC_STATUS;
	typedef enum {PIN_COMPATIBLE, PIN_COMPATIBLE_CYCLE, PIN_COMPATIBLE_FULL, PIN_INCOMPATIBLE} PIN_DYNAMIC_STATUS;
	typedef std::set<ConnPtr> CONNECTIONS_SET;
	typedef std::set<WPinPtr> REQ_PINS_SET;

public:

	friend class Node;
	friend class Model;

public:
	Pin(WNPtr parentNode, const std::string & pinName = std::string(),
		bool required = false, const REQ_PINS_SET & requiredPins = REQ_PINS_SET(),
		const CONNECTIONS_SET & connections = CONNECTIONS_SET());
	
	virtual ~Pin(void);

	NPtr getParent() const;
	const CONNECTIONS_SET & getConnections() const;

	PIN_TYPE getType() const;

	PIN_STATIC_STATUS getStaticStatus() const;
	PIN_DYNAMIC_STATUS getDynamicStatus(PinPtr refPin, MPtr model) const;

	virtual bool isCompatible(CPinPtr pin) const;
	
	bool dependsOnPin(PinPtr pin) const;
	const REQ_PINS_SET & getDependantPins() const;

	bool isRequired() const;
	bool isComplete() const;

	const std::string & getPinName() const;
	void setPinName(const std::string & pinName);

private:
	//void verifyRequiredPins();

	bool addConnection(ConnPtr cn);
	bool removeConnection(ConnPtr cn);
	void clearConnections();
	void setType(PIN_TYPE type);

protected:
	CONNECTIONS_SET connections;
	WNPtr parentNode;
	bool pinRequired;
	REQ_PINS_SET requiredPinsDependency;
	PIN_TYPE pinType;

	std::string pinName;
};

}

#endif