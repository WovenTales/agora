#ifndef AGORA_H
#define AGORA_H

#include <pugixml.hpp>
#include <string>
#include <time.h>

namespace agora {
	enum FeedLang {
		RSS,
		ATOM
	};

	std::string replaceAll(std::string, const std::string&, const std::string&);

	//TODO: see http://atomenabled.org/developers/syndication/#text
	const char *parseAtomTitle(const pugi::xml_node &t);

	time_t parseTime(const char *t);
	time_t parseTime(std::string t);
}

#endif
