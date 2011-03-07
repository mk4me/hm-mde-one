#ifndef __HEADER_GUARD__VDFBASEPIN_H__
#define __HEADER_GUARD__VDFBASEPIN_H__

#include <osgWidget/Widget>
#include <dfmlib/DFPin.h>
#include <map>
#include <vdfmlib/osgUI2DUtils.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseModel;
class osgVDFBaseNode;

class osgVDFBasePin : public osgWidget::Widget
{
protected:

	friend class osgVDFBaseModel;
	friend class osgVDFBaseNode;

public:
	typedef enum {	PIN_OK,
					PIN_ACTIVE,
					//PIN_COMPATIBLE,
					PIN_INCOMPATIBLE,
					PIN_REQUIRED,
					PIN_CONNECTED,
					PIN_INCOMPLETE} PIN_VISUAL_STATUS;

	typedef std::map<osg::Geode*, osgVDFBasePin*> CONNECTIONS_MAP;

public:
	//META_Object(osgWidget::Widget, osgVDFBasePin);

	osgVDFBasePin (const std::string& name = "", dflm::PinPtr pin = dflm::PinPtr(), osgVDFBaseModel * model = 0);
    osgVDFBasePin (const osgVDFBasePin& pin, const osg::CopyOp& co);

	~osgVDFBasePin(void);

    virtual void positioned();

	virtual void managed(osgWidget::WindowManager* wm);

	dflm::PinPtr getModelPin() const;

	osgVDFBaseNode * getParentNode() const;

	PIN_VISUAL_STATUS getStaticPinVisualStatus() const;
	PIN_VISUAL_STATUS getDynamicPinVisualStatus(dflm::PinPtr refPin, dflm::MPtr model) const;
	PIN_VISUAL_STATUS getDynamicPinVisualStatus(osgVDFBasePin * refPin, dflm::MPtr model) const;

	virtual const osgWidget::XYCoord & getCenterPosition();
	virtual osgWidget::XYCoord getCenterPosition() const;
	
	const CONNECTIONS_MAP & getConnections() const;

	void setPinVisualStatus(PIN_VISUAL_STATUS pinVisualStatus);
	PIN_VISUAL_STATUS getPinVisualStatus() const;

protected:
	bool addConnection(osg::Geode * connection, osgVDFBasePin * other);
	bool removeConnection(osg::Geode * connection);

	virtual void graphSetPinStatus(PIN_VISUAL_STATUS pinVisualStatus) = 0;

protected:
	dflm::PinPtr m_pPin;
	CONNECTIONS_MAP m_mConnections;

	osgVDFBaseNode * m_pParentNode;
	osgVDFBaseModel * m_pModel;

	PIN_VISUAL_STATUS m_eVisualStatus;

	osgWidget::XYCoord m_tCenterPosition;
	bool m_bCenterChanged;
};

}

#endif