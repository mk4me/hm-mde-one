#ifndef HEADER_GUARD__DFM_TESTPIN_H__
#define HEADER_GUARD__DFM_TESTPIN_H__

#include <dfmlib/DFPin.h>

class TestDFPin :
	public dflm::DFPin
{
public:
	TestDFPin(const std::string & pinName = std::string(),
		bool required = false, const std::set<dflm::WPinPtr> & requiredPins = std::set<dflm::WPinPtr>(),
		const std::set<dflm::ConnPtr> & connections = std::set<dflm::ConnPtr>());
	virtual ~TestDFPin(void);

	std::string getPinFullName() const;

protected:
	bool onUpdate();
	void copyDataToPin(dflm::DFPinPtr pin);
};


#endif