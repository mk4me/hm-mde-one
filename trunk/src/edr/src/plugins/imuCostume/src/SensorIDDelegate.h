/********************************************************************
	created:  2015/03/09	21:06:53
	filename: SensorIDDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___SENSORIDDELEGATE_H__
#define __HEADER_GUARD___SENSORIDDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <imucostumelib/ImuCostume.h>

class SensorIDDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	SensorIDDelegate(QObject * parent = nullptr);

	virtual ~SensorIDDelegate();

	void setSensors(const imuCostume::Costume::SensorIDsSet & sensors);

	static QString defaultText();

	static int defaultValue();

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

signals:

	void sensorIDReset(int row) const;

private:
	imuCostume::Costume::SensorIDsSet sensors_;
};

#endif	// __HEADER_GUARD___SENSORIDDELEGATE_H__