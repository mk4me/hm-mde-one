#include <webserviceslib/DateTimeUtils.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace webservices
{

const DateTime toTime(const std::string & time)
{
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	static boost::char_separator<char> sep("-T:.");

	DateTime ret;

	//musimy pociąć datę na kawałki        
	tokenizer tokens(time, sep);
	tokenizer::iterator it = tokens.begin();
	ret.setYear(boost::lexical_cast<int>(*it)); ++it;
	ret.setMonth(boost::lexical_cast<int>(*it)); ++it;
	ret.setDay(boost::lexical_cast<int>(*it)); ++it;
	if(it != tokens.end()){
		ret.setHour(boost::lexical_cast<int>(*it)); ++it;
		ret.setMinutes(boost::lexical_cast<int>(*it)); ++it;
		ret.setSeconds(boost::lexical_cast<int>(*it));
	}else{
		ret.setHour(0);
		ret.setMinutes(0);
		ret.setSeconds(0);
	}

	return ret;
}

const std::string toString(const struct tm & time)
{    
	char buffer[26];

	strftime (buffer,80,"%Y-%m-%dT%H:%M:%S.000Z",&time);
	buffer[25] = '\0';
	return std::string(buffer);
}

}
