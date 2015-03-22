/********************************************************************
	created:  2015/03/09	20:45:18
	filename: SkeletonJointsDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__
#define __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <kinematiclib/Joint.h>

class SkeletonJointsDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	SkeletonJointsDelegate(const bool oneToOne = true,
		QObject * parent = nullptr);

	virtual ~SkeletonJointsDelegate();

	void setJoints(kinematic::JointConstPtr root);

	static QString defaultText();

	virtual QWidget * createEditor(QWidget * parent,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

	virtual void setEditorData(QWidget * editor,
		const QModelIndex & index) const override;

	virtual void setModelData(QWidget * editor,
		QAbstractItemModel * model,
		const QModelIndex & index) const override;

	virtual void updateEditorGeometry(QWidget * editor,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

	bool oneToOne() const;

public slots:

	void setOneToOne(const bool oneToOne);

private:
	std::list<std::string> joints;
	bool oneToOne_;
};

#endif	// __HEADER_GUARD___SKELETONJOINTSDELEGATE_H__