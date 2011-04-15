#ifndef HEADER_GUARD__DFPIN_H__
#define HEADER_GUARD__DFPIN_H__

#include <dfmlib/Pin.h>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFPin : public Pin {
public:

	virtual ~DFPin(void);

	bool isUpdated() const;
	void reset();
	bool update();

	static bool isDFPin(PinPtr pin);
	static DFPinPtr getDFPin(PinPtr pin);

protected:

	DFPin(const std::string & nodeName = std::string(),
		bool required = false, const REQ_PINS_SET & requiredPins = REQ_PINS_SET(),
		const CONNECTIONS_SET & connections = CONNECTIONS_SET());

	virtual bool onUpdate();
	virtual void copyDataFromPin(DFPinPtr pin);

protected:
	bool updated;
};

}

#endif