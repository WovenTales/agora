#ifndef NCURSESARTICLEPANEL_H
#define NCURSESARTICLEPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>
#include <vector>


class NcursesArticlePanel : public NcursesPanel {
  private:
	std::vector<std::string> content;

	virtual std::string            name()  { return "article"; };
	virtual Panel                  indic() { return Panel::ArticlePanel; };
	virtual Database::Column::Name line()  { return Database::Column::ArticleContent; };

  public:
	NcursesArticlePanel(bool max = false) : NcursesPanel(max) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	virtual void fill();

	void loadArticle(const Database&, const std::string&);
};

#endif
