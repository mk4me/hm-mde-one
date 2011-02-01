#include "DECreader.h"

///////////////////////////////////////////

DECreader::DECreader(char* mem, unsigned int maxPos)
{

this->memContent = mem;
this->curPos = 0;
this->maxPos = maxPos;

}


DECreader::~DECreader() 
{

delete[] this->memContent;

}

//////////////////////////////////////////

void DECreader::seek(int offset)
{

	if(offset < -1 || offset >= maxPos-1){}
	else
	{
		curPos = offset-1;
	}

}

void DECreader::seek(int offset, bool direction) // true - forward, false - backward 
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


unsigned int DECreader::getCurPos(){

return this->curPos;

}

///////////////////////////////////////////

char DECreader::readNextByte()
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

float DECreader::readNextFloat(){

	if(this->curPos+4 < this->maxPos){

		char tmptmpFloatChars[4];
	this->curPos += 4;

    tmptmpFloatChars[0] = this->memContent[this->curPos-1];
    tmptmpFloatChars[1] = this->memContent[this->curPos];
    tmptmpFloatChars[2] = this->memContent[this->curPos-3];
    tmptmpFloatChars[3] = this->memContent[this->curPos-2];

    if (tmptmpFloatChars[0] || tmptmpFloatChars[1] || tmptmpFloatChars[2] || tmptmpFloatChars[3])
        --tmptmpFloatChars[3];        

    return *(float*)tmptmpFloatChars;

	}else
	{
		return -1;
	}

}

short DECreader::readNextWord(){

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
