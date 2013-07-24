/********************************************************************
	created:	2012/12/27
	created:	27:12:2012   8:34
	filename: 	SceneModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SCENEMODEL_H__
#define HEADER_GUARD_NEWVDF__SCENEMODEL_H__

#include <utils/SmartPtr.h>
#include <CanvasStyleEditor.h>
#include <boost/tuple/tuple.hpp>
#include <dflib/IConnection.h>
#include <dflib/IModel.h>
#include "SceneBuilder.h"
#include <boost/type_traits.hpp>
//#include <plugins/newVdf/TreeBuilder.h>
#include <corelib/IDataManagerReader.h>

namespace vdf {

class IVisualItem;
typedef boost::shared_ptr<df::IModel> IModelPtr;
typedef boost::shared_ptr<df::IConnection> IConnectionPtr;


class PinResolver
{
public:
    PinResolver(IVisualPinPtr p1, IVisualPinPtr p2)
    {
        if (p1->isType(IVisualItem::InputPin)) {
            input = utils::dynamic_pointer_cast<IVisualInputPin>(p1);
            output = utils::dynamic_pointer_cast<IVisualOutputPin>(p2);
        } else {
            input = utils::dynamic_pointer_cast<IVisualInputPin>(p2);
            output = utils::dynamic_pointer_cast<IVisualOutputPin>(p1);
        }

        UTILS_ASSERT(input && output);
    }

    operator IVisualInputPinPtr() { return input; }
    operator IVisualOutputPinPtr() { return output; }

    vdf::IVisualOutputPinPtr getOutput() const { return output; }
    vdf::IVisualInputPinPtr getInput() const { return input; }

private:
    IVisualInputPinPtr input;
    IVisualOutputPinPtr output;
};

class SceneModel : public QObject
{
    Q_OBJECT;
public:
	struct Connections
	{
		std::vector<IVisualPinPtr> possible;
		std::vector<IVisualPinPtr> impossible;
	};
	
	struct MergedItem
	{
		QList<IVisualNodePtr> nodes;
		IVisualNodePtr node;
		QList<IVisualInputPinPtr> inputPins;
		QList<IVisualOutputPinPtr> outputPins;
	};
	DEFINE_SMART_POINTERS(MergedItem);

public:
    SceneModel(CanvasStyleEditorPtr factories);
	virtual ~SceneModel() {}

public slots:
	SceneBuilder::VisualNodeWithPins merge(const QList<QGraphicsItem*>& items);

public:
	IVisualConnectionPtr addConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin);
    void removeConnection(IVisualOutputPinPtr outputPin, IVisualInputPinPtr inputPin);
    void removeConnection(IVisualItemPtr item);

    void addItem(IVisualItemPtr item);
	void removeItem(IVisualItemPtr item);
    const SceneBuilder& getBuilder() const { return builder; }

	bool connectionPossible(IVisualPinPtr pin1, IVisualPinPtr pin2) const;
	const Connections& getPossibleConections(IVisualPinPtr vpin);

	IVisualItemPtr tryGetVisualItem(QGraphicsItem* item);

	void clearScene();
    
	template <class VisualItemT>
	QList<VisualItemT> getVisualItems()
	{
		QList<VisualItemT> ret;
		for (auto it = graphics2Visual.begin(); it != graphics2Visual.end(); ++it) {
			auto item = utils::dynamic_pointer_cast<typename VisualItemT::element_type>(it->second);
			if (item) {
				ret.push_back(item);
			}
		}

		return ret;
	}

	template <class VisualItemT>
	QList<VisualItemT> getVisualItems(const QList<QGraphicsItem *>& graphicsItems)
	{
		QList<VisualItemT> ret;

		QList<VisualItemT> items = getVisualItems<VisualItemT>();
		for (auto it = items.begin(); it != items.end(); ++it) {
			if (graphicsItems.contains((*it)->visualItem())) {
				ret.push_back(*it);
			}
		}
		return ret;
	}
	void run();

signals:
    void visualItemAdded(IVisualItemPtr item);
	void visualItemRemoved(IVisualItemPtr item);

private:
	void addNode(df::INode* node);
	template<class VisualT, class DFNodeT>
	void removeNode(IVisualItemPtr item);
	template<class VisualPinT>
	void removePin(IVisualItemPtr item);
	void removeOutputPins(IVisualNodePtr node);
	void removeInputPins(IVisualNodePtr node); 
	void commonErase( IVisualItemPtr item );

private:
    std::map<QGraphicsItem*, IVisualItemPtr> graphics2Visual;
    SceneBuilder builder;
	std::list<IVisualInputPinPtr> inputPins;
	std::list<IVisualOutputPinPtr> outputPins;
	IModelPtr model;
	mutable Connections pinsHelper;
	std::vector<MergedItemPtr> mergedItems;
};
DEFINE_SMART_POINTERS(SceneModel);
}

#endif
