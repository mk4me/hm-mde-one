#include <core/Filesystem.h>
#include <boost/filesystem.hpp>

using namespace boost;

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

void Filesystem::move(const std::string& pathOld, const std::string& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}
	filesystem::path oldPath(pathOld);
	filesystem::path newPath(pathNew);

	rename(oldPath, newPath);
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::string& mask)
{
	std::vector<std::string> files;

	HANDLE file;
	WIN32_FIND_DATA dataFind;
	bool moreFiles = true;

	//przeszukaj podfoldery
	if(recursive)
	{
		std::vector<std::string> dirs = Filesystem::listSubdirectories(path);
		for(std::vector<std::string>::iterator i = dirs.begin(); i != dirs.end(); ++i)
		{
			std::vector<std::string> subfiles = Filesystem::listFiles((*i), recursive, mask);
			for(std::vector<std::string>::iterator it = subfiles.begin(); it != subfiles.end(); ++it)
			{
				files.push_back((*it));
			}
		}
	}
#if defined(__WIN32__)
	file = ::FindFirstFile(Filesystem::append(path, mask).c_str(), &dataFind);
	while(file != INVALID_HANDLE_VALUE && moreFiles)
	{
		if(dataFind.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			files.push_back(Filesystem::append(path, dataFind.cFileName));
		}
		moreFiles = (::FindNextFile(file, &dataFind) == BOOL(TRUE));
	}
#elif defined(__UNIX__)
		//FIX: do uzupelnienia
#endif
	return files;
}

std::vector<std::string> Filesystem::listSubdirectories(const std::string& path)
{
	std::string copyPath = path;
	Filesystem::preparePath(copyPath);
	std::vector<std::string> subdirs;
	HANDLE file;
	WIN32_FIND_DATA dataFind;
	bool moreFiles = true;
	copyPath.append("*");

#if defined(__WIN32__)
	file = ::FindFirstFile(copyPath.c_str(), &dataFind);
	while(file != INVALID_HANDLE_VALUE && moreFiles)
	{
		if(dataFind.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && dataFind.cFileName[0] != '.')
		{
			subdirs.push_back(Filesystem::append(path, dataFind.cFileName));
		}
		moreFiles = (::FindNextFile(file, &dataFind) == BOOL(TRUE));
	}
#elif defined(__UNIX__)
		//FIX: do uzupelnienia
#endif
	return subdirs;
}

std::string Filesystem::append(const std::string& path, const std::string& path2)
{
	if(path.empty() && path2.empty())
	{
		throw std::exception("Empty strings in use.");
	}
	std::string appended = path;
	Filesystem::preparePath(appended);
	appended.append(path2);
	return appended;
}

void Filesystem::preparePath(std::string& path)
{
	if(path.empty())
	{
		return;
	}
	std::string token = "/";
	if(path[path.size() -1] != '/')
	{
		path.append(token);
	}
	Filesystem::changeSlashes(path);
}

void Filesystem::changeSlashes(std::string& path)
{
	if(path.empty())
	{
		return;
	}
	//dostosuj sciezke
	size_t position = path.find_first_of("\\");
	while (position != path.npos)
	{
		path[position] = '/';
		position = path.find_first_of("\\", position+1);
	}
}
