project(Tests)
set(PROJECTS_NAME Tests)

ADD_PROJECT(vidlib "vidlib;BOOST;CPPUNIT")
ADD_PROJECT(utils "OSG;utils;CPPUNIT")
ADD_PROJECT(kinematiclib "kinematiclib;CPPUNIT")
ADD_PROJECT(dfmlib "testnodefactory;dfmlib;BOOST;CPPUNIT")
ADD_PROJECT(timelinelib "timelinelib;utils;BOOST;CPPUNIT")