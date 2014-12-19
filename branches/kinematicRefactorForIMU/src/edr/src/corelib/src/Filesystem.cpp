#include "CoreLibPCH.h"
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
            for(const auto & dir : dirs)
            {
                std::vector<Path> subfiles = listFiles(dir, recursive);
                for(const auto & file : subfiles)
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
			for (const auto & dir : dirs)
			{
				std::vector<Path> subfiles = listFiles(dir, recursive, mask);
				for (const auto & file : subfiles)
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
			for (const auto & dir : dirs)
			{
				std::vector<Path> subfiles = listFiles(dir, recursive, masks);
				for (const auto & file : subfiles)
				{
					files.push_back(file);
				}
			}
		}
		Iterator end;
		for(Iterator iter(path) ; iter != end ; ++iter)
		{
			for(const auto & mask : masks)
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

const Filesystem::size_t Filesystem::availableSpace(const std::string & path)
{
	return availableSpace(Path(path));
}

const Filesystem::size_t Filesystem::availableSpace(const Path & path)
{
	Filesystem::size_t ret = 0;

	try{
		if (isRegularFile(path) == true){
			auto s = boost::filesystem::space(path.parent_path());
			ret = s.available;
		}
		else{
			auto s = boost::filesystem::space(path);
			ret = s.available;
		}
		
	}
	catch (...){

	}

	return ret;
}

const Filesystem::size_t Filesystem::capacity(const std::string & path)
{
	return capacity(Path(path));
}

const Filesystem::size_t Filesystem::capacity(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{
		auto s = boost::filesystem::space(path);
		ret = s.capacity;
	}
	catch (...){

	}

	return ret;
}

const Filesystem::size_t Filesystem::freeSpace(const std::string & path)
{
	return freeSpace(Path(path));
}

const Filesystem::size_t Filesystem::freeSpace(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{
		auto s = boost::filesystem::space(path);
		ret = s.free;
	}
	catch (...){

	}

	return ret;
}

const Filesystem::size_t Filesystem::size(const std::string & path)
{
	return size(Path(path));
}

const Filesystem::size_t Filesystem::size(const Path & path)
{
	Filesystem::size_t ret = -1;

	try{

		if (isRegularFile(path) == true){
			ret = boost::filesystem::file_size(path);
		}
		else if (isDirectory(path) == true){
			ret = 0;
			for (boost::filesystem::recursive_directory_iterator it(path);
				it != boost::filesystem::recursive_directory_iterator();
				++it)
			{
				if (isDirectory(*it) == false){
					ret += boost::filesystem::file_size(*it);
				}					
			}
		}

	}
	catch (...){

	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////
