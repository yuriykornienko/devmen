#pragma once

// Dpi aware
#include <CDPI/cdpi.h>

class CMainToolBar : public CToolBar
{
public:
    CMainToolBar();
    virtual ~CMainToolBar();

    // Returns toolbar height
    virtual int Create( CWnd* pParentWnd, UINT nIDResource ); // —оздает панель инструментов (CToolBar), загружает ресурсы (LoadToolBar), настраивает список изображений (m_imageList) и позиционирует панель.

public:
    void CreateProgress(); // —оздает индикатор прогресса (CProgressCtrl) в правой части панели инструментов. »спользует анимацию "marquee" (бегуща€ полоса).
    void DestoryProgress();

protected:
    afx_msg BOOL    OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult ); // ќбрабатывает всплывающие подсказки дл€ кнопок панели (например, "–едактировать базу датчиков", "—войства" и т.д.).
    afx_msg void    OnSize( UINT nType, int cx, int cy ); // ѕереопредел€ет обработчик изменени€ размера панели. јвтоматически перемещает индикатор прогресса (m_progress) при изменении размеров окна

    DECLARE_MESSAGE_MAP()

protected:
    CDPI          m_dpiAware;
    CImageList    m_imageList;
    CProgressCtrl m_progress;
};
