/********************************************************************
	created:  2015/01/12	13:36:58
	filename: SamplesStatus.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__
#define __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__

#include <map>
#include <boost/circular_buffer.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>

namespace utils
{
	class SamplesStatus
	{
	public:
		//! Mapa status�w - ich granicznych (g�rnych) warto�ci do faktycznych status�w
		//! Warto�ci powinny by� w przedziale od 0 do 1, w przeciwnym wypakdu zostsan� przeskalowane
		typedef std::map<float, int> StatusMap;

		enum SampleType{
			Positive = 2,
			Negative = 4,
			Unspecified = Positive | Negative
		};

	public:
		//! \param windowSize Historia pr�bek
		//! \param statusMap Mapa status�w
		SamplesStatus(const unsigned int windowSize,
			const StatusMap & statusMap);

		//! \param windowSize Historia pr�bek
		//! \param statusMap Mapa status�w
		SamplesStatus(const unsigned int windowSize,
			StatusMap && statusMap);

		//! Destruktor
		~SamplesStatus();

		static SamplesStatus createBinaryStatus(const int statusLower, const int statusHigher,
			const float splitValue, const unsigned int windowSize);

		static SamplesStatus createBinaryStatus(const int statusLower, const int statusHigher,
			const int minimumPositiveSamples, const unsigned int windowSize);

		//! Dodajemy pozytywn� pr�bk�
		void positiveSample();
		//! Dodajemy negatywn� pr�bk�
		void negativeSample();
		//! Dodajemy pozytywn�/negatywn� pr�bk�
		void unspecifiedSample();
		//! Dodajemy pr�bk�
		//! \param val Warto�� pr�bki
		void sample(const SampleType val);
		//! \return Aktualny status
		int status() const;

	private:
		//! Akumulator dla danych
		mutable boost::accumulators::accumulator_set<float, boost::accumulators::stats<boost::accumulators::tag::rolling_mean>> acc;
		//! Mapa status�w
		const StatusMap statusMap;
	};
}

#endif	// __HEADER_GUARD_UTILS__SAMPLESSTATUS_H__