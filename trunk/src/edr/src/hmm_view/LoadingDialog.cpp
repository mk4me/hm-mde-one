#include <core/IDataManager.h>
#include <core/PluginCommon.h>
#include "LoadingDialog.h"
#include "LoadingThread.h"

bool LoadingDialog::event( QEvent * e )
{
	return QDialog::event(e);
}

LoadingDialog::LoadingDialog() :
	QDialog()
{
	Ui::LoadingDialog::setupUi(this);
}

void LoadingDialog::start( const QString& directoryName )
{
	LoadingThread lt(directoryName);
	QObject::connect(&lt, SIGNAL(sendFile(const QString&)), this, SLOT(setFile(const QString&)), Qt::QueuedConnection);
	QObject::connect(&lt, SIGNAL(sendMinMax(int, int)), this, SLOT(setMinMax(int, int)), Qt::QueuedConnection);
	QObject::connect(&lt, SIGNAL(sendValue(int)), progressBar, SLOT(setValue(int)), Qt::QueuedConnection);
	QObject::connect(&lt, SIGNAL(loadingEnded()), this, SLOT(close()), Qt::QueuedConnection);
	lt.start();
	this->exec();
}

void LoadingDialog::setValue( int value )
{
	progressBar->setValue(0);
}

void LoadingDialog::setMinMax( int min, int max )
{
	progressBar->setMinimum(min);
	progressBar->setMaximum(max);
}

void LoadingDialog::setFile( const QString& filename )
{
	label->setText(filename);
}


