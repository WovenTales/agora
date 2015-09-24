#ifndef FEED_H
#define FEED_H

#include <agora.hxx>
#include <database.hxx>

#include <pugixml.hpp>
#include <string>
#include <time.h>


//! An abstraction for a web news feed
class Feed {
  private:
	//! Assignment operator
	/*! \todo Make public (see Database for model) */
	Feed &operator=(const Feed&);

	/*! \todo Should probably create Document class encapsulating differences in encoding (include Feed::lang) */
	pugi::xml_document *const feed;  //!< Pointer to XML representation of feed
	/*! \todo Rework for less redundancy */
	unsigned char &docRefs;          //!< Counter to prevent early deletion of document
	unsigned char &feedRefs;         //!< Counter tracking instances of feed object

	agora::FeedLang lang;            //!< Encoding language of the feed

	// Expected members
	std::string id;                  //!< Unique ID
	std::string uri;                 //!< URI to locate feed
	std::string title;               //!< Display title
	time_t updated;                  //!< Last time feed document checked

	// Optional members
	std::string author;              //!< Default author of the feed
	/*! \todo Standardize; Big Finish has homepage, XKCD has feed address */
	std::string link;                //!< Link to the feed homepage
	std::string description;         //!< Short description

	//! Parse the feed according to internal language
	void parse();
	//! Parse the feed as an ATOM document
	void parseAtom(const pugi::xml_node&);
	//! Parse the feed as an RSS document
	void parseRss (const pugi::xml_node&);

  public:
	//! Default constructor
	Feed();
	//! Copy constructor
	Feed(const Feed&);
	//! Build feed manually
	Feed(const Database::FeedData&, agora::FeedLang = agora::UNKNOWN_LANG);
	//! Construct feed from file
	Feed(const std::string&, agora::FeedLang = agora::UNKNOWN_LANG);
	//! Standard destructor
	virtual ~Feed();

	//! \return \copybrief author
	/*! \todo Do something different with getters ("Returns \\copybrief"?) so is brief description rather than return statement (improper order of docs) */
	std::string     getAuthor()      const { return author; };
	//! \return \copybrief description
	std::string     getDescription() const { return description; };
	//! \return \copybrief id
	std::string     getID()          const { return id; };
	//! \return \copybrief lang
	agora::FeedLang getLang()        const;
	//! \return \copybrief link
	std::string     getLink()        const { return link; };
	//! \return Root node
	pugi::xml_node  getRoot()        const;
	//! \return \copybrief title
	std::string     getTitle()       const { return title; };
	//! \return \copybrief updated
	time_t          getUpdateTime()  const { return updated; };

	//! \param id new ID string
	void setID(const std::string &id) { this->id = id; };

	//! Increment the number of instances of this object
	void incrementCount() const;
	//! Decrement the number of instances of this object
	void decrementCount() const;
	//! \return The number of references to this Feed instance
	unsigned char getCount()  const { return feedRefs; };

	//! Is the feed "live", i.e. it references a feed document
	bool isLive() const { return (feed == NULL); };

	//! Print the feed to cout, for debugging purposes
	void print() const;
};

#endif
