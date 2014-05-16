BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	Export.h
	IFTPOperations.h
	IWSDLService.h
	CURLFTPHelper.h
	Types.h
	CURLManager.h
)

SET_SOURCE_FILES(
	CURLFTPHelper.cpp
	CURLManager.cpp
)

END_PROJECT()