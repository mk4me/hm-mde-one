#ifndef _C3_DATA_DEF_
#define _C3_DATA_DEF_

#include <core/Header.h>
#include <core/Parameters.h>
#include <core/Data.h>

///////////////////////////////////////////

/*

C3D Data model Class. Consists of header, parameters and data section. Each modelled as separate class.

*/
class C3D_Data {

private:

	MarkerHeader* header;
	Parameters* parameters;
	Data* data;

public:

	C3D_Data(MarkerHeader* header, Parameters* parameters, Data* data);

///////////////////////////////////////////

	MarkerHeader* getHeader() { return header; }
	Parameters* getParameters() { return parameters; }
	Data* getData() { return data; }
	std::string toString();

///////////////////////////////////////////

	~C3D_Data();

};

#endif //_C3_DATA_DEF_