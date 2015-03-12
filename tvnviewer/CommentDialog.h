
#ifndef _COMMENT_DIALOG_H_
#define _COMMENT_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/ComboBox.h"
#include "resource.h"
#include "config-lib/CommentHistory.h"

#include "win-system/RegistryKey.h"
#include "win-system/Registry.h"
#include "NamingDefs.h"



class CommentDialog : public BaseDialog
{
public:
  CommentDialog();
  StringStorage *getComment();

protected:
  BOOL onCommand(UINT controlID, UINT notificationID);
  BOOL onInitDialog();

  ComboBox m_combo;
  StringStorage m_strComment;
  CommentHistory m_commHist;
  RegistryKey m_comHistoryKey;

};

#endif
