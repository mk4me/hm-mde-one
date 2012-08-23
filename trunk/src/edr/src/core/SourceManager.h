/********************************************************************
    created:  2012/03/19
    created:  19:3:2012   11:25
    filename: SourceManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___SOURCEMANAGER_H__
#define HEADER_GUARD___SOURCEMANAGER_H__

#include <core/ISourceManager.h>
#include <core/ISource.h>
#include <map>
#include <vector>
#include "ManagerHelper.h"

class SourceManager: public core::ISourceManager, public ManagerHelper<SourceManager>
{
public:
	//! S≥ownik us≥ug.
	typedef std::map<UniqueID, core::ISourcePtr> SourcesMap;
	//! Sekwencja us≥ug.
	typedef std::vector<core::ISourcePtr> SourcesList;

private:
	//! S≥ownik us≥ug.
	SourcesMap sourcesMap; 
	//! Sekwencja us≥ug.
	SourcesList sourcesList;

public:
	SourceManager();
	virtual ~SourceManager(void);

public:

	void update(double deltaTime);

	//! Metoda finalizujπca wszystkie serwisy  wywo≥ywana przed niszczeniem managera
	void finalizeSources();

	// IServiceManager
public:
	//! Rejestruje ürÛd≥o. èrÛd≥a muszπ mieÊ unikatowe ID!
	//! \param source èrÛd≥o do rejestracji.
	virtual void registerSource(const core::ISourcePtr & source);
	//! \return Liczba ürÛde≥.
	virtual int getNumSources() const;
	//! \param idx Indeks ürÛd≥a.
	//! \return èrÛd≥o o zadanym indeksie.
	virtual core::ISourcePtr getSource(int idx);
	//! \param id ID ürÛd≥a do wyszukania.
	//! \return Odnalezione ürÛd≥o bπdü NULL.
	virtual core::ISourcePtr getSource(UniqueID id);
};

#endif	//	HEADER_GUARD___SOURCEMANAGER_H__
