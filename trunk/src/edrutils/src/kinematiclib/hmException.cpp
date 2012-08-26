#include "stdafx.h"
#include <typeinfo>
#include <kinematiclib/SkeletalParsers.h>

//----------------------------------------------------------------------------------
kinematic::Exception::Exception( const std::string& message ) :
    basicMessage(message)
{
    LOGGER(Logger::Warning, "Exception was thrown : " +  message);
}


const char* kinematic::Exception::what() const throw()
{
    exceptionMessage.clear();
    exceptionMessage += typeid(*this).name();
    exceptionMessage += (": " + basicMessage);
    std::cout << exceptionMessage << std::endl;
    return exceptionMessage.c_str();
}

kinematic::UnableToOpenFileException::UnableToOpenFileException(const std::string& message)
:Exception(message)
{
}
//----------------------------------------------------------------------------------
kinematic::WrongFileException::WrongFileException( const std::string& message )
:Exception(message)
{

}
//----------------------------------------------------------------------------------
kinematic::AcclaimDataNotLoaded::AcclaimDataNotLoaded( const std::string& message )
:Exception(message)
{

}
