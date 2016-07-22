/********************************************************************
	created:  2016/07/18
	filename: DataSource.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__DATASOURCE_H__
#define __HEADER_GUARD_SKIDATA__DATASOURCE_H__

#include <corelib/ISource.h>
#include <utils/Filesystem.h>
#include <map>
#include <list>
#include <corelib/Variant.h>
#include <corelib/IHierarchyItem.h>

namespace skidata
{
	class SkiDataSource : public plugin::ISource
	{
		UNIQUE_ID("{441BB894-1019-4382-BACA-F18A511A49CB}");
		CLASS_DESCRIPTION("Ski data source", "Ski data source");

	private:

		struct RunDescription
		{
			std::list<std::string> files;
			core::VariantsList generatedData;
			bool loaded;
			unsigned int idx;
			core::IHierarchyItemPtr hierarchyItem;
		};

	public:
		//! Konstruktor
		SkiDataSource();
		//! Destruktor wirtualny
		virtual ~SkiDataSource();

		//! Inicjalizacja �r�d�a. Nast�puje ju� po wczytaniu plugin�w i skonstruowaniu
		//! (nie zainicjalizowaniu!) wszystkich �r�de�.
		virtual void init(core::IDataManager * memoryDM,
			core::IStreamDataManager * streamDM,
			core::IFileDataManager * fileDM,
			core::IDataHierarchyManager * hierarchyDM);

		//! Zobacz podobn� metode dla IService
		//! P�na inicjalizacja �r�d�a, nast�puje po wczytaniu i inicjalizacji wszystkich innych �r�de�
		virtual bool lateInit();

		//! Metoda powinna w bezpieczny spos�b zwalniac zasoby, maj�c na uwadze �e niekoniecznie wszystkie us�ugi i zasoby pobrane z zewn�trz s� jeszcze dost�pne.
		//! Ta metoda w szczegolnoscis powinna zamknac wszystkie watki, kt�re uruchomi� serwis, mo�e tez zwalniac pamie� przydzielon� dynamicznie
		//! Generalnie to taki bezpieczny destruktor uniezale�niaj�cy dana us�ug� od pozosta�ych us�ug i przygotowuj�cy ja na usuni�cie
		virtual void finalize();

		//! Metoda aktualizuj�ca pochodzi z w�tku UI! Powinny tu by� realizowane lekkie operacje od�wie�ania widget�w!!
		//! Jako parametr dostajemy przyrost czasu jaki mina� od poprzedniego wywo�ania
		virtual void update(double deltaTime);

		//! �r�d�o nie musi mie� wizualnej reprezentacji.
		//! \return Widget tworzony przez �r�d�o b�d� NULL.
		virtual QWidget* getWidget();
		//! \return Widget kontroluj�cy zachowanie us�ugi/us�ug zale�nych.
		virtual QWidget* getControlWidget();
		//! \return Widget dostarczaj�cy opcji zwi�zanych z us�ug�/us�ugami zale�nymi.
		virtual QWidget* getSettingsWidget();
		//! \param offeredTypes Typy oferowane przez to �r�d�o
		virtual void getOfferedTypes(utils::TypeInfoList & offeredTypes) const;

		unsigned int addRun(const utils::Filesystem::Path & path);
		void removeRun(const utils::Filesystem::Path & path);
		std::list<utils::Filesystem::Path> currentRuns() const;

		void loadRun(const utils::Filesystem::Path & path);
		void unloadRun(const utils::Filesystem::Path & path);

		bool isLoaded(const utils::Filesystem::Path & path);

	private:

		static unsigned int runIdx;
		core::IHierarchyItemPtr hierarchyRoot;
		std::map<utils::Filesystem::Path, RunDescription> data;

		core::IDataManager * memoryDM;			
		core::IFileDataManager * fileDM;
		core::IDataHierarchyManager * hierarchyDM;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__DATASOURCE_H__