#ifndef HEADER_GUARD__MODEL_H__
#define HEADER_GUARD__MODEL_H__

#include <dfmlib/DFLMTypes.h>
#include <dfmlib/Node.h>
#include <dfmlib/Pin.h>
#include <boost/enable_shared_from_this.hpp>
#include <set>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

class Model : public boost::enable_shared_from_this<Model>
{
public:
	typedef std::vector<ConnPtr> CYCLE;
	typedef std::set<NPtr> NODES_SET;
	typedef Pin::CONNECTIONS_SET CONNECTIONS_SET;

public:
	Model(void);
	virtual ~Model(void);

	virtual bool addNode(NPtr node);
	virtual bool removeNode(NPtr node);
	virtual void clearNodes();

	virtual bool canConnect(CPinPtr src, CPinPtr dest) const;
	bool createCycle(CPinPtr src, CPinPtr dest) const;
	virtual CYCLE getCycle(CPinPtr src, CPinPtr dest) const;

	virtual ConnPtr connect(PinPtr src, PinPtr dest);
	virtual bool disconnect(PinPtr src, PinPtr dest);
	virtual bool disconnect(ConnPtr connection);
	virtual void clearConnections();

	const NODES_SET & getNodes() const;
	const CONNECTIONS_SET & getConnections() const;

protected:

	typedef struct MY_PATH_ENTRY{
		NPtr node;
		Node::PINS_SET::const_iterator pinIT;
		CONNECTIONS_SET::const_iterator connIT;
		bool operator==(const MY_PATH_ENTRY & pe) const;
	}PATH_ENTRY;

protected:
	static PATH_ENTRY getFirstNodeOutputConnection(NPtr node);
	static PATH_ENTRY getNextNodeOutputConnection(const PATH_ENTRY & pathElement);

	virtual bool disconnectNode(NPtr node);
	ConnPtr findConnection(CPinPtr src, CPinPtr dest) const;
	static void unregisterPin(PinPtr pin);
	static bool isNodeUnconnected(NPtr node);

	virtual bool quickDisconnect(ConnPtr connection);
	virtual ConnPtr quickConnect(PinPtr src, PinPtr dest);

protected:
	NODES_SET nodes;
	CONNECTIONS_SET connections;
};

}

#endif
