#include "CorePCH.h"
#include <QtCore/QDir>

class ResourcesInitializer
{
public:
	ResourcesInitializer()
	{
		//WA�NE!!
		//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak� bibliotek� i jego resourcy musza by� jawnie inicjalizowane
		//Nazwa resourc�w musi byc unikalna poniewa� Qt "miesza" nazwy metod z nazwamy plik�w resourc�w kt�re chcemy inicjalizowa� tworz�c unikalne statyczne funkcje na potrzeby inicjalizacji
		//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam d� stronki
		Q_INIT_RESOURCE(CoreIcons);
	}

	~ResourcesInitializer() {}
};

ResourcesInitializer resourceInitializer;
