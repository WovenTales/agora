#ifndef NCURSESUI_H
#define NCURSESUI_H

#include <database.hxx>
class NcursesArticlePanel;
class NcursesDatabasePanel;
class NcursesFeedPanel;
#include <ui/ncurses/panelbase.hxx>

#include <curses.h>
#include <math.h>
#include <string>
#include <vector>


//! \todo Generates large amount of leaked memory.
class NcursesUI {
  private:
	//! Allow static singleton without always initing blank window.
	NcursesUI(int) {};

	NcursesUI(const NcursesUI&);
	NcursesUI &operator=(const NcursesUI&);

	static NcursesUI              base;

	static std::vector<Database>  db;

	static NcursesArticlePanel   *articlePanel;
	static NcursesDatabasePanel  *dbPanel;
	static NcursesFeedPanel      *feedPanel;

	static NcursesPanel::Panel focus;

	static NcursesPanel *activePanel();

	static void close();

  public:
	//! \todo Allow startup without active database.
	NcursesUI(const std::string& = std::string(""));
	virtual ~NcursesUI();

	static NcursesPanel::Panel getFocus() { return focus; };

	static void changeTab(bool right) { if (activePanel()->changeTab(right)) draw(); };
	static void scrollTab(bool down) { if (activePanel()->scrollTab(down)) draw(); };
	static void setFocus(NcursesPanel::Panel);

	static void openArticle(const std::string&);
	static void openDatabase(const std::string&);
	static void openDatabase(Database&);
	static void openFeed(const std::string&);

	static void draw();
};

#endif
