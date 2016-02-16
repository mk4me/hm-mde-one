/********************************************************************
created:  2015/07/28	15:04:12
filename: FunctionFeature.h
author:	  Mateusz Janiak

purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__
#define __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__

#include <dataaccessorlib/Accessors.h>
#include <dataaccessorlib/UniformArgumentsFeature.h>
#include <set>
#include <utils/Utils.h>
#include <type_traits>
#include <utils/Export.h>

#if defined (dataaccessorlib_EXPORTS)
#define DATAACCESSORLIB_EXPORT UTILS_DECL_EXPORT
#else
#define DATAACCESSORLIB_EXPORT UTILS_DECL_IMPORT
#endif

namespace dataaccessor
{
	class IFunctionFeature;

	class DATAACCESSORLIB_EXPORT FunctionFeature
	{
	private:
		static const utils::shared_ptr<IFunctionFeature> functionFeature;
		static const utils::shared_ptr<IFunctionFeature> nonFunctionFeature;

	public:

		static inline const utils::shared_ptr<IFunctionFeature> & feature(const bool isFunc)
		{
			return (isFunc == true) ? functionFeature : nonFunctionFeature;
		}
	};

	//! Interfejs opisuj¹cy cechy kana³y reprezentuj¹cego funkcjê - jeden argument jedna wartoœæ,
	//! jak kana³ nie ma tego interfejsu to traktujemy go jako relacje i mo¿liwa jest tylko wersja dyskretna
	//! - musi j¹ implementowaæ, inaczej FAIL
	class IFunctionFeature : public FeatureHelperT<Function>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFunctionFeature() {}
		//! \return Czy funkcja jest parzysta
		virtual bool isFunction() const = 0;

		template<typename ValueType, typename ArgumentType>
		inline static utils::shared_ptr<IFunctionFeature> create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			if (discrete != nullptr) {
				return create(*discrete);
			}
			return FunctionFeature::feature(true);
		}

		template<typename ArgumentType>
		inline static utils::shared_ptr<IFunctionFeature> create(
			const IDiscreteArgumentAccessorT<ArgumentType> & accessor)
		{
			bool isFun = true;

			if (accessor.empty() == true) {
				isFun = false;
			}
			else if ((accessor.size() > 1) && (accessor.getOrCreateFeature<dataaccessor::IUniformArgumentsFeature>() == nullptr)) {
				auto sA = accessor.argument(0);
				auto sB = accessor.argument(1);
				if (sA != sB) {
					std::set<ArgumentType> arguments;
					arguments.insert(sA);
					arguments.insert(sB);
					const auto signum = utils::sign(sB - sA);
					auto locSignum = signum;
					for (decltype(accessor.size()) idx = 2; idx < accessor.size(); ++idx)
					{
						sA = sB;
						sB = accessor.argument(idx);
						locSignum = utils::sign(sB - sA);
						if (locSignum != signum) {
							isFun = false;
							break;
						}

						if (arguments.find(sB) == arguments.end()) {
							arguments.insert(signum > 0 ? arguments.end() : arguments.begin(), sB);
						}
						else {
							isFun = false;
							break;
						}						
					}					
				}
			}

			return FunctionFeature::feature(isFun);
		}
	};
}

#endif	// __HEADER_GUARD_DATAACCESSOR__FUNCTIONFEATURE_H__