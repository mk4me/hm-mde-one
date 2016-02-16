/********************************************************************
created:  2012/03/05
created:  5:3:2012   21:16
filename: IFtpSession.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_NETWORKUTILS__IFTPSESSION_H__
#define HEADER_GUARD_NETWORKUTILS__IFTPSESSION_H__

#include <list>
#include <string>
#include <istream>
#include <utils/SmartPtr.h>
#include <threadingUtils/IOperation.h>
#include <threadingUtils/IProgress.h>

namespace networkUtils
{
	class IFtpSession
	{
	public:

		//! Interfejs do customowej obs�ugi progresu - metoda setProgress powinna by� bardzo wydajna
		class ITransfer : public threadingUtils::IAbortableOperation,
			public threadingUtils::ITimeProgress
		{
		public:
			//! Wyliczenie opisuj�ce typ transferu
			enum Direction
			{
				Download,	//! Pobieranie danych
				Upload		//! Wysy�anie danych
			};

		public:
			//! Wirtualny destruktor
			virtual ~ITransfer() {}
			//! \return Ilo�� danych ju� przetworzona [bytes]
			virtual std::size_t processed() const = 0;
			//! \return Ilo�� danych do przetworzenia
			virtual std::size_t size() const = 0;
			//! \return �rednia pr�dko�� pobierania bytes/second
			virtual float averageSpeed() const = 0;
			//! \return Typ transferu
			virtual Direction direction() const = 0;
			//! \return Plik, kt�rego dotyczy transfer - lokalny lub zdalny
			virtual std::string file() const = 0;
		};

	public:

		//! Destruktor wirtualny
		virtual ~IFtpSession() {}

		//! Metoda wysy�aj�ca na serwer plik. Nazwa wys�anego pliku przechowywana jest w zmiennej filename.
		//! \param localSource nazwa pliku kt�ry wysy�amy na serwer ftp
		//! \param remoteDestination miejsce zapisu naszego pliku na ftp
		//! \return Obiekt obs�uguj�cy post�p operacji i steruj�cy jej wykonaniem
		virtual const ITransfer * putFile(const std::istream * localSource,
			const std::string & remoteDestination) = 0;

		//! Metoda pobieraj�ca plik z serwera. Nazwa pobranego pliku przechowywana jest w zmiennej filename.
		//! \param remoteSource �cia�ka pliku na serwerze ftp kt�ry chcemy �ci�gn��
		//! \param localDestination �cia�ka w kt�rej chcemy zpaisa� �ci�gany plik
		//! \return Obiekt obs�uguj�cy post�p operacji i steruj�cy jej wykonaniem
		virtual const ITransfer * getFile(const std::string& remoteSource,
			std::ostream * localDestination) const = 0;
		//! \param remoteFile Zdalny plik do usuni�cia
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool deleteFile(const std::string& remoteFile) = 0;
		//! \param remoteFile Zdalny plik
		//! \param size [out] Rozmiar pliku, -1 je�li pliku nie ma
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool fileSize(const std::string& remoteFile,
			std::size_t & size) const = 0;
		//! \param dir Katalog do utworzenia
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool createDirectory(const std::string & dir) = 0;
		//! \param dir Katalog do usuni�cia
		//! \param recursive Czy usuwa� jesli nie pusty
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool removeDirectory(const std::string & dir,
			const bool recursive = false) = 0;
		//! \param src �cie�ka elementu �r�d�owego
		//! \param dest �cie�ka elementu docelowego
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool move(const std::string & src, const std::string & dest) = 0;
		//! \param src �cie�ka elementu �r�d�owego
		//! \param dest �cie�ka elementu docelowego
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool copy(const std::string & src, const std::string & dest) = 0;
		//! \param dir Aktualny katalog roboczy
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool setWorkingDirectory(const std::string & dir) = 0;
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool setParentDirectory() = 0;
		//! \param dir [out] Katalog domowy
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool parentDirectory(std::string & dir) const = 0;
		//! \param dir [out] Aktualny katalog roboczy
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool workingDirectory(std::string & dir) const = 0;
		//! \param dir Katalog
		//! \param subDirs [out] Lista podkatalog�w
		//! \param recursive Czy wykonywa� rekurencyjnie ni�ej
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool listSubdirectories(const std::string & dir,
			std::list<std::string> & subDirs,
			const bool recursive = false) const = 0;
		//! \param dir Katalog
		//! \param files [out] Lista plik�w
		//! \param recursive Czy wykonywa� rekurencyjnie ni�ej
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool listFiles(const std::string & dir,
			std::list<std::string> & files,
			const bool recursive = false) const = 0;
		//! \param dir Katalog
		//! \param content [out] Lista element�w
		//! \param recursive Czy wykonywa� rekurencyjnie ni�ej
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool list(const std::string & dir,
			std::list<std::string> & content,
			const bool recursive = false) const = 0;
		//! \param command Komenda FTP
		//! \return Prawda je�li operacja si� powiod�a
		virtual bool execute(const std::string & command) = 0;
	};

	using FtpSessionPtr = utils::shared_ptr<IFtpSession>;
	using FtpSessionConstPtr = utils::shared_ptr<const IFtpSession>;
}

#endif	//	HEADER_GUARD_NETWORKUTILS__IFTPSESSION_H__
