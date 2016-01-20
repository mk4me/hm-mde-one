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
		//! \param singleOutputFolder czy wszystkie produkty maj� trafi� do tego samego folderu?
		//! \param runOnce czy aplikacja ma przetwarza� pliki ci�gle, czy tylko raz i ko�czy� dzia�anie?
		DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath,bool singleOutputFolder = false, bool runOnce = false);
		virtual ~DicomWatcher() {}

	public slots:
		//! Sprawdza folder, je�li pojawi�y si� nowe pliki zip, to rozpoczyna si� przetwarzanie
		//! \param path �cie�ka do folderu, kt�ry uleg� zmianie
		void dirChanged(const QString & path);

	public:
		//! \param path obserwowana �cie�ka
		//! \return zwraca wektor plik�w zip, kt�re jeszcze nie by�y wcze�niej przetwarzane
		std::vector<utils::Filesystem::Path> listFilesToProcess(const std::string& path);

	private:
		//! metoda zako�czy dzia�anie aplikacji, je�li ustawiony jest parametr runOnce
		void exitIfRequested(int errorCode = 0);
		void import(const utils::Filesystem::Path& from, const utils::Filesystem::Path& to);

	private:
		//! przechowuje statusy plik�w
		WatcherConfig config;
		//! �cie�ka do pliku ze statusami plik�w (informacja, czy zosta�y przetworzone)
		const utils::Filesystem::Path outputDir;
		//! czy aplikacja ma przetwarza� pliki ci�gle, czy tylko raz i ko�czy� dzia�anie?
		bool runOnce;
		//! czy wszystkie produkty maj� trafi� do tego samego folderu ?
		bool singleOutputFolder;
};



#endif // HEADER_GUARD 
