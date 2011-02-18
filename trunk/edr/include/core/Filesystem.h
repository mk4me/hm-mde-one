/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe³nienia.
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
	Tworzy foldery z podanej œcie¿ki.
	@param path œcie¿ka z której maj¹ byæ utworzone foldery
	*/
	static void createDirectory(const std::string& path);
	/*
	Usuwa folder z podanej œcie¿ki wraz z zawatoœci¹.
	@param path œcie¿ka do folderu który zostanie usuniêty
	*/
	static void deleteDirectory(const std::string& path);
	/*
	Usuwa pojedynczy plik.
	@param path œcie¿ka do pliku który zostanie usuniêty
	*/
	static void deleteFile(const std::string& path);
	/*
	Przesuwa plik lub folder wraz z zawartoœci¹ o podanej œcie¿ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik siê nadpisze, w przypadku folderów nie (trzeba to naprawiæ).
	@param pathOldFile œcie¿ka do pliku lub folderu który ma byæ przesuniêty
	@param pathNewFile œcie¿ka docelowa przesuniêcia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
	/*
	Listuje wszystkie pliki danego folderu spe³niaj¹ce kryterium maski.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@param mask rozszerzenie pliku, np. ".avi"
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::string& mask);
	/*
	Listuje wszystkie pliki danego folderu spe³niaj¹ce kryterium masek.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@param mask wektor z rozszerzeniami szukanych plikow
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive, const std::vector<std::string>& masks);
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@return lista wszystkich podfolderów wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);

private:
    //! Prywatny konstruktor uniemo¿liwiaj¹cy tworzenie instancji typu.
    Filesystem() {}
};

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////

#endif
