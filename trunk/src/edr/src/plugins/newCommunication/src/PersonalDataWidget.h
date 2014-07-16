/********************************************************************
    created:  2012/03/12
    created:  12:3:2012   17:07
    filename: PersonalDataWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___PERSONALDATAWIDGET_H__
#define HEADER_GUARD___PERSONALDATAWIDGET_H__

#include "ui_PersonalDataWidget.h"
#include <QtWidgets/QFrame>
// TODO gdzie jeszcze jest jaki� min? Wsz�dzie przed Windows.h wstawi�em NOMINMAX
#undef min
#include <QtCore/QDate>

class PersonalDataWidget : public QFrame, public Ui::PersonalDataWidget
{
	Q_OBJECT

public:

	PersonalDataWidget(QWidget * parent = nullptr);
	virtual ~PersonalDataWidget();

public slots:

	void setPhoto(const QPixmap & photo);
	void setName(const QString & name);
	void setSurname(const QString & surname);
	void setGender(const QString & gender);
	void setBirthdate(const QString & birthdate);

	void reset();
};

#endif	//	HEADER_GUARD___PERSONALDATAWIDGET_H__
