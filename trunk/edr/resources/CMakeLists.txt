project(Resources)

set(ALL_RESURCES)

# ustawienia
set(SETTINGS 
	settings/log.ini
)
source_group("settings" FILES ${SETTINGS})
set(ALL_RESURCES ${ALL_RESURCES} ${SETTINGS})

# modele
set(MESHES 
	meshes/fixed_biped.fmesh
	meshes/Alien.tbs
)
source_group("meshes" FILES ${MESHES})
set(ALL_RESURCES ${ALL_RESURCES} ${MESHES})


# pliki shaderów
set(SHADERS 
	shaders/textureRect_yuv_to_rgb.frag
	shaders/texture2D_yuv_to_rgb.frag
	shaders/lighting.vert
	shaders/lighting.frag
)
source_group("shaders" FILES ${SHADERS})
set(ALL_RESURCES ${ALL_RESURCES} ${SHADERS})

set(SCHEMAS
	schemas/http.xsd
	schemas/mime.xsd
	schemas/schema.dtd
	schemas/soap.xsd
	schemas/soap12.xsd
	schemas/soap-encoding.xsd
	schemas/soap-encoding12.xsd
	schemas/wsdl10.xsd
)
source_group("schemas" FILES ${SCHEMAS})
set(ALL_RESURCES ${ALL_RESURCES} ${SCHEMAS})

set(TRIAL
	trial/walkMoCap2.amc
	trial/walkMoCap2.c3d
	trial/walkMoCapVideo.amc
	trial/walkMoCapVideo.asf
	trial/walkMoCapVideo.c3d
)
source_group("trial" FILES ${TRIAL})
set(ALL_RESURCES ${ALL_RESURCES} ${TRIAL})

# style qt
set(APPSKINS 
	app_skins/dark.qss
)
source_group("appskins" FILES ${APPSKINS})
set(ALL_RESURCES ${ALL_RESURCES} ${APPSKINS})

# ikony
file(GLOB ICONS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/icons/*.png")
source_group("icons" FILES ${ICONS})
set(ALL_RESURCES ${ALL_RESURCES} ${ICONS})

# obrazki
file(GLOB IMAGES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/images/*.png")
source_group("images" FILES ${IMAGES})
set(ALL_RESURCES ${ALL_RESURCES} ${IMAGES})

# czy kopiowaæ zasoby?
option(PROJECT_ENABLE_SVN_RESOURCES_COPYING "Enable copying resources?" ON)
if(PROJECT_ENABLE_SVN_RESOURCES_COPYING)

	# stworzenie domyœlnej listy katalogó
	if ( WIN32 )
		set(_DEFAULT_RESOURCES_LOC)
		foreach (buildType ${CMAKE_CONFIGURATION_TYPES})
			list(APPEND _DEFAULT_RESOURCES_LOC "${PROJECT_BUILD_ROOT}/bin/${buildType}/data/resources")
		endforeach()
	else()
		set(_DEFAULT_RESOURCES_LOC "${PROJECT_BUILD_ROOT}/bin/Debug/data/resources")
	endif()
	
	# tak, trzeba wskazaæ œcie¿kê, gdzie to ma nast¹piæ
	set(PROJECT_SVN_RESOURCES_DESTINATION ${_DEFAULT_RESOURCES_LOC} CACHE PATH "Location of resources being part of repository.")
	
	# dodanie projektu, który kopiuje zawartoœæ bie¿¹cego katalogu
	add_custom_target(resources
		SOURCES ${ALL_RESURCES}
	)
	
	# dodanie poleceñ kopiowania
	foreach (path ${PROJECT_SVN_RESOURCES_DESTINATION})
		# wyjaœnienie: komentarz dodajemy ze wszystkimi œcie¿kami, poniewa¿ w VS i tak tylko pierwszy komentarz
		# jest uwzglêdniony
		add_custom_command(TARGET resources PRE_BUILD 
			COMMAND "@CMAKE_COMMAND@" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}" "${path}"
			COMMENT "Copying resources to ${PROJECT_SVN_RESOURCES_DESTINATION}"
		)
	endforeach()
	
	#message("COMMAND @CMAKE_COMMAND@ -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${MOTION_SVN_RESOURCES_DESTINATION}")
else()
	# dodanie projektu bez ¿adnego polecenia - mo¿na ge edytowaæ z IDE
	add_custom_target(resources 
		SOURCES ${ALL_RESURCES}
	)
endif()