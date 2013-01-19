/********************************************************************
    created:  2013/01/18
    created:  18:1:2013   17:02
    filename: CoreActionSide.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COREACTIONSIDE_H__
#define HEADER_GUARD___COREACTIONSIDE_H__

class CoreActionSide
{
public:

	enum SideType {Left, Right};

private:

	SideType side_;

protected:

	CoreActionSide(SideType side);

public:

	virtual ~CoreActionSide();

	const SideType side() const;

};

#endif	//	HEADER_GUARD___COREACTIONSIDE_H__
