#ifndef NCURSESDATABASEPANEL_H
#define NCURSESDATABASEPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>


class NcursesDatabasePanel : public NcursesBasePanel<Database::Table::Feed> {
  private:
	virtual std::string           name()  { return "database"; };
	virtual Panel                 indic() { return Panel::DatabasePanel; };
	virtual Database::Table::Feed line()  { return Database::Table::Feed::Title; };

  public:
	NcursesDatabasePanel(bool focus = false) : NcursesBasePanel(focus) { update(focus); } ;

	virtual int height();
	virtual int width();
	virtual int x();
	virtual int y();

	void loadDatabase(const Database&);
};

#endif
