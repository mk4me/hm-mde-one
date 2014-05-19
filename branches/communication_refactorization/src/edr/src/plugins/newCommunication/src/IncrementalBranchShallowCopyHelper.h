/********************************************************************
	created:	2014/01/19
	created:	19:1:2014   20:27
	filename: 	IncrementalBranchShallowCopyHelper.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_COMMUNICATION__INCREMENTALBRANCHSHALLOWCOPYHELPER_H__
#define HEADER_GUARD_COMMUNICATION__INCREMENTALBRANCHSHALLOWCOPYHELPER_H__

#include <map>
#include <plugins/newCommunication/IDownloadRequest.h>
#include "CommunicationManager.h"
#include <corelib/Filesystem.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <hmdbserviceslib/IncrementalBranchShallowCopy.h>

namespace communication {
	class IncrementalBranchShallowCopyHelper
	{
	public:
		static hmdbServices::IncrementalBranchShallowCopy getIncrementalShallowCopy(CommunicationManager* cm, const std::string& savePath, const hmdbServices::DateTime& dt);

	private:
		IncrementalBranchShallowCopyHelper(CommunicationManager* cm);

		enum State {
			Idle,
			Started,
			Downloading,
			Error,
			Success
		};

		State getState() const;

		//! Metoda wywoływana kiedy pojedyczny plik rozpoczyna ściąganie
		//! \param request Zlecenie którego dotyczy callback
		void onDownloadBegin(const CommunicationManager::BasicRequestPtr & request);

		//! Metoda wywoływana kiedy zadanie jest anulowane
		//! \param request Zlecenie którego dotyczy callback
		void onDownloadCancel(const CommunicationManager::BasicRequestPtr & request);

		//! Metoda wywoływana kiedy pojedyczny plik zostanie ściągnięty
		//! \param request Zlecenie którego dotyczy callback
		void onDownloadComplete(const CommunicationManager::BasicRequestPtr & request);

		//! Metoda wywoływana kiedy zadanie kończy się błędem
		//! \param request Zlecenie którego dotyczy callback
		void onDownloadError(const CommunicationManager::BasicRequestPtr & request, const std::string & error);

	private:
		//! Zbiór callbacków realizujących obsługę stanu zlecenia pojedynczego ściągania
		CommunicationManager::RequestCallbacks singleTransferCallbacks;
		CommunicationManager* communicationManager;
		mutable State state;
		std::string error;
	};
}
#endif	//	HEADER_GUARD___DOWNLOADREQUEST_H__
