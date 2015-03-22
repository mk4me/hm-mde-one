/********************************************************************
	created:  2014/12/30	10:38:16
	filename: OrientationDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__
#define __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <osg/Quat>

class OrientationDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	OrientationDelegate(QObject * parent = nullptr);

	virtual ~OrientationDelegate();

	virtual QWidget * createEditor(QWidget * parent,
		const QStyleOptionViewItem & option,
		const QModelIndex & index) const override;

	virtual void setEditorData(QWidget * editor,
		const QModelIndex & index) const override;

	virtual void setModelData(QWidget * editor,
		QAbstractItemModel * model,
		const QModelIndex & index) const override;

	virtual void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

private slots:

	void onAccept();

	void onReject();

private:

	mutable bool update;
};

#endif	// __HEADER_GUARD_IMU__ORIENTATIONDELEGATE_H__