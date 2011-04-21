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

    //! Struktura przechowuj¹ca informacje o managerach. Tylko do wewnêtrznego u¿ytku.
    struct __ManagersData
    {
        IDataManager* dataManager;
    };

    //! Zmienna defininowana przez makro tworz¹ce pluginy. Tylko do u¿ytku wewnêtrznego.
    extern __ManagersData __managersData;

    //! Makro definiuj¹ce zmienn¹ przechowuj¹c¹ managery. Automatycznie u¿ywane w pluginach.
    #define CORE_DEFINE_MANAGERS_ACCESORS namespace core { __ManagersData __managersData = { nullptr }; }

    //! \return Bie¿¹ca instancja data managera. Rozwi¹zanie w ten sposób, w stosunku do
    //! klasycznego upublicznienia tylko nag³ówków funkcji i schowania definicji, pozwala
    //! na rozwijanie, wiêc jest potencjalnie szybsze.
    inline IDataManager* getDataManager()
    {
        return __managersData.dataManager;
    }

} // namespace core


#endif  // HEADER_GUARD_CORE__PLUGINCOMMON_H__