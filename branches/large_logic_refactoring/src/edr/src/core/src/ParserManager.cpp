#include "CorePCH.h"
#include "ParserManager.h"
#include "DataHierarchyManager.h"
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

using namespace core;

void ParserManager::parsers(ParserPrototypes & parserPrototypes) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		parserPrototypes.push_back(it->first);
	}
}

void ParserManager::parsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		for(auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
			boost::cmatch what;
			if(boost::regex_match(source.c_str(), what, eIT->second.regularExpression)){
				parserPrototypes.push_back(it->first);
				break;
			}
		}
	}
}

void ParserManager::sourceParsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if(it->second.sourceParser){
			for(auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
				boost::cmatch what;
				if(boost::regex_match(source.c_str(), what, eIT->second.regularExpression)){
					parserPrototypes.push_back(it->first);
					break;
				}
			}
		}
	}
}

void ParserManager::streamParsers(const std::string & source, ParserPrototypes & parserPrototypes) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if(it->second.streamParser){
			for(auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
				boost::cmatch what;
				if(boost::regex_match(source.c_str(), what, eIT->second.regularExpression)){
					parserPrototypes.push_back(it->first);
					break;
				}
			}
		}
	}
}

const bool ParserManager::sourceIsAccepted(const std::string & source) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		for(auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
			boost::cmatch what;
			if(boost::regex_match(source.c_str(), what, eIT->second.regularExpression)){
				return true;
			}
		}
	}

	return false;
}

void ParserManager::sourcePossibleTypes(const std::string & source, core::TypeInfoSet & types) const
{
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		for(auto eIT = it->second.expressions.begin(); eIT != it->second.expressions.end(); ++eIT){
			boost::cmatch what;
			if(boost::regex_match(source.c_str(), what, eIT->second.regularExpression)){
				types.insert(eIT->second.types.begin(), eIT->second.types.end());
			}
		}
	}
}

void ParserManager::registerParser(const plugin::IParserPtr & parser)
{
	//! Weryfikujemy ID parsera
	for(auto it = parsers_.begin(); it != parsers_.end(); ++it){
		if(it->first->getID() == parser->getID()){
			CORE_LOG_NAMED_WARNING("parser", "Parser with given ID: " + boost::lexical_cast<std::string>(parser->getID()) + " already registered - skipping registration");
			throw std::runtime_error("Parser with similar ID already registered");
		}
	}

	//! Weryfikujemy możliwości parsowania parsera
	ParserData pData;
	pData.sourceParser = dynamic_cast<plugin::ISourceParserCapabilities*>(parser.get());
	pData.streamParser = dynamic_cast<plugin::IStreamParserCapabilities*>(parser.get());

	if( !(pData.sourceParser || pData.streamParser) ){
		CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->getID()) + " does not support any of known capabilities to parse");
		throw std::runtime_error("Parser does not support any known parse capabilities");
	}

	//! Weryfikujemy wspierane wyrażenia
	plugin::IParser::Expressions expressions;
	parser->acceptedExpressions(expressions);

	if(expressions.empty() == true){
		CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->getID()) + " does not support any expressions");
		throw std::runtime_error("Parser does not support any expresions");
	}

	auto typesHierarchy = getDataHierarchyManager();

	//! Weryfikujemy oferowane typy - zawężamy jeśli coś jest nieznane
	for(auto it = expressions.begin(); it != expressions.end(); ++it){
		ExpressionDescription desc;
		for(auto tIT = it->second.types.begin(); tIT != it->second.types.end(); ++tIT){
			if(typesHierarchy->isRegistered(*tIT)){
				desc.types.insert(*tIT);
			}
		}

		//! Weryfikacja czy wyrażenie wspiera jakieś znane typy
		if(desc.types.empty() == true){
			CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->getID()) + " not offers any known types for expression: " + it->first);
		}else{
			desc.description = it->second.description;
			desc.regularExpression = boost::regex(it->first);
			pData.expressions.insert(Expressions::value_type(it->first, desc));
		}
	}

	//! Weryfikacja czy mamy jakies wyrażenia wspierające znane typy
	if(pData.expressions.empty() == true){
		CORE_LOG_NAMED_WARNING("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->getID()) + " not offers any known types for any offered expression - skiping registration");
	}else{
		parsers_[parser] = pData;
		std::string capabilities;
		if(pData.sourceParser && pData.streamParser){
			capabilities = "custom I/O capabilities and stream capabilities";
		}else if(pData.sourceParser){
			capabilities = "custom I/O capabilities";
		}else{
			capabilities = "stream capabilities";
		}
		CORE_LOG_NAMED_INFO("parser", "Parser with ID: " + boost::lexical_cast<std::string>(parser->getID()) + " successfully registered offering: " + capabilities);
	}
}