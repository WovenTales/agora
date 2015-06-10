#include <database.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <feed.hxx>
#include <logger.hxx>

#include <iostream>
#include <string.h>

using namespace agora;
using namespace pugi;
using namespace std;

/*! \param filename the SQLite3 database to associate this Database with
 */
Database::Database(std::string filename) {
	Logger::log("Opening database '" + filename + "'...", Logger::CONTINUE);

	if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
		//! \todo More robust error handling
		cout << "Bad database " << filename << endl;
	}

	Logger::log("Completed");

	bool init = true;
	sqlite3_exec(db, "PRAGMA table_info(feeds)", (int (*)(void*, int, char**, char**))isEmpty, &init, NULL);

	if (init) {
		// Initialize database
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
	save();
	sqlite3_close(db);
}

/*! If database doesn't contain given ID, returns Article with default values
 *  (mostly empty strings) for all members.
 *
 *  \param id article ID
 *  \return New Article with values according to given ID
 */
Article Database::getArticle(const std::string &id) {
	Logger::log("Requesting article with id '" + id + "'");

	Article out;
	string cmd("SELECT * FROM articles WHERE aID = '" + replaceAll(id, "'", "''") + "';");
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))makeArticle, &out, NULL);

	return out;
}

/*! \param a the article to stage
 */
void Database::stage(Article a) {
	// Create a new instance so user doesn't have to worry about scope
	Article *p = new Article(a);
	Logger::log("Staging article '" + p->getTitle() + "'");

	articleStage.push(p);
}
/*! \param f the feed to stage
 */
void Database::stage(Feed f) {
	// Create a new instance so user doesn't have to worry about scope
	Feed *p = new Feed(f);
	Logger::log("Staging feed '" + p->getTitle() + "'");

	feedStage.push(p);

	string id = p->getID();
	FeedLang lang = p->getLang();
	xml_node root = p->getRoot();
	const char *tag = (lang == ATOM ? "entry" : "item");
	unsigned int count = 0;

	for (xml_node entry = root.child(tag);
	     entry.type(); // != NULL
	     entry = entry.next_sibling(tag)) {
		stage(*new Article(entry, id, lang));
		count++;
	}

	Logger::log("Staged ", Logger::CONTINUE);
	Logger::log(count, Logger::CONTINUE);
	Logger::log(" articles from feed");
}

void Database::save() {
	Logger::log("Committing staged elements");

	unsigned int count = 0;
	string insert("");
	string cols("");
	string vals("");

	while (!feedStage.empty()) {
		const Feed *f = feedStage.front();
		feedStage.pop();

		string id          = replaceAll(f->getID(), "'", "''");
		string title       = replaceAll(f->getTitle(), "'", "''");
		string link        = replaceAll(f->getLink(), "'", "''");
		string author      = replaceAll(f->getAuthor(), "'", "''");
		string description = replaceAll(f->getDescription(), "'", "''");

		Logger::log("Generating command for feed '" + title + "'...", Logger::CONTINUE);

		cols = "fID";
		vals = "'" + id + "'";

		if (title.compare("")) {  // title != ""
			cols += ", fTitle";
			vals += ",'" + title + "'";
		}
		if (link.compare("")) {  // link != ""
			cols += ", fLink";
			vals += ",'" + link + "'";
		}
		if (author.compare("")) {  // author != ""
			cols += ", fAuthor";
			vals += ",'" + author + "'";
		}
		if (description.compare("")) {  // description != ""
			cols += ", fDesc";
			vals += ",'" + description + "'";
		}

		//! \todo Only update what's necessary rather than replacing everything
		insert += "INSERT INTO feeds (" + cols + ") VALUES (" + vals + ");";
		count++;

		Logger::log("Completed");
	}
	Logger::log("Found ", Logger::CONTINUE);
	Logger::log(count, Logger::CONTINUE);
	Logger::log(" feeds");

	count = 0;
	while (!articleStage.empty()) {
		const Article *a = articleStage.front();
		articleStage.pop();

		string id      = replaceAll(a->getID(), "'", "''");
		string feedID  = replaceAll(a->getFID(), "'", "''");
		string title   = replaceAll(a->getTitle(), "'", "''");
		string link    = replaceAll(a->getLink(), "'", "''");
		string author  = replaceAll(a->getAuthor(), "'", "''");
		string summary = replaceAll(a->getSummary(), "'", "''");
		string content = replaceAll(a->getContent(), "'", "''");

		Logger::log("Generating command for article '" + title + "'...", Logger::CONTINUE);

		cols = "aID, fID";
		vals = "'" + id + "','" + feedID + "'";

		if (title.compare("")) {  // title != ""
			cols += ", aTitle";
			vals += ",'" + title + "'";
		}
		if (link.compare("")) {  // link != ""
			cols += ", aLink";
			vals += ",'" + link + "'";
		}
		if (author.compare("")) {  // author != ""
			cols += ", aAuthor";
			vals += ",'" + author + "'";
		}
		if (summary.compare("")) {  // summary != ""
			cols += ", aSummary";
			vals += ",'" + summary + "'";
		}
		if (content.compare("")) {  // content != ""
			//! \bug Trying to insert (or later update with) content results in command being ignored\n
			//! When updated manually, get/makeArticle properly displays, so is likely buffer error of some sort
			cols += ", aContent";
			//vals += ",'" + content + "'";
			vals += ",'SKIPPED'";
		}

		//! \todo Only update what's necessary rather than replacing everything
		insert += "INSERT INTO articles (" + cols + ") VALUES (" + vals + ");";

		delete a;
		count++;

		Logger::log("Completed");
	}
	Logger::log("Found ", Logger::CONTINUE);
	Logger::log(count, Logger::CONTINUE);
	Logger::log(" articles");

	exec(insert);

	Logger::log("Saved staged elements");
}


int Database::isEmpty(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols == 0) {
		*out = true;
	} else {
		*out = false;
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
		curVal = replaceAll(vals[i], "''", "'");

		if (curVal && !strcmp(curName, "aID")) {  // curVal != "" && curName == "aID"
			id = curVal;
		} else if (curVal && !strcmp(curName, "fID")) {  // curVal != "" && curName == "fID"
			feedID = curVal;
		} else if (curVal && !strcmp(curName, "aTitle")) {  // curVal != "" && curName == "aTitle"
			title = curVal;
		} else if (curVal && !strcmp(curName, "aUpdated")) {  // curVal != "" && curName == "aUpdated"
			updated = parseTime(string(curVal));
		} else if (curVal && !strcmp(curName, "aLink")) {  // curVal != "" && curName == "aLink"
			link = curVal;
		} else if (curVal && !strcmp(curName, "aAuthor")) {  // curVal != "" && curName == "aAuthor"
			author = curVal;
		} else if (curVal && !strcmp(curName, "aSummary")) {  // curVal != "" && curName == "aSummary"
			summary = curVal;
		} else if (curVal && !strcmp(curName, "aContent")) {  // curVal != "" && curName == "aContent"
			content = curVal;
		}
	}

	*a = Article(id, feedID, title, link, updated, author, content, summary);

	Logger::log("Completed generating '" + title + "'");

	return 0;
}
