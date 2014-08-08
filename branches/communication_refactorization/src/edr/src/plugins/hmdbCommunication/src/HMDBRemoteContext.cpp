#include "CommunicationPCH.h"
#include "HMDBRemoteContext.h"
#include "HMDBSourceContextOperations.h"
#include <hmdbserviceslib/IFileStoremanWS.h>
#include <plugins/hmdbCommunication/IHMDBDataContext.h>
#include <plugins/hmdbCommunication/IHMDBSession.h>

using namespace hmdbCommunication;

HMDBRemoteContext::HMDBRemoteContext(IHMDBSession * session, const std::string & userHash)
	: session_(session), userHash(userHash)
{

}

HMDBRemoteContext::~HMDBRemoteContext()
{

}

IHMDBSession * HMDBRemoteContext::session()
{
	return session_;
}

const IHMDBSession * HMDBRemoteContext::session() const
{
	return session_;
}

IHMDBRemoteContext::IDownloadOperation * createDownload(hmdbServices::IFileStoremanWS * fs,
	IHMDBFtp * ftp, IHMDBStorage * storage, const IHMDBRemoteContext::CompoundID cid)
{
	return new CompoundFileDownload(cid,
		new PrepareFileToDownloadOperation(fs, cid.fileID),
		new PrepareMemoryTransferOutputOperation(),
		new FTPDownloadFileOperation(ftp),
		new StoreOutputOperation(storage, cid.fileName));
}

const HMDBRemoteContext::SynchronizeOperationPtr HMDBRemoteContext::prepareShallowCopyDownload(IHMDBStorage * storage)
{
	HMDBRemoteContext::SynchronizeOperationPtr ret;
	std::list<IHMDBRemoteContext::IDownloadOperation*> downloads;

	if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){

		IHMDBRemoteContext::CompoundID cid;
		cid.dataReference = IHMDBRemoteContext::Medical;
		cid.fileSize = 0;
		
		{
			cid.fileID = ShallowCopyUtils::MetadataFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Meta);

			downloads.push_back(createDownload(session_->medicalFilestoreman(), session_->medicalFtp(), storage, cid));
		}

		{
			cid.fileID = ShallowCopyUtils::ShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Shallow);

			downloads.push_back(createDownload(session_->medicalFilestoreman(), session_->medicalFtp(), storage, cid));
		}

	}

	if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
		IHMDBRemoteContext::CompoundID cid;
		cid.dataReference = IHMDBRemoteContext::Motion;
		cid.fileSize = 0;

		{
			cid.fileID = ShallowCopyUtils::MetadataFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Meta);

			downloads.push_back(createDownload(session_->motionFilestoreman(), session_->motionFtp(), storage, cid));
		}

		{
			cid.fileID = ShallowCopyUtils::ShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::Shallow);

			downloads.push_back(createDownload(session_->motionFilestoreman(), session_->motionFtp(), storage, cid));
		}

		{
			cid.fileID = ShallowCopyUtils::IncrementalShallowCopyFileID;
			cid.fileName = ShallowCopyUtils::shallowCopyName(userHash, cid.dataReference, ShallowCopyUtils::IncrementalShallow);

			downloads.push_back(createDownload(session_->motionFilestoreman(), session_->motionFtp(), storage, cid));
		}
	}	

	if (downloads.empty() == false){
		ret.reset(new SynchronizeOperation(downloads));
	}

	return ret;
}

const HMDBRemoteContext::DownloadOperationPtr HMDBRemoteContext::prepareFileDownload(const CompoundID & fileID,
	IHMDBStorage * storage)
{
	HMDBRemoteContext::DownloadOperationPtr ret;

	if (fileID.dataReference == IHMDBRemoteContext::Medical){
		if (session_->medicalFilestoreman() != nullptr && session_->medicalFtp() != nullptr){
			ret.reset(new CompoundFileDownload(fileID,
				new PrepareFileToDownloadOperation(session_->medicalFilestoreman(), fileID.fileID),
				new PrepareTmpFileTransferOutputOperation(),
				new FTPDownloadFileOperation(session_->medicalFtp()),
				new StoreOutputOperation(storage, fileID.fileName)));
		}
	}
	else{
		if (session_->motionFilestoreman() != nullptr && session_->motionFtp() != nullptr){
			ret.reset(new CompoundFileDownload(fileID,
				new PrepareFileToDownloadOperation(session_->motionFilestoreman(), fileID.fileID),
				new PrepareTmpFileTransferOutputOperation(),
				new FTPDownloadFileOperation(session_->motionFtp()),
				new StoreOutputOperation(storage, fileID.fileName)));
		}		
	}

	return ret;
}

const IHMDBRemoteContext::TransferOperationPtr HMDBRemoteContext::prepareFileUpload(const std::string & fileName,
	const std::string & path, std::istream * source,
	const DataReference dataReference)
{
	utils::shared_ptr<TransferOperation> ret(new TransferOperation());

	if (dataReference == Motion){
		ret->setTransfer(session_->motionFtp()->preparePut(path + "/" + fileName, source));
	}
	else{
		ret->setTransfer(session_->medicalFtp()->preparePut(path + "/" + fileName, source));
	}

	return ret;
}