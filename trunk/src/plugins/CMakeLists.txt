project(plugins)
set(PROJECTS_NAME plugins)

# animation
ADD_PROJECT(animation "OSG;QT;utils;corelib")
ADD_PROJECT(video "OSG;QT;FFMPEG;STDINT;utils;corelib")
ADD_PROJECT(timeline "BOOST;QT;utils;corelib")
ADD_PROJECT(communication "QT;CURL;WSDLPULL;TINYXML;utils;corelib")
ADD_PROJECT(chart "BOOST;OSG;QT;utils;corelib")
# plugin
# ADD_PROJECT(OSGPlugin "OSG;vm_core")

set(${PROJECTS_NAME}_PROJECTS_PROXY "animation" PARENT_SCOPE)
set(${PROJECTS_NAME}_FOUND 1 PARENT_SCOPE)
set(${PROJECTS_NAME}_LIBRARIES ${PROJECTS_NAME}_PROJECTS_PROXY PARENT_SCOPE)
set(${PROJECTS_NAME}_INCLUDE_DIR "${PROJECT_SOURCE_DIR};${PROJECT_BINARY_DIR}" PARENT_SCOPE)