#include "stdafx.h"

using namespace std; 

// TODO: Sprawdzi� ewentualne wycieki pami�ci przy odk�adaniu i sciaganiu
// nag��wk�w na/ze stosu!!

FileChunk::FileChunk( const TCHAR *name ):
  pCurrentChunk_(NULL) // Zerujemy nag��wek
, pFileName_(NULL)
, pFile_(NULL)
{
  //TODO: Tworzenie pliku, zaczynanie kawa�ka g��wnego, i odk�adanie go na stos!
  // Kopiujemy nazw� pliku 
  size_t length = strlen(name); 
  pFileName_ = new char[length+1]; 
  strcpy_s(pFileName_, length+1, name); 

  // Tworzenie pliku
  fopen_s(&pFile_, name,"wb+"); // otwieramy plik do pisania (nowy plik)
  // Tworzymy kawa�ek g��wny
  BeginChunk(FMODEL_HEADER); 
}

FileChunk::~FileChunk(void)
{
  // Sprawdzi� czy stos pusty!
  // DEBUG****************
  if (chunkStack_.size() != 0)
  {
    // To nie powinno zaistnie�!
    int testint; 
    testint = 616; 
  }
  //**********************

  // Ko�czymy g�owny kawa�ek
  EndChunk(FMODEL_HEADER); 

  // Zamykamy plik  
  fclose(pFile_);
}

// Rozpoczyna chunka
void FileChunk::BeginChunk( short unsigned int chType )
{
  // Je�li to jest podkawa�ek, kawa�ek rodzic dajemy na stos
  if(pCurrentChunk_)
    chunkStack_.push(*pCurrentChunk_); 
  // Tworzymy nowy nag��wek
  pCurrentChunk_ = new Header;  
  // Pobieramy pozycje kawa�ka w pliku
  fgetpos(pFile_, &pCurrentChunk_->headerPos); 
  // ustawiamy typ kawa�ka
  pCurrentChunk_->chType = chType;
  // Rozmiar pocz�tkowy
  pCurrentChunk_->size = 0;
  pCurrentChunk_->size += fwrite(&pCurrentChunk_->chType, SHORT_INT_SIZE, 1, pFile_)*SHORT_INT_SIZE;	// zapisujemy typ
  pCurrentChunk_->size += fwrite(&pCurrentChunk_->size, INT_SIZE, 1, pFile_)*INT_SIZE;	// zapisujemy int

}

// Ko�czy chunk'a
void FileChunk::EndChunk( short unsigned int chType )
{
  Header *pParentChunk; 
  // Update kawa�ka
  fpos_t actual;			// pozycja kursora przed zmiana
  fgetpos(pFile_, &actual); // zapis tej pozycji
  // update header'a. zapisuje ca�kowity rozmiar kawa�ka
  fsetpos(pFile_, &pCurrentChunk_->headerPos); 
  // test 
  //fpos_t pos;
  //fgetpos(pFile,&pos);
  fwrite(&pCurrentChunk_->chType, SHORT_INT_SIZE, 1, pFile_);
  fwrite(&pCurrentChunk_->size, INT_SIZE, 1, pFile_);
  // powr�t do pozycji przed update'm header'a
  fsetpos(pFile_,&actual); 

  // DEBUG************
  // Test czy sci�gnelismy dobry kawa�ek
  if (chType != 0)
  {
    int testint = 0; 
    // Logowanie
    if (chType != pCurrentChunk_->chType)
    {
      // sciagneli�my z�y kawa�ek
      testint = 616; 
    }
    else
      testint = 123; 
  }
  //*******************

  // Sci�gamy ze stosu kawa�ek rodzica i uaktualniamy jego rozmiar
  if(chunkStack_.size() > 0)
  {
    pParentChunk = new Header; 
    *pParentChunk = chunkStack_.top(); 
    chunkStack_.pop(); 
    // Rozmiar rodzica to te� suma rozmiar�w jego podkawa�k�w
    pParentChunk->size += pCurrentChunk_->size; 
    // Koniec aktualnego kawa�ka, czyli teraz aktualnym staje si� by�y rodzic. 
    delete pCurrentChunk_; 
    pCurrentChunk_ = pParentChunk; 
  }
  else
  {
    // Sci�gamy ostatni kawa�ek ze stosu. 
    delete pCurrentChunk_; 
    pCurrentChunk_ = NULL; 
  }
}

// Zapisuje Int'a
void FileChunk::WriteInt(int data)
{
  pCurrentChunk_->size += fwrite(&data, INT_SIZE, 1, pFile_) * INT_SIZE; 
}

// Zapisuje Int'a
void FileChunk::WriteInt(size_t data)
{
  pCurrentChunk_->size += fwrite(&data, INT_SIZE, 1, pFile_) * INT_SIZE; 
}

// Zapisuje Float'a
void FileChunk::WriteFloat(float data)
{
  pCurrentChunk_->size += fwrite(&data, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE; 
}

// Zapisuje Float'a
void FileChunk::WriteDouble(double data)
{
  pCurrentChunk_->size += fwrite(&data, DOUBLE_SIZE, 1, pFile_) * DOUBLE_SIZE; 
}

// Zapisuje short int'a
void FileChunk::WriteShortInt(short int data)
{
  pCurrentChunk_->size += fwrite(&data, SHORT_INT_SIZE, 1, pFile_) * SHORT_INT_SIZE; 
}

// Zapisuje GUID
void FileChunk::WriteGUID(GUID data)
{
  pCurrentChunk_->size += fwrite(&data,1,16,pFile_);
}

// Zapisuje bajt danych
void FileChunk::WriteByte(BYTE data)
{
  pCurrentChunk_->size += fwrite(&data,1,1,pFile_)/*CHAR_SIZE*/;
}

// Zapisuje string
void FileChunk::WriteString(const TCHAR * stri)
{
  Header header;
  header.chType = FMODEL_STRING;
  size_t length = strlen(stri); 
  char *str = new char[length + 1]; 
  strcpy_s(str, length + 1, stri); 
  // Wywalamy /t z nazwy
  for(unsigned int i=0; i<length; ++i)
  {
    if(str[i] == '/t')
    {
      str[i] = ' '; 
    }
  }
  header.size = length + 1 + HEADER_SIZE;
  fwrite(&header.chType, SHORT_INT_SIZE, 1, pFile_);	// zapisujemy typ
  fwrite(&header.size, INT_SIZE, 1, pFile_);	// zapisujemy int

  fwrite(str,1,length+1,pFile_)/*CHAR_SIZE*/;

  pCurrentChunk_->size += header.size;
}

// Zapisuje okre�lon� ilo�� danych
void FileChunk::WriteData(void * data, unsigned int size)
{
  pCurrentChunk_->size += fwrite(data, 1, size, pFile_);
}

// Zapisuje wektor 3d
void FileChunk::WritePoint3(const FMVector3 &data)
{
  pCurrentChunk_->size += fwrite(&data.x, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.y, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.z, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
}

// Zapisuje wektor 2d
void FileChunk::WritePoint2(const FMVector2 &data)
{
  pCurrentChunk_->size += fwrite(&data.x, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.y, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
}

// Zapisuje kwaternion
void FileChunk::WriteQuaternion(const FMQuaternion &data)
{	     
  pCurrentChunk_->size += fwrite(&data.x, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.y, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.z, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
  pCurrentChunk_->size += fwrite(&data.w, FLOAT_SIZE, 1, pFile_) * FLOAT_SIZE;
}
