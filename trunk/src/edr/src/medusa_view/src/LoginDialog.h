/********************************************************************
	created:	2015/09/23
	created:	20:00
	filename: 	LoginDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD___LoginDialog_H__
#define HEADER_GUARD___LoginDialog_H__

#include "ui_LoginDialog.h"

class LoginDialog : public QDialog, private Ui::LoginDialog
{
	Q_OBJECT;
public:
	LoginDialog(QWidget* parent);
	virtual ~LoginDialog() {}

public:
	const QString& getUser();
	const QString& getPassword();
};

#endif // HEADER_GUARD 
