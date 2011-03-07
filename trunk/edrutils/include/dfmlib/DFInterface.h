#ifndef __HEADER_GUARD__DFINTERFACE_H__
#define __HEADER_GUARD__DFINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFInterface
{
public:
	DFInterface(void);
	virtual ~DFInterface(void);

	virtual bool setEnable(bool enable) = 0;
	bool isEnable() const;
	virtual void reset();
	void notify();

protected:

	virtual void process() = 0;

protected:
	bool objectEnable;
};

}

#endif
