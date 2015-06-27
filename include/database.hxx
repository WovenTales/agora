#ifndef DATABASE_H
#define DATABASE_H

#include <agora.hxx>
class Article;
class Feed;
#include <logger.hxx>

#include <algorithm>
#include <initializer_list>
#include <map>
#include <queue>
#include <sqlite3.h>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>


//! An abstraction for a database on file.
//! \todo Update so can have const Database instances (everything complains about exec not being const).
//! \todo Public \c exec exposes too much control; encapsulate with safer get methods.
class Database {
  public:
	struct Table {
	  public:
		friend class Database;

		enum struct Article {
			ID,
			FeedID,
			Title,
			Updated,
			Link,
			Author,
			Summary,
			Content
		};
		static std::string column(Article);
		static std::string table(Article) { return "articles"; };

		enum struct Feed {
			ID,
			Title,
			Link,
			Author,
			Description
		};
		static std::string column(Feed);
		static std::string table(Feed) { return "feeds"; };

		enum struct Meta {
			Title,
			Version
		};
		static std::string column(Meta);
		static std::string table(Meta) { return "meta"; };

		template <typename T> using Unified = T;

	  private:
		template <typename T>
		struct table_less {
			bool operator() (T a, T b) const { return (int(a) < int(b)); };
		};
		template < typename T,
			   typename MAPPED_TYPE,
			   typename ALLOCATOR_TYPE = std::allocator<std::pair<const T, MAPPED_TYPE> > >
		using table_map = std::map<T, MAPPED_TYPE, table_less<T>, ALLOCATOR_TYPE>;

		template <typename T>
		struct table_hash_eq {
			std::size_t operator() (T a) const { return std::hash<int>()(int(a)); };
			bool operator() (T a, T b) const { return (int(a) == int(b)); };
		};
		template < typename T,
			   typename MAPPED_TYPE,
			   typename ALLOCATOR_TYPE = std::allocator<std::pair<const T, MAPPED_TYPE> > >
		using table_hash_map = std::unordered_map<T, MAPPED_TYPE, table_hash_eq<T>, table_hash_eq<T>, ALLOCATOR_TYPE>;
	};

	//! Mappings of (columnName) -> (data), representing a lower-level representation of a returned database query
	template <typename T> using Data     = Table::table_hash_map<Table::Unified<T>, std::string>;
	template <typename T> using DataList = std::vector<Data<T> >;

	typedef Data<Table::Article> ArticleData;
	typedef Data<Table::Feed>    FeedData;
	typedef Data<Table::Meta>    MetaData;

	typedef DataList<Table::Article> ArticleDataList;
	typedef DataList<Table::Meta>    MetaDataList;
	typedef DataList<Table::Feed>    FeedDataList;

  private:
	Database &operator--();

	sqlite3 *db;

	static const Data<Table::Article> ArticleColumnName;
	static const Data<Table::Meta>    MetaColumnName;
	static const Data<Table::Feed>    FeedColumnName;

	std::queue<const Feed*> feedStage;
	std::queue<const Article*> articleStage;

	unsigned char &count;

	//! Close database.
	void close(bool = false);

	//! Execute a command on the database.
	void exec(const std::string&);

	// For use in sqlite3_exec() calls
	static int isEmpty(bool*, int, char*[], char*[]);

  public:
	//! Initialize without physical database.
	Database();
	//! Initialize to a particular file.
	Database(const std::string &);
	//! Copy constructor.
	Database(const Database&);
	//! Standard deconstructor.
	virtual ~Database();

	//! Standard assignment operator.
	Database &operator=(const Database&);

	//! Get title assigned to database.
	std::string getTitle() const;

	//! Request an Article by ID.
	Article        getArticle(const std::string&) const;
	//! Create Article from given data.
	static Article makeArticle(const ArticleData&);

	//! Open specified database file.
	void open(const std::string&);

	//! Stage a Feed for writing.
	void stage(const Feed&);
	//! Stage an Article for writing.
	void stage(const Article&);

	//! Clear all staged changes without saving.
	void clearStaged();
	//! Commit the staged changes to the file.
	void save();
	/*! \class Database
	 *  \todo Add method for removing and/or changing staged elements\n
	 *  Can likely best accomplish at same time as preventing multiple elements with same ID
	 */

// private: void clearStaged(std::queue<T*>&);
#include <database.tcc>
};

#endif
