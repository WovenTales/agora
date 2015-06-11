#ifndef NCURSESUI_H
#define NCURSESUI_H

class NcursesUI {
  private:
	static NcursesUI singleton;

  public:
	NcursesUI();
	virtual ~NcursesUI();

	static void close();
};

#endif
