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


/*! \bug Generates large amount of leaked memory\n
 *       Found that articlePanel wasn't being deleted; retest
 */
class NcursesUI {
  private:
	//! Allow static singleton without always initializing blank window
	NcursesUI(int) {};

	//! Copy constructor
	NcursesUI(const NcursesUI&);
	//! Assignment operator
	NcursesUI &operator=(const NcursesUI&);

	/*! \todo There's really no rush to open anything */
	static NcursesUI              base;          //!< Singleton so will initialize when program starts

	//! The opened databases, in the same order as the tabs in NcursesUI::dbPanel
	static std::vector<Database>  db;

	static NcursesArticlePanel   *articlePanel;  //!< \copybrief NcursesUI::dbPanel
	static NcursesDatabasePanel  *dbPanel;       //!< Pointer to panel
	static NcursesFeedPanel      *feedPanel;     //!< \copybrief NcursesUI::dbPanel

	static NcursesPanel::Panel focus;            //!< Currently active panel

	//! \return Pointer to the focused panel
	static NcursesPanel *activePanel();

	//! Close the UI
	static void close();

  public:
	//! Constructor priming UI with a Database
	/*! \todo Allow startup without active database */
	NcursesUI(const std::string& = std::string(""));
	//! Standard destructor
	virtual ~NcursesUI();

	//! \return \copybrief focus
	static NcursesPanel::Panel getFocus() { return focus; };

	//! Change the tab of the currently active panel
	/*! \param right whether to change to the tab to the right
	 */
	static void changeTab(bool right) { if (activePanel()->changeTab(right)) draw(); };
	//! Scroll the content of the currently active panel
	/*! \param down whether to scroll downward
	 */
	static void scrollTab(bool down) { if (activePanel()->scrollTab(down)) draw(); };
	//! Move the focus to the specified panel
	static void setFocus(NcursesPanel::Panel);

	//! Opens the specified article from the currently displayed database
	static void openArticle(const std::string&);
	//! \copybrief openDatabase(Database&)
	static void openDatabase(const std::string&);
	//! Loads the specified Database into the UI
	static void openDatabase(Database&);
	//! Opens the specfied feed from the currently displayed database
	static void openFeed(const std::string&);

	//! Draw the UI to the screen
	static void draw();
};

#endif
