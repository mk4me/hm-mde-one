/********************************************************************
	created:  2014/11/19	10:49:53
	filename: ProtocolReadBufferHelper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__
#define __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__

#include <imucostumelib/Export.h>
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/CANopenFrame.h>
#include <utils/Debug.h>
#include <vector>

namespace imuCostume
{
	//! Klasa obs�uguj�ca dane z protoko�u komunikacji
	class IMUCOSTUME_EXPORT ProtocolReadBufferHelper
	{
	public:

		//! Identyfikator wiadomo�ci
		struct HeaderProxy
		{
			//! Ca�y identyfikator
			const uint8_t * base;
			//! \return d�ugo�� danych
			inline const uint8_t length() const
			{
				const auto length = base[1] >> 4;
				UTILS_ASSERT(length <= CANopenFrame::SizeLimits::MaxSize);
				return length;
			};
			//! Identyfikator w�z�a
			inline const CANopenFrame::COBID cobID() const { return CANopenFrame::COBID{ uint16_t(((uint8_t)base[1] & 0x0F) << 8) | (uint16_t)base[0] }; };
		};

		//! Klasa obs�uguj�ca interpretacj� ramki can open z bufora
		class IMUCOSTUME_EXPORT CANopenFrameProxy
		{
		public:
			//! \param base Wska�nik rozpocz�cia ramki
			//! \param buffer Bufor w kt�rym ulokowana jest ramka
			CANopenFrameProxy(const uint8_t * base = nullptr);
			//! Destruktor
			~CANopenFrameProxy();

			//! \return Adres pocz�tku ramki
			const uint8_t * base() const;
			//! \return Nag��wek ramki
			const HeaderProxy header() const;
			//! \return Wska�nik bloku danych ramki
			const uint8_t * data() const;
			//! \return Czy mo�na interpretowa� dane ramki
			const bool empty() const;

		private:
			//! Wska�nik poczatku bloku ramki
			const uint8_t * base_;
		};

	private:

		//! \param frameBasis Wska�niki pocz�tk�w ramek CANopen w danych z kostiumu
		ProtocolReadBufferHelper(const std::vector<const uint8_t*> & frameBasis);

	public:

		//! Konstruktor kopiuj�cy
		ProtocolReadBufferHelper();
		//! \param Other Kopiowany obiekt
		ProtocolReadBufferHelper(ProtocolReadBufferHelper & Other);
		//! \param Other Obiekt przejmowany
		ProtocolReadBufferHelper(ProtocolReadBufferHelper && Other);

		//! \param buffer Odczytany bufor z sieci
		//! \param length D�ugo�� danych w buforze
		static ProtocolReadBufferHelper create(const void * buffer, const uint16_t length);

		//! \return Ilo�� ramek w buforze
		const uint16_t size() const;
		//! \return Czy s� ramki w buforze
		const bool empty() const;
		//! \param idx Indeks ramki o kt�ra pytamu
		//! \return Obiekt interpretuj�cy nasza ramk�
		CANopenFrameProxy operator[](const uint16_t idx) const;

		//! \tparam Container Typ kontenera do kt�rego dopisujemy
		//! \param container [out] Kontener do kt�rego dopisujemy
		template<class Container>
		void appendContainer(Container & container)
		{
			for (auto b : frameBasis){
				container.push_back(CANopenFrame(b));
			}
		}

		//! \tparam Container Typ kontenera kt�ry tworzymy
		//! \return Kontener z ramkami CANopen
		template<class Container>
		Container createContainer()
		{
			Container container;
			for (auto b : frameBasis){
				container.push_back(CANopenFrame(b));
			}
			return container;
		}

	private:
		//! Wektor indeks�w pocz�tk�w ramek w buforze
		std::vector<const uint8_t*> frameBasis;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__
