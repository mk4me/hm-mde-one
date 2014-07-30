/********************************************************************
	created:  2014/07/25	14:48:58
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__

namespace hmdbCommunication
{
	//! Typ wyliczeniowy opisuj¹cy typy danych ruchu i medycznych
	enum DataType
	{
		FileType,		//! Plik
		MotionType,		//! Motion
		SessionType,	//! Sesja
		SubjectType,	//! Aktor
		PatientType,	//! Pacjent
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__