SET_PROJECTS_GROUP("testy")

ADD_TEST_PROJECT(vidlib "utils;vidlib;BOOST;CPPUNIT" "vidlib")
ADD_TEST_PROJECT(kinematiclib "utils;kinematiclib;CPPUNIT" "kinematiclib")
ADD_TEST_PROJECT(dfmlib "utils;testnodefactory;dfmlib;BOOST;CPPUNIT" "dfmlib")
ADD_TEST_PROJECT(timelinelib "utils;timelinelib;BOOST;CPPUNIT" "timelinelib")
ADD_TEST_PROJECT(utils "OSG;utils;CPPUNIT" "utils")