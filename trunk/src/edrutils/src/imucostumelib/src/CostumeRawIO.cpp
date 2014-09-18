#include <imucostumelib/CostumeRawIO.h>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/bind.hpp>

using namespace imuCostume;

class CostumeRawIO::CostumeRawIOImpl
{
public:
	CostumeRawIOImpl(const std::string & ip, const unsigned int port) :
		ip_(ip), port_(port), io_service(), socket(io_service),
		serverEndpoint(boost::asio::ip::address::from_string(ip), port)
	{
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

	~CostumeRawIOImpl()
	{
	}

	const std::string ip() const
	{
		return ip_;
	}

	const unsigned int port() const
	{
		return port_;
	}

	const bool receive(CostumeRawIO::Frame & frame, uint16_t & length, const unsigned int timeout)
	{
		std::size_t l = 0;
		CostumeRawIO::Frame locFrame = { 0 };
		bool transferOK = true;

		if (timeout == 0){
			try{
				l = socket.receive_from(boost::asio::buffer(locFrame), serverEndpoint);
			}
			catch (...){
				transferOK = false;
			}
		}
		else{
			boost::system::error_code ec = boost::asio::error::would_block;
			//! Ograniczenie czasowe na odbiór danych
			boost::asio::deadline_timer deadline(io_service);
			deadline.expires_from_now(boost::posix_time::millisec(timeout));

			// Put the actor back to sleep.
			deadline.async_wait(boost::bind(&CostumeRawIOImpl::check_deadline, &deadline, &socket));

			socket.async_receive_from(boost::asio::buffer(locFrame), serverEndpoint,
				boost::bind(&CostumeRawIOImpl::handle_operation, _1, _2, &ec, &l));

			// Block until the asynchronous operation has completed.
			try{
				do {
					io_service.run_one();
				} while (ec == boost::asio::error::would_block);

				if (ec == boost::asio::error::operation_aborted){
					transferOK = false;
				}
			}
			catch (...){
				transferOK = false;
			}

			deadline.cancel();
		}

		transferOK = transferOK && (l > 0);

		if (transferOK == true){
			length = l;
			frame = locFrame;
		}

		return transferOK;
	}

	const bool send(const void * data, const uint16_t length, const unsigned int timeout)
	{
		std::size_t l = 0;

		if (timeout == 0){
			try{
				l = socket.send_to(boost::asio::buffer(data, length), serverEndpoint);
			}
			catch (...){
			}
		}
		else{
			boost::system::error_code ec = boost::asio::error::would_block;
			//! Ograniczenie czasowe na odbiór danych
			boost::asio::deadline_timer deadline(io_service);
			deadline.expires_from_now(boost::posix_time::millisec(timeout));

			// Put the actor back to sleep.
			deadline.async_wait(boost::bind(&CostumeRawIOImpl::check_deadline, &deadline, &socket));

			socket.async_send_to(boost::asio::buffer(data, length), serverEndpoint,
				boost::bind(&CostumeRawIOImpl::handle_operation, _1, _2, &ec, &l));

			// Block until the asynchronous operation has completed.
			try{
				do {
					io_service.run_one();
				} while (ec == boost::asio::error::would_block);
			}
			catch (...){
			}

			deadline.cancel();
		}

		return (l == length);
	}

private:

	static void check_deadline(const boost::asio::deadline_timer * deadline,
		boost::asio::ip::udp::socket * socket)
	{
		// Check whether the deadline has passed. We compare the deadline against
		// the current time since a new asynchronous operation may have moved the
		// deadline before this actor had a chance to run.
		if (deadline->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			// The deadline has passed. The outstanding asynchronous operation needs
			// to be cancelled so that the blocked receive() function will return.
			//
			// Please note that cancel() has portability issues on some versions of
			// Microsoft Windows, and it may be necessary to use close() instead.
			// Consult the documentation for cancel() for further information.
			socket->cancel();
		}
	}

	static void handle_operation(
		const boost::system::error_code& ec, std::size_t length,
		boost::system::error_code* out_ec, std::size_t* out_length)
	{
		*out_ec = ec;
		*out_length = length;
	}

private:

	//! Adres IP kostiumu
	const std::string ip_;
	//! Port kostiumy
	const unsigned int port_;
	//! Obiekty do odbierania danych po UDP
	//! Serwis
	boost::asio::io_service io_service;
	//! Socket
	boost::asio::ip::udp::socket socket;
	//! Endpoint
	boost::asio::ip::udp::endpoint serverEndpoint;
};

CostumeRawIO::CostumeRawIO(const std::string & ip, const unsigned int port)
	: impl(new CostumeRawIOImpl(ip, port))
{
}

CostumeRawIO::~CostumeRawIO()
{
}

const std::string CostumeRawIO::ip() const
{
	return impl->ip();
}

const unsigned int CostumeRawIO::port() const
{
	return impl->port();
}

const bool CostumeRawIO::receive(Frame & frame, uint16_t & length, const unsigned int timeout)
{
	return impl->receive(frame, length, timeout);
}

const bool CostumeRawIO::send(const void * data, const uint16_t length, const unsigned int timeout)
{
	return impl->send(data, length, timeout);
}