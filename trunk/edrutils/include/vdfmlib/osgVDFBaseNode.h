#ifndef HEADER_GUARD__VDFBASENODE_H__
#define HEADER_GUARD__VDFBASENODE_H__

#include <vdfmlib/osgVDFBasePin.h>
#include <osgWidget/Window>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseModel;

class osgVDFBaseNode : public osgWidget::Window
{
public:
	typedef enum {NODE_OK = 0, NODE_COLLISION, NODE_ACTIVE} NODE_VISUAL_STATUS;
	typedef enum {NODE_SOURCE, NODE_VISUALIZE, NODE_PROCESS, NODE_ANALYZE} NODE_TYPE;
	typedef std::set<osg::ref_ptr<osgVDFBasePin> > VISUAL_PIN_SET;

public:

	//META_Window(osgWidget::Window, osgVDFBaseNode);

	osgVDFBaseNode(dflm::NPtr modelNode = dflm::NPtr(), const std::string & nodeName = std::string(), osgVDFBaseModel * model = 0);
	virtual ~osgVDFBaseNode(void);

	dflm::NPtr getModelNode() const;
	osgVDFBaseModel * getModel() const;
	
	void addInPin(osgVDFBasePin * inPin, const std::string & pinName = std::string());
	void addOutPin(osgVDFBasePin * outPin, const std::string & pinName = std::string());

	const VISUAL_PIN_SET & getInPins() const;
	const VISUAL_PIN_SET & getOutPins() const;

	//for debug purpose only!!
	virtual void loggZCoordinates() const;

	virtual void setName(const std::string & name) = 0;
	virtual std::string getName() const = 0;

	void setNodeVisualStatus(NODE_VISUAL_STATUS nodeVisualStatus);
	NODE_VISUAL_STATUS getNodeVisualStatus() const;

protected:

	virtual void graphAddInPin(osgVDFBasePin * inPin, const std::string & pinName) = 0;
	virtual void graphAddOutPin(osgVDFBasePin * outPin, const std::string & pinName) = 0;
	virtual void graphSetNodeStatus(NODE_VISUAL_STATUS nodeVisualStatus) = 0;

	void deleteNode();

protected:

	NODE_VISUAL_STATUS m_eStatus;

	//for logical model
	dflm::NPtr m_pNode;

	//graphical representation
	VISUAL_PIN_SET inPins;
	VISUAL_PIN_SET outPins;

	osgVDFBaseModel * m_pModel;
};

}

#endif
