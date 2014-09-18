#include <imucostumelib/CostumeCANopenIO.h>
#include <utils/Debug.h>
#include <vector>

using namespace imuCostume;

const bool CostumeCANopenIO::send(const int8_t seqNumber, const Frame & canOpenFrame,
	const uint8_t length, const unsigned int timeout, CostumeRawIO & costume)
{
	UTILS_ASSERT((length >= Frame::MinFrameSize && length <= Frame::MaxFrameSize), "Niepoprawny rozmowar ramki CANopen");
	const auto totalLength = 2 + length;
	std::vector<int8_t> dataToTransmit(totalLength, 0);

	// nag³ówek ramki - wymusza wys³anie reszty na CAN
	dataToTransmit[0] = 0x01;
	// mój unikalny identyfikator - dostanê go w potwierdzeniu
	dataToTransmit[1] = seqNumber;
	//przepisuje ramkê CANopen
	std::memcpy(dataToTransmit.data() + 2, canOpenFrame.structure.frame, length);

	return costume.send(dataToTransmit.data(), totalLength, timeout);
}

const bool CostumeCANopenIO::receive(const int8_t seqNumber, Frame & canOpenFrame,
	unsigned int & length, const unsigned int timeout, CostumeRawIO & costume)
{
	CostumeRawIO::Frame localData = { 0 };
	uint16_t localLength = 0;

	bool res = costume.receive(localData, localLength, timeout);

	if (res == true && ((localLength < 2) || ((localLength - 2) > Frame::MaxFrameSize) ||
		(localData[0] != CostumeRawIO::Frame::value_type(0x81)) || (localData[1] != CostumeRawIO::Frame::value_type(seqNumber)))){
		res = false;
	}

	if (res == true){
		length = localLength - 2;
		std::memcpy(canOpenFrame.structure.frame, localData.data() + 2, length);
	}

	return res;
}