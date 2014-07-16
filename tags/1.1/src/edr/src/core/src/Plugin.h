/********************************************************************
    created:  2013/01/25
    created:  25:1:2013   16:22
    filename: Plugin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__PLUGIN_H__
#define HEADER_GUARD_CORE__PLUGIN_H__

#include <corelib/IPlugin.h>
#include <corelib/Filesystem.h>
#include <corelib/IIdentifiable.h>

namespace core {
/**
 *	Kontener na us�ugi.
 */
class Plugin : public IPlugin
{
public:
    //! Typ funkcji inicjuj�cej plugin kontekstem aplikacji i ladowanymi modulami.
    typedef void (*InitializeAndLoadFunction)(IPlugin * plugin, IApplication* coreApplication);
	//! Typ funkcji wype�niaj�cej podstawowy opis pluginu.
	typedef void (*SetPluginDescriptionFunction)(IPlugin * plugin);
    //! Typ funkcji pobieraj�cej wersj� API z kt�r� budowano plugin.
    typedef int (*GetAPIVersionFunction)(int * major, int * minor, int * patch);
    //! Typ funkcji pobieraj�cej typ builda pluginu
    typedef int (*GetBuildTypeFunction)();
    //! Typ funkcji pobieraj�cej wersje najwa�niejszych bibliotek od kt�rych uzale�niony jest core
    typedef int (*GetLibrariesVersionFunction)(int* boostVersion, int* qtVersion, int* stlVersion);
    //! Typ listy us�ug.
    typedef std::vector<plugin::IServicePtr> Services;
    //! Typ listy �r�de� danych DM.
    typedef std::vector<plugin::ISourcePtr> Sources;
    //! Typ listy parser�w.
    typedef std::vector<plugin::IParserPtr> Parsers;
    //! Typ listy wizualizator�w.
    typedef std::vector<plugin::IVisualizerPtr> Visualizers;
    //! Typ listy wrapper�w.
    typedef std::vector<utils::ObjectWrapperPtr> ObjectWrapperPrototypes;

private:
    //! Lista us�ug pluginu.
    Services services;
    //! Lista �r�d� danych DM pluginu.
    Sources sources;
    //! Lista parser�w pluginu.
	Parsers parsers;
    //! Lista wizualizator�w pluginu.
    Visualizers visualizers;
    //! Zbi�r wprowadzanych OW do aplikacji
    ObjectWrapperPrototypes objectWrapperPrototypes;

	//! Kod j�zyka domyslnego
	std::string defaultLanguageCode_;
    //! ID pluginu.
    UniqueID id;
    //! �cie�ka do pluginu.
    Filesystem::Path path;
	//! Opis pluginu
	plugin::Description desc;
	//! Dostawca pluginu
	utils::shared_ptr<plugin::VendorDescription> vendor_;
	//! Wersja pluginu
	utils::shared_ptr<Version> version_;

public:
	//! Domy�lny konstruktor
    Plugin();
	//! Destruktor wirtualny
	virtual ~Plugin();

    //! \return ID pluginu.
    virtual const UniqueID ID() const;

    //! \return Opis pluginu
    virtual const std::string description() const;

    //! \return Nazwa pluginu.
    virtual const std::string name() const;
	//! \return Kr�tka nazwa pluginu.
	virtual const std::string shortName() const;
    //! \return
    const Filesystem::Path& getPath() const;

	//! \return Wersja pluginu
	virtual const core::Version * version() const;
	//! \return Dostawca pluginu
	virtual const plugin::VendorDescription * vendor() const;

    //! \param path
    void setPath(const Filesystem::Path& path);

	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name);

	//! \param major G��wna wersja pluginu
	//! \param minor Podrz�dna wersja pluginu
	//! \param patch Wersja patcha pluginu
	virtual void setVersion(const core::Version::VersionNumberType major,
		const core::Version::VersionNumberType minor,
		const core::Version::VersionNumberType patch);

	//! \param name Nazwa dostawcy
	//! \param shortName Skr�cona nazwa dostawcy
	//! \param description Opis dostawcy
	//! \param contact Kontakt z dostawc�
	virtual void setVendor(const std::string & name,
		const std::string & shortName,
		const std::string & description,
		const std::string & contact);

	//! \param shortName Skr�cona nazwa pluginu
	//! \param description Opis pluginu	
	virtual void setDescription(const std::string & name,		
		const std::string & description);

	//! \param langCode Kod domy�lnego j�zyka pluginu wg ISO639
	virtual void setDefaultLanguageCode(const std::string & langCode);

	//! \return Kod domy�lnego j�zyka pluginu wg ISO639
	const std::string & defaultLanguageCode() const;

	//! \param id Identyfikator pluginu
	virtual void setID(UniqueID id);

    //! \service Us�uga do dodania do pluginu.
    virtual void addService(plugin::IServicePtr service);
    //! \return Liczba us�ug dostarczanych przez plugin.
    int getNumServices() const;
    //! \param i
    const plugin::IServicePtr & getService(int i);

    //! \service �r�d�o DM do dodania do pluginu.
    virtual void addSource(plugin::ISourcePtr source);
    //! \return Liczba �r�de� DM dostarczanych przez plugin.
    int getNumSources() const;
    //! \param i
    const plugin::ISourcePtr & getSource(int i);
	
    //! \parser Parser do dodania do pluginu.
	virtual void addParserPrototype(plugin::IParserPtr parser);
    //! \return Liczba parser�w dostarczanych przez plugin.
    int getNumParsersPrototypes() const;
    //! \param i
	const plugin::IParserPtr & getParserPrototype(int i);

    //! \visualizer Visualizer do dodania do pluginu.
    virtual void addVisualizerPrototype(plugin::IVisualizerPtr visualizer);
    //! \return Liczba visualizer�w dostarczanych przez plugin.
    int getNumVisualizerPrototypes() const;
    //! \param i
    const plugin::IVisualizerPtr & getVisualizerPrototype(int i);

	virtual void addObjectWrapperPrototype(utils::ObjectWrapperPtr objectWrapperPrototype);

    //! \return
    int getNumObjectWrapperPrototypes() const;
    //! \param i
    const utils::ObjectWrapperPtr & getObjectWrapperPrototype(int i);
	//! \return Czy plugin cokolwiek wnosi do aplikacji
	const bool empty() const;
};

//! Definicja wska�nika.
DEFINE_SMART_POINTERS(Plugin);

}

#endif	//	HEADER_GUARD_CORE__PLUGIN_H__
