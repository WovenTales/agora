#ifndef FEED_H
#define FEED_H

#include <agora.hxx>

#include <pugixml.hpp>
#include <string>
#include <time.h>

using namespace std;

class Feed {
  private:
	Feed();

	pugi::xml_node root;

	string id;
	string title;
	//TODO: Include in parsing functions (requires recognizing format of string)
	// Actually, only has limited utility (we've already downloaded the file)
	time_t updated;

	string author;
	//TODO: Contributor tags
	string link;
	string description;

	FeedLang lang;

	//TODO: Category tags (default/additional for entries)
	//TODO: Language

	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);

  public:
	Feed(const char *filename);

	std::string     getAuthor()      const { return author; };
	std::string     getDescription() const { return description; };
	std::string     getID()          const { return id; };
	FeedLang        getLang()        const { return lang; };
	std::string     getLink()        const { return link; };
	pugi::xml_node  getRoot()        const { return root; };
	std::string     getTitle()       const { return title; };
	time_t          getUpdateTime()  const { return updated; };

	void print() const;
};

#endif
