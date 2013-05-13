SET_PROJECTS_GROUP("core")

CONFIG_OPTION(CORE_ENABLE_LOGGING "Enable logging?" ON)
CONFIG_OPTION(CORE_ENABLE_LOG4CXX "Enable logging with LOG4CXX?" ON)
CONFIGURE_PROJECT_DEPENDENCIES("CORE_ENABLE_LOGGING;CORE_ENABLE_LOG4CXX" "LOG4CXX")
ADD_PROJECT(core "BOOST;utils;OSG;QT;TINYXML;corelib;OPENGL")

ADD_PROJECT(corelib "BOOST;OSG;OPENTHREADS;QT;TINYXML;utils")
ADD_PROJECT(coreui "BOOST;utils;QT;TINYXML;corelib;OPENGL;corelib")

SET_PROJECTS_GROUP("plugins")
ADD_PROJECT(plugin_c3d "BOOST;corelib;c3dlib;kinematiclib" "plugins/c3d")
ADD_PROJECT(plugin_video "OSG;QT;vidlib;FFMPEG;TINYXML;corelib;osgui" "plugins/video")
ADD_PROJECT(plugin_newTimeline "BOOST;QT;QWT;corelib;timelinelib;OPENTHREADS" "plugins/newTimeline")
ADD_PROJECT(plugin_kinematic "QT;BOOST;corelib;coreui;plugin_c3d;kinematiclib;OSG;c3dlib;osgui" "plugins/kinematic")
ADD_PROJECT(plugin_newCommunication "QT;CURL;WSDLPULL;TINYXML;corelib;webserviceslib;qthtmlgenlib;SQLCIPHER;QuaZip;plugin_kinematic;" "plugins/newCommunication")
ADD_PROJECT(plugin_newChart "BOOST;OSG;QT;QWT;corelib;coreui;plugin_c3d;c3dlib" "plugins/newChart")
ADD_PROJECT(plugin_subject "BOOST;OSG;QT;corelib;utils;c3dlib;kinematiclib" "plugins/subject")
ADD_PROJECT(plugin_newVdf "BOOST;QT;dflib;corelib;coreui" "plugins/newVdf")
#ADD_PROJECT(plugin_python "BOOST;QT;corelib" "plugins/python")

SET_PROJECTS_GROUP("view")
ADD_PROJECT(old_view "BOOST;utils;OSG;QT;TINYXML;core;corelib;coreui;plugin_newTimeline;OPENGL;c3dlib")
ADD_PROJECT(hmm_view "BOOST;utils;OSG;QT;TINYXML;core;corelib;coreui;c3dlib;QWT;vidlib;plugin_newVdf;plugin_c3d;plugin_newTimeline;plugin_video;plugin_kinematic;plugin_subject")

SET_PROJECTS_GROUP("custom")
ADD_PROJECT(laboratorium_gk "BOOST;utils;OSG;EIGEN3")



BEGIN_INSTALLER("MDE" "MDE")

SET_INSTALLER_VENDOR("PJWSTK")
SET_INSTALLER_VERSION("0.0.1")

BEGIN_INSTALLER_GROUP(core "Core" "Core application elements")
ADD_INSTALLER_GROUP_PROJECT(corelib "corelib" "General purpose library")
ADD_INSTALLER_GROUP_PROJECT(coreui "coreui" "UI support library")
END_INSTALLER_GROUP()

BEGIN_INSTALLER_GROUP(plugins "Plugins" "Built-in application plugins")

ADD_INSTALLER_GROUP_PROJECT(plugin_c3d "c3d" "Plugin supporting reading C3D files data")
ADD_INSTALLER_GROUP_PROJECT(plugin_video "video" "Plugin supporting reading and playing video files")
ADD_INSTALLER_GROUP_PROJECT(plugin_newTimeline "newTimeline" "Plugin offering time managment and synchronization")
ADD_INSTALLER_GROUP_PROJECT(plugin_kinematic "kinematic" "Plugin offering skeleton and kinematic data managment and visualisation")
ADD_INSTALLER_GROUP_PROJECT(plugin_newCommunication "newCommunication" "Plugin offering motion database and services access")
ADD_INSTALLER_GROUP_PROJECT(plugin_newChart "newChart" "Plugin supporting data plots and visualisation")
ADD_INSTALLER_GROUP_PROJECT(plugin_subject "subject" "Plugin supporting motion data organisation")
ADD_INSTALLER_GROUP_PROJECT(plugin_newVdf "newVdf" "Plugins supporting efficient data processing with flexible GUI configuration")

END_INSTALLER_GROUP()

END_INSTALLER()