#include <networkUtils/CURLFTPHelper.h>

using namespace networkUtils;

void CURLFTPHelper::initializeFTPConnection(CURL * curl,
	const std::string & user,
	const std::string & password,
	const SSLHostVerification hostVerification,
	const std::string & caPath)
{
	std::string user_(user);
	std::string password_(password);

	//login, password
	if (user_.empty() == true) {
		user_ = "anonymous";
		password_ = "anonymous@localhost";
	}

	curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
	// tylko IPv4
	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

	//czy weryfikujemy serwer
	if (hostVerification != HVNone){
		// czy serwer ma weryfikowaæ nasz certyfikat
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
		// typ weryfikacji hosta
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, hostVerification);
		// je¿eli jest œcie¿ka certyfikatu to j¹ podajemy
		if (caPath.empty() == false){
			curl_easy_setopt(curl, CURLOPT_CAINFO, caPath.c_str());
		}
	}
	else{
		// czy serwer ma weryfikowaæ nasz certyfikat
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	}
}

size_t read(void* buffer, size_t size, size_t nmemb, void* stream)
{
	std::istream * istream = static_cast<std::istream*>(stream);
	auto offset = size * nmemb;
	try{
		istream->read((char*)buffer, offset);
		if (istream->bad() == true || istream->fail() == true){
			offset = 0;
		}
	}
	catch (...){
		offset = 0;
	}

	return offset;
}

size_t write(void *buffer, size_t size, size_t nmemb, void *stream)
{
	std::ostream * ostream = static_cast<std::ostream*>(stream);
	char * start = static_cast<char*>(buffer);
	auto offset = size * nmemb;
	char * end = start + offset;
	try{
		ostream->write((char*)buffer, offset);
		if (ostream->fail() == true || ostream->bad() == true){
			offset = 0;
		}
		else{
			ostream->flush();
		}
	}
	catch (...){
		offset = 0;
	}

	return offset;
}

int setDownloadCustomProgress(ICURLFTPProgress* progress, curl_off_t t, /* dltotal */ curl_off_t d, /* dlnow */ curl_off_t ultotal, curl_off_t ulnow)
{
	if (progress->aborted() == true) {
		return -1;
	}

	progress->setProcessedData(d >= 0 ? d : 0);
	progress->setProgress(t > 0 ? ((float)d / (float)t) : 1.0);
	return 0;
}

int setUploadCustomProgress(ICURLFTPProgress* progress, curl_off_t t, /* dltotal */ curl_off_t d, /* dlnow */ curl_off_t ultotal, curl_off_t ulnow)
{
	if (progress->aborted() == true) {
		return -1;
	}

	progress->setProcessedData(ulnow >= 0 ? ulnow : 0);
	progress->setProgress(ultotal > 0 ? ((float)ulnow / (float)ultotal) : 1.0);
	return 0;
}

void CURLFTPHelper::initializeFTPDownload(CURL * curl,
	const std::string & remoteFileUrl,
	std::ostream * localDestination,
	ICURLFTPProgress * progress)
{
	curl_easy_setopt(curl, CURLOPT_URL, remoteFileUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, localDestination);

	if (progress != nullptr){
		// przywracamy progres
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, progress);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, setDownloadCustomProgress);
	}
	else{
		// brak progesu domyœlnie
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	}
}

void CURLFTPHelper::initializeFTPUpload(CURL * curl,
	const std::string & remoteFileUrl,
	std::istream * localSource,
	ICURLFTPProgress * progress)
{
	curl_easy_setopt(curl, CURLOPT_URL, remoteFileUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, read);
	curl_easy_setopt(curl, CURLOPT_READDATA, localSource);

	if (progress != nullptr){
		// przywracamy progres
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, progress);
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, setUploadCustomProgress);		
	}
	else{
		// brak progesu domyœlnie
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	}
}