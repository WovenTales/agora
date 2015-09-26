#include <feed.hxx>

class Article;
#include <logger.hxx>

#include <iostream>
#include <limits.h>
#include <sstream>
#include <string.h>


using namespace agora;
using namespace pugi;
using namespace std;


const Database::Table Feed::columns("feeds", {
	{ "id",      "feeds", "fID",      false },
	{ "uri",     "feeds", "fURI",     false },
	{ "title",   "feeds", "fTitle",   true  },  //!< \todo Implement and check for user-specified title
	{ "updated", "feeds", "fUpdated", false },
	{ "link",    "feeds", "fLink",    true  },
	{ "author",  "feeds", "fAuthor",  true  },  //!< \todo Change all feeds using (previous) default author on update
	{ "desc",    "feeds", "fDesc",    true  }
	});


Feed::Feed() : feed(NULL), docRefs(*new unsigned char(1)), feedRefs(*new unsigned char(1)) {
	id = "";
	uri = "";
	title = "";
	updated = time(NULL);
	author = "";
	link = "";
	description = "";
	lang = UNKNOWN_LANG;
}

/*! \param f the feed to copy
 */
Feed::Feed(const Feed &f) : feed(f.feed), docRefs(f.docRefs), feedRefs(*new unsigned char(1)) {
	++docRefs;

	id = f.id;
	uri = f.uri;
	title = f.title;
	updated = f.updated;
	author = f.author;
	link = f.link;
	description = f.description;
	lang = f.lang;
}

/*! \param data     data with which to initialize
 *  \param language encoding of feed
 */
Feed::Feed(const Database::Data &data, agora::FeedLang language) : Feed() {
	for (pair<const Database::Table::Column, string> c : data) {
		if (!c.second.empty()) {
			     if (c.first == Feed::columns["id"])      id          = c.second;
			else if (c.first == Feed::columns["uri"])     uri         = c.second;
			else if (c.first == Feed::columns["title"])   title       = c.second;
			else if (c.first == Feed::columns["updated"]) updated     = parseTime(c.second);
			else if (c.first == Feed::columns["link"])    link        = c.second;
			else if (c.first == Feed::columns["author"])  author      = c.second;
			else if (c.first == Feed::columns["desc"])    description = c.second;
		}
	}
	lang = language;
}

/*! Will usually access remote feed; to refer to a local file, begin the path
 *    with \c %file://
 *
 *  \param uri      file to parse
 *  \param language encoding of feed
 *
 *  \warning If \c l is specified (not agora::UNKNOWN_LANG) but is not the
 *             correct encoding, feed will be improperly parsed; usually better
 *             to let the autodetection determine encoding.
 */
Feed::Feed(const std::string &uri, agora::FeedLang language) : feed(new xml_document()), docRefs(*new unsigned char(1)), feedRefs(*new unsigned char(1)) {
	if (!uri.compare(0, 7, "file://", 0, string::npos)) {  // uri begins with "file://"
		Log << "Loading file from '" << uri << "'" << Log.ENDL;

		feed->load_file(uri.substr(7).c_str());
	} else {
		istringstream data(download(uri));
		feed->load(data);
	}

	this->uri = uri;

	lang = language;  // initialize for getLang()
	lang = getLang();

	parse();
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

	if (feedRefs == 0) {
		//! \todo Better error handling
		cerr << "More deletions of feedRefs than additions" << endl;
	} else {
		feedRefs--;
	}
}


/*! If Feed.lang set to UNKNOWN_LANG, will attempt to detect language based on document.
 */
agora::FeedLang Feed::getLang() const {
	if (lang != UNKNOWN_LANG) {
		return lang;
	}

	if (feed == NULL) {
		//! \todo Implement WARNING level in log
		Log << "getLang attempted language detection on Feed with null document reference" << Log.ENDL;
		return UNKNOWN_LANG;
	}
	if (feed->child("rss").child("channel").type()) {  // not a null node
		Log << "Detected RSS syntax in feed" << Log.ENDL;
		return RSS;
	}
	xml_node nodeFeed = feed->child("feed");
	if (nodeFeed.type() && !strcmp(nodeFeed.attribute("xmlns").value(), "http://www.w3.org/2005/Atom")) {  // not null node && node.xmlns == ...
		Log << "Detected ATOM syntax in feed" << Log.ENDL;
		return ATOM;
	}

	//! \todo Implement WARNING level in log
	Log << "Unable to detect feed language" << Log.ENDL;
	return UNKNOWN_LANG;
}

xml_node Feed::getRoot() const {
	if (lang == RSS) {
		return feed->child("rss").child("channel");
	} else if (lang == ATOM) {
		return feed->child("feed");
	}
}


void Feed::parse() {
	xml_node root = getRoot();

	if (lang == RSS) {
		parseRss(root);
	} else if (lang == ATOM) {
		parseAtom(root);
	} else {
		//! \todo Better error handling
		cerr << "Invalid feed format" << endl;
	}
}

void Feed::parseAtom(const xml_node &feed) {
	Log << "Parsing feed as Atom...";

	lang = ATOM;

	id = feed.child_value("id");
	title = parseAtomTitle(feed.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = feed.child("author").child_value("name");
	link = feed.child("link").attribute("href").value();
	description = parseAtomTitle(feed.child("subtitle"));

	updated = time(NULL);

	// link == "" && id follows URL pattern
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

	updated = time(NULL);

	if (!id.compare("")) {  // id == ""
		//! \todo Generate unique id
	}

	Log << "Completed parsing '" << title << "'" << Log.ENDL;
}


void Feed::print() const {
	Log << "Printing feed:" << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << title << (description != "" ? ": " : "") << description << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << author << (link != "" ? " @ " : "") << link << (Log.ENDL | Log.CONTINUE) << "    ";
	Log << "#" << id << Log.ENDL;
}
