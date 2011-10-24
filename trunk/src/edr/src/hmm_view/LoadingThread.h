/********************************************************************
	created:	2011/08/05
	created:	5:8:2011   9:10
	filename: 	LoadingThread.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__LOADINGTHREAD_H__
#define HEADER_GUARD_HMM__LOADINGTHREAD_H__

#include <QtCore/QThread>
#include <core/Filesystem.h>
#include <core/IManagersAccessor.h>
#include <set>
#include <core/ObjectWrapper.h>

class LoadingThread : public QThread
{
	Q_OBJECT
public:
	LoadingThread(const QString& directoryName);
	~LoadingThread();

public:
	virtual void run();

	signals:
		void sendValue(int value);
		void sendMinMax(int min, int max);
		void sendFile(const QString& filename);
		void loadingEnded();

private:
	QString directoryName;
	int filesToLoad;
	std::set<QString> filesSended;

    core::Files files;
};

#endif
