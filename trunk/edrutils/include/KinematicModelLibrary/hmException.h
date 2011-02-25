#ifndef HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__
#define HEADER_GUARD_KINEMATICMODEL__HMEXCEPTION_H__

namespace hmAnimation
{
    /// \brief  Ogolny wyjatek dla parserow. 
    class HmException : public std::exception
    {
    public:

        /// \brief  Konstruktor. 
        /// \param  Dodatkowa tresc komunikatu
        HmException(const std::string& message);
        virtual ~HmException();
    public:
        /// \brief Metoda zwraca tresc komunikatu.
        ///  Pochodzi z std::exception 
        virtual const char* what() const;

    protected:
        std::string message; //!< Tresc komunikatu
    };

    /// \brief  Wyjatek rzucany, gdy probuje sie wykonac operacje
    ///         na danych ktore powinny byc a nie zostaly wczytane
    class AcclaimDataNotLoaded : public HmException
    {
    public:
        AcclaimDataNotLoaded(const std::string& message);
    };

    /// \brief  Wyjatek rzucany, gdy nie udalo sie otworzyc pliku
    class AcclaimUnableToOpenFileException : public HmException 
    {
    public:
        AcclaimUnableToOpenFileException(const std::string& message);
    };
    
    /// \brief  Wyjatek rzucany, gdy plik zawiera niepoprawna tresc dla parsera
    class AcclaimWrongFileException : public HmException
    {
    public:
        AcclaimWrongFileException(const std::string& message);
    };

    /// \brief  Wyjatek rzucany, gdy probuje sie wykonac niezaimplementowana czesc kodu
    class NotYetImplemented : public HmException
    {
    public:
        NotYetImplemented(const std::string& message) : HmException(message) { };
    };


    class KinematicModelException : public HmException 
    {
        public:
        KinematicModelException(const std::string& message) : HmException(message) { };
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
    };
}

#endif