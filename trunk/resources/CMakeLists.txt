project(Resources)

# modele
set(MESHES 
	meshes/fixed_biped.fmesh
	meshes/trial1.amc
	meshes/trial1.asf
	meshes/trial1.tbs
	meshes/trial2.amc
)
source_group("meshes" FILES ${MESHES})


# pliki shaderów
set(SHADERS 
	shaders/textureRect_yuv_to_rgb.frag
	shaders/texture2D_yuv_to_rgb.frag
)
source_group("shaders" FILES ${SHADERS})

set(SCHEMAS
	schemas/http.xsd
	schemas/http12.xsd
	schemas/mime.xsd
	schemas/schema.dtd
	schemas/soap-encoding.xsd
	schemas/soap-encoding12.xsd
	schemas/soap.xsd
	schemas/wsdl10.xsd
)
source_group("schemas" FILES ${SCHEMAS})


# czy kopiowaæ zasoby?
option(EDR_ENABLE_SVN_RESOURCES_COPYING "Enable copying resources?" OFF)
if(EDR_ENABLE_SVN_RESOURCES_COPYING)
	# tak, trzeba wskazaæ œcie¿kê, gdzie to ma nast¹piæ
	set(EDR_SVN_RESOURCES_DESTINATION "${PROJECT_SOURCE_DIR}/../data/" CACHE PATH "Location of resources being part of repository.")
	# dodanie projektu, który kopiuje zawartoœæ bie¿¹cego katalogu
	add_custom_target(resources ALL
		COMMAND "@CMAKE_COMMAND@" -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${EDR_SVN_RESOURCES_DESTINATION}
		SOURCES ${MESHES} ${SHADERS} ${SCHEMAS}
	)
	#message("COMMAND @CMAKE_COMMAND@ -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${MOTION_SVN_RESOURCES_DESTINATION}")
else()
	# dodanie projektu bez ¿adnego polecenia - mo¿na ge edytowaæ z IDE
	add_custom_target(resources 
		SOURCES ${MESHES} ${SHADERS} ${SCHEMAS}
	)
endif()