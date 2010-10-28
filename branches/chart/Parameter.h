#ifndef _PARAMETER_
#define _PARAMETER_

#include <string>
#include <vector>
#include "ProcessorReader.h"

/*

Class responsible for holding the data from one parameter. It contains its name, description and multi-dimensional values.

*/
class Parameter {

private: 

	std::string name;
	std::string description;
	char dataType;

	float* data; //  float contains enough bits to store int, byte, char ... and ofcourse float too	
	std::vector<short> dimensions; // data dimensions array eg. 1,2,3 means that data is [1][2][3] array

public:

	Parameter();

	std::string getName();
	std::string getDescription();
	char getDataType();

	std::vector<short> getDimensions();
	float* getData();

	void setName(std::string name);
	void setDescription(std::string description);
	void setDataType(char type);

	void addDimension(short dim);
	void setData(float* data);

	////////////////////////////////////////////////////

	~Parameter();

	////////////////////////////////////////////////////

	std::string toString();
	int saveParam(std::ofstream& out, ProcessorReader* proc, char id);

};

#endif //_PARAMETER_



	/*float a = 34958745.22;

  std::cout << std::bitset<std::numeric_limits<float>::digits>(a);
  std::cout << "\n";

	a = 'a';

	std::cout << std::bitset<std::numeric_limits<float>::digits>(a);
	std::cout << "\n";

	a = (short) 1255;

	std::cout << std::bitset<std::numeric_limits<float>::digits>(a);
	std::cout << "\n";

	a = (unsigned char) 128;

	std::cout << std::bitset<std::numeric_limits<float>::digits>(a);
	std::cout << "\n";
	*/