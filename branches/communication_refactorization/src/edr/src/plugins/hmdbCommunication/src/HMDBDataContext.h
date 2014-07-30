/********************************************************************
	created:  2014/07/11	17:36:28
	filename: HMDBDataContext.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__

#include <plugins/hmdbCommunication/IHMDBDataContext.h>

namespace hmdbCommunication
{
	class IHMDBStorage;

	class HMDBDataContext : public IHMDBDataContext
	{
	public:
		HMDBDataContext(IHMDBStorage * storage, const std::string & userName,
			const std::string & userHash);

		virtual ~HMDBDataContext();

		//! \return Obiekt zarz¹dzaj¹cy œci¹gniêtymi danymi
		virtual const IHMDBStorage * storage() const;
		//! \return Obiekt zarz¹dzaj¹cy œci¹gniêtymi danymi
		virtual IHMDBStorage * storage();
		//! \return User hash na potrzeby obs³ugi p³ytkiej kopii bazy danych w storage
		virtual const std::string userName() const;
		//! \return User hash na potrzeby obs³ugi p³ytkiej kopii bazy danych w storage
		virtual const std::string userHash() const;

	private:

		IHMDBStorage * storage_;

		const std::string userHash_;

		const std::string userName_;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__HMDBDATACONTEXT_H__