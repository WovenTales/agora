#ifndef ARTICLE_H
#define ARTICLE_H

#include <agora.hxx>
#include <database.hxx>
class Feed;

#include <pugixml.hpp>
#include <string>
#include <time.h>


//! A particular entry from some feed
class Article {
  private:
	//! Base form of constructor
	Article(const Feed&);
	//! Assignment operator
	/*! \todo Make public (see Database for model) */
	Article &operator=(const Article&);

	// Expected members
	std::string id;       //!< Unique ID
	const Feed &parent;   //!< Containing Feed
	std::string title;    //!< Display title
	time_t updated;       //!< Time of last update
	std::string link;     //!< Permanent link to original article

	// Optional members
	std::string author;   //!< Author of the article
	/*! \todo See http://atomenabled.org/developers/syndication/#contentElement */
	std::string content;  //!< Main content
	std::string summary;  //!< Short description

	//! Parse the article as ATOM
	void parseAtom(const pugi::xml_node&);
	//! Parse the article as RSS
	void parseRss(const pugi::xml_node&);

	//! Helper for constructing object
	void parseArticleData(const Database::Data&);
	
  public:
	//! Default constructor
	Article();
	//! Standard copy constructor
	Article(const Article&);
	//! Construct article from given node
	Article(const pugi::xml_node&, const Feed&, const agora::FeedLang&);
	//! Construct article using given parameters
	Article(const Database::Data&, const std::string& = "");
	//! \copybrief Article(const Database::ArticleData&, const std::string&)
	Article(const Database::Data&, const Feed&);
	//! Standard destructor
	virtual ~Article();

	//! Representation of table \c articles
	static const Database::Table columns;

	//! \return \copybrief author
	std::string getAuthor()     const { return author; };  
	//! \return \copybrief content
	std::string getContent()    const { return content; };
	//! \return ID of parent feed.
	std::string getFID() const;  //Define in .cxx so don't need #include <feed.hxx> here
	//! \return \copybrief id
	std::string getID()         const { return id; };
	//! \return \copybrief link
	std::string getLink()       const { return link; };
	//! \return \copybrief summary
	std::string getSummary()    const { return summary; };
	//! \return \copybrief title
	std::string getTitle()      const { return title; };
	//! \return \copybrief updated
	time_t      getUpdateTime() const { return updated; };

	//! Print the article to the Log, for debugging purposes
	void print() const;
};

#endif
