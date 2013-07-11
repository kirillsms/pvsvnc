#pragma once
#include "gui/BaseDialog.h"
class ChatDialog :
	public BaseDialog
{
public:
	bool m_isClosing;
	ChatDialog(void);
	~ChatDialog(void);
private:
	void initControls();

protected:

	virtual BOOL onInitDialog();
};

