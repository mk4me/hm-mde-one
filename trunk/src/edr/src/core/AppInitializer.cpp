#include "CorePCH.h"
#include <QtCore/QDir>

class ResourcesInitializer
{
public:
	ResourcesInitializer()
	{
		//WAŻNE!!
		//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest taką biblioteką i jego resourcy musza być jawnie inicjalizowane
		//Nazwa resourców musi być unikalna ponieważ Qt "miesza" nazwy metod z nazwamy plików resourców które chcemy inicjalizować tworząc unikalne statyczne funkcje na potrzeby inicjalizacji
		//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dół stronki
		Q_INIT_RESOURCE(CoreIcons);
	}

	~ResourcesInitializer() {}
};

ResourcesInitializer resourceInitializer;
