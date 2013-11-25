#include "CoreLibPCH.h"
#include <boost/foreach.hpp>
#include <corelib/Filesystem.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

using namespace boost::filesystem;

void Filesystem::createDirectory(const std::string& path)
{
	createDirectory(Path(path));
}

void Filesystem::createDirectory(const Path& path)
{
    if(path.empty())
    {
        return;
    }

    if(isDirectory(path) == false)
    {
        create_directories(path);
    }
}

void Filesystem::deleteDirectory(const std::string& path)
{
    deleteDirectory(Path(path));
}

void Filesystem::deleteDirectory(const Path& path)
{
	if(path.empty())
	{
		return;
	}

	if(isDirectory(path) == true)
	{
		remove_all(path);
	}
}

void Filesystem::deleteFile(const std::string& path)
{
    deleteFile(Path(path));
}

void Filesystem::deleteFile(const Path& path)
{
	if(path.empty())
	{
		return;
	}

	if(pathExists(path) == true && isDirectory(path) == false)
	{
		remove(path);
	}
}

void Filesystem::move(const std::string& pathOld, const std::string& pathNew)
{
    move(Path(pathOld), Path(pathNew));
}

void Filesystem::move(const Path& pathOld, const Path& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}

	Filesystem::copy(pathOld, pathNew);
    remove(pathOld);
}

void Filesystem::copy(const std::string& pathOld, const std::string& pathNew)
{
	Filesystem::copy(Path(pathOld), Path(pathNew));
}

void Filesystem::copy(const Path& pathOld, const Path& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}

	boost::filesystem::copy(pathOld, pathNew);
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive)
{
    return listFiles(Path(path), recursive);
}

std::vector<std::string> Filesystem::listFiles(const Path& path, bool recursive)
{
    std::vector<std::string> files;

    if(pathExists(path) == true)
    {
        if(recursive)
        {
            std::vector<std::string> dirs = listSubdirectories(path);
            BOOST_FOREACH(std::string& dir, dirs)
            {
                std::vector<std::string> subfiles = listFiles(dir, recursive);
                BOOST_FOREACH(std::string& file, subfiles)
                {
                    files.push_back(file);
                }
            }
        }
        Iterator end;
        for(Iterator iter(path) ; iter != end ; ++iter)
        {
            if(isDirectory(iter->path()) == false)
            {
                files.push_back(iter->path().string());
            }
        }
    }
    return files;
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::string& mask)
{
    return listFiles(Path(path), recursive, mask);
}

std::vector<std::string> Filesystem::listFiles(const Path& path, bool recursive, const std::string& mask)
{
	std::vector<std::string> files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			std::vector<std::string> dirs = listSubdirectories(path);
			BOOST_FOREACH(std::string& dir, dirs)
			{
				std::vector<std::string> subfiles = listFiles(dir, recursive, mask);
				BOOST_FOREACH(std::string& file, subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			if(isDirectory(iter->path()) == false && fileExtension(iter->path()).compare(mask) == 0)
			{
				files.push_back(iter->path().string());
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks)
{
    return listFiles(Path(path), recursive, masks);
}

std::vector<std::string> Filesystem::listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks)
{
	std::vector<std::string> files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			std::vector<std::string> dirs = listSubdirectories(path);
			BOOST_FOREACH(std::string& dir, dirs)
			{
				std::vector<std::string> subfiles = listFiles(dir, recursive, masks);
				BOOST_FOREACH(std::string& file, subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			BOOST_FOREACH(std::string mask, masks)
			{
				if(isDirectory(iter->path()) == false && fileExtension(iter->path()).compare(mask) == 0)
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
    return listSubdirectories(Path(path));
}

std::vector<std::string> Filesystem::listSubdirectories(const Path& path)
{
	std::vector<std::string> subdirs;

	if(pathExists(path) == true)
	{
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			if(isDirectory(iter->path()) == true)
			{
				subdirs.push_back(iter->path().string());
			}
		}
	}
	return subdirs;
}

bool Filesystem::isRegularFile(const std::string & path)
{
    return isRegularFile(Path(path));
}

bool Filesystem::isRegularFile(const Path & path)
{
    return is_regular_file(path);
}

bool Filesystem::isSymbolicLink(const std::string & path)
{
    return isSymbolicLink(Path(path));
}

bool Filesystem::isSymbolicLink(const Path & path)
{
    return is_symlink(path);
}

bool Filesystem::isDirectory(const std::string & path)
{
    return isDirectory(Path(path));
}

bool Filesystem::isDirectory(const Path & path)
{
    return is_directory(path);
}

bool Filesystem::pathExists(const std::string & path)
{
    return pathExists(Path(path));
}

bool Filesystem::pathExists(const Path & path)
{
    return exists(path);
}

std::string Filesystem::fileExtension(const std::string & path)
{
    return fileExtension(Path(path));
}

std::string Filesystem::fileExtension(const Path & path)
{
    return path.extension().string();
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
