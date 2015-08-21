
#ifndef __P2PEVENTLISTENER_H__
#define __P2PEVENTLISTENER_H__

class P2pEventListener
{
public:
  virtual void onP2pSuccess() = 0;
  virtual void onP2pFailed() = 0;
};

#endif