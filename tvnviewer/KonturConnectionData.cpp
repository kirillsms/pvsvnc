#include "KonturConnectionData.h"


KonturConnectionData::KonturConnectionData(void)
{
}

KonturConnectionData::KonturConnectionData(const KonturConnectionData &connectionData)
: m_id(connectionData.getID())
{
}

KonturConnectionData::~KonturConnectionData(void)
{
}

void KonturConnectionData::setID(const StringStorage *id)
{
  m_id = *id;
}

StringStorage KonturConnectionData::getID() const
{
  return m_id;
}
