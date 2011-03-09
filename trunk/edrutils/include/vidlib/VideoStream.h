/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:15
	filename: VideoStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__
#define HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__

#include <string>
#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/Picture.h>
#include <vidlib/VideoError.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

class Converter;

enum {
    //! Nieprawid≥owy czas
    INVALID_TIMESTAMP = -1
};


/**
 *	Abstrakcyjna klasa dla wejúcia wideo.
 */
class VIDLIB_EXPORT VideoStream
{
public:
    //! Sygnatura callbacka.
    class VIDLIB_EXPORT Callback {
    protected:
        //! Chroniony destruktor - aby zaznaczyÊ, øe strumieÒ nie przemuje callbacka
        //! na w≥asnoúÊ (wskaünika nie moøna skasowaÊ)
        virtual ~Callback() {}
    public:
        //! Callback wywo≥ywany w momencie pominiÍcia pewnej ramki. Mechanizm ten
        //! umoøliwa wydajniejsze buforowanie.
        //! \param picture
        //! \param layers
        //! \param timestamp
        virtual void frameRead(Picture * picture, PictureLayered * layers, double timestamp, double endTimestamp) = 0;
    };

protected:
    //!
    Converter * converter;


private:
    //! Uøywane tylko w przypadku nieobs≥ugiwania wyjπtkÛw.
    VideoError * lastError;
    //! Zbuforowana d≥ugoúÊ ramki.
    double frameDuration;
    //! Zbuforowana liczba klatek na sekundÍ.
    double frameRate;
    //! Zbuforowana d≥ugoúÊ strumienia.
    double duration;
    //!
    double aspectRatio;
    //! Format piksela.
    PixelFormat format;
    //! WysokoúÊ ramki.
    int width;
    //! SzerokoúÊ ramki.
    int height;
    //!
    Callback * callback;
    //! Zastosowany wzorzec "Pimpl" (øeby usunπÊ warningi przy budowie DLL)
    class VideoStreamImpl* impl;

protected:
    //! \param source èrÛd≥o obrazu.
    VideoStream(const std::string& source);

public:
    //! Destruktor wirtualny.
    virtual ~VideoStream();

public:
    //! Odczytuje nastÍpnπ klatkÍ.
    /** Przesuwa siÍ w strumieniu na nastÍpnπ ramkÍ. Modyfikacji ulega
      * czas oraz timestamp (nadane im sπ te same wartoúci
      */
    virtual bool readNext() = 0;

    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const = 0;
    //! \param time Pozycja w ürÛdle
    virtual bool setTime(double time) = 0;
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const = 0;
    //! \return Prawdziwy timestamp kolejnej ramki.
    virtual double getNextFrameTimestamp() const = 0;

    //! Konwertuje bieøπcπ ramkÍ do zadanego formatu. Docelowa ramka
    //! musi byÊ zaalokowana.
    //! \param dst
    virtual bool getFrame(Picture & dst);
    //! Konwertuje bieøπcπ ramkÍ do zadanego formatu. Docelowa ramka
    //! musi byÊ zaalokowana.
    //! \param dst
    virtual bool getFrame(PictureLayered & dst);

    //! \return èrÛd≥o.
    const std::string& getSource() const;
    //! \return SzerokoúÊ strumienia.
    inline int getWidth() const
    {
        return width;
    }
    //! \return WysokoúÊ strumienia.
    inline int getHeight() const
    {
        return height;
    }
    //! \return Format strumienia.
    inline PixelFormat getPixelFormat() const
    {
        return format;
    }
    //! \return D≥ugoúÊ strumienia.
    inline double getDuration() const
    {
        return duration;
    }
    //! \return D≥ugoúÊ ramki w sekundach.
    inline double getFrameDuration() const
    {
        return frameDuration;
    }
    //! \return Liczba klatek na sekundÍ.
    inline double getFramerate() const
    {
        return frameRate;
    }
    //! \return £πczna, przybliøona liczba klatek.
    int getFrameCount() const;

    //! \return Format.
    inline double getAspectRatio() const
    {
        return aspectRatio;
    }
    
    //! \return Znormalizowana pozycja w strumieniu.
    double getNormalizedTime() const;

    //! \param normalizedTime Znormalizowana pozycja w ürÛdle.
    void setNormalizedTime(double normalizedTime);

    //! Czy to koniec strumienia?
    inline bool isEndOfStream() const
    {
        return getFrameTimestamp() >= getDuration();
    }

    //! Metoda uøywana tylko w przypadku wy≥πczenia wyjπtkÛw. Jeúli jakakolwiek
    //! publiczna metoda zwrÛci false moøna sprawdziÊ dok≥adnie b≥πd. Jeúli wyjπtki
    //! sπ w≥πczone zawsze zwrÛci NULL.
    //! \return Ostatni b≥πd.
    const VideoError * getLastError() const;

    //! \return
    inline Callback * getCallback()
    { 
        return callback;
    }
    //! \return
    inline const Callback * getCallback() const
    { 
        return callback;
    }
    //! \param callback Wskaünik na callback. StrumieÒ NIE kontroluje odwo≥ania, tj.
    //!                 samodzielnie nie go nie zwalnia ani nie zeruje.
    inline void setCallback(Callback * callback) 
    { 
        this->callback = callback; 
    }

protected:
    //! Metoda ktÛrπ naleøy wywo≥aÊ po inicjalizacji strumienia w klasie pochodnej.
    //! \param source èrÛd≥o.
    bool onAfterInit(const std::string& source, double frameRate, double duration, 
      PixelFormat format, int width, int height, double aspectRatio);

    //!
    bool notifyError( const VideoError& error );

    //!
    void onFrameRead();

    //! Jeøeli strumieÒ jest w stanie zaprezentowaÊ ramkÍ w postaci spakowanej
    //! powinien prze≥adowaÊ tÍ metodÍ.
    //! \param dst
    virtual bool getData(Picture & /*dst*/) 
    { 
        return false; 
    }

    //! Jeøeli strumieÒ jest w stanie zaprezentowaÊ ramkÍ w postaci zdjÍcia
    //! powinien prze≥adowaÊ tÍ metodÍ.
    //! \param dst
    virtual bool getData(PictureLayered & /*dst*/) 
    { 
        return false; 
    }
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD_VIDLIB__VIDEOSTREAM_H__