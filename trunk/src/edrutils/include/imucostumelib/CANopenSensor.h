/********************************************************************
	created:  2014/09/05	13:08:37
	filename: CANopenSensor.h
	author:	  Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__
#define __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__

#include <imucostumelib/Export.h>
#include <cstdint>
#include <type_traits>
//#include <set>
#include <utils/SmartPtr.h>
#include <utils/Debug.h>

namespace imuCostume
{
	class CostumeRawIO;

	class IMUCOSTUME_EXPORT CANopenSensor
	{
	private:

		class CANopenSensorImpl;

	public:

		struct IMUCOSTUME_EXPORT Buffer
		{
			friend class CANopenSensor;

		private:

			//! Konstruktor domyœlny
			Buffer();

		public:

			//! Destruktor niepolimorficzny
			~Buffer();
			//! \return Dane w buforze
			const uint32_t data() const;

		private:
			//dane fo sformatowaniu gotowe do wys³ania
			uint32_t data_;
		};

		/*
		enum SpeedLevel
		{
		SpeedLevel0,	//! Najwy¿sza prêdkoœæ komunikacji
		SpeedLevel1,
		SpeedLevel2,
		SpeedLevel3,
		SpeedLevel4,
		SpeedLevel5,
		SpeedLevel6,
		SpeedLevel7,
		SpeedLevel8,	//! Najni¿sza prêdkoœæ komunikacji
		};*/

		enum IODataSize
		{
			Size1B,
			Size2B,
			Size4B
		};

		enum OperationStatus
		{
			OPERATION_OK,
			OPERATION_ERROR,
			OPERATION_SEND_FAILURE,
			OPERATION_RECEIVE_FAILURE
		};

		enum ErrorCode
		{
			NO_ERROR = 0x00,
			NO_TOOGLE_BIT = 0x05030000,
			OUT_OF_MEMORY = 0x05030005,
			UNKNOWN_COMMAND = 0x06040001,
			INDEX_NOT_FOUND = 0x06020000,
			READ_ONLY_OBJECT = 0x06010002,
			WRITE_ONLY_OBJECT = 0x06010001,
			PDO_MAPPING_FAILURE = 0x06040041,
			PDO_SIZE_OVERRUN = 0x06040042,
			PARAMETER_SIZE_MISMATCH = 0x06040043,
			INTERNAL_DATA_INCONSISTENCY = 0x06040047,
			FLASH_MEMORY_ACCESS_FAILURE = 0x06060000,
			SUB_INDEX_NOT_FOUND = 0x06090011,
			OUT_OF_RANGE_VALUE = 0x06090030,
			TOO_HIGH_VALUE = 0x06090031,
			TOO_LOW_VALUE = 0x06090032,
			GENERAL_ERROR = 0x08000000,
			SIGNATURE_MISMATCH = 0x08000020,
			TOO_SHORT_DATA = 0x06070013,
			TOO_LONG_DATA = 0x06070012
		};

		enum Dictionary
		{
			DEVICE_TYPE = 0x1000,
			ERROR_REGISTER = 0x1001,
			COB_ID_SYNC = 0x1005,
			COMMON_CYCLE_PERIOD = 0x1006,
			STORE_PARAMETERS = 0x1010,
			RESTORE_DEFAULT_PARAMETERS = 0x1011,
			COB_ID_EMCY = 0x1014,
			PRODUCED_HEARTBEAT_TIME = 0x1017,
			IDENTITY_OBJECT = 0x1018,
			SDO_SERVER = 0x1200,
			RPDO_PARAM_BASE = 0x1400,
			RPDO_MAPPING_BASE = 0x1600,
			TPDO_PARAM_BASE = 0x1800,
			TPDO_MAPPING_BASE = 0x1A00,
			NMT_STARTUP = 0x1F80
		};

		enum StoreParametersSub
		{
			STORE_PARAMETERS_NUMBER_OF_ENTRIES = 0,
			STORE_PARAMETERS_SAVE_ALL = 1,
		};

		enum RestoreParametersSub
		{
			RESTORE_DEFAULT_PARAMETERS_NUMBER_OF_ENTRIES = 0,
			RESTORE_DEFAULT_PARAMETERS_ALL = 1,
		};

		enum IdentityObjectParametersSub
		{
			IDENTITY_OBJECT_NUMBER_OF_ENTRIES = 0,
			IDENTITY_OBJECT_VENDOR_ID = 1,
			IDENTITY_OBJECT_PRODUCT_CODE = 2,
			IDENTITY_OBJECT_REVISION_NUMBER = 3,
			IDENTITY_OBJECT_SERIAL_NUMBER = 4,
		};

		enum SDOServerParametersSub
		{
			SDO_SERVER_NUMBER_OF_ENTRIES = 0,
			SDO_SERVER_COB_ID_CLINET_TO_SERVER = 1,
			SDO_SERVER_COB_ID_SERVER_TO_CLINET = 2,
		};

		enum RPDOParamParametersSub
		{
			RPDO_PARAM_NUMBER_OF_ENTRIES = 0,
			RPDO_PARAM_COB_ID = 1,
			RPDO_PARAM_TRANSMISSION_TYPE = 2,
			RPDO_PARAM_EVENT_TIMER = 5,
		};

		enum RPDOMappingParametersSub
		{
			RPDO_MAPPING_NUMBER_OF_ENTRIES = 0,
			RPDO_MAPPING_PDO_MAPPING_ENTRY_BASE = 1,	//TODO co znaczy YY?
		};

		enum TPDOParamParametersSub
		{
			TPDO_PARAM_NUMBER_OF_ENTRIES = 0,
			TPDO_PARAM_COB_ID = 1,
			TPDO_PARAM_TRANSMISSION_TYPE = 2,
			TPDO_PARAM_INHIBT_TIMER = 3,
			TPDO_PARAM_EVENT_TIMER = 5,
			TPDO_PARAM_SYNC_START_VALUE = 6
		};

		enum TPDOMappingParametersSub
		{
			TPDO_MAPPING_NUMBER_OF_ENTRIES = 0,
			TPDO_MAPPING_PDO_MAPPING_ENTRY_BASE = 1,	// TODO co znaczy YY?
		};

	public:
		//! \param costume Kostium którego czujnikiem chcemy sterowaæ i siê komunikowaæ
		//! \param id Identyfikator czujnika
		//! \param timeout Maksymalny czas na wys³anie i odpowiedŸ wiadomoœci z kostiumu [ms]
		CANopenSensor(CostumeRawIO * costume, const int8_t id, const unsigned int timeout = 300);
		//! Destruktor
		virtual ~CANopenSensor();

		//! \return Timeout oczekiwania na odpowiedŸ z kostiumu
		const unsigned int timeout() const;

		//LSS
		/*
		const OperationStatus select(const bool select);
		static const OperationStatus select(const bool select, const std::set<int8_t> & ids, Costume * costume);
		static const OperationStatus selectAll(const bool select, Costume * costume);
		const OperationStatus setID(const int8_t id);
		const OperationStatus setSpeedLevel(const SpeedLevel speedLevel);
		*/
		const OperationStatus saveConfiguration(ErrorCode & errorCode);

		//NMT
		const OperationStatus start();
		const OperationStatus stop();
		const OperationStatus prepare();

		const OperationStatus reset();
		static const OperationStatus resetAllSensors(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries);
		static const OperationStatus synchronize(CostumeRawIO * costume, const unsigned int timeout, const unsigned int maxRetries);

		const OperationStatus resetCommunication();

		//SDO
		template<typename T>
		const OperationStatus readSDO(const int16_t dictID, IODataSize & dataSize, T & value, ErrorCode & errorCode, const int8_t dictSubID = 0) const
		{
			Buffer buf;
			auto ret = readSDO(dictID, dataSize, buf, errorCode, dictSubID);
			if (ret == OPERATION_OK){
				value = extractData(buf);
			}

			return ret;
		}

		template<typename T>
		const OperationStatus writeSDO(const int16_t dictID, const IODataSize dataSize, const T value, ErrorCode & errorCode, const int8_t dictSubID = 0)
		{
			return writeSDO(dictID, dataSize, formatData(value), errorCode, dictSubID);
		}

	private:

		const OperationStatus readSDO(const int16_t dictID, IODataSize & dataSize, Buffer & data, ErrorCode & errorCode, const int8_t dictSubID = 0) const;
		const OperationStatus writeSDO(const int16_t dictID, const IODataSize dataSize, const Buffer & data, ErrorCode & errorCode, const int8_t dictSubID = 0);

		template<typename T>
		static const Buffer formatData(const T value)
		{
			UTILS_STATIC_ASSERT((std::is_floating_point<T>::value | std::is_integral<T>::value), "Wrong parameter type - should be POD integral or floating point");
			return formatIntegralData(value, std::is_integral<T>());
		}

		template<typename T>
		static const T extractData(const Buffer buffer)
		{
			UTILS_STATIC_ASSERT((std::is_floating_point<T>::value | std::is_integral<T>::value), "Wrong parameter type - should be POD integral or floating point");
			return (T)(extractData(buffer, std::is_integral<T>()));
		}

		static const Buffer formatIntegralData(const int32_t value, std::true_type);
		static const Buffer formatIntegralData(const float value, std::false_type);
		static const uint32_t extractIntegralData(const Buffer buffer, std::true_type);
		static const float extractIntegralData(const Buffer buffer, std::false_type);

	private:

		utils::shared_ptr<CANopenSensorImpl> impl;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__CANOPENSENSOR_H__