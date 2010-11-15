/**
@author Marek Daniluk
*/

#include "FTPS_cURL.h"

using namespace EDRCM;

//temp
std::string const FTPS_cURL::ftps_addr = "ftps://db-bdr.pjwstk.edu.pl/";
std::string const FTPS_cURL::ftps_usr = "testUser";
std::string const FTPS_cURL::ftps_pswd = "testUser";

FTPS_cURL::FTPS_cURL() {
	this->usr = "";
	this->pswd = "";
	this->uri = "";

	curl_global_init(CURL_GLOBAL_ALL);
	this->curl = curl_easy_init();
	if(!this->curl) {
		throw EDRException("Failed to initialize FTPS connection.");
	}
}

FTPS_cURL::FTPS_cURL(const std::string& uri, const std::string& usr, const std::string& pswd) {
	this->usr = usr;
	this->pswd = pswd;
	this->uri = uri;
	if(this->uri[this->uri.size() - 1] != '/') {
		this->uri.append("/");
	}

	curl_global_init(CURL_GLOBAL_ALL);
	this->curl = curl_easy_init();
	if(!this->curl) {
		throw EDRException("Failed to initialize FTPS connection.");
	}
}

FTPS_cURL::~FTPS_cURL() {
	if(this->curl) {
		curl_easy_cleanup(this->curl);
	}
	curl_global_cleanup();
}

void FTPS_cURL::setUri(const std::string& uri) {
	this->uri = uri;
}

void FTPS_cURL::setUser(const std::string& usr) {
	this->usr = usr;
}

void FTPS_cURL::setPassword(const std::string& pswd) {
	this->pswd = pswd;
}

void FTPS_cURL::setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd) {
	this->uri = uri;
	this->usr = usr;
	this->pswd = pswd;
}

const std::string& FTPS_cURL::getUri() const {
	return this->uri;
}

const std::string& FTPS_cURL::getUser() const {
	return this->usr;
}

const std::string& FTPS_cURL::getPassword() const {
	return this->pswd;
}

void FTPS_cURL::get(const std::string& filename) {
	std::string temp;
	temp = filename;
	temp = temp.substr(temp.find_last_of("/") + 1);
	FtpFile ftpfile = {
		temp,
		NULL
	};

	std::string url(this->uri);
	url.append(filename);

	//This option determines whether curl verifies the authenticity of the
	//peer's certificate. A value of 1 means curl verifies; zero means it doesn't.
	curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//login, password
	if(this->usr.length() <= 0) {
		this->usr = "anonymous";
		this->pswd = "anonymous@localhost";
	}
	curl_easy_setopt(this->curl, CURLOPT_USERNAME, this->usr.c_str());
	curl_easy_setopt(this->curl, CURLOPT_PASSWORD, this->pswd.c_str());
	curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, write);
	curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &ftpfile);
#ifdef _DEBUG
	curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
#endif
	this->res = curl_easy_perform(this->curl);
	if(CURLE_OK != this->res) {
		throw EDRException(curl_easy_strerror(this->res));
	}
	if(ftpfile.stream) {
		fclose(ftpfile.stream);
	}
}

void FTPS_cURL::put(const std::string& filename) {
	std::string temp;
	temp = filename;
	temp = temp.substr(temp.find_last_of("/") + 1);
	FtpFile ftpfile = {
		temp,
		NULL
	};
	std::string url(this->uri);
	url.append(filename);
	ftpfile.stream = fopen(temp.c_str(), "rb");
	if(ftpfile.stream == NULL) {
		throw EDRException("No specified file.");
	}
	//This option determines whether curl verifies the authenticity of the
	//peer's certificate. A value of 1 means curl verifies; zero means it doesn't.
	curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	//login, password
	if(this->usr.length() <= 0) {
		this->usr = "anonymous";
		this->pswd = "anonymous@localhost";
	}
	curl_easy_setopt(this->curl, CURLOPT_USERNAME, this->usr.c_str());
	curl_easy_setopt(this->curl, CURLOPT_PASSWORD, this->pswd.c_str());
	curl_easy_setopt(this->curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
	curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(this->curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(this->curl, CURLOPT_READFUNCTION, read);
	curl_easy_setopt(this->curl, CURLOPT_READDATA, ftpfile.stream);
#ifdef _DEBUG
	curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
#endif
	this->res = curl_easy_perform(this->curl);
	if(CURLE_OK != this->res) {
		throw EDRException(curl_easy_strerror(this->res));
	}
	if(ftpfile.stream) {
		fclose(ftpfile.stream);
	}
}

size_t FTPS_cURL::read(void* buffer, size_t size, size_t nmemb, void* stream) {
	size_t retcode = fread(buffer, size, nmemb, (FILE*)stream);
	return retcode;
}

size_t FTPS_cURL::write(void* buffer, size_t size, size_t nmemb, void* stream) {
	FtpFile* out = (FtpFile*)stream;
	if(out && !out->stream) {
		out->stream = fopen(out->filename.c_str(), "wb");
		if(!out->stream) {
			return -1;
		}
	}
	return fwrite(buffer, size, nmemb, out->stream);
}

void FTPS_cURL::cd(const std::string& directory) {
	throw EDRException("not supported yet.");
}

const std::string& FTPS_cURL::pwd() const {
	throw EDRException("not supported yet.");
}

std::vector<const std::string&>& FTPS_cURL::ls() const {
	throw EDRException("not supported yet.");
}

void FTPS_cURL::make(const std::string& dirname) {
	throw EDRException("not supported yet.");
}

void FTPS_cURL::del(const std::string& filename) {
	throw EDRException("not supported yet.");
}

void FTPS_cURL::disconnect() {
	throw EDRException("not supported yet.");
}
