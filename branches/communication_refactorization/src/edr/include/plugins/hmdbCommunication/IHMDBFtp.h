/********************************************************************
	created:  2014/05/21	12:36:11
	filename: IHMDBFtp.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBFTP_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBFTP_H__

#include <networkUtils/IFTPSession.h>
#include <threadingUtils/IProgress.h>
#include <utils/SmartPtr.h>

namespace hmdbCommunication
{
	//! Interfejs do komunikacji ftp bazy danych ruchu
	class IHMDBFtp
	{
	public:

		//! Interfejs transferu
		class ITransfer : public networkUtils::IFtpSession::ITransfer,
			public threadingUtils::IProgress
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITransfer() {}
			//! Domy�lna implementacja post�pu
			virtual const float progress() const
			{
				return (size() != 0) ? ((float)processed() / (float)size()) : 0.0;
			}
		};

		//! Wska�nik dla post�pu
		typedef utils::shared_ptr<ITransfer> TransferPtr;
		//! Wska�nik dla post�pu w wersji do odczytu
		typedef utils::shared_ptr<const ITransfer> TransferConstPtr;

	public:
		//! Desturktor wirtualny
		virtual ~IHMDBFtp() {}

		//! \param destinationFileName Nazwa docelowa uploadowanego pliku na serwerze bazy ruchu
		//! \param stream Strumie� z kt�rego b�dziemy wysy�a� zawarto�� pliku
		//! \param size Rozmiar danych do wys�ania
		virtual TransferPtr put(const std::string & destinationFileName,
			std::istream * stream, const unsigned long long size = 0) = 0;

		//! \param destinationFileName Nazwa pobieranego pliku z serwera bazy ruchu
		//! \param stream Strumie� do kt�rego b�dziemy zapisywa� zawarto�� �ci�ganego pliku
		//! \param size Rozmiar danych do pobrania
		virtual TransferPtr get(const std::string & destinationFileName,
			std::ostream * stream, const unsigned long long size = 0) = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBFTP_H__