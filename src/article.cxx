#include <article.hxx>

#include <logger.hxx>

#include <iostream>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;

/*! Sets all parameters to empty string or equivalent.
 */
Article::Article() {
	id = "";
	feedID = "";
	title = "";
	updated = parseTime("");
	author = "";
	content = "";
	link = "";
	summary = "";
}
/*! \param entry base node of article
 *  \param fID   ID of parent feed
 *  \param lang  language by which to parse node
 */
Article::Article(const pugi::xml_node &entry, std::string fID, const agora::FeedLang lang) {
	feedID = fID;

	if (lang == ATOM) {
		parseAtom(entry);
	} else if (lang == RSS) {
		parseRss(entry);
	} else {
		cerr << "Invalid language for parsing" << endl;
	}
}
/*! \todo Try to find something like Python's parameter dictionay so order's not as difficult to maintain
 *
 *  \param id      article ID
 *  \param feedID  ID of parent feed
 *  \param title   title
 *  \param link    link to original article
 *  \param updated time of last update
 *  \param author  author
 *  \param content content
 *  \param summary summary
 */
Article::Article(std::string id, std::string feedID, std::string title, std::string link, time_t updated, std::string author, std::string content, std::string summary) {
	this->id = id;
	this->feedID = feedID;
	this->title = title;
	this->link = link;
	this->updated = updated;
	this->author = author;
	this->content = content;
	this->summary = summary;
}

void Article::parseAtom(const xml_node &entry) {
	Logger::log("Parsing article as Atom...", Logger::CONTINUE);

	id = entry.child_value("id");
	title = parseAtomTitle(entry.child("title"));

	// pugixml safely gives empty string if node doesn't exist
	author = entry.child("author").child_value("name");
	content = entry.child_value("content");
	link = entry.child_value("link");
	summary = entry.child_value("summary");

	updated = parseTime(entry.child_value("updated"));

	if (!link.compare("") &&  // link == "" &&
	    (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {  // id[0..7] == "http://" || id[0..8] == "https://"
		link = id;
	}

	Logger::log("Completed parsing '" + title + "'");
}

void Article::parseRss(const xml_node &entry) {
	Logger::log("Parsing article as RSS...", Logger::CONTINUE);

	// pugixml safely gives empty string if node doesn't exist
	id = entry.child_value("guid");
	link = entry.child_value("link");
	summary = entry.child_value("description");
	title = entry.child_value("title");

	updated = parseTime(entry.child_value("pubDate"));

	if (!id.compare("") && link.compare("")) {  // id == "" && link != ""
		id = link;
	}
	if (!id.compare("")) {  // id == ""
		//! \todo Generate unique id
	}

	if (!link.compare("") &&  // link == "" &&
	    strcmp(entry.child("guid").attribute("isPermaLink").value(), "false") &&  // isPermaLink != "false" &&
	    (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {  // id[0..7] == "http://" || id[0..8] == "https://"
		link = id;
	}

	Logger::log("Completed parsing '" + title + "'");
}

void Article::print() const {
	cout << title << " (#" << id << ")" << endl;
	cout << author << " @ " << link << endl;
	cout << summary << endl;
	cout << content << endl;
}
