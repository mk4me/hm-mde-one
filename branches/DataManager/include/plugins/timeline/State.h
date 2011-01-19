/********************************************************************
	created:  2010/10/12
	created:  12:10:2010   16:08
	filename: State.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__TIMELINE_STATE_H__
#define __HEADER_GUARD__TIMELINE_STATE_H__

////////////////////////////////////////////////////////////////////////////////
namespace timeline {
////////////////////////////////////////////////////////////////////////////////

//! Stan timeline'a.
struct State 
{
    //! Czy jest odtwarzany?
    bool isPlaying;
    //! Czas.
    double time;
    //! D³ugoœæ strumienia. 
    double length;
    //! Znormalizowany czas.
    double normalizedTime;
    //! Jaka jest skala czasowa?
    double timeScale;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace timeline
////////////////////////////////////////////////////////////////////////////////

#endif  // __HEADER_GUARD__TIMELINE_STATE_H__