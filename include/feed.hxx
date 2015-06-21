#ifndef FEED_H
#define FEED_H

#include <agora.hxx>

#include <map>
#include <pugixml.hpp>
#include <string>
#include <time.h>


//! An abstraction for a web news feed.
class Feed {
  private:
	Feed &operator=(const Feed&);

	pugi::xml_document *const feed;
	//! Counter to prevent early deletion of document.
	unsigned char &docRefs, &feedRefs;

	agora::FeedLang lang;

	// Expected members
	std::string id;
	std::string title;
	// Only has limited utility (we've already downloaded the file)
	time_t updated;

	// Optional members
	std::string author;
	std::string link;
	std::string description;

	/*! \class Feed
	 *  \todo Add:
	 *  contributor tags,
	 *  category tags (as default/additional for entries),
	 *  language
	 */

	void parseAtom(const pugi::xml_node&);
	void parseRss (const pugi::xml_node&);

  public:
	//! Default constructor.
	Feed();
	//! Copy constructor.
	Feed(const Feed&);
	//! Build feed manually.
	Feed(const std::string&, const std::string&);
	//! Construct feed from local file.
	Feed(std::string filename);
	//! Standard deconstructor.
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

	//! \param id new ID string
	void setID(const std::string &id) { this->id = id; };

	//! Increment the number of instances of this object.
	void incrementCount() const;
	//! Decrement the number of instances of this object.
	void decrementCount() const;
	//! \return The number of references to this Feed instance
	unsigned char getCount()  const { return feedRefs; };

	//! Is the feed "live", ie. it references a feed document.
	bool          isLive() const { return (feed); };

	//! Print the feed to cout, for debugging purposes.
	void print() const;
};

#endif
