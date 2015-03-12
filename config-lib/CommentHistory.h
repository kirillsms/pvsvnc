#ifndef _COMMENT_HISTORY_H_
#define _COMMENT_HISTORY_H_

#include "win-system/RegistryKey.h"

#include <vector>

using namespace std;
class CommentHistory
{
public:
  CommentHistory(RegistryKey *key, size_t limit);
  virtual ~CommentHistory();
  void setLimit(size_t limit);
  size_t getLimit() const;
  void load();
  void save();
  void truncate();
  void clear();
  void addCom(const TCHAR *com);
  size_t getCommCount() const;
  const TCHAR *getCom(size_t i) const;

protected:
  void releaseComms();
  void removeCom(const TCHAR *com);

protected:
  vector<StringStorage> m_coms;
  size_t m_limit;
  RegistryKey *m_key;
};

#endif
