project(Examples)
set(PROJECTS_NAME Examples)

if (CONFIG_VIDLIB_ENABLE_OSG)
	ADD_PROJECT(videoPlayerOSG "vidlib;OSG")
endif()

ADD_PROJECT(videoPlayerSDL "vidlib;SDL")
ADD_PROJECT(qtBasicSample "QT")
