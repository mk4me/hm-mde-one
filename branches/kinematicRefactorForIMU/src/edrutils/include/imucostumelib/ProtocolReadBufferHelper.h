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
#include <vector>

namespace imuCostume
{
	//! Klasa obs³uguj¹ca dane z protoko³u komunikacji
	class IMUCOSTUME_EXPORT ProtocolReadBufferHelper
	{
	public:

		//! Identyfikator wiadomoœci
		struct HeaderProxy
		{
			//! Ca³y identyfikator
			const uint8_t * base;
			//! \return d³ugoœæ danych
			inline const uint8_t length() const { return base[1] >> 4; };
			//! Identyfikator wêz³a
			inline const CANopenFrame::COBID cobID() const { return CANopenFrame::COBID{ uint16_t(((uint8_t)base[1] & 0x0F) << 8) | (uint16_t)base[0] }; };
		};

		//! Klasa obs³uguj¹ca interpretacjê ramki can open z bufora
		class IMUCOSTUME_EXPORT CANopenFrameProxy
		{
		public:
			//! \param base Wska¿nik rozpoczêcia ramki
			//! \param buffer Bufor w którym ulokowana jest ramka
			CANopenFrameProxy(const uint8_t * base = nullptr);
			//! Destruktor
			~CANopenFrameProxy();

			//! \return Adres pocz¹tku ramki
			const uint8_t * base() const;
			//! \return Nag³ówek ramki
			const HeaderProxy header() const;
			//! \return WskaŸnik bloku danych ramki
			const uint8_t * data() const;
			//! \return Czy mo¿na interpretowaæ dane ramki
			const bool empty() const;

		private:
			//! WskaŸnik poczatku bloku ramki
			const uint8_t * base_;
		};

	private:

		//! \param frameBasis WskaŸniki pocz¹tków ramek CANopen w danych z kostiumu
		ProtocolReadBufferHelper(const std::vector<const uint8_t*> & frameBasis);

	public:

		//! Konstruktor kopiuj¹cy
		ProtocolReadBufferHelper();
		//! \param Other Kopiowany obiekt
		ProtocolReadBufferHelper(ProtocolReadBufferHelper & Other);
		//! \param Other Obiekt przejmowany
		ProtocolReadBufferHelper(ProtocolReadBufferHelper && Other);

		//! \param buffer Odczytany bufor z sieci
		//! \param length D³ugoœæ danych w buforze
		static ProtocolReadBufferHelper create(const void * buffer, const uint16_t length);

		//! \return Iloœæ ramek w buforze
		const uint16_t size() const;
		//! \return Czy s¹ ramki w buforze
		const bool empty() const;
		//! \param idx Indeks ramki o która pytamu
		//! \return Obiekt interpretuj¹cy nasza ramkê
		CANopenFrameProxy operator[](const uint16_t idx) const;

		//! \tparam Container Typ kontenera do którego dopisujemy
		//! \param container [out] Kontener do którego dopisujemy
		template<class Container>
		void appendContainer(Container & container)
		{
			for (auto b : frameBasis){
				container.push_back(CANopenFrame(b));
			}
		}

		//! \tparam Container Typ kontenera który tworzymy
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
		//! Wektor indeksów pocz¹tków ramek w buforze
		std::vector<const uint8_t*> frameBasis;
	};
}

#endif	// __HEADER_GUARD_IMU_COSTUME__PROTOCOLREADBUFFERHELPER_H__