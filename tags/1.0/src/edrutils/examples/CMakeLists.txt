SET_PROJECTS_GROUP("przyklady")


if (CONFIG_VIDLIB_ENABLE_OSG)
	ADD_PROJECT(videoPlayerOSG "vidlib;OSG")
endif()

if (CONFIG_VIDLIB_ENABLE_OSG)
	ADD_PROJECT(videoPlayerSDL "vidlib;SDL;OSG;BOOST")
else()
	ADD_PROJECT(videoPlayerSDL "vidlib;SDL;BOOST")
endif()

#ADD_PROJECT(QtSample "QT")
ADD_PROJECT(skeletonViewer "kinematiclib;OSG;QT;BOOST;utils")
ADD_PROJECT(QtMainWindowSample "QT")
ADD_PROJECT(QtPicturePlacer "QT;BOOST;TINYXML")
ADD_PROJECT(osgUIShowcase "osgui")
ADD_PROJECT(exampleVDFMLib "testnodefactory;dfmlib;vdfmlib;OSG;BOOST")
ADD_PROJECT(newTimelineSample "OSG;QT;utils;BOOST;timelinelib")
ADD_PROJECT(dflibSumator "utils;BOOST;dfmlib")