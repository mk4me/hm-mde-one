/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe³nienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

struct Filesystem
{
	/*
	Tworzy foldery z podanej œcie¿ki.
	@param path œcie¿ka z której maj¹ byæ utworzone foldery
	*/
	static void createDirectory(const std::string& path);
	/*
	Przesuwa plik lub folder wraz z zawartoœci¹ o podanej œcie¿ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik siê nadpisze, w przypadku folderów nie (trzeba to naprawiæ).
	@param pathOldFile œcie¿ka do pliku lub folderu który ma byæ przesuniêty
	@param pathNewFile œcie¿ka docelowa przesuniêcia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
	/*
	Listuje wszystkie pliki danego folderu.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@param recursive czy szukaæ plików w podfolderach
	@param mask kryteria wyszukiwania plików
	@return lista wszystkich plików wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive = false, const std::string& mask = "*");
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path œcie¿ka do folderu który ma byæ przeszukany
	@return lista wszystkich podfolderów wraz ze œcie¿k¹
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);
private:
	/*
	Unifikuje œcie¿kê ¿eby ³atwiej pracowa³o siê z Filesystem.
	@param path œcie¿ka do przerobienia na przyjazn¹ wersjê dla Filesystem
	*/
	static void preparePath(std::string& path);
	/*
	Zamienia backslashe na slashe.
	@param path œcie¿ka do przerobienia z backslashy na slashe
	*/
	static void changeSlashes(std::string& path);
};
#endif
