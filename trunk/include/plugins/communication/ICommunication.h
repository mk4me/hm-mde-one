/**
@author Marek Daniluk
@brief Interfejs ICommunication dostarcza metod zwi�zanych z komunikacj� klient - serwer. Za jego pomoc�
mo�liwe jest pobieranie informacji o pr�bach pomiarowych przechowywanych na serwerze, oraz pobieranie ich
na lokaln� maszyne.
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
	Pobieranie informacji o pr�bach pomiarowych z serwera.
	*/
	virtual void updateSessionContents() = 0;
	/**
	Pobieranie pojedynczego pliku.
	@param fileID id pliku w bazie danych kt�ry ma by� pobrany
	*/
	virtual void downloadFile(unsigned int fileID) = 0;
	/**
	Pobieranie wszystkich plik�w pr�by pomiarowej.
	@param trialID id triala w bazie danych kt�rego pliki maj� by� pobrane
	*/
	virtual void downloadTrial(unsigned int trialID) = 0;
	/**
	�adowanie lokalnej pr�by pomiarowej do edytora.
	@param name nazwa lokalnej pr�by do za�adowania
	*/
	virtual void loadTrial(const std::string& name) = 0;
	/**
	Pingowanie serwera.
	@return czy serwer odpowiedzia�?
	*/
	virtual bool ping() = 0;
	/**
	Przerwanie pobierania pliku lub triala.
	*/
	virtual void cancelDownloading() = 0;
};
#endif
