/********************************************************************
	created:  2015/03/09	21:17:30
	filename: OrientationEstimationAlgorithmDelegate.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD___ORIENTATIONESTIMATIONALGORITHMDELEGATE_H__
#define __HEADER_GUARD___ORIENTATIONESTIMATIONALGORITHMDELEGATE_H__

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QComboBox>
#include <QtGui/QStandardItemModel>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <utils/StringConversionTools.h>

//! Klasa delegata powzalaj�cego wybra� algorytm estymacji orientacji pojedynczego czujnika
class OrientationEstimationAlgorithmDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	OrientationEstimationAlgorithmDelegate(QObject * parent = nullptr);

	virtual ~OrientationEstimationAlgorithmDelegate();

	//! \param algorithms Pula algorytm�w z kt�rej mo�na wybiera�
	void setOrientationEstimationAlgorithms(const IMU::IIMUDataSource::OrientationEstimationAlgorithms & algorithms);
	//! \return Pula algorytm�w z kt�rej mo�na wybiera�
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientationEstimationAlgorithms() const;
	//! \return Domy�lny text dla wybory algorytmu
	static QString defaultText();
	//! \return Domy�lna warto�� dla wybory algorytmu
	static IMU::IIMUOrientationEstimationAlgorithmConstPtr defaultValue();

	virtual QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const override;

	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const override;

	virtual void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option, const QModelIndex & index) const override;

private:
	IMU::IIMUDataSource::OrientationEstimationAlgorithms algorithms_;
};

#endif	// __HEADER_GUARD___ORIENTATIONESTIMATIONALGORITHMDELEGATE_H__