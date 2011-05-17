#include "CorePCH.h"
#include <core/Filesystem.h>
#include <boost/filesystem.hpp>

using namespace boost;

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

void Filesystem::createDirectory(const std::string& path)
{
	if(path.empty())
	{
		return;
	}

	filesystem::path dirPath(path);

	if(!filesystem::is_directory(dirPath))
	{
		filesystem::create_directories(dirPath);
	}
}

void Filesystem::deleteDirectory(const std::string& path)
{
	if(path.empty())
	{
		return;
	}

	filesystem::path dirPath(path);

	if(filesystem::is_directory(dirPath))
	{
		filesystem::remove_all(dirPath);
	}
}

void Filesystem::deleteFile(const std::string& path)
{
	if(path.empty())
	{
		return;
	}

	filesystem::path dirPath(path);

	if(filesystem::exists(dirPath) && !filesystem::is_directory(dirPath))
	{
		filesystem::remove(dirPath);
	}
}

void Filesystem::move(const std::string& pathOld, const std::string& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}
	filesystem::path oldPath(pathOld);
	filesystem::path newPath(pathNew);

	filesystem::copy_file(oldPath, newPath);
    filesystem::remove(oldPath);
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::string& mask)
{
	std::vector<std::string> files;

	filesystem::path dirPath(path);
	if(filesystem::exists(dirPath))
	{
		if(recursive)
		{
			std::vector<std::string> dirs = Filesystem::listSubdirectories(path);
			BOOST_FOREACH(std::string& dir, dirs)
			{
				std::vector<std::string> subfiles = Filesystem::listFiles(dir, recursive, mask);
				BOOST_FOREACH(std::string& file, subfiles)
				{
					files.push_back(file);
				}
			}
		}
		filesystem::directory_iterator end;
		for(filesystem::directory_iterator iter(dirPath) ; iter != end ; ++iter)
		{
			if(!filesystem::is_directory(iter->status()) && filesystem::extension(iter->path()).compare(mask) == 0)
			{
				files.push_back(iter->path().string());
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks)
{
	std::vector<std::string> files;

	filesystem::path dirPath(path);
	if(filesystem::exists(dirPath))
	{
		if(recursive)
		{
			std::vector<std::string> dirs = Filesystem::listSubdirectories(path);
			BOOST_FOREACH(std::string& dir, dirs)
			{
				std::vector<std::string> subfiles = Filesystem::listFiles(dir, recursive, masks);
				BOOST_FOREACH(std::string& file, subfiles)
				{
					files.push_back(file);
				}
			}
		}
		filesystem::directory_iterator end;
		for(filesystem::directory_iterator iter(dirPath) ; iter != end ; ++iter)
		{
			BOOST_FOREACH(std::string mask, masks)
			{
				if(!filesystem::is_directory(iter->status()) && filesystem::extension(iter->path()).compare(mask) == 0)
				{
					files.push_back(iter->path().string());
				}
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listSubdirectories(const std::string& path)
{
	std::vector<std::string> subdirs;

	filesystem::path dirPath(path);
	if(filesystem::exists(dirPath))
	{
		filesystem::directory_iterator end;
		for(filesystem::directory_iterator iter(dirPath) ; iter != end ; ++iter)
		{
			if(filesystem::is_directory(iter->status()))
			{
				subdirs.push_back(iter->path().string());
			}
		}
	}
	return subdirs;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
