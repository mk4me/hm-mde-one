#include <vidlib/FFmpegVideoStream.h>
#include "FFmpegVideoStreamTest.h"
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>

using namespace vidlib;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FFmpegVideoStreamTest );


typedef boost::scoped_ptr<FFmpegVideoStream> StreamPtr;

#define FILE_PATH "video/1.avi"
#define NONEXISTENT_FILE_PATH "video/nonexistent.avi"

void FFmpegVideoStreamTest::testOpen()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
}

void FFmpegVideoStreamTest::testClose()
{
}

void FFmpegVideoStreamTest::testDecode()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    for ( double t = 0; t <= stream->getDuration(); t += 0.001 ) {
        stream->setTime(t);
    }
}

void FFmpegVideoStreamTest::testOpenFail()
{
    StreamPtr stream(new FFmpegVideoStream(NONEXISTENT_FILE_PATH));
}

void FFmpegVideoStreamTest::testConvert()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    stream->setTime(0);

    PixelFormat formats[] = { PixelFormatYV12, PixelFormatRGB24, PixelFormatBGRA };
    BOOST_FOREACH(PixelFormat format, formats) {
        ScopedPicture pict(stream->getWidth(), stream->getHeight(), format);
        stream->getFrame(pict);
    }
}

void FFmpegVideoStreamTest::testNegativeTimeFail()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    stream->setTime(-1.0);
}

void FFmpegVideoStreamTest::testOverflowTimeFail()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    stream->setTime(stream->getTime() + DBL_MIN);
}

void FFmpegVideoStreamTest::testUndefinedTimeFail()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    ScopedPicture pict(stream->getWidth(), stream->getHeight(), PixelFormatYV12);
    stream->getFrame(pict);
}

void FFmpegVideoStreamTest::testBoundaries()
{
    StreamPtr stream(new FFmpegVideoStream(FILE_PATH));
    stream->setTime(0);
    stream->setTime(stream->getDuration());
    stream->setTime(0);
    stream->setTime(stream->getDuration());
}