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
	//! Słownik usług.
	typedef std::map<UniqueID, core::ISourcePtr> SourcesMap;
	//! Sekwencja usług.
	typedef std::vector<core::ISourcePtr> SourcesList;

private:
	//! Słownik usług.
	SourcesMap sourcesMap; 
	//! Sekwencja usług.
	SourcesList sourcesList;

public:
	SourceManager();
	virtual ~SourceManager(void);

public:

	void update(double deltaTime);

	//! Metoda finalizująca wszystkie serwisy  wywoływana przed niszczeniem managera
	void finalizeSources();

	// IServiceManager
public:
	//! Rejestruje źródło. Źródła muszą mieć unikatowe ID!
	//! \param source Źródło do rejestracji.
	virtual void registerSource(const core::ISourcePtr & source);
	//! \return Liczba źródeł.
	virtual int getNumSources() const;
	//! \param idx Indeks źródła.
	//! \return Źródło o zadanym indeksie.
	virtual core::ISourcePtr getSource(int idx);
	//! \param id ID źródła do wyszukania.
	//! \return Odnalezione źródło bądź NULL.
	virtual core::ISourcePtr getSource(UniqueID id);
};

#endif	//	HEADER_GUARD___SOURCEMANAGER_H__
