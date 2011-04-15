#ifndef HEADER_GUARD__DFMODEL_H__
#define HEADER_GUARD__DFMODEL_H__

#include <dfmlib/DFPin.h>
#include <dfmlib/DFNode.h>
#include <dfmlib/Model.h>
#include <dfmlib/DFInterface.h>
#include <map>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class DFModel : public Model, public DFInterface
{
public:

	typedef std::map<NPtr, Node::PINS_SET > REQUIRING_CONNECTION;
	typedef std::set<DFSNPtr> SOURCE_NODES_SET;

public:
	DFModel(void);
	virtual ~DFModel(void);

	virtual bool addNode(NPtr node);
	virtual bool removeNode(NPtr node);
	virtual void clearNodes();

	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;

	virtual ConnPtr connect(PinPtr src, PinPtr dest);
	virtual bool disconnect(PinPtr src, PinPtr dest);
	virtual bool disconnect(ConnPtr connection);
	virtual void clearConnections();

	const SOURCE_NODES_SET & getSourceNodes() const;
	const NODES_SET & getLeafNodes() const;
	const REQUIRING_CONNECTION & DFModel::getRequiringConnections() const;

protected:
    virtual void onEnableChange();

	virtual bool validateModel() const;
	void resetNodeStates();
	void resetPinStates();
	void resetPinState(PinPtr pin);
	void notifySources();

	ConnPtr quickConnect(PinPtr src, PinPtr dest);

	void process();

	void updateRequiredConnections(NPtr node);

	bool validateOperation() const;
	static bool validateNodeType(NPtr node);

	static bool canDisconnect(ConnPtr connection);
	static bool canRemoveNode(NPtr node);

	bool quickDisconnect(ConnPtr connection);
	
	bool sourcesHaveMoreData() const;

	void updateRequireConnections(PinPtr pin);

private:

	typedef std::pair<Node::PINS_SET::const_iterator,
		CONNECTIONS_SET::const_iterator> OUTPUT_POSITION;

	class FakePin : public virtual DFPin{

	public:
		FakePin(WDFMPtr model);
		bool isCompatible(PinPtr pin) const;
	protected:
		bool onUpdate();
	private:
		WDFMPtr model;
	};

	friend class FakePin;

private:

	void addLeaf(NPtr node);
	void removeLeaf(NPtr node);

	void leafHasProcessedData();

private:

	SOURCE_NODES_SET sourceNodes;
	NODES_SET leafNodes;
	//Node -> FakePin
	std::map<NPtr, PinPtr> leafFakePins;
	REQUIRING_CONNECTION pinsRequiringConnections;
	bool pendingDisable;
	unsigned int finishedLeafes;
};

}

#endif