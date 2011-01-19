#include "INTELreader.h"
#include <fstream>
#include <iostream>

///////////////////////////////////////////

INTELreader::INTELreader(char* mem, unsigned int maxPos)
{

this->memContent = mem;
this->curPos = 0;
this->maxPos = maxPos;

}

INTELreader::~INTELreader() 
{

delete[] this->memContent;

}

//////////////////////////////////////////

void INTELreader::seek(int offset)
{

	if(offset < -1 || offset >= maxPos-1){}
	else
	{
		curPos = offset-1;
	}

}

void INTELreader::seek(int offset, bool direction) // true - forward, false - backward 
{

if(direction) // forward
{

	if(curPos + offset < maxPos && curPos + offset > 0){
		curPos += offset;
	}

}
else // backward
{

	if(curPos - offset > 0 && curPos - offset < maxPos){
		curPos -= offset;
	}

}

}


unsigned int INTELreader::getCurPos(){

return this->curPos;

}

///////////////////////////////////////////

char INTELreader::readNextByte()
{

	if(this->curPos+1 < this->maxPos){

	this->curPos++;
    return this->memContent[this->curPos];

	}
	else
	{
		return -1;
	}

}

float INTELreader::readNextFloat(){

	if(this->curPos+4 < this->maxPos){

		char tmptmpFloatChars[4];
	this->curPos += 4;

    tmptmpFloatChars[0] = this->memContent[this->curPos-3];
    tmptmpFloatChars[1] = this->memContent[this->curPos-2];
    tmptmpFloatChars[2] = this->memContent[this->curPos-1];
    tmptmpFloatChars[3] = this->memContent[this->curPos];     

    return *(float*)tmptmpFloatChars;

	}else
	{
		return -1;
	}
}

short INTELreader::readNextWord(){

	if(this->curPos+1 < this->maxPos){

	this->curPos+=2;
	char tmptmpFloatChars[2];

	tmptmpFloatChars[0] = this->memContent[this->curPos-1];
    tmptmpFloatChars[1] = this->memContent[this->curPos];

	return *(short*)tmptmpFloatChars;

	}
	else
	{
		return -1;
	}

}






/*



	tmpFloatChars[0] = readNextUByte(file);
	tmpFloatChars[1] = readNextUByte(file);
	tmpFloatChars[2] = readNextUByte(file);
	tmpFloatChars[3] = readNextUByte(file); 

	// data interpretation

    tmptmpFloatChars[0] = tmpFloatChars[0];
    tmptmpFloatChars[1] = tmpFloatChars[1];
    tmptmpFloatChars[2] = tmpFloatChars[2];
    tmptmpFloatChars[3] = tmpFloatChars[3];
    //if (tmptmpFloatChars[0] || tmptmpFloatChars[1] || tmptmpFloatChars[2] || tmptmpFloatChars[3])
    //    --tmptmpFloatChars[3];        

	// float return

    return *(float*)tmptmpFloatChars;
}


*/