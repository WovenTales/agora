#include <article.hxx>

#include <database.hxx>

#include <iostream>
#include <string.h>

using namespace pugi;

Article::Article() {
	id = "";
	feedID = "";
	title = "";
	updated = Database::parseTime("");
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
	id = entry.child_value("id");
	title = Database::parseAtomTitle(entry.child("title"));

	// pugixml gives empty string if node doesn't exist
	author = entry.child("author").child_value("name");
	content = entry.child_value("content");
	link = entry.child_value("link");
	summary = entry.child_value("summary");

	if (!link.compare("") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {
		link = id;
	}
}

void Article::parseRss(const xml_node &entry) {
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

	if (!link.compare("") && strcmp(entry.child("guid").attribute("isPermaLink").value(), "false") && (!id.compare(0, 7, "http://") || !id.compare(0, 8, "https://"))) {  // link == "" && isPermaLink != "false" && (id[0..7] == "http://" || id[0..8] == "https://")
		link = id;
	}
}

const void Article::save(Database &db) {
	string insert("INSERT INTO articles (");
	string update;
	string cols("aID, fID");
	string vals("'" + replaceAll(id, "'", "''") + "','" + replaceAll(feedID, "'", "''") + "'");

	if (title.compare("")) {  // title != ""
		cols += ", aTitle";
		vals += ",'" + replaceAll(title, "'", "''") + "'";
	}
	if (link.compare("")) {  // link != ""
		cols += ", aLink";
		vals += ",'" + replaceAll(link, "'", "''") + "'";
	}
	if (author.compare("")) {  // author != ""
		cols += ", aAuthor";
		vals += ",'" + replaceAll(author, "'", "''") + "'";
	}
	if (summary.compare("")) {  // summary != ""
		cols += ", aSummary";
		vals += ",'" + replaceAll(summary, "'", "''") + "'";
	}
	if (content.compare("")) {  // content != ""
		//TODO: Inserting content into non-cout stream skips articles in for loop (in Feed::save)
		//cols += ", aContent";
		//vals += ",'" + replaceAll(content, "'", "''") + "'";
		//update = "UPDATE articles SET aContent = '" + content + "' WHERE aID = '" + replaceAll(id, "'", "''") + "'";
	}

	//TODO: Only update what's necessary
	insert += cols + ") VALUES (" + vals + ");";
	cout << insert << endl;
	//cout << update << endl;
	db.exec(insert);
}

const void Article::print() {
	cout << title << " (#" << id << ")" << endl;
	cout << author << " @ " << link << endl;
	cout << summary << endl;
	cout << content << endl;
}
