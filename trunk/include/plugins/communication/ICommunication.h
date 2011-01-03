/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod zwi¹zanych z komunikacj¹ klient - serwer. Za jego pomoc¹
mo¿liwe jest pobieranie informacji o próbach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokaln¹ maszyne.
*/

#ifndef ICOMMUNICATION_H
#define ICOMMUNICATION_H

class ICommunication
{
public:
	/**
	Wirtualny destruktor
	*/
    virtual ~ICommunication() {}
	/**
	Pobieranie informacji o próbach pomiarowych z serwera.
	*/
	virtual void updateSessionContents() = 0;
	/**
	Pobieranie pojedynczego pliku.
	@param fileID id pliku w bazie danych który ma byæ pobrany
	*/
	virtual void downloadFile(unsigned int fileID) = 0;
	/**
	Pobieranie wszystkich plików próby pomiarowej.
	@param trialID id triala w bazie danych którego pliki maj¹ byæ pobrane
	*/
	virtual void downloadTrial(unsigned int trialID) = 0;
	/**
	£adowanie lokalnej próby pomiarowej do edytora.
	@param name nazwa lokalnej próby do za³adowania
	*/
	virtual void loadTrial(const std::string& name) = 0;
	/**
	Pingowanie serwera.
	@return czy serwer odpowiedzia³?
	*/
	virtual bool ping() = 0;
	/**
	Przerwanie pobierania pliku lub triala.
	*/
	virtual void cancelDownloading() = 0;
};
#endif
