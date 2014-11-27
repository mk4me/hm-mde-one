#include <imucostumelib/ProtocolSendBufferHelper.h>

using namespace imuCostume;


ProtocolSendBufferHelper::ProtocolSendBufferHelper()
{

}

ProtocolSendBufferHelper::ProtocolSendBufferHelper(const ProtocolSendBufferHelper & Other)
	: frames(Other.frames)
{

}

ProtocolSendBufferHelper::ProtocolSendBufferHelper(ProtocolSendBufferHelper && Other)
	: frames(std::move(Other.frames))
{

}

void ProtocolSendBufferHelper::add(const CANopenFrame & frame)
{
	frames.push_back(frame);
}

void ProtocolSendBufferHelper::add(const CANopenFrame & frame, const uint8_t length)
{
	CANopenFrame f(frame);
	updateFrameLength(f, length);
	frames.push_back(f);
}

const bool ProtocolSendBufferHelper::empty() const
{
	return frames.empty();
}

ProtocolSendBufferHelper::Buffer ProtocolSendBufferHelper::createBuffer()
{
	Buffer ret;
	ret.length = 0;
	std::array<uint8_t, CostumeRawIO::MaxDataSize - 1 - 2> localBuffer;

	while (frames.begin() != frames.end()){
		const auto l = 2 + ((frames.front().buffer[1] & 0xF0) >> 4);
		if (l + ret.length > localBuffer.max_size()){
			break;
		}

		std::memcpy(localBuffer.data() + ret.length, frames.front().buffer.data(), l);
		ret.length += l;
		frames.pop_front();
	}

	if (ret.length > 0){
		ret.buffer.reset(new uint8_t[ret.length], utils::array_deleter<uint8_t>());
		std::memcpy(ret.buffer.get(), localBuffer.data(), ret.length);
	}

	return std::move(ret);
}

ProtocolSendBufferHelper::Buffer::Buffer(Buffer && Other)
	: buffer(std::move(Other.buffer)), length(Other.length)
{
	Other.length = 0;
}

void ProtocolSendBufferHelper::updateFrameLength(CANopenFrame & frame, const uint8_t length)
{
	frame.buffer[1] |= ((length - 2) << 4);
}