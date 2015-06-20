#ifndef NCURSESUI_H
#define NCURSESUI_H

#include <database.hxx>
class NcursesDatabasePanel;

#include <string>


//! \todo Generates large amount of leaked memory.
class NcursesUI {
  public:
	enum Panel {
		DatabasePanel,
		None
	};

  private:
	//! Allow static singleton without always initing blank window.
	NcursesUI(int) {};

	NcursesUI(const NcursesUI&);
	Database &operator=(const Database&);

	static NcursesUI             base;

	static Database             *db;
	//! \todo Try to get working without pointer?
	static NcursesDatabasePanel *feedlist;

	static Panel focus;

	static void close();

  public:
  	//! \todo Allow startup without active database.
	NcursesUI(const std::string& = std::string(""));
	virtual ~NcursesUI();

	static Panel getFocus() { return focus; };

	static void toggleFocus();

	static void draw();
};

#endif
