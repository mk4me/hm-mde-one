#include <KinematicModelLibrary/stdafx.h>
#include <typeinfo>
#include <KinematicModelLibrary/SkeletalParsers.h>

//----------------------------------------------------------------------------------
hmAnimation::HmException::HmException( const std::string& message ) {
    //TODO: Czy takie konstrukcje sa bezpieczne ? 
    this->message = message;
    std::string log = typeid(this).name();
    Logger::getInstance().log(Logger::Error, log + " " + message);
}
hmAnimation::HmException::~HmException() {
    
}

const char* hmAnimation::HmException::what() const {
    return message.c_str();
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
