BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	ImuCostume.h
	ImuSensor.h
	CostumeRawIO.h
	CostumeCANopenIO.h
	CANopenSensor.h
	Export.h
)

SET_SOURCE_FILES( 
	ImuCostume.cpp
	ImuSensor.cpp
	CostumeRawIO.cpp
	CostumeCANopenIO.cpp
	CANopenSensor.cpp
)

END_PROJECT()