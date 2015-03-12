
#include "CommentDialog.h"

CommentDialog::CommentDialog()
: BaseDialog(IDD_DCOMMENT),m_commHist(&m_comHistoryKey,99)
{

  StringStorage registryKey;
  registryKey.format(_T("%s\\ComHistory"), RegistryPaths::VIEWER_PATH);
  m_comHistoryKey.open(Registry::getCurrentUserKey(),
                       registryKey.getString(),
                       true);

}


BOOL CommentDialog::onInitDialog() 
{
	
	setControlById(m_combo, IDC_COMM);
    m_commHist.load();

  for (size_t i = 0; i < m_commHist.getCommCount(); i++) {
    m_combo.insertItem(static_cast<int>(i), m_commHist.getCom(i));
  }


  return FALSE;
}

BOOL CommentDialog::onCommand(UINT controlID, UINT notificationID)
{
  if (controlID == IDOK) {
  m_combo.getText(&m_strComment);
  if(m_strComment.getLength()>0){
	m_commHist.load();
	m_commHist.addCom(m_strComment.getString());
	m_commHist.save();
  }


    kill(1);
    return TRUE;
  }
  if (controlID == IDCANCEL) {
    kill(0);
    return TRUE;
  }
  return FALSE;
}

StringStorage *CommentDialog::getComment()
{
  return &m_strComment;
}
