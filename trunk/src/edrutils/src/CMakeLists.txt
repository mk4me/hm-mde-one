ADD_PROJECT(utils "BOOST;QT;OSG")
ADD_PROJECT(threading "utils;BOOST;QT;OSG")

ADD_PROJECT_VARIABLE_DEPENDENCIES(VIDLIB_ENABLE_LOG4CXX "Enable logging with LOG4CXX?" OFF "LOG4CXX")
ADD_PROJECT_VARIABLE_DEPENDENCIES(VIDLIB_ENABLE_OSG "Enable osg for vidlib?" ON "OSG")
ADD_PROJECT(vidlib "BOOST;FFMPEG;utils;STDINT;OPENTHREADS")


ADD_PROJECT(kinematiclib "BOOST;TINYXML;OSG;utils")
ADD_PROJECT(osgui "utils;OSG;QT;BOOST")
ADD_PROJECT(dflib "BOOST;QT;utils")
ADD_PROJECT(dfmlib "BOOST;OSG;utils")
ADD_PROJECT(vdfmlib "osgui;dfmlib;BOOST;OSG;utils")
ADD_PROJECT(testnodefactory "dfmlib;BOOST;utils")
ADD_PROJECT(timelinelib "OPENTHREADS;utils;BOOST;QT")
ADD_PROJECT(c3dlib "BTK;BOOST;OSG;utils")
ADD_PROJECT(webserviceslib "BOOST;TINYXML;CURL;OPENTHREADS;WSDLPULL;utils")
ADD_PROJECT(qthtmlgenlib "BOOST;TINYXML;QT;utils")


BEGIN_INSTALLER("edr_utils" "edr_utils")

SET_INSTALLER_VENDOR("PJWSTK")
SET_INSTALLER_VERSION("0.0.1")

ADD_INSTALLER_GROUP_PROJECT(utils "utils" "General purpose classes")
ADD_INSTALLER_GROUP_PROJECT(vidlib "vidlib" "Video decoding library")
ADD_INSTALLER_GROUP_PROJECT(kinematiclib "kinematiclib" "Library supporting kinematic chains")
ADD_INSTALLER_GROUP_PROJECT(osgui "osgui" "Library extending OSG capabilities")
ADD_INSTALLER_GROUP_PROJECT(dflib "dflib" "Library supporting data flow processing")
ADD_INSTALLER_GROUP_PROJECT(timelinelib "timelinelib" "Library offering timeline functionality")
ADD_INSTALLER_GROUP_PROJECT(c3dlib "c3dlib" "Library offering c3d format support")
ADD_INSTALLER_GROUP_PROJECT(webserviceslib "webserviceslib" "Library offering hml motion database access and functionality")
ADD_INSTALLER_GROUP_PROJECT(qthtmlgenlib "qthtmlgenlib" "Library generating html documents for Qt controls")

END_INSTALLER()
