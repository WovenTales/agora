#include "entry.hxx"

using namespace pugi;

Entry::Entry(xml_node &entry, FeedLang lang) {
	if (lang == ATOM) {
		parseAtom(entry);
	} else if (lang == RSS) {
		parseRss(entry);
	} else {
		cerr << "Invalid language for parsing" << endl;
	}
}

void Entry::parseAtom(xml_node &entry) {
	id = entry.child_value("id");
	title = Feed::parseAtomTitle(entry.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = entry.child("author").child_value("name");
	content = entry.child_value("content");
	link = entry.child_value("link");
	summary = entry.child_value("summary");

	if (!link.compare("") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}
}

void Entry::parseRss(xml_node &entry) {
	id = entry.child_value("guid");
	link = entry.child_value("link");
	summary = entry.child_value("description");
	title = entry.child_value("title");

	if (!id.compare("") && (link.compare("") != 0)) {
		link = id;
	}
	if (!link.compare("") && (strcmp(entry.child("guid").attribute("isPermaLink").value(), "false") != 0) && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}
}

void Entry::print() {
	cout << title << " (#" << id << ")" << endl;
	cout << author << " @ " << link << endl;
	cout << summary << endl;
	//cout << content << endl;
}
