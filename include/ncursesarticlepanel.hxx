#ifndef NCURSESARTICLEPANEL_H
#define NCURSESARTICLEPANEL_H

#include <database.hxx>
#include <ncursespanel.hxx>

#include <string>
#include <vector>


class NcursesArticlePanel : public NcursesListPanel<Database::Table::Article> {
  private:
	std::vector<std::string> content;

	virtual std::string              name()  { return "article"; };
	virtual Panel                    indic() { return Panel::ArticlePanel; };
	virtual Database::Table::Article line()  { return Database::Table::Article::Content; };

  public:
	NcursesArticlePanel(bool focus = false) : NcursesListPanel(focus) { update(); } ;

	virtual inline int height();
	virtual inline int width();
	virtual inline int x();
	virtual inline int y();

	virtual void fill();

	void loadArticle(const Database&, const std::string&);
};

#endif
