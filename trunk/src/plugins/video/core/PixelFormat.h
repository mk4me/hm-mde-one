#ifndef __AVINOUT_PIXELFORMAT_H__
#define __AVINOUT_PIXELFORMAT_H__

////////////////////////////////////////////////////////////////////////////////
namespace vm {
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
} // namespace vm
////////////////////////////////////////////////////////////////////////////////

DECLARE_ENUM_ENCHANCEMENT(vm::PixelFormat, VM_PIXELFORMAT_DEFINITION)

#endif  // __AVINOUT_PIXELFORMAT_H__