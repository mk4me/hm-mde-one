/********************************************************************
	created:  2014/08/19	08:47:18
	filename: IHMDBSourceViewManager.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCEVIEWMANAGER_H__
#define __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCEVIEWMANAGER_H__

#include <QtCore\QString>
#include <QtGui\QIcon>
#include <plugins\hmdbCommunication\IHMDBShallowCopyContext.h>
#include <plugins\hmdbCommunication\IHierarchyPerspective.h>

class QWidget;

namespace hmdbCommunication
{
	class IDataSourcePerspective;
	class IDataSourceContent;
	class ShallowCopyFilter;


	class IHMDBSourceViewManager
	{
	public:

		//! Struktura opisuj�ca konfiguracj� u�ytkownika
		struct UserCredentialsConfiguration
		{
			QString user;		//! User
			QString password;	//! Has�o
		};

		//! Struktura opisuj�ca konfiguracj� storage
		struct StorageConfiguration
		{
			QString path;		//! �cie�ka do storage
			QString password;	//! Has�o
		};

		struct ServiceConfiguration
		{
			QString url;		//! Adres us�ugi
			QString caPath;		//! �cie�ka certyfikatu SSL
		};

		//! Struktura opisuj�ca dost�p do us�ug zdalnych
		struct RemoteConfiguration
		{
			ServiceConfiguration serviceConfiguration;
			UserCredentialsConfiguration userConfiguration;
		};

		//! Struktura opisuj�ca konfiguracje ca�ego kontekstu
		struct ContextConfiguration
		{
			QString name;										//! Nazwa konfiguracji (unikalna!!)
			StorageConfiguration storageConfiguration;			//! Konfiguracja storage
			RemoteConfiguration motionServicesConfiguration;	//! Konfiguracja us�ug ruchu
			RemoteConfiguration motionDataConfiguration;		//! Konfiguracja danych ruchu
			RemoteConfiguration medicalServicesConfiguration;	//! Konfiguracja us�ug medycznych
			RemoteConfiguration medicalDataConfiguration;		//! Konfiguracja danych medycznych
		};

		//! Interfejs widok�w kontekstu �r�d�a
		class IHMDBSourceContextView
		{
		public:
			//! Destruktor wirtualny
			virtual ~IHMDBSourceContextView() {}
			//! \return Nazwa widoku
			virtual const QString name() const = 0;
			//! \param shallowCopyContext Kontekst p�ytkiej kopii bazy danych jakim zasilamy widok
			//! \return Widok obs�uguj�cy kontekst
			virtual QWidget * createView(IHMDBShallowCopyContextPtr shallowCopyContext, IHMDBSourceViewManager * viewManager) = 0;
			//! \return Ikona widoku
			virtual const QIcon icon() const { return QIcon(); };
			//! \return Czy dany widok wymaga po��czenia z us�ugami webowymi
			virtual const bool requiresRemoteContext() const = 0;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IHMDBSourceViewManager() {}
		//! \param viewPrototype Prototyp widoku kontekst�w
		virtual const bool registerViewPrototype(IHMDBSourceContextView * viewPrototype) = 0;
		//! \return Ilo�c zarejestrowanych prototyp�w widok�w
		virtual const unsigned int viewPrototypesCount() const = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * viewPrototype(const QString & prototypeName) = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * viewPrototype(const QString & prototypeName) const = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual IHMDBSourceContextView * viewPrototype(const unsigned int idx) = 0;
		//! \param idx Indeks prototypu widoku
		//! \return Prototyp widoku
		virtual const IHMDBSourceContextView * viewPrototype(const unsigned int idx) const = 0;
		//! \param config Konfiguracja widoku
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda�o si� zarejestrowa� konfiguracj� (nazwa musi by� unikalna!!)
		virtual const bool registerConfiguration(const ContextConfiguration & config,
			const QString & prototypeName = QString()) = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Ilo�� konfiguracji prototypu
		virtual const unsigned int configurationsCount(const QString & prototypeName = QString()) const = 0;
		//! \param idx Indeks konfiguracji
		//! \param prototypeName Nawa prototypu
		//! \return Konfiguracja
		virtual const ContextConfiguration configuration(const unsigned int idx, const QString & prototypeName = QString()) const = 0;		
		//! \param perspective Perspektywa danych
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda�o si� zarejestrowa� perspektyw� danych (nazwa musi by� unikalna!!)
		virtual const bool registerPerspective(IDataSourcePerspective * perspective, const QString & prototypeName = QString()) = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Ilo�� perspektyw prototypu
		virtual const unsigned int perspectivesCount(const QString & prototypeName = QString()) const = 0;
		//! \param idx Indeks perspektywy
		//! \param prototypeName Nawa prototypu
		//! \return Perspektywa
		virtual IDataSourcePerspective * perspective(const unsigned int idx, const QString & prototypeName = QString()) = 0;
		//! \param idx Indeks perspektywy
		//! \param prototypeName Nawa prototypu
		//! \return Perspektywa
		virtual const IDataSourcePerspective * perspective(const unsigned int idx, const QString & prototypeName = QString()) const = 0;
		//! \param viewPrototype Prototyp widoku
		//! \param content Zawarto�� perspektywy
		//! \return Czy uda�o si� zarejestrowa� content (nazwa musi by� unikalna!!)
		virtual const bool registerContent(IDataSourceContent * content, const QString & prototypeName = QString()) = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Ilo�� content� perspektywy
		virtual const unsigned int contentsCount(const QString & prototypeName = QString()) const = 0;
		//! \param idx Indeks contentu
		//! \param prototypeName Nawa prototypu
		//! \return Content
		virtual IDataSourceContent * content(const unsigned int idx, const QString & prototypeName = QString()) = 0;
		//! \param idx Indeks contentu
		//! \param prototypeName Nawa prototypu
		//! \return Content
		virtual const IDataSourceContent * content(const unsigned int idx, const QString & prototypeName = QString()) const = 0;
		//! \param filter Filtr danych
		//! \param prototypeName Nawa prototypu
		//! \return Czy uda�o si� zarejestrowa� filtr (nazwa musi by� unikalna!!)
		virtual const bool registerFilter(ShallowCopyFilter * filter, const QString & prototypeName = QString()) = 0;
		//! \param prototypeName Nawa prototypu
		//! \return Ilo�� filtr�w
		virtual const unsigned int filtersCount(const QString & prototypeName = QString()) const = 0;
		//! \param idx Indeks filtru
		//! \param prototypeName Nawa prototypu
		//! \return Filtr
		virtual ShallowCopyFilter * filter(const unsigned int idx, const QString & prototypeName = QString()) = 0;
		//! \param idx Indeks filtru
		//! \param prototypeName Nawa prototypu
		//! \return Filtr
		virtual const ShallowCopyFilter * filter(const unsigned int idx, const QString & prototypeName = QString()) const = 0;
		//! \param hierarchyPerspective Perspektywa dla analiz
		//! \return Czy uda�o si� doda� perspektywe dla analiz
		virtual const bool addHierarchyPerspective(IHierarchyPerspectivePtr hierarchyPerspective) = 0;
		//! \return Ilo�c zarejestrownaych perspektyw dla analiz
		virtual const unsigned int hierarchyPerspectivesCount() const = 0;
		//! \param idx Indeks perspektywy
		//! \return Perspektywa dla analiz
		virtual const IHierarchyPerspectivePtr hierarchyPerspective(const unsigned int idx) = 0;
		//! \param idx Indeks perspektywy
		//! \return Perspektywa dla analiz
		virtual const IHierarchyPerspectiveConstPtr hierarchyPerspective(const unsigned int idx) const = 0;
	};
}

#endif	// __HEADER_GUARD_HMDBCOMMUNICATION__IHMDBSOURCEVIEWMANAGER_H__