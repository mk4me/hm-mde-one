/********************************************************************
    created:  2011/04/08
    created:  8:4:2011   13:39
    filename: FileSequenceVideoStream.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__
#define HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__

#include <string>
#include <vector>
#include <utils/Utils.h>
#include <vidlib/Export.h>
#include <vidlib/VideoStream.h>

namespace vidlib 
{
    UTILS_PUSH_WARNINGS
    UTILS_DISABLE_DLL_INTERFACE_WARNING

    //! Strumie� video oparty o fizyczny pliki. Do wczytywania plik�w u�ywa obiektu ILoader
    class VIDLIB_EXPORT FileSequenceVideoStream : public VideoStream
    {
    public:
        //!
        class ILoader 
        {
        public:
            //!
            virtual ~ILoader() {}
            //! \param path �cie�ka z kt�rej nale�y odczyta� plik.
            //! \param picture Zdj�cie wynikowe. Powinno mu si� przypisywa� pola, ale nie alokowa�!
            //! \return true gdy si� uda�o, false w przeciwnym razie
            virtual bool readImage(const std::string& path, Picture& picture) = 0;
            //! Klonuje bie��cego loadera. To, czy ma to by� p�ytka czy g��boka kopia, zale�y
            //! od logiki loadera; gdy jest to mo�liwe p�ytka kopia jest preferowana.
            virtual ILoader* clone() const = 0;
        };

    private:
        //! Lista plik�w.
        std::vector<std::string> files;
        //! Bie��ca ramka.
        Picture frame;
        //! Wybrany czas.
        double wantedTime;
        //! Numer bie��cej ramki.
        int currentFrame;
        //! Interfejs do �adowania obrazk�w.
        ILoader* loader;

    public:
        //! \param name
        //! \param framerate
        //! \param files
        //! \param loader Obiekt przejmowany na w�asno��.
        FileSequenceVideoStream(const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader);
        //!
        virtual ~FileSequenceVideoStream();

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //!
        virtual bool readNext();
        //! \return Pozycja w �r�dle.
        virtual double getTime() const;
        //! \param time Pozycja w �r�dle
        virtual bool setTime(double time);
        //! \return Prawdziwy timestamp ramki.
        virtual double getFrameTimestamp() const;
        //! \return Prawdziwy timestamp kolejnej ramki.
        virtual double getNextFrameTimestamp() const;

    protected:
        //! \param dst
        virtual bool getData(Picture & /*dst*/);

    private:
        //! Inicjalizacja.
        //! \param name
        //! \param framerate
        //! \param files
        //! \param loader
        bool init(const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader);
    };

    UTILS_POP_WARNINGS
} // namespace vidlib


#endif  // HEADER_GUARD_VIDLIB__FILESEQUENCEVIDEOSTREAM_H__