#ifndef HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__
#define HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__

#include <boost/utility.hpp>

namespace kinematic
{
    /// \brief  Ogolny wyjątek dla parserow.
    class Exception : public std::exception
    {
    public:
        /// \brief  Konstruktor.
        /// \param  Dodatkowa treść komunikatu
        Exception(const std::string& message);
        virtual ~Exception() throw() {}
    public:
        /// \brief Metoda zwraca treść komunikatu.
        ///  Pochodzi z std::exception
        virtual const char* what() const throw();

    protected:
        std::string basicMessage; //!< treść
        mutable std::string exceptionMessage; //!< Zwrace przez metode what() z dodana informacja z RTTI
    };

    /// \brief  wyjątek rzucany, gdy próbuje się wykonać operacje
    ///         na danych które powinny być a nie zostały wczytane
    class AcclaimDataNotLoaded : public Exception
    {
    public:
        AcclaimDataNotLoaded(const std::string& message);
    };

    /// \brief  wyjątek rzucany, gdy nie udało się otworzyć pliku
    class UnableToOpenFileException : public Exception
    {
    public:
        UnableToOpenFileException(const std::string& message);
    };

    /// \brief  wyjątek rzucany, gdy plik zawiera niepoprawna treść dla parsera
    class WrongFileException : public Exception
    {
    public:
        WrongFileException(const std::string& message);
    };

    /// \brief  wyjątek rzucany, gdy próbuje się wykonać niezaimplementowana część kodu
    class NotYetImplemented : public Exception
    {
    public:
        NotYetImplemented(const std::string& message) : Exception(message) { };
    };

    /// \brief  wyjątek rzucany w przypadku błędnego modelu kinematycznego
    class KinematicModelException : public Exception
    {
        public:
            KinematicModelException(const std::string& message) : Exception(message) { };
            virtual ~KinematicModelException() throw() {}
    };

    /// \brief  wyjątek rzucany, gdy nie ma się mapować nazwy stawu na standard hAnim
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

    /// \brief  wyjątek rzucany, gdy nie ma dostępnego słownika mapującego nazwy do standardu hAnim
    class DictionaryNotLoadedException : public KinematicModelException
    {
    public:
        DictionaryNotLoadedException(const std::string& message) :
            KinematicModelException(message) {}
    };

    /// \brief  wyjątek rzucany, gdy staw nie spełnia standardu hAnim
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
