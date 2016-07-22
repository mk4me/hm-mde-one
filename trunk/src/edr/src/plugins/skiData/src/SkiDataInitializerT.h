/********************************************************************
	created:  2016/07/18
	filename: SkiDataInitializerT.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__
#define __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__

#include <corelib/Variant.h>
#include <dataaccessorlib/Wrappers.h>

namespace skidata
{
	template<typename Extractor>
	class SkiDataInitializerT : public core::IVariantInitializer
	{
	public:

		SkiDataInitializerT(core::VariantConstPtr rawData,
			const dataaccessor::UniformArgumentsGenerator<float> & uag,
			utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature)
			: rawData(rawData), uag(uag), feature(feature)
		{

		}

		virtual ~SkiDataInitializerT() {}

		//! \param object Obiekt który inicjujemy
		virtual void initialize(core::Variant * object) override
		{
			utils::shared_ptr<const SkiData> sd;
			if (rawData->tryGet(sd) == true) {								
				auto data = dataaccessor::wrap(sd, uag, Extractor());
				data->attachFeature(feature);
				object->set(utils::ObjectWrapper::wrap(data));
			}
		}

		//! \return Kopia inicjalizatora
		virtual IVariantInitializer * clone() const override
		{
			return new SkiDataInitializerT<Extractor>(rawData, uag, feature);
		}

	private:
		core::VariantConstPtr rawData;
		dataaccessor::UniformArgumentsGenerator<float> uag;
		utils::shared_ptr<dataaccessor::UniformArgumentsFeature<float>> feature;
	};
}

#endif  // __HEADER_GUARD_SKIDATA__SKIDATAINITIALIZERT_H__