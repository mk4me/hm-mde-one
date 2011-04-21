/********************************************************************
    created:  2011/04/21
    created:  21:4:2011   7:59
    filename: PluginCommon.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGINCOMMON_H__
#define HEADER_GUARD_CORE__PLUGINCOMMON_H__

namespace core 
{
    class IDataManager;

    //! Struktura przechowuj�ca informacje o managerach. Tylko do wewn�trznego u�ytku.
    struct __ManagersData
    {
        IDataManager* dataManager;
    };

    //! Zmienna defininowana przez makro tworz�ce pluginy. Tylko do u�ytku wewn�trznego.
    extern __ManagersData __managersData;

    //! Makro definiuj�ce zmienn� przechowuj�c� managery. Automatycznie u�ywane w pluginach.
    #define CORE_DEFINE_MANAGERS_ACCESORS namespace core { __ManagersData __managersData = { nullptr }; }

    //! \return Bie��ca instancja data managera. Rozwi�zanie w ten spos�b, w stosunku do
    //! klasycznego upublicznienia tylko nag��wk�w funkcji i schowania definicji, pozwala
    //! na rozwijanie, wi�c jest potencjalnie szybsze.
    inline IDataManager* getDataManager()
    {
        return __managersData.dataManager;
    }

} // namespace core


#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__