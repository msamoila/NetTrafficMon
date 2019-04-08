#ifndef __PAGE_ACT_WND_H_
#define __PAGE_ACT_WND_H_

#include "PageWnd.h"
#include "resource.h"

class CPageActivityDlg : public CDialogImpl<CPageActivityDlg, CPageWnd>
{
public:
	enum { IDD = IDD_PAGE_ACT };
protected:
BEGIN_MSG_MAP(CPageActivityDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDC_ADD_ACT, BN_CLICKED, OnClickedAddActivity)
	COMMAND_HANDLER(IDC_MODIFY_ACT, BN_CLICKED, OnClickedModifyActivity)
	COMMAND_HANDLER(IDC_ACT_ID, EN_KILLFOCUS, OnKillfocusActId)
	COMMAND_HANDLER(IDC_ACT_NAME, EN_KILLFOCUS, OnKillfocusActName)
	COMMAND_HANDLER(IDC_ACT_CODE, EN_KILLFOCUS, OnKillfocusActCode)
	COMMAND_HANDLER(IDC_ACT_ADDED_ROLES, LBN_KILLFOCUS, OnKillfocusAddedRoles)
	COMMAND_HANDLER(IDC_ACT_REMOVED_ROLES, LBN_KILLFOCUS, OnKillfocusRemovedRoles)
END_MSG_MAP()
	
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickedAddActivity(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedModifyActivity(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnKillfocusActId(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnKillfocusActName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnKillfocusActCode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnKillfocusAddedRoles(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnKillfocusRemovedRoles(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif // __PAGE_ACT_WND_H_