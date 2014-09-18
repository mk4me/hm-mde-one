#include <imucostumelib/CANopenSensor.h>
#include <utils/Utils.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CostumeCANopenIO.h>
#include <boost/detail/endian.hpp>

using namespace imuCostume;

static const unsigned int MaxRetriesCount = 100;

class SDOMessageFilter
{
public:
	SDOMessageFilter(const int16_t mask = 0xFFFF)
		: mask_(mask)
	{
	}

	~SDOMessageFilter()
	{
	}

	const bool operator()(const CostumeCANopenIO::Frame & message, const unsigned int length) const
	{
		if (length != 8){
			return false;
		}

		if (message.structure.messageID.id != mask_){
			return false;
		}

		return true;
	}

private:
	const int16_t mask_;
};

CANopenSensor::Buffer::Buffer() : data_(0)
{
}

CANopenSensor::Buffer::~Buffer()
{
}

const uint32_t CANopenSensor::Buffer::data() const
{
	return data_;
}

class CANopenSensor::CANopenSensorImpl
{
private:

	typedef boost::array<int8_t, 8> FrameData;

public:
	CANopenSensorImpl(CostumeRawIO * costume, const int8_t id, const unsigned int timeout)
		: id(id), costume(costume), seqNumber(id), timeout_(timeout)
	{
	}

	~CANopenSensorImpl()
	{
	}

	//LSS
	/*
	const CANopenSensor::OperationStatus select(const bool select)
	{
	}

	static const CANopenSensor::OperationStatus select(const bool select, const std::set<int8_t> & ids, Costume * costume)
	{
	}

	static const CANopenSensor::OperationStatus selectAll(const bool select, Costume * costume)
	{
	}

	const CANopenSensor::OperationStatus setID(const int8_t id)
	{
	}

	const CANopenSensor::OperationStatus setSpeedLevel(const CANopenSensor::SpeedLevel speedLevel)
	{
	}
	*/

	const CANopenSensor::OperationStatus saveConfiguration(CANopenSensor::ErrorCode & errorCode)
	{
		return write(CANopenSensor::STORE_PARAMETERS, CANopenSensor::Size4B, CANopenSensor::formatData(int32_t(0x65766173)), errorCode, 1);
	}

	//NMT

	const CANopenSensor::OperationStatus start()
	{
		return trySend(costume, seqNumber, formatNMTFrame(0x01, id), 4, timeout_, 50);
	}

	const CANopenSensor::OperationStatus stop()
	{
		return trySend(costume, seqNumber, formatNMTFrame(0x02, id), 4, timeout_, 50);
	}

	const CANopenSensor::OperationStatus prepare()
	{
		return trySend(costume, seqNumber, formatNMTFrame(0x80, id), 4, timeout_, 50);
	}

	const CANopenSensor::OperationStatus reset()
	{
		return trySend(costume, seqNumber, formatNMTFrame(0x81, id), 4, timeout_, 50);
	}

	static const CANopenSensor::OperationStatus resetAllSensors(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries)
	{
		return trySend(costume, (int8_t)costume, formatNMTFrame(0x81), 4, timeout, maxRetries);
	}

	static const CANopenSensor::OperationStatus synchronize(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries)
	{
		CostumeCANopenIO::Frame frame = { 0 };
		frame.structure.messageID.bytes[0] = 0x80;
		frame.structure.messageID.bytes[1] = 0;
		return trySend(costume, (int8_t)costume, frame, 2, timeout, maxRetries);
	}

	const CANopenSensor::OperationStatus resetCommunication()
	{
		return trySend(costume, seqNumber, formatNMTFrame(0x82, id), 4, timeout_, 50);
	}

	//SDO
	const CANopenSensor::OperationStatus read(const int16_t dictID, CANopenSensor::IODataSize & dataSize, Buffer & out, CANopenSensor::ErrorCode & errorCode, const int8_t dictSubID = 0) const
	{
		CANopenSensor::OperationStatus ret = CANopenSensor::OPERATION_OK;

		CostumeCANopenIO::Frame frame = { 0 };
		frame.structure.frame[0] = id;	//id czujnika
		//offset dla identyfikatorów urz¹dzeñ
		frame.structure.frame[1] = 0x60;	//0x60;
		frame.structure.frame[2] = 0x40;

		CostumeCANopenIO::Frame::MessageID messageID;

		messageID.id = dictID;

		frame.structure.frame[3] = messageID.bytes[1];
		frame.structure.frame[4] = messageID.bytes[0];
		frame.structure.frame[5] = dictSubID;

		ret = trySend(costume, seqNumber, frame, 10, timeout_, 50);

		if (ret == CANopenSensor::OPERATION_OK){
			SDOMessageFilter mf(0x580 | id);
			CostumeCANopenIO::Frame response = { 0 };
			unsigned int length = 0;
			ret = tryReceive(costume, seqNumber, response, length, timeout_, 50, mf);

			if (ret == CANopenSensor::OPERATION_OK){
				int32_t ret = 0;
				std::memcmp(response.structure.data + 4, &ret, 4);
				ret = utils::EndianSwap(ret);

				if (response.structure.data[0] & 0x80){
					errorCode = (CANopenSensor::ErrorCode)(ret);
				}
				else{
					out = CANopenSensor::formatData(ret);
				}
			}
		}

		++seqNumber;

		return ret;
	}

	const CANopenSensor::OperationStatus write(const int16_t dictID, const CANopenSensor::IODataSize dataSize, const Buffer in, CANopenSensor::ErrorCode & errorCode, const int8_t dictSubID)
	{
		CANopenSensor::OperationStatus ret = CANopenSensor::OPERATION_OK;

		CostumeCANopenIO::Frame frame = { 0 };
		frame.structure.frame[0] = id;	//id czujnika
		//offset dla identyfikatorów urz¹dzeñ
		frame.structure.frame[1] = 0x60;	//0x60;

		switch (dataSize)
		{
		case CANopenSensor::Size1B:
			frame.structure.data[0] = 0x2F;
			break;

		case CANopenSensor::Size2B:
			frame.structure.data[0] = 0x2B;
			break;

		case CANopenSensor::Size4B:
			frame.structure.data[0] = 0x23;
			break;
		}

		CostumeCANopenIO::Frame::MessageID messageID;

		messageID.id = dictID;

		frame.structure.data[1] = messageID.bytes[1];
		frame.structure.data[2] = messageID.bytes[0];
		frame.structure.data[3] = dictSubID;

		{
			const auto d = in.data();
			std::memcpy(frame.structure.data + 4, &d, 4);
		}

		ret = trySend(costume, seqNumber, frame, 10, timeout_, 50);

		if (ret == CANopenSensor::OPERATION_OK){
			SDOMessageFilter mf(0x580 | id);
			CostumeCANopenIO::Frame response = { 0 };
			unsigned int length = 0;
			ret = tryReceive(costume, seqNumber, response, length, timeout_, 1000, mf);

			if (ret == CANopenSensor::OPERATION_OK){
				if (response.structure.data[0] & 0x60){
					errorCode = CANopenSensor::NO_ERROR;
				}
				else{
					int32_t ret = 0;
					std::memcmp(response.structure.data + 4, &ret, 4);
					ret = utils::EndianSwap(ret);
					errorCode = (CANopenSensor::ErrorCode)(ret);
				}
			}
		}

		++seqNumber;

		return ret;
	}

private:

	static const CostumeCANopenIO::Frame formatNMTFrame(const int8_t messageID, const int8_t nodeID = 0)
	{
		CostumeCANopenIO::Frame ret = { 0 };
		ret.structure.messageID.id = 0;
		ret.structure.data[0] = messageID;
		ret.structure.data[1] = nodeID;

		return ret;
	}

	static const CANopenSensor::OperationStatus trySend(CostumeRawIO * costume,
		const int8_t seqNumber, const CostumeCANopenIO::Frame & data,
		const unsigned int length, const unsigned int timeout, const unsigned int maxRetries)
	{
		CANopenSensor::OperationStatus ret = CANopenSensor::OPERATION_SEND_FAILURE;

		unsigned int triesCounter = 0;
		bool result = false;
		do
		{
			result = CostumeCANopenIO::send(seqNumber, data, length, timeout, *costume);
		} while (result == false && triesCounter++ < maxRetries);

		if (result == true){
			ret = CANopenSensor::OPERATION_OK;
		}

		return ret;
	}

	static const CANopenSensor::OperationStatus tryReceive(CostumeRawIO * costume,
		const int8_t seqNumber, CostumeCANopenIO::Frame & data,
		unsigned int & length, const unsigned int timeout,
		const unsigned int maxRetries)
	{
		CANopenSensor::OperationStatus ret = CANopenSensor::OPERATION_RECEIVE_FAILURE;

		CostumeCANopenIO::Frame localData = { 0 };
		unsigned int localLength = 0;
		unsigned int triesCounter = 0;
		bool result = false;
		do
		{
			result = CostumeCANopenIO::receive(seqNumber, localData, localLength, timeout, *costume);
		} while (result == false && triesCounter++ < maxRetries);

		if (result == true){
			data = localData;
			length = localLength;
			ret = CANopenSensor::OPERATION_OK;
		}

		return ret;
	}

	template<typename T>
	static const CANopenSensor::OperationStatus tryReceive(CostumeRawIO * costume,
		const int8_t seqNumber, CostumeCANopenIO::Frame & data,
		const unsigned & length, const unsigned int timeout,
		const unsigned int maxRetries, const T & filter)
	{
		CANopenSensor::OperationStatus ret = CANopenSensor::OPERATION_RECEIVE_FAILURE;

		CostumeCANopenIO::Frame localData = { 0 };
		unsigned int localLength = 0;
		unsigned int triesCounter = 0;
		bool result = false;
		do
		{
			result = CostumeCANopenIO::receive(seqNumber, localData, localLength, timeout, *costume);
			if (result == true){
				result = filter(localData, localLength);
			}
		} while (result == false && triesCounter++ < maxRetries);

		if (result == true){
			ret = CANopenSensor::OPERATION_OK;
		}

		return ret;
	}

private:
	//! Identyfikator czujnika IMU
	int8_t id;
	//! Kostium na którym mamy czujnik
	CostumeRawIO * costume;
	//! Numer sekwencyjny dla zapytañ
	mutable int8_t seqNumber;
	//! Timeout dla operacji
	const unsigned int timeout_;
};

CANopenSensor::CANopenSensor(CostumeRawIO * costume, const int8_t id, const unsigned int timeout)
	: impl(new CANopenSensorImpl(costume, id, timeout))
{
}

CANopenSensor::~CANopenSensor()
{
}

//LSS
/*
const CANopenSensor::OperationStatus CANopenSensor::select(const bool select)
{
}

const CANopenSensor::OperationStatus CANopenSensor::setID(const char id)
{
}

const CANopenSensor::OperationStatus CANopenSensor::setSpeedLevel(const SpeedLevel speedLevel)
{
}
*/

const CANopenSensor::OperationStatus CANopenSensor::saveConfiguration(ErrorCode & errorCode)
{
	return impl->saveConfiguration(errorCode);
}

//NMT

const CANopenSensor::OperationStatus CANopenSensor::start()
{
	return impl->start();
}

const CANopenSensor::OperationStatus CANopenSensor::stop()
{
	return impl->stop();
}

const CANopenSensor::OperationStatus CANopenSensor::prepare()
{
	return impl->prepare();
}

const CANopenSensor::OperationStatus CANopenSensor::reset()
{
	return impl->reset();
}

const CANopenSensor::OperationStatus CANopenSensor::resetAllSensors(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries)
{
	return CANopenSensor::CANopenSensorImpl::resetAllSensors(costume, timeout, maxRetries);
}

const CANopenSensor::OperationStatus CANopenSensor::synchronize(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries)
{
	return CANopenSensor::CANopenSensorImpl::synchronize(costume, timeout, maxRetries);
}

const CANopenSensor::OperationStatus CANopenSensor::resetCommunication()
{
	return impl->resetCommunication();
}

//SDO
const CANopenSensor::OperationStatus CANopenSensor::read(const int16_t dictID, IODataSize & dataSize, Buffer & data, ErrorCode & errorCode, const int8_t dictSubID) const
{
	return impl->read(dictID, dataSize, data, errorCode, dictSubID);
}

const CANopenSensor::OperationStatus CANopenSensor::write(const int16_t dictID, const IODataSize dataSize, const Buffer & data, ErrorCode & errorCode, const int8_t dictSubID)
{
	return impl->write(dictID, dataSize, data, errorCode, dictSubID);
}

const CANopenSensor::Buffer CANopenSensor::formatIntegralData(const int32_t value, std::true_type)
{
	Buffer ret;

	ret.data_ = value;

#ifdef BOOST_BIG_ENDIAN
	ret.data_ = utils::EndianSwap(ret.data_);
#endif // BOOST_BIG_ENDIAN

	return ret;
}

const CANopenSensor::Buffer CANopenSensor::formatIntegralData(const float value, std::false_type)
{
	Buffer ret;

	union
	{
		float u;
		uint32_t v;
	} data;

	data.u = value;
	
#ifdef BOOST_BIG_ENDIAN
	data.v = utils::EndianSwap(data.v);
#endif // BOOST_BIG_ENDIAN	

	ret.data_ = data.v;

	return ret;
}

const uint32_t CANopenSensor::extractIntegralData(const Buffer buffer, std::true_type)
{
	return utils::EndianSwap(buffer.data_);
}

const float CANopenSensor::extractIntegralData(const Buffer buffer, std::false_type)
{
	union
	{
		float u;
		uint32_t v;
	} data;

	data.v = utils::EndianSwap(buffer.data_);

	return data.u;
}