
#include <gtest/gtest.h>

#include "SqliteBack.h"
#include "boost/lexical_cast.hpp"
#include <boost/uuid/uuid_io.hpp>

static std::string const path = "testdb.sqlite";

class FlushCatcher: public SqliteBack {
  public:
    FlushCatcher(std::string path) : SqliteBack(path) { };
    StrList cmds;

  protected:
    virtual void flush() {
      cmds.insert(cmds.end(), cmds_.begin(), cmds_.end());
      SqliteBack::flush();
    };
};

class FileDeleter {
  public:
    FileDeleter(std::string path) {
      path_ = path;
    };

    ~FileDeleter() {
      remove(path_.c_str());
    };

  private:
    std::string path_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(SqliteBackTest, CmdGenRegression) {
  FileDeleter fd(path);
  EventManager m;
  FlushCatcher back(path);
  m.registerBackend(&back);

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("monkey"))
   ->addVal("weight", 10)
   ->addVal("height", 5.5)
   ->record();

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("elephant"))
   ->addVal("weight", 1000)
   ->record();

  m.newEvent("DumbTitle")
   ->addVal("animal", std::string("sea cucumber"))
   ->addVal("height", 1.2)
   ->record();

  m.close();

  std::string sid = boost::lexical_cast<std::string>(m.sim_id());

  ASSERT_EQ(back.cmds.size(), 4);
  EXPECT_EQ(back.cmds.at(0), "CREATE TABLE DumbTitle (SimID TEXT, animal TEXT, weight INTEGER, height REAL);");
  EXPECT_EQ(back.cmds.at(1), "INSERT INTO DumbTitle (SimID, animal, weight, height) VALUES (\"" + sid + "\", \"monkey\", 10, 5.5);");
  EXPECT_EQ(back.cmds.at(2), "INSERT INTO DumbTitle (SimID, animal, weight) VALUES (\"" + sid + "\", \"elephant\", 1000);");
  EXPECT_EQ(back.cmds.at(3), "INSERT INTO DumbTitle (SimID, animal, height) VALUES (\"" + sid + "\", \"sea cucumber\", 1.2);");
}
