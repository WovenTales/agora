#include <feed.hxx>

class Article;
#include <logger.hxx>

#include <iostream>
#include <limits.h>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;


const std::map<Feed::Column::Name, std::string> Feed::columns(initMap());
std::map<Feed::Column::Name, std::string> Feed::initMap() {
	map<Column::Name, string> out;
	out[Column::ID]          = "fID";
	out[Column::Title]       = "fTitle";
	out[Column::Link]        = "fLink";
	out[Column::Author]      = "fAuthor";
	out[Column::Description] = "fDesc";
	return out;
}


Feed::Feed() : feed(NULL), docRefs(*new unsigned char(1)), feedRefs(*new unsigned char(1)) {
	id = "";
	title = "";
	updated = parseTime("");
	author = "";
	link = "";
	description = "";
	lang = UNKNOWN_LANG;
}

/*! \param f the feed to copy
 */
Feed::Feed(const Feed &f) : feed(f.feed), docRefs(f.docRefs), feedRefs(*new unsigned char(1)) {
	docRefs++;

	id = f.id;
	title = f.title;
	updated = f.updated;
	author = f.author;
	link = f.link;
	description = f.description;
	lang = f.lang;
}

/*! \param id    feed ID
 *  \param title title
 */
Feed::Feed(const std::string &id, const std::string &title) : feed(NULL), docRefs(*new unsigned char(1)), feedRefs(*new unsigned char(1)) {
	this->id = id;
	this->title = title;
	updated = parseTime("");

	author = "";
	link = "";
	description = "";

	lang = UNKNOWN_LANG;
}

/*! \param filename local file to parse
 */
Feed::Feed(std::string filename) : feed(new xml_document()), docRefs(*new unsigned char(1)), feedRefs(*new unsigned char(1)) {
	feed->load_file(filename.c_str());

	// Can't use getRoot because it needs to know the language
	//   Could set it in that function, but then wouldn't be const
	xml_node rss = feed->child("rss").child("channel");
	xml_node atom = feed->child("feed");

	if (rss.type()) {
		parseRss(rss);
	} else if (atom.type() && !strcmp(atom.attribute("xmlns").value(), "http://www.w3.org/2005/Atom")) {
		parseAtom(atom);
	} else {
		//! \todo Better error handling
		cerr << "Invalid feed format" << endl;
	}
}

Feed::~Feed() {
	decrementCount();
	if (docRefs == 0) {
		delete &docRefs;
		if (feed) {
			delete feed;
		}
	}
	if (feedRefs == 0) {
		delete &feedRefs;
	}
}


void Feed::incrementCount() const {
	if (docRefs == UCHAR_MAX) {
		//! \todo Better error handling
		cerr << "Expand size of docRefs!" << endl;
	} else {
		docRefs++;
	}

	if (feedRefs == UCHAR_MAX) {
		//! \todo Better error handling
		cerr << "Expand size of feedRefs!" << endl;
	} else {
		feedRefs++;
	}
}

void Feed::decrementCount() const {
	if (docRefs == 0) {
		//! \todo Better error handling
		cerr << "More deletions of docRefs than additions" << endl;
	} else {
		docRefs--;
	}

	if (feedRefs == UCHAR_MAX) {
		//! \todo Better error handling
		cerr << "More deletions of feedRefs than additions" << endl;
	} else {
		feedRefs--;
	}
}


xml_node Feed::getRoot() const {
	if (lang == RSS) {
		return feed->child("rss").child("channel");
	} else if (lang == ATOM) {
		return feed->child("feed");
	}
};


void Feed::parseAtom(const xml_node &feed) {
	Log << "Parsing feed as Atom...";

	lang = ATOM;

	id = feed.child_value("id");
	title = parseAtomTitle(feed.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = feed.child("author").child_value("name");
	link = feed.child("link").attribute("href").value();
	description = parseAtomTitle(feed.child("subtitle"));

	updated = parseTime(feed.child_value("updated"));

	if (!link.compare("") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}

	Log << "Completed parsing '" << title << "'" << Log.ENDL;
}

void Feed::parseRss(const xml_node &feed) {
	//! \todo Specifically, RSS 2.0; include support for older standards
	Log << "Parsing feed as RSS...";

	lang = RSS;

	description = feed.child_value("description");
	id = link = feed.child_value("link");
	title = feed.child_value("title");

	updated = parseTime(feed.child_value("pubDate"));

	if (id.compare("")) {  // id != ""
		//! \todo Generate unique id
	}

	Log << "Completed parsing '" << title << "'" << Log.ENDL;
}


void Feed::print() const {
	cout << title << (description != "" ? ": " : "") << description << endl;
	cout << "  " << author << endl;
	cout << "  #" << id << endl;
}
