#include "SkeletonJointsDelegate.h"

SkeletonJointsDelegate::SkeletonJointsDelegate(const bool oneToOne,
	QObject * parent) : QStyledItemDelegate(parent), oneToOne_(oneToOne)
{

}

SkeletonJointsDelegate::~SkeletonJointsDelegate()
{

}

void SkeletonJointsDelegate::setJoints(kinematic::JointConstPtr root)
{
	std::list<std::string> locJoints;

	auto visitor =  [&locJoints](kinematic::JointConstPtr node, const std::size_t level)
	{
		if (node != nullptr){
			locJoints.push_back(node->value.name);
		}
	};
	utils::TreeNode::visitLevelOrder(root, visitor);

	joints.swap(locJoints);
}

QString SkeletonJointsDelegate::defaultText()
{
	return tr("Select joint...");
}

QWidget * SkeletonJointsDelegate::createEditor(QWidget * parent,
	const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	std::unique_ptr<QComboBox> editor(new QComboBox(parent));

	auto locJoints(joints);

	if (oneToOne_ == true){
		auto s = index.model()->rowCount();
		for (unsigned int i = 0; i < s; ++i)
		{
			if (i != index.row())
			{
				auto jointName = index.model()->data(index.model()->index(i, index.column())).toString().toStdString();
				locJoints.remove(jointName);
			}
		}
	}

	editor->addItem(defaultText());
	const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(editor->model());
	QStandardItem* item = model->item(0);

	item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));

	if (locJoints.empty() == false)
	{
		editor->insertSeparator(1);

		for (const auto & j : locJoints)
		{
			editor->addItem(QString::fromStdString(j));
		}
	}

	return editor.release();
}

void SkeletonJointsDelegate::setEditorData(QWidget * editor,
	const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	e->setCurrentText(index.model()->data(index).toString());
}

void SkeletonJointsDelegate::setModelData(QWidget * editor,
	QAbstractItemModel * model,
	const QModelIndex & index) const
{
	QComboBox * e = qobject_cast<QComboBox*>(editor);
	model->setData(index, e->currentText());
}

void SkeletonJointsDelegate::updateEditorGeometry(QWidget * editor,
	const QStyleOptionViewItem & option,
	const QModelIndex & index) const
{
	editor->setGeometry(option.rect);
}

bool SkeletonJointsDelegate::oneToOne() const
{
	return oneToOne_;
}

void SkeletonJointsDelegate::setOneToOne(const bool oneToOne)
{
	oneToOne_ = oneToOne;
}
