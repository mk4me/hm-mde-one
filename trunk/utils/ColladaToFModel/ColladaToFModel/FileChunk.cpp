#include "stdafx.h"

using namespace std; 

// TODO: Sprawdziæ ewentualne wycieki pamiêci przy odk³adaniu i sciaganiu
// nag³ówków na/ze stosu!!

FileChunk::FileChunk( const TCHAR *name ):
  pCurrentChunk_(NULL) // Zerujemy nag³ówek
, pFileName_(NULL)
, pFile_(NULL)
{
  //TODO: Tworzenie pliku, zaczynanie kawa³ka g³ównego, i odk³adanie go na stos!
  // Kopiujemy nazwê pliku 
  size_t length = strlen(name); 
  pFileName_ = new char[length+1]; 
  strcpy_s(pFileName_, length+1, name); 

  // Tworzenie pliku
  fopen_s(&pFile_, name,"wb+"); // otwieramy plik do pisania (nowy plik)
  // Tworzymy kawa³ek g³ówny
  BeginChunk(FMODEL_HEADER); 
}

FileChunk::~FileChunk(void)
{
  // Sprawdziæ czy stos pusty!
  // DEBUG****************
  if (chunkStack_.size() != 0)
  {
    // To nie powinno zaistnieæ!
    int testint; 
    testint = 616; 
  }
  //**********************

  // Koñczymy g³owny kawa³ek
  EndChunk(FMODEL_HEADER); 

  // Zamykamy plik  
  fclose(pFile_);
}

// Rozpoczyna chunka
void FileChunk::BeginChunk( short unsigned int chType )
{
  // Jeœli to jest podkawa³ek, kawa³ek rodzic dajemy na stos
  if(pCurrentChunk_)
    chunkStack_.push(*pCurrentChunk_); 
  // Tworzymy nowy nag³ówek
  pCurrentChunk_ = new Header;  
  // Pobieramy pozycje kawa³ka w pliku
  fgetpos(pFile_, &pCurrentChunk_->headerPos); 
  // ustawiamy typ kawa³ka
  pCurrentChunk_->chType = chType;
  // Rozmiar pocz¹tkowy
  pCurrentChunk_->size = 0;
  pCurrentChunk_->size += fwrite(&pCurrentChunk_->chType, SHORT_INT_SIZE, 1, pFile_)*SHORT_INT_SIZE;	// zapisujemy typ
  pCurrentChunk_->size += fwrite(&pCurrentChunk_->size, INT_SIZE, 1, pFile_)*INT_SIZE;	// zapisujemy int

}

// Koñczy chunk'a
void FileChunk::EndChunk( short unsigned int chType )
{
  Header *pParentChunk; 
  // Update kawa³ka
  fpos_t actual;			// pozycja kursora przed zmiana
  fgetpos(pFile_, &actual); // zapis tej pozycji
  // update header'a. zapisuje ca³kowity rozmiar kawa³ka
  fsetpos(pFile_, &pCurrentChunk_->headerPos); 
  // test 
  //fpos_t pos;
  //fgetpos(pFile,&pos);
  fwrite(&pCurrentChunk_->chType, SHORT_INT_SIZE, 1, pFile_);
  fwrite(&pCurrentChunk_->size, INT_SIZE, 1, pFile_);
  // powrót do pozycji przed update'm header'a
  fsetpos(pFile_,&actual); 

  // DEBUG************
  // Test czy sci¹gnelismy dobry kawa³ek
  if (chType != 0)
  {
    int testint = 0; 
    // Logowanie
    if (chType != pCurrentChunk_->chType)
    {
      // sciagneliœmy z³y kawa³ek
      testint = 616; 
    }
    else
      testint = 123; 
  }
  //*******************

  // Sci¹gamy ze stosu kawa³ek rodzica i uaktualniamy jego rozmiar
  if(chunkStack_.size() > 0)
  {
    pParentChunk = new Header; 
    *pParentChunk = chunkStack_.top(); 
    chunkStack_.pop(); 
    // Rozmiar rodzica to te¿ suma rozmiarów jego podkawa³ków
    pParentChunk->size += pCurrentChunk_->size; 
    // Koniec aktualnego kawa³ka, czyli teraz aktualnym staje siê by³y rodzic. 
    delete pCurrentChunk_; 
    pCurrentChunk_ = pParentChunk; 
  }
  else
  {
    // Sci¹gamy ostatni kawa³ek ze stosu. 
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

// Zapisuje okreœlon¹ iloœæ danych
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
