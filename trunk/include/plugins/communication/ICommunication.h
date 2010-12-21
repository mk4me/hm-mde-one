/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod zwiazanych z komunikacja klient - serwer. Za jego pomoca
mozliwe jest pobieranie informacji o probach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokalna maszyne.
*/

#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

class ICommunication
{
public:
    virtual ~ICommunication() {}

    //! Pobieranie informacji o probach pomiarowych z serwera.
	virtual void updateSessionContents() = 0;

    //! Pobieranie pojedynczego pliku.
    //! \param fileID id pliku w bazie danych ktory ma byc pobrany
	virtual void downloadFile(unsigned int fileID) = 0;

    //! Pobieranie wszystkich plikow proby pomiarowej.
    //! \param trialID id triala w bazie danych ktorego pliki maja byc pobrane
	virtual void downloadTrial(unsigned int trialID) = 0;

    //! Ladowanie lokalnej proby pomiarowej do edytora.
    //! \param name nazwa lokalnej proby do zaladowania
	virtual void loadTrial(const std::string& name) = 0;

    //! Pingowanie serwera.
    //! \return czy serwer odpowiedzial?
	virtual bool ping() = 0;

    //! Przerwanie pobierania pliku lub triala.
	virtual void cancelDownloading() = 0;
};
#endif
