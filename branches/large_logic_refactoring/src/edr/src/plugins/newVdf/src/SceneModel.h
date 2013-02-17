/********************************************************************
	created:	2012/12/27
	created:	27:12:2012   8:34
	filename: 	SceneModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__SCENEMODEL_H__
#define HEADER_GUARD_NEWVDF__SCENEMODEL_H__

#include <core/SmartPtr.h>
#include <CanvasStyleEditor.h>
#include <boost/tuple/tuple.hpp>
#include <dflib/IConnection.h>
#include <dflib/IModel.h>
#include "SceneBuilder.h"
#include <boost/type_traits.hpp>

class IVisualItem;
typedef boost::shared_ptr<df::IModel> IModelPtr;
typedef boost::shared_ptr<df::IConnection> IConnectionPtr;

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
	typedef core::shared_ptr<MergedItem> MergedItemPtr;
	typedef core::shared_ptr<const MergedItem> MergedItemConstPtr;

public:
    SceneModel(CanvasStyleEditorPtr factories);
	virtual ~SceneModel() {}

public slots:
	void merge(const QList<QGraphicsItem*>& items);

public:
    //IVisualConnection* addConnection(QGraphicsItem* item1, QGraphicsItem* item2);
	IVisualConnectionPtr addConnection(IVisualPinPtr pin1, IVisualPinPtr pin2);
	void removeConnection(IVisualPinPtr pin1, IVisualPinPtr pin2);
    void addItem(IVisualItemPtr item);
	void removeItem(IVisualItemPtr item);
    const SceneBuilder& getBuilder() const { return builder; }

	bool connectionPossible(IVisualPinPtr pin1, IVisualPinPtr pin2) const;
	//bool connectionPossible(QGraphicsItem* pin1, QGraphicsItem* pin2) const;
	const Connections& getPossibleConections(IVisualPinPtr vpin);

	IVisualItemPtr tryGetVisualItem(QGraphicsItem* item);

	template <class VisualItemT>
	QList<VisualItemT> getVisualItems()
	{
		QList<VisualItemT> ret;
		for (auto it = graphics2Visual.begin(); it != graphics2Visual.end(); ++it) {
			VisualItemT item = core::dynamic_pointer_cast<VisualItemT::element_type>(it->second);
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
	df::IInputPin* getInputPin(df::INode* node, int index);
	df::IOutputPin* getOutputPin(df::INode* node, int index);
	template<class VisualT, class DFNodeT>
	void removeNode(IVisualItemPtr item);

private:
    std::map<QGraphicsItem*, IVisualItemPtr> graphics2Visual;
    SceneBuilder builder;
	std::list<IVisualInputPinPtr> inputPins;
	std::list<IVisualOutputPinPtr> outputPins;
	IModelPtr model;
	mutable Connections pinsHelper;
	std::vector<MergedItemPtr> mergedItems;
};

typedef core::shared_ptr<SceneModel> SceneModelPtr;
typedef core::shared_ptr<const SceneModel> SceneModelConstPtr;


#endif
