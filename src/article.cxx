#include <article.hxx>

#include <logger.hxx>

#include <iostream>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;


/*! Sets all parameters to empty string or equivalent.
 */
Article::Article(const Feed &p) : parent(p) {
	id = "";
	title = "";
	updated = parseTime("");
	author = "";
	content = "";
	link = "";
	summary = "";
}

Article::Article(const Article &a) : parent(a.parent) {
	parent.incrementCount();

	id = a.id;
	title = a.title;
	updated = a.updated;
	author = a.author;
	content = a.content;
	link = a.link;
	summary = a.summary;
}

/*! \param entry base node of article
 *  \param feed  parent feed
 *  \param lang  language by which to parse node
 */
Article::Article(const pugi::xml_node &entry, const Feed &feed, const FeedLang &lang) : parent(feed) {
	parent.incrementCount();

	if (lang == ATOM) {
		parseAtom(entry);
	} else if (lang == RSS) {
		parseRss(entry);
	} else {
		//! \todo Better error handling
		cerr << "Invalid language for parsing" << endl;
	}
}

void Article::parseArticleData(const Database::ArticleData &data) {
	for (pair<const Database::Table::Article, string> c : data) {
		if (!c.second.empty()) {
			switch (c.first) {
				case Database::Table::Article::ID      : id      = c.second;            break;
				case Database::Table::Article::Title   : title   = c.second;            break;
				case Database::Table::Article::Updated : updated = parseTime(c.second); break;
				case Database::Table::Article::Link    : link    = c.second;            break;
				case Database::Table::Article::Author  : author  = c.second;            break;
				case Database::Table::Article::Summary : summary = c.second;            break;
				case Database::Table::Article::Content : content = c.second;            break;
			}
		}
	}
}
/*! \param data data with which to initialize
 *  \param fID  ID of the parent feed
 */
Article::Article(const Database::ArticleData &data, const std::string &fID) : Article(*new Feed({{ Database::Table::Feed::ID, fID }})) {
	parseArticleData(data);
}
/*! \param data data with which to initialize
 *  \param p    feed to link as parent
 */
Article::Article(const Database::ArticleData &data, const Feed &p) : Article(p) {
	parseArticleData(data);
}

Article::~Article() {
	parent.decrementCount();
	if (!parent.getCount()) {
		delete &parent;
	}
}


// Define here so don't need #include <feed.hxx> in the header
string Article::getFID() const {
	return parent.getID();
}


/*! \param entry the pugi::xml_node containing the article data
 */
void Article::parseAtom(const pugi::xml_node &entry) {
	Log << "Parsing article as Atom...";

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

	Log << "Completed parsing '" << title << "'" << Log.ENDL;
}

/*! \param entry the pugi::xml_node containing the article data
 */
void Article::parseRss(const pugi::xml_node &entry) {
	Log << "Parsing article as RSS...";

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

	if (!link.compare("") &&                                                      // link == "" &&
	    strcmp(entry.child("guid").attribute("isPermaLink").value(), "false") &&  // isPermaLink != "false" &&
	    (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {        // id[0..7] == "http://" || id[0..8] == "https://"
		link = id;
	}

	Log << "Completed parsing '" << title << "'" << Log.ENDL;
}


void Article::print() const {
	Log << "Printing article:" << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << title << " (#" << id << ")" << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << author << " @ " << link << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << summary << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << content << Log.ENDL;
}
