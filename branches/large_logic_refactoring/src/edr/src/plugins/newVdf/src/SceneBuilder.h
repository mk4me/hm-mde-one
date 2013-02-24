/********************************************************************
	created:	2013/01/04
	created:	4:1:2013   8:12
	filename: 	SceneBuilder.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SCENEBUILDER_H__
#define HEADER_GUARD_NEWVDF__SCENEBUILDER_H__


#include <corelib/SmartPtr.h>
#include <CanvasStyleEditor.h>
#include <plugins/newVdf/IVisualNode.h>
#include <boost/tuple/tuple.hpp>

namespace vdf {

class IVisualItem;

class SceneBuilder
{
	friend class SceneModel;
public:
    typedef std::vector<IVisualPinPtr> Pins;
    typedef boost::tuple<IVisualNodePtr, Pins, Pins> VisualNodeWithPins;
public:
    SceneBuilder(CanvasStyleEditorPtr factories);
    virtual ~SceneBuilder() {}

    IVisualConnectionPtr createConnection(IVisualPinPtr pin1, IVisualPinPtr pin2);

    VisualNodeWithPins createType(const QString& text, df::INode* prototype, int in, int out) const;
    VisualNodeWithPins createSource(const QString& text, df::INode* prototype, int out) const;
    VisualNodeWithPins createSink(const QString& text, df::INode* prototype, int in) const;
    VisualNodeWithPins createProcessing(const QString& text, df::INode* prototype, int in, int out) const;
    vdf::CanvasStyleEditorPtr getFactories() const { return factories; }
private:
    Pins addInputPins(IVisualSinkNodePtr sink, int count) const;
    Pins addOutputPins(IVisualSourceNodePtr source, int count) const;

private:
    CanvasStyleEditorPtr factories;
};

}

#endif
