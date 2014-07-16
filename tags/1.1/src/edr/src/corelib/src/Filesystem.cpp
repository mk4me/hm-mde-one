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
    auto files = listFiles(Path(path), recursive);

	std::vector<std::string> ret;

	for(auto it = files.begin(); it != files.end(); ++it){
		ret.push_back((*it).string());
	}

	return ret;
}

std::vector<Filesystem::Path> Filesystem::listFiles(const Path& path, bool recursive)
{
    std::vector<Path> files;

    if(pathExists(path) == true)
    {
        if(recursive)
        {
            std::vector<Path> dirs = listSubdirectories(path);
            BOOST_FOREACH(Path& dir, dirs)
            {
                std::vector<Path> subfiles = listFiles(dir, recursive);
                BOOST_FOREACH(Path& file, subfiles)
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
                files.push_back(iter->path());
            }
        }
    }
    return files;
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::string& mask)
{
    auto files = listFiles(Path(path), recursive, mask);

	std::vector<std::string> ret;

	for(auto it = files.begin(); it != files.end(); ++it){
		ret.push_back((*it).string());
	}

	return ret;
}

std::vector<Filesystem::Path> Filesystem::listFiles(const Path& path, bool recursive, const std::string& mask)
{
	std::vector<Path> files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			std::vector<Path> dirs = listSubdirectories(path);
			BOOST_FOREACH(Path& dir, dirs)
			{
				std::vector<Path> subfiles = listFiles(dir, recursive, mask);
				BOOST_FOREACH(Path& file, subfiles)
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
				files.push_back(iter->path());
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks)
{
    auto files = listFiles(Path(path), recursive, masks);

	std::vector<std::string> ret;

	for(auto it = files.begin(); it != files.end(); ++it){
		ret.push_back((*it).string());
	}

	return ret;
}

std::vector<Filesystem::Path> Filesystem::listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks)
{
	std::vector<Path> files;

	if(pathExists(path) == true)
	{
		if(recursive)
		{
			std::vector<Path> dirs = listSubdirectories(path);
			BOOST_FOREACH(Path& dir, dirs)
			{
				std::vector<Path> subfiles = listFiles(dir, recursive, masks);
				BOOST_FOREACH(Path& file, subfiles)
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
					files.push_back(iter->path());
				}
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listFilteredFiles(const std::string& path, bool recursive, boost::function<bool(const std::string&)> filter)
{
	auto _filter = [&](const Path& p) {
		return filter(p.string());
	};

	auto pathFiles = listFilteredFiles(Path(path), recursive, _filter);
	std::vector<std::string> files;
	for (auto it = pathFiles.begin(); it != pathFiles.end(); ++it) {
		files.push_back(it->string());
	}

	return files;
}

std::vector<Filesystem::Path> Filesystem::listFilteredFiles(const Path& path, bool recursive, boost::function<bool(const Filesystem::Path&)> filter)
{

	std::vector<Path> files;

	if (pathExists(path) == true)
	{
		if (recursive)
		{
			std::vector<Path> dirs = listSubdirectories(path);
			BOOST_FOREACH(Path& dir, dirs)
			{
				std::vector<Path> subfiles = listFilteredFiles(dir, recursive, filter);
				BOOST_FOREACH(Path& file, subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for (Iterator iter(path); iter != end; ++iter)
		{
			if (isDirectory(iter->path()) == false && filter(iter->path()))	{
				files.push_back(iter->path());
			}
		}
	}
	return files;
}

std::vector<std::string> Filesystem::listSubdirectories(const std::string& path)
{
    auto subdirs = listSubdirectories(Path(path));

	std::vector<std::string> ret;

	for(auto it = subdirs.begin(); it != subdirs.end(); ++it){
		ret.push_back((*it).string());
	}

	return ret;
}

std::vector<Filesystem::Path> Filesystem::listSubdirectories(const Path& path)
{
	std::vector<Path> subdirs;

	if(pathExists(path) == true)
	{
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			if(isDirectory(iter->path()) == true)
			{
				subdirs.push_back(iter->path());
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
