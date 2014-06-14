// JsonConvertDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JsonConvert.h"
#include "JsonConvertDlg.h"
#include "LoadFile.h"
#include "JsonLoader.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif



char	g_CurrExeDir[_MAX_PATH];
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CJsonConvertDlg �Ի���




CJsonConvertDlg::CJsonConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJsonConvertDlg::IDD, pParent)
	, m_strTemplateJSON(_T(""))
	, m_strSourceJSON(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJsonConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTemplateJSON);
	DDX_Text(pDX, IDC_EDIT2, m_strSourceJSON);
	DDX_Text(pDX, IDC_EDIT3, m_strExportJSON);

}

BEGIN_MESSAGE_MAP(CJsonConvertDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CJsonConvertDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CJsonConvertDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CJsonConvertDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CJsonConvertDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CJsonConvertDlg ��Ϣ�������

BOOL CJsonConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	::GetCurrentDirectory(_MAX_PATH,g_CurrExeDir);
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CJsonConvertDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CJsonConvertDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CJsonConvertDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CJsonConvertDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString		szFilterFDlg = "JSON�ļ� (*.json)|*.json"; 
	CFileDialog hFileDlg( TRUE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strTemplateJSON = hFileDlg.GetPathName();
		UpdateData(FALSE);

	}
}

void CJsonConvertDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString		szFilterFDlg = "JSON�ļ� (*.json)|*.json"; 
	CFileDialog hFileDlg( TRUE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strSourceJSON = hFileDlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CJsonConvertDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString		szFilterFDlg = "JSON�ļ� (*.json)||"; 
	CFileDialog hFileDlg( FALSE ,  NULL, NULL, NULL,szFilterFDlg, NULL);
	hFileDlg.m_ofn.lpstrInitialDir = g_CurrExeDir;

	if(hFileDlg.DoModal() == IDOK)
	{
		m_strExportJSON = hFileDlg.GetPathName();
		CString strTemp = m_strExportJSON;
		strTemp.MakeLower();
		if(strTemp.Find(".json") == -1)
		{
			m_strExportJSON += ".json";
		}
		UpdateData(FALSE);
	
	}
}

void CJsonConvertDlg::OnBnClickedOk()
{

	if(m_strTemplateJSON == "")
	{
		MessageBox("The Template Json File can't be empty !","Error",MB_OK);
		return ;
	}
	if(m_strSourceJSON == "")
	{
		MessageBox("The Source Json File can't be empty !","Error",MB_OK);
		return ;
	}
	if(m_strExportJSON == "")
	{
		MessageBox("The Target Json File can't be empty !","Error",MB_OK);
		return ;
	}



	// TODO: �ڴ���ӿؼ�֪ͨ����������
	JsonLoader	tCocoLoader_Old;
	size_t	 ulSize = 0;
	if( true == GetFileSize(m_strTemplateJSON ,&ulSize))
	{

		char*	pBinBuff = new char[ulSize];

		if( pBinBuff )
		{
			size_t	tRealLoadSize = 0;

			if(false == LoadFile( m_strTemplateJSON,pBinBuff,ulSize,&tRealLoadSize))
			{
				if(pBinBuff)
				{
					delete[] pBinBuff ;
					pBinBuff = NULL   ;
				}
				ulSize = 0;
				MessageBox("Load Template File Failed!","Error",MB_OK);
				return ;
			}

			DWORD dwStartTime = ::GetTickCount();
			//��ʼ�������ɵĶ�ȡ���

			tCocoLoader_Old.ReadCocoJsonBuff(pBinBuff);


			//delete[] pBinBuff ;
			//pBinBuff = NULL   ;

		}
		ulSize = 0;
	}

	JsonLoader	tCocoLoader_New;
	ulSize = 0;
	if( true == GetFileSize(m_strSourceJSON ,&ulSize))
	{

		char*	pBinBuff = new char[ulSize];

		if( pBinBuff )
		{
			size_t	tRealLoadSize = 0;

			if(false == LoadFile(m_strSourceJSON,pBinBuff,ulSize,&tRealLoadSize))
			{
				if(pBinBuff)
				{
					delete[] pBinBuff ;
					pBinBuff = NULL   ;
				}
				ulSize = 0;
				MessageBox("Load Source Json File Failed!","Error",MB_OK);
				return ;
			}

			DWORD dwStartTime = ::GetTickCount();
			//��ʼ�������ɵĶ�ȡ���
			tCocoLoader_New.ReadCocoJsonBuff(pBinBuff);

			delete[] pBinBuff ;
			pBinBuff = NULL   ;

		}
		ulSize = 0;
	}

	//ȡ��JSON���
	stJsonNode	tJsonNode_Old = tCocoLoader_Old.GetRootJsonNode();
	//ȡ��JSON���
	stJsonNode	tJsonNode_New = tCocoLoader_New.GetRootJsonNode();

	//����
	stJsonNode	tGoodJsonNode = tJsonNode_New.GetGoodJsonNode(&tCocoLoader_Old);

	//��ӡ
	const char* szPathName = m_strExportJSON;
	tGoodJsonNode.Print((void*)szPathName);

	MessageBox("Convert Finished!","OK",MB_OK);
}

BOOL CJsonConvertDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			return FALSE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}
