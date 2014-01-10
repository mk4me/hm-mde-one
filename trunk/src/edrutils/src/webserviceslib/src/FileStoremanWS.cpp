#include <webserviceslib/FileStoremanWS.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/DateTimeUtils.h>
#include <time.h>

namespace webservices
{

SimpleFileStoremanWS::SimpleFileStoremanWS(const WSConnectionPtr & connection)
	: WebServiceT<IBasicStoremanWS>(connection)
{
	
}

SimpleFileStoremanWS::~SimpleFileStoremanWS()
{

}

void SimpleFileStoremanWS::downloadComplete(const int id, const std::string & path)
{
	connection()->setOperation("DownloadComplete");
	connection()->setValue("fileID", id);
	connection()->setValue("path", path);
	connection()->invoke();
}

const xmlWsdl::FileData SimpleFileStoremanWS::retrieve(const int id)
{	
	connection()->setOperation("RetrieveFile");
	connection()->setValue("fileID", id);
	connection()->invoke(true);

	xmlWsdl::FileData ret;
	connection()->getValue("FileLocation", ret.fileLocation);
	connection()->getValue("SubdirPath", ret.subdirPath);

	return ret;
}


PhotoStoremanWS::PhotoStoremanWS(const WSConnectionPtr & connection)
	: WebServiceT<IBasicStoremanWS>(connection)
{

}

PhotoStoremanWS::~PhotoStoremanWS()
{

}

void PhotoStoremanWS::downloadComplete(const int id, const std::string & path)
{
	connection()->setOperation("DownloadComplete");
	connection()->setValue("photoID", id);
	connection()->setValue("path", path);
	connection()->invoke();
}

const xmlWsdl::FileData PhotoStoremanWS::retrieve(const int id)
{	
	connection()->setOperation("RetrievePhoto");
	connection()->setValue("photoID", id);
	connection()->invoke(true);

	xmlWsdl::FileData ret;
	connection()->getValue("FileLocation", ret.fileLocation);
	connection()->getValue("SubdirPath", ret.subdirPath);

	return ret;
}


ShallowStoremanWS::ShallowStoremanWS(const WSConnectionPtr & connection)
	: WebServiceT<IShallowStoremanWS>(connection)	
{
	
}

ShallowStoremanWS::~ShallowStoremanWS()
{

}

const std::string ShallowStoremanWS::getShallowCopy()
{
	connection()->setOperation("GetShallowCopy");
	connection()->invoke(true);

	std::string ret;
	connection()->getValue("GetShallowCopyResult", ret);

	return ret;
}

const std::string ShallowStoremanWS::getMetadata()
{	
	connection()->setOperation("GetMetadata");
	connection()->invoke(true);

	std::string ret;
	connection()->getValue("GetMetadataResult", ret);

	return ret;
}

void ShallowStoremanWS::downloadComplete(const std::string & path)
{	
	SimpleFileStoremanWS(connection()).downloadComplete(0, path);
}

MotionFileStoremanWS::MotionFileStoremanWS(const WSConnectionPtr & connection)
	: WebServiceT<IMotionFileStoremanWS>(connection)
{

}

MotionFileStoremanWS::~MotionFileStoremanWS()
{

}

void MotionFileStoremanWS::downloadComplete(const int fileID, const std::string & path)
{
	SimpleFileStoremanWS(connection()).downloadComplete(fileID, path);
}

void MotionFileStoremanWS::downloadComplete(const std::string & path)
{
	ShallowStoremanWS(connection()).downloadComplete(path);
}

const xmlWsdl::FileData MotionFileStoremanWS::retrieve(const int fileID)
{	
    return SimpleFileStoremanWS(connection()).retrieve(fileID);
}

const std::string MotionFileStoremanWS::getShallowCopy()
{	
    return ShallowStoremanWS(connection()).getShallowCopy();
}

const std::string MotionFileStoremanWS::getShallowCopyIncrement(const DateTime & dateTime)
{
    connection()->setOperation("GetShallowCopyIncrement");
    connection()->setValue("since", toString(dateTime));
    connection()->invoke(true);
    
	std::string ret;
    connection()->getValue("GetShallowCopyIncrementResult", ret);

    return ret;
}
		
const std::string MotionFileStoremanWS::getMetadata()
{
	return ShallowStoremanWS(connection()).getMetadata();
}

const int MotionFileStoremanWS::storeSessionFile(const int sessionID,
	const std::string & path, const std::string & description,
	const std::string & filename)
{
	connection()->setOperation("StoreSessionFile");
	connection()->setValue("sessionId", sessionID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->setValue("filename", filename);
	connection()->invoke(true);
		
	int val = -1;
	connection()->getValue("StoreSessionFileResult", val);

	return val;
}

void MotionFileStoremanWS::storeSessionFiles(const int sessionID,
	const std::string & path, const std::string & description)
{
	connection()->setOperation("StoreSessionFiles");
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->invoke();	
}

const int MotionFileStoremanWS::storeTrialFile(const int trialID,
	const std::string & path, const std::string & description,
	const std::string& filename)
{	
	connection()->setOperation("StoreTrialFile");
	connection()->setValue("trialID", trialID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->setValue("filename", filename);
	connection()->invoke(true);

	int val = -1;
	connection()->getValue("StoreTrialFileResult", val);

	return val;
	
}

void MotionFileStoremanWS::storeTrialFiles(const int trialID,
	const std::string & path, const std::string & description)
{
	connection()->setOperation("StoreTrialFiles");
	connection()->setValue("trialID", trialID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->invoke();
}

const int MotionFileStoremanWS::storeMeasurementConfFile(const int mcID,
	const std::string & path, const std::string & description,
	const std::string & filename)
{	
	connection()->setOperation("StoreMeasurementConfFile");
	connection()->setValue("mcID", mcID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->setValue("filename", filename);
	connection()->invoke(true);

	int val = -1;
	connection()->getValue("StoreMeasurementConfFileResult", val);

	return val;
}

void MotionFileStoremanWS::storeMeasurementConfFiles(const int mcID,
	const std::string & path, const std::string & description)
{
	connection()->setOperation("StoreMeasurementConfFiles");
	connection()->setValue("mcID", mcID);
	connection()->setValue("path", path);
	connection()->setValue("description", description);
	connection()->invoke();
}

void MotionFileStoremanWS::replaceFile( int fileID, const std::string& path, std::string& filename )
{
    connection()->setOperation("ReplaceFile");
    connection()->setValue("fileID", fileID);
    connection()->setValue("path", path);
    connection()->setValue("filename", filename);
    connection()->invoke();
}



//
//void MotionFileStoremanService::storePerformerFiles(int performerID, const std::string& path)
//{
//	this->setOperation("StorePerformerFiles");
//	if(invoker->status()) {
//		if(!invoker->setValue("performerID", toString<int>(performerID)) ||
//			!invoker->setValue("path", path/*new std::string(path)*/)) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker->invoke()) {
//			throw std::runtime_error(invoker->errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker->errors().c_str());
//	}
//}
//

MedicalFileStoremanWS::MedicalFileStoremanWS(const WSConnectionPtr & connection)
	: WebServiceT<IMedicalFileStoremanWS>(connection)
{
	
}

MedicalFileStoremanWS::~MedicalFileStoremanWS()
{

}

void MedicalFileStoremanWS::downloadComplete(const int fileID,
	const std::string & path)
{
	PhotoStoremanWS(connection()).downloadComplete(fileID, path);
}

void MedicalFileStoremanWS::downloadComplete(const std::string & path)
{	
	ShallowStoremanWS(connection()).downloadComplete(path);
}

const xmlWsdl::FileData MedicalFileStoremanWS::retrieve(const int fileID)
{
	return PhotoStoremanWS(connection()).retrieve(fileID);
}

const std::string MedicalFileStoremanWS::getShallowCopy()
{	
	return ShallowStoremanWS(connection()).getShallowCopy();
}

const std::string MedicalFileStoremanWS::getMetadata()
{
	return ShallowStoremanWS(connection()).getMetadata();
}

}
