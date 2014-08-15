/********************************************************************
	created:  2014/07/25	14:48:58
	filename: Types.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__

#include <map>
#include <hmdbserviceslib/ShallowCopy.h>
#include <plugins/hmdbCommunication/IHMDBRemoteContext.h>

namespace hmdbCommunication
{
	//! Typ wyliczeniowy opisuj�cy typy danych ruchu i medycznych
	enum DataType
	{
		FileType,		//! Plik
		MotionType,		//! Motion
		SessionType,	//! Sesja
		SubjectType,	//! Aktor
		PatientType,	//! Pacjent
	};

	//! Mapa identyfikator�w plikw do �ci�gni�cia z odpowiadaj�cymi �cie�kami na ftp
	typedef std::map<hmdbServices::ID, IHMDBRemoteContext::CompoundID> StorageFileNames;
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__TYPES_H__