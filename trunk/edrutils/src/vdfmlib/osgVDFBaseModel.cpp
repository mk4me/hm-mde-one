#include "StdAfx.h"

#include <vdfmlib/osgVDFBaseModel.h>
#include <osgui/KeyboardMapperHandler.h>
#include <osgui/Borderized.h>

#include <dfmlib/Connection.h>
#include <dfmlib/DFSourceNode.h>

#include <boost/bind.hpp>
#include <boost/bind.hpp>
#include <vector>
#include <algorithm>
#include <limits>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

////////////////////////////////////////////////////////////////////////////////
namespace osgVDF{
////////////////////////////////////////////////////////////////////////////////

osgVDFBaseModel::osgVDFBaseModel(osgViewer::View* view, osgWidget::point_type width,
	osgWidget::point_type height, unsigned int nodeMask, unsigned int flags)
	: osgWidget::WindowManager(view, width, height, nodeMask, flags),
	currentNodeDescriptor(0), logicalModel(new dflm::DFModel()),
	modelBoundingBox(0,0,0, width, height, 0), connectingStarted(false), connectingCurrentPin(0), focusedNode(0),
	minIntersectRatioToSelect(0.5), maxDistToDelConnection(50), toolbarVisible(false), visualConnectionNormalColor(osgWidget::Color(0,0,0,1)),
	visualConnectionHighlightColor(osgWidget::Color(1,1,0,1)), lastHighlightedConnection(0), contextMenuOn(false), moveStarted(false),
    lastButton(0)//, contextMenu(new osgVDFContextMenu())
	{

	//add event to create nodes
	view->addEventHandler(new UserSpaceClick(this));
	view->addEventHandler(new osgui::KeyboardMapperHandler(this));
	view->addEventHandler(new VisualSelectionManager(this));
	addEventCallback(new ModelResizeHandler(this));

    //addChild(contextMenu);

	nodePushEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeClick, this, osgWidget::EVENT_MOUSE_PUSH);
	nodeDragEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeDrag, this, osgWidget::EVENT_MOUSE_DRAG);
	nodeReleaseEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeRelease, this, osgWidget::EVENT_MOUSE_RELEASE);
	nodeEnterEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeEnter, this, osgWidget::EVENT_MOUSE_ENTER);
	nodeLeaveEvent = new osgWidget::Callback(&osgVDFBaseModel::onNodeLeave, this, osgWidget::EVENT_MOUSE_LEAVE);

	pinPushEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinClick, this, osgWidget::EVENT_MOUSE_PUSH);
	pinDragEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinDrag, this, osgWidget::EVENT_MOUSE_DRAG);
	pinReleaseEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinRelease, this, osgWidget::EVENT_MOUSE_RELEASE);
	pinEnterEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinEnter, this, osgWidget::EVENT_MOUSE_ENTER);
	pinLeaveEvent = new osgWidget::Callback(&osgVDFBaseModel::onPinLeave, this, osgWidget::EVENT_MOUSE_LEAVE);

	connectionStartUpdate = boost::bind(&osgVDFBaseModel::setVisualConnectionStart, this, _1, _2);
	connectionEndUpdate = boost::bind(&osgVDFBaseModel::setVisualConnectionEnd, this, _1, _2);

	selectionActionKeys.insert(osgGA::GUIEventAdapter::KEY_Control_L);
}


osgVDFBaseModel::~osgVDFBaseModel(void)
{
}

dflm::CDFMPtr osgVDFBaseModel::getLogicalModel() const{
	return logicalModel;
}

bool osgVDFBaseModel::isEmptyArea(const osgWidget::XYCoord & posToStart){
	osgWidget::WidgetList widgetList;

	if(!pickAtXY(posToStart.x(), posToStart.y(), widgetList)) return true;
	return false;
}

bool osgVDFBaseModel::addNode(dflm::NPtr node, osg::ref_ptr<osg::Image> image,	const std::string &  name){
	if(logicalModel->addNode(node) == true){

		//create visual node representation
		osg::ref_ptr<osgVDFBaseNode> vnode(createVisualNode(node, image, name));
		
		//create logical node -> visual node assosiation
		nodesLogicalToGraph[node] = vnode;
		graphNodes.insert(vnode);
		
		
		//create logical pins -> visual pins assosiations
		//configure pins
		//build pins compatybility map
		osgVDFBaseNode::VISUAL_PIN_SET nodePins(vnode->getOutPins());
		nodePins.insert(vnode->getInPins().begin(), vnode->getInPins().end());

		PINS_COMPATIBILITY_MAP tmpPinsCompatibility;

		for(osgVDFBaseNode::VISUAL_PIN_SET::const_iterator it = nodePins.begin(); it != nodePins.end(); it++){
			(*it)->setPinVisualStatus((*it)->getStaticPinVisualStatus());
			
			pinsLogicalToGraph[(*it)->getModelPin()] = *it;
			configureVisualPin(*it);
			
			dflm::Pin::PIN_TYPE pinType = (*it)->getModelPin()->getType();

			if(pinType == dflm::Pin::PIN_IN){
				pinsConnectionsUpdate[*it] = connectionEndUpdate;
			}else if(pinType == dflm::Pin::PIN_OUT){
				pinsConnectionsUpdate[*it] = connectionStartUpdate;
			}

			osgVDFBaseNode::VISUAL_PIN_SET compatible;

			for(PINS_COMPATIBILITY_MAP::iterator iT = compatiblePins.begin(); iT != compatiblePins.end(); iT++){

				if(pinType == dflm::Pin::PIN_IN){
					if(iT->first->getModelPin()->getType() == dflm::Pin::PIN_OUT
						&& (*it)->getModelPin()->isCompatible(iT->first->getModelPin())){
							compatible.insert(iT->first);
							iT->second.insert(*it);
					}
				}else if(pinType == dflm::Pin::PIN_OUT){
					if(iT->first->getModelPin()->getType() == dflm::Pin::PIN_IN
						&& iT->first->getModelPin()->isCompatible((*it)->getModelPin())){
							compatible.insert(iT->first);
							iT->second.insert(*it);
					}
				}
			}

			tmpPinsCompatibility[*it] = compatible;

		}

		compatiblePins.insert(tmpPinsCompatibility.begin(), tmpPinsCompatibility.end());

		//configure node - mouse events
		configureVisualNode(vnode);

		//add this representation to WM
		addChild(vnode);

		//hide connections
		hideConnections();

		return true;
	}

	return false;
}

void osgVDFBaseModel::configureVisualNode(osgVDFBaseNode * node){
	node->addEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG | osgWidget::EVENT_MASK_FOCUS | osgWidget::EVENT_MASK_MOUSE_MOVE);

	node->addCallback(nodePushEvent);
	node->addCallback(nodeDragEvent);
	node->addCallback(nodeReleaseEvent);
	node->addCallback(nodeEnterEvent);
	node->addCallback(nodeLeaveEvent);

	node->setNodeVisualStatus(osgVDFBaseNode::NODE_OK);
}

void osgVDFBaseModel::configureVisualPin(osgVDFBasePin * pin){
	pin->addEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG | osgWidget::EVENT_MASK_MOUSE_MOVE);
	
	pin->addCallback(pinPushEvent);
	pin->addCallback(pinDragEvent);
	pin->addCallback(pinReleaseEvent);
	pin->addCallback(pinEnterEvent);
	pin->addCallback(pinLeaveEvent);
	
	pin->setPinVisualStatus(pin->getStaticPinVisualStatus());
}

bool osgVDFBaseModel::deleteNode(dflm::NPtr node){
	return deleteNode(node, false);
}

bool osgVDFBaseModel::deleteNode(dflm::NPtr node, bool clearEvent){
	dflm::Model::NODES_SET inputNodes;

	for(dflm::Node::PINS_SET::iterator it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
		for(dflm::Pin::CONNECTIONS_SET::iterator iT = (*it)->getConnections().begin(); iT != (*it)->getConnections().end(); iT++){
			inputNodes.insert((*iT)->getOther(*it)->getParent());
		}
	}

	if(logicalModel->removeNode(node) == true){
		//remove visual node representation
		deselectNode(node);
		osgVDFBaseNode * vnode = getVisualNode(node);

		//get pins to delete
		osgVDFBaseNode::VISUAL_PIN_SET pinsToRemove(vnode->getInPins());
		pinsToRemove.insert(vnode->getOutPins().begin(),
			vnode->getOutPins().end());

		//get connections to delete
		//get pins to update
		osgVDFBasePin::CONNECTIONS_MAP connectionsToRemove;
		for(osgVDFBaseNode::VISUAL_PIN_SET::const_iterator it = pinsToRemove.begin();
			it != pinsToRemove.end(); it++){

				connectionsToRemove.insert((*it)->getConnections().begin(),
					(*it)->getConnections().end());
		}

		//delete all connections
		//delete connections mappings
		//refresh required pins
		for(osgVDFBasePin::CONNECTIONS_MAP::const_iterator it = connectionsToRemove.begin();
			it != connectionsToRemove.end(); it++){
				it->second->removeConnection(it->first);
				it->second->setPinVisualStatus(it->second->getStaticPinVisualStatus());
				
				//remove connection from connection map
				REV_CONNECTIONS_MAPPING::iterator iT = connectionsGraphToLogical.find(it->first);
				connectionsLogicalToGraph.erase(iT->second);
				connectionsGraphToLogical.erase(iT);

				//remove visual connection from WM
				removeChild(it->first);
		}

		//delete pins mappings
		//delete pins compatibility
		for(osgVDFBaseNode::VISUAL_PIN_SET::const_iterator it = pinsToRemove.begin();
			it != pinsToRemove.end(); it++){

			pinsLogicalToGraph.erase((*it)->getModelPin());
			compatiblePins.erase(*it);
			pinsConnectionsUpdate.erase(*it);

			for(PINS_COMPATIBILITY_MAP::iterator iT = compatiblePins.begin(); iT != compatiblePins.end(); iT++){
				iT->second.erase(*it);
			}
		}

		//delete nodes mappings
		nodesLogicalToGraph.erase(node);
		graphNodes.erase(vnode);

		//refresh output pins of affected nodes
		for(dflm::Model::NODES_SET::iterator it = inputNodes.begin(); it != inputNodes.end(); it++){
			refreshOutputPins(nodesLogicalToGraph[*it]);
		}

		if(clearEvent){
			// wyczyszczenie _lastEvent oraz flag przycisków
			pointerMove(-FLT_MAX, -FLT_MAX);
			// wyczyszczenie _lastPush
			mouseReleasedLeft(0, 0);
		}

		removeChild(vnode);
		hideConnections();
		return true;
	}

	return false;
}

dflm::Model::NODES_SET osgVDFBaseModel::getNodesInArea(const osg::BoundingBox & area){
	dflm::Model::NODES_SET ret;

	for(NODES_MAPPING::const_iterator it = nodesLogicalToGraph.begin(); it != nodesLogicalToGraph.end(); it++){
		osg::BoundingBox nodeBB(osgui::Utils2D::generateBox(it->second));
		if(area.intersects(nodeBB) == true){
			nodeBB = area.intersect(nodeBB);
			osgWidget::point_type intersectArea = (nodeBB.xMax() - nodeBB.xMin()) * (nodeBB.yMax() - nodeBB.yMin());
			osgWidget::point_type nodeArea = it->second->getWidth() * it->second->getHeight();
			if(nodeArea > 0 && intersectArea / nodeArea >= minIntersectRatioToSelect){
				ret.insert(it->first);
			}
		}
	}

	return ret;
}

osgWidget::XYCoord osgVDFBaseModel::getFreePlaceForNode(dflm::NPtr node, const osgWidget::XYCoord & posToStart){
	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return posToStart;
	}
	
	return osgui::Utils2D::findFreeSpaceInNearby(posToStart,vnode->getWidth(), vnode->getHeight(), this);
}

void osgVDFBaseModel::selectNode(dflm::NPtr node){
	if(isNodeSelected(node) == true){
		return;
	}

	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return;
	}

	bool tVis = isTooltipVisible();
	if(tVis == true){
		hideTooltip();
	}

	//remember current node status
	selectionPrevNodesStatus[vnode] = vnode->getNodeVisualStatus();
	//set new node status - active
	vnode->setNodeVisualStatus(osgVDFBaseNode::NODE_ACTIVE);
	vnode->setStrata(osgWidget::Window::STRATA_FOREGROUND);
	//mark node as selected
	selectedNodes.insert(vnode);
	refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::selectNodes(const dflm::Model::NODES_SET & nodes){
	bool tVis = false;
	if(nodes.empty() == false && isTooltipVisible() == true){
		hideTooltip();
		tVis = true;
	}
	
	for(dflm::Model::NODES_SET::const_iterator it = nodes.begin(); it != nodes.end(); it++){
		selectNode(*it);
	}

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::selectNodes(const osg::BoundingBox & rect){
	selectNodes(getNodesInArea(rect));
}

void osgVDFBaseModel::selectAllNodes(){
	selectNodes(logicalModel->getNodes());
}

bool osgVDFBaseModel::isNodeSelected(dflm::NPtr node) const{
	osgVDFBaseNode* vnode = getVisualNode(node);
	if(vnode == 0){
		return false;
	}

	return selectedNodes.find(vnode) != selectedNodes.end();
}

void osgVDFBaseModel::deselectNode(dflm::NPtr node){
	if(isNodeSelected(node) == false){
		return;
	}

	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return;
	}

	bool tVis = isTooltipVisible();
	if(tVis == true){
		hideTooltip();
	}

	//return node its original state before
	vnode->setNodeVisualStatus(selectionPrevNodesStatus[vnode]);

	//remove node entry in state history
	selectionPrevNodesStatus.erase(vnode);

	//mark node as selected
	selectedNodes.erase(vnode);
	vnode->setStrata(osgWidget::Window::STRATA_NONE);
	//setFocused(m_pFocusedNode);

	if(focusedNode == vnode){
		focusedNode = 0;
		if(selectedNodes.empty() == false){
			focusedNode = *(selectedNodes.begin());
		}else{
			focusedNode = *(graphNodes.begin());
		}
	}

	refreshFocused();

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::deselectNodes(const dflm::Model::NODES_SET & nodes){
	bool tVis = false;
	if(nodes.empty() == false && isTooltipVisible() == true){
		hideTooltip();
		tVis = true;
	}
	
	for(dflm::Model::NODES_SET::const_iterator it = nodes.begin(); it != nodes.end(); it++){
		deselectNode(*it);
	}

	if(tVis == true){
		showTooltip();
	}
}

void osgVDFBaseModel::deselectNodes(const osg::BoundingBox & rect){
	deselectNodes(getNodesInArea(rect));
}

void osgVDFBaseModel::deselectAllNodes(){
	for(NODES_MAPPING::const_iterator it = nodesLogicalToGraph.begin(); it != nodesLogicalToGraph.end(); it++){
		deselectNode(it->first);
	}
}

dflm::Model::NODES_SET osgVDFBaseModel::getSelectedNodes() const{
	dflm::Model::NODES_SET ret;

	for(VNODES_SET::iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++){
		ret.insert((*it)->getModelNode());
	}

	return ret;
}

osg::BoundingBox osgVDFBaseModel::getSelectedNodesBoundingBox() const{
	if(selectedNodes.empty() == false){
		VNODES_SET::iterator it = selectedNodes.begin();
		osg::BoundingBox ret(osgui::Utils2D::generateBox(*it));
		it++;
		for( ; it != selectedNodes.end(); it++){
			ret.expandBy(osgui::Utils2D::generateBox(*it));
		}

		return ret;
	}

	return osg::BoundingBox();
}

bool osgVDFBaseModel::isNodeInCollision(dflm::NPtr node) const{
	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return false;
	}
	
	bool ret = false;
	osg::BoundingBox vnodeBB(osgui::Utils2D::generateBox(vnode));
	for(NODES_MAPPING::const_iterator it = nodesLogicalToGraph.begin(); it != nodesLogicalToGraph.end(); it++){
		if(node != it->first && vnodeBB.intersects(osgui::Utils2D::generateBox(it->second)) == true){
			ret = true;
			break;
		}
	}

	return ret;
}

bool osgVDFBaseModel::isNodeInVisibleArea(dflm::NPtr node) const{
	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return false;
	}

	return osg::BoundingBox(0,0,0,this->getWidth(), this->getHeight(), 0).intersects(osgui::Utils2D::generateBox(vnode));
}

bool osgVDFBaseModel::isNodeFullyInVisibleArea(dflm::NPtr node) const{
	if(isNodeInVisibleArea(node) == true){
		osg::BoundingBox vnodeBB(osgui::Utils2D::generateBox(getVisualNode(node)));
		return osg::BoundingBox(0,0,0,this->getWidth(), this->getHeight(), 0).intersect(vnodeBB).radius2() == vnodeBB.radius2();
	}

	return false;
}

void osgVDFBaseModel::hideConnections(){
	osgWidget::point_type z = getMinNodesZ();
	for(REV_CONNECTIONS_MAPPING::iterator it = connectionsGraphToLogical.begin(); it != connectionsGraphToLogical.end(); it++){
		setVisualConnectionZ(it->first, z);
	}
}

osgWidget::XYCoord osgVDFBaseModel::getNodePosition(dflm::NPtr node){
	osgVDFBaseNode * vnode = getVisualNode(node);
	if(vnode == 0){
		return osgWidget::XYCoord();
	}

	return vnode->getAbsoluteOrigin();
}

osgWidget::XYCoord osgVDFBaseModel::getNodeSize(dflm::NPtr node){
	osgVDFBaseNode* vnode = getVisualNode(node);
	if(vnode == 0){
		return osgWidget::XYCoord();
	}
	
	return osgWidget::XYCoord(vnode->getWidth(), vnode->getHeight());
}

void osgVDFBaseModel::moveNode(dflm::NPtr node, const osgWidget::XYCoord & moveStep){
	osgVDFBaseNode* vnode = getVisualNode(node);
	if(vnode == 0){
		return;
	}

	if(isTooltipVisible() == true){
		hideTooltip();
	}

	vnode->addOrigin(moveStep.x(), moveStep.y());
	vnode->resizeAdd(0,0);
}

void osgVDFBaseModel::moveSelectedNodes(const osgWidget::XYCoord & moveStep){
	if(selectedNodes.empty() == false && isTooltipVisible() == true){
		hideTooltip();
	}
	
	for(VNODES_SET::iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++){
		(*it)->addOrigin(moveStep.x(), moveStep.y());
		(*it)->resizeAdd(0,0);
	}

	if(selectedNodes.empty() == false){
		updateCollisions();
	}
}

void osgVDFBaseModel::setNodePosition(dflm::NPtr node, const osgWidget::XYCoord & nodePos){
	osgVDFBaseNode* vnode = getVisualNode(node);
	if(vnode == 0){
		return;
	}

	vnode->setOrigin(nodePos.x(), nodePos.y());
	vnode->resizeAdd(0,0);
}

void osgVDFBaseModel::setSelectedNodesPosition(const osgWidget::XYCoord & nodesPos){
	if(selectedNodes.empty() == false){
		osg::BoundingBox selBB = getSelectedNodesBoundingBox();
		osgWidget::XYCoord move(nodesPos);
		move.x() -= selBB.xMin();
		move.y() -= selBB.yMin();
		moveSelectedNodes(move);
	}
}

dflm::ConnPtr osgVDFBaseModel::connect(dflm::PinPtr src, dflm::PinPtr dest){
	dflm::ConnPtr ret(logicalModel->connect(src,dest));
	if(ret == 0){
		return ret;
	}

	osgVDFBasePin * vsrc = pinsLogicalToGraph[src];
	osgVDFBasePin * vdest = pinsLogicalToGraph[dest];

	//create visual connection
	osg::ref_ptr<osg::Geode> vconnection = createVisualConnection(vsrc, vdest);
	setVisualConnectionZ(vconnection, getMinNodesZ());
	setVisualConnectionColor(vconnection, visualConnectionNormalColor);
	
	//update visual pins
	addConnectionToPin(vsrc, vconnection, vdest);
	addConnectionToPin(vdest, vconnection, vsrc);

	//update output pins of destination node
	refreshOutputPins(vdest->getParentNode());

	//update mappings
	connectionsLogicalToGraph[ret] = vconnection;
	connectionsGraphToLogical[vconnection] = ret;

	//update view
	addChild(vconnection);

	return ret;
}

bool osgVDFBaseModel::disconnect(dflm::ConnPtr connection){	
	if(logicalModel->disconnect(connection) == false){
		return false;
	}

	osg::Geode * vconnection = connectionsLogicalToGraph[connection];

	//update connection pins
	osgVDFBasePin* vpin = pinsLogicalToGraph[connection->getSrc()];
	removeConnectionFromPin(vpin,vconnection);
	vpin = pinsLogicalToGraph[connection->getDest()];
	removeConnectionFromPin(vpin,vconnection);

	//update output pins of destination node
	refreshOutputPins(vpin->getParentNode());

	//remove visual connection
	connectionsLogicalToGraph.erase(connection);
	connectionsGraphToLogical.erase(vconnection);
	removeChild(vconnection);

	return true;
}

osgWidget::point_type osgVDFBaseModel::getAreaRatioToSelect() const{
	return minIntersectRatioToSelect;
}

void osgVDFBaseModel::setAreaRatioToSelect(osgWidget::point_type areaRatio){
	minIntersectRatioToSelect = areaRatio;
}

osgWidget::point_type osgVDFBaseModel::getMinDistToDelConnection() const {
	return maxDistToDelConnection;
}

void osgVDFBaseModel::setMinDistToDelConnection(osgWidget::point_type dist) {
	maxDistToDelConnection = dist;
}

const osgui::KeyboardMapper::Keys & osgVDFBaseModel::getSelectionActionKeys() const{
	return selectionActionKeys;
}

void osgVDFBaseModel::setSelectionActionKeys(const osgui::KeyboardMapper::Keys & keys){
	selectionActionKeys = keys;
}

bool osgVDFBaseModel::deleteNodeSelf(osgVDFBaseNode * node){
	return deleteNode(node->getModelNode(), true);
}

void osgVDFBaseModel::refreshPinVisualState(osgVDFBasePin * pin){
	pin->setPinVisualStatus(pin->getStaticPinVisualStatus());
}

void osgVDFBaseModel::addConnectionToPin(osgVDFBasePin * pin, osg::Geode * connection, osgVDFBasePin * other){
	pin->addConnection(connection, other);
	refreshPinVisualState(pin);
}

void osgVDFBaseModel::removeConnectionFromPin(osgVDFBasePin * pin, osg::Geode * connection){
	pin->removeConnection(connection);
	refreshPinVisualState(pin);
}

void osgVDFBaseModel::refreshOutputPins(osgVDFBaseNode * node){
	for(osgVDFBaseNode::VISUAL_PIN_SET::iterator it = node->getOutPins().begin(); it != node->getOutPins().end(); it++){
		//refresh pin visual status
		refreshPinVisualState(*it);
	}
}

void osgVDFBaseModel::showDefaultToolbar(bool show){
	if(toolbarVisible == show){
		return;
	}

	toolbarVisible = show;

	if(toolbarVisible == true){
		if(toolbar == 0){
			toolbar = new osgui::Toolbar();
			this->addChild(toolbar);
			fillToolbarWithGroups(toolbar);
			for(std::set<osgVDFNodeTypeDescriptor*>::iterator it = nodeTypesMissing.begin();
				it != nodeTypesMissing.end(); it++){

				graphAddNodeTypeToToolbar(*it);
			}

			nodeTypesMissing.swap(std::set<osgVDFNodeTypeDescriptor*>());
		}

		//m_pToolbar->setOrigin(0,this->getHeight() - m_pToolbar->getHeight());
		toolbar->setOrigin(0,0);
		toolbar->resize(this->getWidth(), toolbar->getHeight());
		toolbar->update();
		//TODO
		//if some collision with nodes - move nodes down to avoid collision
		toolbar->show();

	}else{
		toolbar->hide();
	}
}

bool osgVDFBaseModel::registerNodeType(osgVDFNodeTypeDescriptor * descriptor){
	if(nodeTypesToControls.find(descriptor) != nodeTypesToControls.end()){
		return true;
	}

	bool ret = true;
	if(toolbar != 0){
		ret = graphAddNodeTypeToToolbar(descriptor);
	}else{
		nodeTypesToControls[descriptor] = 0;
		nodeTypesMissing.insert(descriptor);
	}

	return ret;
}

bool osgVDFBaseModel::graphAddNodeTypeToToolbar(osgVDFNodeTypeDescriptor* nodeTypeDescriptor){
	int tab = -1;
	switch(nodeTypeDescriptor->getNodeType()){
	case osgVDFBaseNode::NODE_ANALYZE:
		tab = 1;
		break;
	case osgVDFBaseNode::NODE_PROCESS:
		tab = 2;
		break;
	case osgVDFBaseNode::NODE_SOURCE:
		tab = 0;
		break;
	case osgVDFBaseNode::NODE_VISUALIZE:
		tab = 3;
		break;
	}

	if(tab == -1){
		return false;
	}

	BUTTON * newLabel = new BUTTON(nodeTypeDescriptor->getButtonText(),"");
	newLabel->setToggleEnabled(true);
	newLabel->setToggled(false);

	newLabel->setNormalStyle("osgVDF.toolbar.button.normal");
	newLabel->setHoverStyle("osgVDF.toolbar.button.hover");
	newLabel->setToggleStyle("osgVDF.toolbar.button.toggle");
	newLabel->setPushedStyle("osgVDF.toolbar.button.pushed");


	newLabel->addCallback(new osgWidget::Callback(&osgVDFBaseModel::onNodeDescriptorPush, this, osgWidget::EVENT_MOUSE_PUSH, newLabel));
	newLabel->setEventMask(osgWidget::EVENT_MASK_MOUSE_CLICK);

	newLabel->setCanFill(false);
	controlsToNodeTypes[newLabel] = nodeTypeDescriptor;
	nodeTypesToControls[nodeTypeDescriptor] = newLabel;
	if(nodeTypeDescriptor->getButtonImage() != nullptr){
		newLabel->setColor(osgWidget::Color(1,1,1,1));
		newLabel->setSize(nodeTypeDescriptor->getButtonImage()->s(), nodeTypeDescriptor->getButtonImage()->t());
		newLabel->setImage(nodeTypeDescriptor->getButtonImage(),true, true);

		osgui::Tooltip * tooltip = new osgui::Tooltip();
        addChild(tooltip);
		tooltip->setText(nodeTypeDescriptor->getButtonText());
		tooltip->registerItem(newLabel);
		tooltips.insert(tooltip);
	}else{
		newLabel->setLabel(nodeTypeDescriptor->getButtonText());
		newLabel->setSize(0,0);
		newLabel->setFontSize(13);
	}

	toolbar->addTabElement(tab, newLabel);

	return true;
}

osgVDFBaseNode* osgVDFBaseModel::getVisualNode(dflm::NPtr node) const{
	NODES_MAPPING::const_iterator it = nodesLogicalToGraph.find(node);
	if(it != nodesLogicalToGraph.end()){
		return it->second;
	}

	return 0;
}

void osgVDFBaseModel::fillToolbarWithGroups(osgui::Toolbar * toolbar){
	if(toolbar == 0){
		return;
	}

	osgWidget::Label * label = new osgui::Borderized<osgWidget::Label>("sourceTab");
	toolbar->addTab(label);
    label->setLabel("Source");

	label = new osgui::Borderized<osgWidget::Label>("analysisTab");
	toolbar->addTab(label);
    label->setLabel("Analysis");

	label = new osgui::Borderized<osgWidget::Label>("processingTab");
	toolbar->addTab(label);
    label->setLabel("Processing");

	label = new osgui::Borderized<osgWidget::Label>("visualizeTab");
	toolbar->addTab(label);
    label->setLabel("Visualize");

    toolbar->resize();
}

osgWidget::point_type osgVDFBaseModel::getMinNodesZ(){
	osgWidget::point_type ret = 0;

	if(graphNodes.empty() == false){
		VNODES_SET::iterator it = graphNodes.begin();
		ret = osgui::Utils2D::calcAbsZ(*it);
		it++;
		for( ; it != graphNodes.end(); it++){
			ret = std::min(ret,osgui::Utils2D::calcAbsZ(*it));
		}
	}

	return ret;

}

osgWidget::point_type osgVDFBaseModel::getMaxNodesZ(){
	osgWidget::point_type ret = 0;

	if(graphNodes.empty() == false){
		VNODES_SET::iterator it = graphNodes.begin();
		ret = osgui::Utils2D::calcAbsZ(*it);
		it++;
		for( ; it != graphNodes.end(); it++){
			ret = std::max(ret,osgui::Utils2D::calcAbsZ(*it));
		}
	}

	return ret;

}

bool osgVDFBaseModel::onNodeDescriptorPush(osgWidget::Event& ev){
	currentNodeDescriptor = controlsToNodeTypes[ev.getWidget()];
	lastButton = (BUTTON*)ev.getWidget();
	return false;
}

osgVDFBaseModel::VNODES_SET osgVDFBaseModel::getSelectedNodesCollisions(){
	VNODES_SET ret;

	std::vector<osgVDFBaseNode*> diff(nodesLogicalToGraph.size() - selectedNodes.size());
	std::set_difference(graphNodes.begin(), graphNodes.end(), selectedNodes.begin(), selectedNodes.end(), diff.begin());

	for(VNODES_SET::const_iterator it = selectedNodes.begin(); it != selectedNodes.end(); it++){
		osg::BoundingBox selBB = osgui::Utils2D::generateBox(*it);
		for(std::vector<osgVDFBaseNode*>::iterator iT = diff.begin(); iT != diff.end(); iT++){
			if(selBB.intersects(osgui::Utils2D::generateBox(*iT)) == true){
				ret.insert(*iT);
			}
		}
	}

	return ret;
}

void osgVDFBaseModel::updateCollisions(){
	//process collisions

	//get current collisions
	VNODES_SET collisions = getSelectedNodesCollisions();

	//if collisions still exist
	if(collisions.empty() == false){

		//create difference between previous collision list and current collision list
		std::vector<osgVDFBaseNode*> difference(collisionNodes.size());
		std::vector<osgVDFBaseNode*>::iterator diffEnd = std::set_difference(collisionNodes.begin(),
			collisionNodes.end(), collisions.begin(), collisions.end(), difference.begin());

		//return nodes previous states if they are not in collision any more
		for(std::vector<osgVDFBaseNode*>::iterator it = difference.begin(); it != diffEnd; it++){
			(*it)->setNodeVisualStatus(movePrevNodesStatus[*it]);
			movePrevNodesStatus.erase(*it);
		}

		//fill new collision points
		for(VNODES_SET::iterator it = collisions.begin(); it != collisions.end(); it++){
			if(movePrevNodesStatus.find(*it) == movePrevNodesStatus.end()){
				movePrevNodesStatus[*it] = (*it)->getNodeVisualStatus();
				(*it)->setNodeVisualStatus(osgVDFBaseNode::NODE_COLLISION);
				collisionNodes.insert(*it);
			}
		}
	}else{
		for(NODE_PREV_STATUS::iterator it = movePrevNodesStatus.begin();
			it != movePrevNodesStatus.end(); it++){

				it->first->setNodeVisualStatus(it->second);
		}
		collisionNodes.swap(VNODES_SET());
		movePrevNodesStatus.swap(NODE_PREV_STATUS());
	}
}

bool osgVDFBaseModel::onNodeClick(osgWidget::Event& ev){
	
	if(ev.getWindowManager()->isLeftMouseButtonDown() == false){
		return false;
	}
	
	osgVDFBaseNode * currentNode = dynamic_cast<osgVDFBaseNode*>(ev.getWindow());
	
	focusedNode = currentNode;

	moveStarted = true;
	dflm::NPtr node = currentNode->getModelNode();
	bool selActionKeysPressed = areKeysPressed(selectionActionKeys);
	if(isNodeSelected(node) == true){
		if(selActionKeysPressed == true){
			moveStarted = false;
			deselectNode(node);
		}
	}else{
		if(selActionKeysPressed == true){
			selectNode(node);
		}else{
			deselectAllNodes();
			selectNode(node);
		}
	}

	if(moveStarted == true){
		selectionBoundingBox = getSelectedNodesBoundingBox();

		//remember initial position in case of collision and revert position of selection
		initialMoveCoords.x() = selectionBoundingBox.xMin();
		initialMoveCoords.y() = selectionBoundingBox.yMin();

		updateCollisions();
	}

	return false;
}

bool osgVDFBaseModel::onNodeDrag(osgWidget::Event& ev){

	if(moveStarted == true){
		osgWidget::XYCoord move(ev.x, ev.y);
		osg::BoundingBox selBB(selectionBoundingBox);
		selBB = osgui::Utils2D::translateBoundingBox(selBB, move);
		if(selBB.intersect(modelBoundingBox).radius2() == selBB.radius2() && (toolbarVisible == false || (toolbarVisible == true && osgui::Utils2D::generateBox(toolbar).intersects(selBB) == false))){
			moveSelectedNodes(move);
			selectionBoundingBox = selBB;
		}
	}

	return false;
}

bool osgVDFBaseModel::onNodeRelease(osgWidget::Event& ev){

	if(moveStarted == true){
		if(collisionNodes.empty() == false){
			//return to original position
			setSelectedNodesPosition(osgWidget::XYCoord(initialMoveCoords));
		}
		moveStarted = false;
	}

	return false;
}

bool osgVDFBaseModel::onPinClick(osgWidget::Event& ev){

	//event pin
	if(ev.getWindowManager()->isLeftMouseButtonDown() == false){
		return false;
	}

	connectingStartPin = dynamic_cast<osgVDFBasePin*>(ev.getWidget());

	if(connectingStarted == false && (connectingStartPin->getModelPin()->getType() == dflm::Pin::PIN_OUT ||
		connectingStartPin->getModelPin()->getConnections().empty() == true)){
			//mark that connecting has started
			connectingStarted = true;

			if(isTooltipVisible() == true){
				hideTooltip();
			}

			//get mouse pose to track it later with onPinDrag or onPinMove action
			connectingCurrentPose.x() = ev.x;
			connectingCurrentPose.y() = ev.y;

			//highlight its compatible pins
			osgVDFBaseNode::VISUAL_PIN_SET & cPins = compatiblePins[connectingStartPin];
			savePinsStatus(cPins);
			highlightCompatiblePins(connectingStartPin, cPins);

			//create graphical connection representation

			visualConnection = startVisualConnection(connectingStartPin);
			setVisualConnectionZ(visualConnection, 0);
			setVisualConnectionColor(visualConnection, visualConnectionHighlightColor);

			if(connectingStartPin->getModelPin()->getType() == dflm::Pin::PIN_IN){
				connectionContinue = connectionStartUpdate;
			}else if(connectingStartPin->getModelPin()->getType() == dflm::Pin::PIN_OUT){
				connectionContinue = connectionEndUpdate;
			}

			setVisualConnectionStart(visualConnection, connectingStartPin->getCenterPosition());
			setVisualConnectionEnd(visualConnection, connectingStartPin->getCenterPosition());

			conectingPrevStartPinStatus = connectingStartPin->getPinVisualStatus();
			connectingStartPin->setPinVisualStatus(osgVDFBasePin::PIN_CONNECTED);

			addChild(visualConnection);
			//setFocused(m_pFocusedNode);
			refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onPinDrag(osgWidget::Event& ev){
	if(connectingStarted == true){
		//update graphical connection shape and position
		connectingCurrentPose.x() += ev.x;
		connectingCurrentPose.y() += ev.y;

		//update graphical connection
		connectionContinue(visualConnection, connectingCurrentPose);

		//check if we are over some pin!!
		//if yes highlight it properly and save its pointer
		//else reset destination pin, reset its state
		osg::Vec3f pos3D(connectingCurrentPose.x(), connectingCurrentPose.y(),0);
		bool isPin = false;
		for(PINS_COMPATIBILITY_MAP::const_iterator it = compatiblePins.begin();
			it != compatiblePins.end(); it++){

				//onEnter
				if(it->first != connectingStartPin && osgui::Utils2D::generateBox(it->first).contains(pos3D) == true){
					isPin = true;
					if(connectingCurrentPin != it->first){
						if(connectingCurrentPin != 0){
							//reset status
							connectingCurrentPin->setPinVisualStatus(conectingPrevCurrentPinStatus);
						}

						connectingCurrentPin = it->first;
						conectingPrevCurrentPinStatus = connectingCurrentPin->getPinVisualStatus();
						osgVDFBasePin::PIN_VISUAL_STATUS pinNewStatus = osgVDFBasePin::PIN_CONNECTED;

						if(connectingPrevPinsStatus.find(connectingCurrentPin) == connectingPrevPinsStatus.end() ||
							connectingCurrentPin->getDynamicPinVisualStatus(connectingStartPin,logicalModel) == osgVDFBasePin::PIN_INCOMPATIBLE){
								pinNewStatus = osgVDFBasePin::PIN_INCOMPATIBLE;
						}

						connectingCurrentPin->setPinVisualStatus(pinNewStatus);
					}

					break;
				}
		}

		//on leave current pin
		if(isPin == false && connectingCurrentPin != 0){
			connectingCurrentPin->setPinVisualStatus(conectingPrevCurrentPinStatus);
			connectingCurrentPin = 0;
		}
	}

	return false;
}

bool osgVDFBaseModel::onPinRelease(osgWidget::Event& ev){

	if(connectingStarted == true){
		restorePinsStatus();
		removeChild(visualConnection);
		if(connectingCurrentPin != 0){
			if(connectingStartPin->getModelPin()->getType() == dflm::Pin::PIN_IN){
				std::swap(connectingStartPin, connectingCurrentPin);
				std::swap(conectingPrevStartPinStatus, conectingPrevCurrentPinStatus);
			}

			//if pins can be connected connect them and reset connecting
			if(connect(connectingStartPin->getModelPin(), connectingCurrentPin->getModelPin()) == 0)
			{
				connectingCurrentPin->setPinVisualStatus(conectingPrevCurrentPinStatus);
				connectingStartPin->setPinVisualStatus(conectingPrevStartPinStatus);
			}
		}else{
			connectingStartPin->setPinVisualStatus(conectingPrevStartPinStatus);
		}

		//setFocused(m_pFocusedNode);
		refreshFocused();

		//reset connecting
		connectingStarted = false;
		visualConnection = 0;
		connectingStartPin = connectingCurrentPin = 0;
		connectionContinue = connectionUpdate();
	}

	return false;
}

void osgVDFBaseModel::savePinsStatus(const osgVDFBaseNode::VISUAL_PIN_SET & pins){
	for(osgVDFBaseNode::VISUAL_PIN_SET::const_iterator it = pins.begin(); it != pins.end(); it++){
		connectingPrevPinsStatus[*it] = (*it)->getPinVisualStatus();
	}
}

void osgVDFBaseModel::highlightCompatiblePins(osgVDFBasePin * startPin, const osgVDFBaseNode::VISUAL_PIN_SET & pins){
	for(osgVDFBaseNode::VISUAL_PIN_SET::const_iterator it = pins.begin(); it != pins.end(); it++){
		osgVDFBasePin::PIN_VISUAL_STATUS pinStatus = (*it)->getDynamicPinVisualStatus(startPin->getModelPin(), logicalModel);
		if(pinStatus == osgVDFBasePin::PIN_ACTIVE){
			(*it)->setPinVisualStatus(pinStatus);
		}
	}
}

void osgVDFBaseModel::restorePinsStatus(){
	for(PIN_PREV_STATUS::iterator it = connectingPrevPinsStatus.begin(); it != connectingPrevPinsStatus.end(); it++){
		it->first->setPinVisualStatus(it->second);
	}

	connectingPrevPinsStatus.swap(PIN_PREV_STATUS());
}

void osgVDFBaseModel::updatePinsConnections(osgVDFBasePin * pin){
	connectionUpdate connUp(pinsConnectionsUpdate[pin]);
	osgWidget::XYCoord pos = pin->getCenterPosition();
	for(osgVDFBasePin::CONNECTIONS_MAP::const_iterator it = pin->getConnections().begin(); it != pin->getConnections().end(); it++){
		connUp(it->first, pos);
	}
}

bool osgVDFBaseModel::onPinEnter(osgWidget::Event& ev){
	
	if(connectingStarted == true){
		return false;
	}

	osgVDFBasePin * pin = dynamic_cast<osgVDFBasePin *>(ev.getWidget());

	std::vector<std::string> pinErrors = getPinErrors(pin->getModelPin());

	if(pinErrors.empty() == false){
		hideTooltip();

		std::vector<std::string>::iterator it = pinErrors.begin();
		std::string text(*it);

		it++;

		for( ; it != pinErrors.end(); it++){
			text += "\n" + *it;
		}

		setTooltipText(text);

		osgWidget::XYCoord pos = pin->getParent()->getAbsoluteOrigin() + pin->getOrigin();
		pos.x() += pin->getWidth();
		pos.y() += pin->getHeight();
				
		showTooltip(pos);
		refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onPinLeave(osgWidget::Event& ev){

	if(isTooltipVisible() == true){

		hideTooltip();
		refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onNodeEnter(osgWidget::Event& ev){
	osgVDFBaseNode * node = dynamic_cast<osgVDFBaseNode *>(ev.getWindow());
	std::vector<std::string> nodeErrors = getNodeErrors(node->getModelNode());
	if(nodeErrors.empty() == false){
		hideTooltip();

		std::vector<std::string>::iterator it = nodeErrors.begin();
		std::string text(*it);

		it++;

		for( ; it != nodeErrors.end(); it++){
			text += "\n" + *it;
		}

		setTooltipText(text);

		osgWidget::XYCoord pos = node->getAbsoluteOrigin();
		pos.x() += node->getWidth();
		pos.y() += node->getHeight();
		showTooltip(pos);
		//refreshFocused();
	}

	return false;
}

bool osgVDFBaseModel::onNodeLeave(osgWidget::Event& ev){
	if(isTooltipVisible() == true){
		hideTooltip();
		refreshFocused();
	}

	return false;
}

std::vector<std::string> osgVDFBaseModel::getPinErrors(dflm::PinPtr pin){
	std::vector<std::string> ret;

	if(pin->getType() == dflm::Pin::PIN_IN){
		if(pin->isRequired() == true && pin->getConnections().empty() == true){
			ret.push_back("Pin wymagany - musi byæ pod³¹czony!");
		}
	}else if(pin->getType() == dflm::Pin::PIN_OUT && pin->getConnections().empty() == false && pin->isComplete() == false){
		std::string message("Pin wyjsciowy jest polaczony i zale¿ny od pinów wejœciowych:");
		dflm::Pin::REQ_PINS_SET dependant = pin->getDependantPins();
		for(dflm::Pin::REQ_PINS_SET::iterator it = dependant.begin(); it != dependant.end(); it++){
			if((*it).lock()->getConnections().empty() == true){
				message += " " + (*it).lock()->getPinName();
			}
		}

		message += ". Podlacz te piny!";
		ret.push_back(message);
	}

	return ret;
}

std::vector<std::string> osgVDFBaseModel::getNodeErrors(dflm::NPtr node){
	std::vector<std::string> ret;
	
	//if(dynamic_cast<chart::DFSourceNode*>(node.get()) == 0){
	if(boost::dynamic_pointer_cast<dflm::DFSourceNode>(node) == 0){
		if(node->getInPins().empty() == false){
			if(node->anyInPinConnected() == false){
				ret.push_back("Wezel musi miec podlaczony minimum jeden pin wejsciowy");
			}

			const dflm::Node::PINS_SET & pinsSet = node->getInPins();
			for(dflm::Node::PINS_SET::const_iterator it = pinsSet.begin(); it != pinsSet.end(); it++){
				if(getPinErrors(*it).empty() == false){
					ret.push_back("Problem z pinami wejsciowymi - sprawdz piny wejsciowe");
					break;
				}
			}
		}
		
		if(node->getOutPins().empty() == false){
			const dflm::Node::PINS_SET & pinsSet = node->getOutPins();
			for(dflm::Node::PINS_SET::const_iterator it = pinsSet.begin(); it != pinsSet.end(); it++){
				if(getPinErrors(*it).empty() == false){
					ret.push_back("Problem z pinami wyjsciowymi - sprawdz piny wyjsciowe");
					break;
				}
			}
		}
	}

	return ret;
}

osgVDFBaseModel::VisualSelectionManager::VisualSelectionManager(osgVDFBaseModel * m) : model(m), visualSelectionStarted(false){

}

osgVDFBaseModel::VisualSelectionManager::~VisualSelectionManager(){

}

bool osgVDFBaseModel::VisualSelectionManager::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){
	
		//do this if the descriptor is set
		//check if mouse was pushed - left button!!

		if(visualSelectionStarted == false && model->isEmptyArea(osgWidget::XYCoord(gea.getX(), gea.getY())) == false){
			return false;
		}

		model->focusedNode = 0;

		osgGA::GUIEventAdapter::EventType e = gea.getEventType();
		switch(e){
		case osgGA::GUIEventAdapter::PUSH:
			visualSelectionStarted = gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ? true : false;
			if(visualSelectionStarted == true){
				//reset previous selection
				if(model->areKeysPressed(model->selectionActionKeys) == false){
					model->deselectAllNodes();
				}

				//prepare first frame position
				model->selectionStartPose = osgWidget::XYCoord(gea.getX(), gea.getY());
				model->visualSelection = model->createVisualSelection(model->selectionStartPose);
				model->addChild(model->visualSelection);
			}
			break;

		case osgGA::GUIEventAdapter::DRAG:
			if(visualSelectionStarted == true){
				//continue with frame
				model->continueVisulaSelection(model->visualSelection, osgWidget::XYCoord(gea.getX(), gea.getY()));
				
				osg::BoundingBox locBB(std::min(gea.getX(), model->selectionStartPose.x()),
					std::min(gea.getY(), model->selectionStartPose.y()), 0,
					std::max(gea.getX(), model->selectionStartPose.x()),
					std::max(gea.getY(), model->selectionStartPose.y()), 0);

				dflm::Model::NODES_SET currentlySelectedNodes =  model->getNodesInArea(locBB);

				VNODES_SET cs;
				for(dflm::Model::NODES_SET::iterator it = currentlySelectedNodes.begin(); it != currentlySelectedNodes.end(); it++){
					cs.insert(model->nodesLogicalToGraph[*it]);
					model->selectNode(*it);
				}

				std::vector<osgVDFBaseNode*> diff(model->selectedNodes.size());
				std::vector<osgVDFBaseNode*>::iterator endDiff = std::set_difference(model->selectedNodes.begin(), model->selectedNodes.end(),
					cs.begin(), cs.end(), diff.begin());

				//deselect missing
				for(std::vector<osgVDFBaseNode*>::iterator it = diff.begin(); it != endDiff; it++){
					model->deselectNode((*it)->getModelNode());
				}
			}

			break;

		case osgGA::GUIEventAdapter::RELEASE:
			if(visualSelectionStarted == true){
				visualSelectionStarted = false;

				//m_pModel->pointerMove(-FLT_MAX, -FLT_MAX);
				// wyczyszczenie _lastPush
				//m_pModel->mouseReleasedLeft(0, 0);

				//remove selection frame
				model->removeChild(model->visualSelection);
				model->visualSelection = 0;
			}
			break;
		}

		return false;
}

osgVDFBaseModel::ModelResizeHandler::ModelResizeHandler(osgVDFBaseModel * m) : model(m){

}

osgVDFBaseModel::ModelResizeHandler::~ModelResizeHandler(){

}

bool osgVDFBaseModel::ModelResizeHandler::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

	if(gea.getEventType() == osgGA::GUIEventAdapter::RESIZE){
		model->modelBoundingBox.xMax() = gea.getWindowWidth();
		model->modelBoundingBox.yMax() = gea.getWindowHeight();
		
		if(model->toolbar != 0){
			//resize toolbar to fit width, keep height
			model->toolbar->resize(gea.getWindowWidth(), model->toolbar->getHeight());
		}
	}

	return false;
}


osgVDFBaseModel::UserSpaceClick::UserSpaceClick(osgVDFBaseModel * m) : model(m){

}

osgVDFBaseModel::UserSpaceClick::~UserSpaceClick(){

}

bool osgVDFBaseModel::UserSpaceClick::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

        if(model->contextMenuOn == true && gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON){
            model->contextMenu->hideMenu();
        }

	//do this if the descriptor is set
	//check if mouse was pushed - left button!!
	if(model->lastButton != 0 && model->lastButton->isToggled() == true && model->currentNodeDescriptor != 0 &&
		gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
		model->isEmptyArea(osgWidget::XYCoord(gea.getX(), gea.getY())) == true){

		static int nodeCounter = 0;

		if(model->toolbarVisible == true && model->modelBoundingBox.contains(osg::Vec3f(gea.getX(), gea.getY(), 0)) == false){
			//jesli klik w toolbar to nic nie robimy!!
			return false;
		}

		dflm::NPtr node = model->currentNodeDescriptor->createNode();
		std::stringstream ss;
		ss << nodeCounter++;


		//to ensure unique names for nodes!! important for connections unique naming!!
		node->setNodeName(ss.str() + node->getNodeName());
		model->addNode(node, 0, node->getNodeName());
		model->focusedNode = model->getVisualNode(node);
		osgWidget::XYCoord nSize = model->getNodeSize(node);
		model->setNodePosition(node, osgWidget::XYCoord(gea.getX() - nSize.x() / 2.0, gea.getY() - nSize.y() / 2.0));
	}else if(model->contextMenuOn == false && gea.getEventType() == osgGA::GUIEventAdapter::PUSH && gea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON){
		//collect connections in nearby to delete
		osgWidget::XYCoord pos(gea.getX(), gea.getY());
		//go through all points in all connections and identify the clossest one to delete
		CONNECTIONS_DELETE_MAP toDelete;
		
		for(osgVDFBaseModel::REV_CONNECTIONS_MAPPING::iterator it = model->connectionsGraphToLogical.begin();
			it != model->connectionsGraphToLogical.end(); it++){

			osg::Geometry * geom = dynamic_cast<osg::Geometry*>(it->first->getDrawable(0));
			osg::Vec3Array * vertexData = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
			bool inserted = false;
			float minDist = std::numeric_limits<float>::max();
			unsigned int idx = 0;
			for(unsigned int i = 0; i < vertexData->size(); i++){
				float distLoc = std::pow(pos.x() - (*vertexData)[i][0],2) + std::pow(pos.y() - (*vertexData)[i][1],2);
				if(distLoc <= model->maxDistToDelConnection){
					//add to delete map
					toDelete[it->second->getConnectionName()] = it->first;
					inserted = true;
					break;
				}else if(distLoc < minDist){
					idx = i;
					minDist = distLoc;
				}
			}

			if(inserted == false && vertexData->size() > 1){
				double lineDist = std::numeric_limits<double>::max();
				double segDist = std::numeric_limits<double>::max();
				//use closest found element to interpolate
				if(idx == 0){
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[1][0], (*vertexData)[1][1], segDist, lineDist);
				}else if(idx == vertexData->size()-1){
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx-1][0], (*vertexData)[idx-1][1], segDist, lineDist);
				}else{
					double tmpSegDist = std::numeric_limits<double>::max();
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx-1][0], (*vertexData)[idx-1][1], tmpSegDist, lineDist);
					DistanceFromLine(gea.getX(), gea.getY(), (*vertexData)[idx][0], (*vertexData)[idx][1], (*vertexData)[idx+1][0], (*vertexData)[idx+1][1], segDist, lineDist);
					segDist = std::min(segDist, tmpSegDist);
				}

				if(segDist <= model->maxDistToDelConnection){
					toDelete[it->second->getConnectionName()] = it->first;
				}
			}
		}

		if(toDelete.empty() == false){
			//show menu context

            //model->contextMenu->clearMenu();
            if(model->contextMenu != nullptr){
                model->contextMenu->hideMenu();
                model->removeChild(model->contextMenu);
                model->contextMenu = nullptr;
            }

			model->showConnectionsDeleteContextMenu(toDelete, osgWidget::XYCoord(gea.getX(), gea.getY()));
			model->contextMenuOn = true;
		}
	}

	return false;
}

void osgVDFBaseModel::UserSpaceClick::DistanceFromLine(double cx, double cy, double ax, double ay ,
					  double bx, double by, double &distanceSegment,
					  double &distanceLine)
{

	//
	// find the distance from the point (cx,cy) to the line
	// determined by the points (ax,ay) and (bx,by)
	//
	// distanceSegment = distance from the point to the line segment
	// distanceLine = distance from the point to the line (assuming
	//					infinite extent in both directions
	//

/*

Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).
    Let P be the point of perpendicular projection of C on AB.  The parameter
    r, which indicates P's position along AB, is computed by the dot product 
    of AC and AB divided by the square of the length of AB:
    
    (1)     AC dot AB
        r = ---------  
            ||AB||^2
    
    r has the following meaning:
    
        r=0      P = A
        r=1      P = B
        r<0      P is on the backward extension of AB
        r>1      P is on the forward extension of AB
        0<r<1    P is interior to AB
    
    The length of a line segment in d dimensions, AB is computed by:
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 + ... + (Bd-Ad)^2)

    so in 2D:   
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 )
    
    and the dot product of two vectors in d dimensions, U dot V is computed:
    
        D = (Ux * Vx) + (Uy * Vy) + ... + (Ud * Vd)
    
    so in 2D:   
    
        D = (Ux * Vx) + (Uy * Vy) 
    
    So (1) expands to:
    
            (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
        r = -------------------------------
                          L^2

    The point P can then be found:

        Px = Ax + r(Bx-Ax)
        Py = Ay + r(By-Ay)

    And the distance from A to P = r*L.

    Use another parameter s to indicate the location along PC, with the 
    following meaning:
           s<0      C is left of AB
           s>0      C is right of AB
           s=0      C is on AB

    Compute s as follows:

            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------
                        L^2


    Then the distance from C to P = |s|*L.

*/


	double r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
	double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
	double r = r_numerator / r_denomenator;
//
    double px = ax + r*(bx-ax);
    double py = ay + r*(by-ay);
//     
    double s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

	distanceLine = fabs(s)*sqrt(r_denomenator);

//
// (xx,yy) is the point on the lineSegment closest to (cx,cy)
//
	double xx = px;
	double yy = py;

	if ( (r >= 0) && (r <= 1) )
	{
		distanceSegment = distanceLine;
	}
	else
	{

		double dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
		double dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
		if (dist1 < dist2)
		{
			xx = ax;
			yy = ay;
			distanceSegment = sqrt(dist1);
		}
		else
		{
			xx = bx;
			yy = by;
			distanceSegment = sqrt(dist2);
		}


	}

	return;
}

void osgVDFBaseModel::showConnectionsDeleteContextMenu(const osgVDFBaseModel::CONNECTIONS_DELETE_MAP & toDelete, const osgWidget::XYCoord & pos){
	
    contextMenu = new osgui::ContextMenu();
    contextMenu->setMenuOnCloseCallback(std::string(), boost::bind(&osgVDFBaseModel::closeContextMenu, this, _1));
    
    for(CONNECTIONS_DELETE_MAP::const_iterator it = toDelete.begin(); it != toDelete.end(); it++){
		osgui::ContextMenu::OnClickCallback cc = boost::bind(&osgVDFBaseModel::removeConnection, this, connectionsGraphToLogical[it->second], _1, _2);
		osgui::ContextMenu::OnHoverCallback hc = boost::bind(&osgVDFBaseModel::highlightConnection, this, it->second, _1, _2);
		contextMenu->addMenuItem(it->first, false, cc, hc);
	}

    addChild(contextMenu);
    
    setFocused(contextMenu);
	hideConnections();
	contextMenu->showMenu(pos);
}

void osgVDFBaseModel::highlightConnection(osg::Geode * connection, const std::string & connectionName, bool highlight){
	
	if(highlight == true){
		setVisualConnectionColor(connection, visualConnectionHighlightColor);
		lastHighlightedConnection = connection;
	}else if(lastHighlightedConnection != 0){
		setVisualConnectionColor(connection, visualConnectionNormalColor);
		lastHighlightedConnection = 0;
	}

}

void osgVDFBaseModel::removeConnection(dflm::ConnPtr connection, const std::string & connectionName, bool checked){
	if(lastHighlightedConnection == connectionsLogicalToGraph[connection]){
		lastHighlightedConnection = 0;
	}
	
	disconnect(connection);
	
}

void osgVDFBaseModel::closeContextMenu(osgui::ContextMenu * menu){
	contextMenuOn = false;

    setFocused(0);

	if(lastHighlightedConnection != nullptr){
		setVisualConnectionColor(lastHighlightedConnection, visualConnectionNormalColor); 
		lastHighlightedConnection = nullptr;
	}
}

void osgVDFBaseModel::refreshFocused(osgWidget::Window * toFocus){
	if(focusedNode != nullptr){
		focusedNode->setStrata(osgWidget::Window::STRATA_NONE);
		setFocused(focusedNode);
	}else if(selectedNodes.empty() == false){
		setFocused(*(selectedNodes.begin()));
	}else if(graphNodes.empty() == false){
		setFocused(*(graphNodes.begin()));
	}else{
		setFocused(toFocus);
	}

	hideConnections();
}

}