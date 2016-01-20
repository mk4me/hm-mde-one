Dicom watcher, wersja 0.1, 27.10.2015

1.
Program obserwuje podany folder. Jeśli trafi tam spakowany plik zawierający strukturę DICOM, to zostanie on rozpakowany, skonwertowany i umieszczony w folderze wynikowym.

2.
Dostępne opcje:
  -h, --help                Wyświetla pomoc
  -v, --version             Wyświetla info o wersji
  -s, --status-file <file>  Plik statusu. Przechowuje on informacje o już przetworzonych plikach. Unika się wtedy wielokrotnego przetwarzania tych samych danych.
  -p, --print-on-console    Program będzie logował wszystkie operacje na konsoli. 
  -l, --log-file <file>     Informacje o wszystkich wykonanych operacjach trafią do pliku 

3.
Przykład użycia:
	dicomWatcher -s E:\watcher\status.sw -p -l E:\watcher\log.txt E:\watcher\input E:\watcher\output