#ifndef NCURSESDATABASEPANEL_H
#define NCURSESDATABASEPANEL_H

class Database;
#include <ncursespanel.hxx>
#include <ncursesui.hxx>

#include <vector>


class NcursesDatabasePanel : public NcursesPanel {
  private:
	std::vector<std::string>                  dbs;
	std::vector<std::vector<Database::Data> > feeds;

	unsigned char index;

	virtual NcursesUI::Panel indic() { return NcursesUI::DatabasePanel; };

	void loadDatabase(Database&);

  public:
	NcursesDatabasePanel(bool = false);
	NcursesDatabasePanel(Database&, bool = false);

	virtual int height();
	virtual int width();

	virtual void draw();
};

#endif
