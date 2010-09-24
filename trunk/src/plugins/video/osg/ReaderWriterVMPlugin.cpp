/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#include <osgDB/Registry>
#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>

#include "../core/VMPrivate.h"
#include "../core/FFmpegVideoStream.h"
#include "VideoImageStream.h"

/**
 *	Wtyczka do OSG.
 */
class ReaderWriterVMPlugin : public osgDB::ReaderWriter
{
public:

  //! 
  ReaderWriterVMPlugin()
  {
    supportsProtocol("http","Read video/audio from http using ffmpeg.");
    supportsProtocol("rtsp","Read video/audio from rtsp using ffmpeg.");

    supportsExtension("vm",     "");
    supportsExtension("avi",    "");
    supportsExtension("flv",    "Flash video");
    supportsExtension("mov",    "Quicktime");
    supportsExtension("ogg",    "Theora movie format");
    supportsExtension("mpg",    "Mpeg movie format");
    supportsExtension("mpv",    "Mpeg movie format");
    supportsExtension("wmv",    "Windows Media Video format");
    supportsExtension("mkv",    "Matroska");
    supportsExtension("mjpeg",  "Motion JPEG");
    supportsExtension("mp4",    "MPEG-4");
    supportsExtension("sav",    "MPEG-4");
    supportsExtension("3gp",    "MPEG-4");
    supportsExtension("sdp",    "MPEG-4");
    supportsExtension("m1v",    "MPEG-1");

    // teoretycznie to powinno znaleŸæ siê gdzieœ w kodzie FFmpegVideoStream,
    // ale nie mam pomys³u, jak to tam umieœciæ
    vm::FFmpegVideoStream::setLockManager(lockManager);
  }

  //! 
  virtual ~ReaderWriterVMPlugin()
  {
  }

  //! 
  virtual const char * className() const
  {
    return "ReaderWriterVMPlugin";
  }

  //! 
  //! \param filename
  //! \param options
  virtual ReadResult readImage(const std::string & filename, const osgDB::ReaderWriter::Options * options) const
  {
    const std::string ext = osgDB::getLowerCaseFileExtension(filename);
    
    if (ext=="vm") { 
      return readImage(osgDB::getNameLessExtension(filename),options);
    }

    if (filename.compare(0, 5, "/dev/")==0) {
      return readImageStream(filename, options);
    }

    if (! acceptsExtension(ext)) {
      return ReadResult::FILE_NOT_HANDLED;
    }

    const std::string path = osgDB::containsServerAddress(filename) ?
      filename :
      osgDB::findDataFile(filename, options);

    if (path.empty()) {
      return ReadResult::FILE_NOT_FOUND;
    }

    return readImageStream(path, options);
  }
  
  //! 
  //! \param filename
  //! \param options
  ReadResult readImageStream(const std::string& filename, const osgDB::ReaderWriter::Options * options) const
  {
    OSG_INFO<< "ReaderWriterVMPlugin::readImage " << filename << std::endl;

    osg::ref_ptr<vmOSGPlugin::VideoImageStream> image_stream(new vmOSGPlugin::VideoImageStream);

    if (! image_stream->open(filename)) {
      return ReadResult::FILE_NOT_HANDLED;
    }

    return image_stream.release();
  }

private:
  //! 
  //! \param mutex
  //! \param op
  static int lockManager(void **mutex, vm::FFmpegVideoStream::LockOp op)
  {    
    using namespace vm;
    // pobieramy mutexa
    OpenThreads::Mutex **m=(OpenThreads::Mutex**)mutex;
    // operacja na mutexie
    switch (op) {
      case FFmpegVideoStream::LockOpCreate:
        *m=new OpenThreads::Mutex;
        return !*m;
      case FFmpegVideoStream::LockOpDestroy:
        delete *m;
        return 0;
      case FFmpegVideoStream::LockOpObtain:
        (*m)->lock();
        return 0;
      case FFmpegVideoStream::LockOpRelease:
        (*m)->unlock();
        return 0;
      default:
        return -1;
    }
  }
};

REGISTER_OSGPLUGIN(VideoManager, ReaderWriterVMPlugin)