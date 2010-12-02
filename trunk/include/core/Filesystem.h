/**
@author Marek Daniluk
@brief Klasa Filesystem z podstawowymi operacjami na plikach. Do uzupe�nienia.
*/
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <vector>
#include <string>

struct Filesystem
{
	/*
	Tworzy foldery z podanej �cie�ki.
	@param path �cie�ka z kt�rej maj� by� utworzone foldery
	*/
	static void createDirectory(const std::string& path);
	/*
	Przesuwa plik lub folder wraz z zawarto�ci� o podanej �cie�ce. W przypadku istnienia
	w miejscu docelowym pliku o tej samej nazwie plik si� nadpisze, w przypadku folder�w nie (trzeba to naprawi�).
	@param pathOldFile �cie�ka do pliku lub folderu kt�ry ma by� przesuni�ty
	@param pathNewFile �cie�ka docelowa przesuni�cia
	*/
	static void move(const std::string& pathOld, const std::string& pathNew);
	/*
	Listuje wszystkie pliki danego folderu.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@param recursive czy szuka� plik�w w podfolderach
	@param mask kryteria wyszukiwania plik�w
	@return lista wszystkich plik�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listFiles(const std::string& path, bool recursive = false, const std::string& mask = "*");
	/*
	Listuje wszystkie podfoldery danego folderu.
	@param path �cie�ka do folderu kt�ry ma by� przeszukany
	@return lista wszystkich podfolder�w wraz ze �cie�k�
	*/
	static std::vector<std::string> listSubdirectories(const std::string& path);
	/*
	Do��cza �cie�k� path2 do �cie�ki path.
	@param path �cie�ka do folderu do kt�rego ma by� do��czony path2
	@param path2 �cie�ka do folderu lub pliku kt�ry ma by� do��czony do path
	@return �cie�ka utworzona z do��czenia path2 do path
	*/
	static std::string append(const std::string& path, const std::string& path2);
private:
	/*
	Unifikuje �cie�k� �eby �atwiej pracowa�o si� z Filesystem.
	@param path �cie�ka do przerobienia na przyjazn� wersj� dla Filesystem
	*/
	static void preparePath(std::string& path);
	/*
	Zamienia backslashe na slashe.
	@param path �cie�ka do przerobienia z backslashy na slashe
	*/
	static void changeSlashes(std::string& path);
};
#endif
