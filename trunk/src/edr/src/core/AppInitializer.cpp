#include "CorePCH.h"
#include <QtCore/QDir>

class ResourcesInitializer
{
public:
	ResourcesInitializer()
	{
		//WAďż˝NE!!
		//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest takďż˝ bibliotekďż˝ i jego resourcy musza byďż˝ jawnie inicjalizowane
		//Nazwa resourcďż˝w musi byÄ‡ unikalna poniewaďż˝ Qt "miesza" nazwy metod z nazwamy plikďż˝w resourcďż˝w ktďż˝re chcemy inicjalizowaďż˝ tworzďż˝c unikalne statyczne funkcje na potrzeby inicjalizacji
		//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dďż˝ stronki
		Q_INIT_RESOURCE(CoreIcons);
	}

	~ResourcesInitializer() {}
};

ResourcesInitializer resourceInitializer;
