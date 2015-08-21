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


/*! \todo Convert HTML to formatted text (check http://www.blackbeltcoder.com/Articles/strings/convert-html-to-text).
 *  \todo Split for loop into other function, and only call in update.
 *  \bug  Correctly handle unicode.
 */
void NcursesArticlePanel::fill() {
	int s = content.size();

	vector<string> split;
	int y = 1;
	for (string line : content) {
		if (y >= height()) {
			//! \todo Could definitely look better.
			mvwaddch(panel, height() - 2, width() - 1, ACS_UARROW);
			mvwaddch(panel, height() - 1, width() - 1, ACS_DARROW);
			break;
		}

		//! \todo Implement scrollable view.
		if (line.size() <= width() - 3) {
			mvwaddstr(panel, y, 2, line.c_str());
		} else {
			split = splitString(line, ' ');
			int c = split.size();

			line = "";
			for (int j = 0; j + 1 < c; ++j) {
				if (line.empty()) {
					line = split[j];
				} else {
					line += " " + split[j];
				}

				if ((line.size() + split[j + 1].size() + 1) > width() - 3) {
					mvwaddstr(panel, y, 2, line.c_str());
					line = "";
					++y;
				}

				if (y == height()) {
					break;
				}
			}

			if (y < height()) {
				mvwaddstr(panel, y, 2, (line + (line.empty() ? "" : " ") + split[c - 1]).c_str());
			}
		}

		++y;
	}
}


void NcursesArticlePanel::loadArticle(const Database &db, const std::string &aID) {
	Database::ArticleDataList t = db.getColumns<Database::Table::Article>({ Database::Table::Article::Title }, {{ Database::Table::Article::ID, aID }});

	if (t.size()) {
		tabs.push_back(t[0][Database::Table::Article::Title]);
		content = splitString(db.getArticle(aID).getContent());

		++activeTab;
	}
}
