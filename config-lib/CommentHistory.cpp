#include "CommentHistory.h"
#include "win-system/Registry.h"
#include <crtdbg.h>

CommentHistory::CommentHistory(RegistryKey *key, size_t limit)
: m_key(key), m_limit(limit)
{
}

CommentHistory::~CommentHistory()
{
  releaseComms();
}

void CommentHistory::setLimit(size_t limit)
{
  bool truncationNeeded = limit < m_limit;

  m_limit = limit;

  if (truncationNeeded) {
    truncate();
  }
}

size_t CommentHistory::getLimit() const
{
  return m_limit;
}

void CommentHistory::load()
{
  releaseComms();

  StringStorage valueName;
  StringStorage value;

  for (size_t i = 0; i < m_limit; i++) {
    valueName.format(_T("%d"), i);
    if (!m_key->getValueAsString(valueName.getString(), &value)) {
      break;
    }
    m_coms.push_back(value);
  }
}

void CommentHistory::save()
{
  StringStorage valueName;

  size_t count = m_coms.size();

  for (size_t i = 0; i < min(count, m_limit); i++) {
    valueName.format(_T("%u"), i);
    const TCHAR *value = m_coms.at(i).getString();

    m_key->setValueAsString(valueName.getString(), value);
  }

  if (count > m_limit) {
    truncate();
  }
}

void CommentHistory::truncate()
{
  StringStorage valueName;
  StringStorage value;

  size_t i = (size_t)m_limit;

  while (true) {
    valueName.format(_T("%u"), i);

    if (i >= getCommCount()) {
      return ;
    }

    removeCom(getCom(i));

    if (!m_key->getValueAsString(valueName.getString(), &value)) {
      break;
    }

    m_key->deleteSubKey(value.getString());
    m_key->deleteValue(valueName.getString());

    i++;
  }

  load();
}

void CommentHistory::clear()
{
  StringStorage valueName;

  for (size_t i = 0; i < m_coms.size(); i++) {
    valueName.format(_T("%u"), i);

    m_key->deleteSubKey(m_coms.at(i).getString());
    m_key->deleteValue(valueName.getString());
  }

  releaseComms();
}

void CommentHistory::addCom(const TCHAR *com)
{
  StringStorage comS(com);

  for (vector<StringStorage>::iterator it = m_coms.begin(); it != m_coms.end(); it++) {
    if (it->isEqualTo(&comS)) {
      m_coms.erase(it);
      break;
    }
  }

  m_coms.insert(m_coms.begin(), comS);
}

size_t CommentHistory::getCommCount() const
{
  return m_coms.size();
}

const TCHAR *CommentHistory::getCom(size_t i) const
{
  return m_coms.at(i).getString();
}

void CommentHistory::releaseComms()
{
  m_coms.clear();
}

void CommentHistory::removeCom(const TCHAR *com)
{
  StringStorage comS(com);

  for (vector<StringStorage>::iterator it = m_coms.begin(); it != m_coms.end(); it++) {
    if (it->isEqualTo(&comS)) {
      m_coms.erase(it);
      break;
    }
  }
}
