#ifndef ENTRY_H
#define ENTRY_H

#include <iostream>
#include <pugixml.hpp>
#include <string>
#include <time.h>

#include "feed.hxx"

using namespace std;

class Entry {
  private:
	Entry();

	string id;
	string title;
	//TODO: Include in parsing functions (requires recognizing format of string)
	time_t updated;

	string author;
	//TODO: see http://atomenabled.org/developers/syndication/#contentElement
	string content;
	string link;
	//TODO: Include in parsing functions (would require recognizing format of string)
	time_t published;
	string summary;

	//TODO: Category tags
	//TODO: Manage <source>
	
	void parseAtom(pugi::xml_node&);
	void parseRss(pugi::xml_node&);
	
  public:
	Entry(pugi::xml_node&, FeedLang);

	void print();
};

#endif
