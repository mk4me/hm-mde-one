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

//! Klasa odbiera sygna³, ¿e folder siê zmieni³ i jeœli to mo¿liwe konwertuje dicom na nasz format
class DicomWatcher : public QObject 
{
	Q_OBJECT;
	public:
		//! Konstruktor
		//! \param outputDir folder wyjœciowy, jeœli nie istnieje, to zostanie utworzony
		//! \param configPath œcie¿ka do pliku ze statusami plików (informacja, czy zosta³y przetworzone). Jesli nie ma to zostanie utworzony
		DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath);
		virtual ~DicomWatcher() {}

	public slots:
		//! Sprawdza folder, jeœli pojawi³y siê nowe pliki zip, to rozpoczyna siê przetwarzanie
		//! \param path œcie¿ka do folderu, który uleg³ zmianie
		void dirChanged(const QString & path);

	private:
		//! przechowuje statusy plików
		WatcherConfig config;
		//! œcie¿ka do pliku ze statusami plików (informacja, czy zosta³y przetworzone)
		const utils::Filesystem::Path outputDir;
};



#endif // HEADER_GUARD 
