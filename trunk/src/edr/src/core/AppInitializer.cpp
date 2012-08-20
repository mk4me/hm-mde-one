#include "CorePCH.h"

class ResourcesInitializer
{
public:
	ResourcesInitializer()
	{
		//WA¯NE!!
		//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak¹ bibliotek¹ i jego resourcy musza byæ jawnie inicjalizowane
		//Nazwa resourców musi byc unikalna poniewa¿ Qt "miesza" nazwy metod z nazwamy plików resourców które chcemy inicjalizowaæ tworz¹c unikalne statyczne funkcje na potrzeby inicjalizacji
		//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dó³ stronki
		Q_INIT_RESOURCE(CoreIcons);
	}

	~ResourcesInitializer() {}
};

ResourcesInitializer resourceInitializer;