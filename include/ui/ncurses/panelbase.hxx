#ifndef NCURSESPANEL_H
#define NCURSESPANEL_H

#include <database.hxx>
#include <logger.hxx>

#include <curses.h>
#include <string>
#include <vector>


//! \todo Smooth so don't take up more space than necessary (DVI shouldn't require resizing of panels)
//! \todo Rework as part of Panel classes rather than (harder-to-manage) defines
#define DBPANEL_WIDTH_ACTIVE       60
#define DBPANEL_WIDTH_MINIMIZED    std::max(20, std::min(COLS / 5, 60))
#define FEEDPANEL_HEIGHT_ACTIVE    20
#define FEEDPANEL_HEIGHT_MINIMIZED 10


//! Base class for all panels in the Ncurses interface
class NcursesPanel {
  public:
	//! Provide a way to reference each panel
	enum struct Panel {
		ArticlePanel,   //!< NcursesArticlePanel
		DatabasePanel,  //!< NcursesDatabasePanel
		FeedPanel       //!< NcursesFeedPanel
	};

  protected:
	/*! \warning Does not remain constant; copying pointed location is unsafe. */
	WINDOW                   *panel;  //!< The Ncurses window containing the panel
	/*! \todo Needs to be more complex - switching to database shouldn't
	 *        affect the vertical border between feed and article
	 */
	bool expanded;                    //!< Whether the panel is currently in focus

	std::vector<std::string>  tabs;   //!< A list of tabs open in the panel
	unsigned char activeTab;          //!< The index of the currently displayed tab

	//! Return the name of the panel for e.g. logging
	virtual std::string name() =0;
	//! Return the Panel representing this panel
	virtual Panel       indic() =0;

  public:
	//! Standard NcursesPanel constructor
	NcursesPanel(bool = false);
	//! Default destructor
	virtual ~NcursesPanel();

	/*! \todo Centralize so we don't need to rewrite for every panel */
	//! Get current height of panel
	virtual int height() =0;
	//! Get current width of panel
	virtual int width() =0;
	//! Get current x-position of panel
	virtual int x() =0;
	//! Get current y-position of panel
	virtual int y() =0;

	//! \return \copybrief activeTab
	unsigned char getActiveTab() { return activeTab; };

	//! Switch active tab
	bool changeTab(bool);
	//! Scroll tab content
	/*! \param down whether to scroll downward
	 *  \return whether the panel needs to be redrawn
	 */
	virtual bool scrollTab(bool down) =0;

	//! Draw panel to screen
	void draw();
	//! Draw content of active tab
	virtual void fill() =0;
	//! Update panel dimensions
	void update();
};

//! Base class for panels consisting of a navigable list
/*! \tparam T the Database::Table enum describing the displayed list \e elements
 */
template <typename T>
class NcursesListPanel : public NcursesPanel {
  protected:
	//! List of each element to display
	std::vector<Database::DataList<T> >  data;

	//! Index of the currently selected item
	unsigned char activeData;

	//! Column of the Database::DataList to display
	/*! \return the relevant Database::Table column
	 */
	virtual T line() =0;

/* public:
 * NcursesListPanel(bool = false);
 * virtual bool scrollTab(bool);
 * virtual void fill();
 */
#include <ui/ncurses/panelbase.tcc>
};

#endif
