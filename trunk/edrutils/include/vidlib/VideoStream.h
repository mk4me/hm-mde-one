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
    //! Nieprawid�owy czas
    INVALID_TIMESTAMP = -1
};


/**
 *	Abstrakcyjna klasa dla wej�cia wideo.
 */
class VIDLIB_EXPORT VideoStream
{
public:
    //! Sygnatura callbacka.
    class VIDLIB_EXPORT Callback {
    protected:
        //! Chroniony destruktor - aby zaznaczy�, �e strumie� nie przemuje callbacka
        //! na w�asno�� (wska�nika nie mo�na skasowa�)
        virtual ~Callback() {}
    public:
        //! Callback wywo�ywany w momencie pomini�cia pewnej ramki. Mechanizm ten
        //! umo�liwa wydajniejsze buforowanie.
        //! \param picture
        //! \param layers
        //! \param timestamp
        virtual void frameRead(Picture * picture, PictureLayered * layers, double timestamp, double endTimestamp) = 0;
    };

protected:
    //!
    Converter * converter;


private:
    //! U�ywane tylko w przypadku nieobs�ugiwania wyj�tk�w.
    VideoError * lastError;
    //! Zbuforowana d�ugo�� ramki.
    double frameDuration;
    //! Zbuforowana liczba klatek na sekund�.
    double frameRate;
    //! Zbuforowana d�ugo�� strumienia.
    double duration;
    //!
    double aspectRatio;
    //! Format piksela.
    PixelFormat format;
    //! Wysoko�� ramki.
    int width;
    //! Szeroko�� ramki.
    int height;
    //!
    Callback * callback;
    //! Zastosowany wzorzec "Pimpl" (�eby usun�� warningi przy budowie DLL)
    class VideoStreamImpl* impl;

protected:
    //! \param source �r�d�o obrazu.
    VideoStream(const std::string& source);

public:
    //! Destruktor wirtualny.
    virtual ~VideoStream();

public:
    //! Odczytuje nast�pn� klatk�.
    /** Przesuwa si� w strumieniu na nast�pn� ramk�. Modyfikacji ulega
      * czas oraz timestamp (nadane im s� te same warto�ci
      */
    virtual bool readNext() = 0;

    //! \retrun Pozycja w strumieniu.
    virtual double getTime() const = 0;
    //! \param time Pozycja w �r�dle
    virtual bool setTime(double time) = 0;
    //! \return Prawdziwy timestamp ramki.
    virtual double getFrameTimestamp() const = 0;
    //! \return Prawdziwy timestamp kolejnej ramki.
    virtual double getNextFrameTimestamp() const = 0;

    //! Konwertuje bie��c� ramk� do zadanego formatu. Docelowa ramka
    //! musi by� zaalokowana.
    //! \param dst
    virtual bool getFrame(Picture & dst);
    //! Konwertuje bie��c� ramk� do zadanego formatu. Docelowa ramka
    //! musi by� zaalokowana.
    //! \param dst
    virtual bool getFrame(PictureLayered & dst);

    //! \return �r�d�o.
    const std::string& getSource() const;
    //! \return Szeroko�� strumienia.
    inline int getWidth() const
    {
        return width;
    }
    //! \return Wysoko�� strumienia.
    inline int getHeight() const
    {
        return height;
    }
    //! \return Format strumienia.
    inline PixelFormat getPixelFormat() const
    {
        return format;
    }
    //! \return D�ugo�� strumienia.
    inline double getDuration() const
    {
        return duration;
    }
    //! \return D�ugo�� ramki w sekundach.
    inline double getFrameDuration() const
    {
        return frameDuration;
    }
    //! \return Liczba klatek na sekund�.
    inline double getFramerate() const
    {
        return frameRate;
    }
    //! \return ��czna, przybli�ona liczba klatek.
    int getFrameCount() const;

    //! \return Format.
    inline double getAspectRatio() const
    {
        return aspectRatio;
    }
    
    //! \return Znormalizowana pozycja w strumieniu.
    double getNormalizedTime() const;

    //! \param normalizedTime Znormalizowana pozycja w �r�dle.
    void setNormalizedTime(double normalizedTime);

    //! Czy to koniec strumienia?
    inline bool isEndOfStream() const
    {
        return getFrameTimestamp() >= getDuration();
    }

    //! Metoda u�ywana tylko w przypadku wy��czenia wyj�tk�w. Je�li jakakolwiek
    //! publiczna metoda zwr�ci false mo�na sprawdzi� dok�adnie b��d. Je�li wyj�tki
    //! s� w��czone zawsze zwr�ci NULL.
    //! \return Ostatni b��d.
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
    //! \param callback Wska�nik na callback. Strumie� NIE kontroluje odwo�ania, tj.
    //!                 samodzielnie nie go nie zwalnia ani nie zeruje.
    inline void setCallback(Callback * callback) 
    { 
        this->callback = callback; 
    }

protected:
    //! Metoda kt�r� nale�y wywo�a� po inicjalizacji strumienia w klasie pochodnej.
    //! \param source �r�d�o.
    bool onAfterInit(const std::string& source, double frameRate, double duration, 
      PixelFormat format, int width, int height, double aspectRatio);

    //!
    bool notifyError( const VideoError& error );

    //!
    void onFrameRead();

    //! Je�eli strumie� jest w stanie zaprezentowa� ramk� w postaci spakowanej
    //! powinien prze�adowa� t� metod�.
    //! \param dst
    virtual bool getData(Picture & /*dst*/) 
    { 
        return false; 
    }

    //! Je�eli strumie� jest w stanie zaprezentowa� ramk� w postaci zdj�cia
    //! powinien prze�adowa� t� metod�.
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