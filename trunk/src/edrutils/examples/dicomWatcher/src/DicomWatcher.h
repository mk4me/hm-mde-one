/********************************************************************
	created:	2015/10/21
	created:	14:01
	filename: 	DicomWatcher.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM_WATCHER__DicomWatcher_H__
#define HEADER_GUARD_DICOM_WATCHER__DicomWatcher_H__

#include <QtCore/QObject>
#include <QtCore/QString>

class DicomWatcher : public QObject 
{
	Q_OBJECT;
	public:
		virtual ~DicomWatcher() {}

	public slots:
		void dirChanged(const QString & path);
		void fileChanged(const QString & path);
};


#endif // HEADER_GUARD 
