#include "MIPSreader.h"
#include <fstream>
///////////////////////////////////////////

MIPSreader::MIPSreader(char* mem, unsigned int maxPos)
{

this->memContent = mem;
this->curPos = 0;
this->maxPos = maxPos;

}


MIPSreader::~MIPSreader() 
{

delete[] this->memContent;

}

//////////////////////////////////////////

void MIPSreader::seek(int offset)
{

	if(offset < 0 || offset > static_cast<int>(maxPos)){}
	else
	{
		curPos = offset;
	}

}

void MIPSreader::seek(int offset, bool direction) // true - forward, false - backward 
{



}


unsigned int MIPSreader::getCurPos(){

return 0;

}

///////////////////////////////////////////


char MIPSreader::readNextByte()
{

return 0;

}

float MIPSreader::readNextFloat(){

return 0;

}

short MIPSreader::readNextWord(){

	return 0;

}