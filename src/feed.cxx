#include <feed.hxx>

#include <article.hxx>
#include <database.hxx>

#include <iostream>
#include <string.h>

using namespace std;
using namespace pugi;

Feed::Feed(const char *filename) {
	xml_document feed;
	xml_parse_result parsedFeed = feed.load_file(filename);
	initialize(feed);
}

void Feed::initialize(const xml_document &d) {	
	xml_node rss = d.child("rss").child("channel");
	xml_node feed = d.child("feed");

	if (rss.type()) {
		parseRss(rss);
	} else if (feed.type() && !strcmp(feed.attribute("xmlns").value(), "http://www.w3.org/2005/Atom")) {
		parseAtom(feed);
	} else {
		//TODO: Better error handling
		cerr << "Invalid feed format" << endl;
	}
}

void Feed::parseAtom(const xml_node &feed) {
	lang = ATOM;
	root = feed;

	id = feed.child_value("id");
	title = Database::parseAtomTitle(feed.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = feed.child("author").child_value("name");
	link = feed.child("link").attribute("href").value();
	description = Database::parseAtomTitle(feed.child("subtitle"));

	if (!link.compare("") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}
}

void Feed::parseRss(const xml_node &feed) {
	//TODO: Specifically, RSS 2.0; include support for older standards.
	lang = RSS;
	root = feed;

	description = feed.child_value("description");
	id = link = feed.child_value("link");
	title = feed.child_value("title");

	if (id.compare("")) {  // id != ""
		//TODO: Generate unique id
	}
}

const void Feed::save(Database &db) {
	string cmd("INSERT INTO feeds (");
	string cols("fID");
	string vals("'" + replaceAll(id, "'", "''") + "'");

	if (title.compare("")) {  // title != ""
		cols += ", fTitle";
		vals += ",'" + replaceAll(title, "'", "''") + "'";
	}
	if (link.compare("")) {  // link != ""
		cols += ", fLink";
		vals += ",'" + replaceAll(link, "'", "''") + "'";
	}
	if (author.compare("")) {  // author != ""
		cols += ", fAuthor";
		vals += ",'" + replaceAll(author, "'", "''") + "'";
	}
	if (description.compare("")) {  // description != ""
		cols += ", fDesc";
		vals += ",'" + replaceAll(description, "'", "''") + "'";
	}

	//TODO: Only update what's necessary
	cmd += cols + ") VALUES (" + vals + ");";
	db.exec(cmd);
	cout << cmd << endl;

	//TODO: Skips top feed if database already exists
	//  Continue to have problems wth that in general
	const char *tag = (lang == ATOM ? "entry" : "item");
	unsigned int count = 0;
	for (xml_node entry = root.child(tag);
	     entry.type(); // != NULL
	     entry = entry.next_sibling(tag)) {
		Article a(entry, id, lang);
		a.save(db);
		count++;
	}
	cout << count << " articles" << endl;
}

const void Feed::print() {
	cout << title << (description != "" ? ": " : "") << description << endl;
	cout << "  " << author << endl;
	cout << "  #" << id << endl;
}
