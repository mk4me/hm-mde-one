set(TESTS_DEPENDENCIES "CPPUNIT")
ADD_TEST_PROJECT(test_corelib "corelib;coreui" "corelib")
ADD_TEST_PROJECT(test_coreui "coreui" "coreui")
ADD_TEST_PROJECT(test_mde_view "core;corelib;coreui;utils;c3dlib;timelinelib;kinematiclib;dflib;plugin_newTimeline;plugin_newVdf" "mde_view")
ADD_TEST_PROJECT(test_medusa_view "core;corelib;coreui;utils;c3dlib;timelinelib;kinematiclib;dflib;plugin_newTimeline;plugin_dicom" "medusa_view")
ADD_TEST_PROJECT(test_newVdf "dflib;core;kinematiclib;vidlib;plugin_subject;plugin_newCommunication;plugin_newTimeline;plugin_c3d;plugin_video;coreui" "plugins/newVdf")
ADD_TEST_PROJECT(test_dfElements "dflib;coreui;plugin_newVdf;c3dlib;plugin_c3d;plugin_newCommunication" "plugins/dfElements")
ADD_TEST_PROJECT(test_dicom "plugin_newCommunication;plugin_subject;CURL;WSDLPULL;BOOST;coreui;QWT;webserviceslib" "plugins/dicom")
