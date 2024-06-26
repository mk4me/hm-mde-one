/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupełnienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <corelib/Export.h>
#include <vector>
#include <set>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/function.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class CORELIB_EXPORT Filesystem
{
public:
    //! Typy do obsługi ścieżek
    typedef boost::filesystem::path Path;
    typedef boost::filesystem::directory_entry DirectoryEntry;
    typedef boost::filesystem::directory_iterator Iterator;
    typedef boost::filesystem::recursive_directory_iterator RecursiveIterator;

public:
	/*
	Tworzy foldery z podanej ścieżki.
	@param path ścieżka z której mają być utworzone foldery
	*/
	static void createDirectory(const std::string& path);
    static void createDirectory(const Path& path);
	/*
	Usuwa folder z podanej ścieżki wraz z zawatością.
	@param path ścieżka do folderu który zostanie usunięty
	*/
	static void deleteDirectory(const std::string& path);
    static void deleteDirectory(const Path& path);
	/*
	Usuwa pojedynczy plik.
	@param path ścieżka do pliku który zostanie usunięty
	*/
	static void deleteFile(const std::string& path);
    static void deleteFile(const Path& path);
	/*
	Przesuwa plik lub folder wraz z zawartością o podanej ścieżce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik się nadpisze, w przypadku folderów nie (trzeba to naprawić).
	@param pathOldFile ścieżka do pliku lub folderu który ma być przesunięty
	@param pathNewFile ścieżka docelowa przesunięcia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
    static void move(const Path& pathOld, const Path& pathNew);

	/*
	Kopiuje plik lub folder wraz z zawartością o podanej ścieżce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik się nadpisze, w przypadku folderów nie (trzeba to naprawić).
	@param pathOldFile ścieżka do pliku lub folderu który ma być kopiowany
	@param pathNewFile ścieżka docelowa kopiowania
	*/
    static void copy(const std::string& pathOld, const std::string& pathNew);
    static void copy(const Path& pathOld, const Path& pathNew);

    /*
	Listuje wszystkie pliki danego folderu spełniające kryterium maski.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@return lista wszystkich plików wraz ze ścieżką
	*/
    static std::vector<std::string> listFiles(const std::string& path, bool recursive = false);
    static std::vector<Path> listFiles(const Path& path, bool recursive = false);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium maski.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plików wraz ze ścieżką
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::string& mask);
    static std::vector<Path> listFiles(const Path& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium masek.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plików wraz ze ścieżką
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks);
    static std::vector<Path> listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie pliki danego folderu spełniające kryterium przekazywanego filtru.
	@param path ścieżka do folderu który ma być przeszukany
	@param recursive czy szukać plików w podfolderach
	@param filter funkcja testująca przeszukiwane pliki
	@return lista wszystkich plików wraz ze ścieżką
	*/
	static std::vector<std::string> listFilteredFiles(const std::string& path, bool recursive, boost::function<bool(const std::string&)> filter);
	static std::vector<Path> listFilteredFiles(const Path& path, bool recursive, boost::function<bool(const Path&)> filter);          
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path ścieżka do folderu który ma być przeszukany
	@return lista wszystkich podfolderów wraz ze ścieżką
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);
    static std::vector<Path> listSubdirectories(const Path& path);
    /*
	Sprawdza czy podana scieżka wskazuje zwykły plik
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje zwykły plik
	*/
    static bool isRegularFile(const std::string & path);
    static bool isRegularFile(const Path & path);
    /*
	Sprawdza czy podana scieżka wskazuje link symboliczny
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje link symboliczny
	*/
    static bool isSymbolicLink(const std::string & path);
    static bool isSymbolicLink(const Path & path);
    /*
	Sprawdza czy podana scieżka wskazuje katalog
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka wskazuje katalog
	*/
    static bool isDirectory(const std::string & path);
    static bool isDirectory(const Path & path);
    /*
	Sprawdza czy podana scieżka istnieje
	@param path ścieżka do sprawdzenia
	@return true jeśli ścieżka istnieje
	*/
    static bool pathExists(const std::string & path);
    static bool pathExists(const Path & path);
    /*
	Wyciąga rozszerzenie pliku ze ściezki
	@param path ścieżka pliku
	@return rozszerzenie pliku
	*/
    static std::string fileExtension(const std::string & path);
    static std::string fileExtension(const Path & path);

private:
    //! Prywatny konstruktor uniemożliwiający tworzenie instancji typu.
    Filesystem() {}
};

// Zbiór ścieżek
typedef std::set<Filesystem::Path> Paths;
// Zbiór plików
typedef Paths Files;


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
