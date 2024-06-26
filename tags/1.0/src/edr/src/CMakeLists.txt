SET_PROJECTS_GROUP("core")

CONFIG_OPTION(CORE_ENABLE_LOGGING "Enable logging?" ON)
CONFIG_OPTION(CORE_ENABLE_LOG4CXX "Enable logging with LOG4CXX?" ON)
CONFIGURE_PROJECT_DEPENDENCIES("CORE_ENABLE_LOGGING;CORE_ENABLE_LOG4CXX" "LOG4CXX")
ADD_PROJECT(core "corelib;OPENGL")

ADD_PROJECT(corelib "BOOST;OSG;threading;OPENTHREADS;QT;TINYXML;utils")
ADD_PROJECT(coreui "OPENGL;corelib")

SET_PROJECTS_GROUP("plugins")
ADD_PROJECT(plugin_c3d "corelib;c3dlib;kinematiclib" "plugins/c3d")
ADD_PROJECT(plugin_dicom "BOOST;corelib;DCMTK;STDINT" "plugins/dicom")
ADD_PROJECT(plugin_fileSource "BOOST;corelib" "plugins/fileSource")
#ADD_PROJECT(plugin_sampleService "BOOST;corelib" "plugins/sampleService")
ADD_PROJECT(plugin_video "vidlib;FFMPEG;corelib;osgui" "plugins/video")
ADD_PROJECT(plugin_newTimeline "QWT;corelib;timelinelib;OPENTHREADS" "plugins/newTimeline")
ADD_PROJECT(plugin_kinematic "coreui;plugin_c3d;kinematiclib;osgui" "plugins/kinematic")
ADD_PROJECT(plugin_newCommunication "CURL;WSDLPULL;corelib;webserviceslib;qthtmlgenlib;SQLCIPHER;QUAZIP;plugin_kinematic;plugin_video" "plugins/newCommunication")
ADD_PROJECT(plugin_newChart "QWT;coreui;plugin_c3d" "plugins/newChart")
ADD_PROJECT(plugin_subject "corelib;vidlib;plugin_c3d" "plugins/subject")
ADD_PROJECT(plugin_hmm "corelib;coreui;c3dlib;QWT;vidlib;kinematiclib;plugin_subject" "plugins/hmmlib")
ADD_PROJECT(plugin_newVdf "dflib;plugin_subject;plugin_newCommunication;plugin_newTimeline;plugin_c3d;plugin_video;coreui" "plugins/newVdf")
ADD_PROJECT(plugin_dfElements "dflib;coreui;plugin_newVdf;plugin_c3d" "plugins/dfElements")
#ADD_PROJECT(plugin_python "BOOST;QT;corelib" "plugins/python")

SET_PROJECTS_GROUP("view")
ADD_PROJECT(old_view "core;coreui;plugin_newTimeline;OPENGL;c3dlib")
#ADD_PROJECT(hmm_view "core;corelib;coreui;plugin_dfElements;plugin_newVdf;plugin_c3d;plugin_newTimeline;plugin_video;plugin_kinematic;plugin_subject;plugin_hmm")
ADD_PROJECT(mde_view "core;coreui;c3dlib;timelinelib;kinematiclib;dflib;plugin_newTimeline;plugin_newVdf")

#ADD_PROJECT(medusa_view "core;coreui;c3dlib;timelinelib;kinematiclib;dflib;plugin_newTimeline;plugin_newVdf")
#ADD_PROJECT(console_view "BOOST;utils;QT;TINYXML;core;corelib;coreui;plugin_newTimeline")

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

#ADD_INSTALLER_GROUP_PROJECT(old_view "old_view" "Debelopers view" "HIDDEN" "Full Min")
#ADD_INSTALLER_GROUP_PROJECT(hmm_view "hmm_view" "Medical view" "HIDDEN" "Full Min")
ADD_INSTALLER_GROUP_PROJECT(mde_view "mde_view" "Medical view" "HIDDEN" "Full Min")

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
ADD_INSTALLER_GROUP_PROJECT(plugin_dfElements "dfElements" "Plugins delivering universal helper classes for newVdf" "" "Full")

END_INSTALLER_GROUP()

END_INSTALLER()
