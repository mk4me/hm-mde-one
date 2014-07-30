/********************************************************************
	created:  2012/03/08
	created:  8:3:2012   11:09
	filename: ShallowCopyUtils.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__
#define HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__

#include <plugins/hmdbCommunication/Types.h>
#include <plugins/hmdbCommunication/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>
#include <plugins/hmdbCommunication/IHMDBLocalContext.h>
#include <corelib/Filesystem.h>

namespace hmdbCommunication
{
	class IHMDBStorageOperations;

	//! Klasa pomagająca tworzyć/zarządzać płytkimi kopiami danych
	class ShallowCopyUtils
	{
	public:

		//! Typ wuliczeniowy opisujący składowe płytkiej kopii bazy danych
		enum ShallowCopyType
		{
			Shallow = -1,				//! Perspektywa danych
			Meta = -2,					//! Metadane
			IncrementalShallow = -3,	//! Przyrostowa perspektywa danych
			IncrementalMeta = -4		//! Przyrostowe metadane
		};

		//! Identyfikator pliku płytkiej kopii bazy danych
		static const hmdbServices::ID ShallowCopyFileID = Shallow;
		//! Identyfikator pliku metadanych
		static const hmdbServices::ID MetadataFileID = Meta;
		//! Identyfikator pliku przyrostowej płytkiej kopii bazy danych
		static const hmdbServices::ID IncrementalShallowCopyFileID = IncrementalShallow;
		//! Identyfikator pliku przyrostowych metadanych
		static const hmdbServices::ID IncrementalMetadataFileID = IncrementalMeta;

		//! Mapa identyfikatorów plikw do ściągnięcia z odpowiadającymi ścieżkami na ftp
		typedef std::map<hmdbServices::ID, std::string> StorageFileNames;
		//! Mapa pochodzenia plików i ich opisu
		typedef std::map<IHMDBRemoteContext::DataReference, StorageFileNames> GroupedStorageFileNames;
		//! Mapa użytkowników zapisanych w Storage i ich plików płytkiej kopii bazy danych
		typedef std::map<std::string, GroupedStorageFileNames> StorageUserFiles;

	public:

		//! \param shallowCopy Płytka kopia bazy danych do wypełnienia
		//! \param motionShallowCopy Strukień płytkiej kopi danych ruchu
		//! \param motionMetadata Strumień metadanych płytkiej kopi danych ruchu
		//! \param medicalShallowCopy Strumień danych płytkiej kopi danych medycznych
		//! \param medicalMetadata Strumień metadanych płytkiej kopi danych medycznych
		static void createShallowCopy(ShallowCopy & shallowCopy,
			std::istream * motionShallowCopy, std::istream * motionMetadata,
			std::istream * medicalShallowCopy, std::istream * medicalMetadata);

		static const ShallowCopyPtr extractShallowCopy(const std::string & userHash,
			const IHMDBStorageOperations * storage);

		//! \param shallowCopy Płytka kopia bazy danych
		//! \return Czy płytka kopia bazy danych jest spójna i poprawna
		static const bool checkShallowCopyIntegrity(const ShallowCopy & shallowCopy);

		//! \param shallowCopy Płytka kopia bazy danych
		//! \param time Czas względem którego weryfikujemy wpłytką kopię
		//! \return Czy płytka kopia bazy danych wymaga odświeżenia (ściągnięcia nowych plików
		static const bool shallowCopyRequiresRefresh(const ShallowCopy & shallowCopy,
			const hmdbServices::DateTime & time);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \param recursive Czy interesują nas wszystkie pliki czy specyficzne
		//! \return Zbiór plików
		static const StorageFileNames files(const DataType dataType,
			const ShallowCopy & shallowCopy, const bool recursive = true);

		//! \param dataType Typ danych dla któego chcemy wyciągnąc pliki
		//! \param id Identyfikator danych
		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \param recursive Czy interesują nas wszystkie pliki czy specyficzne
		//! \return Zbiór plików
		static const StorageFileNames files(const DataType dataType,
			const hmdbServices::ID id, const ShallowCopy & shallowCopy,
			const bool recursive = true);

		//! \param shallowCopy Płytka kopia na bazie której wyciągamy pliki
		//! \return Zbiór plików
		static const StorageFileNames files(const ShallowCopy & shallowCopy);

		//! \return Czy płytka kopia bazy danych jest dostepna w storage
		static const bool shallowCopyInStorage(const std::string & userHash,
			const ShallowCopyType shallowType,
			const IHMDBStorageOperations * storage);

		//! Usuwa wszystkie pliki płytkiej kopii bazy danych ze storage
		static void removeAllShallowCopies(IHMDBStorageOperations * storage);
		//! \param dataReference Referencja danych
		//! \param id Identyfikator pliku płytkiej kopii
		//! \param stream Strumien do czytania danych płytkiej kopii
		static void storeShallowCopy(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType, std::istream * stream, IHMDBStorageOperations * storage);
		//! \param dataReference Referencja danych
		//! \param id Identyfikator pliku płytkiej kopii
		//! \return Strumien do czytania danych lub nullptr jeśli nie ma
		static std::istream * shallowCopyStream(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType, const IHMDBStorageOperations * storage);

		static const std::string shallowCopyName(const std::string & userHash,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType);

		static const std::list<std::string> allShallowCopiesNames(IHMDBStorageOperations * storage);

		static const std::list<std::string> filterShallowCopiesNames(const std::list<std::string> & src,
			const IHMDBRemoteContext::DataReference dataReference,
			const ShallowCopyType shallowType);

		static const std::list<std::string> removeUserShallowCopiesNames(const std::list<std::string> & src,
			const std::string & userHash);

		static const std::string userHash(const std::string & user, const std::string & password);

		//! \param storage Miejsce składowania danych
		//! \return Lista plików płytkiej kopii bazy danych
		static const StorageUserFiles groupedShallowCopiesNames(IHMDBStorageOperations * storage);
	};

}

#endif	//	HEADER_GUARD_HMDBCOMMUNICATION__SHALLOWCOPYUTILS_H__
