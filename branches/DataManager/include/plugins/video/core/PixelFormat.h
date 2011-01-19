#ifndef __AVINOUT_PIXELFORMAT_H__
#define __AVINOUT_PIXELFORMAT_H__

#include <utils/Enum.h>

////////////////////////////////////////////////////////////////////////////////
namespace video {
////////////////////////////////////////////////////////////////////////////////

//! Definicja formatów danych
#define VM_PIXELFORMAT_DEFINITION \
  PixelFormatYV12,\
  PixelFormatRESERVED0,\
  PixelFormatRGB24,\
  PixelFormatRESERVED1,\
  PixelFormatARGB = 27,\
  PixelFormatBGRA = 30,\
  PixelFormatUndefined

//! Format danych.
enum PixelFormat {
  VM_PIXELFORMAT_DEFINITION
};

////////////////////////////////////////////////////////////////////////////////
} // namespace video
////////////////////////////////////////////////////////////////////////////////

DECLARE_ENUM_ENCHANCEMENT(video::PixelFormat, VM_PIXELFORMAT_DEFINITION)

#endif  // __AVINOUT_PIXELFORMAT_H__