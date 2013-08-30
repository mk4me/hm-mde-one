/********************************************************************
    created:  2013/01/07
    created:  7:1:2013   12:31
    filename: IApplication.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IAPPLICATION_H__
#define HEADER_GUARD___IAPPLICATION_H__

namespace core {

	class ILog;
	class IPath;
	class IDataManagerReader;
	class IParserManagerReader;
	class IStreamManagerReader;
	class IFileManagerReader;
	class IDataHierarchyManagerReader;
	class IPluginStorage;
	class IServiceManager;	
	class ISourceManager;
	class IVisualizerManager;
	class IThreadPool;
	class IJobManager;

	//! Interfejs klasy dostarczaj�cej pluginom og�lnych zasob�w aplikacji:
	//! interfejs �cie�ek, interfejs do czytania danych,
	//! interfejs do logowania oraz interfejs przechowywania blok�w pami�ci o zadanym rozmiarze
	//! wg kluczy

	//! WA�NE - pozosta�e elementy aplikacji kt�rych tutaj nie ma b�d� dostarczane poszczeg�lnym modu�om indywidualnie
	//! wg przyj�tych regu� i potrzeb
	class IApplication {

	public:

		virtual ~IApplication() {}

		//! \return Interfejs �cie�ek aplikacji
		virtual IPath * pathInterface() = 0;
		//! \return Interfejs do logowania
		virtual ILog * log() = 0;
		//! \return Interfejs do czytania danych
		virtual IDataManagerReader * dataManagerReader() = 0;
		//! \return Interfejs do czytania informacji o parserach
		virtual IParserManagerReader * parserManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IStreamManagerReader * streamDataManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IFileManagerReader * fileDataManagerReader() = 0;
		//! \return Interfejs do czytania opisu zarejestrowanych danych
		virtual IDataHierarchyManagerReader * dataHierarchyManagerReader() = 0;
		//! \return Interfejs do dedykowanej przestrzeni sk��dowania danych
		virtual IPluginStorage * pluginStorage() = 0;
		//! \return Interfejs dost�pu do serwis�w
		virtual IServiceManager * serviceManager() = 0;
		//! \return Interfejs dost�pu do �r�de�
		virtual ISourceManager * sourceManager() = 0;
		//! \return Interfejs managera wizualizator�w
		virtual IVisualizerManager * visualizerManager() = 0;
		//! \return Interfejs puli w�tk�w
		virtual IThreadPool * threadPool() = 0;
		//! \return Interfejs managera zada�
		virtual IJobManager * jobManager() = 0;
	};

}

#endif	//	HEADER_GUARD___IAPPLICATION_H__
