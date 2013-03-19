#include "CorePCH.h"
#include "Plugin.h"

using namespace core;

Plugin::Plugin()
{

}

UniqueID Plugin::getID() const
{
	return id;
}     

const std::string Plugin::getDescription() const
{
	return name;
}

const std::string Plugin::getName() const
{
	return name;
}

const Filesystem::Path& Plugin::getPath() const
{ 
	return path;
}

void Plugin::setPath(const Filesystem::Path& path) 
{ 
	this->path = path; 
}

void Plugin::setName(const std::string & name)
{
	this->name = name;
}

void Plugin::setDescription(const std::string & description)
{
	this->description = description;
}

void Plugin::setID(UniqueID id)
{
	this->id = id;
}

void Plugin::addService(plugin::IServicePtr service)
{
	services.push_back(service);
}

int Plugin::getNumServices() const
{
	return static_cast<int>(services.size());
}

const plugin::IServicePtr & Plugin::getService(int i)
{
	return services[i];
}

void Plugin::addSource(plugin::ISourcePtr source)
{
	sources.push_back(source);
}

int Plugin::getNumSources() const
{
	return static_cast<int>(sources.size());
}

const plugin::ISourcePtr & Plugin::getSource(int i)
{
	return sources[i];
}

void Plugin::addParserPrototype(plugin::IParserPtr parser)
{
	this->parsers.push_back(parser);
}

int Plugin::getNumParsersPrototypes() const
{
	return static_cast<int>(this->parsers.size());
}

const plugin::IParserPtr & Plugin::getParserPrototype(int i)
{
	return this->parsers[i];
}

void Plugin::addVisualizerPrototype(plugin::IVisualizerPtr visualizer)
{
	this->visualizers.push_back(visualizer);
}

int Plugin::getNumVisualizerPrototypes() const
{
	return static_cast<int>(this->visualizers.size());
}

const plugin::IVisualizerPtr & Plugin::getVisualizerPrototype(int i)
{
	return this->visualizers[i];
}

void Plugin::addObjectWrapperPrototype(core::ObjectWrapperPtr objectWrapperPrototype)
{
	this->objectWrapperPrototypes.push_back(objectWrapperPrototype);
}

int Plugin::getNumObjectWrapperPrototypes() const
{
	return static_cast<int>(this->objectWrapperPrototypes.size());
}

const ObjectWrapperPtr & Plugin::getObjectWrapperPrototype(int i)
{
	return this->objectWrapperPrototypes[i];
}