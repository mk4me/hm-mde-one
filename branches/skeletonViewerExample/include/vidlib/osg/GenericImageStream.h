/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:15
	filename: GenericImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__
#define __HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__



#include <vidlib/osg/VideoImageStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//! ImageStream korzystaj¹cy z FFmpegVideoStream
template <class InnerStream>
class VIDLIB_EXPORT GenericImageStream : public VideoImageStream
{
public:
    /** Domyœlne implementacje */
    META_Object(vidlib, GenericImageStream);

    /** Konstruktor zeruj¹cy */
    GenericImageStream()
    {}

    /** Konstruktor kopiuj¹cy */
    GenericImageStream(const GenericImageStream& image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY) :
    VideoImageStream(image, copyop)
    {}

    /** Otwiera strumieñ */
    virtual bool open(const std::string & filename)
    {
        try {
            setStream( new InnerStream(filename) );
            return true;
        } catch ( VideoError& error ) {
            OSG_FATAL<<"open("<<filename<<") reported an error: "<< error.what() << std::endl;
            return false;
        }
    }
};


////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD_VIDLIB__GENERICIMAGESTREAM_H__