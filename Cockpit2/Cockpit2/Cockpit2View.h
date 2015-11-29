
// Cockpit2View.h : interface of the CCockpit2View class
//

#pragma once


class CCockpit2View : public CView
{
protected: // create from serialization only
	CCockpit2View();
	DECLARE_DYNCREATE(CCockpit2View)

// Attributes
public:
	CCockpit2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	//lab1
	void IzotropniMapping(CDC * pDC);
	void Prozori(CDC* pDC, CRect rect, int length, COLORREF color);
	void InstrumentTabla(CDC* pDC, CRect rect, COLORREF color);
	void Pravougaonik(CDC* pDC, CRect rect, COLORREF color);
	void ZaobljeniPravougaonik(CDC* pDC, CRect wall, COLORREF color);
	void DrawPoint(CDC* pDC, CRect wall);
	void testing(CDC* pDC, CRect wall);

	//lab2
	void nacrtajSat(CDC* pDC, CRect wall, CRect sat, int brPodeoka, CString vrednosti[], int brVrednosti, double pocetniUgao, double krajnjiUgao, int tipKazaljke, double ugaoKazaljke);
	void nacrtajZiroskop(CDC* pDC, CRect wall, CRect sat, double ugao);
	void nacrtajStrelicu(CDC* pDC, const CRect wall, const CRect sat, double ugao, int tipStrelice);
	void nacrtajVrednosti(CDC* pDC, CRect wall, int brPodeoka, CString vrednosti[], int brVrednosti, int brMedjuPodeoka, double pocetniUgao, double krajnjiUgao);
	void nacrtajPodeoke(CDC* pDC, CRect wall, int brPodeoka, int brMedjuPodeoka, double pocetniUgao, double krajnjiUgao);
	double scaleValue(double value, double oldMin, double oldMax, double newMin, double newMax);
	void nacrtajFuelWatch(CDC* pDC, CRect wall, CRect watch, COLORREF fuelColor[]);

	HENHMETAFILE createClassicNeedle(CDC* pDC, CRect wall);
	HENHMETAFILE createCircularNeedle(CDC* pDC, CRect wall);
	HENHMETAFILE createPlaneNeedle(CDC* pDC, CRect wall);

	//lab3
	void nacrtajAvion(CDC* painter, CRect prozor, CString putanja);

	double stepen_u_radian(double stepen);

	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CCockpit2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Cockpit2View.cpp
inline CCockpit2Doc* CCockpit2View::GetDocument() const
   { return reinterpret_cast<CCockpit2Doc*>(m_pDocument); }
#endif

