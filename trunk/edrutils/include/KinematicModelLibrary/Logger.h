#ifndef HEADER_GUARD_KINEMATICMODEL__LOGGER_H__
#define HEADER_GUARD_KINEMATICMODEL__LOGGER_H__

#include <string>
#include <iostream>
namespace kinematic {
    
    /// \brief Makro, dzieki ktoremu nie wykona sie niepotrzebne skladanie wiadomosci (message)
    #define LOGGER(severity, basicMessage) \
                           if (Logger::getInstance().getLogCallback() && (severity) >= Logger::getInstance().getOutSeverity()) { \
                               Logger::getInstance().log((severity), (basicMessage)); \
                           }
//! Klasa, która jest odpowiedzialna za logowanie informacji.
class Logger
{
public:
    //! Poziom waznosci komunikatow.
    enum LogSeverity {
        Debug,
        Info,
        Warning,
        Error,
    };
    //! wskaznik na funkcje do ktorej trafiaja komunikaty
    typedef void (*LogCallback)(LogSeverity severity, const std::string& msg);

private:
    Logger() {
        setOutSeverity(Logger::Debug);      
        setLogCallback(Logger::logConsole);
    }
    Logger(const Logger &);
    Logger& operator=(const Logger&);
public:
    //! zwraca instancje logera
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

public:
    //! Metoda, dzieki ktorej wysyla sie komunikaty
    //! \param severity
    //! \param msg
    void log(LogSeverity severity, const std::string& msg) {
        if (callback && severity >= outSeverity) {
            callback(severity, msg);
        }
    }

    //! Metoda zmienia wskaznik na funkcje logujaca (domyslnie jest konsola)
    //! \param _callback wskaznik na nowa funkcje (moze byc NULL)
    void setLogCallback(LogCallback _callback) { callback = _callback; }
    LogCallback getLogCallback() { return callback; }
    void setOutSeverity(LogSeverity severity) {
        this->outSeverity = severity;
    }
    LogSeverity getOutSeverity() {
        return this->outSeverity;
    }

private:
    //! Metoda wypisuje informacje prosto do konsoli
    //! \param severity waznosc komunikatu
    //! \param msg tresc wiadomosci
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
    //! Wskaznik na funkcje logujaca
    LogCallback callback;
    //! obecny poziom logowania (wiadomosci o nizszym poziomie nie beda logowane)
    LogSeverity outSeverity;
};
} 
#endif