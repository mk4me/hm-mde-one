#include <webserviceslib/DateTime.h>
#include <time.h>

namespace webservices
{
	//! \param time Czas w formie stringu kt�ry chcemy rozpakowa� do naszgo formatu czasu
	const DateTime toTime(const std::string & time);

	//! \param time Czas kt�y konwertujemy do ci�gu znak�w w formacie UTC
	const std::string toString(const struct tm & time);
}