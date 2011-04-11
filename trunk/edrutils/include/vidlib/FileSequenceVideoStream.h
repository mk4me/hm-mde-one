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

    //! Strumieñ video oparty o fizyczny pliki. Do wczytywania plików u¿ywa obiektu ILoader
    class VIDLIB_EXPORT FileSequenceVideoStream : public VideoStream
    {
    public:
        //!
        class ILoader 
        {
        public:
            //!
            virtual ~ILoader() {}
            //! \param path Œcie¿ka z której nale¿y odczytaæ plik.
            //! \param picture Zdjêcie wynikowe. Powinno mu siê przypisywaæ pola, ale nie alokowaæ!
            //! \return true gdy siê uda³o, false w przeciwnym razie
            virtual bool readImage(const std::string& path, Picture& picture) = 0;
            //! Klonuje bie¿¹cego loadera. To, czy ma to byæ p³ytka czy g³êboka kopia, zale¿y
            //! od logiki loadera; gdy jest to mo¿liwe p³ytka kopia jest preferowana.
            virtual ILoader* clone() const = 0;
        };

    private:
        //! Lista plików.
        std::vector<std::string> files;
        //! Bie¿¹ca ramka.
        Picture frame;
        //! Wybrany czas.
        double wantedTime;
        //! Numer bie¿¹cej ramki.
        int currentFrame;
        //! Interfejs do ³adowania obrazków.
        ILoader* loader;

    public:
        //! \param name
        //! \param framerate
        //! \param files
        //! \param loader Obiekt przejmowany na w³asnoœæ.
        FileSequenceVideoStream(const std::string& name, double framerate, const std::vector<std::string>& files, ILoader* loader);
        //!
        virtual ~FileSequenceVideoStream();

    // VideoStream
    public:
        //!
        virtual VideoStream* clone() const;
        //!
        virtual bool readNext();
        //! \return Pozycja w Ÿródle.
        virtual double getTime() const;
        //! \param time Pozycja w Ÿródle
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