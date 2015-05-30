#ifndef ARTICLE_H
#define ARTICLE_H

#include "agora.hxx"
#include "database.hxx"

#include <iostream>
#include <pugixml.hpp>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>

using namespace std;

class Article {
  private:
	Article();

	string id;
	string feedID;
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
	
	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);
	
  public:
	Article(const pugi::xml_node&, string fID, const FeedLang);

	const void save(Database&);

	const void print();
};

#endif
