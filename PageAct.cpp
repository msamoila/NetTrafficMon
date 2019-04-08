#include "stdafx.h"
#include "PageAct.h"
#include "DlgOptions.h"


LRESULT CPageActivityDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPageWnd::OnInitDialog(uMsg, wParam, lParam, bHandled);
	CheckDlgButton(IDC_ADD_ACT, m_pParent->m_bAddNewAct ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_MODIFY_ACT,  m_pParent->m_bModifyAct ? BST_CHECKED : BST_UNCHECKED);
	
	SetDlgItemInt(IDC_ACT_ID, m_pParent->m_lActID, FALSE);
	SetDlgItemTextW(m_hWnd, IDC_ACT_NAME, m_pParent->m_szActName);
	SetDlgItemTextW(m_hWnd, IDC_ACT_CODE, m_pParent->m_szActCode);

	const LPCTSTR szRoles[] =
	{
		_T("ROLE_ADMINISTRATOR"),
		_T("ROLE_CLERK"),
		_T("ROLE_PERFORMER"),
		_T("ROLE_RADIOLOGIST"),
		_T("ROLE_WEBUSER"),
		_T("ROLE_LIBRARIAN"),
		_T("ROLE_REFERRING_PHYSICIAN"),
		_T("ROLE_RADIOLOGY_RESIDENT"),	
		_T("ROLE_ICU_STAFF"),	
		_T("ROLE_ER_STAFF"),			
		_T("ROLE_DIAGNOSTIC_AUDITOR"),
		_T("ROLE_PERFORMING_PHYSICIAN"),
		_T("ROLE_REQUESTING_PHYSICIAN"),
		_T("ROLE_WEBGXUSER"),
		_T("ROLE_REFERENCE_WEBGXUSER")
	};

	const GX_PACS::ERoles arrroles[] = 
	{
		GX_PACS::ROLE_ADMINISTRATOR,
		GX_PACS::ROLE_CLERK,
		GX_PACS::ROLE_PERFORMER,
		GX_PACS::ROLE_RADIOLOGIST,
		GX_PACS::ROLE_WEBUSER,
		GX_PACS::ROLE_LIBRARIAN,
		GX_PACS::ROLE_REFERRING_PHYSICIAN,
		GX_PACS::ROLE_RADIOLOGY_RESIDENT,		
		GX_PACS::ROLE_ICU_STAFF,	
		GX_PACS::ROLE_ER_STAFF,			
		GX_PACS::ROLE_DIAGNOSTIC_AUDITOR,
		GX_PACS::ROLE_PERFORMING_PHYSICIAN,
		GX_PACS::ROLE_REQUESTING_PHYSICIAN,
		GX_PACS::ROLE_WEBGXUSER,
		GX_PACS::ROLE_REFERENCE_WEBGXUSER,
		GX_PACS::ROLE_MAX
	};
	
	long i, j, k;
	for(i=0; arrroles[i] != GX_PACS::ROLE_MAX; i++)
	{
		j = SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_ADDSTRING, 0, (LPARAM)szRoles[i]);
		SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_SETITEMDATA, j, arrroles[i]);

		j = SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_ADDSTRING, 0, (LPARAM)szRoles[i]);
		SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_SETITEMDATA, j, arrroles[i]);
		
		for(k=0; k < m_pParent->m_eActAddRoles.GetSize(); k++)
		{
			if(m_pParent->m_eActAddRoles[k] == arrroles[i])
			{
				SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_SETSEL, TRUE, j);
				break;
			}
		}

		for(k=0; k < m_pParent->m_eActRemRoles.GetSize(); k++)
		{
			if(m_pParent->m_eActRemRoles[k] == arrroles[i])
			{
				SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_SETSEL, TRUE, j);
				break;
			}
		}
	}	
	return 0;
}
LRESULT CPageActivityDlg::OnClickedAddActivity(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pParent->m_bAddNewAct = IsDlgButtonChecked(IDC_ADD_ACT) == BST_CHECKED;
	return 0;
}
LRESULT CPageActivityDlg::OnClickedModifyActivity(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pParent->m_bModifyAct = IsDlgButtonChecked(IDC_MODIFY_ACT) == BST_CHECKED;
	return 0;
}
LRESULT CPageActivityDlg::OnKillfocusActId(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pParent->m_lActID = GetDlgItemInt(IDC_ACT_ID, NULL, FALSE);
	return 0;
}
LRESULT CPageActivityDlg::OnKillfocusActName(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pParent->m_szActName.Empty();
	GetDlgItemText(IDC_ACT_NAME, m_pParent->m_szActName.m_str);
	return 0;
}
LRESULT CPageActivityDlg::OnKillfocusActCode(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pParent->m_szActCode.Empty();
	GetDlgItemText(IDC_ACT_CODE, m_pParent->m_szActCode.m_str);
	return 0;
}

LRESULT CPageActivityDlg::OnKillfocusAddedRoles(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int i, j = SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_GETSELCOUNT);
	if(j==0)
		return 0;
	m_pParent->m_eActAddRoles.RemoveAll();
	int* arrSelItemsIdx = new int[j];

	SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_GETSELITEMS, j, (LPARAM)arrSelItemsIdx);
	
	for(i=0; i<j; i++)
	{
		GX_PACS::ERoles role = (GX_PACS::ERoles)SendDlgItemMessage(IDC_ACT_ADDED_ROLES, LB_GETITEMDATA, arrSelItemsIdx[i]);
		m_pParent->m_eActAddRoles.Add( role );
	}

	delete [] arrSelItemsIdx;
	return 0;
}

LRESULT CPageActivityDlg::OnKillfocusRemovedRoles(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int i, j = SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_GETSELCOUNT);
	if(j==0)
		return 0;
	m_pParent->m_eActRemRoles.RemoveAll();
	int* arrSelItemsIdx = new int[j];

	SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_GETSELITEMS, j, (LPARAM)arrSelItemsIdx);
	
	for(i=0; i<j; i++)
	{
		GX_PACS::ERoles role = (GX_PACS::ERoles)SendDlgItemMessage(IDC_ACT_REMOVED_ROLES, LB_GETITEMDATA, arrSelItemsIdx[i]);
		m_pParent->m_eActRemRoles.Add( role );
	}

	delete [] arrSelItemsIdx;
	return 0;
}