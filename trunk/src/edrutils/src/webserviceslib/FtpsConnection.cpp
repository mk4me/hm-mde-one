/**
@author Marek Daniluk
*/

#include <stdexcept>

#include <stdio.h>
#include <curl/curl.h>
#include <webserviceslib/FtpsConnection.h>

namespace webservices
{

//! Model PIMPL - chowam curla
class FtpsConnectionImpl
{
public:
    //! Domy�lny konstruktor
    FtpsConnectionImpl() : curl(nullptr)
    {
        progress_.progress = 0;
        progress_.abort = false;
        progress_.customProgress = nullptr;

        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();
        if(!curl) {
            curl_global_cleanup();
            throw std::runtime_error("Failed to initialize FTPS connection.");
        }
    }

    //! Destruktor
    ~FtpsConnectionImpl()
    {
        if(curl) {
            try{
                curl_easy_cleanup(curl);
            }catch(...){

            }
        }
        curl_global_cleanup();
    }

public:

    //! Metoda inicjalizuje obiekt
    void configure(const std::string & url = std::string(),const std::string & user = std::string(),
        const std::string & password = std::string())
    {
        url_ = url;
        user_ = user;
        password_  = password;

        if(url_[url_.size() - 1] != '/') {
            url_.append("/");
        }

    }

    FtpsConnection::OperationStatus get(const std::string & remoteSource, const std::string & localDestination, FtpsConnection::IProgress * customProgress)
    {
        FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
        std::string().swap(errorMessage_);
        progress_.progress = 0;
        progress_.abort = false;
        progress_.customProgress = customProgress;

        FILE* ftpfile = fopen(localDestination.c_str(), "wb");

        if(ftpfile == nullptr){
            errorMessage_ = "Could not create file for writing";
            return FtpsConnection::Error;
        }

        std::string url(url_);
        url.append(remoteSource);

        //This option determines whether curl verifies the authenticity of the
        //peer's certificate. A value of 1 means curl verifies; zero means it doesn't.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        //login, password
        if(user_.empty() == true) {
            user_ = "anonymous";
            password_ = "anonymous@localhost";
        }

        curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ftpfile);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        if(customProgress == nullptr){
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, setDownloadProgress);
        }else{
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, setDownloadCustomProgress);
        }

        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &progress_);

        res = curl_easy_perform(curl);

        fclose(ftpfile);

        if(CURLE_OK != res) {

            try{
                remove(localDestination.c_str());
            }catch(...){

            }

            if(res == CURLE_ABORTED_BY_CALLBACK) {
                ret = FtpsConnection::Cancelled;
            }else{
                errorMessage_ = curl_easy_strerror(res);
                ret = FtpsConnection::Error;
            }
        }

        curl_easy_reset(curl);

        return ret;
    }

    FtpsConnection::OperationStatus put(const std::string& localSource, const std::string& remoteDestination, FtpsConnection::IProgress * customProgress)
    {
        FtpsConnection::OperationStatus ret = FtpsConnection::Complete;
        std::string().swap(errorMessage_);

        std::string url(url_);
        url.append(remoteDestination);
        FILE * ftpfile = fopen(localSource.c_str(), "rb");
        if(ftpfile == nullptr) {
            //throw std::runtime_error("No specified file.");
            errorMessage_ = "Could not open specyfied file";
            return FtpsConnection::Error;
        }
        //This option determines whether curl verifies the authenticity of the
        //peer's certificate. A value of 1 means curl verifies; zero means it doesn't.
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        //login, password
        if(user_.empty() == true) {
            user_ = "anonymous";
            password_ = "anonymous@localhost";
        }

        curl_easy_setopt(curl, CURLOPT_USERNAME, user_.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password_.c_str());
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read);
        curl_easy_setopt(curl, CURLOPT_READDATA, ftpfile);

        if(customProgress == nullptr){
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, setUploadProgress);
        }else{
            curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, setUploadCustomProgress);
        }

        res = curl_easy_perform(curl);

        fclose(ftpfile);

        if(CURLE_OK != res) {

            if(res == CURLE_ABORTED_BY_CALLBACK) {
                ret = FtpsConnection::Cancelled;
            }else{
                errorMessage_ = curl_easy_strerror(res);
                ret = FtpsConnection::Error;
            }
        }

        curl_easy_reset(curl);

        return ret;
    }

private:

    /**
    @author Marek Daniluk
    @brief Struktura pozwalaj�ca na anulowanie operacji. Przechowuje te� informacj� o post�pie operacji.
    */
    struct Progress
    {
        /**
        Post�p
        */
        int progress;

        /**
        Postep szczegolowy wg klienta
        */
        FtpsConnection::IProgress * customProgress;

        /**
        czy anulowa�?
        */
        bool abort;
    };
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy przesy�aniu danych na serwer.
    Daje nam informacj� zwrotn� o ilo�ci danych wys�anych na serwer.
    @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba element�w do odczytania
    @param stream wska�nik na strumie� danych
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t read(void* buffer, size_t size, size_t nmemb, void* stream)
    {
        return fread(buffer, size, nmemb, (FILE*)stream);
    }
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy odbieraniu danych z serwera.
    Daje nam informacj� zwrotn� o ilo�ci danych pobranych z serwera.
    @param buffer wska�nik do bloku pami�ci o rozmiarze size*nmemb
    @param size rozmiar w bajtach elementu do odczytania
    @param nmemb liczba element�w do odczytania
    @param stream wska�nik na strumie� danych
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t write(void *buffer, size_t size, size_t nmemb, void *stream)
    {
        return fwrite(buffer, size, nmemb, (FILE*)stream);
    }
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji post�pu.
    Daje nam informacje zwrotna o procentowym post�pie aktualnej operacji.
    @param progress wska�nik na struktur� typu Progress
    @param t wielo�� ca�kowita pliku pobieranego
    @param d obecna ilo�� pobranych bajt�w
    @param ultotal wielko�� ca�kowita pliku wysy�anego
    @param ulnow obecna ilo�� wys�anych bajt�w
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t setDownloadProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow)
    {
        if(progress->abort) {
            return -1;
        }
        progress->progress = static_cast<int>((d * 100.0)/t);
        return 0;
    }
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji costomowego post�pu przez interfejs IProgress.
    Daje nam informacje zwrotna o procentowym post�pie aktualnej operacji.
    @param progress wska�nik na struktur� typu Progress
    @param t wielo�� ca�kowita pliku pobieranego
    @param d obecna ilo�� pobranych bajt�w
    @param ultotal wielko�� ca�kowita pliku wysy�anego
    @param ulnow obecna ilo�� wys�anych bajt�w
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t setDownloadCustomProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow)
    {
        if(progress->abort || progress->customProgress->aborted()) {
            return -1;
        }

        double val = (d * 100.0) / t;

        progress->progress = static_cast<int>(val);

        progress->customProgress->setProgress(val);

        return 0;
    }
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji post�pu.
    Daje nam informacje zwrotna o procentowym post�pie aktualnej operacji.
    @param progress wska�nik na struktur� typu Progress
    @param t wielo�� ca�kowita pliku pobieranego
    @param d obecna ilo�� pobranych bajt�w
    @param ultotal wielko�� ca�kowita pliku wysy�anego
    @param ulnow obecna ilo�� wys�anych bajt�w
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t setUploadProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow)
    {
        if(progress->abort) {
            return -1;
        }
        progress->progress = static_cast<int>((ulnow * 100.0)/ultotal);
        return 0;
    }
    /**
    Metoda statyczna (wymagana przez curla) typu callback wymagana przy aktualizacji costomowego post�pu przez interfejs IProgress.
    Daje nam informacje zwrotna o procentowym post�pie aktualnej operacji.
    @param progress wska�nik na struktur� typu Progress
    @param t wielo�� ca�kowita pliku pobieranego
    @param d obecna ilo�� pobranych bajt�w
    @param ultotal wielko�� ca�kowita pliku wysy�anego
    @param ulnow obecna ilo�� wys�anych bajt�w
    @return ilo�� bajt�w przetworzonych przez funkcj�
    */
    static size_t setUploadCustomProgress(Progress* progress, double t, /* dltotal */ double d, /* dlnow */ double ultotal, double ulnow)
    {
        if(progress->abort || progress->customProgress->aborted()) {
            return -1;
        }

        double val = (ulnow * 100.0)/ultotal;

        progress->progress = static_cast<int>(val);

        progress->customProgress->setProgress(val);

        return 0;
    }

public:
    /**
    Pole klasy przechowuj�ce url do serwera FTPS.
    */
    std::string url_;
    /**
    Pole klasy przechowuj�ce nazw� u�ytkownika.
    */
    std::string user_;
    /**
    Pole klasy przechowuj�ce has�o u�ytkownika.
    */
    std::string password_;
    /**
    Pole klasy przechowuj�ce informacje o procentowym post�pie operacji. S�u�y te� do przerwa� operacji.
    */
    Progress progress_;

    //! Informacja o b��dzie
    std::string errorMessage_;

    /**
    Wska�nik na obiekt typu CURL potrzebny do operacji ftpowych.
    */
    CURL* curl;
    /**
    Pole klasy przechowuj�ce wyniki dzia�ania funkcji biblioteki LibCurl.
    */
    CURLcode res;
};

FtpsConnection::FtpsConnection() : impl(new FtpsConnectionImpl())
{

}

FtpsConnection::FtpsConnection(const std::string & url, const std::string & user, const std::string & password)
    : impl(new FtpsConnectionImpl())
{
    impl->configure(url, user, password);
}

FtpsConnection::~FtpsConnection()
{

}

void FtpsConnection::setUrl(const std::string & url)
{
    impl->url_ = url;
}

void FtpsConnection::setUser(const std::string & user)
{
    impl->user_ = user;
}

void FtpsConnection::setPassword(const std::string & password)
{
    impl->password_ = password;
}

void FtpsConnection::setCredentials(const std::string & user, const std::string & password)
{
    impl->configure(impl->url_, user, password);
}

const std::string FtpsConnection::url() const
{
    return impl->url_;
}

const std::string FtpsConnection::user() const
{
    return impl->user_;
}

const std::string FtpsConnection::password() const
{
    return impl->password_;
}

const std::string FtpsConnection::errorMessage() const
{
    return impl->errorMessage_;
}

const FtpsConnection::OperationStatus FtpsConnection::get(const std::string& remoteSource, const std::string& localDestination, IProgress * customProgress)
{
    return impl->get(remoteSource, localDestination, customProgress);
}

const FtpsConnection::OperationStatus FtpsConnection::put(const std::string& localSource, const std::string& remoteDestination, IProgress * customProgress)
{
    return impl->put(localSource, remoteDestination, customProgress);
}

const int FtpsConnection::progress() const
{
    return impl->progress_.progress;
}

void FtpsConnection::abort()
{
    impl->progress_.abort = true;
}

}
