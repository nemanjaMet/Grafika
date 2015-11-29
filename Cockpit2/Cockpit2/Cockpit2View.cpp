
// Cockpit2View.cpp : implementation of the CCockpit2View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Cockpit2.h"
#endif

#include "math.h"
#include "Cockpit2Doc.h"
#include "Cockpit2View.h"
#include "DImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159265

// CCockpit2View

IMPLEMENT_DYNCREATE(CCockpit2View, CView)

BEGIN_MESSAGE_MAP(CCockpit2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CCockpit2View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CCockpit2View construction/destruction

CCockpit2View::CCockpit2View()
{
	// TODO: add construction code here

}

CCockpit2View::~CCockpit2View()
{
}

BOOL CCockpit2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CCockpit2View drawing

void CCockpit2View::nacrtajSat(CDC * pDC, CRect wall, CRect sat, int brPodeoka, CString vrednosti[], int brVrednosti, double pocetniUgao, double krajnjiUgao, int tipKazaljke, double ugaoKazaljke)
{
	// Olovka i cetka
	//CPen myPen = CPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen myPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen* defPen = pDC->SelectObject(&myPen);

	CBrush myBrush(RGB(0, 0, 0));
	CBrush * defBrush = pDC->SelectObject(&myBrush);

	//Crtanje spoljnog kruga
	pDC->Ellipse(sat);

	//Crtanje podeoka
	nacrtajPodeoke(pDC, sat, 12, 1, pocetniUgao, krajnjiUgao);

	//Crtanje teksta
	nacrtajVrednosti(pDC, sat, 12, vrednosti, brVrednosti, 1, pocetniUgao, krajnjiUgao);

	//Crtanje strelice
	//Pozicija

	int centarX = (sat.left + sat.right) / 2;
	int centarY = (sat.top + sat.bottom) / 2;

	CPoint pTL = CPoint(centarX - 2, sat.top + 5);
	CPoint pBR = CPoint(centarX + 2, sat.bottom - 5);

	CRect strelicaPravougaonik = CRect(pTL, pBR);

	//painter->Rectangle(strelicaPravougaonik);

	// ROTACIJA STRELICE 

	//Graficki mod
	int prevMod = SetGraphicsMode(pDC->m_hDC, GM_ADVANCED);

	XFORM xForm, xFormOld;
	GetWorldTransform(pDC->m_hDC, &xFormOld);

	//Translacija na koordinatni pocetak
	xForm.eM11 = (FLOAT) 1.0;
	xForm.eM12 = (FLOAT) 0.0;
	xForm.eM21 = (FLOAT) 0.0;
	xForm.eM22 = (FLOAT) 1.0;
	xForm.eDx = (FLOAT)-centarX;
	xForm.eDy = (FLOAT)-centarY;

	double ugaoRad = stepen_u_radian(ugaoKazaljke);

	//Rotacija
	SetWorldTransform(pDC->m_hDC, &xForm);

	xForm.eM11 = (FLOAT)cos(ugaoRad);
	xForm.eM12 = (FLOAT)sin(ugaoRad);
	xForm.eM21 = (FLOAT)-sin(ugaoRad);
	xForm.eM22 = (FLOAT)cos(ugaoRad);
	xForm.eDx = (FLOAT)centarX;
	xForm.eDy = (FLOAT)centarY;

	//Translacija * Rotacija
	ModifyWorldTransform(pDC->m_hDC, &xForm, MWT_RIGHTMULTIPLY);

	//Crtanje

	if (tipKazaljke != 3)
	{
		nacrtajStrelicu(pDC, wall, strelicaPravougaonik, ugaoKazaljke, tipKazaljke);
	}
	else if (tipKazaljke == 3)
	{
		pTL = CPoint(centarX - 7, sat.top + 5);
		pBR = CPoint(centarX + 7, sat.bottom - 5);
		strelicaPravougaonik = CRect(pTL, pBR);
		nacrtajStrelicu(pDC, wall, strelicaPravougaonik, ugaoKazaljke, tipKazaljke);
	}

	//Povratak grafickog moda
	SetWorldTransform(pDC->m_hDC, &xFormOld);
	SetGraphicsMode(pDC->m_hDC, prevMod);



	//Povratak olovke i cetke
	pDC->SelectObject(defPen);
	pDC->SelectObject(defBrush);

	myPen.DeleteObject();
	myBrush.DeleteObject();
}

void CCockpit2View::nacrtajZiroskop(CDC * pDC, CRect wall, CRect sat, double ugao)
{
	// Tacke od znacaja
	CPoint leviCentar, desniCentar, Centar;

	leviCentar = CPoint(sat.left, (sat.top + sat.bottom) / 2);
	desniCentar = CPoint(sat.right, (sat.top + sat.bottom) / 2);
	Centar = CPoint((sat.left + sat.right) / 2, (sat.top + sat.bottom) / 2);

	//Olovka i cetka
	//CPen myPen = CPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen myPen(PS_SOLID, 1, RGB(255, 255, 255));
	CPen* defPen = pDC->SelectObject(&myPen);

	CBrush myBlueBrush(RGB(48, 176, 224));
	CBrush mySandBrush(RGB(208, 176, 128));
	CBrush * defBrush = pDC->SelectObject(&myBlueBrush);

	// ---- ROTACIJA ---- 
	int prevMod;
	XFORM xForm, xFormOld;

	if (ugao != 0)
	{
		//Graficki mod
		prevMod = SetGraphicsMode(pDC->m_hDC, GM_ADVANCED);
		GetWorldTransform(pDC->m_hDC, &xFormOld);

		//Transliranje na pocetak 
		int translateX = (Centar.x);
		int translateY = (Centar.y);

		//Translacija na koordinatni pocetak
		xForm.eM11 = (FLOAT) 1.0;
		xForm.eM12 = (FLOAT) 0.0;
		xForm.eM21 = (FLOAT) 0.0;
		xForm.eM22 = (FLOAT) 1.0;
		xForm.eDx = (FLOAT)-translateX;
		xForm.eDy = (FLOAT)-translateY;

		//Ugao rotacije
		double ugaoRad = stepen_u_radian(ugao);

		///Rotacija
		SetWorldTransform(pDC->m_hDC, &xForm);

		xForm.eM11 = (FLOAT)cos(ugaoRad);
		xForm.eM12 = (FLOAT)sin(ugaoRad);
		xForm.eM21 = (FLOAT)-sin(ugaoRad);
		xForm.eM22 = (FLOAT)cos(ugaoRad);
		xForm.eDx = (FLOAT)translateX;
		xForm.eDy = (FLOAT)translateY;

		// Translacija * Rotacija
		ModifyWorldTransform(pDC->m_hDC, &xForm, MWT_RIGHTMULTIPLY);
	}

	//--- Crtanje

	//Gornji luk
	pDC->BeginPath();
	pDC->SetArcDirection(AD_CLOCKWISE);

	pDC->Arc(sat, leviCentar, desniCentar);

	pDC->EndPath();
	pDC->StrokeAndFillPath();

	//Donji luk
	pDC->SelectObject(&mySandBrush);
	pDC->BeginPath();
	pDC->SetArcDirection(AD_COUNTERCLOCKWISE);

	pDC->Arc(sat, leviCentar, desniCentar);

	pDC->EndPath();
	pDC->StrokeAndFillPath();

	//Gornji podeoci
	nacrtajPodeoke(pDC, sat, 12, 2, 300 + ugao, 420 + ugao);

	//Donji podeoci
	CPoint krajPrvog, krajDrugog, krajTreceg, krajCetvrtog, krajPetog;

	krajPrvog = CPoint(Centar.x, Centar.y + 9);
	krajDrugog = CPoint(Centar.x - 5, Centar.y + 7);
	krajTreceg = CPoint(Centar.x + 5, Centar.y + 7);
	krajCetvrtog = CPoint(Centar.x - 10, Centar.y + 5);
	krajPetog = CPoint(Centar.x + 10, Centar.y + 5);

	pDC->MoveTo(Centar);
	pDC->LineTo(krajPrvog);

	pDC->MoveTo(Centar);
	pDC->LineTo(krajDrugog);

	pDC->MoveTo(Centar);
	pDC->LineTo(krajTreceg);

	pDC->MoveTo(Centar);
	pDC->LineTo(krajCetvrtog);

	pDC->MoveTo(Centar);
	pDC->LineTo(krajPetog);

	//--- Vracanje grafickog moda i transformacije
	if (ugao != 0)
	{
		SetWorldTransform(pDC->m_hDC, &xFormOld);
		SetGraphicsMode(pDC->m_hDC, prevMod);
	}

	//--- Vracanje cetkica

	pDC->SelectObject(defPen);
	myPen.DeleteObject();
	pDC->SelectObject(defBrush);
	myBlueBrush.DeleteObject();
	mySandBrush.DeleteObject();
}

void CCockpit2View::nacrtajStrelicu(CDC * pDC, const CRect wall, const CRect sat, double ugao, int tipStrelice)
{
	HENHMETAFILE MF;
	CString MetaFileName;

	switch (tipStrelice)
	{
	case 1:
		//MetaFileName = CString("SimplyNeedle.emf");
		MF = createClassicNeedle(pDC, sat);
		break;

	case 2:
		//MetaFileName = CString("ComplexNeedle.emf");
		MF = createCircularNeedle(pDC, sat);
		break;

	case 3:
		//MetaFileName = CString("PlaneNeedle.emf");
		MF = createPlaneNeedle(pDC, sat);
		break;

	default:
		return;
	}

	MF = GetEnhMetaFile(MetaFileName);

	pDC->PlayMetaFile(MF, sat);

	DeleteEnhMetaFile(MF);
}

void CCockpit2View::nacrtajVrednosti(CDC * pDC, CRect wall, int brPodeoka, CString vrednosti[], int brVrednosti, int brMedjuPodeoka, double pocetniUgao, double krajnjiUgao)
{
	//Font
	CFont Font, *oldFont;
	int charHeight = wall.Height() * 0.18;
	int charWidth = wall.Height() * 0.08;
	int lineLength = wall.Height() * 0.08;

	Font.CreateFontW(charHeight, charWidth, 0, 0, FW_BLACK, 0, 0, 0, 0, 0, 0, 0, 0, CString("Arial"));

	oldFont = pDC->SelectObject(&Font);
	pDC->SetBkColor(RGB(0, 0, 0));
	pDC->SetTextColor(RGB(255, 255, 255));

	//Translacija i rotacija teksta pozicije
	int translateTextX = (wall.left + wall.Width() / 2);
	int translateTextY = (wall.top + wall.Height() / 2);

	int razlikaUglova = abs((int)(pocetniUgao - krajnjiUgao));

	//Graficki mod
	int prevMod = SetGraphicsMode(pDC->m_hDC, GM_ADVANCED);

	XFORM xForm, xFormOld;
	GetWorldTransform(pDC->m_hDC, &xFormOld);

	for (int i = 0; i < brVrednosti && brVrednosti != 0; i++)
	{
		CString stringVrednost = vrednosti[i];

		int stringDuzina = charWidth * stringVrednost.GetLength();

		int stringStartX = wall.left + wall.Width() / 2 - stringDuzina / 2;
		int stringStartY = wall.top + lineLength + 0.03*wall.Height();

		//Transliranje do koordinatnog pocetka, simulirajuci da je koordinatni pocetak centar kruga, tj sata
		xForm.eM11 = (FLOAT) 1.0;
		xForm.eM12 = (FLOAT) 0.0;
		xForm.eM21 = (FLOAT) 0.0;
		xForm.eM22 = (FLOAT) 1.0;
		xForm.eDx = (FLOAT)-translateTextX;
		xForm.eDy = (FLOAT)-translateTextY;

		//Odredjivanje ugla rotiranje
		double ugao = (razlikaUglova / (brVrednosti - 1)) * i + pocetniUgao;
		double ugaoRad = stepen_u_radian(ugao);

		//Rotacija linije sa translacijom do odredisne pozicije
		SetWorldTransform(pDC->m_hDC, &xForm);

		xForm.eM11 = (FLOAT)cos(ugaoRad);
		xForm.eM12 = (FLOAT)sin(ugaoRad);
		xForm.eM21 = (FLOAT)-sin(ugaoRad);
		xForm.eM22 = (FLOAT)cos(ugaoRad);
		xForm.eDx = (FLOAT)translateTextX;
		xForm.eDy = (FLOAT)translateTextY;

		ModifyWorldTransform(pDC->m_hDC, &xForm, MWT_RIGHTMULTIPLY);

		pDC->TextOutW(stringStartX, stringStartY, stringVrednost);
	}

	//Vracanje grafickog moda
	SetWorldTransform(pDC->m_hDC, &xFormOld);
	SetGraphicsMode(pDC->m_hDC, prevMod);
}

void CCockpit2View::nacrtajPodeoke(CDC * pDC, CRect wall, int brPodeoka, int brMedjuPodeoka, double pocetniUgao, double krajnjiUgao)
{
	//Olovka
	CPen pen, *oldPen = NULL;

	CPoint pocetnaTacka = CPoint(wall.left + wall.Width() / 2, wall.top);
	CPoint krajnjaTacka = CPoint(wall.left + wall.Width() / 2, wall.top + 0.2*wall.Width() / 2);

	//Graficki mod
	int prevMod = SetGraphicsMode(pDC->m_hDC, GM_ADVANCED);

	XFORM xForm, xFormOld;
	GetWorldTransform(pDC->m_hDC, &xFormOld);

	//Transliranje na pocetak 
	int translateX = (wall.left + wall.Width() / 2);
	int translateY = (wall.top + wall.Height() / 2);

	int razlikaUglova = abs((int)(pocetniUgao - krajnjiUgao));

	for (int i = 0; i <= brPodeoka && brPodeoka != 0; i++)
	{
		//Translacija na koordinatni pocetak
		xForm.eM11 = (FLOAT) 1.0;
		xForm.eM12 = (FLOAT) 0.0;
		xForm.eM21 = (FLOAT) 0.0;
		xForm.eM22 = (FLOAT) 1.0;
		xForm.eDx = (FLOAT)-translateX;
		xForm.eDy = (FLOAT)-translateY;

		//Ugao rotacije
		double ugao = (razlikaUglova / brPodeoka) * i + pocetniUgao;
		double ugaoRad = stepen_u_radian(ugao);

		///Rotacija
		SetWorldTransform(pDC->m_hDC, &xForm);

		xForm.eM11 = (FLOAT)cos(ugaoRad);
		xForm.eM12 = (FLOAT)sin(ugaoRad);
		xForm.eM21 = (FLOAT)-sin(ugaoRad);
		xForm.eM22 = (FLOAT)cos(ugaoRad);
		xForm.eDx = (FLOAT)translateX;
		xForm.eDy = (FLOAT)translateY;

		// Translacija * Rotacija
		ModifyWorldTransform(pDC->m_hDC, &xForm, MWT_RIGHTMULTIPLY);

		int debljinaLinije, duzinaLinije;

		if (i % (brMedjuPodeoka + 1) == 0)
		{
			duzinaLinije = wall.Height() * 0.15;
			debljinaLinije = 1;
		}
		else
		{
			duzinaLinije = wall.Height() * 0.09;
			debljinaLinije = 0;
		}

		pen.CreatePen(PS_SOLID, debljinaLinije, RGB(255, 255, 255));
		oldPen = pDC->SelectObject(&pen);

		krajnjaTacka.y = wall.top + duzinaLinije;

		pDC->MoveTo(pocetnaTacka);
		pDC->LineTo(krajnjaTacka);

		//Vracanje olovke
		pDC->SelectObject(oldPen);
		pen.DeleteObject();
	}

	//Vracanje grafickog moda
	SetWorldTransform(pDC->m_hDC, &xFormOld);
	SetGraphicsMode(pDC->m_hDC, prevMod);

	pDC->SelectObject(oldPen);
	pen.DeleteObject();
}

double CCockpit2View::scaleValue(double value, double oldMin, double oldMax, double newMin, double newMax)
{
	return (value - oldMin)*(newMax - newMin) / (oldMax - oldMin) + newMin;
}

void CCockpit2View::nacrtajFuelWatch(CDC * pDC, CRect wall, CRect watch, COLORREF fuelColor[])
{
	CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(pen);
	CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(32, 32, 32));
	pDC->SelectObject(newBrush);

	int width = wall.Width();
	int height = wall.Height();

	CPoint p1;
	CPoint p2;
	//CRect krug;

	/////////ARC
	//pDC->BeginPath();

	p1.SetPoint(width * 0.395, height *  0.66);
	p2.SetPoint(width * 0.463, height *  0.745);
	//krug.SetRect(p1, p2);
	
	p1.SetPoint(width * 0.395, height *  0.69);
	p2.SetPoint(width * 0.463, height *  0.69);
	//pDC->MoveTo(width * 0.44, height *  0.72);
	//pDC->ArcTo(krug, p2, p1);

	///////////FUEL1////////////////////////
	int startEndHeightArc = (watch.bottom + watch.top) / 2.03;

	pDC->BeginPath();
	CPoint p3, p4;
	p3.SetPoint(watch.left, startEndHeightArc);
	p4.SetPoint(watch.right, startEndHeightArc);
	
	p1.SetPoint(watch.left + watch.Width() * 0.25, watch.bottom + watch.Height() * 0.25);
	p2.SetPoint(watch.left + watch.Width() * 0.75, watch.bottom - watch.Height() * 0.25);
	CRect partWatch;
	partWatch.SetRect(p1, p2);
	startEndHeightArc = (partWatch.bottom + partWatch.top) / 2.03;
	p1.SetPoint(partWatch.left, startEndHeightArc);
	p2.SetPoint(partWatch.right, startEndHeightArc);

	pDC->MoveTo(watch.left + watch.Width() * 0.75, startEndHeightArc);
	pDC->ArcTo(watch, p4, p3);
	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(partWatch, p1, p2);

	pDC->EndPath();
	pDC->StrokeAndFillPath();


	//pDC->Rectangle(watch);
	//pDC->Rectangle(partWatch);
	CRect rectStrelica = partWatch;

	//zeleni deo (srednji)
	CPen greenPen(PS_SOLID, 2, fuelColor[1]);
	pDC->SelectObject(greenPen);
	p1.SetPoint(watch.left + watch.Width() * 0.12, watch.top + watch.Height() * 0.25);
	p2.SetPoint(watch.right - watch.Width() * 0.1, watch.bottom - watch.Height() * 0.1);
	partWatch.SetRect(p1, p2);
	startEndHeightArc = (partWatch.bottom + partWatch.top) / 2.04;
	p1.SetPoint(partWatch.left, startEndHeightArc);
	p2.SetPoint(partWatch.right, startEndHeightArc);
	pDC->Arc(partWatch, p1, p2);

	//Crveni deo (levi deo)
	CPen redPen(PS_SOLID, 2, fuelColor[0]);
	pDC->SelectObject(redPen);
	p1.SetPoint(partWatch.left, startEndHeightArc);
	p2.SetPoint(partWatch.left + watch.Width() * 0.3, startEndHeightArc);
	pDC->Arc(partWatch, p1, p2);
	
	//Zuti deo (desni deo)
	CPen yellowPen(PS_SOLID, 2, fuelColor[2]);
	pDC->SelectObject(yellowPen);
	p1.SetPoint(partWatch.right - watch.Width() * 0.3, startEndHeightArc);
	p2.SetPoint(partWatch.right, startEndHeightArc);
	pDC->SetArcDirection(AD_COUNTERCLOCKWISE);
	pDC->Arc(partWatch, p2, p1);

	//Strelice
	//createClassicNeedle(pDC, watch);
	//pDC->Rectangle(rectStrelica);
	CPoint nizStrelica[3] = {
	CPoint(rectStrelica.left + rectStrelica.Width() / 2  - 1, rectStrelica.bottom ),
	CPoint(rectStrelica.left + rectStrelica.Width() / 2 + 1, rectStrelica.bottom),
	CPoint(partWatch.left + partWatch.Width() / 2, partWatch.top - 2),
	};

	pDC->BeginPath();
	CPen pen3(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(pen3);
	CBrush newBrush3;
	newBrush3.CreateSolidBrush(RGB(255, 255, 255));
	pDC->SelectObject(newBrush3);

	pDC->Polygon(nizStrelica, 3);

	pDC->EndPath();
	pDC->StrokeAndFillPath();

	redPen.DeleteObject();
	yellowPen.DeleteObject();
	greenPen.DeleteObject();
	pen.DeleteObject();
	newBrush.DeleteObject();
}

HENHMETAFILE CCockpit2View::createClassicNeedle(CDC * pDC, CRect wall)
{
	CMetaFileDC MetaDC;
	MetaDC.CreateEnhanced(pDC, CString("SimplyNeedle.emf"), wall, CString("classicNeedle"));

	HENHMETAFILE MF;

	CBrush brush, *oldBrush = NULL;
	brush.CreateSolidBrush(RGB(255, 0, 0));

	CPoint p1 = CPoint(2500, 0);
	CPoint p2 = CPoint(5000, 20000);
	CPoint p3 = CPoint(5000, 60000);
	CPoint p4 = CPoint(0, 60000);
	CPoint p5 = CPoint(0, 20000);
	CPoint p6 = p1;

	CPoint line[] = { p1, p2, p3, p4, p5, p6 };

	pDC->SelectObject(&brush);
	MetaDC.Polygon(line, 6);
	pDC->SelectObject(oldBrush);
	brush.DeleteObject();

	MF = MetaDC.CloseEnhanced();
	MetaDC.DeleteDC();

	return MF;
}

HENHMETAFILE CCockpit2View::createCircularNeedle(CDC * pDC, CRect wall)
{
	CMetaFileDC MetaDC;
	MetaDC.CreateEnhanced(pDC, CString("ComplexNeedle.emf"), wall, CString("CircularNeedle"));

	HENHMETAFILE MF;

	double width = wall.Width();
	double height = wall.Height();
	double dveTrecine = 0.66;

	CPen pen, *oldPen;
	pen.CreatePen(PS_SOLID, width / 100, RGB(255, 255, 255));

	CBrush brush, *oldBrush;
	brush.CreateSolidBrush(RGB(255, 255, 255));

	//Gornja Polovina kazaljke
	CPoint p1 = CPoint(wall.left + width / 2, wall.top);
	CPoint p2 = CPoint(wall.left + 0.75 * width, wall.top + height / 12);
	CPoint p3 = CPoint(p2.x, wall.top + height / 2);
	CPoint p4 = CPoint(wall.left + 0.25 * width, p3.y);
	CPoint p5 = CPoint(p4.x, p2.y);
	CPoint p6 = p1;

	CPoint lineTopHalf[] = { p1, p2, p3, p4, p5, p6 };

	oldPen = pDC->SelectObject(&pen);

	oldBrush = pDC->SelectObject(&brush);
	pDC->Polygon(lineTopHalf, 6);
	pDC->SelectObject(oldBrush);
	brush.DeleteObject();

	//Donja polovina kruga
	int radius = width / 2;

	double alfa = acos(0.5);

	//---prva linija
	CPoint line1[2];
	line1[0] = CPoint(wall.left + width / 4, wall.top + height / 2);
	line1[1] = CPoint(line1[0].x, wall.top + (dveTrecine * height) - sin(alfa) * radius);

	//---prvi luk
	CPoint arc1P1 = CPoint(wall.left, wall.top + (dveTrecine * height) - width / 2);
	CPoint arc1P2 = CPoint(wall.right, arc1P1.y + width);
	CPoint arc1P3 = CPoint(wall.left + width / 4, wall.top + (dveTrecine * height) - radius * sin(alfa));
	CPoint arc1P4 = CPoint(arc1P3.x + 0.1, wall.top + (dveTrecine * height) + radius * sin(alfa));

	//---druga linija
	CPoint line2[2];
	line2[0] = CPoint(line1[0].x, wall.top + (dveTrecine * height) + radius * sin(alfa));
	line2[1] = CPoint(line2[0].x, wall.bottom - radius - radius * sin(alfa));

	//---donji luk
	CPoint arc2P1 = CPoint(arc1P1.x, wall.bottom - width);
	CPoint arc2P2 = wall.BottomRight();
	CPoint arc2P3 = CPoint(wall.left + radius / 2 + 0.1, wall.bottom - radius - radius * sin(alfa) + 0.1);
	CPoint arc2P4 = CPoint(wall.right - radius / 2, arc2P3.y);

	//---treca linija
	CPoint line3[2];
	line3[0] = arc2P4;
	line3[1] = CPoint(line3[0].x, line2[0].y);

	//---treci luk
	CPoint arc3P1 = arc1P1;
	CPoint arc3P2 = arc1P2;
	CPoint arc3P3 = line3[1];
	CPoint arc3P4 = CPoint(line3[1].x, line1[1].y);

	//---cetvrta linija
	CPoint line4[2];
	line4[0] = CPoint(line3[1].x, line1[1].y);
	line4[1] = CPoint(line4[0].x, line1[0].y);

	//---peta (gornja) linija
	CPoint line5[2];
	line5[0] = line4[1];
	line5[1] = line1[0];

	pDC->BeginPath();

	//painter->Polygon(lineTopHalf, 6);

	//prva linija
	pDC->MoveTo(line1[0]);
	pDC->LineTo(line1[1]);

	//prvi luk
	pDC->Arc(arc1P1.x, arc1P1.y, arc1P2.x, arc1P2.y, arc1P3.x, arc1P3.y, arc1P4.x, arc1P4.y);

	//druga linija
	pDC->MoveTo(line2[0]);
	pDC->LineTo(line2[1]);

	//donji luk
	pDC->Arc(arc2P1.x, arc2P1.y, arc2P2.x, arc2P2.y, arc2P3.x, arc2P3.y, arc2P4.x, arc2P4.y);

	//treca linija
	pDC->MoveTo(line3[0]);
	pDC->LineTo(line3[1]);

	//treci luk
	pDC->Arc(arc3P1.x, arc3P1.y, arc3P2.x, arc3P2.y, arc3P3.x, arc3P3.y, arc3P4.x, arc3P4.y);

	//cetvrta linija
	pDC->MoveTo(line4[0]);
	pDC->LineTo(line4[1]);

	//peta linija
	pDC->MoveTo(line5[0]);
	pDC->LineTo(line5[1]);

	pDC->EndPath();

	pDC->StrokePath();
	pDC->SelectObject(oldPen);
	pen.DeleteObject();

	pen.CreatePen(PS_SOLID, width / 100, RGB(255, 255, 255));
	brush.CreateSolidBrush(RGB(0, 0, 0));

	oldPen = pDC->SelectObject(&pen);
	oldBrush = pDC->SelectObject(&brush);

	int pola_sirine = (wall.left + wall.right) / 2;
	int pola_visine = (wall.top + wall.bottom) / 2;

	pDC->Ellipse(pola_sirine - 1, pola_visine, pola_sirine + 1, pola_visine);

	pDC->SelectObject(oldBrush);
	pDC->SelectObject(oldPen);

	brush.DeleteObject();
	pen.DeleteObject();

	MF = MetaDC.CloseEnhanced();
	MetaDC.DeleteDC();

	return MF;
}

HENHMETAFILE CCockpit2View::createPlaneNeedle(CDC * pDC, CRect wall)
{
	CMetaFileDC MetaDC;
	MetaDC.CreateEnhanced(pDC, CString("PlaneNeedle.emf"), wall, CString("Plane"));

	HENHMETAFILE MF;

	double width = wall.Width();
	double height = wall.Height();
	int left = wall.left;
	int right = wall.right;
	int top = wall.top;
	int bottom = wall.bottom;
	int centerX = wall.CenterPoint().x;
	int centerY = wall.CenterPoint().y;

	CPen pen, *oldPen = NULL;
	pen.CreatePen(PS_SOLID, width / 100, RGB(255, 0, 0));

	CPoint p1 = CPoint(left + width / 2, top);
	CPoint p2 = CPoint(centerX + 0.125 * width, top + 0.3 * height);
	CPoint p3 = CPoint(right, top + 0.5 * height);
	CPoint p4 = CPoint(p3.x, p3.y + 0.1 * height);
	CPoint p5 = CPoint(p2.x, p3.y + abs(p3.y - p4.y) / 2);
	CPoint p6 = CPoint((centerX + p5.x) / 2, bottom - height / 4);
	CPoint p7 = CPoint(left + 0.75 * width, bottom - 0.075 * height);
	CPoint p8 = CPoint(p7.x, bottom);
	CPoint p9 = CPoint(p5.x, p7.y);
	CPoint p10 = CPoint(left + width / 2, bottom);

	CPoint p9P = CPoint(1.999*left + width - p9.x, p9.y);
	CPoint p8P = CPoint(1.999*left + width - p8.x, p8.y);
	CPoint p7P = CPoint(1.999*left + width - p7.x, p7.y);
	CPoint p6P = CPoint(1.999*left + width - p6.x, p6.y);
	CPoint p5P = CPoint(1.999*left + width - p5.x, p5.y);
	CPoint p4P = CPoint(1.999*left + width - p4.x, p4.y);
	CPoint p3P = CPoint(1.999*left + width - p3.x, p3.y);
	CPoint p2P = CPoint(1.999*left + width - p2.x, p2.y);

	CPoint line[] = { p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p9P, p8P, p7P, p6P, p5P, p4P, p3P, p2P, p1 };

	pDC->SelectObject(&pen);
	pDC->Polygon(line, 20);
	pDC->SelectObject(oldPen);
	pen.DeleteObject();

	MF = MetaDC.CloseEnhanced();
	MetaDC.DeleteDC();

	return MF;
}

double radian_u_stepen(double radian)
{
	return (180 / PI) * radian;
}

void CCockpit2View::nacrtajAvion(CDC * painter, CRect prozor, CString putanja)
{
	int width = prozor.Width();
	int height = prozor.Height();

	CBitmap bmpImage;
	//BOOL suc = bmpImage.LoadBitmap(IDB_BITMAP6);
	bmpImage.LoadBitmapW(IDB_BITMAP2);
	CBitmap bmpMask;
	BITMAP bm;

	bmpImage.GetBitmap(&bm);

	bmpMask.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	CDC * SrcDC = new CDC();
	SrcDC->CreateCompatibleDC(NULL);
	CDC * DstDC = new CDC();
	DstDC->CreateCompatibleDC(NULL);

	CBitmap * pOldSrcBmp = SrcDC->SelectObject(&bmpImage);
	CBitmap * pOldDstBmp = DstDC->SelectObject(&bmpMask);

	COLORREF clrTopLeft = SrcDC->GetPixel(0, 0);
	COLORREF clrSaveBk = SrcDC->SetBkColor(clrTopLeft);

	DstDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, SrcDC, 0, 0, SRCCOPY);

	COLORREF clrSaveDstText = SrcDC->SetTextColor(RGB(255, 255, 255));
	SrcDC->SetBkColor(RGB(0, 0, 0));
	SrcDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, DstDC, 0, 0, SRCAND);

	DstDC->SetTextColor(clrSaveDstText);
	SrcDC->SetBkColor(clrSaveBk);
	SrcDC->SelectObject(pOldSrcBmp);
	DstDC->SelectObject(pOldDstBmp);
	SrcDC->DeleteDC();
	delete SrcDC;
	DstDC->DeleteDC();
	delete DstDC;

	CDC* MemDC = new CDC();
	MemDC->CreateCompatibleDC(NULL);
	CBitmap * bmpOldT = MemDC->SelectObject(&bmpMask);
	//painter->BitBlt(0,0,bm.bmWidth,bm.bmHeight,MemDC,0,0,SRCAND);
	painter->StretchBlt(width / 3, height / 3, bm.bmWidth / 8, bm.bmHeight / 8, MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCAND);


	MemDC->SelectObject(&bmpImage);

	//painter->BitBlt(0,0,bm.bmWidth,bm.bmHeight,MemDC,0,0,SRCPAINT);
	painter->StretchBlt(width / 3, height / 3, bm.bmWidth / 8, bm.bmHeight / 8, MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCPAINT);

	MemDC->SelectObject(bmpOldT);
	MemDC->DeleteDC();
	delete MemDC;

}

double CCockpit2View::stepen_u_radian(double stepen)
{
	return (PI / 180) * stepen;
}

void CCockpit2View::OnDraw(CDC* pDC)
{
	CCockpit2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	IzotropniMapping(pDC);
}


// CCockpit2View printing


void CCockpit2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CCockpit2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CCockpit2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CCockpit2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CCockpit2View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CCockpit2View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CCockpit2View diagnostics

#ifdef _DEBUG
void CCockpit2View::AssertValid() const
{
	CView::AssertValid();
}

void CCockpit2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCockpit2Doc* CCockpit2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCockpit2Doc)));
	return (CCockpit2Doc*)m_pDocument;
}
#endif //_DEBUG


// CCockpit2View message handlers

void CCockpit2View::IzotropniMapping(CDC * pDC)
{
	CRect rect;
	GetClientRect(&rect);

	pDC->SetMapMode(MM_ISOTROPIC);
	pDC->SetWindowExt(100, 200);
	pDC->SetViewportExt(rect.right, rect.bottom);
	pDC->SetWindowOrg(0, 0);

	double p = 0.05;

	CRect zid;
	zid.SetRect(0, 0, 0, 200);
	if (rect.Height() != 0)
		zid.SetRect(0, 0, 200 * rect.Width() / rect.Height(), 200);


	if (rect.Height() != 0)
		Pravougaonik(pDC, zid, RGB(128, 128, 128));

	InstrumentTabla(pDC, zid, RGB(255, 0, 0));
	//nije neophodno da seposalju tacke testiranje radjeno
	Prozori(pDC, zid, 7, RGB(255, 0, 0));
	ZaobljeniPravougaonik(pDC, zid, RGB(32, 32, 32));

	//Desni donji sat za visinu
	float width = 3 * p*min(15.2*p*zid.Width(), zid.Height());
	int desniSatX = zid.Width() / 2 + 2.9*width;
	int desniSatY = 16.5*p*zid.Height() - 2;
	
	/*//test
	CPoint p1(zid.Width() * 0.698, zid.Height() * 0.83);
	CPoint p2(zid.Width() * 0.77, zid.Height() * 0.978);
	CRect desniSat(p1, p2);*/
	CRect desniSat = CRect(desniSatX, desniSatY, desniSatX + width, desniSatY + width);
	CString vrednosti[7] = { CString("-3"), CString("-2"), CString("-1"), CString("0"), CString("1"), CString("2"), CString("3") };
	nacrtajSat(pDC, zid, desniSat, 13, vrednosti, 7, 120, 420, 2, 45);

	//Levi donji sat za brzinu
	int leviSatX = zid.Width() / 2 - 3.9*width;
	int leviSatY = 16.5*p*zid.Height() - 2;
	/*//test
	p1.SetPoint(zid.Width() * 0.23, zid.Height() * 0.83);
	p2.SetPoint(zid.Width() * 0.3, zid.Height() * 0.978);
	CRect leviSat(p1, p2);*/
	CRect leviSat = CRect(leviSatX, leviSatY, leviSatX + width, leviSatY + width);

	CString vrednosti1[8] = { CString("4"), CString("8"), CString("12"), CString("16"), CString("20"), CString("24"), CString("28") , CString("32") };
	nacrtajSat(pDC, zid, leviSat, 30, vrednosti1, 8, 40, 340, 2, 180);
	//painter->Rectangle(leviSat);

	//Levi gornji sat za kompas
	int leviGornjiSatX = zid.Width() / 2 - 2.8*width;
	int leviGornjiSatY = 15 * p*zid.Height() - 2;
	CRect leviGornjiSat = CRect(leviGornjiSatX, leviGornjiSatY, leviGornjiSatX + width, leviGornjiSatY + width);

	CString vrednosti2[8] = { CString("N"), CString("NE"), CString("E"), CString("SE"), CString("S"), CString("SW"), CString("W"), CString("NW") };
	nacrtajSat(pDC, zid, leviGornjiSat, 16, vrednosti2, 8, 0, 320, 3, 90);

	//Desni gornji sat ziroskop 
	int desniGornjiSatX = zid.Width() / 2 + 1.8*width;
	int desniGornjiSatY = 15 * p*zid.Height() - 2;
	CRect desniGornjiSat = CRect(desniGornjiSatX, desniGornjiSatY, desniGornjiSatX + width, desniGornjiSatY + width);
	nacrtajZiroskop(pDC, zid, desniGornjiSat, 0);

	//testing(pDC, zid);

	//PRVI FUEL
	COLORREF green(RGB(0, 255, 0));
	COLORREF red(RGB(255, 0, 0));
	COLORREF yellow(RGB(255, 255, 0));
	COLORREF fuelColor[] = { red, green, yellow };
	CPoint p1, p2;
	p1.SetPoint(zid.Width() * 0.395, zid.Height() *  0.66);
	p2.SetPoint(zid.Width() * 0.463, zid.Height() *  0.745);
	CRect watch(p1, p2);
	nacrtajFuelWatch(pDC, zid, watch, fuelColor);

	//DRUGI FUEL
	fuelColor[0] = yellow;
	fuelColor[1] = green;
	fuelColor[2] = red;
	p1.SetPoint(zid.Width() * 0.531, zid.Height() * 0.66);
	p2.SetPoint(zid.Width() * 0.6, zid.Height() * 0.745);
	watch.SetRect(p1, p2);
	nacrtajFuelWatch(pDC, zid, watch, fuelColor);
	DrawPoint(pDC, zid);
}

void CCockpit2View::Prozori(CDC * pDC, CRect rect, int length, COLORREF color)
{
	float a = 0.07;
	float b = 2 * a;

	CPoint LeftWnd[16] = {
		CPoint(0, a * rect.Height()),
		CPoint(0, 2 * a * rect.Height()),
		CPoint(0, 2 * a * rect.Height()),
		CPoint(0, 11 * a * rect.Height()),

		CPoint(a * rect.Width(), 10.5 * a * rect.Height()),
		CPoint(a * rect.Width(), 10.5 * a * rect.Height()),
		CPoint(2 * a * rect.Width(), 10 * a * rect.Height()),

		CPoint(2.5 * a * rect.Width(), 9.4 * a * rect.Height()),
		CPoint(2.6 * a * rect.Width(), 8.7 * a * rect.Height()),
		CPoint(2.5 * a * rect.Width(), 8 * a * rect.Height()),

		CPoint(2.167 * a * rect.Width(), 6 * a * rect.Height()),
		CPoint(1.83 * a * rect.Width(), 4 * a * rect.Height()),
		CPoint(1.5 * a * rect.Width(), 2 * a * rect.Height()),

		CPoint(a * rect.Width(), 1.1 * a * rect.Height()),
		CPoint(a * rect.Width(), 1.1 * a * rect.Height()),
		CPoint(0, a * rect.Height()),
	};

	CPoint RightWnd[16];

	float topX2 = 2.25 * a * rect.Width(), topY2 = 2 * a * rect.Height();
	float topX1 = a * rect.Width() + topX2, topY1 = 1.2 * a * rect.Height();
	float middleBottomX1 = 3.25 * a * rect.Width();
	float middleBottomX2 = 4.5 * a * rect.Width();
	float temp = 4 * a * rect.Width();

	CPoint MiddleWnd[25] = {

		CPoint(topX1, topY1),
		CPoint((topX1 + topX2) / 2, topY1 * 0.9),
		CPoint(0.9 * (topX1 + topX2) / 2, topY1 * 0.9),
		CPoint(topX2, topY2),

		CPoint(3.25 * a * rect.Width(), 8 * a * rect.Height()),
		CPoint(3.25 * a * rect.Width(), 8 * a * rect.Height()),
		CPoint(3.25 * a * rect.Width(), 8 * a * rect.Height()),

		CPoint(middleBottomX1, 8.55 * a * rect.Height()),
		CPoint(4 * a * rect.Width(), 9.3 * a * rect.Height()),
		CPoint(middleBottomX2, 9 * a * rect.Height()),

		CPoint(0.5 * rect.Width() + a, 8 * a * rect.Height()),
		CPoint(0.5 * rect.Width() - a, 8 * a * rect.Height()),
		CPoint(rect.Width() - middleBottomX2, 9 * a * rect.Height()),

		CPoint(rect.Width() - temp , 9.3 * a * rect.Height()),
		CPoint(rect.Width() - middleBottomX1, 8.55 * a * rect.Height()),
		CPoint(rect.Width() - middleBottomX1, 8 * a * rect.Height()),

		CPoint(rect.Width() - middleBottomX1, 8 * a * rect.Height()),
		CPoint(rect.Width() - middleBottomX1, 8 * a * rect.Height()),
		CPoint(rect.Width() - topX2, topY2),

		CPoint(rect.Width() - 0.9*(topX1 + topX2) / 2, topY1 * 0.9),
		CPoint(rect.Width() - (topX1 + topX2) / 2, topY1 * 0.9),
		CPoint(rect.Width() - topX1, topY1),

		CPoint(0.5 * rect.Width(), topY1 * 1.2),
		CPoint(0.5 * rect.Width(), topY1 * 1.2),
		CPoint(topX1, topY1),
	};

	for (int i = 0; i < 16; i++)
	{
		RightWnd[i].x = rect.Width() - LeftWnd[i].x;
		RightWnd[i].y = LeftWnd[i].y;
	}



	pDC->BeginPath();

	pDC->PolyBezier(LeftWnd, 16);
	pDC->PolyBezier(RightWnd, 16);
	pDC->PolyBezier(MiddleWnd, 25);

	pDC->EndPath();

	/*CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(128, 224, 255));
	pDC->SelectObject(newBrush);*/
	CPen newPen(PS_SOLID, 5, RGB(0, 0, 0));
	pDC->SelectObject(newPen);

	//////////NEBO//////////////////////
	DImage slika;
	slika.Load(CString("E:\Sky.jpg"));
	CBitmap * mapa = slika.GetBitMap();
	CBrush neboCetka(mapa);
	pDC->SelectObject(neboCetka);
	////////////////////////////////////

	pDC->StrokeAndFillPath();
	
	///////AVION/////////////
	nacrtajAvion(pDC, rect, CString("Suhoj.bmp"));
	//////////////////////////////

	newPen.DeleteObject();
	neboCetka.DeleteObject();

	
}

void CCockpit2View::InstrumentTabla(CDC * pDC, CRect rect, COLORREF color)
{
	float leftX = 0.22 * rect.Width(), topLeftX = leftX * 1.17;
	float firstRoundY = 0.79 * rect.Height();
	float a = 1.12, b = 1.02;
	float coefY = 0.91;
	CPoint InstrumentTable[19] = {

		CPoint(leftX, rect.Height()),
		CPoint(leftX, firstRoundY),
		CPoint(leftX, firstRoundY),
		CPoint(leftX, firstRoundY),

		CPoint(leftX * b, firstRoundY * coefY),
		CPoint(leftX * a,  firstRoundY * coefY),
		CPoint(topLeftX,  firstRoundY * 0.9),

		CPoint(0.5 * rect.Width(), firstRoundY * 0.77),
		CPoint(0.5 * rect.Width(), firstRoundY * 0.77),
		CPoint(rect.Width() - topLeftX, firstRoundY * 0.9),

		CPoint(rect.Width() - topLeftX, firstRoundY * 0.9),
		CPoint(rect.Width() - topLeftX, firstRoundY * 0.9),
		CPoint(rect.Width() - topLeftX, firstRoundY * 0.9),

		CPoint(rect.Width() - leftX * a, firstRoundY * coefY),
		CPoint(rect.Width() - leftX * b, firstRoundY * coefY),
		CPoint(rect.Width() - leftX, firstRoundY),

		CPoint(rect.Width() - leftX, rect.Height() * 0.9),
		CPoint(rect.Width() - leftX, rect.Height() * 0.9),
		CPoint(rect.Width() - leftX, rect.Height()),
	};

	pDC->BeginPath();
	pDC->PolyBezier(InstrumentTable, 19);
	pDC->EndPath();

	CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(64, 64, 64));
	pDC->SelectObject(newBrush);
	CPen newPen(PS_SOLID, 3, RGB(0, 0, 0));
	pDC->SelectObject(newPen);

	pDC->StrokeAndFillPath();
}

void CCockpit2View::Pravougaonik(CDC * pDC, CRect rect, COLORREF color)
{
	CBrush newBrush;
	newBrush.CreateSolidBrush(color);

	CBrush* oldBrush = pDC->SelectObject(&newBrush);

	pDC->Rectangle(rect);

	pDC->SelectObject(oldBrush);

	newBrush.DeleteObject();
}

void CCockpit2View::ZaobljeniPravougaonik(CDC * pDC, CRect wall, COLORREF color)
{
	CBrush newBrush;
	newBrush.CreateSolidBrush(color);
	CPen newPen(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(newPen);
	pDC->SelectObject(newBrush);

	float coef = 0.02;
	float rectWidth = 0.1 * wall.Width();
	float roundCoef = 0.3;

	///////////////My_RoundRect/////////////////
	CPoint leftTop(wall.Width() * 0.478, wall.Height() * 0.978);
	CPoint rightBottom(wall.Width() * 0.38, wall.Height() * 0.745);
	CRect leftRect;
	leftRect.SetRect(leftTop, rightBottom);
	leftTop.SetPoint(wall.Width() * 0.521, wall.Height() * 0.745);
	rightBottom.SetPoint(wall.Width() * 0.617, wall.Height() * 0.978);
	CRect rightRect;
	rightRect.SetRect(leftTop, rightBottom);

	///////////////////////////////////////////
	

	pDC->RoundRect(&leftRect, CPoint(roundCoef * rectWidth, roundCoef * rectWidth));
	CBrush newBrush2(RGB(0, 0, 0));
	pDC->SelectObject(newBrush2);
	pDC->RoundRect(&rightRect, CPoint(roundCoef * rectWidth, roundCoef * rectWidth));

	////////////MAPA////////////////////////
	leftTop.SetPoint(wall.Width() * 0.527, wall.Height() * 0.755);
	rightBottom.SetPoint(wall.Width() * 0.613, wall.Height() * 0.96);
	rightRect.SetRect(leftTop, rightBottom);
	DImage x;
	x.Load(CString("E:\Map.png"));
	CRect pictureSize = new CRect(0, 0, x.Width(), x.Height());
	x.Draw(pDC, pictureSize, rightRect);

	/*DImage slika;
	slika.Load(CString("E:\Map.png"));
	
	CBitmap * mapa = slika.GetBitMap();
	CBrush neboCetka(mapa);
	pDC->SelectObject(neboCetka);
	pDC->BeginPath();
	pDC->RoundRect(&rightRect, CPoint(roundCoef * rectWidth, roundCoef * rectWidth));
	pDC->EndPath();
	pDC->StrokeAndFillPath();*/
	////////////////////////////////////////

	//Write string
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0, 255, 0));
	float topOffset = leftRect.Height() * 0.2;
	float leftOffset = leftRect.Width() * 0.05;

	CFont font;
	int test = leftRect.left * 0.03;
	//int fontSize = abs(leftRect.left * 0.03);
	//int fontSize = abs(test);
	//font.CreateFontW(fontSize,0,0,0,0,0,0,0,0,0,0,0,0,CString("Arial"));

	//////////////////////MY_TEXT//////////////////////////////

	leftTop.SetPoint(wall.Width() * 0.385, wall.Height() * 0.763);
	rightBottom.SetPoint(wall.Width() * 0.47, wall.Height() * 0.96);
	leftRect.SetRect(leftTop, rightBottom);

	font.CreatePointFont(leftRect.Height() * 0.4 + leftRect.Width() * 0.4, CString("Arial"));
	CString s1("358mph\n\nH:28°\n\nA: 284°\n\n1217fpm");
	CString s2("1285m\n\n43°19°29\"\n\n21°54°11\"\n\n08:54:29");

	//////////////////////////////////////////////////////////
	CFont* pOldFont = pDC->SelectObject(&font);


	pDC->DrawText(s1, leftRect, DT_LEFT);
	pDC->DrawText(s2, leftRect, DT_RIGHT);

	pDC->SelectObject(pOldFont);
	font.DeleteObject();
	newBrush.DeleteObject();
	newBrush2.DeleteObject();
}

void CCockpit2View::DrawPoint(CDC * pDC, CRect wall)
{
	int width = wall.Width();
	int height = wall.Height();
	int bottom = wall.bottom;
	int const numberOfPoints = 4;

	int centarX = width * 0.478;
	int TopY = height * 0.75;
	int rightX = width * 0.75;
	int leftX = width * 0.2;

	double ph = -height * 0.12;
	double pw = width * 0.25;
	CPoint points[numberOfPoints] =
	{
		CPoint(wall.Width() * 0.531, wall.Height() * 0.66),
		CPoint(wall.Width() * 0.6, wall.Height() * 0.745),
		/*CPoint(width * 0.44 , height *  0.72),
		CPoint(width * 0.416, height *  0.72),
		CPoint(width * 0.463 , height *  0.745),
		CPoint(width * 0.395, height *  0.745),*/

		//CPoint((width * 0.75 - width * 0.2) / 2 + width * 0.2, height * 0.55),
		//	CPoint(wall.Width() * 0.385, wall.Height() * 0.763),
		//	CPoint(wall.Width() * 0.47, wall.Height() * 0.96),
		//CPoint(centarX * 1.02, height * 0.70),
	};
	int p = width * 0.001 - height * 0.001;
	CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
	pDC->SelectObject(pen);


	for (int i = 0; i < numberOfPoints; i++)
	{
		pDC->MoveTo(points[i]);
		pDC->LineTo(points[i]);
	}
	//pDC->MoveTo(point);
	//pDC->LineTo(point);
	pen.DeleteObject();
}

void CCockpit2View::testing(CDC * pDC, CRect wall)
{
	CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
	pDC->SelectObject(pen);
	CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(32, 32, 32));
	pDC->SelectObject(newBrush);

	int width = wall.Width();
	int height = wall.Height();

	CPoint p1;
	CPoint p2;
	CRect krug;

	/*//KRUGOVI
	///////////// prvi krug
	p1.SetPoint(width * 0.23, height * 0.83);
	p2.SetPoint(width * 0.3, height * 0.978);
	krug.SetRect(p1, p2);

	pDC->Ellipse(krug);
	//////////////////////////////////////

	////////////drugi krug
	p1.SetPoint(width * 0.305, height * 0.78);
	p2.SetPoint(width * 0.372, height * 0.93);
	krug.SetRect(p1, p2);

	pDC->Ellipse(krug);
	///////////////////////////////////

	////////////prvi krug desna strana
	p1.SetPoint(width * 0.623, height * 0.78);
	p2.SetPoint(width * 0.692, height * 0.93);
	krug.SetRect(p1, p2);

	pDC->Ellipse(krug);
	//////////////////////////////////


	////////////drugi krug desna strana
	p1.SetPoint(width * 0.698, height * 0.83);
	p2.SetPoint(width * 0.77, height * 0.978);
	krug.SetRect(p1, p2);

	pDC->Ellipse(krug);
	//////////////////////////////////*/

	/////////ARC
	pDC->BeginPath();

	p1.SetPoint(width * 0.395, height *  0.66);
	p2.SetPoint(width * 0.463, height *  0.745);
	krug.SetRect(p1, p2);
	double pp1W = width * 0.395;
	double pp1H = height *  0.66;
	double pp2W = width * 0.463;
	double pp2H = height *  0.745;
	double bottom = krug.bottom;
	double top = krug.top;
	double lef = krug.left;
	double rig = krug.right;
	double meddd = height *  0.69;
	double wid = krug.Width();
	double heig = krug.Height();

	CPoint p3 = krug.CenterPoint();
	int med = (int)(bottom + top) / 2.03;

	p1.SetPoint(width * 0.395, height *  0.69);
	p2.SetPoint(width * 0.463, height *  0.69);
	pDC->MoveTo(width * 0.44, height *  0.72);
	pDC->ArcTo(krug, p2, p1);

	p1.SetPoint(width * 0.44, height *  0.74);
	p2.SetPoint(width * 0.416, height *  0.7);
	krug.SetRect(p1, p2);
	p1.SetPoint(width * 0.44, height *  0.72);
	p2.SetPoint(width * 0.416, height *  0.72);
	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(krug, p2, p1);

	pDC->EndPath();
	pDC->StrokeAndFillPath();
	//pDC->FloodFill(width * 0.416, height *  0.7, RGB(255, 255, 255));

	// zeleni deo
	CPen pen2(PS_SOLID, 2, RGB(0, 255, 0));
	pDC->SelectObject(pen2);
	p1.SetPoint(width * 0.408, height *  0.675);
	p2.SetPoint(width * 0.452, height *  0.74);
	krug.SetRect(p1, p2);
	p1.SetPoint(width *  0.41, height *  0.69);
	p2.SetPoint(width * 0.45, height *  0.69);
	pDC->Arc(krug, p1, p2);

	// crveni deo
	CPen pen3(PS_SOLID, 2, RGB(255, 0, 0));
	pDC->SelectObject(pen3);
	p2.SetPoint(width * 0.415, height *  0.67);
	pDC->Arc(krug, p1, p2);

	// zuti deo
	CPen pen4(PS_SOLID, 2, RGB(255, 255, 0));
	pDC->SelectObject(pen4);
	p1.SetPoint(width *  0.444, height *  0.67);
	p2.SetPoint(width * 0.45, height *  0.69);
	pDC->SetArcDirection(AD_COUNTERCLOCKWISE);
	pDC->Arc(krug, p2, p1);

	/////////////FUEL2///////////////////

	pDC->SelectObject(pen);

	pDC->BeginPath();
	p1.SetPoint(width * 0.515, height *  0.66);
	p2.SetPoint(width * 0.583, height *  0.745);
	krug.SetRect(p1, p2);
	p1.SetPoint(width * 0.515, height *  0.69);
	p2.SetPoint(width * 0.583, height *  0.69);
	pDC->MoveTo(width * 0.56, height *  0.72);
	pDC->ArcTo(krug, p2, p1);

	p1.SetPoint(width * 0.56, height *  0.74);
	p2.SetPoint(width * 0.536, height *  0.7);
	krug.SetRect(p1, p2);
	p1.SetPoint(width * 0.56, height *  0.72);
	p2.SetPoint(width * 0.536, height *  0.72);
	pDC->SetArcDirection(AD_CLOCKWISE);
	pDC->ArcTo(krug, p2, p1);

	pDC->EndPath();
	pDC->StrokeAndFillPath();

	// zeleni deo
	pDC->SelectObject(pen2);
	p1.SetPoint(width * 0.528, height *  0.675);
	p2.SetPoint(width * 0.572, height *  0.74);
	krug.SetRect(p1, p2);
	p1.SetPoint(width *  0.53, height *  0.69);
	p2.SetPoint(width * 0.57, height *  0.69);
	pDC->Arc(krug, p1, p2);

	// crveni deo
	pDC->SelectObject(pen4);
	p2.SetPoint(width * 0.535, height *  0.67);
	pDC->Arc(krug, p1, p2);

	// zuti deo
	pDC->SelectObject(pen3);
	p1.SetPoint(width *  0.564, height *  0.67);
	p2.SetPoint(width * 0.57, height *  0.69);
	pDC->SetArcDirection(AD_COUNTERCLOCKWISE);
	pDC->Arc(krug, p2, p1);

	/////////////////////////////////////
	///////////STRELICE

	/////////////////////////////////////
	pen4.DeleteObject();
	pen3.DeleteObject();
	pen2.DeleteObject();
	pen.DeleteObject();
	newBrush.DeleteObject();
}


