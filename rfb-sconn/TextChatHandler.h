
#ifndef __TEXTCHATHANDLER_H__
#define __TEXTCHATHANDLER_H__

#include "RfbDispatcherListener.h"
#include "RfbCodeRegistrator.h"

#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "log-writer/LogWriter.h"

#include "thread/AutoLock.h"

#include "tvnserver-app/ChatDialog.h"


class TextChatHandler : public RfbDispatcherListener

{
public:
  TextChatHandler(RfbCodeRegistrator *registrator,
                             RfbOutputGate *output,
                             LogWriter *log, ChatDialog * chatDialog);
  virtual ~TextChatHandler();
  void sendMsgToClient(const StringStorage * msg);

protected:

  virtual void onRequest(UINT32 reqCode, RfbInputGate *input);
  RfbOutputGate *m_output;
  LogWriter *m_log;
  ChatDialog * m_chatdialog;
  
};

#endif // __TEXTCHATHANDLER_H__
