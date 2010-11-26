#include <core/Filesystem.h>

void Filesystem::createDirectory(const std::string& path)
{
	if(path.empty())
	{
		return;
	}
	std::string copyPath = path;
	Filesystem::preparePath(copyPath);

	std::string token = "/";
	size_t s = 0;
	size_t e = 0;
	bool end = false;
	std::vector<std::string> vec;

	while(true)
	{
		s = 0;
		if((e = copyPath.find(token)) == copyPath.npos)
			break;
		vec.push_back(copyPath.substr(s, e));
		copyPath = copyPath.substr(e + token.size());
		s = e;
	}
	std::string dirs("");
	for(std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		dirs.append((*it)).append(token);
#if defined(__WIN32__)
		BOOL ok = ::CreateDirectory(dirs.c_str(), 0);
		if(!ok && ::GetLastError() != 183)
		{
			throw std::runtime_error("Cannot create directory.");
		}
#elif defined(__UNIX__)
		//FIX: do uzupelnienia
#endif
	}
}

void Filesystem::move(const std::string& pathOld, const std::string& pathNew)
{
	if(pathOld.empty() || pathNew.empty())
	{
		return;
	}
	std::string copyOldPath = pathOld;
	std::string copyNewPath = pathNew;
	Filesystem::changeSlashes(copyOldPath);
	Filesystem::changeSlashes(copyNewPath);

#if defined(__WIN32__)
	//przenies plik
	BOOL ok = ::MoveFileEx(copyOldPath.c_str(), copyNewPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_REPLACE_EXISTING);
	if(ok == 0)
	{
		throw std::runtime_error("Cannot move file or directory.");
	}
#elif defined(__UNIX__)
		//FIX: do uzupelnienia
#endif
}

std::vector<std::string> Filesystem::listFiles(const std::string& path, bool recursive, const std::string& mask)
{
	std::string copyPath = path;
	Filesystem::preparePath(copyPath);
	std::vector<std::string> files;
	HANDLE file;
	WIN32_FIND_DATA dataFind;
	bool moreFiles = true;
	std::string fileMask = copyPath;

	fileMask.append(mask);

	//przeszukaj podfoldery
	if(recursive)
	{
		std::vector<std::string> dirs = Filesystem::listSubdirectories(path);
		for(int i = 0; i < dirs.size(); i++)
		{
			Filesystem::preparePath(dirs[i]);
			std::vector<std::string> subfiles = Filesystem::listFiles(dirs[i], recursive, mask);
			for(int i = 0; i < subfiles.size(); i++)
			{
				files.push_back(subfiles[i]);
			}
		}
	}
#if defined(__WIN32__)
	file = ::FindFirstFile(fileMask.c_str(), &dataFind);
	while(file != INVALID_HANDLE_VALUE && moreFiles)
	{
		if(dataFind.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
		{
			std::string subfile = path;
			subfile.append(dataFind.cFileName);
			files.push_back(subfile);
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
			std::string subdir = path;
			subdir.append(dataFind.cFileName);
			subdirs.push_back(subdir);
		}
		moreFiles = (::FindNextFile(file, &dataFind) == BOOL(TRUE));
	}
#elif defined(__UNIX__)
		//FIX: do uzupelnienia
#endif
	return subdirs;
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
