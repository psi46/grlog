// rootgui.cpp

#include "stdafx.h"


ClassImp(GrLogMainFrame)

void GrLogMainFrame::AddButton(TGCompositeFrame *frame,
							   char *name, char *caller)
{
	TGTextButton *btn = new TGTextButton(frame,name);
	btn->Connect("Clicked()","GrLogMainFrame",this, caller);
	frame->AddFrame(btn,
		new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,3,3,2,2));
}


GrLogMainFrame::GrLogMainFrame(CWaferDataBase &waferData,
	const TGWindow *p, UInt_t w, UInt_t h)
	: fDatabase(waferData), TGMainFrame(p,w,h)
{
	fMain = new TGMainFrame(p,w,h, kHorizontalFrame);

	// -- embedded canvas
	fEcanvas = new TRootEmbeddedCanvas("ECanvas",fMain, 630, 800); // 470, 600
	fMain->AddFrame(fEcanvas,
		new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 5,5,5,5));

	TCanvas *fCanvas = fEcanvas->GetCanvas();
	CreateSubPads(fCanvas);

	// --- create button frame -------------------------------------------
	TGVerticalFrame *buttonFrame = new TGVerticalFrame(fMain,50,200);

	// --- wafer map selection
	TGButtonGroup *selWmap = new TGButtonGroup(buttonFrame,
		"wafer map", kVerticalFrame);
	TGRadioButton *db, *rb;
	db = new TGRadioButton(selWmap, new TGHotString("c&lass"),     WMAP_CLASS);
	db->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("&fail"),      WMAP_FAILCODE);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("&bin"),       WMAP_BIN);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
//	rb = new TGRadioButton(selWmap, new TGHotString("&group"),     WMAP_PICGROUP);
//	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Idig on"),    WMAP_IDIGON);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Iana on"),    WMAP_IANAON);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Idig init"),  WMAP_IDIGINI);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Iana init"),  WMAP_IANAINI);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("VA@24mA"),    WMAP_VANA);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("VD reg"),WMAP_PC_VDREG);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("CalDel"), WMAP_CALDEL);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("PM"), WMAP_PM);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Ph1mean"), WMAP_PH1MEAN);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Ph21mean"), WMAP_PH21MEAN);
	rb->Connect("Clicked()", "GrLogMainFrame", this, "DoSetMapMode()");


	db->SetState(kButtonDown);
//	rb->Connect("Pressed()","GrLogMainFrame",this,"SetMapMode(=20)");
//	selWmap->Connect("Clicked(ULong_t)","GrLogMainFrame",this,"SetMapMode(ULong_t)");
	buttonFrame->AddFrame(selWmap,
		new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,3,3,2,2));

	// --- button "chip"
	AddButton(buttonFrame, "&chip", "DoOpenChip()");

	// --- button "fail list"
	AddButton(buttonFrame, "fail &list", "DoFailList()");

	// --- button "class list"
	AddButton(buttonFrame, "class list", "DoClassList()");

	// --- button "report"
	AddButton(buttonFrame, "&report", "DoReport()");
	
	// --- button "statistics"
	AddButton(buttonFrame, "&statistics", "DoStatistics()");

	// --- button "pick file"
	AddButton(buttonFrame, "&pick file", "DoGeneratePickFile()");

	// --- button "JSON files"
	AddButton(buttonFrame, "&JSON File", "DoGenerateJSONfile()");
	
	// --- button "XML files"
	AddButton(buttonFrame, "&XML Files", "DoGenerateXMLfiles()");

	// --- button "full report"
	AddButton(buttonFrame, "&full report", "DoFullReport()");

	// --- button "PD report"
	AddButton(buttonFrame, "&pd report", "DoPDReport()");  //for PD results and internal db

	// --- button "PSI report"
	AddButton(buttonFrame, "&pd report", "DoPSIReport()");  //for wafer classification

	// --- button "wafer map"
	AddButton(buttonFrame, "&wafer map", "DoGenerateWaferMap()");

	// --- button "print"
	AddButton(buttonFrame, "p&rint",   "PrintWaferMap()");

	// --- button "Exit"
	TGTextButton *exit = new TGTextButton(buttonFrame,"e&xit",
		"gApplication->Terminate(0)");
	buttonFrame->AddFrame(exit,
		new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,3,3,2,2));

	fMain->AddFrame(buttonFrame,new TGLayoutHints(kLHintsLeft,5,10,3,3));
	// -------------------------------------------------------------------

	SetMapMode(WMAP_CLASS);

	fMain->SetWindowName("Wafer View");
	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();
}


GrLogMainFrame::~GrLogMainFrame()
{
	fMain->Cleanup();
	delete fMain;
}


void GrLogMainFrame::CreateSubPads(TPad *upad)
{
	if (!upad->IsEditable()) return;

	TPad *padsav = (TPad*)gPad;
	upad->cd();

	TPad *pad;
	char *name  = new char [strlen(upad->GetName())+6];

	Int_t color = upad->GetFillColor();

	sprintf(name,"%s_0",upad->GetName());
	pad = new TPad(name,name,0.01,0.33,0.99,0.99,color);
	pad->SetNumber(1);
	pad->Draw();
	pad->SetEditable(kFALSE);

	sprintf(name,"%s_1",upad->GetName());
	pad = new TPad(name,name,0.01,0.01,0.99,0.32,color);
	pad->SetNumber(2);
	pad->Draw();

	delete [] name;

	upad->Modified();
	if (padsav) padsav->cd();
}


void GrLogMainFrame::DoSetMapMode()
{
	TGButton *btn = (TGButton *) gTQSender;
	Int_t id = btn->WidgetId();
	if (id == fWmapMode) return;

//	printf("Wafer map changed to %i\n", id);
	SetMapMode(id);
}


void GrLogMainFrame::SetMapMode(int mode)
{
	if (mode>=0) fWmapMode = mode;

	switch (fWmapMode)
	{
		case WMAP_BIN:
			fWmapInfo.UpdateBin(fDatabase);
			fWmap.UpdateBin(fDatabase);
			break;

		case WMAP_CLASS:
			fWmapInfo.UpdateClass(fDatabase);
			fWmap.UpdateClass(fDatabase);
			break;

		case WMAP_FAILCODE:
			fWmapInfo.UpdateFailCode(fDatabase);
			fWmap.UpdateFailCode(fDatabase);
			break;

/*
		case WMAP_PICGROUP:
			fWmapInfo.UpdatePic(fDatabase);
			fWmap.UpdatePic(fDatabase);
			break;
*/
		case WMAP_IDIGON:
			fWmapInfo.UpdateIdOn(fDatabase);
			fWmap.UpdateIdOn(fDatabase);
			break;
		case WMAP_IANAON:
			fWmapInfo.UpdateIaOn(fDatabase);
			fWmap.UpdateIaOn(fDatabase);
			break;

		case WMAP_IDIGINI:
			fWmapInfo.UpdateIdInit(fDatabase);
			fWmap.UpdateIdInit(fDatabase);
			break;

		case WMAP_IANAINI:
			fWmapInfo.UpdateIaInit(fDatabase);
			fWmap.UpdateIaInit(fDatabase);
			break;

		case WMAP_VANA:
			fWmapInfo.UpdateVanaDAC(fDatabase);
			fWmap.UpdateVanaDAC(fDatabase);
			break;

		case WMAP_PC_VDREG:
			fWmapInfo.UpdateVdreg(fDatabase);
			fWmap.UpdateVdreg(fDatabase);
			break;

		case WMAP_CALDEL:
			fWmapInfo.UpdateCalDel(fDatabase);
			fWmap.UpdateCalDel(fDatabase);
			break;

		case WMAP_PM:
			fWmapInfo.UpdatePm(fDatabase);
			fWmap.UpdatePm(fDatabase);
			break;

		case WMAP_PH1MEAN:
			fWmapInfo.UpdatePh1mean(fDatabase);
			fWmap.UpdatePh1mean(fDatabase);
			break;

		case WMAP_PH21MEAN:
			fWmapInfo.UpdatePh21mean(fDatabase);
			fWmap.UpdatePh21mean(fDatabase);
			break;
	}
	UpdateWmap();
}


void GrLogMainFrame::DoOpenChip()
{
	CChip *chip = fDatabase.GetFirst();
	if (chip==NULL) return;
	CChipView *chipview = 
		new CChipView(chip, gClient->GetRoot(),250,200);
}


void GrLogMainFrame::DoReport() //renamed
{
	std::string filename = gName.GetName_Report();
	_mkdir(gName.GetPath_Report().c_str());
	if (fDatabase.GenerateDataTable(filename))
		printf("Report %s created.\n", filename.c_str());
	else
		printf("Could not create &s!\n", filename.c_str());
}


void GrLogMainFrame::DoClassList() //new
{
	std::string filename = gName.GetName_ClassList();

	_mkdir(gName.GetPath_ClassList().c_str());
	if (fDatabase.GenerateClassList(filename))
		printf("Class list %s created.\n", filename.c_str());
	else
		printf("Could not create class list %s!\n", filename.c_str());
}


void GrLogMainFrame::DoFailList() //renamed.
{
	std::string filename = gName.GetName_FailList();
	_mkdir(gName.GetPath_FailList().c_str());
	if (fDatabase.GenerateErrorReport(filename))
		printf("Fail list %s created.\n", filename.c_str());
	else
		printf("Could not create fail list %s!\n", filename.c_str());
}


void GrLogMainFrame::DoStatistics()
{
	std::string filename = gName.GetName_Statistics();
	//_mkdir(gName.GetPath(s,NULL));
	if (fDatabase.GenerateStatistics(filename))
		printf("Statistics file &s created.\n", filename.c_str());
	else
		printf("Could not create %s!\n", filename.c_str());
}


void GrLogMainFrame::DoGeneratePickFile()
{
	std::string filename = gName.GetName_Pick();
	_mkdir(gName.GetPath_Pick().c_str());
	if (fDatabase.GeneratePickFile(filename))
		printf("Pick file %s created.\n", filename.c_str());
	else
		printf("Could not create %s!\n", filename.c_str());
}


void GrLogMainFrame::DoGenerateJSONfile() //json file for the official db
{
	std::string filename = gName.GetName_JSON();
	_mkdir(gName.GetPath_JSON().c_str());
	if (fDatabase.GenerateJSONfile(filename))
		printf("JSON file %s created.\n", filename.c_str());
	else
		printf("Could not create %s!\n", filename.c_str());
}


void GrLogMainFrame::DoGenerateXMLfiles()
{
	std::string pathname = gName.GetPath_XML();
	_mkdir(pathname.c_str());

	if (fDatabase.GenerateXML(pathname))
		printf("XML-files created.\n");
	else
		printf("Could not create XML-files!\n");
}


void GrLogMainFrame::DoPDReport() //Padova test report
{
	fWmapMode = WMAP_CLASS;
	DoGenerateWaferMap();
	fWmapMode = WMAP_FAILCODE;
	DoGenerateWaferMap();
	DoGenerateJSONfile();
	DoClassList();
}


void GrLogMainFrame::DoFullReport()  //full report -- just maps and json file
{
//	DoFailList();
//	DoReport();
//	DoStatistics();
//	DoGeneratePickFile();
	fWmapMode = WMAP_FAILCODE;
	DoGenerateWaferMap();
	fWmapMode = WMAP_CLASS;
	DoGenerateWaferMap();
	DoGenerateJSONfile();
//	DoGenerateXMLfiles();
}


void GrLogMainFrame::DoGenerateWaferMap()
{
	std::string filename;
	_mkdir(gName.GetPath_WaferMap().c_str());
	unsigned int mode;
	switch (fWmapMode)
	{
		case WMAP_BIN:
			mode = 0; 
			filename = gName.GetName_WaferMap("bin");
			break;
		case WMAP_FAILCODE:
			mode = 1;
			filename = gName.GetName_WaferMap("fail");
			break;
		case WMAP_CLASS:
			mode = 2;
			filename = gName.GetName_WaferMap("class");
			break;
		default: return;
	}
	if (fDatabase.GenerateWaferMap(filename, mode))
		printf("Wafer map %s created.\n", filename.c_str());
	else
		printf("Could not create %s!\n", filename.c_str());
}


void GrLogMainFrame::PrintWaferMap()
{
	char *p;
	switch (fWmapMode)
	{
		case WMAP_BIN:      p = "bin";   break;
		case WMAP_CLASS:    p = "class"; break;
		case WMAP_FAILCODE: p = "fail";  break;
		case WMAP_PICGROUP: p = "pic";   break;
		case WMAP_IDIGON:   p = "idig0"; break;
		case WMAP_IANAON:   p = "iana0"; break;
		case WMAP_IDIGINI:  p = "idigi"; break;
		case WMAP_IANAINI:  p = "ianai"; break;
		case WMAP_VANA:     p = "vana";  break;
		case WMAP_PC_VDREG: p = "vdreg"; break;
		default: return;
	}

	TCanvas *fCanvas = fEcanvas->GetCanvas();
	fCanvas->Print(gName.GetName().c_str());
}


void GrLogMainFrame::UpdateWmap()
{

	TCanvas *fCanvas = fEcanvas->GetCanvas();

	fCanvas->cd(1);
	fWmap.Draw();

	fCanvas->cd(2);
	fWmapInfo.Draw();

	fCanvas->cd();
	fCanvas->Update();
}

