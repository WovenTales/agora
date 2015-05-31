#ifndef ARTICLE_H
#define ARTICLE_H

#include <agora.hxx>
class Database;

#include <pugixml.hpp>
#include <string>
#include <time.h>

using namespace std;

class Article {
  private:
	string id;
	string feedID;
	string title;
	//TODO: Include in parsing functions (requires recognizing format of string)
	time_t updated;

	string author;
	//TODO: see http://atomenabled.org/developers/syndication/#contentElement
	string content;
	string link;
	string summary;

	//TODO: Category tags
	//TODO: Manage <source>
	
	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);
	
  public:
	Article();
	Article(const pugi::xml_node&, string fID, const FeedLang);
	Article(string id, string feedID, string title, time_t updated, string author ="", string content ="", string link ="", string summary ="");
	
	string getTitle() { return title; };

	void setAuthor(string s) { author = s; };
	void setContent(string s) { content = s; };
	void setFID(string s) { feedID = s; };
	void setID(string s) { id = s; };
	void setLink(string s) { link = s; };
	void setSummary(string s) { summary = s; };
	void setTitle(string s) { title = s; };
	void setUpdate(time_t t) { updated = t; };

	const void save(Database&);

	const void print();
};

#endif