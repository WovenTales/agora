#include "feed.hxx"

using namespace std;
using namespace pugi;

Feed::Feed(char *filename) {
	xml_document feed;
	xml_parse_result parsedFeed = feed.load_file(filename);
	initialize(feed);
}

void Feed::initialize(xml_document &root) {	
	xml_node rss = root.child("rss").child("channel");
	xml_node feed = root.child("feed");

	if (rss.type() != node_null) {
		parseRss(rss);

		for (xml_node entry = rss.child("item"); entry.type() != node_null; entry = entry.next_sibling("item")) {
			Entry e(entry, RSS);
			e.print();
		}
	} else if ((feed.type() != node_null) && !strcmp(feed.attribute("xmlns").value(), "http://www.w3.org/2005/Atom")) {
		parseAtom(feed);

		for (xml_node entry = feed.child("entry"); entry.type() != node_null; entry = entry.next_sibling("entry")) {
			Entry e(entry, ATOM);
			e.print();
		}
	} else {
		//TODO: Better error handling
		cerr << "Invalid feed format" << endl;
	}
}

void Feed::parseAtom(xml_node &feed) {
	lang = ATOM;

	id = feed.child_value("id");
	title = parseAtomTitle(feed.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = feed.child("author").child_value("name");
	link = feed.child("link").attribute("href").value();
	description = parseAtomTitle(feed.child("subtitle"));

	if (!link.compare("") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}
}

void Feed::parseRss(xml_node &feed) {
	//TODO: Specifically, RSS 2.0; include support for older standards.
	lang = RSS;

	description = feed.child_value("description");
	id = link = feed.child_value("link");
	title = feed.child_value("title");
}


void Feed::print() {
	cout << title << (description != "" ? ": " : "") << description << endl;
	cout << "  " << author << endl;
	cout << "  #" << id << endl;
}


#ifdef DEBUG_TEST_FEED
int main(int argc, char *argv[]) {
	Feed f(argv[1]);
	f.print();
}
#endif
