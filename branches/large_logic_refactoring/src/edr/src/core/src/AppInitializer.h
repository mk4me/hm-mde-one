/********************************************************************
	created:	2011/07/29
	created:	29:7:2011   10:41
	filename: 	AppInitializer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__APPINITIALIZER_H__
#define HEADER_GUARD_CORE__APPINITIALIZER_H__
#include "PluginLoader.h"
#include <utils/Push.h>

DEFINE_CORE_APPLICATION_ACCESSOR;

namespace core {
class AppInitializer
{
public:

    //! Metoda uruchamiająca aplikacje, pobiera jako parametr wzorca widok który będzie uruchomiony, widok powinien dziedziczyć po MainWindow
	template<class FrontpageWidget>
	static int start(int & argc, char *argv[])
	{
		//using namespace core;
	    // rev - statyczna
        //UTILS_STATIC_ASSERT((boost::is_base_of<MainWindow, FrontpageWidget>::value), "Klasa widoku musi dziedziczyć po klasie MainWindow");

		int result = 0;

		shared_ptr<Application> coreApplication(new Application());
		utils::Push<Application*> app(__application, coreApplication.get());

		try{
			if(coreApplication->initUIContext(argc, argv) == 0){
				try{
					// tworzymy widok
					FrontpageWidget widget;
					//inicjalizujemy widok
					coreApplication->initWithUI(&widget);

					result = coreApplication->run();
				}catch(...){
					CORE_LOG_ERROR("Unknown error while UI initialization or run");
					result = -1;
				}
			}else{
				result = -2;
			}
		}catch(...){
			result = -3;
		}

		return result;
	}
};
}
#endif
