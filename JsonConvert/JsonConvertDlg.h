// JsonConvertDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CJsonConvertDlg �Ի���
class CJsonConvertDlg : public CDialog
{
// ����
public:
	CJsonConvertDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_JSONCONVERT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strTemplateJSON;
	CString m_strSourceJSON;
	CString m_strExportJSON;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
