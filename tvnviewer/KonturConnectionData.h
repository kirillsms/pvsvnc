#pragma once
#include "ConnectionData.h"
class KonturConnectionData :
	public ConnectionData
{
public:
	KonturConnectionData(void);
	KonturConnectionData(const KonturConnectionData &connectionData);
	virtual ~KonturConnectionData(void);
	void setID(const StringStorage *);
	StringStorage getID() const;

protected:
  StringStorage m_id;
};

