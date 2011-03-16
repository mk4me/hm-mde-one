/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/FtpsConnection.h>
#include <core/Filesystem.h>

using namespace communication;

FtpsConnection::FtpsConnection()
{
    this->usr = "";
    this->pswd = "";
    this->uri = "";
    this->progress.progress = 0;
    this->progress.abort = false;

    curl_global_init(CURL_GLOBAL_ALL);
    this->curl = curl_easy_init();
    if(!this->curl) {
        throw std::runtime_error("Failed to initialize FTPS connection.");
    }
}

FtpsConnection::FtpsConnection(const std::string& uri, const std::string& usr, const std::string& pswd)
    : uri(uri), usr(usr), pswd(pswd)
{
    if(this->uri[this->uri.size() - 1] != '/') {
        this->uri.append("/");
    }
    this->progress.progress = 0;
    this->progress.abort = false;

    curl_global_init(CURL_GLOBAL_ALL);
    this->curl = curl_easy_init();
    if(!this->curl) {
        throw std::runtime_error("Failed to initialize FTPS connection.");
    }
}

FtpsConnection::~FtpsConnection()
{
    if(this->curl) {
        curl_easy_cleanup(this->curl);
    }
    curl_global_cleanup();
}

void FtpsConnection::setUri(const std::string& uri)
{
    this->uri = uri;
}

void FtpsConnection::setUser(const std::string& usr)
{
    this->usr = usr;
}

void FtpsConnection::setPassword(const std::string& pswd)
{
    this->pswd = pswd;
}

void FtpsConnection::setCredentials(const std::string& uri, const std::string& usr, const std::string& pswd)
{
    this->uri = uri;
    this->usr = usr;
    this->pswd = pswd;
}

const std::string& FtpsConnection::getUri() const
{
    return this->uri;
}

const std::string& FtpsConnection::getUser() const
{
    return this->usr;
}

const std::string& FtpsConnection::getPassword() const
{
    return this->pswd;
}

void FtpsConnection::get(const std::string& filename)
{
    progress.progress = 0;
    progress.abort = false;
    std::string temp;
    temp = filename;
    temp = temp.substr(temp.find_last_of("/") + 1);
    FtpFile ftpfile =
    {
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
    curl_easy_setopt(this->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(this->curl, CURLOPT_PROGRESSFUNCTION, setProgress);
    curl_easy_setopt(this->curl, CURLOPT_PROGRESSDATA, &this->progress);

    this->res = curl_easy_perform(this->curl);
    if(CURLE_OK != this->res) {
        if(res == CURLE_ABORTED_BY_CALLBACK) {
            if(ftpfile.stream) {
                fclose(ftpfile.stream);
            }
            core::Filesystem::deleteFile(temp);
        }
        throw std::runtime_error(curl_easy_strerror(this->res));
    }
    if(ftpfile.stream) {
        fclose(ftpfile.stream);
    }
}

void FtpsConnection::put(const std::string& filename)
{
    std::string temp;
    temp = filename;
    temp = temp.substr(temp.find_last_of("/") + 1);
    FtpFile ftpfile =
    {
        temp,
        NULL
    };
    std::string url(this->uri);
    url.append(filename);
    ftpfile.stream = fopen(temp.c_str(), "rb");
    if(ftpfile.stream == NULL) {
        throw std::runtime_error("No specified file.");
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

    this->res = curl_easy_perform(this->curl);
    if(CURLE_OK != this->res) {
        throw std::runtime_error(curl_easy_strerror(this->res));
    }
    if(ftpfile.stream) {
        fclose(ftpfile.stream);
    }
}

size_t FtpsConnection::read(void* buffer, size_t size, size_t nmemb, void* stream)
{
    size_t retcode = fread(buffer, size, nmemb, (FILE*)stream);
    return retcode;
}

size_t FtpsConnection::write(void* buffer, size_t size, size_t nmemb, void* stream)
{
    FtpFile* out = (FtpFile*)stream;
    if(out && !out->stream) {
        out->stream = fopen(out->filename.c_str(), "wb");
        if(!out->stream) {
            return -1;
        }
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

size_t FtpsConnection::setProgress(Progress* progress, double t, double d, double ultotal, double ulnow)
{
    if(progress->abort) {
        return -1;
    }
    progress->progress = static_cast<int>((d * 100.0)/t);
    return 0;
}

int FtpsConnection::getProgress() const
{
    return this->progress.progress;
}

void FtpsConnection::abort()
{
    progress.abort = true;
}
