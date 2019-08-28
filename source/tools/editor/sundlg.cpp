#define SUNDLG_CPP
/*************************************************************************************************\
sunDlg.cpp			: Implementation of the sunDlg component. dialog for sun
settings
//---------------------------------------------------------------------------//
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
//===========================================================================//
\*************************************************************************************************/

#include "sunDlg.h"
#include "resource.h"
#include "camera.h"
#include "utilities.h"
#include "EditorInterface.h"

BEGIN_MESSAGE_MAP(SunDlg, CDialog)
//{{AFX_MSG_MAP(SunDlg)
ON_WM_CTLCOLOR()
ON_BN_CLICKED(IDC_AMBIENT_BUTTON, OnAmbientButton)
ON_BN_CLICKED(IDC_LIGHT_BUTTON, OnLightButton)
ON_BN_CLICKED(IDC_RECALCULATE_TERRAIN_SHADOWS_BUTTON, OnRecalculateTerrainShadowsButton)
ON_BN_CLICKED(IDC_REFRACTALIZE_TERRAIN_BUTTON, OnRefractalizeTerrainButton)
//}}AFX_MSG_MAP
ON_CONTROL_RANGE(EN_CHANGE, IDC_LIGHTCOLOR, IDC_AMBIENT2, OnChangeLightcolor)
END_MESSAGE_MAP()

SunDlg::SunDlg() :
	CDialog(IDD_LIGHT)
{
	//{{AFX_DATA_INIT(SunDlg)
	m_Yaw = 0.0f;
	m_InitialSunPitch = 0.0f;
	//}}AFX_DATA_INIT
}

void
SunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SunDlg)
	DDX_Control(pDX, IDC_LIGHTCOLOR, m_Lightcolour);
	DDX_Control(pDX, IDC_AMBIENT, m_AmbientEdit);
	DDX_Text(pDX, IDC_YAW, m_Yaw);
	DDX_Text(pDX, IDC_INITIAL_PITCH, m_InitialSunPitch);
	//}}AFX_DATA_MAP
}

void
SunDlg::Init()
{
	uint32_t color = eye->dayAmbientBlue | (eye->dayAmbientGreen << 8) | (eye->dayAmbientRed << 16);
	displayInHex(color, m_AmbientEdit);
	color = eye->dayLightBlue | (eye->dayLightGreen << 8) | (eye->dayLightRed << 16);
	displayInHex(color, m_Lightcolour);
	m_InitialSunPitch = eye->lightPitch;
	m_Yaw = eye->lightYaw;
	UpdateData(0);
}

void
SunDlg::displayInHex(int32_t number, CEdit& edit)
{
	// turn this into hex
	CString text;
	text.Format("0x%x", number);
	edit.SetWindowText(text);
}

int32_t
SunDlg::getHexValue(CEdit& edit)
{
	CString str;
	edit.GetWindowText(str);
	str.MakeLower();
	str.Replace("0x", 0);
	int32_t value;
	sscanf_s(str, "%x", &value);
	return value;
}

void
SunDlg::applyChanges()
{
	UpdateData();
	int32_t color = getHexValue(m_Lightcolour);
	eye->lightBlue = eye->dayLightBlue = (color & 0xff);
	eye->lightGreen = eye->dayLightGreen = (color & 0x0000ff00) >> 8;
	eye->lightRed = eye->dayLightRed = (color & 0x00ff0000) >> 16;
	eye->setLightcolour(0, color);
	color = getHexValue(m_AmbientEdit);
	eye->ambientBlue = eye->dayAmbientBlue = (color & 0xff);
	eye->ambientGreen = eye->dayAmbientGreen = (color & 0x0000ff00) >> 8;
	eye->ambientRed = eye->dayAmbientRed = (color & 0x00ff0000) >> 16;
	if (eye->lightYaw != m_Yaw)
	{
		eye->lightYaw = m_Yaw;
		eye->rotateLightRight(0.0);
	}
	if (eye->lightPitch != m_InitialSunPitch)
	{
		eye->lightPitch = eye->dayLightPitch = m_InitialSunPitch;
		eye->rotateLightUp(0.0);
		eye->rotateLightDown(0.0);
	}
	eye->update();
}

void
SunDlg::OnOK()
{
	applyChanges();
	CDialog::OnOK();
}

HBRUSH
SunDlg::OnCtlcolour(CDC* pDC, CWnd* pWnd, uint32_t nCtlcolour)
{
	int32_t ID = pWnd->GetDlgCtrlID();
	HBRUSH hbr = CDialog ::OnCtlcolour(pDC, pWnd, nCtlcolour);
	if (ID == IDC_LIGHTCOLOR || ID == IDC_AMBIENT || ID == IDC_LIGHTCOLOR2 || ID == IDC_AMBIENT2 || ID == IDC_SUNSETCOLOR)
	{
		int32_t i = getHexValue(*(CEdit*)pWnd);
		if (backgroundBrush.m_hObject)
			backgroundBrush.DeleteObject();
		// need to swap r's and blue's.
		uint32_t reverse = reverseRGB(i);
		backgroundBrush.CreateSolidBrush(reverse);
		pDC->SetBkcolour(reverse);
		if (((i & 0xff) + ((i & 0xff00) >> 8) + ((i & 0xff0000) >> 16)) / 3 < 85)
			pDC->SetTextcolour(0x00ffffff);
		return (HBRUSH)backgroundBrush.m_hObject;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

BOOL
SunDlg::OnInitDialog()
{
	CDialog ::OnInitDialog();
	Init();
	return TRUE; // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
}

// end of file ( sunDlg.cpp )

void
SunDlg::OnAmbientButton()
{
	DocolourDlg(m_AmbientEdit);
}

void
SunDlg::DocolourDlg(CEdit& edit)
{
	CString tmpStr;
	int32_t base = getHexValue(edit);
	base &= 0x00ffffff;
	CcolourDialog dlg(reverseRGB(base), nullptr, this);
	if (IDOK == dlg.DoModal())
	{
		base = reverseRGB(dlg.Getcolour());
		tmpStr.Format("0x%x", base);
		edit.SetWindowText(tmpStr);
	}
}

void
SunDlg::OnLightButton()
{
	DocolourDlg(m_Lightcolour);
}

void
SunDlg::OnChangeLightcolor(uint32_t ID)
{
	CString text;
	GetDlgItem(ID)->GetWindowText(text);
	bool bChanged = false;
	int32_t i = 0;
	if (text.GetLength() > 1 && (text[0] == '0' && (text[1] == 'x' || text[i] == 'X')))
		i = 2;
	for (; i < text.GetLength(); ++i)
	{
		if (!isxdigit(text[i]))
		{
			text.Remove(text[i]);
			bChanged = true;
		}
	}
	if (bChanged)
		GetDlgItem(ID)->SetWindowText(text);
	GetDlgItem(ID)->RedrawWindow();
}

void
SunDlg::OnRecalculateTerrainShadowsButton()
{
	EditorInterface::instance()->SetBusyMode();
	applyChanges();
	EditorInterface::instance()->SafeRunGameOSLogic();
	land->reCalcLight(true);
	EditorInterface::instance()->SafeRunGameOSLogic();
	EditorInterface::instance()->updateTacMap();
	EditorInterface::instance()->UnsetBusyMode();
}

void
SunDlg::OnRefractalizeTerrainButton()
{
	applyChanges();
	EditorInterface::instance()->RefractalizeTerrain(1);
}
