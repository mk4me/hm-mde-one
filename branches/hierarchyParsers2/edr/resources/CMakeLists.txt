#project(Resources)

set(ALL_RESURCES)

# ustawienia
set(SETTINGS 
	deploy/settings/log.ini
)
source_group("deploy\\settings" FILES ${SETTINGS})
set(ALL_RESURCES ${ALL_RESURCES} ${SETTINGS})

# pliki shaderów
set(SHADERS 
	deploy/shaders/textureRect_yuv_to_rgb.frag
	deploy/shaders/texture2D_yuv_to_rgb.frag
	deploy/shaders/lighting.vert
	deploy/shaders/lighting.frag
)
source_group("deploy\\shaders" FILES ${SHADERS})
set(ALL_RESURCES ${ALL_RESURCES} ${SHADERS})

# schematy dla wsdlpull
set(SCHEMAS
	deploy/schemas/http.xsd
	deploy/schemas/mime.xsd
	deploy/schemas/schema.dtd
	deploy/schemas/soap.xsd
	deploy/schemas/soap12.xsd
	deploy/schemas/soap-encoding.xsd
	deploy/schemas/soap-encoding12.xsd
	deploy/schemas/wsdl10.xsd
	deploy/schemas/measurementconfs.xml
)
source_group("deploy\\schemas" FILES ${SCHEMAS})
set(ALL_RESURCES ${ALL_RESURCES} ${SCHEMAS})

# pliki vsk opisuj¹ce modele szkieletowe
set(TRIAL
	deploy/trial/CZD.vsk
	deploy/trial/M39.vsk
	deploy/trial/M53.vsk
)
source_group("deploy\\trial" FILES ${TRIAL})
set(ALL_RESURCES ${ALL_RESURCES} ${TRIAL})

# style qt
set(APPSKINS 
	deploy/app_skins/dark.qss
	deploy/app_skins/hmm.qss
)
source_group("deploy\\appskins" FILES ${APPSKINS})
set(ALL_RESURCES ${ALL_RESURCES} ${APPSKINS})

# ikony
file(GLOB ICONS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/embedded/icons/*.png")
source_group("embedded\\icons" FILES ${ICONS})
set(ALL_RESURCES ${ALL_RESURCES} ${ICONS})

# ikony vdf
#file(GLOB ICONSVDF RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/deploy/icons/vdf/*.png")
#source_group("deploy\\iconsvdf" FILES ${ICONSVDF})
#set(ALL_RESURCES ${ALL_RESURCES} ${ICONSVDF})

# obrazki
file(GLOB IMAGES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/deploy/images/*.png")
source_group("deploy\\images" FILES ${IMAGES})
set(ALL_RESURCES ${ALL_RESURCES} ${IMAGES})

# jêzyki
file(GLOB LANGS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/deploy/lang/lang_*.ts")
source_group("deploy\\lang" FILES ${LANGS})
set(ALL_RESURCES ${ALL_RESURCES} ${LANGS})

# czy kopiowaæ zasoby?
option(SOLUTION_ENABLE_SVN_RESOURCES_COPYING "Enable copying resources?" ON)
if(SOLUTION_ENABLE_SVN_RESOURCES_COPYING)

	# stworzenie domyœlnej listy katalogów
	if ( WIN32 )
		set(_DEFAULT_RESOURCES_LOC)
		foreach (buildType ${CMAKE_CONFIGURATION_TYPES})
			list(APPEND _DEFAULT_RESOURCES_LOC "${PROJECT_BUILD_ROOT}/bin/${buildType}/resources")
		endforeach()
	else()
		set(_DEFAULT_RESOURCES_LOC "${PROJECT_BUILD_ROOT}/bin/Debug/resources")
	endif()
	
	# tak, trzeba wskazaæ œcie¿kê, gdzie to ma nast¹piæ
	set(SOLUTION_SVN_RESOURCES_DESTINATION ${_DEFAULT_RESOURCES_LOC} CACHE PATH "Location of resources being part of repository.")
	
	# dodanie projektu, który kopiuje zawartoœæ bie¿¹cego katalogu
	add_custom_target(resources
		SOURCES ${ALL_RESURCES}
	)
	
	# dodanie poleceñ kopiowania
	foreach (path ${SOLUTION_SVN_RESOURCES_DESTINATION})
		# wyjaœnienie: komentarz dodajemy ze wszystkimi œcie¿kami, poniewa¿ w VS i tak tylko pierwszy komentarz
		# jest uwzglêdniony
		add_custom_command(TARGET resources PRE_BUILD 
			COMMAND "@CMAKE_COMMAND@" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/deploy" "${path}"
			COMMENT "Copying resources to ${SOLUTION_SVN_RESOURCES_DESTINATION}"
		)
	endforeach()
	
	#message("COMMAND @CMAKE_COMMAND@ -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${MOTION_SVN_RESOURCES_DESTINATION}")
else()
	# dodanie projektu bez ¿adnego polecenia - mo¿na ge edytowaæ z IDE
	add_custom_target(resources 
		SOURCES ${ALL_RESURCES}
	)	
endif()