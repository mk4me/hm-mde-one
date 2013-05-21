/********************************************************************
    created:  2012/12/10
    created:  10:12:2012   16:22
    filename: MRConnection.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRCONNECTION_H__
#define HEADER_GUARD___MRCONNECTION_H__

#include <dflib/IConnection.h>

class MRInputPin;
class MROutputPin;

class MRConnection
{
public:
	MRConnection(MROutputPin * src, MRInputPin * dest, df::IConnection * connection);
	~MRConnection();

public:

	//! \return Weze� �r�d�owy
	MROutputPin * source();

	//! \return Weze� docelowy
	MRInputPin * destination();

	//! \return Weze� �r�d�owy
	const MROutputPin * source() const;

	//! \return Weze� docelowy
	const MRInputPin * destination() const;

	df::IConnection * innerConnection();
	const df::IConnection * innerConnection() const;

private:
	MROutputPin * source_;
	MRInputPin * destination_;
	df::IConnection * innerConnection_;
};

#endif	//	HEADER_GUARD___MRCONNECTION_H__
