/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:35
	filename: VideoImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__
#define __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__

#include <vidlib/Config.h>

#include <set>
#include <osg/ref_ptr>
#include <osg/observer_ptr>

#include <osg/ImageStream>
#include <OpenThreads/Thread>
#include <osg/Timer>

#include <utils/PtrWrapper.h>
#include <utils/PtrPolicyOSG.h>

#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////


/**
 *	Strumie� wideo.
 */
class VIDLIB_EXPORT VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
public:
    /** Domy�lne implementacje */
    META_Object(vidlib, VideoImageStream);

    //! Domy�lny format.
    static const PixelFormat defaultFormat = PixelFormatBGRA;

protected:
    //! Wrapper na surowy strumie�.
    typedef utils::PtrWrapper<VideoStream, utils::PtrPolicyOSG> Stream;
    //! Wrapper na mutex.
    typedef utils::Adapter<OpenThreads::Mutex, utils::PtrPolicyOSG> Mutex;
    //! Wrapper na timer.
    typedef utils::Adapter<osg::Timer, utils::PtrPolicyOSG> Timer;
    //! Typ blokady opartej na muteksie.
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
    //! Typ p�ytkich kopii strumienia.
    typedef utils::Adapter<std::set<osg::observer_ptr<VideoImageStream> >, utils::PtrPolicyOSG > Copies;

    //! Wewn�trzy strumie�.
    osg::ref_ptr<Stream> innerStream; 
    /** Mutex */
    osg::ref_ptr<Mutex> mutex;
    /** Timer */
    osg::ref_ptr<Timer> timer;
    /** */
    osg::ref_ptr<Copies> copies;

    /** Skala czasowa */
    double timeScale;
    /** Poprzednia pozycja w strumieniu */
    double prevTimestamp;
    /** Format piksela */
    PixelFormat targetFormat;
    //! Maksymalna szeroko��.
    int maxWidth;
    /** Bufor na bie��c� ramk� */
    Picture currentPicture;

public:
    /** Konstruktor zeruj�cy */
    VideoImageStream();
    /** Konstruktor kopiuj�cy */
    VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
    /** */
    virtual ~VideoImageStream();

    //! Otwiera strumie�
    //! \param filename �cie�ka do pliku.
    //! \param dummy Pozostawi� warto�� domy�ln�.
    template <class VideoStreamImpl>
    bool open(const std::string & filename, VideoStreamImpl* dummy = NULL)
    {
        try {
            setStream( new VideoStreamImpl(filename) );
            return true;
        } catch ( VideoError& error ) {
            OSG_FATAL<<"open("<<filename<<") reported an error: "<< error.what() << std::endl;
            return false;
        }
    }

    /** Odtwarzanie strumienia (nowy w�tek!) */
    virtual void play();
    /** Zatrzymanie strumienia */
    virtual void pause();
    /** Odtwarzanie "do ty�u" */
    virtual void rewind();
    /** Wyszukiwanie w strumieniu */
    virtual void seek(double time);
    /** Zamkni�cie strumienia */
    virtual void quit(bool waitForThreadToExit = true);

    /** Czas utworzenia */
    virtual double getCreationTime() const;
    /** D�ugo�� strumienia */
    virtual double getLength() const;
    /** Liczba klatek na sekund� */
    virtual double getFrameRate() const;
    /** Czas w strumieniu */
    virtual double getReferenceTime() const;
    /** Czas w strumieniu */
    virtual void setReferenceTime(double value) { seek(value); }
    /** Skala czasowa */
    virtual void setTimeMultiplier(double timescale);
    /** Skala czasowa */
    virtual double getTimeMultiplier() const;
    /** Czy wideo ma kana� alpha? */
    virtual bool isImageTranslucent() const;




    


    //!
    virtual bool requiresUpdateCall() const;
    //!
    virtual void update(osg::NodeVisitor* nv);

    //! \return Format do kt�rego film jest konwertowany.
    inline PixelFormat getTargetFormat() const
    {
        return targetFormat;
    }
    //! \param targetFormat Format do kt�rego film jest konwertowany.
    void setTargetFormat(PixelFormat targetFormat); 

    //! \return Maksymalna szeroko�� konwertowanego strumienia.
    inline int getMaxWidth() const
    { 
        return maxWidth;
    }
    //! \param maxWidth Maksymalna szeroko�� konwertowanego strumienia.
    void setMaxWidth(int maxWidth);

protected:

    /** W�tek odtwarzaj�cy */
    virtual void run();
    /** Tryb zap�tlania */
    virtual void applyLoopingMode();
    /** Ustawia bie��c� ramk� (je�li jest nowsza ni� poprzednia)*/
    virtual void publishFrameAndNotify();
    /** Ustawia bie��c� ramk� (je�li jest nowsza ni� poprzednia)*/
    virtual void publishFrame();
    /** Ustawia bie��cy czas */
    virtual void setStreamTime( double time );
    /** Ustawia bie��c� ramk� */
    void reloadImage(Picture& picture);


    //! \param stream Wewn�trzny strumie�.
    void setStream( VideoStream* stream );

    //! \return Wewn�trzny strumie�.
    inline VideoStream* getStream()
    {
        return *(innerStream.get());
    }
    //! \return Wewn�trzny strumie�.
    inline const VideoStream* getStream() const
    {
        return *(innerStream.get());
    }
    //!
    inline OpenThreads::Mutex& getMutex()
    {
        return mutex->get();
    }
    //!
    inline osg::Timer& getTimer()
    {
        return timer->get();
    }

};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif // __HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__