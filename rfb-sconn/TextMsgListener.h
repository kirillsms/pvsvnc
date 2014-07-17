

#ifndef __TEXTMSGLISTENER_H__
#define __TEXTMSGLISTENER_H__

class TextMsgListener
{
public:
  virtual ~TextMsgListener() {}

  virtual void onTextMsg(StringStorage * msg) = 0;
};

#endif // __TEXTMSGLISTENER_H__
