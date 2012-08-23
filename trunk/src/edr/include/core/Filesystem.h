/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe�nienia.
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
    //! Typy do obs�ugi �cie�ek
    typedef boost::filesystem::path Path;
    typedef boost::filesystem::directory_entry DirectoryEntry;
    typedef boost::filesystem::directory_iterator Iterator;
    typedef boost::filesystem::recursive_directory_iterator RecursiveIterator;

public:
	/*
	Tworzy foldery z podanej �cie�ki.
	@param path �cie�ka z kt�rej maj� by� utworzone foldery
	*/
	static void createDirectory(const std::string& path);
    static void createDirectory(const Path& path);
	/*
	Usuwa folder z podanej �cie�ki wraz z zawato�ci�.
	@param path �cie�ka do folderu kt�ry zostanie usuni�ty
	*/
	static void deleteDirectory(const std::string& path);
    static void deleteDirectory(const Path& path);
	/*
	Usuwa pojedynczy plik.
	@param path �cie�ka do pliku kt�ry zostanie usuni�ty
	*/
	static void deleteFile(const std::string& path);
    static void deleteFile(const Path& path);
	/*
	Przesuwa plik lub folder wraz z zawarto�ci� o podanej �cie�ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik si� nadpisze, w przypadku folder�w nie (trzeba to naprawi�).
	@param pathOldFile �cie�ka do pliku lub folderu kt�ry ma by� przesuni�ty
	@param pathNewFile �cie�ka docelowa przesuni�cia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
    static void move(const Path& pathOld, const Path& pathNew);

	/*
	Kopiuje plik lub folder wraz z zawarto�ci� o podanej �cie�ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik si� nadpisze, w przypadku folder�w nie (trzeba to naprawi�).
	@param pathOldFile �cie�ka do pliku lub folderu kt�ry ma by� kopiowany
	@param pathNewFile �cie�ka docelowa kopiowania
	*/
    static void copy(const std::string& pathOld, const std::string& pathNew);
    static void copy(const Path& pathOld, const Path& pathNew);

    /*
	Listuje wszystkie pliki danego folderu spe�niaj�ce kryterium maski.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
    static std::vector<std::string> listFiles(const std::string& path, bool recursive = false);
    static std::vector<std::string> listFiles(const Path& path, bool recursive = false);
	/*
	Listuje wszystkie pliki danego folderu spe�niaj�ce kryterium maski.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::string& mask);
    static std::vector<std::string> listFiles(const Path& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spe�niaj�ce kryterium masek.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks);
    static std::vector<std::string> listFiles(const Path& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@return lista wszystkich podfolder�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);
    static std::vector<std::string> listSubdirectories(const Path& path);
    /*
	Sprawdza czy podana scie�ka wskazuje zwyk�y plik
	@param path �cie�ka do sprawdzenia
	@return true je�li �cie�ka wskazuje zwyk�y plik
	*/
    static bool isRegularFile(const std::string & path);
    static bool isRegularFile(const Path & path);
    /*
	Sprawdza czy podana scie�ka wskazuje link symboliczny
	@param path �cie�ka do sprawdzenia
	@return true je�li �cie�ka wskazuje link symboliczny
	*/
    static bool isSymbolicLink(const std::string & path);
    static bool isSymbolicLink(const Path & path);
    /*
	Sprawdza czy podana scie�ka wskazuje katalog
	@param path �cie�ka do sprawdzenia
	@return true je�li �cie�ka wskazuje katalog
	*/
    static bool isDirectory(const std::string & path);
    static bool isDirectory(const Path & path);
    /*
	Sprawdza czy podana scie�ka istnieje
	@param path �cie�ka do sprawdzenia
	@return true je�li �cie�ka istnieje
	*/
    static bool pathExists(const std::string & path);
    static bool pathExists(const Path & path);
    /*
	Wyci�ga rozszerzenie pliku ze �ciezki
	@param path �cie�ka pliku
	@return rozszerzenie pliku
	*/
    static std::string fileExtension(const std::string & path);
    static std::string fileExtension(const Path & path);

private:
    //! Prywatny konstruktor uniemo�liwiaj�cy tworzenie instancji typu.
    Filesystem() {}
};

// Zbi�r �cie�ek
typedef std::set<Filesystem::Path> Paths;
// Zbi�r plik�w
typedef Paths Files;


////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
