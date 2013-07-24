#include "KonturTvnViewer.h"
#include "KonturViewerInstance.h"

const TCHAR KonturTvnViewer::REPEATERHOST[] =   _T("vnc.kontur.ru:5901");
KonturTvnViewer::KonturTvnViewer(HINSTANCE appInstance, const TCHAR *windowClassName,
						   const TCHAR *viewerWindowClassName)
: TvnViewer(appInstance, windowClassName, viewerWindowClassName),
  m_skbLoginDialog(0)
{
  m_skbLoginDialog = new KonturLoginDialog(this);
}


KonturTvnViewer::~KonturTvnViewer(void)
{
  delete m_skbLoginDialog;
}

void KonturTvnViewer::showKonturLoginDialog()
{
  m_skbLoginDialog->loadIcon(IDI_APPICON);
  m_skbLoginDialog->show();
  addModelessDialog(m_skbLoginDialog->getControl()->getWindow());
}

bool KonturTvnViewer::isVisibleLoginDialog() const
{
  return !!m_skbLoginDialog->getControl()->getWindow();
}

void KonturTvnViewer::newConnection(const StringStorage *id, const ConnectionConfig *config)
{
  KonturConnectionData *conData = new KonturConnectionData;
  conData->setHost(new StringStorage(REPEATERHOST));
  conData->setID(id);
  runRepeaterInstance(conData, config);
}

void KonturTvnViewer::newConnection(const ConnectionData *conData, const ConnectionConfig *config)
{
  ConnectionData *copyConData = new ConnectionData(*conData);
  runInstance(copyConData, config);
}

void KonturTvnViewer::runRepeaterInstance(KonturConnectionData *conData, const ConnectionConfig *config)
{
  ViewerInstance *viewerInst = new KonturViewerInstance(this, conData, config);
  viewerInst->start();

  addInstance(viewerInst);
}