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
}

namespace plugin {

	class IPath;
	class IDataManagerReader;
	class IStreamDataManagerReader;
	class IFileDataManagerReader;
	class IDataHierarchyManagerReader;
	class IExtensionManagerReader;	
	class IPluginStorage;
	class IServiceManager;	

	//! Interfejs klasy dostarczaj¹cej pluginom ogólnych zasobów aplikacji:
	//! interfejs œcie¿ek, interfejs do czytania danych,
	//! interfejs do logowania oraz interfejs przechowywania bloków pamiêci o zadanym rozmiarze
	//! wg kluczy

	//! WA¯NE - pozosta³e elementy aplikacji których tutaj nie ma bêd¹ dostarczane poszczególnym modu³om indywidualnie
	//! wg przyjêtych regu³ i potrzeb
	class IApplication {

	public:

		//! \return Interfejs œcie¿ek aplikacji
		virtual IPath * pathInterface() = 0;
		//! \return Interfejs do logowania
		virtual core::ILog * log() = 0;
		//! \return Interfejs do czytania danych
		virtual IDataManagerReader * dataManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IStreamDataManagerReader * streamDataManagerReader() = 0;
		//! \return Interfejs do czytania danych strumieniowych
		virtual IFileDataManagerReader * fileDataManagerReader() = 0;
		//! \return Interfejs do czytania opisu zarejestrowanych danych
		virtual IDataHierarchyManagerReader * dataHierarchyManagerReader() = 0;
		//! \return Interfejs do czytania opisu zarejestrowanych rozszerzeñ plików przez parsery
		virtual IExtensionManagerReader * dataExtensionManagerReader() = 0;
		//! \return Interfejs do dedykowanej przestrzeni sk³¹dowania danych
		virtual IPluginStorage * pluginStorage() = 0;
		//! \return Interfejs dostêpu do serwisów
		virtual IServiceManager * serviceManager() = 0;
	};

}

#endif	//	HEADER_GUARD___IAPPLICATION_H__
