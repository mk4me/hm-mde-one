#ifndef HEADER_GUARD__DFINTERFACE_H__
#define HEADER_GUARD__DFINTERFACE_H__

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFInterface
{
public:
	DFInterface(bool enable = true);
	virtual ~DFInterface(void);

	bool setEnable(bool enable);
	bool isEnable() const;
	virtual void reset();
	void notify();

protected:
    virtual void onEnableChange();
	virtual void process() = 0;

private:
	bool objectEnable;
};

}

#endif
