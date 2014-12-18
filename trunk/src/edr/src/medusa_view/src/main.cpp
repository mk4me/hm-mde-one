#include "MdePCH.h"
#include <core/AppInitializer.h>
#include "MdeMainWindow.h"

//inline static const std::streamsize forceReadSome(std::istream * stream,
//	char * out, const std::streamsize bytesCount)
//{
//	auto state = stream->rdstate();
//	stream->read(out, bytesCount);
//	std::streamsize bytes = stream->gcount();
//	stream->clear(state | (stream->eof() == true ? std::ios_base::eofbit : 0));
//	return bytes;
//}


int main(int argc, char* argv[])
{
	/*char* buffer = new char[256 * 64];
	std::ifstream istr("C:\\Users\\Wojciech\\Documents\\PJWSTK\\MEDUSA\\MEDUSA\\tmp\\2014-09-09-S0001-T0021.test-student.xml");
	int offset = 256*64;
	offset = forceReadSome(&istr, (char*)buffer, offset);*/
	return core::AppInitializer::runApp<MdeMainWindow>(argc, argv, "MEDUSA");
}
