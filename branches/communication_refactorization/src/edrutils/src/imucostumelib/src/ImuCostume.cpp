#include <imucostumelib/ImuCostume.h>
#include <threadingUtils/SynchronizationPolicies.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>

//! Implementacja funkcjonalnoœci kostiumu
class imuCostume::CostumeImpl
{
public:
	//! Rozmiar bufora na dane z kostiumu
	static const unsigned int ImuBufferDataSize = 1024;

public:
	CostumeImpl(const std::string & ip, const unsigned int port, const float timeout) :
		ip_(ip), port_(port), timeout_(0.0), ready_(false), imusNumber_(Costume::MaxIMUsPerCostume),
		io_service(), socket(io_service),
		serverEndpoint(ip.empty() == true ? boost::asio::ip::address_v4::broadcast() :
		boost::asio::ip::address::from_string(ip), port),
		deadline(io_service)
	{
		setTimeout(timeout);

		boost::system::error_code error;
		socket.open(boost::asio::ip::udp::v4(), error);
		if (!error) {
			socket.bind(
				boost::asio::ip::udp::endpoint(
				boost::asio::ip::address_v4::any(),
				port));
		}
		else{
			throw std::runtime_error(error.message());
		}
	}

	//! Destruktor
	~CostumeImpl()
	{
	}

	const float timeout() const
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch);
		return timeout_;
	}

	void setTimeout(const float timeout)
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch);
		timeout_ = timeout;
	}

	//! \return Adres kostiumu
	const std::string & ip() const
	{
		return ip_;
	}
	//! \return Port na którym mamy kostium
	const unsigned int port() const
	{
		return port_;
	}
	//! \return Iloœæ czujników IMU w kostiumie
	const unsigned int imusNumber() const
	{
		return imusNumber_;
	}
	//! \return Opis odebranych danych kostiumu - odczyty IMU + stan odczytów
	const Costume::CostumePacket costumePacket() const
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch);
		return costumePacket_;
	}
	//! \return Czy dane gotowe do odbioru
	const bool ready() const
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch);
		return ready_;
	}

	//! Metoda czytaj¹ca pojedynczy pakiet danych z kostiumu
	void readPacket()
	{
		threadingUtils::ScopedLock<threadingUtils::StrictSyncPolicy> lock(synch);

		const boost::posix_time::time_duration td(boost::posix_time::millisec(1000 * timeout_));

		boost::system::error_code ec = boost::asio::error::would_block;

		boost::array<char, ImuBufferDataSize> buf;
		std::size_t length = 0;

		deadline.expires_from_now(td);

		// Put the actor back to sleep.
		deadline.async_wait(boost::bind(&CostumeImpl::check_deadline, this));

		socket.async_receive_from(boost::asio::buffer(buf), serverEndpoint,
			boost::bind(&CostumeImpl::handle_receive, _1, _2, &ec, &length));

		// Block until the asynchronous operation has completed.
		do {
			io_service.run_one();
		} while (ec == boost::asio::error::would_block);

		deadline.cancel();

		costumePacket_.data.resize(Costume::MaxIMUsPerCostume);
		costumePacket_.status.resize(Costume::MaxIMUsPerCostume);

		if (length == ImuBufferDataSize){
			ready_ = true;

			// Chyba moje dane
			// Próbuje to rozpakowaæ
			// Jak wszystko ok to zaznaczam ¿e ready

			std::fill(costumePacket_.status.begin(), costumePacket_.status.end(), Costume::DATA);

			for (unsigned int i = 0; i < Costume::MaxIMUsPerCostume; ++i)
			{
				//il.ramek, dl. ramki
				//auto offs = 4 * 8 * i;
				auto offs = i << 5;

				{
					osg::Vec3 acc;

					acc.x() = int16_t((uint16_t(buf[offs + 1]) << 8) | uint16_t(buf[offs + 0])) / 1024.0f;
					acc.y() = int16_t((uint16_t(buf[offs + 3]) << 8) | uint16_t(buf[offs + 2])) / 1024.0f;
					acc.z() = int16_t((uint16_t(buf[offs + 5]) << 8) | uint16_t(buf[offs + 4])) / 1024.0f;

					costumePacket_.data[i].accelerometer = acc;
					offs += 8;
				}

				{
					osg::Vec3 mag;

					mag.x() = int16_t((uint16_t(buf[offs + 1]) << 8) | uint16_t(buf[offs + 0])) / 2048.0f;
					mag.y() = int16_t((uint16_t(buf[offs + 3]) << 8) | uint16_t(buf[offs + 2])) / 2048.0f;
					mag.z() = int16_t((uint16_t(buf[offs + 5]) << 8) | uint16_t(buf[offs + 4])) / 2048.0f;

					costumePacket_.data[i].magnetometer = mag;
					offs += 8;
				}

				{
					osg::Vec3 gyro;

					gyro.x() = int16_t((uint16_t(buf[offs + 1]) << 8) | uint16_t(buf[offs + 0])) / 1024.0f;
					gyro.y() = int16_t((uint16_t(buf[offs + 3]) << 8) | uint16_t(buf[offs + 2])) / 1024.0f;
					gyro.z() = int16_t((uint16_t(buf[offs + 5]) << 8) | uint16_t(buf[offs + 4])) / 1024.0f;

					offs += 8;
					costumePacket_.data[i].gyroscope = gyro;
				}

				{
					osg::Quat orient;

					orient.w() = float((uint16_t(buf[offs + 1]) << 8) | uint16_t(buf[offs + 0]));
					orient.x() = float((uint16_t(buf[offs + 3]) << 8) | uint16_t(buf[offs + 2]));
					orient.y() = float((uint16_t(buf[offs + 5]) << 8) | uint16_t(buf[offs + 4]));
					orient.z() = float((uint16_t(buf[offs + 7]) << 8) | uint16_t(buf[offs + 6]));

					costumePacket_.data[i].orientation = orient;
				}
			}
		}
		else{
			std::fill(costumePacket_.status.begin(), costumePacket_.status.end(), Costume::NODATA);
		}
	}

private:

	void check_deadline()
	{
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline.expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			// The deadline has passed. The outstanding asynchronous operation needs
			// to be cancelled so that the blocked receive() function will return.
			//
			// Please note that cancel() has portability issues on some versions of
			// Microsoft Windows, and it may be necessary to use close() instead.
			// Consult the documentation for cancel() for further information.
			socket.cancel();
		}
	}

	static void handle_receive(
		const boost::system::error_code& ec, std::size_t length,
		boost::system::error_code* out_ec, std::size_t* out_length)
	{
		*out_ec = ec;
		*out_length = length;
	}

private:
	//! Timeout
	float timeout_;
	//! Adres IP kostiumu
	std::string ip_;
	//! Port kostiumy
	unsigned int port_;
	//! Iloœc czujników na kostiumie
	unsigned int imusNumber_;
	//! Dane i ich status
	Costume::CostumePacket costumePacket_;
	//! Czy dane gotowe
	volatile bool ready_;
	//! Obiekt synchronizuj¹cy odczyt danych i pobieranie danych
	mutable threadingUtils::StrictSyncPolicy synch;
	//! Obiekty do odbierania danych po UDP
	//! Serwis
	boost::asio::io_service io_service;
	//! Socket
	boost::asio::ip::udp::socket socket;
	//! Endpoint
	boost::asio::ip::udp::endpoint serverEndpoint;
	//! Ograniczenie czasowe na odbiór danych
	boost::asio::deadline_timer deadline;
};

using namespace imuCostume;

Costume::Costume(const std::string & ip /* = std::string() */,
	const unsigned int port /* = 1234 */,
	const float timeout /* = 0.01 */) : impl_(new CostumeImpl(ip, port, timeout))
{
}

Costume::~Costume()
{
}

const std::string & Costume::ip() const
{
	return impl_->ip();
}

const unsigned int Costume::port() const
{
	return impl_->port();
}

const unsigned int Costume::imusNumber() const
{
	return impl_->imusNumber();
}

const Costume::CostumePacket Costume::costumePacket() const
{
	return impl_->costumePacket();
}

const bool Costume::ready() const
{
	return impl_->ready();
}

void Costume::readPacket()
{
	return impl_->readPacket();
}

const float Costume::timeout() const
{
	return impl_->timeout();
}

void Costume::setTimeout(const float timeout)
{
	if (timeout <= 0.0){
		throw std::invalid_argument("Non-positive timeout value");
	}

	impl_->setTimeout(timeout);
}