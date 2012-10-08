#ifndef HEADER_GUARD_KINEMATICMODEL__LOGGER_H__
#define HEADER_GUARD_KINEMATICMODEL__LOGGER_H__

#include <string>
#include <iostream>

namespace kinematic {
    
    /// \brief Makro, dzięki któremu nie wykona się niepotrzebne składanie wiadomości (message)
    #define LOGGER(severity, basicMessage) \
                           if (Logger::getInstance().getLogCallback() && (severity) >= Logger::getInstance().getOutSeverity()) { \
                               Logger::getInstance().log((severity), (basicMessage)); \
                           }
//! Klasa, która jest odpowiedzialna za logowanie informacji.
class Logger
{
public:
    //! poziom ważności komunikatów.
    enum LogSeverity {
        Debug,
        Info,
        Warning,
        Error,
    };
    //! wskaźnik na funkcję do której trafiają komunikaty
    typedef void (*LogCallback)(LogSeverity severity, const std::string& msg);

private:
    //! prywatny konstruktor, zabronione jest kopiowanie, ustawia domyślne wartości
    Logger() {
        setOutSeverity(Logger::Debug);      
        setLogCallback(Logger::logConsole);
    }
    //! prywatny konstruktor kopiujący, zabronione jest kopiowanie
    Logger(const Logger &);
    //! prywatny operator przypisania, zabronione jest kopiowanie
    Logger& operator=(const Logger&);
public:
    //! zwraca instancję logera
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

public:
    //! Metoda, dzięki której wysyla się komunikaty
    //! \param severity 
    //! \param msg
    void log(LogSeverity severity, const std::string& msg) {
        if (callback && severity >= outSeverity) {
            callback(severity, msg);
        }
    }

    //! Metoda zmienia wskaźnik na funkcje logującą (domyślnie jest konsola)
    //! \param _callback wskaźnik na nowa funkcję (moze być NULL)
    void setLogCallback(LogCallback _callback) { callback = _callback; }
    //! \return wskaźnik na funkcje logującą (domyślnie jest konsola)
    LogCallback getLogCallback() { return callback; }
    //! ustawia poziom ważności komunikatów
    void setOutSeverity(LogSeverity severity) {
        this->outSeverity = severity;
    }
    //! \return poziom ważności komunikatów
    LogSeverity getOutSeverity() {
        return this->outSeverity;
    }

private:
    //! Metoda wypisuje informacje prosto do konsoli
    //! \param severity ważność komunikatu
    //! \param msg treść wiadomości
    static void logConsole(LogSeverity severity, const std::string& msg) {
        switch (severity) {
            case Debug:     std::cout << "DEBUG: ";     break;
            case Error:     std::cout << "ERROR: ";     break;
            case Info:      std::cout << "INFO: ";      break;
            case Warning:   std::cout << "WARNING: ";   break;
        }
        std::cout << msg << std::endl;
    }


private:
    //! wskaźnik na funkcje logująca
    LogCallback callback;
    //! obecny poziom logowania (wiadomości o nizszym poziomie nie beda logowane)
    LogSeverity outSeverity;
};
} 
#endif
