#include <corelib/Version.h>
#include <sstream>

using namespace core;

const int Version::major()
{
	return CORE_API_MAJOR;
}

const int Version::minor()
{
	return CORE_API_MINOR;
}

const int Version::patch()
{
	return CORE_API_PATCH;
}

const std::string Version::version()
{
	return version(major(), minor(), patch());
}

const std::string Version::version(const int major, const int minor, const int patch)
{
	std::stringstream ss;
	ss << major << "." << minor << "." << patch;

	return ss.str();
}

const bool Version::isAtLeast(const int major, const int minor, const int patch)
{
	return (10000 * major + 1000 * minor + patch) <=
		(10000 * Version::major() + 1000 * Version::minor() + Version::patch());
}

const bool Version::hasFeature(const std::string & feature)
{
	return false;
}