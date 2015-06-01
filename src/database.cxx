#include <database.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <feed.hxx>

#include <iostream>
#include <string.h>

using namespace pugi;

Database::Database(const char *filename) {
	if (sqlite3_open(filename, &db) != SQLITE_OK) {
		//TODO: More robust error handling
		cout << "Bad database " << filename << endl;
	}

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
	}
}

Database::~Database() {
	sqlite3_close(db);
}

Article Database::getArticle(const string &id) {
	Article out;
	string cmd("SELECT * FROM articles where aID = '" + replaceAll(id, "'", "''") + "';");
	sqlite3_exec(db, cmd.c_str(), (int (*)(void*, int, char**, char**))makeArticle, &out, NULL);
	return out;
}

void Database::stage(const Feed &f) {
	feedStage.push(f);

	//TODO: Skips top feed if database already exists
	//  Continue to have problems wth that in general
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
	cout << "Staged " << count << " articles" << endl;
}

void Database::save() {
	unsigned int count = 0;
	string cmd("");
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
		cmd += "INSERT INTO feeds (" + cols + ") VALUES (" + vals + ");";
		count++;
	}
	cout << "Saved " << count << " feeds" << endl;

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
			cols += ", aContent";
			vals += ",'" + replaceAll(content, "'", "''") + "'";
		}

		//TODO: Only update what's necessary rather than replacing everything
		cmd += "INSERT INTO articles (" + cols + ") VALUES (" + vals + ");";

		delete a;
		count++;
	}
	cout << cmd << endl;
	exec(cmd);
	cout << "Saved " << count << " articles" << endl;
}


int Database::existed(bool *out, int cols, char *data[], char *colNames[]) {
	if (cols > 0) {
		*out = true;
	}
	return 0;
}

int Database::makeArticle(Article *a, int cols, char *vals[], char *names[]) {
	char *curName, *curVal;
	string id, feedID, title, author, content, link, summary;
	time_t updated;

	for (int i = 0; i < cols; i++) {
		curName = names[i];
		curVal = vals[i];

		if (curVal && !strcmp(curName, "aID")) {  // curVal != NULL && curName == "aID"
			id = curVal;
		} else if (curVal && !strcmp(curName, "fID")) {  // curVal != NULL && curName == "fID"
			feedID = string(curVal);
		} else if (curVal && !strcmp(curName, "aTitle")) {  // curVal != NULL && curName == "aTitle"
			title = string(curVal);
		} else if (curVal && !strcmp(curName, "aUpdated")) {  // curVal != NULL && curName == "aUpdated"
			updated = parseTime(string(curVal));
		} else if (curVal && !strcmp(curName, "aLink")) {  // curVal != NULL && curName == "aLink"
			link = string(curVal);
		} else if (curVal && !strcmp(curName, "aAuthor")) {  // curVal != NULL && curName == "aAuthor"
			author = string(curVal);
		} else if (curVal && !strcmp(curName, "aSummary")) {  // curVal != NULL && curName == "aSummary"
			summary = string(curVal);
		} else if (curVal && !strcmp(curName, "aContent")) {  // curVal != NULL && curName == "aContent"
			content = string(curVal);
		}
	}

	*a = Article(id, feedID, title, updated, author, content, link, summary);

	return 0;
}
