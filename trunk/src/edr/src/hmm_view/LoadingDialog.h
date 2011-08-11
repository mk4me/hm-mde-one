/********************************************************************
	created:	2011/08/04
	created:	4:8:2011   13:28
	filename: 	LoadingDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__LOADINGDIALOG_H__
#define HEADER_GUARD_HMM__LOADINGDIALOG_H__

#include "ui_LoadingDialog.h"
#include <core/Filesystem.h>

class LoadingDialog : public QDialog, private Ui::LoadingDialog
{
	Q_OBJECT
public:
	LoadingDialog();
	virtual ~LoadingDialog() {}

public:
	virtual bool event ( QEvent * e );

public:
	void start(const QString& directoryName);

protected slots:
	void setValue(int value);
	void setMinMax(int min, int max);
	void setFile(const QString& filename);
};

#endif
