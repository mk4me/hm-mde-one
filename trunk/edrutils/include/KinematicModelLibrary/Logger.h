#ifndef HEADER_GUARD_KINEMATICMODEL__LOGGER_H__
#define HEADER_GUARD_KINEMATICMODEL__LOGGER_H__

namespace hmAnimation {
//! Klasa, która jest odpowiedzialna za logowanie informacji.
class Logger
{
public:
    //! Poziom waznosci komunikatow.
    enum LogSeverity {
        Info,
        Debug,
        Error,
        Warning,
    };
    //! wskaznik na funkcje do ktorej trafiaja komunikaty
    typedef void (*LogCallback)(LogSeverity severity, const std::string& msg);

private:
    Logger() {}
    Logger(const Logger &);
    Logger& operator=(const Logger&);
public:
    static Logger& getInstance() {
        static Logger instance;
        instance.setLogCallback(Logger::logConsole);
        return instance;
    }

public:
    //! Metoda, dzieki ktorej wysyla sie komunikaty
    //! \param severity
    //! \param msg
    void log(LogSeverity severity, const std::string& msg) {
        if (callback) {
            callback(severity, msg);
        }
    }

    //! Metoda zmienia wskaznik na funkcje logujaca (domyslnie jest konsola)
    //! \param _callback wskaznik na nowa funkcje (moze byc NULL)
    void setLogCallback(LogCallback _callback) { callback = _callback; }

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
};
} 
#endif