project(plugins)
set(PROJECTS_NAME plugins)

ADD_PROJECT(c3d "BOOST;corelib;C3DLIB")
# ADD_PROJECT(allC3D "BOOST;corelib;C3DLIB")
ADD_PROJECT(video "OSG;QT;VIDLIB;FFMPEG;corelib")
ADD_PROJECT(newTimeline "BOOST;QT;QWT;corelib;TIMELINELIB;OSG")	# OSG na potrzeby OpenThreads!!
ADD_PROJECT(communication "QT;CURL;WSDLPULL;TINYXML;corelib")
#ADD_PROJECT(newCommunication "QT;CURL;WSDLPULL;TINYXML;corelib")
ADD_PROJECT(kinematic "QT;BOOST;corelib;KINEMATICLIB;OSG;C3DLIB")
#ADD_PROJECT(chart "BOOST;OSG;QT;corelib;C3DLIB")
ADD_PROJECT(newChart "BOOST;OSG;QT;QWT;corelib;C3DLIB")
ADD_PROJECT(subject "BOOST;OSG;QT;corelib;C3DLIB;KINEMATICLIB")
ADD_PROJECT(dataExplorer "BOOST;OSG;QT;corelib")