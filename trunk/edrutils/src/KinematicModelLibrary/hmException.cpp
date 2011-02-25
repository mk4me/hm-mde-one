#include "stdafx.h"
#include <typeinfo>
#include <KinematicModelLibrary/SkeletalParsers.h>

//----------------------------------------------------------------------------------
hmAnimation::HmException::HmException( const std::string& message ) {
    //TODO: Czy takie konstrukcje sa bezpieczne ? 
    this->message = message;
    Logger::getInstance().log(Logger::Warning, "Exception was thrown : " +  message);
}
hmAnimation::HmException::~HmException() {
    
}

const char* hmAnimation::HmException::what() const {
    std::string exc = typeid(*this).name();
    exc += (": " + message);
    return exc.c_str();
}
//----------------------------------------------------------------------------------
hmAnimation::AcclaimUnableToOpenFileException::AcclaimUnableToOpenFileException( const std::string& message )
:HmException(message)
{

}
//----------------------------------------------------------------------------------
hmAnimation::AcclaimWrongFileException::AcclaimWrongFileException( const std::string& message )
:HmException(message)
{

}
//----------------------------------------------------------------------------------
hmAnimation::AcclaimDataNotLoaded::AcclaimDataNotLoaded( const std::string& message )
:HmException(message)
{

}
