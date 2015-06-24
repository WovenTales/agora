#ifndef NCURSESDATABASEPANEL_H
#define NCURSESDATABASEPANEL_H

class Database;
#include <ncursespanel.hxx>
#include <ncursesui.hxx>


class NcursesDatabasePanel : public NcursesPanel {
  private:
	virtual std::string      name()  { return "database"; };
	virtual NcursesUI::Panel indic() { return NcursesUI::DatabasePanel; };

  public:
	NcursesDatabasePanel(bool max = false) : NcursesPanel(max) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	virtual void fill();

	void loadDatabase(const Database&);
};

#endif
