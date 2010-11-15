# przygotowanie do szukania
FIND_INIT(OSG osg)
FIND_INCLUDE_PLATFORM_HEADERS(OSG osg)

# OSG
set(OSG_VERSIONS "67,66,65,64,63,62")
FIND_SHARED(OSG_LIBCORE osg "osg<${OSG_VERSIONS}>-osg")
FIND_SHARED(OSG_LIBDB osgDB "osg<${OSG_VERSIONS}>-osgDB")
FIND_SHARED(OSG_LIBUTIL osgUtil "osg<${OSG_VERSIONS}>-osgUtil")
FIND_SHARED(OSG_LIBGA osgGA "osg<${OSG_VERSIONS}>-osgGA")
FIND_SHARED(OSG_LIBVIEWER osgViewer "osg<${OSG_VERSIONS}>-osgViewer")
FIND_SHARED(OSG_LIBTEXT osgText "osg<${OSG_VERSIONS}>-osgText")
FIND_SHARED(OSG_WIDGET osgWidget "osg<${OSG_VERSIONS}>-osgWidget")

# OpenThreads
set(OPENTHREADS_VERSIONS "12,11")
FIND_SHARED(OSG_LIBOPENTHREADS OpenThreads "ot<${OPENTHREADS_VERSIONS}>-OpenThreads")

set(OSGPLUGINS_VERSIONS "2.9.10;2.9.9;2.9.8;2.9.7;2.9.6")
# jeœli wersja zosta³a wybrana, to zawê¿amy wybór
if (OSG_VERSION)
	set (OSGPLUGINS_VERSIONS ${OSG_VERSION})
endif()

# wyszukanie pluginów; powinny znajdowaæ siê w zagnie¿d¿onym katalogu
# algorytm wyszukiwania podmienia wewnêtrzne zmienne FIND_DIR_DEBUG/RELASE
# pozwalaj¹c tym samym na przeszukiwanie innych ni¿ domyœlne katalogów
set(FIND_DIR_DEBUG_BACKUP ${FIND_DIR_DEBUG})
set(FIND_DIR_RELASE_BACKUP ${FIND_DIR_RELEASE})
foreach (version ${OSGPLUGINS_VERSIONS})
	set(FIND_DIR_DEBUG "${FIND_DIR_DEBUG_BACKUP}/osgPlugins-${version}")
	set(FIND_DIR_RELEASE "${FIND_DIR_RELASE_BACKUP}/osgPlugins-${version}")
	FIND_MODULE(OSG_PLUGIN_PNG FALSE "osgdb_png")
	# ustawiamy domyœln¹ lokalizacjê pluginów
	if (OSG_PLUGIN_PNG_FOUND)
		set(OSG_VERSION "${version}" CACHE STRING "Subfolder for osg plugins in the target directory")
	endif()
endforeach()
# poza znalezieniem trzeba jeszcze ustawiæ suffix œcie¿ki docelowej
# dla pluginów
set(FIND_MODULE_PREFIX_osgdb_png "osgPlugins-${OSG_VERSION}/")
set(FIND_MODULE_PREFIX_osgdb_pngd "osgPlugins-${OSG_VERSION}/")

# skopiowanie
FIND_FINISH(OSG)

# sprawdzenie
if (OSG_LIBCORE_FOUND AND
	OSG_LIBDB_FOUND AND
	OSG_LIBOPENTHREADS_FOUND AND
	OSG_LIBUTIL_FOUND AND
	OSG_LIBGA_FOUND AND
	OSG_LIBVIEWER_FOUND AND
	OSG_LIBTEXT_FOUND AND
	OSG_WIDGET_FOUND AND
	OSG_PLUGIN_PNG_FOUND)
	set(OSG_FOUND 1)
else()
	message("Nie znaleziono którejœ z bibliotek osg (wersje ${OSG_VERSIONS}), OpenThreads(wersje ${OPENTHREADS_VERSIONS}) lub pluginów (wersje ${OSGPLUGINS_VERSIONS})")
endif()
