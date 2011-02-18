/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe�nienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////



class Filesystem
{
public:
	/*
	Tworzy foldery z podanej �cie�ki.
	@param path �cie�ka z kt�rej maj� by� utworzone foldery
	*/
	static void createDirectory(const std::string& path);
	/*
	Usuwa folder z podanej �cie�ki wraz z zawato�ci�.
	@param path �cie�ka do folderu kt�ry zostanie usuni�ty
	*/
	static void deleteDirectory(const std::string& path);
	/*
	Usuwa pojedynczy plik.
	@param path �cie�ka do pliku kt�ry zostanie usuni�ty
	*/
	static void deleteFile(const std::string& path);
	/*
	Przesuwa plik lub folder wraz z zawarto�ci� o podanej �cie�ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik si� nadpisze, w przypadku folder�w nie (trzeba to naprawi�).
	@param pathOldFile �cie�ka do pliku lub folderu kt�ry ma by� przesuni�ty
	@param pathNewFile �cie�ka docelowa przesuni�cia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
	/*
	Listuje wszystkie pliki danego folderu spe�niaj�ce kryterium maski.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spe�niaj�ce kryterium masek.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@return lista wszystkich podfolder�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);

private:
    //! Prywatny konstruktor uniemo�liwiaj�cy tworzenie instancji typu.
    Filesystem() {}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
