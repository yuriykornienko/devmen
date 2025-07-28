#pragma once

// Dpi aware
#include <CDPI/cdpi.h>

class CMainToolBar : public CToolBar
{
public:
    CMainToolBar();
    virtual ~CMainToolBar();

    // Returns toolbar height
    virtual int Create( CWnd* pParentWnd, UINT nIDResource ); // ������� ������ ������������ (CToolBar), ��������� ������� (LoadToolBar), ����������� ������ ����������� (m_imageList) � ������������� ������.

public:
    void CreateProgress(); // ������� ��������� ��������� (CProgressCtrl) � ������ ����� ������ ������������. ���������� �������� "marquee" (������� ������).
    void DestoryProgress();

protected:
    afx_msg BOOL    OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult ); // ������������ ����������� ��������� ��� ������ ������ (��������, "������������� ���� ��������", "��������" � �.�.).
    afx_msg void    OnSize( UINT nType, int cx, int cy ); // �������������� ���������� ��������� ������� ������. ������������� ���������� ��������� ��������� (m_progress) ��� ��������� �������� ����

    DECLARE_MESSAGE_MAP()

protected:
    CDPI          m_dpiAware;
    CImageList    m_imageList;
    CProgressCtrl m_progress;
};
