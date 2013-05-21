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

namespace core {
/**
 *	Kontener na us≥ugi.
 */
class Plugin : public IPlugin
{
public:
    //! Typ funkcji tworzπcej plugin.
    typedef void (*FillFunction)(IPlugin * plugin, IApplication* coreApplication);
	//! Typ funkcji tworzπcej plugin.
	typedef void (*SetIDNameFunction)(IPlugin * plugin);
    //! Typ funkcji pobierajπcej wersjÍ pluginu.
    typedef int (*GetInterfaceVersionFunction)();
    //! Typ funkcji pobierajπcej typ builda pluginu
    typedef int (*GetBuildTypeFunction)();
    //!
    typedef int (*GetLibrariesVersionFunction)(int* boostVersion, int* qtVersion, int* stlVersion);
    //! Typ listy us≥ug.
    typedef std::vector<plugin::IServicePtr> Services;
    //! Typ listy ürÛde≥ danych DM.
    typedef std::vector<plugin::ISourcePtr> Sources;
    //! Typ listy parserÛw.
    typedef std::vector<plugin::IParserPtr> Parsers;
    //! Typ listy wizualizatorÛw.
    typedef std::vector<plugin::IVisualizerPtr> Visualizers;
    //! Typ listy wrapperÛw.
    typedef std::vector<ObjectWrapperPtr> ObjectWrapperPrototypes;

private:
    //! Lista us≥ug pluginu.
    Services services;
    //! Lista ürÛdÍ≥ danych DM pluginu.
    Sources sources;
    //! Lista parserÛw pluginu.
	Parsers parsers;
    //! Lista wizualizatorÛw pluginu.
    Visualizers visualizers;
    //! ZbiÛr wprowadzanych OW do aplikacji
    ObjectWrapperPrototypes objectWrapperPrototypes;

    //! Nazwa pluginu.
    std::string name;
	//! Nazwa pluginu.
	std::string description;
    //! ID pluginu.
    UniqueID id;
    //! åcieøka do pluginu.
    Filesystem::Path path;

public:
    Plugin();

public:
    //! \return ID pluginu.
    virtual UniqueID getID() const;

    //! \return nazwa pluginu
    virtual const std::string getDescription() const;

    //! \return Nazwa pluginu.
    const std::string getName() const;
    //! \return
    const Filesystem::Path& getPath() const;
    //! \param path
    void setPath(const Filesystem::Path& path);

	//! \param name Nazwa pluginu
	virtual void setName(const std::string & name);

	//! \param  description Opis pluginu
	virtual void setDescription(const std::string & description);

	//! \param id Identyfikator pluginu
	virtual void setID(UniqueID id);

    //! \service Us≥uga do dodania do pluginu.
    virtual void addService(plugin::IServicePtr service);
    //! \return Liczba us≥ug dostarczanych przez plugin.
    int getNumServices() const;
    //! \param i
    const plugin::IServicePtr & getService(int i);

    //! \service èrÛd≥o DM do dodania do pluginu.
    virtual void addSource(plugin::ISourcePtr source);
    //! \return Liczba ürÛde≥ DM dostarczanych przez plugin.
    int getNumSources() const;
    //! \param i
    const plugin::ISourcePtr & getSource(int i);
	
    //! \parser Parser do dodania do pluginu.
	virtual void addParserPrototype(plugin::IParserPtr parser);
    //! \return Liczba parserÛw dostarczanych przez plugin.
    int getNumParsersPrototypes() const;
    //! \param i
	const plugin::IParserPtr & getParserPrototype(int i);

    //! \visualizer Visualizer do dodania do pluginu.
    virtual void addVisualizerPrototype(plugin::IVisualizerPtr visualizer);
    //! \return Liczba visualizerÛw dostarczanych przez plugin.
    int getNumVisualizerPrototypes() const;
    //! \param i
    const plugin::IVisualizerPtr & getVisualizerPrototype(int i);

	virtual void addObjectWrapperPrototype(core::ObjectWrapperPtr objectWrapperPrototype);

    //! \return
    int getNumObjectWrapperPrototypes() const;
    //! \param i
    const ObjectWrapperPtr & getObjectWrapperPrototype(int i);
};

//! Definicja wskaünika.
typedef shared_ptr<Plugin> PluginPtr;
typedef shared_ptr<const Plugin> PluginConstPtr;

}

#endif	//	HEADER_GUARD_CORE__PLUGIN_H__
