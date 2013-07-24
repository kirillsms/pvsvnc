#pragma once
#include "TvnViewer.h"
#include "KonturLoginDialog.h"
#include "KonturConnectionData.h"

class KonturTvnViewer :
	public TvnViewer
{
public:
	static const TCHAR REPEATERHOST[];
	KonturTvnViewer(HINSTANCE appInstance,
				 const TCHAR *windowClassName,
				 const TCHAR *viewerWindowClassName);

	virtual ~KonturTvnViewer(void);
	virtual void newConnection(const StringStorage *id, const ConnectionConfig *connectionConfig);
	virtual void newConnection(const ConnectionData *conData, const ConnectionConfig *config);
	void showKonturLoginDialog();

private:
	KonturLoginDialog *m_skbLoginDialog;


protected:
	virtual bool isVisibleLoginDialog() const;
	void runRepeaterInstance(KonturConnectionData *conData, const ConnectionConfig *config);
};

