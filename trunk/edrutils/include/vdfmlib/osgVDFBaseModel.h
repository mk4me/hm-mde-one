#ifndef HEADER_GUARD__VDFBASEMODEL_H__
#define HEADER_GUARD__VDFBASEMODEL_H__

#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <boost/function.hpp>
#include <boost/bimap.hpp>

#include <osgui/KeyboardMapper.h>

#include <osgui/Buttonized.h>
#include <osgui/Borderized.h>

#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Connection.h>
#include <dfmlib/Model.h>

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

class osgVDFBaseModel : public osgWidget::WindowManager, public osgui::KeyboardMapper, public osgui::TooltipHost
{

//friend class osgVDFBaseNode;

public:
    //! Typ akcji na potrzeby realizacji usuwania po��cze� przez zewn�trzne menu kontekstowe
    typedef boost::function<void(void)> ConnectionAction;

    //! Para akcji - pod�wietlenia danego po��czenia wizualnego i jego usuni�cia na potrzeby realizacji tego zadania przez zewn�trzne menu kontekstowe - np Qt
    typedef std::pair<ConnectionAction, ConnectionAction> ConnectionActions;

    //! Typ mapuj�cy nazw� po��czenia i akcje z nim zwi�zane
    typedef std::map<std::string, ConnectionActions> ConnectionsActionsMapping; 

    //! Typ akcji w�z�a
    typedef boost::function<void(const osgVDFBaseNode *, const osgWidget::XYCoord &)> NodeAction;

    //! Akcja usuwajaca wezel
    typedef boost::function<void(const osgVDFBaseNode *)> NodeDeleteAction;

protected:
    
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopeLock;

    //! Typ s�u��cy do �adowania konfiguracji w�z��w
    typedef std::map<dflm::NPtr, osgWidget::XYCoord> NodesPositions;

    //! Mapa ��cz�ca w�z�y logiczne z ich wizualnymi reprezentacjami
    typedef std::map<dflm::NPtr, osg::ref_ptr<osgVDFBaseNode>> NodesMapping;

    //! Mapa ��cz�ca piny logiczne z ich wizualnymi reprezentacjami
    typedef std::map<dflm::PinPtr, osgVDFBasePin*> PinsMapping;

    //typedef boost::bimap< set_of<dflm::ConnPtr>, set_of<osg::ref_ptr<osg::Geode> > > CoonMapp;

    //! Mapa ��cz�ca logiczne po��czenia z ich wizualnymi reprezentacjami
    typedef std::map<dflm::ConnPtr, osg::ref_ptr<osg::Geode> > ConnectionsMapping;

    //! Mapa ��cz�ca wizualne po��czenia z ich logicznymi reprezentacjami
    typedef std::map<osg::ref_ptr<osg::Geode>, dflm::ConnPtr> RevConnectionsMapping;

    //! Zbi�r wizualnych w�z��w
    typedef osgVDFBaseNode::VNodes VNodes;

    //! Mapa poprzednich wizualnych status�w wizualnych w�z��w
    typedef std::map<osgVDFBaseNode*, osgVDFBaseNode::VisualStatus> VNodesPrevVisualStatus;

    //! Mapa poprzednich wizualnych status�w wizualnych pin�w
    typedef std::map<osgVDFBasePin*, osgVDFBasePin::VisualStatus> VPinPrevVisualStatus;

    //! Mapa kompatybilno�ci pin�w wizualnych
    typedef std::map<osgVDFBasePin*, osgVDFBaseNode::Pins> VPinsCompatibility;

private:

    typedef osgVDFBasePin::VisualConnectionsUpdater VisualConnectionUpdater;

    //! Typ przycisku w OSG na potrzeby realizacji wewn�trznego toolbara z w�z�ami
    typedef osgui::Buttonized<osgui::Borderized<osgWidget::Label>, osgui::TextStylePolicy> Button;

    //! Klasa przechwytuj�ca zmian� rozmiaru okna i aktualizuj�ca ten rozmiar dla VDF
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


    //! Klasa odpowiedzialna za zarz�dzanie zaznaczeniami wizualnych w�z��w
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

    //! Klasa odpowiadaj�ca za obs�uge akcji u�ytkownika na przestrzeni VDF - tam gdzie osadzone s� w�z�y, toolbar i po��czenia
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

        const osgVDFBaseNode* getTopNodeInPoint(const osgWidget::XYCoord & pos) const;

    protected:
        osg::observer_ptr<osgVDFBaseModel> model;
    };


    friend class NodeDescriptorClick;
    friend class osgVDFBaseNode;
    friend class osgVDFBasePin;


public:

    osgVDFBaseModel(osgViewer::View* view = 0, osgWidget::point_type width = 0.0f, osgWidget::point_type height = 0.0f,
        unsigned int nodeMask = 0, unsigned int flags = 0, const dflm::MPtr & model = dflm::MPtr());

	~osgVDFBaseModel(void);

    virtual void setModel(const dflm::MPtr & model, const NodesPositions & nodesPositions = NodesPositions());

    const dflm::MPtr & getModel() const;

    //! \param dewscriptor Klasa opisuj�ca typ w�z�a dodawanego do defualtowego toolbara
    void registerNodeType(osgVDFNodeTypeDescriptor * descriptor);

    //! \param lock Czy zablokowac do wizualnej edycji logiki i w�a�ciwo�ci modelu - tylko wizualna zmiana layoutu mo�liwa
    void lockForEdition(bool lock);

    //! \return Czy zablokowano do edycji wizualnej
    bool isLockedForEdition() const;

    //-------------------------- NODES LAYOUT MANAGMENT ------------------------------------

    //! \param pos Pozycja kt�ra testujemy czy jest wolna
    //! \return true je�li podana pozycja jest wolna, nizaj�ta przez �aden w�ze� wizualny
	bool isPointFree(const osgWidget::XYCoord & pos) const;

    //! \param area Obszar z kt�rego w�z�y chcemy pobra�
    //! \return Kolekcja w�z��w z zadanego obszaru
    dflm::Model::Nodes getNodesInArea(const osg::BoundingBox & area) const;

    //! \param node W�z� dla kt�rego poszukujemy pustego miejsca
    //! \posToStart Pozycja startowa od kt�rej powinni�my zacz�� szuka�
    //! \return Pozycja znalezionego wolnego miejsca lub pozycja startowa je�li nic nie znaleziono
    virtual osgWidget::XYCoord getFreePlaceForNode(const dflm::NPtr & node, const osgWidget::XYCoord & posToStart) const;

    //----------------------------- LOGIC EDITION ------------------------------------------

    //! \param node W�ze� logiczny do doania
    //! \param image Obrazek zwi�zany z podanym w�z�em (powinien by� obslugiwany przez implementacj� w�z�a wizualnego)
    //! \param name Nazwa w�z�a
    //! \param pos Pozycja do wstawienia w�z�a
	void addNode(const dflm::NPtr & node, osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
		const std::string &  name = std::string(), const osgWidget::XYCoord & pos = osgWidget::XYCoord(-1,-1));
	
    //! \param node W�ze� do usuni�cia
    void removeNode(const dflm::NPtr & node);

    //! Roz�ancza w�ze�, usuwa wszystkie jego po��czenia
    //! \param node W�ze� do roz��czenia
    void disconnectNode(const dflm::NPtr & node);

    //! \param src Pin kt�ry chcemy ��czy�
    //! \param dest Pin kt�ry chcemy ��czy�
    //! \return Po��czenie mi�dzy pinami
    virtual dflm::ConnPtr connect(const dflm::PinPtr & src, const dflm::PinPtr & dest);

    //! \param connection Po��czenie kt�re usuwamy
    virtual void disconnect(const dflm::ConnPtr & connection);

    //! Usuwa wszystkie w�z�y i ich po��czenia
    void removeAllNodes();

    //! Usuwa wszystkie po��czenia
    void removceAllConnections();
    
    //----------------------- SELECTION ---------------------------------------------

    //! \node W�ze� do zaznaczenia
	void selectNode(const dflm::NPtr & node);

    //! \param rect Obszar w ktorym zaznaczamy wszystkie wezly
    void selectNodes(const osg::BoundingBox & rect);

    //! Zaznacza wszystkie w�z�yu
	void selectAllNodes();

    //! \param node W�ze� do odznaczenia
	void deselectNode(const dflm::NPtr & node);

    //! \param Obszar w ktorym odznaczamy wszystkie wezly
    void deselectNodes(const osg::BoundingBox & rect);
    
    //! Odznacza wszystkie w�z�y
	void deselectAllNodes();

	//! \return Zaznaczone w�z�y
    const dflm::Model::Nodes & getSelectedNodes() const;

    //! \param node W�ze� kt�ry testujemy pod k�tem zaznaczenia
    //! \return true je�li w�ze� zaznaczony
    bool isNodeSelected(const dflm::NPtr & node) const;

    //------------------------------ VISUAL NODES PROPERTIES ---------------------------------

    //! \param node W�ze� kt�ry testujemy czy jest w kolizji
    //! \return true je�li w�ze� bvierze udzia� w kolizji
	bool isNodeInCollision(const dflm::NPtr & node) const;

    //! \param node W�ze� kt�ry testujemy na wyst�powanie w widzialnym obszarze
    //! \return true je�li w�ze� jest cho� cz�sciowo zawarty w widocznym obszarze
	bool isNodeInVisibleArea(const dflm::NPtr & node) const;

    //! \param W�ze� kt�ry testujmey czy jest ca�kowicie widzoczny
    //! \return true je�li w�ze� ca�kowicie widoczny
	bool isNodeFullyInVisibleArea(const dflm::NPtr & node) const;

    //! \param node W�ze� kt�rego pozycj� chcemy pobra�
    //! \return Pozycja w�z�a
	osgWidget::XYCoord getNodePosition(const dflm::NPtr & node);

    //! \param node W�ze� kt�rego rozmiar chcemy pobra�
    //! \return Rozmiar danego w�z�a
	osgWidget::XYCoord getNodeSize(const dflm::NPtr & node);

    //! \param nodes Kolekcja w�z��w kt�rych obszar chcemy dosta�
    //! \return obszar na bazie w�z��w kt�re wyst�puja w modelu
    osg::BoundingBox getNodesBoundingBox(const dflm::Model::Nodes & nodes ) const;

    //! \param node W�ze� kt�y przesuwamy wzgl�dem jego aktualnej pozycji
    //! \param moveStep Przesuni�cie w�z�a wzgl�dem jego aktualnej pozycji
	void setNodeRelativePosition(const dflm::NPtr & node, const osgWidget::XYCoord & moveStep);

    //! \param node W�ze� kt�y ustawiamy bezwzgl�dnie
    //! \param nodePos Nowa pozycja w�z�a
	void setNodeAbsolutePosition(const dflm::NPtr & node, const osgWidget::XYCoord & nodePos);

    //---------------------------------- VMODEL CONFIGURATION --------------------------------------

    //! \param show Czy pokazywa� domy�lny toolbar
	void showDefaultToolbar(bool show);

    //! \return Minimalny obszar wezla jaki trzeba zaznaczyc aby wezel by� uznany za zaznaczony/odznaczony
    //! wartosc procentowa powierzchni wez�a z przedzialu [0..1], gdzie 0 oznacza zaznaczenie na kontakt z w�z�em a 1 wymaga obj�cia ca�ego w�z�a
	osgWidget::point_type getMinSelectedNodeAreaRatioToSelect() const;

    //! \param areaRatio Procentowy obszar w�z�a jaki nale�y obj�� by zaznaczy�/odznaczy� w�ze�
	void setMinSelectedNodeAreaRatioToSelect(osgWidget::point_type areaRatio);

    //! \return Zbi�r klawiszy modyfikuj�cych spos�b zaznaczania
	const osgui::KeyboardMapper::Keys & getSelectionActionKeys() const;

    //! \param keys Zbi�r klawisdzy modyfikuj�cy spos�b zaznaczania
	void setSelectionActionKeys(const osgui::KeyboardMapper::Keys & keys);

    //! \return Mnimalny dystans pomiedzy aktualn� pozycj� a po��czeniem aby uzna� po�aczenie jako potencjalnie wybrane
	osgWidget::point_type getMinDistToDelConnection() const;

    //! \param dist Minimalny dystans kursora do po��czenia aby uzna� po��czenie jako potencjalnie wybrane
	void setMinDistToDelConnection(osgWidget::point_type dist);

    //! \param Normalny kolor po��czenia
	osgWidget::Color getVisualConnectionNormalColor() const;

    //! \param color Normalny kolor po��czenia
	void setVisualConnectionNormalColor( const osgWidget::Color & color);
	
    //! \return Kolor zaznaczonego po��czenia
	osgWidget::Color getVisualConnectionHighlightColor() const;
    //! \param color Kolor zaznaczonego po�aczenia
	void setVisualConnectionHighlightColor( const osgWidget::Color & color);

    //--------------------------------------- NODES ACTIONS -----------------------------------------------------------

    void setNodeClickAction(const NodeAction & clickAction);

    const NodeAction & getNodeClickAction() const;

    void setNodeDoubleClickAction(const NodeAction & doubleClickAction);

    const NodeAction & getNodeDoubleClickAction() const;

    void setNodeDeleteAction(const NodeDeleteAction & deleteAction);

    const NodeDeleteAction & getNodeDeleteAction() const;

    void setNodeContextAction(const NodeAction & contextAction);

    const NodeAction & getNodeContextAction() const;

private:

    //user interaction - connection managment
    virtual osg::Geode* createVisualConnection() = 0;
    virtual void setVisualConnectionEnd(osg::Geode * connection, const osgWidget::XYCoord & endPos) = 0;
    virtual void setVisualConnectionStart(osg::Geode * connection, const osgWidget::XYCoord & startPos) = 0;
    virtual void setVisualConnectionZ(osg::Geode * connection, osgWidget::point_type z) = 0;
    virtual void setVisualConnectionColor(osg::Geode * connection, const osgWidget::Color & color ) = 0;

    //user interaction - selection managment
    virtual osg::Geode* createVisualSelection(const osgWidget::XYCoord & startPos) = 0;
    virtual void continueVisulaSelection(osg::Geode * selection, const osgWidget::XYCoord & continuePos) = 0;

    //visual components delivery    
    virtual osgVDFBaseNode * createVisualNode(osg::ref_ptr<osg::Image> image = osg::ref_ptr<osg::Image>(),
        const std::string &  name = std::string()) = 0;

    virtual osgVDFBasePin * createVisualPin(const std::string &  name = std::string()) = 0;

    virtual void showConnectionsDeleteContextMenu(const ConnectionsActionsMapping & toDelete, const ConnectionAction & leaveContextMenu, const osgWidget::XYCoord & pos);

    //-----------------------------------------------------------------------------------------------------------------

    osgWidget::XYCoord innerGetNodePosition( const dflm::NPtr & node );
    bool innerIsPointFree( const osgWidget::XYCoord &posToStart ) const;
    bool innerIsNodeSelected(osgVDFBaseNode * vnode) const;
    bool innerIsNodeSelected(const dflm::NPtr & node) const;

	osgWidget::point_type getMinNodesZ();
	osgWidget::point_type getMaxNodesZ();

	void hideConnections();

	static void refreshPinVisualState(osgVDFBasePin * pin);
	static void addConnectionToPin(osgVDFBasePin * pin, osg::Geode * connection, osgVDFBasePin * other);
	static void removeConnectionFromPin(osgVDFBasePin * pin, osg::Geode * connection);

	void configureVisualNode(osgVDFBaseNode * node);
	void configureVisualPin(osgVDFBasePin * pin);

	static void refreshOutputPins(osgVDFBaseNode * node);

	osgVDFBaseNode* getVisualNode(const dflm::NPtr & node) const;

	void refreshFocused(osgWidget::Window * toFocus = 0);

    void saveCurrentVisualNodesStates();

    void restorePreviousVisualNodeStates();

    void updateVisualNodeStatus(osgVDFBaseNode * vnode, const VNodes & nodes);

    void deselectVisualNodes(const VNodes & vnodes, const VNodes & refNodes);

    void deselectAllVisualNodes();

    VNodes getCollisionNodes(osgVDFBaseNode * vnode, const VNodes & nodes) const;

    dflm::Model::Nodes filterNodes(const dflm::Model::Nodes & nodes) const;

    VNodes getVNodesFromNodes(const dflm::Model::Nodes & nodes) const;
    dflm::Model::Nodes getNodesFromVNodes(const VNodes & vnodes) const;

    osg::BoundingBox innerGetNodesBoundingBox( const VNodes & vnodes) const;

    VNodes getVisualNodesInArea(const osg::BoundingBox & area) const;

    void positionNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &nodePos );

    void moveNode( osgVDFBaseNode* vnode, const osgWidget::XYCoord &moveStep );
    void moveNodes( const VNodes & vnodes, const osgWidget::XYCoord &moveStep );

    void clearVisualSelections();
    void clearVisualNodes();
    void clearVisualConnections();

    void innerSelectNode(const dflm::NPtr & node);
    void innerDeselectNode(const dflm::NPtr & node);

    void innerSelectNode(osgVDFBaseNode * node);
    void innerDeselectNode(osgVDFBaseNode * node);

    void selectVisualNode(osgVDFBaseNode * node);
    void deselectVisualNode(osgVDFBaseNode * node, const VNodes & nodes);


    void selectNodes(const dflm::Model::Nodes & nodes);
    
    void deselectNodes(const dflm::Model::Nodes & nodes);

    void init( osgViewer::View* view );


	void updatePinsConnections(osgVDFBasePin * pin);

	void removeNode(const dflm::NPtr & node, bool clearEvent);
	void deleteNodeSelf(osgVDFBaseNode * node);

	bool onNodeClick(osgWidget::Event& ev);
	bool onNodeDrag(osgWidget::Event& ev);
	bool onNodeRelease(osgWidget::Event& ev);
	void updateCollisions();
	VNodes getSelectedNodesCollisions();

	bool onPinClick(osgWidget::Event& ev);
	bool onPinDrag(osgWidget::Event& ev);
	bool onPinRelease(osgWidget::Event& ev);

	bool onPinEnter(osgWidget::Event& ev);
	bool onPinLeave(osgWidget::Event& ev);

	bool onNodeEnter(osgWidget::Event& ev);
	bool onNodeLeave(osgWidget::Event& ev);

	std::vector<std::string> getPinErrors(const dflm::PinPtr & pin);
	std::vector<std::string> getNodeErrors(const dflm::NPtr & node);

	void savePinsStatus(const osgVDFBaseNode::Pins & pins);
	void highlightCompatiblePins(osgVDFBasePin * startPin, const osgVDFBaseNode::Pins & pins);
	void restorePinsStatus();

	bool onNodeDescriptorPush(osgWidget::Event& ev);

	void fillToolbarWithGroups(osgui::Toolbar * toolbar);
    void graphAddNodeTypeToToolbar(osgVDFNodeTypeDescriptor* nodeTypeDescriptor);

    void highlightConnection(osg::Geode * connection);

    void clearHighlightedConnection();

    bool onLeaveContextMenu(osgWidget::Event& ev);

    static void contextMenuActionWrapper(const ConnectionAction & action, const std::string& name, bool state);

	void removeConnection(const dflm::ConnPtr & connection, const std::string & connectionName, bool checked);
	void onCloseContextMenu(osgui::ContextMenu * menu);

protected:

	osgVDFNodeTypeDescriptor * currentNodeDescriptor;
	std::map<osgWidget::Widget*,osgVDFNodeTypeDescriptor*> controlsToNodeTypes;
	std::map<osgVDFNodeTypeDescriptor*,osgWidget::Widget*> nodeTypesToControls;
	std::set<osgVDFNodeTypeDescriptor*> nodeTypesMissing;

protected:
	dflm::MPtr logicModel;
	
	NodesMapping nodesLogicalToGraph;
	VNodes visualNodes;
	PinsMapping pinsLogicalToGraph;
	VPinsCompatibility compatiblePins;

	ConnectionsMapping connectionsLogicalToGraph;
	RevConnectionsMapping connectionsGraphToLogical;

	VNodes selectedVNodes;
    VNodes deselectedVNodes;
	KeyboardMapper::Keys selectionActionKeys;
	osgWidget::point_type minIntersectRatioToSelect;

	//connection events
	osgWidget::point_type maxDistToDelConnection;

	osgWidget::Color visualConnectionNormalColor;
	osgWidget::Color visualConnectionHighlightColor;
	
private:
	VNodesPrevVisualStatus selectionPrevNodesStatus;	
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

    bool moveStartedWithViewCollision;
	osgWidget::XYCoord initialMoveCoords;
	bool moveStarted;
	VNodesPrevVisualStatus movePrevNodesStatus;
	VNodes collisionNodes;
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
	VPinPrevVisualStatus connectingPrevPinsStatus;
	osgVDFBasePin::VisualStatus conectingPrevStartPinStatus;
	osgVDFBasePin::VisualStatus conectingPrevCurrentPinStatus;
	
    VisualConnectionUpdater connectionContinue;

	VisualConnectionUpdater connectionStartUpdate;
	VisualConnectionUpdater connectionEndUpdate;
    
    dflm::Model::Nodes selectedNodes;

	osg::ref_ptr<osgui::ContextMenu> contextMenu;
	osg::Geode * lastHighlightedConnection;
	bool contextMenuOn;

	std::set<osg::ref_ptr<osgui::Tooltip> > tooltips;
	osg::ref_ptr<osgui::Tooltip> currentNodeTooltip;
	osg::ref_ptr<osgui::Tooltip> currentPinTooltip;

	osg::ref_ptr<osgui::Toolbar> toolbar;
	bool toolbarVisible;
	Button * lastButton;

    OpenThreads::Mutex stateMutex;

    NodeDeleteAction deleteNodeAction;

    NodeAction clickNodeAction;

    NodeAction doubleClickNodeAction;

    NodeAction contextNodeAction;
};


}

#endif