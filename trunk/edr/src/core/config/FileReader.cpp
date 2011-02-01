#include "FileReader.h"

//------------------------------------------------------------------------------------------------
void FileReader::move( int t )
{
    if(line + t < 0)
    {
        line = 0;
        file.seekg(0, std::ios_base::beg);
        return;
    }

    line += t;
    if(t > 0) //jeœli jest wiêksze ni¿ zero to idziemy do przodu
    {
        for(; t > 0; t--)
        {
            char c;
            do{ file.read(&c, 1); } while(c != '\n');
        }
        return;
    }
    if(t < 0) //jeœli jest mniejsze to idzemy do ty³u
    {
        for(; t > 0; t--)
        {
            while(file.peek() != '\n') { file.seekg(-1, std::ios_base::cur); } //cofamy do pocz¹tku tej linii
            file.seekg(-1, std::ios_base::cur); //przeskakujemy znak koñca linii
            while(file.peek() != '\n') { file.seekg(-1, std::ios_base::cur); } //cofamy siê do pocz¹tku poprzedniej linii
            file.seekg(1, std::ios_base::cur); //przeskakujemy znak koñca linii
        }
    }
}

//------------------------------------------------------------------------------------------------
void FileReader::goTo( unsigned int l )
{
    move(l - line); //idzemy do tej linii
    line = l;
}

//------------------------------------------------------------------------------------------------
void FileReader::operator>>(std::string& v)
{
    if(!file.eof())
    {
        getline(file,v);
//         do
//         {
//             file.read(&c, 1);
//             if(c != '\n' || c!= '\r' || c!=0x10)
//                 v += c;
//         }
//         while(c != '\n' && !file.eof());
        line++;
    }
}