/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   8:04
    filename: AntropometricDataWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__
#define HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__

#include <qthtmlgenlib/TableGenerator.h>
#include <QtGui/QTextEdit>

class AntropometricDataWidget : public QTextEdit
{
	Q_OBJECT

public:

	AntropometricDataWidget(QWidget * parent = nullptr);
	virtual ~AntropometricDataWidget();

public slots:

	void setAutoUpdate(bool autoUpdate = true);

	void setBodyMass(const QString & val);
	void setBodyHeight(const QString & val);
	void setASIS(const QString & val);
	
	void setLeftLegLength(const QString & val);
	void setRightLegLenght(const QString & val);
	void setLeftKneeWidth(const QString & val);
	void setRightKneeWidth(const QString & val);
	void setLeftAnkleWidth(const QString & val);
	void setRightAnkleWidth(const QString & val);
	void setLeftCircuitThigh(const QString & val);
	void setRightCircuitThight(const QString & val);
	void setLeftCircuitShank(const QString & val);
	void setRightCircuitShank(const QString & val);
	void setLeftShoulderOffset(const QString & val);
	void setRightShoulderOffset(const QString & val);
	void setLeftElbowWidth(const QString & val);
	void setRightElbowWidth(const QString & val);
	void setLeftWristWidth(const QString & val);
	void setRightWristWidth(const QString & val);
	void setLeftWristThickness(const QString & val);
	void setRightWristThickness(const QString & val);
	void setLeftHandWidth(const QString & val);
	void setRightHandWidth(const QString & val);
	void setLeftHandThickness(const QString & val);
	void setRightHandThickness(const QString & val);

	void resetAntropometricData(const QString & val = QString("-"));

private:

	void initAntropometric();
	void initAntropometricHeadersStructure();
	void initAntropometricContent();
	void initAntropometricStyles();
	void initAntropometricAtributes();

	void updateContent();
	void tryUpdateContent();
	void tryChangeContent();

private:

	struct AntropometricTabel {
		htmlGen::HtmlDataTableContent antropometricContent;
		htmlGen::TableHeadersStructureDescriptor antropometricHeadersStructure;
		htmlGen::HtmlDataTableCellAttributes antropometricCellsAttributes;
		htmlGen::HtmlDataTableStyles antropometricStyles;
	};

private:

	bool autoUpdate_;
	bool changed_;
	AntropometricTabel antropometricGeneralTable;
	AntropometricTabel antropometricLeftRightTable;
};

#endif // HEADER_GUARD___ANTROPOMETRICDATAWIDGET_H__