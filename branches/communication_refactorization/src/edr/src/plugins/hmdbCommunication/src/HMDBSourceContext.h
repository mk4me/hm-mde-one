/********************************************************************
	created:  2014/06/02	14:28:10
	filename: HMDBSourceContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBSourceContext.h>

namespace hmdbCommunication
{
	class HMDBSourceContext : public IHMDBSourceContext
	{
	public:
		//! Konstruktor
		//! \param dataContext Kontekst danych
		//! \param localContext Kontekst danych lokalnych
		//! \param remoteContext Kontekst zdalnych danych i operacji
		HMDBSourceContext(IHMDBDataContext * dataContext,
			IHMDBLocalContext * localContext,
			IHMDBRemoteContext * remoteContext);
		//! Destruktor wirtualny
		virtual ~HMDBSourceContext();

		//! Kontekst lokalny
		virtual IHMDBLocalContext * localContext();
		virtual const IHMDBLocalContext * localContext() const;

		//! Kontekst zdalny
		virtual IHMDBRemoteContext * remoteContext();
		virtual const IHMDBRemoteContext * remoteContext() const;

		//! Kontekst danych
		virtual IHMDBDataContext * dataContext();
		virtual const IHMDBDataContext * dataContext() const;

	private:
		//! Sesja us³ug bazy danych
		utils::shared_ptr<IHMDBLocalContext> localContext_;
		//! Storage dla danych
		utils::shared_ptr<IHMDBRemoteContext> remoteContext_;
		//! Aktualna p³ytka kopia bazy danych
		utils::shared_ptr<IHMDBDataContext> dataContext_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBSOURCECONTEXT_H__