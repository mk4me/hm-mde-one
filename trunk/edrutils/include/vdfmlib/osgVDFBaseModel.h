#ifndef HEADER_GUARD__VDFBASEMODEL_H__
#define HEADER_GUARD__VDFBASEMODEL_H__

#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <boost/function.hpp>

#include <osgui/KeyboardMapper.h>

#include <osgui/Buttonized.h>
#include <osgui/Borderized.h>

#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Connection.h>
#include <dfmlib/DFModel.h>

#include <vdfmlib/osgVDFNodeTypeDescriptor.h>

#include <osgui/Toolbar.h>
#include <osgui/ContextMenu.h>
#include <osgui/TooltipHost.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

class osgVDFBaseNode;
class osgVDFBasePin;
class osgVDFContextMenu;

class osgVDFBaseModel : public osgWidget::WindowManager, public osgui::KeyboardMapper, public osgui::TooltipHost{
public:
	osgVDFBaseModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
		unsigned int nodeMask = 0, unsigned int flags = 0);

	~osgVDFBaseModel(void);

	bool registerNodeType(osgVDFNodeTypeDescriptor * descriptor);

	bool isEmptyArea(const osgWidget::XYCoord & posToStart);

	bool addNode(dflm::NPtr node, osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string());
	bool deleteNode(dflm::NPtr node);

	dflm::Model::NODES_SET getNodesInArea(const osg::BoundingBox & area);
	virtual osgWidget::XYCoord getFreePlaceForNode(dflm::NPtr node, const osgWidget::XYCoord & posToStart);

	void selectNode(dflm::NPtr node);
	void selectNodes(const dflm::Model::NODES_SET & nodes);
	void selectNodes(const osg::BoundingBox & rect);
	void selectAllNodes();

	void deselectNode(dflm::NPtr node);
	void deselectNodes(const dflm::Model::NODES_SET & nodes);
	void deselectNodes(const osg::BoundingBox & rect);
	void deselectAllNodes();

	bool isNodeSelected(dflm::NPtr node) const;
	dflm::Model::NODES_SET getSelectedNodes() const;
	osg::BoundingBox getSelectedNodesBoundingBox() const;

	bool isNodeInCollision(dflm::NPtr node) const;
	bool isNodeInVisibleArea(dflm::NPtr node) const;
	bool isNodeFullyInVisibleArea(dflm::NPtr node) const;

	osgWidget::XYCoord getNodePosition(dflm::NPtr node);
	osgWidget::XYCoord getNodeSize(dflm::NPtr node);

	void moveNode(dflm::NPtr node, const osgWidget::XYCoord & moveStep);
	void moveSelectedNodes(const osgWidget::XYCoord & moveStep);

	void setNodePosition(dflm::NPtr node, const osgWidget::XYCoord & nodePos);
	void setSelectedNodesPosition(const osgWidget::XYCoord & nodesPos);

	dflm::ConnPtr connect(dflm::PinPtr src, dflm::PinPtr dest);
	bool disconnect(dflm::ConnPtr connection);	

	void showDefaultToolbar(bool show);

	osgWidget::point_type getAreaRatioToSelect() const;
	void setAreaRatioToSelect(osgWidget::point_type areaRatio);

	const osgui::KeyboardMapper::Keys & getSelectionActionKeys() const;
	void setSelectionActionKeys(const osgui::KeyboardMapper::Keys & keys);

	osgWidget::point_type getMinDistToDelConnection() const;
	void setMinDistToDelConnection(osgWidget::point_type dist);

	osgWidget::Color getVisualConnectionNormalColor() const { return visualConnectionNormalColor; }
	void setVisualConnectionNormalColor(osgWidget::Color val) { visualConnectionNormalColor = val; }
	
	osgWidget::Color getVisualConnectionHighlightColor() const { return visualConnectionHighlightColor; }
	void setVisualConnectionHighlightColor(osgWidget::Color val) { visualConnectionHighlightColor = val; }

	dflm::CDFMPtr getLogicalModel() const;

protected:
	typedef std::map<dflm::NPtr, osg::ref_ptr<osgVDFBaseNode>> NODES_MAPPING;
	typedef std::map<dflm::PinPtr, osgVDFBasePin*> PINS_MAPPING;
	typedef std::map<dflm::ConnPtr, osg::ref_ptr<osg::Geode> > CONNECTIONS_MAPPING;
	typedef std::map<osg::ref_ptr<osg::Geode>, dflm::ConnPtr> REV_CONNECTIONS_MAPPING;
	typedef std::set<osgVDFBaseNode*> VNODES_SET;
	typedef std::map<osgVDFBaseNode*, osgVDFBaseNode::NODE_VISUAL_STATUS> NODE_PREV_STATUS;
	typedef std::map<osgVDFBasePin*, osgVDFBasePin::PIN_VISUAL_STATUS> PIN_PREV_STATUS;
	typedef std::map<osgVDFBasePin*, osgVDFBaseNode::VISUAL_PIN_SET> PINS_COMPATIBILITY_MAP;

	typedef std::map<std::string, osg::Geode*> CONNECTIONS_DELETE_MAP;

private:
	//typedef void (osgVDFBaseModel::*connUpdate)(osg::Geode*, const osgWidget::XYCoord &);
	typedef boost::function<void(osg::Geode*, const osgWidget::XYCoord&)> connectionUpdate;

	typedef std::map<osgVDFBasePin*, connectionUpdate> PINS_CONNECTIONS_UPDATE_MAP;
	typedef osgui::Buttonized<osgui::Borderized<osgWidget::Label>, osgui::TextStylePolicy> BUTTON;
	//typedef osgui::Buttonized<osgWidget::Label> BUTTON;

private:

	class ModelResizeHandler : public osgGA::GUIEventHandler{

	public:
		ModelResizeHandler(osgVDFBaseModel * m);
		virtual ~ModelResizeHandler();

		virtual bool handle(const osgGA::GUIEventAdapter& gea,
			osgGA::GUIActionAdapter&      gaa,
			osg::Object*                  obj,
			osg::NodeVisitor*             nv
			);

	protected:
		osg::observer_ptr<osgVDFBaseModel> model;
	};

	friend class ModelResizeHandler;

	class VisualSelectionManager : public osgGA::GUIEventHandler{

	public:
		VisualSelectionManager(osgVDFBaseModel * m);

		virtual ~VisualSelectionManager();

		virtual bool handle(const osgGA::GUIEventAdapter& gea,
			osgGA::GUIActionAdapter&      gaa,
			osg::Object*                  obj,
			osg::NodeVisitor*             nv
			);

	protected:
		osg::observer_ptr<osgVDFBaseModel> model;
		bool visualSelectionStarted;
	};

	friend class VisualSelectionManager;

	class UserSpaceClick : public osgGA::GUIEventHandler{
	public:
		UserSpaceClick(osgVDFBaseModel * m);

		virtual ~UserSpaceClick();

		virtual bool handle(const osgGA::GUIEventAdapter& gea,
			osgGA::GUIActionAdapter&      gaa,
			osg::Object*                  obj,
			osg::NodeVisitor*             nv
		);

		protected:
			static void DistanceFromLine(double cx, double cy, double ax, double ay ,
				double bx, double by, double &distanceSegment,
				double &distanceLine);

	protected:
		osg::observer_ptr<osgVDFBaseModel> model;
	};


	friend class NodeDescriptorClick;
	friend class osgVDFBaseNode;
	friend class osgVDFBasePin;

protected:
	osgWidget::point_type getMinNodesZ();
	osgWidget::point_type getMaxNodesZ();

	void hideConnections();

	static void refreshPinVisualState(osgVDFBasePin * pin);
	static void addConnectionToPin(osgVDFBasePin * pin, osg::Geode * connection, osgVDFBasePin * other);
	static void removeConnectionFromPin(osgVDFBasePin * pin, osg::Geode * connection);

	virtual void configureVisualNode(osgVDFBaseNode * node);
	virtual void configureVisualPin(osgVDFBasePin * pin);
	virtual void showConnectionsDeleteContextMenu(const CONNECTIONS_DELETE_MAP & toDelete, const osgWidget::XYCoord & pos);

	static void refreshOutputPins(osgVDFBaseNode * node);

	osgVDFBaseNode* getVisualNode(dflm::NPtr) const;

	void refreshFocused(osgWidget::Window * toFocus = 0);

private:
	void updatePinsConnections(osgVDFBasePin * pin);

	bool osgVDFBaseModel::deleteNode(dflm::NPtr node, bool clearEvent);
	bool deleteNodeSelf(osgVDFBaseNode * node);

	virtual osg::ref_ptr<osgVDFBaseNode> createVisualNode(dflm::NPtr node, osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string()) = 0;
	
	//user interaction - connection managment
	virtual osg::ref_ptr<osg::Geode> createVisualConnection(osgVDFBasePin * pinA, osgVDFBasePin * pinB) = 0;
	virtual osg::ref_ptr<osg::Geode> startVisualConnection(osgVDFBasePin * startPin) = 0;
	virtual void setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos) = 0;
	virtual void setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos) = 0;
	virtual void setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z) = 0;

	//user interaction - selection managment
	virtual osg::ref_ptr<osg::Geode> createVisualSelection(const osgWidget::XYCoord & startPos) = 0;
	virtual void continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos) = 0;

	virtual void setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color ) = 0;

	bool onNodeClick(osgWidget::Event& ev);
	bool onNodeDrag(osgWidget::Event& ev);
	bool onNodeRelease(osgWidget::Event& ev);
	void updateCollisions();
	VNODES_SET getSelectedNodesCollisions();

	bool onPinClick(osgWidget::Event& ev);
	bool onPinDrag(osgWidget::Event& ev);
	bool onPinRelease(osgWidget::Event& ev);

	bool onPinEnter(osgWidget::Event& ev);
	bool onPinLeave(osgWidget::Event& ev);

	bool onNodeEnter(osgWidget::Event& ev);
	bool onNodeLeave(osgWidget::Event& ev);

	std::vector<std::string> getPinErrors(dflm::PinPtr pin);
	std::vector<std::string> getNodeErrors(dflm::NPtr node);

	void savePinsStatus(const osgVDFBaseNode::VISUAL_PIN_SET & pins);
	void highlightCompatiblePins(osgVDFBasePin * startPin, const osgVDFBaseNode::VISUAL_PIN_SET & pins);
	void restorePinsStatus();

private:
	bool onNodeDescriptorPush(osgWidget::Event& ev);

	void fillToolbarWithGroups(osgui::Toolbar * toolbar);
	bool graphAddNodeTypeToToolbar(osgVDFNodeTypeDescriptor* nodeTypeDescriptor);

	void highlightConnection(osg::Geode * connection, const std::string & connectionName, bool highlight);
	void removeConnection(dflm::ConnPtr connection, const std::string & connectionName, bool checked);
	void onCloseContextMenu(osgui::ContextMenu * menu);

protected:

	osgVDFNodeTypeDescriptor * currentNodeDescriptor;
	std::map<osgWidget::Widget*,osgVDFNodeTypeDescriptor*> controlsToNodeTypes;
	std::map<osgVDFNodeTypeDescriptor*,osgWidget::Widget*> nodeTypesToControls;
	std::set<osgVDFNodeTypeDescriptor*> nodeTypesMissing;

protected:
	dflm::DFMPtr logicalModel;
	
	NODES_MAPPING nodesLogicalToGraph;
	VNODES_SET graphNodes;
	PINS_MAPPING pinsLogicalToGraph;
	PINS_COMPATIBILITY_MAP compatiblePins;

	CONNECTIONS_MAPPING connectionsLogicalToGraph;
	REV_CONNECTIONS_MAPPING connectionsGraphToLogical;

	VNODES_SET selectedNodes;
	KeyboardMapper::Keys selectionActionKeys;
	osgWidget::point_type minIntersectRatioToSelect;

	//connection events
	osgWidget::point_type maxDistToDelConnection;

	osgWidget::Color visualConnectionNormalColor;
	osgWidget::Color visualConnectionHighlightColor;
	
private:
	NODE_PREV_STATUS selectionPrevNodesStatus;	
	osgWidget::XYCoord selectionStartPose;
	osgVDFBaseNode * focusedNode;

	osg::ref_ptr<osg::Geode> visualSelection;
	osg::ref_ptr<osg::Geode> visualConnection;

	//node events
	osg::ref_ptr<osgWidget::Callback> nodePushEvent;
	osg::ref_ptr<osgWidget::Callback> nodeDragEvent;
	osg::ref_ptr<osgWidget::Callback> nodeReleaseEvent;
	osg::ref_ptr<osgWidget::Callback> nodeEnterEvent;
	osg::ref_ptr<osgWidget::Callback> nodeLeaveEvent;

	osgWidget::XYCoord initialMoveCoords;
	bool moveStarted;
	NODE_PREV_STATUS movePrevNodesStatus;
	VNODES_SET collisionNodes;
	osg::BoundingBox modelBoundingBox;
	osg::BoundingBox selectionBoundingBox;

	//pin events
	osg::ref_ptr<osgWidget::Callback> pinPushEvent;
	osg::ref_ptr<osgWidget::Callback> pinDragEvent;
	osg::ref_ptr<osgWidget::Callback> pinReleaseEvent;
	osg::ref_ptr<osgWidget::Callback> pinEnterEvent;
	osg::ref_ptr<osgWidget::Callback> pinLeaveEvent;

	bool connectingStarted;
	osgWidget::XYCoord connectingCurrentPose;
	osgVDFBasePin* connectingStartPin;
	osgVDFBasePin* connectingCurrentPin;
	PIN_PREV_STATUS connectingPrevPinsStatus;
	osgVDFBasePin::PIN_VISUAL_STATUS conectingPrevStartPinStatus;
	osgVDFBasePin::PIN_VISUAL_STATUS conectingPrevCurrentPinStatus;
	connectionUpdate connectionContinue;

	connectionUpdate connectionStartUpdate;
	connectionUpdate connectionEndUpdate;

	PINS_CONNECTIONS_UPDATE_MAP pinsConnectionsUpdate;

	osg::ref_ptr<osgui::ContextMenu> contextMenu;
	osg::Geode * lastHighlightedConnection;
	bool contextMenuOn;

	std::set<osg::ref_ptr<osgui::Tooltip> > tooltips;
	osg::ref_ptr<osgui::Tooltip> currentNodeTooltip;
	osg::ref_ptr<osgui::Tooltip> currentPinTooltip;

	osg::ref_ptr<osgui::Toolbar> toolbar;
	bool toolbarVisible;
	BUTTON * lastButton;
};


}

#endif