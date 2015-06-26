#ifndef NCURSESDATABASEPANEL_H
#define NCURSESDATABASEPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>


class NcursesDatabasePanel : public NcursesPanel {
  private:
	virtual std::string            name()  { return "database"; };
	virtual Panel                  indic() { return Panel::DatabasePanel; };
	virtual Database::Column::Name line()  { return Database::Column::FeedTitle; };

  public:
	NcursesDatabasePanel(bool max = false) : NcursesPanel(max) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	void loadDatabase(const Database&);
};

#endif
