#pragma once
#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class FileReader
{
public:
    FileReader(const char* name) : file(name), line(0) { };
    void init(const char* name) { file.open(name, ios_base::in | ios_base::out); line = 0; };
    void operator >>(std::string& v);

    // void operator >>(T& v) { char c; do{ file.read(&c, 1); file >> v;  } while(c != '\n'); }
    // template <typename T>
    // void operator >>(T& v) { file >> v; }

    void move(int t);

    void goTo(unsigned int l);
    void Close() {file.close(); };

    unsigned int getLine() { return line; }
    std::ifstream* getFile() { return &file; }
private:
    unsigned int line;
    std::ifstream file;
};



#endif // FILEREADER_H