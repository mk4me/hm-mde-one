/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe³nienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <set>
#include <string>

#include <boost/filesystem.hpp>
//#include <boost/range.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class Filesystem
{
public:
    //! Typy do obs³ugi œcie¿ek
    typedef boost::filesystem::path Path;
    typedef boost::filesystem::directory_entry DirectoryEntry;
    typedef boost::filesystem::directory_iterator Iterator;
    typedef boost::filesystem::recursive_directory_iterator RecursiveIterator;

public:
	/*
	Tworzy foldery z podanej œcie¿ki.
	@param path œcie¿ka z której maj¹ byæ utworzone foldery
	*/
	static void createDirectory(const std::string& path);
    static void createDirectory(const Path& path);
	/*
	Usuwa folder z podanej œcie¿ki wraz z zawatoœci¹.
	@param path œcie¿ka do folderu który zostanie usuniêty
	*/
	static void deleteDirectory(const std::string& path);
    static void deleteDirectory(const Path& path);
	/*
	Usuwa pojedynczy plik.
	@param path œcie¿ka do pliku który zostanie usuniêty
	*/
	static void deleteFile(const std::string& path);
    static void deleteFile(const Path& path);
	/*
	Przesuwa plik lub folder wraz z zawartoœci¹ o podanej œcie¿ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik siê nadpisze, w przypadku folderów nie (trzeba to naprawiæ).
	@param pathOldFile œcie¿ka do pliku lub folderu który ma byæ przesuniêty
	@param pathNewFile œcie¿ka docelowa przesuniêcia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
    static void move(const Path& pathOld, const Path& pathNew);

	/*
	Kopiuje plik lub folder wraz z zawartoœci¹ o podanej œcie¿ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik siê nadpisze, w przypadku folderów nie (trzeba to naprawiæ).
	@param pathOldFile œcie¿ka do pliku lub folderu który ma byæ kopiowany
	@param pathNewFile œcie¿ka docelowa kopiowania
	*/
    static void copy(const std::string& pathOld, const std::string& pathNew);
    static void copy(const Path& pathOld, const Path& pathNew);

    /*
	Listuje wszystkie pliki danego folderu spe³niaj¹ce kryterium maski.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
    static std::vector<std::string> listFiles(const std::string& path, bool recursive = false);
    static std::vector<std::string> listFiles(const Path& path, bool recursive = false);
	/*
	Listuje wszystkie pliki danego folderu spe³niaj¹ce kryterium maski.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::string& mask);
    static std::vector<std::string> listFiles(const Path& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spe³niaj¹ce kryterium masek.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks);
    static std::vector<std::string> listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@return lista wszystkich podfolderów wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);
    static std::vector<std::string> listSubdirectories(const Path& path);
    /*
	Sprawdza czy podana scie¿ka wskazuje zwyk³y plik
	@param path œcie¿ka do sprawdzenia
	@return true jeœli œcie¿ka wskazuje zwyk³y plik
	*/
    static bool isRegularFile(const std::string & path);
    static bool isRegularFile(const Path & path);
    /*
	Sprawdza czy podana scie¿ka wskazuje link symboliczny
	@param path œcie¿ka do sprawdzenia
	@return true jeœli œcie¿ka wskazuje link symboliczny
	*/
    static bool isSymbolicLink(const std::string & path);
    static bool isSymbolicLink(const Path & path);
    /*
	Sprawdza czy podana scie¿ka wskazuje katalog
	@param path œcie¿ka do sprawdzenia
	@return true jeœli œcie¿ka wskazuje katalog
	*/
    static bool isDirectory(const std::string & path);
    static bool isDirectory(const Path & path);
    /*
	Sprawdza czy podana scie¿ka istnieje
	@param path œcie¿ka do sprawdzenia
	@return true jeœli œcie¿ka istnieje
	*/
    static bool pathExists(const std::string & path);
    static bool pathExists(const Path & path);
    /*
	Wyci¹ga rozszerzenie pliku ze œciezki
	@param path œcie¿ka pliku
	@return rozszerzenie pliku
	*/
    static std::string fileExtension(const std::string & path);
    static std::string fileExtension(const Path & path);

private:
    //! Prywatny konstruktor uniemo¿liwiaj¹cy tworzenie instancji typu.
    Filesystem() {}
};

// Zbiór œcie¿ek
typedef std::set<Filesystem::Path> Paths;
// Zbiór plików
typedef Paths Files;


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
