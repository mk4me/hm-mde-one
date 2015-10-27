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
#include <utils/Filesystem.h>
#include "WatcherConfig.h"

//! Klasa odbiera sygna�, �e folder si� zmieni� i je�li to mo�liwe konwertuje dicom na nasz format
class DicomWatcher : public QObject 
{
	Q_OBJECT;
	public:
		//! Konstruktor
		//! \param outputDir folder wyj�ciowy, je�li nie istnieje, to zostanie utworzony
		//! \param configPath �cie�ka do pliku ze statusami plik�w (informacja, czy zosta�y przetworzone). Jesli nie ma to zostanie utworzony
		DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath);
		virtual ~DicomWatcher() {}

	public slots:
		//! Sprawdza folder, je�li pojawi�y si� nowe pliki zip, to rozpoczyna si� przetwarzanie
		//! \param path �cie�ka do folderu, kt�ry uleg� zmianie
		void dirChanged(const QString & path);

	private:
		//! przechowuje statusy plik�w
		WatcherConfig config;
		//! �cie�ka do pliku ze statusami plik�w (informacja, czy zosta�y przetworzone)
		const utils::Filesystem::Path outputDir;
};



#endif // HEADER_GUARD 
