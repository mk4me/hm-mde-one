#ifndef HEADER_GUARD__DFMTYPES_H__
#define HEADER_GUARD__DFMTYPES_H__

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace dflm{
////////////////////////////////////////////////////////////////////////////////

	class ConnectionManager;
	class Node;
	class DFNode;
	class DFSourceNode;
	class Connection;
	class Pin;
	class DFPin;
	class Model;
	class DFModel;

	typedef boost::shared_ptr<Node> NPtr;
	typedef boost::shared_ptr<const Node> CNPtr;
	typedef boost::weak_ptr<Node> WNPtr;
	typedef boost::weak_ptr<const Node> CWNPtr;

	typedef boost::shared_ptr<DFNode> DFNPtr;
	typedef boost::shared_ptr<const DFNode> CDFNPtr;
	typedef boost::weak_ptr<DFNode> WDFNPtr;
	typedef boost::weak_ptr<const DFNode> CWDFNPtr;

	typedef boost::shared_ptr<DFSourceNode> DFSNPtr;
	typedef boost::shared_ptr<const DFSourceNode> CDFSNPtr;
	typedef boost::weak_ptr<DFSourceNode> WDFSNPtr;
	typedef boost::weak_ptr<const DFSourceNode> CWDFSNPtr;

	typedef boost::shared_ptr<Connection> ConnPtr;
	typedef boost::shared_ptr<const Connection> CConnPtr;
	typedef boost::weak_ptr<Connection> WConnPtr;
	typedef boost::weak_ptr<const Connection> CWConnPtr;

	typedef boost::shared_ptr<Pin> PinPtr;
	typedef boost::shared_ptr<const Pin> CPinPtr;
	typedef boost::weak_ptr<Pin> WPinPtr;
	typedef boost::weak_ptr<const Pin> CWPinPtr;

	typedef boost::shared_ptr<DFPin> DFPinPtr;
	typedef boost::shared_ptr<const DFPin> CDFPinPtr;
	typedef boost::weak_ptr<DFPin> WDFPinPtr;
	typedef boost::weak_ptr<const DFPin> CWDFPinPtr;

	typedef boost::shared_ptr<Model> MPtr;
	typedef boost::shared_ptr<const Model> CMPtr;
	typedef boost::weak_ptr<Model> WMPtr;
	typedef boost::weak_ptr<const Model> CWMPtr;

	typedef boost::shared_ptr<DFModel> DFMPtr;
	typedef boost::shared_ptr<const DFModel> CDFMPtr;
	typedef boost::weak_ptr<DFModel> WDFMPtr;
	typedef boost::weak_ptr<const DFModel> CWDFMPtr;
}

#endif
