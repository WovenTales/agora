#include <database.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <feed.hxx>
#include <logger.hxx>

#include <iostream>
#include <sstream>
#include <string.h>

using namespace agora;
using namespace pugi;

Database::Database(const char *filename) {
	ostringstream msg("");
	msg << "Opening database '" << filename << "'...";
	Logger::log(msg.str(), Logger::CONTINUE);

	if (sqlite3_open(filename, &db) != SQLITE_OK) {
		//TODO: More robust error handling
		cout << "Bad database " << filename << endl;
	}

	Logger::log("Completed");

	bool old = false;
	sqlite3_exec(db, "PRAGMA table_info(feeds)", (int (*)(void*, int, char**, char**))existed, &old, NULL);

	if (!old) {
		sqlite3_exec(db, "CREATE TABLE feeds (fID        NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                      "fTitle,"
		                                      "fUpdated,"
		                                      "fLink,"
		                                      "fAuthor,"
		                                      "fDesc);"
				 "CREATE TABLE articles (aID       NOT NULL PRIMARY KEY ON CONFLICT REPLACE,"
		                                        "fID,"
		                                        "aTitle,"
		                                        "aUpdated,"
		                                        "aLink,"
		                                        "aAuthor,"
		                                        "aSummary,"
		                                        "aContent);",
		             NULL, NULL, NULL);
		Logger::log("New database initialized");
	}
}

Database::~Database() {
	sqlite3_close(db);
}

Article Database::getArticle(const string &id) {
	Logger::log("Requesting article with id '" + id + "'");

	Article out;
	string cmd("SELECT * FROM articles WHERE aID = '" + replaceAll(id, "'", "''") + "';");
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))makeArticle, &out, NULL);

	return out;
}

void Database::stage(Article *a) {
	Logger::log("Staging article '" + a->getTitle() + "'");
	articleStage.push(a);
}
void Database::stage(const Feed &f) {
	Logger::log("Staging feed '" + f.getTitle() + "'");

	feedStage.push(f);

	string id = f.getID();
	FeedLang lang = f.getLang();
	xml_node root = f.getRoot();
	const char *tag = (lang == ATOM ? "entry" : "item");
	unsigned int count = 0;

	for (xml_node entry = root.child(tag);
	     entry.type(); // != NULL
	     entry = entry.next_sibling(tag)) {
		count++;
		stage(new Article(entry, id, lang));
	}

	ostringstream msg("");
	msg << "Staged " << count << " articles from feed";
	Logger::log(msg.str());
}

void Database::save() {
	Logger::log("Committing staged elements");

	unsigned int count = 0;
	string insert("");
	string cols("");
	string vals("");

	while (!feedStage.empty()) {
		Feed f = feedStage.front();
		feedStage.pop();

		string id = f.getID();
		string title = f.getTitle();
		string link = f.getLink();
		string author = f.getAuthor();
		string description = f.getDescription();

		Logger::log("Generating command for feed '" + title + "'...", Logger::CONTINUE);

		cols = "fID";
		vals = "'" + replaceAll(id, "'", "''") + "'";

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

		//TODO: Only update what's necessary rather than replacing everything
		insert += "INSERT INTO feeds (" + cols + ") VALUES (" + vals + ");";
		count++;

		Logger::log("Completed");
	}
	ostringstream msgFeed("");
	msgFeed << "Found " << count << " feeds";
	Logger::log(msgFeed.str());

	count = 0;
	while (!articleStage.empty()) {
		Article *a = articleStage.front();
		articleStage.pop();

		string id = a->getID();
		string feedID = a->getFID();
		string title = a->getTitle();
		string link = a->getLink();
		string author = a->getAuthor();
		string summary = a->getSummary();
		string content = a->getContent();

		Logger::log("Generating command for article '" + title + "'...", Logger::CONTINUE);

		cols = "aID, fID";
		vals = "'" + replaceAll(id, "'", "''") + "','" + replaceAll(feedID, "'", "''") + "'";

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
			//TODO: Trying to insert (or later update with) content results in command being ignored
			//  When updated manually, get/makeArticle properly displays, so is likely buffer error of some sort
			cols += ", aContent";
			//vals += ",'" + replaceAll(content, "'", "''") + "'";
			vals += ",'SKIPPED'";
		}

		//TODO: Only update what's necessary rather than replacing everything
		insert += "INSERT INTO articles (" + cols + ") VALUES (" + vals + ");";

		delete a;
		count++;

		Logger::log("Completed");
	}
	ostringstream msgArticle("");
	msgArticle << "Found " << count << " articles";
	Logger::log(msgArticle.str());

	exec(insert);

	Logger::log("Saved staged elements");
}


int Database::existed(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols > 0) {
		*out = true;
	}
	return 0;
}

int Database::makeArticle(Article *a, int cols, char *vals[], char *names[]) {
	Logger::log("Generating article...", Logger::CONTINUE);

	char *curName, *curVal;
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	for (int i = 0; i < cols; i++) {
		curName = names[i];
		curVal = vals[i];

		if (curVal && !strcmp(curName, "aID")) {  // curVal != "" && curName == "aID"
			id = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "fID")) {  // curVal != "" && curName == "fID"
			feedID = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "aTitle")) {  // curVal != "" && curName == "aTitle"
			title = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "aUpdated")) {  // curVal != "" && curName == "aUpdated"
			updated = parseTime(string(curVal));
		} else if (curVal && !strcmp(curName, "aLink")) {  // curVal != "" && curName == "aLink"
			link = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "aAuthor")) {  // curVal != "" && curName == "aAuthor"
			author = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "aSummary")) {  // curVal != "" && curName == "aSummary"
			summary = replaceAll(curVal, "''", "'");
		} else if (curVal && !strcmp(curName, "aContent")) {  // curVal != "" && curName == "aContent"
			content = replaceAll(curVal, "''", "'");
		}
	}

	*a = Article(id, feedID, title, updated, author, content, link, summary);

	Logger::log("Completed generating '" + title + "'");

	return 0;
}
