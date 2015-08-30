#ifndef NCURSESDATABASEPANEL_H
#define NCURSESDATABASEPANEL_H

#include <database.hxx>
#include <ui/ncurses/panelbase.hxx>

#include <string>


//! Lists each Feed contained in a Database
class NcursesDatabasePanel : public NcursesListPanel<Database::Table::Feed> {
  private:
	virtual std::string           name()  { return "database"; };
	virtual Panel                 indic() { return Panel::DatabasePanel; };
	virtual Database::Table::Feed line()  { return Database::Table::Feed::Title; };

  public:
	//! \copydoc NcursesPanel(bool)
	NcursesDatabasePanel(bool focus = false) : NcursesListPanel(focus) { update(); } ;

	virtual int height();
	virtual int width();
	virtual int x();
	virtual int y();

	//! Open a Database in a new tab
	void loadDatabase(const Database&);
};

#endif
