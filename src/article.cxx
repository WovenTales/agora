#include <article.hxx>

#include <logger.hxx>

#include <iostream>
#include <sstream>
#include <string.h>

using namespace pugi;

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
Article::Article(const xml_node &entry, string fID, const FeedLang lang) {
	feedID = fID;

	if (lang == ATOM) {
		parseAtom(entry);
	} else if (lang == RSS) {
		parseRss(entry);
	} else {
		cerr << "Invalid language for parsing" << endl;
	}
}
Article::Article(string i, string f, string t, time_t u, string a, string c, string l, string s) {
	id = i;
	feedID = f;
	title = t;
	updated = u;
	author = a;
	content = c;
	link = l;
	summary = s;
}

void Article::parseAtom(const xml_node &entry) {
	Logger::log("Parsing article as Atom...", Logger::CONTINUE);

	id = entry.child_value("id");
	title = parseAtomTitle(entry.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = entry.child("author").child_value("name");
	content = entry.child_value("content");
	link = entry.child_value("link");
	summary = entry.child_value("summary");

	if (!link.compare("") &&  // link == "" &&
	    (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {  // id[0..7] == "http://" || id[0..8] == "https://"
		link = id;
	}

	Logger::log("Completed parsing " + title);
}

void Article::parseRss(const xml_node &entry) {
	Logger::log("Parsing article as RSS...", Logger::CONTINUE);

	id = entry.child_value("guid");
	link = entry.child_value("link");
	summary = entry.child_value("description");
	title = entry.child_value("title");

	if (!id.compare("") && link.compare("")) {  // id == "" && link != ""
		id = link;
	}
	if (!id.compare("")) {  // id == ""
		//TODO: Generate unique id
	}

	if (!link.compare("") &&  // link == "" &&
	    strcmp(entry.child("guid").attribute("isPermaLink").value(), "false") &&  // isPermaLink != "false" &&
	    (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {  // id[0..7] == "http://" || id[0..8] == "https://"
		link = id;
	}

	Logger::log("Completed parsing " + title);
}

void Article::print() const {
	cout << title << " (#" << id << ")" << endl;
	cout << author << " @ " << link << endl;
	cout << summary << endl;
	cout << content << endl;
}
