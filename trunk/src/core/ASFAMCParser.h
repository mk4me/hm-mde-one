#ifndef ASFAMCPARSER_H
#define ASFAMCPARSER_H


#include "ASFAMCData.h"

// Read ASF and AMC files -> pass an asf filename then an amc one.



class ASFAMCParser 
{
public:
    ASFAMCParser();
    ~ASFAMCParser();

    int readAcclaimFiles( char* ASFFileName, char* AMCFileName ) ;
private:

};


#endif //ASFAMCPARSER_H