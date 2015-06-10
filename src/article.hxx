#ifndef ARTICLE_H
#define ARTICLE_H

#include <agora.hxx>
class Feed;

#include <pugixml.hpp>
#include <string>
#include <time.h>

//! A particular entry from some feed
class Article {
  private:
	// Expected members
	std::string id;
	std::string feedID;
	std::string title;
	time_t updated;
	std::string link;

	// Optional members
	std::string author;
	std::string content;  //!\todo See http://atomenabled.org/developers/syndication/#contentElement
	std::string summary;

	/*! \todo Add:
	 *  category tags
	 *  \<source> management
	 */

	void parseAtom(const pugi::xml_node&);
	void parseRss(const pugi::xml_node&);
	
  public:
	//! Default constructor.
	Article();
	//! Construct article from given node.
	Article(const pugi::xml_node&, std::string, const agora::FeedLang);
	//! Construct article using given parameters.
	Article(std::string, std::string, std::string, std::string, time_t, std::string ="", std::string ="", std::string ="");
	
	//! \return Author  
	std::string getAuthor()     const { return author; };  
	//! \return Content
	std::string getContent()    const { return content; };
	//! \return ID of parent feed
	std::string getFID()        const { return feedID; };
	//! \return %Article ID
	std::string getID()         const { return id; };
	//! \return Link to original article
	std::string getLink()       const { return link; };
	//! \return Summary of content
	std::string getSummary()    const { return summary; };
	//! \return Title
	std::string getTitle()      const { return title; };
	//! \return Time of last update
	time_t      getUpdateTime() const { return updated; };

	//! Print the article to cout, for debugging purposes
	void print() const;
};

#endif
