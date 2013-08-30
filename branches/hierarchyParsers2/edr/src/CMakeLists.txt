SET_PROJECTS_GROUP("core")

CONFIG_OPTION(CORE_ENABLE_LOGGING "Enable logging?" ON)
CONFIG_OPTION(CORE_ENABLE_LOG4CXX "Enable logging with LOG4CXX?" ON)
CONFIGURE_PROJECT_DEPENDENCIES("CORE_ENABLE_LOGGING;CORE_ENABLE_LOG4CXX" "LOG4CXX")
ADD_PROJECT(core "BOOST;utils;threading;OSG;QT;TINYXML;corelib;OPENGL")

ADD_PROJECT(corelib "BOOST;OSG;threading;OPENTHREADS;QT;TINYXML;utils")
ADD_PROJECT(coreui "BOOST;utils;QT;TINYXML;corelib;OPENGL;corelib")
#ADD_PROJECT(hmmlib  "BOOST;utils;OSG;QT;TINYXML;core;corelib;coreui;c3dlib;QWT;vidlib;kinematiclib")

SET_PROJECTS_GROUP("plugins")
ADD_PROJECT(plugin_c3d "BOOST;corelib;c3dlib;kinematiclib" "plugins/c3d")
#ADD_PROJECT(plugin_dicom "BOOST;corelib;DCMTK;STDINT" "plugins/dicom")
ADD_PROJECT(plugin_fileSource "BOOST;corelib" "plugins/fileSource")
#ADD_PROJECT(plugin_sampleService "BOOST;corelib" "plugins/sampleService")
ADD_PROJECT(plugin_video "OSG;QT;vidlib;FFMPEG;TINYXML;corelib;osgui" "plugins/video")
ADD_PROJECT(plugin_newTimeline "BOOST;QT;QWT;corelib;timelinelib;OPENTHREADS" "plugins/newTimeline")
ADD_PROJECT(plugin_kinematic "QT;BOOST;corelib;coreui;plugin_c3d;kinematiclib;OSG;c3dlib;osgui" "plugins/kinematic")
ADD_PROJECT(plugin_newCommunication "QT;CURL;WSDLPULL;TINYXML;corelib;webserviceslib;qthtmlgenlib;SQLCIPHER;QUAZIP;plugin_kinematic;plugin_video;vidlib" "plugins/newCommunication")
ADD_PROJECT(plugin_newChart "BOOST;OSG;QT;QWT;corelib;coreui;plugin_c3d;c3dlib" "plugins/newChart")
ADD_PROJECT(plugin_subject "BOOST;OSG;QT;corelib;utils;c3dlib;kinematiclib;vidlib;plugin_c3d" "plugins/subject")
ADD_PROJECT(plugin_hmm "BOOST;utils;OSG;QT;TINYXML;core;corelib;coreui;c3dlib;QWT;vidlib;kinematiclib;plugin_subject" "plugins/hmmlib")
ADD_PROJECT(plugin_newVdf "utils;BOOST;QT;OSG;utils;dflib;core;corelib;kinematiclib;vidlib;plugin_subject;plugin_newTimeline;plugin_c3d;plugin_video;coreui" "plugins/newVdf")
#ADD_PROJECT(plugin_newVdf "BOOST;UTILS;OSG;QT;core;corelib;DFLIB;coreui;VIDLIB;plugin_newTimeline;plugin_video;plugin_c3d;plugin_subject" "plugins/newVdf")
ADD_PROJECT(plugin_dfElements "BOOST;QT;dflib;corelib;coreui;utils;plugin_newVdf;c3dlib;plugin_c3d;plugin_newCommunication" "plugins/dfElements")
#ADD_PROJECT(plugin_python "BOOST;QT;corelib" "plugins/python")

SET_PROJECTS_GROUP("view")
#ADD_PROJECT(hmmlib  "BOOST;UTILS;OSG;QT;TINYXML;core;corelib;coreui;C3DLIB;QWT;VIDLIB;plugin_newVdf;plugin_dfElements;plugin_c3d;plugin_newTimeline;plugin_video;plugin_kinematic;plugin_subject")
ADD_PROJECT(old_view "BOOST;utils;OSG;QT;TINYXML;core;corelib;coreui;plugin_newTimeline;OPENGL;c3dlib")
#ADD_PROJECT(hmm_view "core;corelib;coreui;plugin_dfElements;plugin_newVdf;plugin_c3d;plugin_newTimeline;plugin_video;plugin_kinematic;plugin_subject;plugin_hmm")
ADD_PROJECT(mde_view "core;corelib;coreui;utils;c3dlib;timelinelib;kinematiclib;dflib;plugin_newTimeline;plugin_newVdf")


BEGIN_INSTALLER("MDE" "MDE")

SET_INSTALLER_VENDOR("PJWSTK")
SET_INSTALLER_VERSION("0.0.1")
SET_INSTALLER_ADDITIONAL_INFO("http://hm.pjwstk.edu.pl" "http://hmkb.pjwstk.edu.pl" "Marek.Kulbacki@pjwstk.edu.pl")
SET_INSTALLER_BRANDING_IMAGE("graphic/IKONA.png")
SET_INSTALLER_PRODUCT_ICONS("graphic/IKONA.ico" "graphic/IKONA.ico")
SET_INSTALLER_LICENSE("license.txt")

ADD_INSTALLER_INSTALLATION_TYPE(Full "Complete installation")
ADD_INSTALLER_INSTALLATION_TYPE(Min "Minimal installation")

BEGIN_INSTALLER_GROUP(core "Core" "Core application elements")
ADD_INSTALLER_GROUP_PROJECT(corelib "corelib" "General purpose library" "REQUIRED" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(coreui "coreui" "UI support library" "REQUIRED" "Full Min")

ADD_INSTALLER_GROUP_PROJECT(old_view "old_view" "Debelopers view" "HIDDEN" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(hmm_view "hmm_view" "Medical view" "HIDDEN" "Full Min")

END_INSTALLER_GROUP()

BEGIN_INSTALLER_GROUP(plugins "Plugins" "Built-in application plugins")

ADD_INSTALLER_GROUP_PROJECT(plugin_c3d "c3d" "Plugin supporting reading C3D files data" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_video "video" "Plugin supporting reading and playing video files" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_newTimeline "newTimeline" "Plugin offering time managment and synchronization" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_kinematic "kinematic" "Plugin offering skeleton and kinematic data managment and visualisation" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_newCommunication "newCommunication" "Plugin offering motion database and services access" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_newChart "newChart" "Plugin supporting data plots and visualisation" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_subject "subject" "Plugin supporting motion data organisation" "" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(plugin_newVdf "newVdf" "Plugins supporting efficient data processing with flexible GUI configuration" "" "Full")

END_INSTALLER_GROUP()

END_INSTALLER()
