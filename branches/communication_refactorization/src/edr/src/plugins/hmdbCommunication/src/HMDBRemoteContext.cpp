#include "CommunicationPCH.h"
#include "HMDBRemoteContext.h"
#include "HMDBSourceContextOperations.h"
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>
#include "MemoryStorage.h"

using namespace hmdbCommunication;

HMDBRemoteContext::HMDBRemoteContext(IHMDBSessionPtr session, const std::string & userHash)
	: session_(session), userHash(userHash)
{

}

HMDBRemoteContext::~HMDBRemoteContext()
{

}

const IHMDBSessionPtr HMDBRemoteContext::session()
{
	return session_;
}

const IHMDBSessionConstPtr HMDBRemoteContext::session() const
{
	return session_;
}

const HMDBRemoteContext::SynchronizeOperationPtr HMDBRemoteContext::prepareSynchronization(IHMDBStoragePtr storage)
{
	HMDBRemoteContext::SynchronizeOperationPtr ret;
	std::list<IHMDBRemoteContext::DownloadOperationPtr> downloads;	

	if (storage == nullptr){
		storage.reset(new MemoryStorage);
	}

	if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){

		IHMDBRemoteContext::CompoundID cid;
		cid.dataReference = IHMDBRemoteContext::Medical;
		cid.fileSize = 0;
		
		{
			cid.fileID = ShallowCopyUtils::MetadataFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Meta);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->medicalFilestoreman(), cid.fileID));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(cid, pftdOp, ptoOp, session_->medicalFtp(), storage)));			
		}

		{
			cid.fileID = ShallowCopyUtils::ShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Shallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->medicalFilestoreman(), cid.fileID));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(cid, pftdOp, ptoOp, session_->medicalFtp(), storage)));			
		}

	}

	if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
		IHMDBRemoteContext::CompoundID cid;
		cid.dataReference = IHMDBRemoteContext::Motion;
		cid.fileSize = 0;

		{
			cid.fileID = ShallowCopyUtils::MetadataFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Meta);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), cid.fileID));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(cid, pftdOp, ptoOp, session_->motionFtp(), storage)));
		}

		{
			cid.fileID = ShallowCopyUtils::ShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Shallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), cid.fileID));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(cid, pftdOp, ptoOp, session_->motionFtp(), storage)));			
		}

		{
			cid.fileID = ShallowCopyUtils::IncrementalShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::IncrementalShallow);
			utils::shared_ptr<MemoryTransferIO> ptoOp(new MemoryTransferIO());
			utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(session_->motionFilestoreman(), cid.fileID));
			downloads.push_back(IHMDBRemoteContext::DownloadOperationPtr(new FileDownload(cid, pftdOp, ptoOp, session_->motionFtp(), storage)));			
		}
	}	

	if (downloads.empty() == false){
		ret.reset(new SynchronizeOperation(downloads, storage));
	}

	return ret;
}

const HMDBRemoteContext::DownloadOperationPtr HMDBRemoteContext::prepareFileDownload(const CompoundID & fileID,
	IHMDBStoragePtr storage)
{
	HMDBRemoteContext::DownloadOperationPtr ret;

	hmdbServices::IFileStoremanWS * fsWS = nullptr;
	IHMDBFtp * ftp = nullptr;

	if (fileID.dataReference == IHMDBRemoteContext::Medical){
		if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){
			fsWS = session_->medicalFilestoreman();
			ftp = session_->medicalFtp();
		}
	}
	else{
		if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
			fsWS = session_->motionFilestoreman();
			ftp = session_->motionFtp();
		}		
	}

	if (fsWS != nullptr && ftp != nullptr){
		utils::shared_ptr<TmpFileTransferIO> ptoOp(new TmpFileTransferIO());
		utils::shared_ptr<PrepareHMDB> pftdOp(new PrepareHMDB(fsWS, fileID.fileID));
		ret.reset(new FileDownload(fileID, pftdOp, ptoOp, ftp, storage));
	}

	return ret;
}

const IHMDBFtp::TransferPtr HMDBRemoteContext::prepareFileUpload(const std::string & fileName,
	const std::string & path, IHMDBStorage::IStreamPtr source,
	const DataReference dataReference)
{

	if (dataReference == Motion){
		return session_->motionFtp()->preparePut(path + "/" + fileName, source);
	}
	else{
		return session_->medicalFtp()->preparePut(path + "/" + fileName, source);
	}
}