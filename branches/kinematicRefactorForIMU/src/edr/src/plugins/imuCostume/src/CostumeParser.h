/********************************************************************
	created:  2015/01/22	09:03:26
	filename: CostumeParser.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__COSTUMEPARSER_H__
#define __HEADER_GUARD_IMU__COSTUMEPARSER_H__

#include <ostream>
#include <corelib/IParser.h>
#include <utils/SmartPtr.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <utils/ObjectWrapper.h>
#include <utils/ClonePolicies.h>

namespace IMU
{
	class CostumeParser : public plugin::ISourceParser
	{
		UNIQUE_ID("33178925-FC36-4260-B0EC-25F5268BE1F2")
			CLASS_DESCRIPTION("Costume data parser", "Costume data parser")
	public:
		virtual ~CostumeParser() {}
		CostumeParser() {}

	public:
		virtual void acceptedExpressions(Expressions & expressions) const override;
		virtual IParser* create() const override;
		virtual void getObject(core::Variant& object, const core::VariantsVector::size_type idx) const override;
		virtual void reset() override;
		virtual void parse(const std::string& path) override;

		static void save(std::ostream & stream, const IIMUDataSource::CostumesDataFrame & data);
		static void save(std::ostream & stream, const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data);
		static void save(const std::string & path, const IIMUDataSource::CostumesRecordingDataBuffer::ListT & data);

	private:
		utils::ObjectWrapperPtr dataWrapper;
	};
}

DEFINE_WRAPPER(IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT, PtrPolicyStd, ClonePolicyCopyConstructor);

#endif	// __HEADER_GUARD_IMU__COSTUMEPARSER_H__