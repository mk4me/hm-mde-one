BEGIN_PYTHON_PROJECT()

SET_PRIVATE_HEADERS(
	SimpleLogic.h
)

SET_SOURCE_FILES (
	PythonPlugin.cpp
	SimpleLogic.cpp
)



#SET_PROJECT_ADDITIONAL_INSTALLS(
#	plugin_simplePython.pyd
#)

END_PYTHON_PROJECT()