#ifndef NCURSESARTICLEPANEL_H
#define NCURSESARTICLEPANEL_H

#include <database.hxx>
#include <ui/ncurses/panelbase.hxx>

#include <string>
#include <vector>


//! Displays the content of an Article
class NcursesArticlePanel : public NcursesPanel {
  private:
	//! List of content for each tab
	std::vector<std::string> content;

	virtual std::string name()  { return "article"; };
	virtual Panel       indic() { return Panel::ArticlePanel; };

  public:
	//! \copydoc NcursesPanel(bool)
	NcursesArticlePanel(bool focus = false) : NcursesPanel(focus) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	virtual bool scrollTab(bool);

	virtual void fill();

	//! Open an Article in a new tab
	void loadArticle(const Database&, const std::string&);
};

#endif
