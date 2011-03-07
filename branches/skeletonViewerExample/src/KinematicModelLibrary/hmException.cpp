#include "stdafx.h"
#include <typeinfo>
#include <KinematicModelLibrary/SkeletalParsers.h>

//----------------------------------------------------------------------------------
kinematic::HmException::HmException( const std::string& message ) {
    //TODO: Czy takie konstrukcje sa bezpieczne ? 
    this->message = message;
    //Logger::getInstance().log(Logger::Warning, "Exception was thrown : " +  message);
    LOGGER(Logger::Warning, "Exception was thrown : " +  message);
}
kinematic::HmException::~HmException() {
    
}

const char* kinematic::HmException::what() const {
    std::string exc = typeid(*this).name();
    exc += (": " + message);
    return exc.c_str();
}
//----------------------------------------------------------------------------------
kinematic::UnableToOpenFileException::UnableToOpenFileException( const std::string& message )
:HmException(message)
{

}
//----------------------------------------------------------------------------------
kinematic::AcclaimWrongFileException::AcclaimWrongFileException( const std::string& message )
:HmException(message)
{

}
//----------------------------------------------------------------------------------
kinematic::AcclaimDataNotLoaded::AcclaimDataNotLoaded( const std::string& message )
:HmException(message)
{

}
