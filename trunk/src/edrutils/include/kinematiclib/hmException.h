#ifndef HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__
#define HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__

#include <boost/utility.hpp>

namespace kinematic
{
    /// \brief  Ogolny wyjatek dla parserow.
    class Exception : public std::exception
    {
    public:
        /// \brief  Konstruktor.
        /// \param  Dodatkowa tresc komunikatu
        Exception(const std::string& message);
        virtual ~Exception() throw() {}
    public:
        /// \brief Metoda zwraca tresc komunikatu.
        ///  Pochodzi z std::exception
        virtual const char* what() const throw();

    protected:
        std::string basicMessage; //!< Tresc
        mutable std::string exceptionMessage; //!< Zwrace przez metode what() z dodana informacja z RTTI
    };

    /// \brief  Wyjatek rzucany, gdy probuje sie wykonac operacje
    ///         na danych ktore powinny byc a nie zostaly wczytane
    class AcclaimDataNotLoaded : public Exception
    {
    public:
        AcclaimDataNotLoaded(const std::string& message);
    };

    /// \brief  Wyjatek rzucany, gdy nie udalo sie otworzyc pliku
    class UnableToOpenFileException : public Exception
    {
    public:
        UnableToOpenFileException(const std::string& message);
    };

    /// \brief  Wyjatek rzucany, gdy plik zawiera niepoprawna tresc dla parsera
    class WrongFileException : public Exception
    {
    public:
        WrongFileException(const std::string& message);
    };

    /// \brief  Wyjatek rzucany, gdy probuje sie wykonac niezaimplementowana czesc kodu
    class NotYetImplemented : public Exception
    {
    public:
        NotYetImplemented(const std::string& message) : Exception(message) { };
    };


    class KinematicModelException : public Exception
    {
        public:
            KinematicModelException(const std::string& message) : Exception(message) { };
            virtual ~KinematicModelException() throw() {}
    };

    class UnableToMapJointException : public KinematicModelException
    {
        std::string unmappedJoint;
    public:
        UnableToMapJointException(const std::string& jointName) :
          KinematicModelException("Unable to map : " + jointName),
          unmappedJoint(jointName)
          {
          }
        UnableToMapJointException(const std::string& message, std::string jointName) :
          KinematicModelException(message),
          unmappedJoint(jointName)
          {
          }

          virtual ~UnableToMapJointException() throw() {}
    };

    class DictionaryNotLoadedException : public KinematicModelException
    {
    public:
        DictionaryNotLoadedException(const std::string& message) :
            KinematicModelException(message) {}
    };

    class NotHAnimJointException : public KinematicModelException
    {
        std::string notFoundJoint;
    public:
        NotHAnimJointException(const std::string& jointName) :
          KinematicModelException("Joint: "+ jointName + " is not in h-anim 1.1"),
          notFoundJoint(jointName)
          {
          }
        NotHAnimJointException(const std::string& message, std::string jointName) :
          KinematicModelException(message),
          notFoundJoint(jointName)
          {
          }
        virtual ~NotHAnimJointException() throw() {}
    };
}

#endif
