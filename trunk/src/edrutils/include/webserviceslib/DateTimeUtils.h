#include <webserviceslib/DateTime.h>
#include <time.h>

namespace webservices
{
	//! \param time Czas w formie stringu który chcemy rozpakowaæ do naszgo formatu czasu
	const DateTime toTime(const std::string & time);

	//! \param time Czas któy konwertujemy do ci¹gu znaków w formacie UTC
	const std::string toString(const struct tm & time);
}