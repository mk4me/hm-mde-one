/********************************************************************
    created:  2013/02/06
    created:  6:2:2013   10:02
    filename: LabeledDoubleSpinBox.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___LABELEDDOUBLESPINBOX_H__
#define HEADER_GUARD___LABELEDDOUBLESPINBOX_H__

#include "ui_LabeledDoubleSpinBox.h"

#include <QtGui/QWidget>

class LabeledDoubleSpinBox : public QWidget, public Ui::LabeledDoubleSpinBox
{
	Q_OBJECT;

public:
	explicit LabeledDoubleSpinBox(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~LabeledDoubleSpinBox();
};

#endif	//	HEADER_GUARD___LABELEDDOUBLESPINBOX_H__
