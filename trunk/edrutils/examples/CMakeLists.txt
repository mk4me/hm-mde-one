project(Examples)
set(PROJECTS_NAME Examples)

if (CONFIG_VIDLIB_ENABLE_OSG)
	ADD_PROJECT(videoPlayerOSG "vidlib;OSG")
endif()

ADD_PROJECT(videoPlayerSDL "vidlib;SDL")
#ADD_PROJECT(QtSample "QT")
ADD_PROJECT(skeletonViewer "kinematiclib;OSG;QT;BOOST;utils")
ADD_PROJECT(QtMainWindowSample "QT")
ADD_PROJECT(osgUIShowcase "osgui")
ADD_PROJECT(exampleVDFMLib "testnodefactory;dfmlib;vdfmlib;OSG;BOOST")
#ADD_PROJECT(superTest "OSG;QT;utils;BOOST")