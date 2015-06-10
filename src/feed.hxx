#ifndef FEED_H
#define FEED_H

#include <agora.hxx>

#include <pugixml.hpp>
#include <string>
#include <time.h>

using namespace std;

//! An abstraction for a web news feed
class Feed {
  private:
	Feed();

	pugi::xml_document *feed;

	// Expected members
	string id;
	string title;
	// Only has limited utility (we've already downloaded the file)
	time_t updated;

	// Optional members
	string author;
	string link;
	string description;

	/*!\todo Add:
	 * contributor tags,
	 * category tags (as default/additional for entries),
	 * language
	 */

	agora::FeedLang lang;

	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);

  public:
	//! Construct feed from local file
	Feed(std::string filename);
	//! Standard deconstructor
	virtual ~Feed();

	//! \return Author
	std::string     getAuthor()      const { return author; };
	//! \return Description
	std::string     getDescription() const { return description; };
	//! \return %Feed ID
	std::string     getID()          const { return id; };
	//! \return Encoding language
	agora::FeedLang getLang()        const { return lang; };
	//! \return Link to homepage
	std::string     getLink()        const { return link; };
	//! \return Root node
	pugi::xml_node  getRoot()        const;
	//! \return Title
	std::string     getTitle()       const { return title; };
	//! \return Time of last update
	time_t          getUpdateTime()  const { return updated; };

	//! Print the feed to cout, for debugging purposes
	void print() const;
};

#endif
