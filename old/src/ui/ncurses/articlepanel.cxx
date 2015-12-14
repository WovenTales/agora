#include <ui/ncurses/articlepanel.hxx>

#include <agora.hxx>
#include <article.hxx>
#include <database.hxx>
#include <ui/ncurses/ui.hxx>

#include <curses.h>

using namespace agora;
using namespace std;


int NcursesArticlePanel::height() {
	return LINES - y();
}

int NcursesArticlePanel::width() {
	return COLS - x();
}

int NcursesArticlePanel::x() {
	return (NcursesUI::getFocus() == Panel::DatabasePanel ? DBPANEL_WIDTH_ACTIVE : DBPANEL_WIDTH_MINIMIZED);
}

int NcursesArticlePanel::y() {
	return (NcursesUI::getFocus() == Panel::FeedPanel ? FEEDPANEL_HEIGHT_ACTIVE : FEEDPANEL_HEIGHT_MINIMIZED);
}


/*! \todo Implement
 */
bool NcursesArticlePanel::scrollTab(bool down) {
	return false;
}


/*! \todo Convert HTML to formatted text (check http://www.blackbeltcoder.com/Articles/strings/convert-html-to-text)
 *  \todo Split for loop into other function, and only call in update\n
 *        This would probably best be handled by inserting control characters at line
 *        breakpoints for each width, and saving it as a single string
 */
void NcursesArticlePanel::fill() {
	int s = content.size();

	vector<string> split;
	int y = 1;
	int h = height() - 1;
	int w = width() - 4;
	for (string line : content) {
		// Draw arrows if content exceeds window
		if (y >= h) {
			//! \todo Could definitely look better
			mvwaddch(panel, height() - 2, width() - 1, ACS_UARROW);
			mvwaddch(panel, height() - 1, width() - 1, ACS_DARROW);
			break;
		}

		//! \todo Implement scrollable view
		if (line.size() <= w) {  // If entire line can fit in window
			mvwaddstr(panel, y, 2, line.c_str());
		} else {
			// Split string into individual words
			split = splitString(line, ' ');
			int c = split.size() - 1;

			// Reusing line here as old value now in split
			line = "";
			for (int j = 0; j < c; ++j) {
				if (line.empty()) {
					line = split[j];
				} else {
					line += " " + split[j];
				}

				// If the line plus the next word does not fit in the window
				if ((line.size() + split[j + 1].size() + 1) > w) {
					if (line.size() > w) {
						// Current line may still be longer if it's a single word
						mvwaddnstr(panel, y, 2, line.c_str(), w - 3);
						waddstr(panel, "...");
					} else {
						mvwaddstr(panel, y, 2, line.c_str());
					}

					// Setup for remaining part of split
					line = "";
					++y;
				}

				// Ensure we don't spill out the bottom
				if (y == h) {
					break;
				}
			}

			// Add the last word and print if we have space
			if (y < h) {
				mvwaddstr(panel, y, 2, (line + (line.empty() ? "" : " ") + split[c]).c_str());
			}
		}

		++y;
	}
}


/*! \param db  the Database containing the desired article
 *  \param aID the article ID to search for
 */
void NcursesArticlePanel::loadArticle(const Database &db, const std::string &aID) {
	// We only need to display some of the data
	Database::DataList t = db.getColumns({ Article::columns["title"] }, {{ Article::columns.getID(), aID }});

	//! \todo Is this really the test we want?
	if (t.size()) {  // t.size != 0
		tabs.push_back(t[0][Article::columns["title"]]);
		//! \todo Data retrieval methods are inconsistent
		content = splitString(db.getArticle(aID).getContent());

		++activeTab;
	}
}
