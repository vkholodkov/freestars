#include "FSClient.h"
#include ".\graphics.h"
#include <wx/image.h>

FSC_Graphics TheGraphics;

DEFINE_CLASS_NAME(ComponentPicture)
BEGIN_TX_LOAD_TABLE(ComponentPicture)
	TX_ITEM(Component)
	TX_ITEM(FileName)
END_TX_LOAD_TABLE()

DEFINE_CLASS_NAME(RaceEmbleme)
BEGIN_TX_LOAD_TABLE(RaceEmbleme)
	TX_ITEM(ID)
	TX_ITEM(FileNameBig)
END_TX_LOAD_TABLE()



const char ComponentPictureVectorName[] = "ComponentPictures";
const char RaceEmblemeVectorName[] = "RaceEmblemes";

//Terraform, minelay, driver and gate icons have some additional texts on them 
static void AddCenterText(wxBitmap &bmp, const wxString &txt);
static void AddTwoTexts(wxBitmap &bmp, const wxString &txt1,const wxString &txt2);


void FSC_Graphics::Load(void)
{
	TiXmlDocument doc("rules\\Graphics.xml");
	doc.SetCondenseWhiteSpace(false);
	if (!doc.LoadFile()) 
	{
		wxMessageBox("Cant load Graphics.xml!","Sorry!");
		return;
	}
	const TiXmlNode * gf = doc.FirstChild("GraphicsFile");
	if (!gf)
	{
		wxMessageBox("Graphics.xml is invalid!","Sorry!");
		return;
	}
#	pragma TBD ("check meta info")

	if (!ComponentPictures.ParseNode(gf))
	{
		wxMessageBox("No component pictures in Graphics.xml!","Sorry!");
		return;
	}


	if (!RaceEmblemes.ParseNode(gf))
	{
		wxMessageBox("No race emblemes in Graphics.xml!","Sorry!");
		return;
	}

	loaded = true;
}

const char *FSC_Graphics::GetBigEmblemeName(int ID) const
{
	for (int i = RaceEmblemes.m_vec.size()-1; i >= 0; i--)
	{
		if (ID == RaceEmblemes.m_vec[i]->ID)
			return RaceEmblemes.m_vec[i]->FileNameBig;
	}
	return NULL;
}



const char *FSC_Graphics::GetComponentPictureName(const FreeStars::Component *comp) const
{
	for (int i = ComponentPictures.m_vec.size()-1; i >= 0; i--)
	{
		if ( comp->GetName().compare (ComponentPictures.m_vec[i]->Component) == 0)
			return ComponentPictures.m_vec[i]->FileName;
	}
	return NULL;
}

#pragma TBD("move component drawing here from shipdesigner")
/* i will move component drawing here
const std::deque<FreeStars::Component *> &comp_que = TheGame->GetComponents();

	for (int i = 0; i < comp_que.size(); i++)
	{
		const char *bmp_name = TheGraphics.GetComponentPictureName(comp_que[i]);
		wxBitmap bmp(bmp_name != NULL?bmp_name:"graphics/Unknown.png", wxBITMAP_TYPE_PNG);
		//some components should have texts
		wxString textie, textie2;
		switch(comp_que[i]->GetType())
		{
		case FreeStars::CT_TERRAFORM:
			textie = "±";
			textie << comp_que[i]->GetTerraLimit();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_MINELAY:
			textie = "";
			textie << comp_que[i]->GetMineAmount();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_DRIVER:
			textie = " "; //driver icons are little off
			textie << comp_que[i]->GetDriverSpeed();
			AddCenterText(bmp, textie);
			break;
		case FreeStars::CT_GATE:
			textie = "";
			textie2 = "";
			if (comp_que[i]->GetGateMass() == -1)
			{
				textie << "OO"; //got lazy to draw 8 on side
			}
			else
			{
				textie << comp_que[i]->GetGateMass() << "kT" ;
			}
			if (comp_que[i]->GetGateRange() == -1)
			{
				textie2 << "OO"; //got lazy to draw 8 on side
			}
			else
			{
				textie2 << comp_que[i]->GetGateRange() << "ly" ;
			}
			AddTwoTexts(bmp, textie, textie2);
			break;
	
		default:
			//no texts
			break;
		}
*/

static void AddTwoTexts(wxBitmap &bmp, const wxString &txt1,const wxString &txt2)
{
	int W_bmp = bmp.GetWidth();
	int H_bmp = bmp.GetHeight();

	//create bmp for drawing 
	wxBitmap temp_bmp(W_bmp, H_bmp);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(temp_bmp);
	//use Almost Black background because of black bordered text
	wxBrush almost_black(wxColour(1,1,1));
	temp_dc.SetBackground(almost_black);
	temp_dc.Clear();
	//draw our text
	temp_dc.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false, _T("Arial")));
	int W_txt1, H_txt1, W_txt2, H_txt2, X_txt1, Y_txt1, X_txt2, Y_txt2;
	temp_dc.GetTextExtent(txt1, &W_txt1, &H_txt1);
	temp_dc.GetTextExtent(txt2, &W_txt2, &H_txt2);
	X_txt1 = (W_bmp-W_txt1)/2;
	X_txt2 = (W_bmp-W_txt2)/2;
	Y_txt1 = (H_bmp-H_txt1-H_txt2-H_txt1/4)/2;
	Y_txt2 = Y_txt1+H_txt1+H_txt1/4;
	temp_dc.SetTextForeground(*wxBLACK);
	temp_dc.DrawText(txt1,X_txt1-1,Y_txt1-1);
	temp_dc.DrawText(txt1,X_txt1+1,Y_txt1-1);
	temp_dc.DrawText(txt1,X_txt1-1,Y_txt1+1);
	temp_dc.DrawText(txt1,X_txt1+1,Y_txt1+1);
	temp_dc.DrawText(txt2,X_txt2-1,Y_txt2-1);
	temp_dc.DrawText(txt2,X_txt2+1,Y_txt2-1);
	temp_dc.DrawText(txt2,X_txt2-1,Y_txt2+1);
	temp_dc.DrawText(txt2,X_txt2+1,Y_txt2+1);
	temp_dc.SetTextForeground(*wxWHITE);
	temp_dc.DrawText(txt1,X_txt1,Y_txt1);
	temp_dc.DrawText(txt2,X_txt2,Y_txt2);
	//convert to image that has background transparent
	wxImage temp_im = temp_bmp.ConvertToImage();
	temp_im.SetMaskColour(1,1,1); //same Almost Black
	temp_im.InitAlpha(); 
	//image goes to another bmp 
	wxBitmap temp_bmp2(temp_im);
	temp_dc.SelectObject(temp_bmp2);
	//and blit drawn text to our original bmp
	wxMemoryDC bmp_dc;
	bmp_dc.SelectObject(bmp);
	bmp_dc.Blit(0, 0, W_bmp, H_bmp, & temp_dc, 0, 0, wxCOPY, false);
	//done! probably destructors do these anyway?
	temp_dc.SelectObject(wxNullBitmap);
	bmp_dc.SelectObject(wxNullBitmap);
};

static void AddCenterText(wxBitmap &bmp, const wxString &txt)
{
	int W_bmp = bmp.GetWidth();
	int H_bmp = bmp.GetHeight();

	//create bmp for drawing 
	wxBitmap temp_bmp(W_bmp, H_bmp);
	wxMemoryDC temp_dc;
	temp_dc.SelectObject(temp_bmp);
	//use Almost White background because of white bordered text
	wxBrush almost_white(wxColour(254,254,254));
	temp_dc.SetBackground(almost_white);
	temp_dc.Clear();
	//draw our text
	temp_dc.SetFont(wxFont(16, wxSWISS, wxNORMAL, wxBOLD, false, _T("Lucida Console")));
	int W_txt, H_txt, X_txt, Y_txt;
	temp_dc.GetTextExtent(txt, &W_txt, &H_txt);
	X_txt = (W_bmp-W_txt)/2;
	Y_txt = (H_bmp-H_txt)/2;
	temp_dc.SetTextForeground(*wxWHITE);
	temp_dc.DrawText(txt,X_txt-1,Y_txt-1);
	temp_dc.DrawText(txt,X_txt+1,Y_txt-1);
	temp_dc.DrawText(txt,X_txt-1,Y_txt+1);
	temp_dc.DrawText(txt,X_txt+1,Y_txt+1);
	temp_dc.SetTextForeground(*wxBLACK);
	temp_dc.DrawText(txt,X_txt,Y_txt);
	//convert to image that has background transparent
	wxImage temp_im = temp_bmp.ConvertToImage();
	temp_im.SetMaskColour(254,254,254); //same Almost White
	temp_im.InitAlpha(); 
	//image goes to another bmp 
	wxBitmap temp_bmp2(temp_im);
	temp_dc.SelectObject(temp_bmp2);
	//and blit drawn text to our original bmp
	wxMemoryDC bmp_dc;
	bmp_dc.SelectObject(bmp);
	bmp_dc.Blit(0, 0, W_bmp, H_bmp, & temp_dc, 0, 0, wxCOPY, false);
	//done! probably destructors do these anyway?
	temp_dc.SelectObject(wxNullBitmap);
	bmp_dc.SelectObject(wxNullBitmap);
};
