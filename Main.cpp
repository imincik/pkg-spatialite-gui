/*
/ Main.cpp
/ the main core of spatialite_gui  - a SQLite /SpatiaLite GUI tool
/
/ version 1.7, 2013 May 8
/
/ Author: Sandro Furieri a-furieri@lqt.it
/
/ Copyright (C) 2008-2013  Alessandro Furieri
/
/    This program is free software: you can redistribute it and/or modify
/    it under the terms of the GNU General Public License as published by
/    the Free Software Foundation, either version 3 of the License, or
/    (at your option) any later version.
/
/    This program is distributed in the hope that it will be useful,
/    but WITHOUT ANY WARRANTY; without even the implied warranty of
/    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/    GNU General Public License for more details.
/
/    You should have received a copy of the GNU General Public License
/    along with this program.  If not, see <http://www.gnu.org/licenses/>.
/
*/

#include "Classdef.h"

#include "wx/menu.h"
#include "wx/aboutdlg.h"
#include "wx/filename.h"
#include "wx/config.h"
#include "wx/tokenzr.h"

#include <spatialite.h>
#include <proj_api.h>
#include <geos_c.h>

//
// ICONs in XPM format [universally portable]
//
#include "icons/icon.xpm"
#include "icons/icon_info.xpm"
#include "icons/create_new.xpm"
#include "icons/connect.xpm"
#include "icons/disconnect.xpm"
#include "icons/memdb_load.xpm"
#include "icons/memdb_new.xpm"
#include "icons/memdb_clock.xpm"
#include "icons/memdb_save.xpm"
#include "icons/vacuum.xpm"
#include "icons/sql_script.xpm"
#include "icons/composer.xpm"
#include "icons/loadshp.xpm"
#include "icons/virtshp.xpm"
#include "icons/loadtxt.xpm"
#include "icons/virttxt.xpm"
#include "icons/loaddbf.xpm"
#include "icons/virtdbf.xpm"
#include "icons/loadxl.xpm"
#include "icons/virtxl.xpm"
#include "icons/network.xpm"
#include "icons/exif.xpm"
#include "icons/loadxml.xpm"
#include "icons/srids.xpm"
#include "icons/charset.xpm"
#include "icons/help.xpm"
#include "icons/about.xpm"
#include "icons/exit.xpm"
#include "icons/attach.xpm"
#include "icons/sql_log.xpm"
#include "icons/db_status.xpm"
#include "icons/checkgeom.xpm"
#include "icons/sanegeom.xpm"
#include "icons/wfs.xpm"
#include "icons/dxf.xpm"


#if defined(_WIN32) && !defined(__MINGW32__)
#define unlink	_unlink
#endif

IMPLEMENT_APP(MyApp)
     bool MyApp::OnInit()
{
//
// main APP implementation
//
  wxString path;
  if (argc > 1)
    path = argv[1];
  MyFrame *frame =
    new MyFrame(wxT("spatialite_gui      [a GUI tool for SQLite/SpatiaLite]"),
                wxPoint(0, 0), wxSize(700, 480));
  frame->Show(true);
  SetTopWindow(frame);
  frame->LoadConfig(path);
  return true;
}

MyFrame::MyFrame(const wxString & title, const wxPoint & pos, const wxSize & size):
wxFrame((wxFrame *) NULL, -1, title, pos,
        size)
{
//
// main GUI frame constructor
//
  MemoryDatabase = false;
  AutoSaveInterval = 0;
  LastTotalChanges = 0;
  TimerAutoSave = NULL;
  SqlLogEnabled = false;
//
// initializing CHARSET lists
//
  CharsetsLen = 79;
  Charsets = new wxString[CharsetsLen];
  CharsetsNames = new wxString[CharsetsLen];
  *(CharsetsNames + 0) = wxT("ARMSCII-8    Armenian");
  *(CharsetsNames + 1) = wxT("ASCII        US-ASCII");
  *(CharsetsNames + 2) = wxT("BIG5         Chinese/Traditional");
  *(CharsetsNames + 3) = wxT("BIG5-HKSCS   Chinese/Hong Kong");
  *(CharsetsNames + 4) = wxT("BIG5-HKSCS:1999");
  *(CharsetsNames + 5) = wxT("BIG5-HKSCS:2001");
  *(CharsetsNames + 6) = wxT("CP850        DOS/OEM Western Europe");
  *(CharsetsNames + 7) = wxT("CP862        DOS/OEM Hebrew");
  *(CharsetsNames + 8) = wxT("CP866        DOS/OEM Cyrillic");
  *(CharsetsNames + 9) = wxT("CP874        DOS/OEM Thai");
  *(CharsetsNames + 10) = wxT("CP932        DOS/OEM Japanese");
  *(CharsetsNames + 11) = wxT("CP936        DOS/OEM Chinese");
  *(CharsetsNames + 12) = wxT("CP949        DOS/OEM Korean");
  *(CharsetsNames + 13) = wxT("CP950        DOS/OEM Chinese/Big5");
  *(CharsetsNames + 14) = wxT("CP1133       Laotian");
  *(CharsetsNames + 15) = wxT("CP1250       Windows Central Europe");
  *(CharsetsNames + 16) = wxT("CP1251       Windows Cyrillic");
  *(CharsetsNames + 17) = wxT("CP1252       Windows Latin 1");
  *(CharsetsNames + 18) = wxT("CP1253       Windows Greek");
  *(CharsetsNames + 19) = wxT("CP1254       Windows Turkish");
  *(CharsetsNames + 20) = wxT("CP1255       Windows Hebrew");
  *(CharsetsNames + 21) = wxT("CP1256       Windows Arabic");
  *(CharsetsNames + 22) = wxT("CP1257       Windows Baltic");
  *(CharsetsNames + 23) = wxT("CP1258       Windows Vietnamese");
  *(CharsetsNames + 24) = wxT("EUC-CN       Chinese");
  *(CharsetsNames + 25) = wxT("EUC-JP       Japanese");
  *(CharsetsNames + 26) = wxT("EUC-KR       Korean");
  *(CharsetsNames + 27) = wxT("EUC-TW       Taiwan");
  *(CharsetsNames + 28) = wxT("GB18030      Chinese/National Standard");
  *(CharsetsNames + 29) = wxT("GBK          Chinese/Simplified");
  *(CharsetsNames + 30) = wxT("Georgian-Academy");
  *(CharsetsNames + 31) = wxT("Georgian-PS");
  *(CharsetsNames + 32) = wxT("HZ           Chinese");
  *(CharsetsNames + 33) = wxT("ISO-2022-CN  Chinese");
  *(CharsetsNames + 34) = wxT("ISO-2022-CN-EXT");
  *(CharsetsNames + 35) = wxT("ISO-2022-JP  Japanese");
  *(CharsetsNames + 36) = wxT("ISO-2022-JP-1");
  *(CharsetsNames + 37) = wxT("ISO-2022-JP-2");
  *(CharsetsNames + 38) = wxT("ISO-2022-KR  Korean");
  *(CharsetsNames + 39) = wxT("ISO-8859-1   Latin-1 Western European");
  *(CharsetsNames + 40) = wxT("ISO-8859-2   Latin-2 Central European");
  *(CharsetsNames + 41) = wxT("ISO-8859-3   Latin-3 South European");
  *(CharsetsNames + 42) = wxT("ISO-8859-4   Latin-4 North European");
  *(CharsetsNames + 43) = wxT("ISO-8859-5   Latin/Cyrillic");
  *(CharsetsNames + 44) = wxT("ISO-8859-6   Latin/Arabic");
  *(CharsetsNames + 45) = wxT("ISO-8859-7   Latin/Greek");
  *(CharsetsNames + 46) = wxT("ISO-8859-8   Latin/Hebrew");
  *(CharsetsNames + 47) = wxT("ISO-8859-9   Latin-5 Turkish");
  *(CharsetsNames + 48) = wxT("ISO-8859-10  Latin-6 Nordic");
  *(CharsetsNames + 49) = wxT("ISO-8859-11  Latin/Thai");
  *(CharsetsNames + 50) = wxT("ISO-8859-13  Latin-7 Baltic Rim");
  *(CharsetsNames + 51) = wxT("ISO-8859-14  Latin-8 Celtic");
  *(CharsetsNames + 52) = wxT("ISO-8859-15  Latin-9");
  *(CharsetsNames + 53) = wxT("ISO-8859-16  Latin-10 South-Eastern European");
  *(CharsetsNames + 54) = wxT("JOHAB        Korean");
  *(CharsetsNames + 55) = wxT("KOI8-R       Russian");
  *(CharsetsNames + 56) = wxT("KOI8-U       Ukrainian");
  *(CharsetsNames + 57) = wxT("KOI8-RU      Belarusian");
  *(CharsetsNames + 58) = wxT("KOI8-T       Tajik");
  *(CharsetsNames + 59) = wxT("MacArabic    MAC Arabic");
  *(CharsetsNames + 60) = wxT("MacCentralEurope");
  *(CharsetsNames + 61) = wxT("MacCroatian  MAC Croatian");
  *(CharsetsNames + 62) = wxT("MacCyrillic  MAC Cyrillic");
  *(CharsetsNames + 63) = wxT("MacGreek     MAC Greek");
  *(CharsetsNames + 64) = wxT("MacHebrew    MAC Hebrew");
  *(CharsetsNames + 65) = wxT("MacIceland   MAC Iceland");
  *(CharsetsNames + 66) = wxT("Macintosh");
  *(CharsetsNames + 67) = wxT("MacRoman     MAC European/Western languages");
  *(CharsetsNames + 68) = wxT("MacRomania   MAC Romania");
  *(CharsetsNames + 69) = wxT("MacThai      MAC Thai");
  *(CharsetsNames + 70) = wxT("MacTurkish   MAC Turkish");
  *(CharsetsNames + 71) = wxT("MacUkraine   MAC Ukraine");
  *(CharsetsNames + 72) = wxT("MuleLao-1    Laotian");
  *(CharsetsNames + 73) = wxT("PT154        Kazakh");
  *(CharsetsNames + 74) = wxT("RK1048       Kazakh");
  *(CharsetsNames + 75) = wxT("SHIFT_JIS    Japanese");
  *(CharsetsNames + 76) = wxT("TCVN         Vietnamese");
  *(CharsetsNames + 77) = wxT("TIS-620      Thai");
  *(CharsetsNames + 77) = wxT("UTF-8        UNICODE/Universal");
  *(CharsetsNames + 78) = wxT("VISCII       Vietnamese");
  *(Charsets + 0) = wxT("ARMSCII-8");
  *(Charsets + 1) = wxT("ASCII");
  *(Charsets + 2) = wxT("BIG5");
  *(Charsets + 3) = wxT("BIG5-HKSCS");
  *(Charsets + 4) = wxT("BIG5-HKSCS:1999");
  *(Charsets + 5) = wxT("BIG5-HKSCS:2001");
  *(Charsets + 6) = wxT("CP850");
  *(Charsets + 7) = wxT("CP862");
  *(Charsets + 8) = wxT("CP866");
  *(Charsets + 9) = wxT("CP874");
  *(Charsets + 10) = wxT("CP932");
  *(Charsets + 11) = wxT("CP936");
  *(Charsets + 12) = wxT("CP949");
  *(Charsets + 13) = wxT("CP950");
  *(Charsets + 14) = wxT("CP1133");
  *(Charsets + 15) = wxT("CP1250");
  *(Charsets + 16) = wxT("CP1251");
  *(Charsets + 17) = wxT("CP1252");
  *(Charsets + 18) = wxT("CP1253");
  *(Charsets + 19) = wxT("CP1254");
  *(Charsets + 20) = wxT("CP1255");
  *(Charsets + 21) = wxT("CP1256");
  *(Charsets + 22) = wxT("CP1257");
  *(Charsets + 23) = wxT("CP1258");
  *(Charsets + 24) = wxT("EUC-CN");
  *(Charsets + 25) = wxT("EUC-JP");
  *(Charsets + 26) = wxT("EUC-KR");
  *(Charsets + 27) = wxT("EUC-TW");
  *(Charsets + 28) = wxT("GB18030");
  *(Charsets + 29) = wxT("GBK");
  *(Charsets + 30) = wxT("Georgian-Academy");
  *(Charsets + 31) = wxT("Georgian-PS");
  *(Charsets + 32) = wxT("HZ");
  *(Charsets + 33) = wxT("ISO-2022-CN");
  *(Charsets + 34) = wxT("ISO-2022-CN-EXT");
  *(Charsets + 35) = wxT("ISO-2022-JP");
  *(Charsets + 36) = wxT("ISO-2022-JP-1");
  *(Charsets + 37) = wxT("ISO-2022-JP-2");
  *(Charsets + 38) = wxT("ISO-2022-KR");
  *(Charsets + 39) = wxT("ISO-8859-1");
  *(Charsets + 40) = wxT("ISO-8859-2");
  *(Charsets + 41) = wxT("ISO-8859-3");
  *(Charsets + 42) = wxT("ISO-8859-4");
  *(Charsets + 43) = wxT("ISO-8859-5");
  *(Charsets + 44) = wxT("ISO-8859-6");
  *(Charsets + 45) = wxT("ISO-8859-7");
  *(Charsets + 46) = wxT("ISO-8859-8");
  *(Charsets + 47) = wxT("ISO-8859-9");
  *(Charsets + 48) = wxT("ISO-8859-10");
  *(Charsets + 49) = wxT("ISO-8859-11");
  *(Charsets + 50) = wxT("ISO-8859-13");
  *(Charsets + 51) = wxT("ISO-8859-14");
  *(Charsets + 52) = wxT("ISO-8859-15");
  *(Charsets + 53) = wxT("ISO-8859-16");
  *(Charsets + 54) = wxT("JOHAB");
  *(Charsets + 55) = wxT("KOI8-R");
  *(Charsets + 56) = wxT("KOI8-U");
  *(Charsets + 57) = wxT("KOI8-RU");
  *(Charsets + 58) = wxT("KOI8-T");
  *(Charsets + 59) = wxT("MacArabic");
  *(Charsets + 60) = wxT("MacCentralEurope");
  *(Charsets + 61) = wxT("MacCroatian");
  *(Charsets + 62) = wxT("MacCyrillic");
  *(Charsets + 63) = wxT("MacGreek");
  *(Charsets + 64) = wxT("MacHebrew");
  *(Charsets + 65) = wxT("MacIceland");
  *(Charsets + 66) = wxT("Macintosh");
  *(Charsets + 67) = wxT("MacRoman");
  *(Charsets + 68) = wxT("MacRomania");
  *(Charsets + 69) = wxT("MacThai");
  *(Charsets + 70) = wxT("MacTurkish");
  *(Charsets + 71) = wxT("MacUkraine");
  *(Charsets + 72) = wxT("MuleLao-1");
  *(Charsets + 73) = wxT("PT154");
  *(Charsets + 74) = wxT("RK1048");
  *(Charsets + 75) = wxT("SHIFT_JIS");
  *(Charsets + 76) = wxT("TCVN");
  *(Charsets + 77) = wxT("TIS-620");
  *(Charsets + 77) = wxT("UTF-8");
  *(Charsets + 78) = wxT("VISCII");
  LocaleCharset = wxString::FromUTF8(gaiaGetLocaleCharset());
  DefaultCharset = LocaleCharset;
  AskCharset = false;

  HelpPane = false;
  SqliteHandle = NULL;
  InternalCache = NULL;
  SqlitePath = wxT("");
  BtnConnect = new wxBitmap(connect_xpm);
  BtnCreateNew = new wxBitmap(create_new_xpm);
  BtnDisconnect = new wxBitmap(disconnect_xpm);
  BtnMemDbLoad = new wxBitmap(memdb_load_xpm);
  BtnMemDbNew = new wxBitmap(memdb_new_xpm);
  BtnMemDbClock = new wxBitmap(memdb_clock_xpm);
  BtnMemDbSave = new wxBitmap(memdb_save_xpm);
  BtnVacuum = new wxBitmap(vacuum_xpm);
  BtnSqlScript = new wxBitmap(sql_script_xpm);
  BtnQueryComposer = new wxBitmap(composer_xpm);
  BtnLoadShp = new wxBitmap(loadshp_xpm);
  BtnVirtualShp = new wxBitmap(virtshp_xpm);
  BtnLoadTxt = new wxBitmap(loadtxt_xpm);
  BtnVirtualTxt = new wxBitmap(virttxt_xpm);
  BtnLoadDbf = new wxBitmap(loaddbf_xpm);
  BtnVirtualDbf = new wxBitmap(virtdbf_xpm);
  BtnLoadXL = new wxBitmap(loadxl_xpm);
  BtnVirtualXL = new wxBitmap(virtxl_xpm);
  BtnNetwork = new wxBitmap(network_xpm);
  BtnExif = new wxBitmap(exif_xpm);
  BtnLoadXml = new wxBitmap(loadxml_xpm);
  BtnSrids = new wxBitmap(srids_xpm);
  BtnCharset = new wxBitmap(charset_xpm);
  BtnHelp = new wxBitmap(help_xpm);
  BtnAbout = new wxBitmap(about_xpm);
  BtnExit = new wxBitmap(exit_xpm);
  BtnAttach = new wxBitmap(attach_xpm);
  BtnSqlLog = new wxBitmap(sql_log_xpm);
  BtnDbStatus = new wxBitmap(db_status_xpm);
  BtnCheckGeom = new wxBitmap(checkgeom_xpm);
  BtnSaneGeom = new wxBitmap(sanegeom_xpm);
  BtnWFS = new wxBitmap(wfs_xpm);
  BtnDXF = new wxBitmap(dxf_xpm);

//
// setting up the application icon
//      
  wxIcon MyIcon(icon_xpm);
  SetIcon(MyIcon);

//
// setting up panes
//
  TableTree = new MyTableTree(this);
  QueryView = new MyQueryView(this);
  RsView = new MyResultSetView(this);
  Manager.SetManagedWindow(this);
  wxAuiPaneInfo paneSql = wxAuiPaneInfo().Top();
  paneSql.Name(wxT("sql_stmt"));
  paneSql.CaptionVisible(false);
  paneSql.Floatable(true);
  paneSql.Dockable(true);
  paneSql.Movable(true);
  paneSql.Gripper(true);
  paneSql.CloseButton(false);
  paneSql.BestSize(wxSize(200, 120));
  Manager.AddPane(QueryView, paneSql);
  wxAuiPaneInfo paneView = wxAuiPaneInfo().Centre();
  paneView.Name(wxT("result_set"));
  paneView.CaptionVisible(false);
  paneView.Floatable(true);
  paneView.Dockable(true);
  paneView.Movable(true);
  paneView.Gripper(false);
  paneView.CloseButton(false);
  Manager.AddPane(RsView, paneView);
  wxAuiPaneInfo paneTree = wxAuiPaneInfo().Left();
  paneTree.Name(wxT("tree_view"));
  paneTree.CaptionVisible(false);
  paneTree.Floatable(true);
  paneTree.Dockable(true);
  paneTree.Movable(true);
  paneTree.Gripper(true);
  paneTree.CloseButton(false);
  paneTree.BestSize(wxSize(200, 480));
  Manager.AddPane(TableTree, paneTree, wxPoint(0, 10));
  Manager.Update();
  Centre();

//
// setting up the status bar
//
  wxStatusBar *statusBar = new wxStatusBar(this);
  SetStatusBar(statusBar);

//
// setting up the menu bar
//
  wxMenu *menuFile = new wxMenu;
  wxMenuItem *menuItem;
  menuItem =
    new wxMenuItem(menuFile, ID_Connect,
                   wxT("&Connecting an existing SQLite DB"));
  menuItem->SetBitmap(*BtnConnect);
  menuFile->Append(menuItem);
  menuItem =
    new wxMenuItem(menuFile, ID_CreateNew,
                   wxT("Creating a &New (empty) SQLite DB"));
  menuItem->SetBitmap(*BtnCreateNew);
  menuFile->Append(menuItem);
  wxMenu *memoryMenu = new wxMenu();
  menuItem =
    new wxMenuItem(memoryMenu, ID_MemoryDbLoad,
                   wxT("&Loading an existing DB into the MEMORY-DB"));
  menuItem->SetBitmap(*BtnMemDbLoad);
  memoryMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(memoryMenu, ID_MemoryDbNew,
                   wxT("Creating a &New (empty) MEMORY-DB"));
  menuItem->SetBitmap(*BtnMemDbNew);
  memoryMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(memoryMenu, ID_MemoryDbClock,
                   wxT("&AutoSaving the current MEMORY-DB"));
  menuItem->SetBitmap(*BtnMemDbClock);
  memoryMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(memoryMenu, ID_MemoryDbSave,
                   wxT("&Saving the current MEMORY-DB"));
  menuItem->SetBitmap(*BtnMemDbSave);
  memoryMenu->Append(menuItem);
  menuFile->AppendSubMenu(memoryMenu, wxT("&MEMORY-DB"));
  menuItem =
    new wxMenuItem(menuFile, ID_Disconnect,
                   wxT("&Disconnecting current SQLite DB"));
  menuItem->SetBitmap(*BtnDisconnect);
  menuFile->Append(menuItem);
  menuItem =
    new wxMenuItem(menuFile, ID_Vacuum,
                   wxT("&Optimizing current SQLite DB [VACUUM]"));
  menuItem->SetBitmap(*BtnVacuum);
  menuFile->Append(menuItem);
  menuFile->AppendSeparator();
  wxMenu *advancedMenu = new wxMenu();
  menuItem =
    new wxMenuItem(advancedMenu, ID_SqlScript, wxT("&Execute SQL script"));
  menuItem->SetBitmap(*BtnSqlScript);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem =
    new wxMenuItem(advancedMenu, ID_QueryViewComposer,
                   wxT("Query/View &Composer"));
  menuItem->SetBitmap(*BtnQueryComposer);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_LoadShp, wxT("&Load Shapefile"));
  menuItem->SetBitmap(*BtnLoadShp);
  advancedMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(advancedMenu, ID_VirtualShp, wxT("&Virtual Shapefile"));
  menuItem->SetBitmap(*BtnVirtualShp);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_LoadTxt, wxT("Load CSV/&TXT"));
  menuItem->SetBitmap(*BtnLoadTxt);
  advancedMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(advancedMenu, ID_VirtualTxt, wxT("Virtual &CSV/TXT"));
  menuItem->SetBitmap(*BtnVirtualTxt);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_LoadDbf, wxT("Load &DBF"));
  menuItem->SetBitmap(*BtnLoadDbf);
  advancedMenu->Append(menuItem);
  menuItem = new wxMenuItem(advancedMenu, ID_VirtualDbf, wxT("Virtual &DBF"));
  menuItem->SetBitmap(*BtnVirtualDbf);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_LoadXL, wxT("Load &XLS"));
  menuItem->SetBitmap(*BtnLoadXL);
  advancedMenu->Append(menuItem);
  menuItem = new wxMenuItem(advancedMenu, ID_VirtualXL, wxT("Virtual &XLS"));
  menuItem->SetBitmap(*BtnVirtualXL);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_Network, wxT("Build &Network"));
  menuItem->SetBitmap(*BtnNetwork);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem = new wxMenuItem(advancedMenu, ID_Exif, wxT("Import &EXIF photos"));
  menuItem->SetBitmap(*BtnExif);
  advancedMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(advancedMenu, ID_LoadXml, wxT("Import &XML Documents"));
  menuItem->SetBitmap(*BtnLoadXml);
  advancedMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(advancedMenu, ID_WFS,
                   wxT("Import data from &WFS datasource"));
  menuItem->SetBitmap(*BtnWFS);
  advancedMenu->Append(menuItem);
  menuItem = new wxMenuItem(advancedMenu, ID_DXF, wxT("Import &DXF drawings"));
  menuItem->SetBitmap(*BtnDXF);
  advancedMenu->Append(menuItem);
  advancedMenu->AppendSeparator();
  menuItem =
    new wxMenuItem(advancedMenu, ID_Srids, wxT("&Search SRID by name"));
  menuItem->SetBitmap(*BtnSrids);
  advancedMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(advancedMenu, ID_Charset, wxT("&Default Output Charset"));
  menuItem->SetBitmap(*BtnCharset);
  advancedMenu->Append(menuItem);
  menuFile->AppendSubMenu(advancedMenu, wxT("&Advanced"));
  menuFile->AppendSeparator();
  menuItem = new wxMenuItem(menuFile, ID_Attach, wxT("&Attach DataBase"));
  menuItem->SetBitmap(*BtnAttach);
  menuFile->Append(menuItem);
  menuFile->AppendSeparator();
  menuItem =
    new wxMenuItem(menuFile, ID_SqlLog, wxT("&SQL Log"), wxT("&SQL Log"),
                   wxITEM_CHECK);
  menuFile->Append(menuItem);
  menuItem = new wxMenuItem(menuFile, ID_DbStatus, wxT("&DB Status"));
  menuFile->Append(menuItem);
  menuFile->AppendSeparator();
  menuItem = new wxMenuItem(menuFile, ID_CheckGeom, wxT("&Check Geometries"));
  menuFile->Append(menuItem);
  menuItem = new wxMenuItem(menuFile, ID_SaneGeom, wxT("&Sanitize Geometries"));
  menuFile->Append(menuItem);
  menuFile->AppendSeparator();
  menuItem = new wxMenuItem(menuFile, ID_Help, wxT("&Help"));
  menuItem->SetBitmap(*BtnHelp);
  menuItem = new wxMenuItem(menuFile, wxID_ABOUT, wxT("&About ..."));
  menuItem->SetBitmap(*BtnAbout);
  menuFile->Append(menuItem);
  menuFile->AppendSeparator();
  menuItem = new wxMenuItem(menuFile, wxID_EXIT, wxT("&Quit"));
  menuItem->SetBitmap(*BtnExit);
  menuFile->Append(menuItem);
  wxMenuBar *menuBar = new wxMenuBar;
  menuBar->Append(menuFile, wxT("&Files"));
  SetMenuBar(menuBar);

//
// setting up menu initial state 
//
  menuBar->Enable(ID_Disconnect, false);
  menuBar->Enable(ID_MemoryDbClock, false);
  menuBar->Enable(ID_MemoryDbSave, false);
  menuBar->Enable(ID_Vacuum, false);
  menuBar->Enable(ID_SqlScript, false);
  menuBar->Enable(ID_QueryViewComposer, false);
  menuBar->Enable(ID_LoadShp, false);
  menuBar->Enable(ID_VirtualShp, false);
  menuBar->Enable(ID_LoadTxt, false);
  menuBar->Enable(ID_VirtualTxt, false);
  menuBar->Enable(ID_LoadDbf, false);
  menuBar->Enable(ID_VirtualDbf, false);
  menuBar->Enable(ID_LoadXL, false);
  menuBar->Enable(ID_VirtualXL, false);
  menuBar->Enable(ID_Network, false);
  menuBar->Enable(ID_Exif, false);
  menuBar->Enable(ID_LoadXml, false);
  menuBar->Enable(ID_Srids, false);
  menuBar->Enable(ID_Attach, false);
  menuBar->Enable(ID_SqlLog, false);
  menuBar->Enable(ID_DbStatus, false);
  menuBar->Enable(ID_CheckGeom, false);
  menuBar->Enable(ID_SaneGeom, false);
  menuBar->Enable(ID_WFS, false);
  menuBar->Enable(ID_DXF, false);

//
// setting up the toolbar
//      
  wxToolBar *toolBar = CreateToolBar();
  toolBar->AddTool(ID_Connect, wxT("Connecting an existing SQLite DB"),
                   *BtnConnect, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Connecting an existing SQLite DB"));
  toolBar->AddTool(ID_CreateNew, wxT("Creating a &New (empty) SQLite DB"),
                   *BtnCreateNew, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Creating a &New (empty) SQLite DB"));
  toolBar->AddTool(ID_MemoryDbLoad,
                   wxT("Loading an existing DB into the MEMORY-DB"),
                   *BtnMemDbLoad, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Loading an existing DB into the MEMORY-DB"));
  toolBar->AddTool(ID_MemoryDbNew, wxT("Creating a New (empty) MEMORY-DB"),
                   *BtnMemDbNew, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Creating a New (empty) MEMORY-DB"));
  toolBar->AddTool(ID_MemoryDbClock, wxT("AutoSaving the current MEMORY-DB"),
                   *BtnMemDbClock, wxNullBitmap, wxITEM_NORMAL,
                   wxT("AutoSaving the current MEMORY-DB"));
  toolBar->AddTool(ID_MemoryDbSave, wxT("Saving the current MEMORY-DB"),
                   *BtnMemDbSave, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Saving the current MEMORY-DB"));
  toolBar->AddTool(ID_Disconnect, wxT("Disconnecting current SQLite DB"),
                   *BtnDisconnect, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Disconnecting current SQLite DB"));
  toolBar->AddTool(ID_Vacuum, wxT("Optimizing current SQLite DB [VACUUM]"),
                   *BtnVacuum, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Optimizing current SQLite DB [VACUUM]"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Attach, wxT("Attach DataBase"),
                   *BtnAttach, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Attach DataBase"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_SqlLog, wxT("SQL Log"),
                   *BtnSqlLog, wxNullBitmap, wxITEM_CHECK, wxT("SQL Log"));
  toolBar->AddTool(ID_DbStatus, wxT("DB Status"),
                   *BtnDbStatus, wxNullBitmap, wxITEM_NORMAL, wxT("DB Status"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_CheckGeom, wxT("Check Geometries"),
                   *BtnCheckGeom, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Check Geometries"));
  toolBar->AddTool(ID_SaneGeom, wxT("Sanitize Geometries"), *BtnSaneGeom,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Sanitize Geometries"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_SqlScript, wxT("Execute SQL script"), *BtnSqlScript,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Execute SQL script"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_QueryViewComposer, wxT("Query/View Composer"),
                   *BtnQueryComposer, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Query/View Composer"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_LoadShp, wxT("Load Shapefile"), *BtnLoadShp, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Load Shapefile"));
  toolBar->AddTool(ID_VirtualShp, wxT("Virtual Shapefile"), *BtnVirtualShp,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Virtual Shapefile"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_LoadTxt, wxT("Load CSV/TXT"), *BtnLoadTxt, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Load CSV/TXT"));
  toolBar->AddTool(ID_VirtualTxt, wxT("Virtual CSV/TXT"), *BtnVirtualTxt,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Virtual CSV/TXT"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_LoadDbf, wxT("Load DBF"), *BtnLoadDbf, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Load DBF"));
  toolBar->AddTool(ID_VirtualDbf, wxT("Virtual DBF"), *BtnVirtualDbf,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Virtual DBF"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_LoadXL, wxT("Load XLS"), *BtnLoadXL, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Load XLS"));
  toolBar->AddTool(ID_VirtualXL, wxT("Virtual XLS"), *BtnVirtualXL,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Virtual XLS"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Network, wxT("Build Network"), *BtnNetwork, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Build Network"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Exif, wxT("Import EXIF photos"), *BtnExif, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Import EXIF photos"));
  toolBar->AddTool(ID_LoadXml, wxT("Import XML Documents"), *BtnLoadXml,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Import XML Documents"));
  toolBar->AddTool(ID_WFS, wxT("Import data from WFS datasource"), *BtnWFS,
                   wxNullBitmap, wxITEM_NORMAL,
                   wxT("Import data from WFS datasource"));
  toolBar->AddTool(ID_DXF, wxT("Import DXF drawings"), *BtnDXF,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Import DXF drawings"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Srids, wxT("Search SRID by name"), *BtnSrids,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Search SRID by name"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Charset, wxT("Default Output Charset"), *BtnCharset,
                   wxNullBitmap, wxITEM_NORMAL, wxT("Default Output Charset"));
  //toolBar->AddSeparator();
  toolBar->AddTool(ID_Help, wxT("Help"), *BtnHelp, wxNullBitmap, wxITEM_NORMAL,
                   wxT("Help"));
  toolBar->AddTool(wxID_ABOUT, wxT("About ..."), *BtnAbout, wxNullBitmap,
                   wxITEM_NORMAL, wxT("About ..."));
  //toolBar->AddSeparator();
  toolBar->AddTool(wxID_EXIT, wxT("Quit"), *BtnExit, wxNullBitmap,
                   wxITEM_NORMAL, wxT("Quit"));
  toolBar->Realize();
  SetToolBar(toolBar);

//
// setting up the toolbar initial state
//
  toolBar->EnableTool(ID_Disconnect, false);
  toolBar->EnableTool(ID_MemoryDbClock, false);
  toolBar->EnableTool(ID_MemoryDbSave, false);
  toolBar->EnableTool(ID_Vacuum, false);
  toolBar->EnableTool(ID_SqlScript, false);
  toolBar->EnableTool(ID_QueryViewComposer, false);
  toolBar->EnableTool(ID_LoadShp, false);
  toolBar->EnableTool(ID_VirtualShp, false);
  toolBar->EnableTool(ID_LoadTxt, false);
  toolBar->EnableTool(ID_VirtualTxt, false);
  toolBar->EnableTool(ID_LoadDbf, false);
  toolBar->EnableTool(ID_VirtualDbf, false);
  toolBar->EnableTool(ID_LoadXL, false);
  toolBar->EnableTool(ID_VirtualXL, false);
  toolBar->EnableTool(ID_Network, false);
  toolBar->EnableTool(ID_Exif, false);
  toolBar->EnableTool(ID_LoadXml, false);
  toolBar->EnableTool(ID_Srids, false);
  toolBar->EnableTool(ID_Attach, false);
  toolBar->EnableTool(ID_SqlLog, false);
  toolBar->EnableTool(ID_DbStatus, false);
  toolBar->EnableTool(ID_CheckGeom, false);
  toolBar->EnableTool(ID_SaneGeom, false);
  toolBar->EnableTool(ID_WFS, false);
  toolBar->EnableTool(ID_DXF, false);

// updating the status bar
  UpdateStatusBar();

//
// setting up event handlers for menu and toolbar
//
  Connect(ID_Connect, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnConnect);
  Connect(ID_CreateNew, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnCreateNew);
  Connect(ID_Disconnect, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnDisconnect);
  Connect(ID_MemoryDbLoad, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnMemoryDbLoad);
  Connect(ID_MemoryDbNew, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnMemoryDbNew);
  Connect(ID_MemoryDbClock, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnMemoryDbClock);
  Connect(ID_MemoryDbSave, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnMemoryDbSave);
  Connect(ID_Vacuum, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnVacuum);
  Connect(ID_SqlScript, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnSqlScript);
  Connect(ID_QueryViewComposer, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnQueryViewComposer);
  Connect(ID_LoadShp, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnLoadShp);
  Connect(ID_VirtualShp, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnVirtualShp);
  Connect(ID_LoadTxt, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnLoadTxt);
  Connect(ID_VirtualTxt, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnVirtualTxt);
  Connect(ID_LoadDbf, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnLoadDbf);
  Connect(ID_VirtualDbf, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnVirtualDbf);
  Connect(ID_LoadXL, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnLoadXL);
  Connect(ID_VirtualXL, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnVirtualXL);
  Connect(ID_Network, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnNetwork);
  Connect(ID_Exif, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnImportExifPhotos);
  Connect(ID_LoadXml, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnImportXmlDocuments);
  Connect(ID_WFS, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnImportWFS);
  Connect(ID_DXF, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnImportDXF);
  Connect(ID_Srids, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnSrids);
  Connect(ID_Charset, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnCharset);
  Connect(ID_Attach, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnAttachDatabase);
  Connect(ID_SqlLog, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnSqlLog);
  Connect(ID_DbStatus, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnDbStatus);
  Connect(ID_CheckGeom, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnCheckGeometries);
  Connect(ID_SaneGeom, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnSanitizeGeometries);
  Connect(ID_Help, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnHelp);
  Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnAbout);
  Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyFrame::OnQuit);
  Connect(wxID_ANY, wxEVT_MOTION, wxMouseEventHandler(MyFrame::OnMouseMove),
          NULL, this);

//
// setting up event handlers for mouse
//
  TableTree->Connect(wxID_ANY, wxEVT_MOTION,
                     wxMouseEventHandler(MyFrame::OnMouseMove), NULL, this);
  GetMenuBar()->Connect(wxID_ANY, wxEVT_MOTION,
                        wxMouseEventHandler(MyFrame::OnMouseMove), NULL, this);
  GetStatusBar()->Connect(wxID_ANY, wxEVT_MOTION,
                          wxMouseEventHandler(MyFrame::OnMouseMove), NULL,
                          this);
//
// setting up a Timer event handler for AutoSave
//
  Connect(ID_AUTO_SAVE_TIMER, wxEVT_TIMER,
          wxTimerEventHandler(MyFrame::OnTimerAutoSave), NULL, this);
}

MyFrame::~MyFrame()
{
//
// main GUI frame destructor
//
  if (TimerAutoSave)
    {
      TimerAutoSave->Stop();
      delete TimerAutoSave;
    }
  LastDitchMemoryDbSave();
  ConfigLayout = Manager.SavePerspective();
  GetPosition(&ConfigPaneX, &ConfigPaneY);
  GetSize(&ConfigPaneWidth, &ConfigPaneHeight);
  SaveConfig();
  Manager.UnInit();
  if (SqliteHandle)
    sqlite3_close(SqliteHandle);
  if (InternalCache)
    spatialite_cleanup_ex(InternalCache);
  if (BtnConnect != NULL)
    delete BtnConnect;
  if (BtnCreateNew != NULL)
    delete BtnCreateNew;
  if (BtnDisconnect != NULL)
    delete BtnDisconnect;
  if (BtnMemDbLoad != NULL)
    delete BtnMemDbLoad;
  if (BtnMemDbNew != NULL)
    delete BtnMemDbNew;
  if (BtnMemDbClock != NULL)
    delete BtnMemDbClock;
  if (BtnMemDbSave != NULL)
    delete BtnMemDbSave;
  if (BtnVacuum != NULL)
    delete BtnVacuum;
  if (BtnSqlScript != NULL)
    delete BtnSqlScript;
  if (BtnQueryComposer != NULL)
    delete BtnQueryComposer;
  if (BtnLoadShp != NULL)
    delete BtnLoadShp;
  if (BtnVirtualShp != NULL)
    delete BtnVirtualShp;
  if (BtnLoadTxt != NULL)
    delete BtnLoadTxt;
  if (BtnVirtualTxt != NULL)
    delete BtnVirtualTxt;
  if (BtnLoadDbf != NULL)
    delete BtnLoadDbf;
  if (BtnVirtualDbf != NULL)
    delete BtnVirtualDbf;
  if (BtnLoadXL != NULL)
    delete BtnLoadXL;
  if (BtnVirtualXL != NULL)
    delete BtnVirtualXL;
  if (BtnNetwork != NULL)
    delete BtnNetwork;
  if (BtnExif != NULL)
    delete BtnExif;
  if (BtnLoadXml != NULL)
    delete BtnLoadXml;
  if (BtnSrids != NULL)
    delete BtnSrids;
  if (BtnHelp != NULL)
    delete BtnHelp;
  if (BtnAttach != NULL)
    delete BtnAttach;
  if (BtnSqlLog != NULL)
    delete BtnSqlLog;
  if (BtnDbStatus != NULL)
    delete BtnDbStatus;
  if (BtnCheckGeom != NULL)
    delete BtnCheckGeom;
  if (BtnSaneGeom != NULL)
    delete BtnSaneGeom;
  if (BtnAbout != NULL)
    delete BtnAbout;
  if (BtnExit != NULL)
    delete BtnExit;
  if (Charsets)
    delete[]Charsets;
  if (CharsetsNames)
    delete[]CharsetsNames;
}

void MyFrame::SaveConfig()
{
//
// saves layout configuration
//

  wxConfig *config = new wxConfig(wxT("SpatialiteGui"));
  config->Write(wxT("Layout"), ConfigLayout);
  config->Write(wxT("PaneX"), ConfigPaneX);
  config->Write(wxT("PaneY"), ConfigPaneY);
  config->Write(wxT("PaneWidth"), ConfigPaneWidth);
  config->Write(wxT("PaneHeight"), ConfigPaneHeight);
  config->Write(wxT("SqlitePath"), SqlitePath);
  config->Write(wxT("LastDirectory"), LastDirectory);
  delete config;
}

void MyFrame::LoadConfig(wxString & externalPath)
{
//
// loads layout configuration
//
  ConfigLayout = wxT("");
  ConfigDbPath = wxT("");
  ConfigDir = wxT("");
  wxConfig *config = new wxConfig(wxT("SpatialiteGui"));
  config->Read(wxT("Layout"), &ConfigLayout);
  config->Read(wxT("PaneX"), &ConfigPaneX, -1);
  config->Read(wxT("PaneY"), &ConfigPaneY, -1);
  config->Read(wxT("PaneWidth"), &ConfigPaneWidth, -1);
  config->Read(wxT("PaneHeight"), &ConfigPaneHeight, -1);
  config->Read(wxT("SqlitePath"), &ConfigDbPath);
  config->Read(wxT("LastDirectory"), &ConfigDir);
  delete config;
  Hide();
  if (externalPath.Len() > 0)
    {
      // applying the external path
      wxFileName file(externalPath);
      ConfigDir = file.GetPath();
      ConfigDbPath = externalPath;
    }
  if (ConfigLayout.Len() > 0)
    Manager.LoadPerspective(ConfigLayout, true);
  if (ConfigPaneX >= 0 && ConfigPaneY >= 0 && ConfigPaneWidth > 0
      && ConfigPaneHeight > 0)
    SetSize(ConfigPaneX, ConfigPaneY, ConfigPaneWidth, ConfigPaneHeight);
  if (ConfigDir.Len() > 0)
    LastDirectory = ConfigDir;
  if (ConfigDbPath.Len() > 0)
    {
      SqlitePath = ConfigDbPath;
      if (OpenDB() == false)
        SqlitePath = wxT("");
      else
        {
          bool metadata = CheckMetadata();
          wxMenuBar *menuBar = GetMenuBar();
          menuBar->Enable(ID_Connect, false);
          menuBar->Enable(ID_MemoryDbLoad, false);
          menuBar->Enable(ID_MemoryDbNew, false);
          if (MemoryDatabase == true)
            {
              menuBar->Enable(ID_MemoryDbSave, true);
              menuBar->Enable(ID_MemoryDbClock, true);
          } else
            {
              menuBar->Enable(ID_MemoryDbSave, false);
              menuBar->Enable(ID_MemoryDbClock, false);
            }
          menuBar->Enable(ID_CreateNew, false);
          menuBar->Enable(ID_Disconnect, true);
          menuBar->Enable(ID_Vacuum, true);
          menuBar->Enable(ID_SqlScript, true);
          menuBar->Enable(ID_QueryViewComposer, HasViewsMetadata());
          menuBar->Enable(ID_LoadShp, true);
          menuBar->Enable(ID_VirtualShp, true);
          menuBar->Enable(ID_LoadTxt, true);
          menuBar->Enable(ID_VirtualTxt, true);
          menuBar->Enable(ID_LoadDbf, true);
          menuBar->Enable(ID_VirtualDbf, true);
          menuBar->Enable(ID_LoadXL, true);
          menuBar->Enable(ID_VirtualXL, true);
          menuBar->Enable(ID_Network, true);
          menuBar->Enable(ID_Exif, true);
          menuBar->Enable(ID_LoadXml, true);
          menuBar->Enable(ID_WFS, true);
          menuBar->Enable(ID_DXF, true);
          menuBar->Enable(ID_Srids, metadata);
          menuBar->Enable(ID_Attach, true);
          menuBar->Enable(ID_SqlLog, true);
          menuBar->Enable(ID_DbStatus, true);
          menuBar->Enable(ID_CheckGeom, true);
          menuBar->Enable(ID_SaneGeom, true);
          EnableSqlLog();
          menuBar->Check(ID_SqlLog, SqlLogEnabled);
          wxToolBar *toolBar = GetToolBar();
          toolBar->EnableTool(ID_Connect, false);
          toolBar->EnableTool(ID_MemoryDbLoad, false);
          toolBar->EnableTool(ID_MemoryDbNew, false);
          if (MemoryDatabase == true)
            {
              toolBar->EnableTool(ID_MemoryDbSave, true);
              toolBar->EnableTool(ID_MemoryDbClock, true);
          } else
            {
              toolBar->EnableTool(ID_MemoryDbSave, false);
              toolBar->EnableTool(ID_MemoryDbClock, false);
            }
          toolBar->EnableTool(ID_CreateNew, false);
          toolBar->EnableTool(ID_Disconnect, true);
          toolBar->EnableTool(ID_Vacuum, true);
          toolBar->EnableTool(ID_SqlScript, true);
          toolBar->EnableTool(ID_QueryViewComposer, HasViewsMetadata());
          toolBar->EnableTool(ID_LoadShp, true);
          toolBar->EnableTool(ID_VirtualShp, true);
          toolBar->EnableTool(ID_LoadTxt, true);
          toolBar->EnableTool(ID_VirtualTxt, true);
          toolBar->EnableTool(ID_LoadDbf, true);
          toolBar->EnableTool(ID_VirtualDbf, true);
          toolBar->EnableTool(ID_LoadXL, true);
          toolBar->EnableTool(ID_VirtualXL, true);
          toolBar->EnableTool(ID_Network, true);
          toolBar->EnableTool(ID_Exif, true);
          toolBar->EnableTool(ID_LoadXml, true);
          toolBar->EnableTool(ID_WFS, true);
          toolBar->EnableTool(ID_DXF, true);
          toolBar->EnableTool(ID_Srids, metadata);
          toolBar->EnableTool(ID_Attach, true);
          toolBar->EnableTool(ID_SqlLog, true);
          toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
          toolBar->EnableTool(ID_DbStatus, true);
          toolBar->EnableTool(ID_CheckGeom, true);
          toolBar->EnableTool(ID_SaneGeom, true);
          UpdateStatusBar();
        }
    }
  Show();
  if (AutoFDOmsg.Len() > 0)
    {
      wxMessageBox(AutoFDOmsg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                   this);
      AutoFDOmsg = wxT("");
    }
}

void MyFrame::OnQuit(wxCommandEvent & WXUNUSED(event))
{
//
// EXIT - event handler
//
  CloseDB();
  Close(true);
}

void MyFrame::OnAttachDatabase(wxCommandEvent & WXUNUSED(event))
{
//
// Attach Database - event handler
//
  int ret;
  wxString lastDir;
  wxString path;
  wxString suffixList =
    wxT("SpatiaLite DB (*.sqlite;*.atlas)|*.sqlite;*.atlas|");
  suffixList += wxT("SQLite DB (*.sqlite)|*.sqlite|");
  suffixList += wxT("LibreAtlas DB (*.atlas)|*.atlas|");
  suffixList += wxT("All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("Attach DataBase"),
                          wxT(""), wxT("db.sqlite"), suffixList,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      if (DoAttachDatabase(path) == true)
        {
          wxFileName file(fileDialog.GetPath());
          lastDir = file.GetPath();
          SetLastDirectory(lastDir);
          InitTableTree();
        }
    }
}

void MyFrame::EnableSqlLog()
{
//
// enables the SQL Log if the currently connected DB is v.4.0.0
//
  if (GetMetaDataType() == METADATA_CURRENT)
    SqlLogEnabled = true;
  else
    SqlLogEnabled = false;
}

void MyFrame::OnSqlLog(wxCommandEvent & WXUNUSED(event))
{
//
// SQL Log - event handler
//
  wxMenuBar *menuBar = GetMenuBar();
  wxToolBar *toolBar = GetToolBar();
  if (SqlLogEnabled == true)
    {
      SqlLogEnabled = false;
      menuBar->Check(ID_SqlLog, false);
      toolBar->ToggleTool(ID_SqlLog, false);
  } else
    {
      EnableSqlLog();
      menuBar->Check(ID_SqlLog, SqlLogEnabled);
      toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
    }
}

void MyFrame::OnDbStatus(wxCommandEvent & WXUNUSED(event))
{
//
// DB Status and internal statistics
//
  DbStatusDialog *stats = new DbStatusDialog();
  stats->Create(this);
  stats->Show();
}

void MyFrame::OnCheckGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// Checking all geometry columns
//
  wxString msg =
    wxT("Do you really intend checking for validity all Geometries ?\n\n");
  msg +=
    wxT
    ("This will imply evaluating ST_IsValid() for each single Geometry stored\n");
  msg +=
    wxT
    ("within any \"layer\" defined in \"geometry_columns\", and could require\n");
  msg += wxT("a substantial time for a huge DB\n\n");
  msg += wxT("A HTML diagnostic report will be created.");
  int ret = wxMessageBox(msg, wxT("spatialite_gui"), wxYES_NO | wxICON_QUESTION,
                         this);
  if (ret != wxYES)
    return;

  char *err_msg = NULL;
  char output_dir[1024];
  wxDirDialog dirDialog(this, wxT("Directory for Diagnostic Report"));
  ret = dirDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(output_dir, dirDialog.GetPath().ToUTF8());
      ::wxBeginBusyCursor();
      int invalids;
      ret =
        check_all_geometry_columns(GetSqlite(), output_dir, &invalids,
                                   &err_msg);
      ::wxEndBusyCursor();
      if (ret == 0)
        {
          // reporting some error condition
          msg = wxT("Some unexpected error occurred:\n\n");
          if (err_msg != NULL)
            {
              msg += wxString::FromUTF8(err_msg);
              free(err_msg);
          } else
            msg += wxT("Sorry, no further details are available");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      } else
        {
          int mode;
          if (invalids > 0)
            {
              msg =
                wxT
                ("ATTENTION: some invalid Geometries have been detected !!!\n\n");
              mode = wxICON_WARNING;
          } else
            {
              msg =
                wxT
                ("No invalid Geometries have been detected; the whole DB is full valid\n\n");
              mode = wxICON_INFORMATION;
            }
          msg += wxT("A full diagnostic report has been created.\n");
          msg +=
            wxT
            ("Please point your WEB Browser at the following HTML document containing the report:\n\n");
#if defined(_WIN32)
          const char *delim = "\\";
#else
          const char *delim = "/";
#endif
          msg +=
            dirDialog.GetPath() + wxString::FromUTF8(delim) + wxT("index.html");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | mode, this);
        }
    }
}

void MyFrame::OnSanitizeGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// Sanitizing all invalid geometries
//
  int ret;
  SanitizeAllGeometriesDialog dlg;
  dlg.Create(this);
  ret = dlg.ShowModal();
  if (ret != wxYES)
    return;

  char tmp_prefix[1024];
  char *err_msg = NULL;
  char output_dir[1024];
  wxString msg;
  strcpy(tmp_prefix, dlg.GetTmpPrefix().ToUTF8());

  wxDirDialog dirDialog(this, wxT("Directory for Diagnostic Report"));
  ret = dirDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(output_dir, dirDialog.GetPath().ToUTF8());
      ::wxBeginBusyCursor();
      int not_validated;
      ret =
        sanitize_all_geometry_columns(GetSqlite(), tmp_prefix, output_dir,
                                      &not_validated, &err_msg);
      ::wxEndBusyCursor();
      if (ret == 0)
        {
          // reporting some error condition
          msg = wxT("Some unexpected error occurred:\n\n");
          if (err_msg != NULL)
            {
              msg += wxString::FromUTF8(err_msg);
              free(err_msg);
          } else
            msg += wxT("Sorry, no further details are available");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      } else
        {
          int mode;
          if (not_validated > 0)
            {
              msg =
                wxT
                ("ATTENTION: some invalid Geometries still remain invalid !!!\n\n");
              mode = wxICON_WARNING;
          } else
            {
              msg =
                wxT
                ("All invalid Geometries have been saned; the whole DB is now full valid\n\n");
              mode = wxICON_INFORMATION;
            }
          msg += wxT("A full diagnostic report has been created.\n");
          msg +=
            wxT
            ("Please point your WEB Browser at the following HTML document containing the report:\n\n");
#if defined(_WIN32)
          const char *delim = "\\";
#else
          const char *delim = "/";
#endif
          msg +=
            dirDialog.GetPath() + wxString::FromUTF8(delim) + wxT("index.html");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | mode, this);
        }
    }
}

void MyFrame::OnHelp(wxCommandEvent & WXUNUSED(event))
{
//
// HELP - event handler
//
  if (HelpPane == true)
    return;
  HelpDialog *help = new HelpDialog(this);
  help->Show();
}

void MyFrame::OnAbout(wxCommandEvent & WXUNUSED(event))
{
//
// ABOUT dialog - event handler
//
  bool has_libxml2 = false;
  char ver[128];
  wxAboutDialogInfo dlg;
  dlg.SetIcon(wxIcon(icon_info_xpm));
  dlg.SetName(wxT("spatialite_gui"));
  const char *version = VERSION;
  dlg.SetVersion(wxString::FromUTF8(version));
  wxString str = wxT("a GUI-tool for SQLite / SpatiaLite\n\n");
  sprintf(ver, "%d.%d.%d", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER);
  str += wxT("wxWidgets version ") + wxString::FromUTF8(ver) + wxT("\n");
  strcpy(ver, spatialite_version());
  str += wxT("SpatiaLite version ") + wxString::FromUTF8(ver) + wxT("\n");
  strcpy(ver, sqlite3_libversion());
  str += wxT("SQLite version ") + wxString::FromUTF8(ver) + wxT("\n");
  strcpy(ver, GEOSversion());
  str += wxT("GEOS version ") + wxString::FromUTF8(ver) + wxT("\n");
  strcpy(ver, pj_get_release());
  str += wxT("PROJ.4 version ") + wxString::FromUTF8(ver) + wxT("\n");
  if (GetLwGeomVersion(ver))
    {
      // printing out the LWGEOM version if supported
      str += wxT("LWGEOM version ") + wxString::FromUTF8(ver) + wxT("\n");
    }
  if (GetLibXml2Version(ver))
    {
      // printing out the LIBXML2 version if supported
      str += wxT("LIBXML2 version ") + wxString::FromUTF8(ver) + wxT("\n");
      has_libxml2 = true;
    }
  str += wxT("\nSQLite's extension 'SpatiaLite' enabled\n");
  str += wxT("SQLite's extension 'VirtualShape' enabled\n");
  str += wxT("SQLite's extension 'VirtualDbf' enabled\n");
  str += wxT("SQLite's extension 'VirtualXL' enabled\n");
  str += wxT("SQLite's extension 'VirtualText' enabled\n");
  if (has_libxml2)
    str += wxT("SQLite's extension 'VirtualXPath' enabled\n");
  str += wxT("SQLite's extension 'VirtualNetwork' enabled\n");
  str += wxT("SQLite's extension 'RTree' enabled\n");
  str += wxT("SQLite's extension 'MbrCache' enabled\n");
  str += wxT("SQLite's extension 'VirtualFDO' enabled\n\n");
  dlg.SetDescription(str);
  dlg.SetCopyright(wxT("by Alessandro Furieri - 2008/2013"));
  dlg.SetWebSite(wxT("http://www.gaia-gis.it"));
  wxString license =
    wxT("This program is free software; you can redistribute it\n");
  license +=
    wxT("and/or modify it under the terms of the GNU General Public License\n");
  license += wxT("(GPL) as published by the Free Software Foundation\n\n");
  license +=
    wxT
    ("A copy of the GPL can be found at\nhttp://www.gnu.org/licenses/gpl.txt");
  dlg.SetLicense(license);
  ::wxAboutBox(dlg);
}

void MyFrame::OnMouseMove(wxMouseEvent & WXUNUSED(event))
{
//
// MOUSE motion - event handler
//
  UpdateStatusBar();
}

void MyFrame::UpdateStatusBar()
{
//
// updating the status bar
//
  if (GetStatusBar() == NULL)
    return;
  if (MemoryDatabase == true)
    {
      GetStatusBar()->SetStatusText(wxT("Current SQLite DB: MEMORY-DB"), 0);
      QueryView->ShowControls();
  } else
    {
      if (SqlitePath.Len() < 1)
        GetStatusBar()->SetStatusText(wxT("not connected"), 0);
      else
        GetStatusBar()->SetStatusText(wxT("Current SQLite DB: ") + SqlitePath,
                                      0);
      if (SqlitePath.Len() < 1)
        {
          QueryView->HideControls();
          RsView->HideControls();
      } else
        QueryView->ShowControls();
    }
}

void MyFrame::OnConnect(wxCommandEvent & WXUNUSED(event))
{
//
// connecting to an existent SQLite DB
//
  int ret;
  wxString lastDir;
  wxString suffixList =
    wxT("SpatiaLite DB (*.sqlite;*.atlas)|*.sqlite;*.atlas|");
  suffixList += wxT("SQLite DB (*.sqlite)|*.sqlite|");
  suffixList += wxT("LibreAtlas DB (*.atlas)|*.atlas|");
  suffixList += wxT("All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("DB connection"),
                          wxT(""), wxT("db.sqlite"), suffixList,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      SqlitePath = fileDialog.GetPath();
      if (OpenDB() == false)
        SqlitePath = wxT("");
      else
        {
          wxFileName file(fileDialog.GetPath());
          lastDir = file.GetPath();
          SetLastDirectory(lastDir);
          bool metadata = CheckMetadata();
          wxMenuBar *menuBar = GetMenuBar();
          menuBar->Enable(ID_Connect, false);
          menuBar->Enable(ID_CreateNew, false);
          menuBar->Enable(ID_Disconnect, true);
          menuBar->Enable(ID_MemoryDbLoad, false);
          menuBar->Enable(ID_MemoryDbNew, false);
          if (MemoryDatabase == true)
            {
              menuBar->Enable(ID_MemoryDbSave, true);
              menuBar->Enable(ID_MemoryDbClock, true);
          } else
            {
              menuBar->Enable(ID_MemoryDbSave, false);
              menuBar->Enable(ID_MemoryDbClock, false);
            }
          menuBar->Enable(ID_Vacuum, true);
          menuBar->Enable(ID_SqlScript, true);
          menuBar->Enable(ID_QueryViewComposer, HasViewsMetadata());
          menuBar->Enable(ID_LoadShp, true);
          menuBar->Enable(ID_VirtualShp, true);
          menuBar->Enable(ID_LoadTxt, true);
          menuBar->Enable(ID_VirtualTxt, true);
          menuBar->Enable(ID_LoadDbf, true);
          menuBar->Enable(ID_VirtualDbf, true);
          menuBar->Enable(ID_LoadXL, true);
          menuBar->Enable(ID_VirtualXL, true);
          menuBar->Enable(ID_Network, true);
          menuBar->Enable(ID_Exif, true);
          menuBar->Enable(ID_LoadXml, true);
          menuBar->Enable(ID_WFS, true);
          menuBar->Enable(ID_DXF, true);
          menuBar->Enable(ID_Srids, metadata);
          menuBar->Enable(ID_Attach, true);
          menuBar->Enable(ID_SqlLog, true);
          menuBar->Enable(ID_DbStatus, true);
          menuBar->Enable(ID_CheckGeom, true);
          menuBar->Enable(ID_SaneGeom, true);
          EnableSqlLog();
          menuBar->Check(ID_SqlLog, SqlLogEnabled);
          wxToolBar *toolBar = GetToolBar();
          toolBar->EnableTool(ID_Connect, false);
          toolBar->EnableTool(ID_CreateNew, false);
          toolBar->EnableTool(ID_Disconnect, true);
          toolBar->EnableTool(ID_MemoryDbLoad, false);
          toolBar->EnableTool(ID_MemoryDbNew, false);
          if (MemoryDatabase == true)
            {
              toolBar->EnableTool(ID_MemoryDbSave, true);
              toolBar->EnableTool(ID_MemoryDbClock, true);
          } else
            {
              toolBar->EnableTool(ID_MemoryDbSave, false);
              toolBar->EnableTool(ID_MemoryDbClock, false);
            }
          toolBar->EnableTool(ID_Vacuum, true);
          toolBar->EnableTool(ID_SqlScript, true);
          toolBar->EnableTool(ID_QueryViewComposer, HasViewsMetadata());
          toolBar->EnableTool(ID_LoadShp, true);
          toolBar->EnableTool(ID_VirtualShp, true);
          toolBar->EnableTool(ID_LoadTxt, true);
          toolBar->EnableTool(ID_VirtualTxt, true);
          toolBar->EnableTool(ID_LoadDbf, true);
          toolBar->EnableTool(ID_VirtualDbf, true);
          toolBar->EnableTool(ID_LoadXL, true);
          toolBar->EnableTool(ID_VirtualXL, true);
          toolBar->EnableTool(ID_Network, true);
          toolBar->EnableTool(ID_Exif, true);
          toolBar->EnableTool(ID_LoadXml, true);
          toolBar->EnableTool(ID_WFS, true);
          toolBar->EnableTool(ID_DXF, true);
          toolBar->EnableTool(ID_Srids, metadata);
          toolBar->EnableTool(ID_Attach, true);
          toolBar->EnableTool(ID_SqlLog, true);
          toolBar->EnableTool(ID_DbStatus, true);
          toolBar->EnableTool(ID_CheckGeom, true);
          toolBar->EnableTool(ID_SaneGeom, true);
          toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
          UpdateStatusBar();
        }
    }
  if (AutoFDOmsg.Len() > 0)
    {
      wxMessageBox(AutoFDOmsg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                   this);
      AutoFDOmsg = wxT("");
    }
}

void MyFrame::OnDisconnect(wxCommandEvent & WXUNUSED(event))
{
//
// disconnecting current SQLite DB
//
  if (TimerAutoSave)
    {
      TimerAutoSave->Stop();
      delete TimerAutoSave;
      TimerAutoSave = NULL;
    }
  CloseDB();
  ExternalSqlitePath = wxT("");
  wxMenuBar *menuBar = GetMenuBar();
  menuBar->Enable(ID_Connect, true);
  menuBar->Enable(ID_CreateNew, true);
  menuBar->Enable(ID_Disconnect, false);
  menuBar->Enable(ID_MemoryDbLoad, true);
  menuBar->Enable(ID_MemoryDbNew, true);
  menuBar->Enable(ID_MemoryDbSave, false);
  menuBar->Enable(ID_MemoryDbClock, false);
  menuBar->Enable(ID_Vacuum, false);
  menuBar->Enable(ID_SqlScript, false);
  menuBar->Enable(ID_QueryViewComposer, false);
  menuBar->Enable(ID_LoadShp, false);
  menuBar->Enable(ID_VirtualShp, false);
  menuBar->Enable(ID_LoadTxt, false);
  menuBar->Enable(ID_VirtualTxt, false);
  menuBar->Enable(ID_LoadDbf, false);
  menuBar->Enable(ID_VirtualDbf, false);
  menuBar->Enable(ID_LoadXL, false);
  menuBar->Enable(ID_VirtualXL, false);
  menuBar->Enable(ID_Network, false);
  menuBar->Enable(ID_Exif, false);
  menuBar->Enable(ID_LoadXml, false);
  menuBar->Enable(ID_WFS, false);
  menuBar->Enable(ID_DXF, false);
  menuBar->Enable(ID_Srids, false);
  menuBar->Enable(ID_Attach, false);
  menuBar->Enable(ID_SqlLog, false);
  menuBar->Enable(ID_DbStatus, false);
  menuBar->Enable(ID_CheckGeom, false);
  menuBar->Enable(ID_SaneGeom, false);
  SqlLogEnabled = false;
  wxToolBar *toolBar = GetToolBar();
  toolBar->EnableTool(ID_Connect, true);
  toolBar->EnableTool(ID_CreateNew, true);
  toolBar->EnableTool(ID_Disconnect, false);
  toolBar->EnableTool(ID_MemoryDbLoad, true);
  toolBar->EnableTool(ID_MemoryDbNew, true);
  toolBar->EnableTool(ID_MemoryDbSave, false);
  toolBar->EnableTool(ID_MemoryDbClock, false);
  toolBar->EnableTool(ID_Vacuum, false);
  toolBar->EnableTool(ID_SqlScript, false);
  toolBar->EnableTool(ID_QueryViewComposer, false);
  toolBar->EnableTool(ID_LoadShp, false);
  toolBar->EnableTool(ID_VirtualShp, false);
  toolBar->EnableTool(ID_LoadTxt, false);
  toolBar->EnableTool(ID_VirtualTxt, false);
  toolBar->EnableTool(ID_LoadDbf, false);
  toolBar->EnableTool(ID_VirtualDbf, false);
  toolBar->EnableTool(ID_LoadXL, false);
  toolBar->EnableTool(ID_VirtualXL, false);
  toolBar->EnableTool(ID_Network, false);
  toolBar->EnableTool(ID_Exif, false);
  toolBar->EnableTool(ID_LoadXml, false);
  toolBar->EnableTool(ID_WFS, false);
  toolBar->EnableTool(ID_DXF, false);
  toolBar->EnableTool(ID_Srids, false);
  toolBar->EnableTool(ID_Attach, false);
  toolBar->EnableTool(ID_SqlLog, false);
  toolBar->ToggleTool(ID_SqlLog, false);
  toolBar->EnableTool(ID_DbStatus, false);
  toolBar->ToggleTool(ID_DbStatus, false);
  toolBar->EnableTool(ID_CheckGeom, false);
  toolBar->ToggleTool(ID_CheckGeom, false);
  toolBar->EnableTool(ID_SaneGeom, false);
  toolBar->ToggleTool(ID_SaneGeom, false);
  UpdateStatusBar();
}

void MyFrame::OnCreateNew(wxCommandEvent & WXUNUSED(event))
{
//
// creating a new, empty SQLite DB
//
  int retdlg;
  int ret;
  wxString lastDir;
  bool metadata;
  wxFileDialog fileDialog(this, wxT("Creating a new, empty DB"),
                          wxT(""), wxT("db.sqlite"),
                          wxT
                          ("SQLite DB (*.sqlite)|*.sqlite|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  retdlg = fileDialog.ShowModal();
  if (retdlg == wxID_OK)
    {
      // creating the new DB
      SqlitePath = fileDialog.GetPath();
      ret = CreateDB();
      if (ret == false)
        goto error;
      metadata = CheckMetadata();
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      wxMenuBar *menuBar = GetMenuBar();
      menuBar->Enable(ID_Connect, false);
      menuBar->Enable(ID_CreateNew, false);
      menuBar->Enable(ID_Disconnect, true);
      menuBar->Enable(ID_MemoryDbLoad, false);
      menuBar->Enable(ID_MemoryDbNew, false);
      menuBar->Enable(ID_MemoryDbSave, false);
      menuBar->Enable(ID_MemoryDbClock, false);
      menuBar->Enable(ID_Vacuum, true);
      menuBar->Enable(ID_SqlScript, true);
      menuBar->Enable(ID_QueryViewComposer, HasViewsMetadata());
      menuBar->Enable(ID_LoadShp, true);
      menuBar->Enable(ID_VirtualShp, true);
      menuBar->Enable(ID_LoadTxt, true);
      menuBar->Enable(ID_VirtualTxt, true);
      menuBar->Enable(ID_LoadDbf, true);
      menuBar->Enable(ID_VirtualDbf, true);
      menuBar->Enable(ID_LoadXL, true);
      menuBar->Enable(ID_VirtualXL, true);
      menuBar->Enable(ID_Network, true);
      menuBar->Enable(ID_Exif, true);
      menuBar->Enable(ID_LoadXml, true);
      menuBar->Enable(ID_WFS, true);
      menuBar->Enable(ID_DXF, true);
      menuBar->Enable(ID_Srids, metadata);
      menuBar->Enable(ID_Attach, true);
      menuBar->Enable(ID_SqlLog, true);
      menuBar->Enable(ID_DbStatus, true);
      menuBar->Enable(ID_CheckGeom, true);
      menuBar->Enable(ID_SaneGeom, true);
      EnableSqlLog();
      menuBar->Check(ID_SqlLog, SqlLogEnabled);
      wxToolBar *toolBar = GetToolBar();
      toolBar->EnableTool(ID_Connect, false);
      toolBar->EnableTool(ID_CreateNew, false);
      toolBar->EnableTool(ID_Disconnect, true);
      toolBar->EnableTool(ID_MemoryDbLoad, false);
      toolBar->EnableTool(ID_MemoryDbNew, false);
      toolBar->EnableTool(ID_MemoryDbSave, false);
      toolBar->EnableTool(ID_MemoryDbClock, false);
      toolBar->EnableTool(ID_Vacuum, true);
      toolBar->EnableTool(ID_SqlScript, true);
      toolBar->EnableTool(ID_QueryViewComposer, HasViewsMetadata());
      toolBar->EnableTool(ID_LoadShp, true);
      toolBar->EnableTool(ID_VirtualShp, true);
      toolBar->EnableTool(ID_LoadTxt, true);
      toolBar->EnableTool(ID_VirtualTxt, true);
      toolBar->EnableTool(ID_LoadDbf, true);
      toolBar->EnableTool(ID_VirtualDbf, true);
      toolBar->EnableTool(ID_LoadXL, true);
      toolBar->EnableTool(ID_VirtualXL, true);
      toolBar->EnableTool(ID_Network, true);
      toolBar->EnableTool(ID_Exif, true);
      toolBar->EnableTool(ID_LoadXml, true);
      toolBar->EnableTool(ID_WFS, true);
      toolBar->EnableTool(ID_DXF, true);
      toolBar->EnableTool(ID_Srids, metadata);
      toolBar->EnableTool(ID_Attach, true);
      toolBar->EnableTool(ID_SqlLog, true);
      toolBar->EnableTool(ID_DbStatus, true);
      toolBar->EnableTool(ID_CheckGeom, true);
      toolBar->EnableTool(ID_SaneGeom, true);
      toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
      UpdateStatusBar();
      return;
  } else
    return;
error:
  unlink(SqlitePath.ToUTF8());
  wxString msg = wxT("An error occurred\nno DB was created");
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  return;
}

void MyFrame::OnMemoryDbLoad(wxCommandEvent & WXUNUSED(event))
{
//
// loading an external DB into the MEMORY-DB
//
  sqlite3 *extSqlite = NULL;
  sqlite3_backup *backup;
  int retdlg;
  int ret;
  wxString lastDir;
  char path[1024];
  wxString error;
  char *errMsg = NULL;
  wxFileDialog fileDialog(this,
                          wxT("Loading an existing DB into the MEMORY-DB"),
                          wxT(""), wxT("db.sqlite"),
                          wxT
                          ("SQLite DB (*.sqlite)|*.sqlite|All files (*.*)|*.*"),
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  retdlg = fileDialog.ShowModal();
  if (retdlg == wxID_OK)
    {
      // opening the external DB
      ExternalSqlitePath = fileDialog.GetPath();
      strcpy(path, ExternalSqlitePath.ToUTF8());
      ret = sqlite3_open_v2(path, &extSqlite, SQLITE_OPEN_READWRITE, NULL);
      if (ret)
        {
          // an error occurred
          wxString errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          error = wxT("Failure while connecting to DB\n\n");
          error += errCause;
          error += wxT("\n");
          goto stop;
        }
      ret =
        sqlite3_open_v2(":memory:", &SqliteHandle,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
      if (ret)
        {
          // an error occurred
          wxString errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          error = wxT("MEMORY-DB: an error occurred \n\n");
          error += errCause;
          error += +wxT("\n");
          goto stop;
        }
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      backup = sqlite3_backup_init(SqliteHandle, "main", extSqlite, "main");
      if (!backup)
        goto stop;
      while (1)
        {
          ret = sqlite3_backup_step(backup, 1024);
          if (ret == SQLITE_DONE)
            break;
        }
      ret = sqlite3_backup_finish(backup);
      sqlite3_close(extSqlite);
// setting up the internal cache
      InternalCache = spatialite_alloc_connection();
      spatialite_init_ex(SqliteHandle, InternalCache, 0);
// activating Foreign Key constraints
      ret =
        sqlite3_exec(SqliteHandle, "PRAGMA foreign_keys = 1", NULL, 0, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("Unable to activate FOREIGN_KEY constraints"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          goto stop;
        }
      MemoryDatabase = true;
      AutoSaveInterval = 120;
      AutoFDOStart();
      InitTableTree();
      bool metadata = CheckMetadata();
      wxMenuBar *menuBar = GetMenuBar();
      menuBar->Enable(ID_Connect, false);
      menuBar->Enable(ID_CreateNew, false);
      menuBar->Enable(ID_Disconnect, true);
      menuBar->Enable(ID_MemoryDbLoad, false);
      menuBar->Enable(ID_MemoryDbNew, false);
      if (MemoryDatabase == true)
        {
          menuBar->Enable(ID_MemoryDbSave, true);
          menuBar->Enable(ID_MemoryDbClock, true);
      } else
        {
          menuBar->Enable(ID_MemoryDbSave, false);
          menuBar->Enable(ID_MemoryDbClock, false);
        }
      menuBar->Enable(ID_Vacuum, true);
      menuBar->Enable(ID_SqlScript, true);
      menuBar->Enable(ID_QueryViewComposer, HasViewsMetadata());
      menuBar->Enable(ID_LoadShp, true);
      menuBar->Enable(ID_VirtualShp, true);
      menuBar->Enable(ID_LoadTxt, true);
      menuBar->Enable(ID_VirtualTxt, true);
      menuBar->Enable(ID_LoadDbf, true);
      menuBar->Enable(ID_VirtualDbf, true);
      menuBar->Enable(ID_LoadXL, true);
      menuBar->Enable(ID_VirtualXL, true);
      menuBar->Enable(ID_Network, true);
      menuBar->Enable(ID_Exif, true);
      menuBar->Enable(ID_LoadXml, true);
      menuBar->Enable(ID_WFS, true);
      menuBar->Enable(ID_DXF, true);
      menuBar->Enable(ID_Srids, metadata);
      menuBar->Enable(ID_Attach, true);
      menuBar->Enable(ID_SqlLog, true);
      menuBar->Enable(ID_DbStatus, true);
      menuBar->Enable(ID_CheckGeom, true);
      menuBar->Enable(ID_SaneGeom, true);
      EnableSqlLog();
      menuBar->Check(ID_SqlLog, SqlLogEnabled);
      wxToolBar *toolBar = GetToolBar();
      toolBar->EnableTool(ID_Connect, false);
      toolBar->EnableTool(ID_CreateNew, false);
      toolBar->EnableTool(ID_Disconnect, true);
      toolBar->EnableTool(ID_MemoryDbLoad, false);
      toolBar->EnableTool(ID_MemoryDbNew, false);
      if (MemoryDatabase == true)
        {
          toolBar->EnableTool(ID_MemoryDbSave, true);
          toolBar->EnableTool(ID_MemoryDbClock, true);
      } else
        {
          toolBar->EnableTool(ID_MemoryDbSave, false);
          toolBar->EnableTool(ID_MemoryDbClock, false);
        }
      toolBar->EnableTool(ID_Vacuum, true);
      toolBar->EnableTool(ID_SqlScript, true);
      toolBar->EnableTool(ID_QueryViewComposer, HasViewsMetadata());
      toolBar->EnableTool(ID_LoadShp, true);
      toolBar->EnableTool(ID_VirtualShp, true);
      toolBar->EnableTool(ID_LoadTxt, true);
      toolBar->EnableTool(ID_VirtualTxt, true);
      toolBar->EnableTool(ID_LoadDbf, true);
      toolBar->EnableTool(ID_VirtualDbf, true);
      toolBar->EnableTool(ID_LoadXL, true);
      toolBar->EnableTool(ID_VirtualXL, true);
      toolBar->EnableTool(ID_Network, true);
      toolBar->EnableTool(ID_Exif, true);
      toolBar->EnableTool(ID_LoadXml, true);
      toolBar->EnableTool(ID_WFS, true);
      toolBar->EnableTool(ID_DXF, true);
      toolBar->EnableTool(ID_Srids, metadata);
      toolBar->EnableTool(ID_Attach, true);
      toolBar->EnableTool(ID_SqlLog, true);
      toolBar->EnableTool(ID_DbStatus, true);
      toolBar->EnableTool(ID_CheckGeom, true);
      toolBar->EnableTool(ID_SaneGeom, true);
      toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
      UpdateStatusBar();
      if (AutoSaveInterval <= 0)
        {
          if (TimerAutoSave)
            {
              TimerAutoSave->Stop();
              delete TimerAutoSave;
              TimerAutoSave = NULL;
            }
      } else
        {
          //
          // starting the AutoSave timer
          //
          if (!TimerAutoSave)
            TimerAutoSave = new wxTimer(this, ID_AUTO_SAVE_TIMER);
          else
            TimerAutoSave->Stop();
          LastTotalChanges = 0;
          TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
        }
    }
  if (AutoFDOmsg.Len() > 0)
    {
      wxMessageBox(AutoFDOmsg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                   this);
      AutoFDOmsg = wxT("");
    }
  return;
stop:
  MemoryDatabase = false;
  if (SqliteHandle)
    sqlite3_close(SqliteHandle);
  if (extSqlite)
    sqlite3_close(extSqlite);
  if (InternalCache)
    spatialite_cleanup_ex(InternalCache);
  wxString msg = wxT("MEMORY-DB wasn't loaded\n\n");
  msg += error;
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  SqliteHandle = NULL;
  InternalCache = NULL;
}

void MyFrame::OnMemoryDbNew(wxCommandEvent & WXUNUSED(event))
{
//
//creating a new MEMORY-DB
//
  int ret;
  wxToolBar *toolBar;
  wxMenuBar *menuBar;
  bool metadata;
  MemoryDatabase = true;
  AutoSaveInterval = 120;
  ExternalSqlitePath = wxT("");
// creating the new MEMORY-DB
  ret = CreateDB();
  if (ret == false)
    goto error;
  metadata = CheckMetadata();
  menuBar = GetMenuBar();
  menuBar->Enable(ID_Connect, false);
  menuBar->Enable(ID_CreateNew, false);
  menuBar->Enable(ID_Disconnect, true);
  menuBar->Enable(ID_MemoryDbLoad, false);
  menuBar->Enable(ID_MemoryDbNew, false);
  menuBar->Enable(ID_MemoryDbSave, true);
  menuBar->Enable(ID_MemoryDbClock, true);
  menuBar->Enable(ID_Vacuum, true);
  menuBar->Enable(ID_SqlScript, true);
  menuBar->Enable(ID_QueryViewComposer, HasViewsMetadata());
  menuBar->Enable(ID_LoadShp, true);
  menuBar->Enable(ID_VirtualShp, true);
  menuBar->Enable(ID_LoadTxt, true);
  menuBar->Enable(ID_VirtualTxt, true);
  menuBar->Enable(ID_LoadDbf, true);
  menuBar->Enable(ID_VirtualDbf, true);
  menuBar->Enable(ID_LoadXL, true);
  menuBar->Enable(ID_VirtualXL, true);
  menuBar->Enable(ID_Network, true);
  menuBar->Enable(ID_Exif, true);
  menuBar->Enable(ID_LoadXml, true);
  menuBar->Enable(ID_WFS, true);
  menuBar->Enable(ID_DXF, true);
  menuBar->Enable(ID_Srids, metadata);
  menuBar->Enable(ID_Attach, true);
  menuBar->Enable(ID_SqlLog, true);
  menuBar->Enable(ID_DbStatus, true);
  menuBar->Enable(ID_CheckGeom, true);
  menuBar->Enable(ID_SaneGeom, true);
  EnableSqlLog();
  menuBar->Check(ID_SqlLog, SqlLogEnabled);
  toolBar = GetToolBar();
  toolBar->EnableTool(ID_Connect, false);
  toolBar->EnableTool(ID_CreateNew, false);
  toolBar->EnableTool(ID_Disconnect, true);
  toolBar->EnableTool(ID_MemoryDbLoad, false);
  toolBar->EnableTool(ID_MemoryDbNew, false);
  toolBar->EnableTool(ID_MemoryDbSave, true);
  toolBar->EnableTool(ID_MemoryDbClock, true);
  toolBar->EnableTool(ID_Vacuum, true);
  toolBar->EnableTool(ID_SqlScript, true);
  toolBar->EnableTool(ID_QueryViewComposer, HasViewsMetadata());
  toolBar->EnableTool(ID_LoadShp, true);
  toolBar->EnableTool(ID_VirtualShp, true);
  toolBar->EnableTool(ID_LoadTxt, true);
  toolBar->EnableTool(ID_VirtualTxt, true);
  toolBar->EnableTool(ID_LoadDbf, true);
  toolBar->EnableTool(ID_VirtualDbf, true);
  toolBar->EnableTool(ID_LoadXL, true);
  toolBar->EnableTool(ID_VirtualXL, true);
  toolBar->EnableTool(ID_Network, true);
  toolBar->EnableTool(ID_Exif, true);
  toolBar->EnableTool(ID_LoadXml, true);
  toolBar->EnableTool(ID_WFS, true);
  toolBar->EnableTool(ID_DXF, true);
  toolBar->EnableTool(ID_Srids, metadata);
  toolBar->EnableTool(ID_Attach, true);
  toolBar->EnableTool(ID_SqlLog, true);
  toolBar->EnableTool(ID_DbStatus, true);
  toolBar->EnableTool(ID_CheckGeom, true);
  toolBar->EnableTool(ID_SaneGeom, true);
  toolBar->ToggleTool(ID_SqlLog, SqlLogEnabled);
  UpdateStatusBar();
  if (AutoSaveInterval <= 0)
    {
      if (TimerAutoSave)
        {
          TimerAutoSave->Stop();
          delete TimerAutoSave;
          TimerAutoSave = NULL;
        }
  } else
    {
      //
      // starting the AutoSave timer
      //
      if (!TimerAutoSave)
        TimerAutoSave = new wxTimer(this, ID_AUTO_SAVE_TIMER);
      else
        TimerAutoSave->Stop();
      LastTotalChanges = 0;
      TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
    }
  return;
error:
  wxString msg = wxT("An error occurred\nno MEMORY-DB was created");
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  return;
}

bool MyFrame::MemoryDbSave()
{
//
// trying to export the MEMORY-DB into an external DB
//
  sqlite3 *extSqlite = NULL;
  sqlite3_backup *backup;
  char path[1024];
  char bak_path[1024];
  int ret;
  wxString error;
  if (ExternalSqlitePath.Len() == 0)
    return false;
  ::wxBeginBusyCursor();
  strcpy(path, ExternalSqlitePath.ToUTF8());
  strcpy(bak_path, path);
  strcat(bak_path, ".bak");
  unlink(bak_path);
  rename(path, bak_path);
  ret =
    sqlite3_open_v2(path, &extSqlite,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (ret)
    {
      // an error occurred
      wxString errCause = wxString::FromUTF8(sqlite3_errmsg(extSqlite));
      error = wxT("An error occurred\n\n");
      error += errCause;
      error += +wxT("\n");
      error += ExternalSqlitePath;
      goto stop;
    }
  backup = sqlite3_backup_init(extSqlite, "main", SqliteHandle, "main");
  if (!backup)
    goto stop;
  while (1)
    {
      ret = sqlite3_backup_step(backup, 1024);
      if (ret == SQLITE_DONE)
        break;
    }
  ret = sqlite3_backup_finish(backup);
  sqlite3_close(extSqlite);
  unlink(bak_path);
  ::wxEndBusyCursor();
  LastTotalChanges = sqlite3_total_changes(SqliteHandle);
  return true;
stop:
  if (extSqlite)
    sqlite3_close(extSqlite);
  wxString msg = wxT("Backup failure: MEMORY-DB wasn't saved\n\n");
  msg += error;
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  ExternalSqlitePath = wxT("");
  ::wxEndBusyCursor();
  return false;
}

void MyFrame::OnMemoryDbSave(wxCommandEvent & WXUNUSED(event))
{
//
//  exporting the MEMORY-DB into an external DB 
//
  int retdlg;
  wxString lastDir;
  if (ExternalSqlitePath.Len() > 0)
    {
      if (MemoryDbSave() == true)
        {
          wxMessageBox(wxT("Ok, MEMORY-DB was succesfully saved"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          if (AutoSaveInterval <= 0)
            {
              if (TimerAutoSave)
                {
                  TimerAutoSave->Stop();
                  delete TimerAutoSave;
                  TimerAutoSave = NULL;
                }
          } else
            {
              //
              // restarting the AutoSave timer
              //
              if (!TimerAutoSave)
                TimerAutoSave = new wxTimer(this, ID_AUTO_SAVE_TIMER);
              else
                TimerAutoSave->Stop();
              TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
            }
          return;
        }
    }
  wxFileDialog fileDialog(this, wxT("Saving the MEMORY-DB"),
                          wxT(""), wxT("db.sqlite"),
                          wxT
                          ("SQLite DB (*.sqlite)|*.sqlite|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  retdlg = fileDialog.ShowModal();
  if (retdlg == wxID_OK)
    {
      // exporting the external DB
      ExternalSqlitePath = fileDialog.GetPath();
      if (MemoryDbSave() == true)
        {
          wxMessageBox(wxT("Ok, MEMORY-DB was succesfully saved"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          wxFileName file(fileDialog.GetPath());
          lastDir = file.GetPath();
          SetLastDirectory(lastDir);
          if (AutoSaveInterval <= 0)
            {
              if (TimerAutoSave)
                {
                  TimerAutoSave->Stop();
                  delete TimerAutoSave;
                  TimerAutoSave = NULL;
                }
          } else
            {
              //
              // restarting the AutoSave timer
              //
              if (!TimerAutoSave)
                TimerAutoSave = new wxTimer(this, ID_AUTO_SAVE_TIMER);
              else
                TimerAutoSave->Stop();
              TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
            }
        }
    }
}

void MyFrame::OnMemoryDbClock(wxCommandEvent & WXUNUSED(event))
{
//
//  setting up AutoSave for MEMORY-DB 
//
  AutoSaveDialog dlg;
  dlg.Create(this, ExternalSqlitePath, AutoSaveInterval);
  int ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      AutoSaveInterval = dlg.GetSeconds();
      if (AutoSaveInterval <= 0)
        {
          //
          // stopping the AutoSave timer
          //
          if (TimerAutoSave)
            {
              TimerAutoSave->Stop();
              delete TimerAutoSave;
              TimerAutoSave = NULL;
            }
      } else
        {
          //
          // restarting the AutoSave timer
          //
          if (!TimerAutoSave)
            TimerAutoSave = new wxTimer(this, ID_AUTO_SAVE_TIMER);
          else
            TimerAutoSave->Stop();
          TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
        }
    }
}

void MyFrame::DbPagesCount(int *total, int *frees)
{
//
// computing the DB pages count
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  char *value;
  *total = 0;
  *frees = 0;
  sql = wxT("PRAGMA page_count");
  ret = sqlite3_get_table(GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 0];
          *total = atoi(value);
        }
    }
  sqlite3_free_table(results);
  sql = wxT("PRAGMA freelist_count");
  ret = sqlite3_get_table(GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 0];
          *frees = atoi(value);
        }
    }
  sqlite3_free_table(results);
}

bool MyFrame::GetLwGeomVersion(char *buf)
{
//
// retieving the LWGEOM version (if enabled)
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  bool ok = false;

  if (GetSqlite() == NULL)
    return false;
  ret = sqlite3_get_table(GetSqlite(), "SELECT lwgeom_version()", &results,
                          &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    return false;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          const char *version = results[(i * columns) + 0];
          if (version != NULL)
            {
              strcpy(buf, version);
              ok = true;
            }
        }
    }
  sqlite3_free_table(results);
  return ok;
}

bool MyFrame::GetLibXml2Version(char *buf)
{
//
// retieving the LIBXML2 version (if enabled)
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  bool ok = false;

  if (GetSqlite() == NULL)
    return false;
  ret = sqlite3_get_table(GetSqlite(), "SELECT libxml2_version()", &results,
                          &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    return false;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          const char *version = results[(i * columns) + 0];
          if (version != NULL)
            {
              strcpy(buf, version);
              ok = true;
            }
        }
    }
  sqlite3_free_table(results);
  return ok;
}

void MyFrame::OnVacuum(wxCommandEvent & WXUNUSED(event))
{
//
// performing a VACUUM in order to reorganize the current DB
//
  char *errMsg = NULL;
  int totalPages;
  int freePages;
  int totalPages2;
  int freePages2;
  wxString msg;
  char dummy[128];
  DbPagesCount(&totalPages, &freePages);
  if (!freePages)
    {
      msg = wxT("The current DB doesn't requires to be VACUUMed\n\n");
      msg += wxT("Total Pages: ");
      sprintf(dummy, "%d\n", totalPages);
      msg += wxString::FromUTF8(dummy);
      msg += wxT("Free Pages: 0\n\n");
      msg += wxT("Free Ratio: 0.0%");
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
      return;
  } else
    {
      double ratio = 100.0 / ((double) totalPages / (double) freePages);
      if (ratio >= 33.33)
        msg = wxT("The current DB urgently requires to be VACUUMed\n\n");
      else if (ratio >= 10.0)
        msg = wxT("The current DB may usefully be VACUUMed\n\n");
      else
        msg =
          wxT("The current DB doesn't strictly requires to be VACUUMed\n\n");
      msg += wxT("Total Pages: ");
      sprintf(dummy, "%d\n", totalPages);
      msg += wxString::FromUTF8(dummy);
      msg += wxT("Free Pages: ");
      sprintf(dummy, "%d\n\n", freePages);
      msg += wxString::FromUTF8(dummy);
      msg += wxT("Free Ratio: ");
      sprintf(dummy, "%1.2f%%\n", ratio);
      msg += wxString::FromUTF8(dummy);
      msg += wxT("\n\nDo you confirm VACUUMing the current DB ?");
      int ret =
        wxMessageBox(msg, wxT("spatialite_gui"), wxYES_NO | wxICON_QUESTION,
                     this);
      if (ret != wxYES)
        return;
    }
  ::wxBeginBusyCursor();
  int ret = sqlite3_exec(SqliteHandle, "ANALYZE; VACUUM;", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
  } else
    {
      DbPagesCount(&totalPages2, &freePages2);
      msg = wxT("Current DB was succesfully optimized");
      if (totalPages2 < totalPages)
        {
          sprintf(dummy, "\n\n%d unused pages where reclaimed",
                  totalPages - totalPages2);
          msg += wxString::FromUTF8(dummy);
        }
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
    }
  ::wxEndBusyCursor();
}

char *MyFrame::ReadSqlLine(FILE * in, int *len, int *eof)
{
//
// reading an SQL script line
//
  int c;
  *eof = 0;
  int size = 4096;
  char *line = (char *) malloc(size);
  int off = 0;
  while ((c = getc(in)) != EOF)
    {
      // consuming input one chat at each time
      if (off == size)
        {
          // buffer overflow; reallocating a bigger one
          // presumably this is because there is some BLOB, so we'll grow by 1MB at each time
          size += 1024 * 1024;
          line = (char *) realloc(line, size);
        }
      *(line + off) = c;
      off++;
      if (c == '\n')
        {
          // end of line marker
          *(line + off) = '\0';
          *len = off;
          return line;
        }
      if (c == ';')
        {
          // end of SQL statement marker
          *(line + off) = '\0';
          *len = off;
          return line;
        }
    }
// EOF reached
  *len = off;
  *eof = 1;
  return line;
}

void MyFrame::OnQueryViewComposer(wxCommandEvent & WXUNUSED(event))
{
//
// invoking the Query/View composer tool
//
  QueryViewComposer();
}

void MyFrame::OnSqlScript(wxCommandEvent & WXUNUSED(event))
{
//
// executing an SQL Script
//
  int ret;
  wxString lastDir;
  wxString path;
  wxString charset;
  FILE *sql;
  char *line = NULL;
  char *statement = NULL;
  int stmt_len = 0;
  char *prev_stmt;
  int prev_len;
  int eof;
  int rowNo = 1;
  int stmt = 0;
  int len;
  wxString msg;
  char dummy[128];
  void *cvtCS = NULL;
  char *utf8stmt = NULL;
  int cvtErr;
  char x_path[1024];
  char x_table[1024];
  char x_column[1024];
  char x_charset[1024];
  char x_type[1024];
  int rows;
  unsigned int urows;
  wxString extPath;
  wxString extTable;
  wxString extCharset;
  wxString extColumn;
  wxString extType;
  int extWorksheetIndex;
  int extFirstTitle;
  int srid;
  bool coerce2D;
  bool compressed;
  int command;
  bool stmtDone;
  wxString workingDir;
  wxFileDialog fileDialog(this, wxT("SQL Script"),
                          wxT(""),
                          wxT("init_spatialite.sql"),
                          wxT("SQL script (*.sql)|*.sql|All files (*.*)|*.*"),
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      SqlScriptDialog dlg;
      dlg.Create(this, path, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        charset = dlg.GetCharset();
      else
        return;
      // opening the SQL script
      sql = fopen(path.ToUTF8(), "r");
      if (sql == NULL)
        {
          wxMessageBox(wxT("can't open: ") + fileDialog.GetPath(),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      } else
        {
          wxFileName file(fileDialog.GetPath());
          lastDir = file.GetPath();
          SetLastDirectory(lastDir);
          cvtCS = gaiaCreateUTF8Converter(charset.ToUTF8());
          if (!cvtCS)
            {
              msg = charset + wxT(": unsupported CharacterSet");
              wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_WARNING,
                           this);
              goto stop;
            }
          ::wxBeginBusyCursor();
          workingDir = wxGetCwd();
          wxSetWorkingDirectory(lastDir);
          while (1)
            {
              // reading the SQL script lines
              line = ReadSqlLine(sql, &len, &eof);
              if (len > 0)
                {
                  if (statement == NULL)
                    {
                      statement = line;
                      stmt_len = len;
                  } else
                    {
                      // appending line to SQL statement
                      prev_stmt = statement;
                      prev_len = stmt_len;
                      stmt_len = prev_len + len;
                      statement = (char *) malloc(stmt_len + 1);
                      memcpy(statement, prev_stmt, prev_len);
                      memcpy(statement + prev_len, line, len);
                      *(statement + stmt_len) = '\0';
                      free(prev_stmt);
                      free(line);
                      line = NULL;
                    }
              } else
                {
                  free(line);
                  line = NULL;
                }
              if (statement)
                {
                  stmtDone = false;
                  command = TestDotCommand(statement);
                  if (command == CMD_SQLLOG)
                    {
                      // simply ignoring
                      free(statement);
                      statement = NULL;
                      stmt_len = 0;
                      stmt++;
                      stmtDone = true;
                    }
                  if (command == CMD_LOADSHP || command == CMD_LOADDBF
                      || command == CMD_LOADXL || command == CMD_DUMPSHP
                      || command == CMD_DUMPDBF)
                    {
                      utf8stmt =
                        gaiaConvertToUTF8(cvtCS, statement + 9, stmt_len,
                                          &cvtErr);
                      if (cvtErr || !utf8stmt)
                        {
                          Rollback();
                          msg =
                            wxT
                            ("SQL Script abnormal termination\nillegal character sequence");
                          msg +=
                            wxT("\n\nROLLBACK was automatically performed");
                          wxMessageBox(msg, wxT("spatialite_gui"),
                                       wxOK | wxICON_WARNING, this);
                          goto stop;
                        }
                      if (command == CMD_LOADSHP)
                        {
                          if (IsDotCommandLoadShp
                              (utf8stmt, x_path, x_table, x_charset,
                               x_column, &srid, &coerce2D, &compressed))
                            {
                              load_shapefile(SqliteHandle, x_path, x_table,
                                             x_charset, srid, x_column,
                                             coerce2D, compressed, 0, 0, &rows,
                                             NULL);
                              free(statement);
                              statement = NULL;
                              stmt_len = 0;
                              stmt++;
                              stmtDone = true;
                            }
                        }
                      if (command == CMD_LOADDBF)
                        {
                          if (IsDotCommandLoadDbf
                              (utf8stmt, x_path, x_table, x_charset) == true)
                            {
                              load_dbf(SqliteHandle, x_path, x_table, x_charset,
                                       0, &rows, NULL);
                              free(statement);
                              statement = NULL;
                              stmt_len = 0;
                              stmt++;
                              stmtDone = true;
                            }
                        }
                      if (command == CMD_LOADXL)
                        {
                          if (IsDotCommandLoadXL
                              (utf8stmt, x_path, x_table, &extWorksheetIndex,
                               &extFirstTitle) == true)
                            {
#ifndef OMIT_FREEXL             /* only if FreeXL is enabled */
                              load_XL(SqliteHandle, x_path, x_table,
                                      extWorksheetIndex, extFirstTitle, &urows,
                                      NULL);
                              free(statement);
                              statement = NULL;
                              stmt_len = 0;
                              stmt++;
                              stmtDone = true;
#else /* FreeXL isn't enabled */
                              stmtDone = false;
#endif /* end FreeXL conditional support */
                            }
                        }
                      if (command == CMD_DUMPSHP)
                        {
                          if (IsDotCommandDumpShp
                              (utf8stmt, x_table, x_column, x_path,
                               x_charset, x_type) == true)
                            {
                              dump_shapefile(SqliteHandle, x_table, x_column,
                                             x_path, x_charset, x_type, 0,
                                             &rows, NULL);
                              free(statement);
                              statement = NULL;
                              stmt_len = 0;
                              stmt++;
                              stmtDone = true;
                            }
                        }
                      free(utf8stmt);
                      utf8stmt = NULL;
                    }
                  if (stmtDone)
                    ;
                  else if (sqlite3_complete(statement))
                    {
                      // executing the SQL statement
                      utf8stmt =
                        gaiaConvertToUTF8(cvtCS, statement, stmt_len, &cvtErr);
                      free(statement);
                      statement = NULL;
                      stmt_len = 0;
                      if (cvtErr || !utf8stmt)
                        {
                          Rollback();
                          msg =
                            wxT
                            ("SQL Script abnormal termination\nillegal character sequence");
                          msg +=
                            wxT("\n\nROLLBACK was automatically performed");
                          wxMessageBox(msg, wxT("spatialite_gui"),
                                       wxOK | wxICON_WARNING, this);
                          goto stop;
                        }
                      if (ExecuteSql(utf8stmt, rowNo) == false)
                        {
                          Rollback();
                          msg =
                            wxT
                            ("SQL Script abnormal termination\nan error occurred");
                          msg +=
                            wxT("\n\nROLLBACK was automatically performed");
                          wxMessageBox(msg, wxT("spatialite_gui"),
                                       wxOK | wxICON_WARNING, this);
                          goto stop;
                      } else
                        {
                          stmt++;
                          free(utf8stmt);
                          utf8stmt = NULL;
                        }
                    }
                }
              rowNo++;
              if (eof)
                break;
            }
          sprintf(dummy,
                  "SQL Script normal termination\n\n%d SQL statements where performed",
                  stmt);
          msg = wxString::FromUTF8(dummy);
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                       this);
        stop:
          if (cvtCS)
            gaiaFreeUTF8Converter(cvtCS);
          if (utf8stmt)
            free(utf8stmt);
          if (statement)
            free(statement);
          if (line)
            free(line);
          fclose(sql);
          wxSetWorkingDirectory(workingDir);
          ::wxEndBusyCursor();
        }
    }
}

void MyFrame::OnLoadShp(wxCommandEvent & WXUNUSED(event))
{
//
// loading a shapefile
//
  int ret;
  wxString table;
  wxString column = wxT("Geometry");
  wxString charset;
  int srid = 0;
  int coerce2D;
  int compressed;
  int spatial_index;
  wxString path;
  wxString lastDir;
  wxFileDialog fileDialog(this, wxT("Load Shapefile"),
                          wxT(""),
                          wxT("shapefile.shp"),
                          wxT("Shapefile (*.shp)|*.shp|All files (*.*)|*.*"),
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  if (GetMetaDataType() != METADATA_CURRENT)
    srid = -1;                  // old default SRID
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      table = file.GetName();
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      LoadShpDialog dlg;
      dlg.Create(this, path, table, srid, column, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          int rt;
          int rows;
          char x_path[1024];
          char x_table[1024];
          char x_column[1024];
          char x_gtype[1024];
          char *gtype;
          char x_pkey[1024];
          char *pkey;
          char x_charset[1024];
          char err_msg[1024];
          SetLastDirectory(lastDir);
          strcpy(x_path, path.ToUTF8());
          strcpy(x_table, dlg.GetTable().ToUTF8());
          srid = dlg.GetSrid();
          strcpy(x_column, dlg.GetColumn().ToUTF8());
          strcpy(x_charset, dlg.GetCharset().ToUTF8());
          if (dlg.ApplyCoertion2D() == true)
            coerce2D = 1;
          else
            coerce2D = 0;
          if (dlg.ApplyCompression() == true)
            compressed = 1;
          else
            compressed = 0;
          if (dlg.CreateSpatialIndex() == true)
            spatial_index = 1;
          else
            spatial_index = 0;
          if (dlg.IsUserDefinedGType() == false)
            gtype = NULL;
          else
            {
              strcpy(x_gtype, dlg.GetGeometryType().ToUTF8());
              gtype = x_gtype;
            }
          if (dlg.IsUserDefinedPKey() == false)
            pkey = NULL;
          else
            {
              strcpy(x_pkey, dlg.GetPKColumn().ToUTF8());
              pkey = x_pkey;
            }
          ::wxBeginBusyCursor();
          rt = load_shapefile_ex(SqliteHandle, x_path, x_table, x_charset, srid,
                                 x_column, gtype, pkey, coerce2D, compressed, 0,
                                 spatial_index, &rows, err_msg);
          ::wxEndBusyCursor();
          if (rt)
            {
              wxMessageBox(wxT("load shp OK:") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_INFORMATION, this);
              InitTableTree();
          } else
            wxMessageBox(wxT("load shp error:") +
                         wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                         wxOK | wxICON_ERROR, this);
        }
    }
}

void MyFrame::OnVirtualShp(wxCommandEvent & WXUNUSED(event))
{
//
// creating a VirtualShape
//
  int ret;
  wxString charset;
  int srid;
  char dummy[128];
  wxString sql;
  wxString path;
  wxString table;
  wxString lastDir;
  wxString geometryType;
  char *errMsg = NULL;
  char xname[1024];
  sqlite3 *sqlite = GetSqlite();
  wxFileDialog fileDialog(this, wxT("VirtualShape"),
                          wxT(""),
                          wxT("shapefile.shp"),
                          wxT("Shapefile (*.shp)|*.shp|All files (*.*)|*.*"),
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      VirtualShpDialog dlg;
      dlg.Create(this, path, table, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          table = dlg.GetTable();
          srid = dlg.GetSrid();
          charset = dlg.GetCharset();
      } else
        return;
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      sql = wxT("CREATE VIRTUAL TABLE ");
      strcpy(xname, table.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nUSING VirtualShape('");
      sql += file.GetPath();
      sql += file.GetPathSeparator();
      sql += file.GetName();
      sql += wxT("',\n'");
      sql += charset;
      sprintf(dummy, "', %d", srid);
      sql += wxString::FromUTF8(dummy);
      sql += wxT(")");
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      wxMessageBox(wxT("Virtual Table ") + table +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      InitTableTree();
    }
}

void MyFrame::OnLoadTxt(wxCommandEvent & WXUNUSED(event))
{
//
// loading a CSV/TXT
//
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  wxString lastDir;
  bool first_titles;
  bool decimal_comma;
  char separator;
  char text_separator;
  wxString filelist = wxT("TXT and CSV files (*.txt;*.csv)|*.txt;*.csv");
  filelist +=
    wxT("|Text file (*.txt)|*.txt|CSV file (*.csv)|*.csv|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("Load CSV/TXT"),
                          wxT(""),
                          wxT("textfile.txt"),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      LoadTxtDialog dlg;
      dlg.Create(this, path, table, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          SetLastDirectory(lastDir);
          table = dlg.GetTable();
          charset = dlg.GetCharset();
          first_titles = dlg.IsFirstLineTitles();
          decimal_comma = dlg.IsDecimalPointComma();
          separator = dlg.GetSeparator();
          text_separator = dlg.GetTextSeparator();
          char decimal_separator = '.';
          if (decimal_comma == true)
            decimal_separator = ',';
          LoadText(path, table, charset, first_titles, decimal_separator,
                   separator, text_separator);
        }
    }
}

void MyFrame::OnVirtualTxt(wxCommandEvent & WXUNUSED(event))
{
//
// creating a VirtualText
//
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  wxString lastDir;
  bool first_titles;
  bool decimal_comma;
  char separator;
  char text_separator;
  char dummy[16];
  char *errMsg = NULL;
  char xname[1024];
  sqlite3 *sqlite = GetSqlite();
  wxString filelist = wxT("TXT and CSV files (*.txt;*.csv)|*.txt;*.csv");
  filelist +=
    wxT("|Text file (*.txt)|*.txt|CSV file (*.csv)|*.csv|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("VirtualText"),
                          wxT(""),
                          wxT("textfile.txt"),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      VirtualTxtDialog dlg;
      dlg.Create(this, path, table, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          table = dlg.GetTable();
          charset = dlg.GetCharset();
          first_titles = dlg.IsFirstLineTitles();
          decimal_comma = dlg.IsDecimalPointComma();
          separator = dlg.GetSeparator();
          text_separator = dlg.GetTextSeparator();
      } else
        return;
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      sql = wxT("CREATE VIRTUAL TABLE ");
      strcpy(xname, table.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nUSING VirtualText('");
      sql += path;
      sql += wxT("',\n'");
      sql += charset;
      if (first_titles == true)
        sql += wxT("', 1");
      else
        sql += wxT("', 0");
      if (decimal_comma == true)
        sql += wxT(", COMMA");
      else
        sql += wxT(", POINT");
      if (text_separator == '\'')
        sql += wxT(", SINGLEQUOTE");
      else if (text_separator == '"')
        sql += wxT(", DOUBLEQUOTE");
      else
        sql += wxT(", NONE");
      if (separator == '\t')
        sql += wxT(", TAB");
      else
        {
          sprintf(dummy, ", '%c'", separator);
          sql += wxString::FromUTF8(dummy);
        }
      sql += wxT(")");
      ::wxBeginBusyCursor();
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      ::wxEndBusyCursor();
      wxMessageBox(wxT("Virtual Table ") + table +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      InitTableTree();
    }
}

void MyFrame::OnLoadDbf(wxCommandEvent & WXUNUSED(event))
{
//
// loading a DBF
//
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  wxString lastDir;
  wxString filelist = wxT("DBF files (*.dbf)|*.dbf|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("Load DBF"),
                          wxT(""),
                          wxT("dbfile.dbf"),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      LoadDbfDialog dlg;
      dlg.Create(this, path, table, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          int rt;
          int rows;
          char x_path[1024];
          char x_table[1024];
          char x_charset[1024];
          char x_pkey[1024];
          char *pkey;
          char err_msg[1024];
          SetLastDirectory(lastDir);
          strcpy(x_path, path.ToUTF8());
          strcpy(x_table, dlg.GetTable().ToUTF8());
          strcpy(x_charset, dlg.GetCharset().ToUTF8());
          if (dlg.IsUserDefinedPKey() == false)
            pkey = NULL;
          else
            {
              strcpy(x_pkey, dlg.GetPKColumn().ToUTF8());
              pkey = x_pkey;
            }
          rt =
            load_dbf_ex(SqliteHandle, x_path, x_table, pkey, x_charset, 0,
                        &rows, err_msg);
          if (rt)
            wxMessageBox(wxT("load dbf OK:") +
                         wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                         wxOK | wxICON_INFORMATION, this);
          else
            wxMessageBox(wxT("load dbf error:") +
                         wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                         wxOK | wxICON_ERROR, this);
        }
    }
}

void MyFrame::OnVirtualDbf(wxCommandEvent & WXUNUSED(event))
{
//
// creating a VirtualDbf
//
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  wxString lastDir;
  char *errMsg = NULL;
  char xname[1024];
  sqlite3 *sqlite = GetSqlite();
  wxString filelist = wxT("DBF files (*.dbf)|*.dbf|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("VirtualDbf"),
                          wxT(""),
                          wxT("dbfile.dbf"),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      VirtualDbfDialog dlg;
      dlg.Create(this, path, table, LocaleCharset);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          table = dlg.GetTable();
          charset = dlg.GetCharset();
      } else
        return;
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      sql = wxT("CREATE VIRTUAL TABLE ");
      strcpy(xname, table.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nUSING VirtualDbf('");
      sql += path;
      sql += wxT("', '");
      sql += charset;
      sql += wxT("')");
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      wxMessageBox(wxT("Virtual Table ") + table +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      InitTableTree();
    }
}

void MyFrame::OnLoadXL(wxCommandEvent & WXUNUSED(event))
{
//
// loading an XLS
//
#ifndef OMIT_FREEXL             /* FreeXL is enabled */
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  int sheetIndex;
  int firstLineTitles;
  char err_msg[1024];
  unsigned int rows;
  wxString lastDir;
  wxString filelist =
    wxT("Microsoft Excel spreadsheets (*.xls)|*.xls|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("Load XLS"), wxT(""),
                          wxT("spreadsheet.xls"), filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      LoadXLDialog dlg;
      dlg.Create(this, path, table);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          char x_path[1024];
          char x_table[1024];
          table = dlg.GetTable();
          sheetIndex = dlg.GetWorksheetIndex();
          if (dlg.IsFirstLineTitles() == true)
            firstLineTitles = 1;
          else
            firstLineTitles = 0;
          strcpy(x_path, path.ToUTF8());
          strcpy(x_table, table.ToUTF8());
          if (!load_XL
              (SqliteHandle, x_path, x_table, sheetIndex, firstLineTitles,
               &rows, err_msg))
            {
              wxMessageBox(wxT("load XL error:") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
            }
        }
    }
#else /* FreeXL disabled */
  wxString msg = wxT("This copy of spatialite_gui was built explictly\n");
  msg += wxT("disabling the FreeXL support.\n");
  msg += wxT("Sorry ...");
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_EXCLAMATION, this);
#endif /* end FreeXL conditional */
}

void MyFrame::OnVirtualXL(wxCommandEvent & WXUNUSED(event))
{
//
// creating a VirtualXL
//
#ifndef OMIT_FREEXL             /* FreeXL is enabled */
  int ret;
  wxString charset;
  wxString sql;
  wxString path;
  wxString table;
  int sheetIndex;
  bool firstLineTitles;
  wxString lastDir;
  char *errMsg = NULL;
  char xname[1024];
  sqlite3 *sqlite = GetSqlite();
  wxString filelist =
    wxT("Microsoft Excel spreadsheets (*.xls)|*.xls|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("VirtualXL"), wxT(""),
                          wxT("spreadsheet.xls"), filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      path = fileDialog.GetPath();
      wxFileName file(path);
      table = file.GetName();
      VirtualXLDialog dlg;
      dlg.Create(this, path, table);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          table = dlg.GetTable();
          sheetIndex = dlg.GetWorksheetIndex();
          firstLineTitles = dlg.IsFirstLineTitles();
      } else
        return;
      lastDir = file.GetPath();
      SetLastDirectory(lastDir);
      sql = wxT("CREATE VIRTUAL TABLE ");
      strcpy(xname, table.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nUSING VirtualXL('");
      sql += path;
      sprintf(xname, "', %d, %d)", sheetIndex,
              (firstLineTitles == true) ? 1 : 0);
      sql += wxString::FromUTF8(xname);
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      wxMessageBox(wxT("Virtual Table ") + table +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      InitTableTree();
    }
#else /* FreeXL disabled */
  wxString msg = wxT("This copy of spatialite_gui was built explictly\n");
  msg += wxT("disabling the FreeXL support.\n");
  msg += wxT("Sorry ...");
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_EXCLAMATION, this);
#endif /* end FreeXL conditional */
}

void MyFrame::OnNetwork(wxCommandEvent & WXUNUSED(event))
{
//
// building a Network
//
  NetworkDialog dlg;
  int ret;
  wxString table;
  wxString from;
  wxString to;
  wxString geom;
  wxString name;
  bool isGeomLength;
  wxString cost;
  bool isBidirectional;
  bool isOneWays;
  wxString oneWayToFrom;
  wxString oneWayFromTo;
  bool aStarSupported;
  dlg.Create(this);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      table = dlg.GetTableName();
      from = dlg.GetFromColumn();
      to = dlg.GetToColumn();
      geom = dlg.GetGeomColumn();
      name = dlg.GetNameColumn();
      isGeomLength = dlg.IsGeomLength();
      cost = dlg.GetCostColumn();
      isBidirectional = dlg.IsBidirectional();
      isOneWays = dlg.IsOneWays();
      oneWayToFrom = dlg.GetOneWayToFrom();
      oneWayFromTo = dlg.GetOneWayFromTo();
      aStarSupported = dlg.IsAStarSupported();
      BuildNetwork(table, from, to, geom, name, isGeomLength, cost,
                   isBidirectional, isOneWays, oneWayFromTo, oneWayToFrom,
                   aStarSupported);
    }
}

void MyFrame::OnImportXmlDocuments(wxCommandEvent & WXUNUSED(event))
{
//
// importing XML Documents
//
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */
  XmlDocumentsDialog dlg;
  int ret;
  wxString path;
  bool isFolder;
  int compressed;
  bool isInternalSchema;
  char *schemaURI;
  char xschema[8192];
  wxString lastDir;
  wxString dir_path;
  wxString xml_path;
  wxString suffix;
  wxString table;
  wxString pkName;
  wxString xmlColumn;
  wxString inPathColumn;
  wxString parseErrColumn;
  wxString validateErrColumn;
  wxString schemaUriColumn;
  wxString filelist = wxT("XML Documents (*.xml)|*.xml|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("XML File/Folder selection"),
                          wxT(""),
                          wxT(""),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      xml_path = fileDialog.GetPath();
      wxFileName file(xml_path);
      dir_path = file.GetPath();
  } else
    return;
  dlg.Create(this, dir_path, xml_path);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      ::wxBeginBusyCursor();
      SetLastDirectory(dir_path);
      isFolder = dlg.IsFolder();
      if (isFolder == true)
        path = dlg.GetDirPath();
      else
        path = dlg.GetXmlPath();
      suffix = dlg.GetSuffix();
      table = dlg.GetTargetTable();
      pkName = dlg.GetPkName();
      xmlColumn = dlg.GetXmlColumn();
      compressed = dlg.IsCompressed();
      wxString schema = dlg.GetSchemaURI();
      isInternalSchema = dlg.IsInternalSchemaURI();
      if (schema.Len() == 0)
        schemaURI = NULL;
      else
        {
          strcpy((char *) xschema, schema.ToUTF8());
          schemaURI = xschema;
        }
      inPathColumn = dlg.GetInPathColumn();
      schemaUriColumn = dlg.GetSchemaUriColumn();
      parseErrColumn = dlg.GetParseErrorColumn();
      validateErrColumn = dlg.GetValidateErrorColumn();
      wxString currDir =::wxGetCwd();
      ::wxSetWorkingDirectory(dir_path);
      ImportXmlDocuments(path, isFolder, suffix, table, pkName, xmlColumn,
                         inPathColumn, schemaUriColumn, parseErrColumn,
                         validateErrColumn, compressed, schemaURI,
                         isInternalSchema);
      ::wxSetWorkingDirectory(currDir);
      ::wxEndBusyCursor();
      InitTableTree();
    }
#else

  wxMessageBox(wxT
               ("Sorry, spatialite_gui was built disabling LIBXML2\n\nUnsupported operation"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);

#endif /* end LIBXML2 conditionals */
}

void MyFrame::OnImportWFS(wxCommandEvent & WXUNUSED(event))
{
//
// importing data from WFS datasource
//
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */
  WfsDialog dlg;
  dlg.Create(this);
  dlg.ShowModal();

#else

  wxMessageBox(wxT
               ("Sorry, spatialite_gui was built disabling LIBXML2\n\nUnsupported operation"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);

#endif /* end LIBXML2 conditionals */
}

void MyFrame::OnImportDXF(wxCommandEvent & WXUNUSED(event))
{
//
// importing DXF file(s)
//
  DxfDialog dlg;
  int ret;
  wxString path;
  bool isFolder;
  wxString lastDir;
  wxString dir_path;
  wxString dxf_path;
  wxString prefix;
  wxString layer;
  int srid;
  bool force2d;
  bool force3d;
  bool mixed;
  bool linked;
  bool unlinked;
  bool append;
  wxString filelist = wxT("DXF drawing file (*.dxf)|*.dxf|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("DXF File/Folder selection"),
                          wxT(""),
                          wxT(""),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      dxf_path = fileDialog.GetPath();
      wxFileName file(dxf_path);
      dir_path = file.GetPath();
  } else
    return;
  dlg.Create(this, dir_path, dxf_path);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      ::wxBeginBusyCursor();
      SetLastDirectory(dir_path);
      isFolder = dlg.IsFolder();
      if (isFolder == true)
        path = dlg.GetDirPath();
      else
        path = dlg.GetDxfPath();
      prefix = dlg.GetPrefix();
      layer = dlg.GetSingleLayer();
      srid = dlg.GetSrid();
      force2d = dlg.IsForce2D();
      force3d = dlg.IsForce3D();
      mixed = dlg.IsImportMixed();
      linked = dlg.IsLinkedRings();
      unlinked = dlg.IsUnlinkedRings();
      append = dlg.IsAppendMode();
      wxString currDir =::wxGetCwd();
      ::wxSetWorkingDirectory(dir_path);
      ImportDXFfiles(path, isFolder, prefix, layer, srid, force2d, force3d,
                     mixed, linked, unlinked, append);
      ::wxSetWorkingDirectory(currDir);
      ::wxEndBusyCursor();
      InitTableTree();
    }
}

void MyFrame::OnImportExifPhotos(wxCommandEvent & WXUNUSED(event))
{
//
// importing EXIF Photos
//
  ExifDialog dlg;
  int ret;
  wxString path;
  bool isFolder;
  bool isMetadata;
  bool isGpsOnly;
  wxString lastDir;
  wxString dir_path;
  wxString img_path;
  wxString filelist = wxT("JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg|");
  filelist += wxT("All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("EXIF File/Folder selection"),
                          wxT(""),
                          wxT(""),
                          filelist,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      img_path = fileDialog.GetPath();
      wxFileName file(img_path);
      dir_path = file.GetPath();
  } else
    return;
  dlg.Create(this, dir_path, img_path);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      SetLastDirectory(dir_path);
      isFolder = dlg.IsFolder();
      if (isFolder == true)
        path = dlg.GetDirPath();
      else
        path = dlg.GetImgPath();
      isMetadata = dlg.IsMetadata();
      isGpsOnly = dlg.IsGpsOnly();
      ImportExifPhotos(path, isFolder, isMetadata, isGpsOnly);
    }
}

void MyFrame::OnSrids(wxCommandEvent & WXUNUSED(event))
{
//
// searching a SRID by name
//
  SearchSridDialog dlg;
  int ret;
  wxString string;
  wxString sql;
  dlg.Create(this);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      sql = wxT("SELECT * FROM spatial_ref_sys\n");
      if (dlg.IsSearchBySrid() == true)
        {
          // searching by SRID
          char txt[128];
          sprintf(txt, "WHERE srid = %d", dlg.GetSrid());
          sql += wxString::FromUTF8(txt);
      } else
        {
          // searching by NAME
          string = dlg.GetString();
          sql += wxT("WHERE ref_sys_name LIKE '%");
          sql += string;
          sql += wxT("%'\nORDER BY srid");
        }
      QueryView->SetSql(sql, true);
    }
}

void MyFrame::OnCharset(wxCommandEvent & WXUNUSED(event))
{
//
// setting the default CHARSET
//
  DefaultCharsetDialog dlg;
  int ret;
  dlg.Create(this, DefaultCharset, AskCharset);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      DefaultCharset = dlg.GetCharset();
      AskCharset = dlg.IsSetAskCharset();
    }
}

void MyFrame::OnTimerAutoSave(wxTimerEvent & WXUNUSED(event))
{
//
// AutoSave - Timer event handler
// 
  int tc = sqlite3_total_changes(SqliteHandle);
  if (tc != LastTotalChanges)
    MemoryDbSave();
  if (AutoSaveInterval <= 0)
    {
      delete TimerAutoSave;
      TimerAutoSave = NULL;
  } else
    TimerAutoSave->Start(AutoSaveInterval * 1000, wxTIMER_ONE_SHOT);
}

bool MyFrame::ExecuteSql(const char *sql, int rowNo)
{
//
// executes an SQL statement from the SQL script
//
  int ret;
  char *errMsg = NULL;
  wxString msg;
  char dummy[128];
  ret = sqlite3_exec(SqliteHandle, sql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      sprintf(dummy, "row %d\n\nSQLite SQL error: ", rowNo);
      msg = wxString::FromUTF8(dummy);
      wxMessageBox(msg + wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  return true;
}

void MyFrame::Rollback()
{
//
// performing a ROLLBACK
//
  sqlite3_exec(SqliteHandle, "ROLLBACK", NULL, NULL, NULL);
}

bool MyFrame::OpenDB()
{
//
// establishing a physical connetion to some DB SQLite
//
  int ret;
  char *errMsg = NULL;
  ret =
    sqlite3_open_v2(SqlitePath.ToUTF8(), &SqliteHandle, SQLITE_OPEN_READWRITE,
                    NULL);
  if (ret)
    {
      // an error occurred
      wxString errCause;
      errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      sqlite3_close(SqliteHandle);
      wxMessageBox(wxT("Failure while connecting to DB\n\n") + errCause +
                   wxT("\n") + SqlitePath, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      SqliteHandle = NULL;
      InternalCache = NULL;
      ClearTableTree();
      MemoryDatabase = false;
      return false;
    }
// setting up the internal cache
  InternalCache = spatialite_alloc_connection();
  spatialite_init_ex(SqliteHandle, InternalCache, 0);
// enabling LOAD_EXTENSION
  ret = sqlite3_enable_load_extension(SqliteHandle, 1);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("Unable to enable LOAD_EXTENSION"),
                   wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
      sqlite3_free(errMsg);
    }
// activating Foreign Key constraints
  ret = sqlite3_exec(SqliteHandle, "PRAGMA foreign_keys = 1", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("Unable to activate FOREIGN_KEY constraints"),
                   wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
      sqlite3_free(errMsg);
    }
  AutoFDOStart();
  InitTableTree();
  LoadHistory();
  return true;
}

void MyFrame::LastDitchMemoryDbSave()
{
//
// performing the last desperate attempt to save a MEMORY-DB
//
  int tc;
  int ret;
  wxString lastDir;
  if (MemoryDatabase == false)
    return;
  if (!SqliteHandle)
    return;
  tc = sqlite3_total_changes(SqliteHandle);
  if (tc == LastTotalChanges)
    return;
  while (1)
    {
      // OK, this MEMORY-DB needs to be saved
      if (MemoryDbSave() == true)
        break;
      // we must ask the user
      wxString msg =
        wxT("WARNING: the MEMORY-DB contains uncommitted changes\n\n");
      msg += wxT("The MEMORY_DB is intrinsecally volatile, so these changes\n");
      msg +=
        wxT("will be irremediably lost if you don't export them to some\n");
      msg += wxT("persistent storage [i.e. on the file-system]\n\n");
      msg +=
        wxT
        ("Do you want to export [SAVE] the MEMORY-DB to some external database ?");
      ret =
        wxMessageBox(msg, wxT("spatialite_gui"), wxYES_NO | wxICON_QUESTION,
                     this);
      if (ret != wxYES)
        break;
      // asking a PATHNAME to the user
      wxFileDialog fileDialog(this, wxT("Saving the MEMORY-DB"),
                              wxT(""), wxT("db.sqlite"),
                              wxT
                              ("SQLite DB (*.sqlite)|*.sqlite|All files (*.*)|*.*"),
                              wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                              wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
      lastDir = GetLastDirectory();
      if (lastDir.Len() >= 1)
        fileDialog.SetDirectory(lastDir);
      ret = fileDialog.ShowModal();
      if (ret == wxID_OK)
        {
          // exporting the external DB
          ExternalSqlitePath = fileDialog.GetPath();
          if (MemoryDbSave() == true)
            {
              wxMessageBox(wxT("Ok, MEMORY-DB was succesfully saved"),
                           wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                           this);
              break;
            }
        }
    }
}

void MyFrame::CloseDB()
{
//
// disconnecting current SQLite DB
//
  if (!SqliteHandle)
    return;
  AutoFDOStop();
  if (AutoFDOmsg.Len() > 0)
    wxMessageBox(AutoFDOmsg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                 this);
  LastDitchMemoryDbSave();
  sqlite3_close(SqliteHandle);
  SqliteHandle = NULL;
  spatialite_cleanup_ex(InternalCache);
  InternalCache = NULL;
  SqlitePath = wxT("");
  MemoryDatabase = false;
  ClearTableTree();
}

bool MyFrame::CreateDB()
{
// creating a new, empty SQLite DB
  int ret;
  char path[1024];
  char *errMsg = NULL;
  if (MemoryDatabase == true)
    strcpy(path, ":memory:");
  else
    {
      strcpy(path, SqlitePath.ToUTF8());
      unlink(path);
    }
  ret =
    sqlite3_open_v2(path, &SqliteHandle,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (ret)
    {
      // an error occurred
      wxString errCause;
      errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      sqlite3_close(SqliteHandle);
      wxMessageBox(wxT("An error occurred\n\n") + errCause + wxT("\n") +
                   SqlitePath, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                   this);
      SqliteHandle = NULL;
      InternalCache = NULL;
      ClearTableTree();
      MemoryDatabase = false;
      return false;
    }
// setting up the internal cache
  InternalCache = spatialite_alloc_connection();
  spatialite_init_ex(SqliteHandle, InternalCache, 0);
// activating Foreign Key constraints
  ret = sqlite3_exec(SqliteHandle, "PRAGMA foreign_keys = 1", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("Unable to activate FOREIGN_KEY constraints"),
                   wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
      sqlite3_free(errMsg);
      SqliteHandle = NULL;
      ClearTableTree();
      MemoryDatabase = false;
      return false;
    }
  InitializeSpatialMetadata();
  AutoFDOStart();
  InitTableTree();
  return true;
}

void MyFrame::InitializeSpatialMetadata()
{
// attempting to perform self-initialization for a newly created DB
  int ret;
  char sql[1024];
  char *errMsg = NULL;
  int count;
  int i;
  char **results;
  int rows;
  int columns;

  if (SqliteHandle == NULL)
    return;
// checking if this DB is really empty 
  strcpy(sql, "SELECT Count(*) from sqlite_master");
  ret = sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    return;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        count = atoi(results[(i * columns) + 0]);
    }
  sqlite3_free_table(results);

  if (count > 0)
    return;

// all right, it's empty: proceding to initialize
  strcpy(sql, "SELECT InitSpatialMetadata(1)");
  ret = sqlite3_exec(SqliteHandle, sql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("Unable to initialite SpatialMetadata: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
}

bool MyFrame::HasHistory(void)
{
// checks if the "sql_statement" table exists
  int ret;
  int i;
  char **results;
  int rows;
  int columns;
  wxString sql;
  bool ok_sql = false;
  bool ok_cause = false;
  bool ok_time = false;
  char *errMsg = NULL;

  sql = wxT("PRAGMA table_info(sql_statements_log)");
  ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          const char *name = results[(i * columns) + 1];
          wxString x = wxString::FromUTF8(name);
          if (x.CmpNoCase(wxT("sql_statement")) == 0)
            ok_sql = true;
          if (x.CmpNoCase(wxT("error_cause")) == 0)
            ok_cause = true;
          if (x.CmpNoCase(wxT("sql_statement")) == 0)
            ok_time = true;
        }
    }
  sqlite3_free_table(results);
  if (ok_sql && ok_cause && ok_time)
    return true;
  return false;
}

void MyFrame::LoadHistory(void)
{
// attempting to load the most recent SQL history
  int ret;
  int i;
  char **results;
  int rows;
  int columns;
  wxString sql;
  char *errMsg = NULL;
  if (!HasHistory())
    return;

  sql = wxT("SELECT sql_statement FROM sql_statements_log ");
  sql += wxT("WHERE error_cause = 'success' ORDER BY time_end DESC ");
  sql += wxT("LIMIT 25");
  ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i < rows; i++)
        {
          const char *name = results[(i * columns) + 1];
          wxString x = wxString::FromUTF8(name);
          QueryView->GetHistory()->Prepend(x);
        }
    }
  sqlite3_free_table(results);
  QueryView->SetHistoryStates();
}

void MyFrame::DoubleQuotedSql(char *buf)
{
// well-formatting a string to be used as an SQL name
  char tmp[1024];
  char *in = tmp;
  char *out = buf;
  strcpy(tmp, buf);
  *out++ = '"';
  while (*in != '\0')
    {
      if (*in == '"')
        *out++ = '"';
      *out++ = *in++;
    }
  *out++ = '"';
  *out = '\0';
}

bool MyFrame::IsSpatialIndex(wxString & tableName)
{
// testing if this table belongs to some R*Tree Spatial Index
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *geom;
  char dummy[2048];
  wxString tblName;
  wxString sql;
// fetching any defined Spatial Index
  sql =
    wxT
    ("SELECT f_table_name, f_geometry_column FROM geometry_columns WHERE spatial_index_enabled = 1");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    return false;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          geom = results[(i * columns) + 1];
          sprintf(dummy, "idx_%s_%s", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_node", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_parent", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_rowid", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
        }
    }
  sqlite3_free_table(results);
  return false;
}

bool MyFrame::IsSpatialIndex(wxString & dbAlias, wxString & tableName)
{
// testing if this table belongs to some R*Tree Spatial Index
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *geom;
  char dummy[2048];
  wxString tblName;
  wxString sql;
// fetching any defined Spatial Index
  sql = wxT("SELECT f_table_name, f_geometry_column FROM ");
  sql += dbAlias + wxT(".geometry_columns WHERE spatial_index_enabled = 1");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    return false;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          geom = results[(i * columns) + 1];
          sprintf(dummy, "idx_%s_%s", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_node", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_parent", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
          sprintf(dummy, "idx_%s_%s_rowid", name, geom);
          tblName = wxString::FromUTF8(dummy);
          if (tableName.CmpNoCase(tblName) == 0)
            return true;
        }
    }
  sqlite3_free_table(results);
  return false;
}

void MyFrame::ElementaryGeoms(wxString & inTable, wxString & geometry,
                              wxString & outTable, wxString & pKey,
                              wxString & multiID, wxString & type, int *srid,
                              wxString & coordDims, bool * spIdx)
{
// ancillary SQL for ElementaryGeoms
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *gtp;
  char *dims;
  char dummy[2048];
  wxString sql;
  bool ok;
  int metadata_type;

  outTable = inTable + wxT("_elem");
  pKey = wxT("pk_elem");
  multiID = wxT("multi_id");
  type = wxT("*** Error ***");
  *srid = 0;
  coordDims = wxT("*** Error ***");
  *spIdx = false;

// fetching metadata
  metadata_type = GetMetaDataType();
  if (metadata_type == METADATA_LEGACY)
    sql =
      wxT
      ("SELECT type, coord_dimension, srid, spatial_index_enabled FROM geometry_columns ");
  else if (metadata_type == METADATA_CURRENT)
    sql =
      wxT
      ("SELECT geometry_type, srid, spatial_index_enabled FROM geometry_columns ");
  else
    return;
  sql += wxT("WHERE Lower(f_table_name) = Lower('");
  strcpy(dummy, inTable.ToUTF8());
  CleanSqlString(dummy);
  sql += wxString::FromUTF8(dummy);
  sql += wxT("') AND Lower(f_geometry_column) = Lower('");
  strcpy(dummy, geometry.ToUTF8());
  CleanSqlString(dummy);
  sql += wxString::FromUTF8(dummy);
  sql += wxT("')");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          if (metadata_type == METADATA_LEGACY)
            {
              /* legacy Spatial MetaData layout */
              gtp = results[(i * columns) + 0];
              dims = results[(i * columns) + 1];
              *srid = atoi(results[(i * columns) + 2]);
              if (atoi(results[(i * columns) + 3]) == 0)
                *spIdx = false;
              else
                *spIdx = true;
              if (strcasecmp(gtp, "POINT") == 0
                  || strcasecmp(gtp, "MULTIPOINT") == 0)
                type = wxString::FromUTF8("POINT");
              else if (strcasecmp(gtp, "LINESTRING") == 0
                       || strcasecmp(gtp, "MULTILINESTRING") == 0)
                type = wxString::FromUTF8("LINESTRING");
              else if (strcasecmp(gtp, "POLYGON") == 0
                       || strcasecmp(gtp, "MULTIPOLYGON") == 0)
                type = wxString::FromUTF8("POLYGON");
              else
                type = wxString::FromUTF8("GEOMETRY");
              coordDims = wxString::FromUTF8(dims);
          } else
            {
              /* current Spatial MetaData layout */
              switch (atoi(results[(i * columns) + 0]))
                {
                  case 0:
                  case 1:
                  case 2:
                  case 3:
                  case 7:
                    type = wxT("GEOMETRY");
                    coordDims = wxT("XY");
                    break;
                  case 1000:
                  case 1001:
                  case 1002:
                  case 1003:
                  case 1007:
                    type = wxT("GEOMETRY");
                    coordDims = wxT("XYZ");
                    break;
                  case 2000:
                  case 2001:
                  case 2002:
                  case 2003:
                  case 2007:
                    type = wxT("GEOMETRY");
                    coordDims = wxT("XYM");
                    break;
                  case 3000:
                  case 3001:
                  case 3002:
                  case 3003:
                  case 3007:
                    type = wxT("GEOMETRY");
                    coordDims = wxT("XYZM");
                    break;
                  case 4:
                    type = wxT("POINT");
                    coordDims = wxT("XY");
                    break;
                  case 1004:
                    type = wxT("POINT");
                    coordDims = wxT("XYZ");
                    break;
                  case 2004:
                    type = wxT("POINT");
                    coordDims = wxT("XYM");
                    break;
                  case 3004:
                    type = wxT("POINT");
                    coordDims = wxT("XYZM");
                    break;
                  case 5:
                    type = wxT("LINESTRING");
                    coordDims = wxT("XY");
                    break;
                  case 1005:
                    type = wxT("LINESTRING");
                    coordDims = wxT("XYZ");
                    break;
                  case 2005:
                    type = wxT("LINESTRING");
                    coordDims = wxT("XYM");
                    break;
                  case 3005:
                    type = wxT("LINESTRING");
                    coordDims = wxT("XYZM");
                    break;
                  case 6:
                    type = wxT("POLYGON");
                    coordDims = wxT("XY");
                    break;
                  case 1006:
                    type = wxT("POLYGON");
                    coordDims = wxT("XYZ");
                    break;
                  case 2006:
                    type = wxT("POLYGON");
                    coordDims = wxT("XYM");
                    break;
                  case 3006:
                    type = wxT("POLYGON");
                    coordDims = wxT("XYZM");
                    break;
                };
              *srid = atoi(results[(i * columns) + 1]);
              if (atoi(results[(i * columns) + 2]) == 0)
                *spIdx = false;
              else
                *spIdx = true;
            }
        }
    }
  sqlite3_free_table(results);

  ok = true;
  while (ok)
    {
      // creating an unique PrimaryKey name
      ok = false;
      sql = wxT("PRAGMA table_info(");
      strcpy(dummy, inTable.ToUTF8());
      DoubleQuotedSql(dummy);
      sql += wxString::FromUTF8(dummy);
      sql += wxT(")");
      ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              wxString x = wxString::FromUTF8(name);
              if (x.CmpNoCase(pKey) == 0)
                {
                  pKey += wxT("_1");
                  ok = true;
                  break;
                }
            }
        }
      sqlite3_free_table(results);
    }

  ok = true;
  while (ok)
    {
      // creating an unique MultiID name
      ok = false;
      sql = wxT("PRAGMA table_info(");
      strcpy(dummy, inTable.ToUTF8());
      DoubleQuotedSql(dummy);
      sql += wxString::FromUTF8(dummy);
      sql += wxT(")");
      ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              wxString x = wxString::FromUTF8(name);
              if (x.CmpNoCase(multiID) == 0)
                {
                  multiID += wxT("_1");
                  ok = true;
                  break;
                }
            }
        }
      sqlite3_free_table(results);
    }

  ok = true;
  while (ok)
    {
      // creating an unique Table name
      ok = false;
      sql =
        wxT
        ("SELECT Count(*) FROM sqlite_master WHERE type = 'table' AND Lower(tbl_name) = Lower('");
      strcpy(dummy, outTable.ToUTF8());
      CleanSqlString(dummy);
      sql += wxString::FromUTF8(dummy);
      sql += wxT("')");
      ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (atoi(results[(i * columns) + 0]) != 0)
                {
                  outTable += wxT("_1");
                  ok = true;
                  break;
                }
            }
        }
      sqlite3_free_table(results);
    }
}

bool MyFrame::DoElementaryGeometries(wxString & inTable, wxString & geometry,
                                     wxString & outTable, wxString & pKey,
                                     wxString & multiID, wxString & type,
                                     int srid, wxString & coordDims, bool spIdx)
{
// testing if this table belongs to some R*Tree Spatial Index
  int ret;
  int i;
  char **results;
  int rows;
  int columns;
  wxString sql;
  wxString sql2;
  wxString sql3;
  wxString sql4;
  wxString sql_geom;
  wxString sql_spidx;
  wxString sqlx;
  char dummy[8192];
  char *errMsg = NULL;
  bool comma = false;
  sqlite3_stmt *stmt_in = NULL;
  sqlite3_stmt *stmt_out = NULL;
  int n_columns;
  sqlite3_int64 id = 0;
  int geom_idx = -1;

// starts a transaction 
  ret = sqlite3_exec(SqliteHandle, "BEGIN", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("BEGIN error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }

  sql = wxT("SELECT ");
  sql2 = wxT("INSERT INTO ");
  sql3 = wxT(") VALUES (NULL, ?");
  sql4 = wxT("CREATE TABLE ");
  strcpy(dummy, outTable.ToUTF8());
  DoubleQuotedSql(dummy);
  sql2 += wxString::FromUTF8(dummy);
  sql2 += wxT(" (");
  sql4 += wxString::FromUTF8(dummy);
  strcpy(dummy, pKey.ToUTF8());
  DoubleQuotedSql(dummy);
  sql2 += wxString::FromUTF8(dummy);
  sql2 += wxT(", ");
  strcpy(dummy, multiID.ToUTF8());
  DoubleQuotedSql(dummy);
  sql2 += wxString::FromUTF8(dummy);
  sql4 += wxT(" (\n\t");
  strcpy(dummy, pKey.ToUTF8());
  DoubleQuotedSql(dummy);
  sql4 += wxString::FromUTF8(dummy);
  sql4 += wxT(" INTEGER PRIMARY KEY AUTOINCREMENT");
  sql4 += wxT(",\n\t");
  strcpy(dummy, multiID.ToUTF8());
  DoubleQuotedSql(dummy);
  sql4 += wxString::FromUTF8(dummy);
  sql4 += wxT(" INTEGER NOT NULL");

  sqlx = wxT("PRAGMA table_info(");
  strcpy(dummy, inTable.ToUTF8());
  DoubleQuotedSql(dummy);
  sqlx += wxString::FromUTF8(dummy);
  sqlx += wxT(")");
  ret = sqlite3_get_table(SqliteHandle, sqlx.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          strcpy(dummy, results[(i * columns) + 1]);
          DoubleQuotedSql(dummy);
          if (comma)
            sql += wxT(", ");
          else
            comma = true;
          sql += wxString::FromUTF8(dummy);
          sql2 += wxT(", ");
          sql2 += wxString::FromUTF8(dummy);
          sql3 += wxT(", ?");

          wxString x = wxString::FromUTF8(results[(i * columns) + 1]);
          if (x.CmpNoCase(geometry) == 0)
            geom_idx = i - 1;
          else
            {
              sql4 += wxT(",\n\t");
              sql4 += wxString::FromUTF8(dummy);
              sql4 += wxT(" ") + wxString::FromUTF8(results[(i * columns) + 2]);
              if (atoi(results[(i * columns) + 3]) != 0)
                sql4 += wxT(" NOT NULL");
            }
        }
    }
  sqlite3_free_table(results);
  if (geom_idx < 0)
    goto abort;

  sql += wxT(" FROM ");
  strcpy(dummy, inTable.ToUTF8());
  DoubleQuotedSql(dummy);
  sql += wxString::FromUTF8(dummy);
  sql2 += sql3;
  sql2 += wxT(")");
  sql4 += wxT(")");

  sql_geom = wxT("SELECT AddGeometryColumn('");
  strcpy(dummy, outTable.ToUTF8());
  CleanSqlString(dummy);
  sql_geom += wxString::FromUTF8(dummy);
  sql_geom += wxT("', '");
  strcpy(dummy, geometry.ToUTF8());
  CleanSqlString(dummy);
  sql_geom += wxString::FromUTF8(dummy);
  sql_geom += wxT("', ");
  sprintf(dummy, "%d, '", srid);
  sql_geom += wxString::FromUTF8(dummy);
  strcpy(dummy, type.ToUTF8());
  CleanSqlString(dummy);
  sql_geom += wxString::FromUTF8(dummy);
  sql_geom += wxT("', '");
  strcpy(dummy, coordDims.ToUTF8());
  CleanSqlString(dummy);
  sql_geom += wxString::FromUTF8(dummy);
  sql_geom += wxT("')");
  if (spIdx == true)
    {
      sql_spidx = wxT("SELECT CreateSpatialIndex('");
      strcpy(dummy, outTable.ToUTF8());
      CleanSqlString(dummy);
      sql_spidx += wxString::FromUTF8(dummy);
      sql_spidx += wxT("', '");
      strcpy(dummy, geometry.ToUTF8());
      CleanSqlString(dummy);
      sql_spidx += wxString::FromUTF8(dummy);
      sql_spidx += wxT("')");
    }
// creating the output table
  strcpy(dummy, sql4.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, dummy, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE TABLE error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// creating the output Geometry
  strcpy(dummy, sql_geom.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, dummy, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("AddGeometryColumn error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (spIdx == true)
    {
      // creating the R*Tree Spatial Index
      strcpy(dummy, sql_spidx.ToUTF8());
      ret = sqlite3_exec(SqliteHandle, dummy, NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("CreateSpatialIndex error: ") +
                       wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto abort;
        }
    }
// preparing the INPUT statement
  strcpy(dummy, sql.ToUTF8());
  ret = sqlite3_prepare_v2(SqliteHandle, dummy, strlen(dummy), &stmt_in, NULL);
  if (ret != SQLITE_OK)
    {
      wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      wxMessageBox(wxT("[IN]SQL error: ") + err, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      goto abort;
    }
// preparing the OUTPUT statement
  strcpy(dummy, sql2.ToUTF8());
  ret = sqlite3_prepare_v2(SqliteHandle, dummy, strlen(dummy), &stmt_out, NULL);
  if (ret != SQLITE_OK)
    {
      wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      wxMessageBox(wxT("[OUT]SQL error: ") + err, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      goto abort;
    }
// data transfer
  n_columns = sqlite3_column_count(stmt_in);
  while (1)
    {
      ret = sqlite3_step(stmt_in);
      if (ret == SQLITE_DONE)
        break;
      if (ret == SQLITE_ROW)
        {
          gaiaGeomCollPtr g =
            gaiaFromSpatiaLiteBlobWkb((const unsigned char *)
                                      sqlite3_column_blob(stmt_in, geom_idx),
                                      sqlite3_column_bytes(stmt_in, geom_idx));
          if (!g)
            {
              // NULL input geometry    
              sqlite3_reset(stmt_out);
              sqlite3_clear_bindings(stmt_out);
              sqlite3_bind_int64(stmt_out, 1, id);
              sqlite3_bind_null(stmt_out, geom_idx + 2);

              for (i = 0; i < n_columns; i++)
                {
                  int type = sqlite3_column_type(stmt_in, i);
                  if (i == geom_idx)
                    continue;
                  switch (type)
                    {
                      case SQLITE_INTEGER:
                        sqlite3_bind_int64(stmt_out, i + 2,
                                           sqlite3_column_int(stmt_in, i));
                        break;
                      case SQLITE_FLOAT:
                        sqlite3_bind_double(stmt_out, i + 2,
                                            sqlite3_column_double(stmt_in, i));
                        break;
                      case SQLITE_TEXT:
                        sqlite3_bind_text(stmt_out, i + 2,
                                          (const char *)
                                          sqlite3_column_text(stmt_in, i),
                                          sqlite3_column_bytes(stmt_in, i),
                                          SQLITE_STATIC);
                        break;
                      case SQLITE_BLOB:
                        sqlite3_bind_blob(stmt_out, i + 2,
                                          sqlite3_column_blob(stmt_in, i),
                                          sqlite3_column_bytes(stmt_in, i),
                                          SQLITE_STATIC);
                        break;
                      case SQLITE_NULL:
                      default:
                        sqlite3_bind_null(stmt_out, i + 2);
                        break;
                    };
                }

              ret = sqlite3_step(stmt_out);
              if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                ;
              else
                {
                  wxString err =
                    wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
                  wxMessageBox(wxT("[OUT]step error: ") + err,
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  goto abort;
                }
          } else
            {
              // separating Elementary Geoms
              gaiaPointPtr pt;
              gaiaLinestringPtr ln;
              gaiaPolygonPtr pg;
              gaiaGeomCollPtr outGeom;
              pt = g->FirstPoint;
              while (pt)
                {
                  // separating Points
                  outGeom = GeomFromPoint(pt, g->Srid);
                  sqlite3_reset(stmt_out);
                  sqlite3_clear_bindings(stmt_out);
                  sqlite3_bind_int64(stmt_out, 1, id);
                  if (!outGeom)
                    sqlite3_bind_null(stmt_out, geom_idx + 2);
                  else
                    {
                      unsigned char *blob;
                      int size;
                      gaiaToSpatiaLiteBlobWkb(outGeom, &blob, &size);
                      sqlite3_bind_blob(stmt_out, geom_idx + 2, blob, size,
                                        free);
                      gaiaFreeGeomColl(outGeom);
                    }

                  for (i = 0; i < n_columns; i++)
                    {
                      int type = sqlite3_column_type(stmt_in, i);
                      if (i == geom_idx)
                        continue;
                      switch (type)
                        {
                          case SQLITE_INTEGER:
                            sqlite3_bind_int64(stmt_out, i + 2,
                                               sqlite3_column_int(stmt_in, i));
                            break;
                          case SQLITE_FLOAT:
                            sqlite3_bind_double(stmt_out, i + 2,
                                                sqlite3_column_double(stmt_in,
                                                                      i));
                            break;
                          case SQLITE_TEXT:
                            sqlite3_bind_text(stmt_out, i + 2,
                                              (const char *)
                                              sqlite3_column_text(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_BLOB:
                            sqlite3_bind_blob(stmt_out, i + 2,
                                              sqlite3_column_blob(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_NULL:
                          default:
                            sqlite3_bind_null(stmt_out, i + 2);
                            break;
                        };
                    }

                  ret = sqlite3_step(stmt_out);
                  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                    ;
                  else
                    {
                      wxString err =
                        wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
                      wxMessageBox(wxT("[OUT]step error: ") + err,
                                   wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                                   this);
                      goto abort;
                    }
                  pt = pt->Next;
                }
              ln = g->FirstLinestring;
              while (ln)
                {
                  // separating Linestrings
                  outGeom = GeomFromLinestring(ln, g->Srid);
                  sqlite3_reset(stmt_out);
                  sqlite3_clear_bindings(stmt_out);
                  sqlite3_bind_int64(stmt_out, 1, id);
                  if (!outGeom)
                    sqlite3_bind_null(stmt_out, geom_idx + 2);
                  else
                    {
                      unsigned char *blob;
                      int size;
                      gaiaToSpatiaLiteBlobWkb(outGeom, &blob, &size);
                      sqlite3_bind_blob(stmt_out, geom_idx + 2, blob, size,
                                        free);
                      gaiaFreeGeomColl(outGeom);
                    }

                  for (i = 0; i < n_columns; i++)
                    {
                      int type = sqlite3_column_type(stmt_in, i);
                      if (i == geom_idx)
                        continue;
                      switch (type)
                        {
                          case SQLITE_INTEGER:
                            sqlite3_bind_int64(stmt_out, i + 2,
                                               sqlite3_column_int(stmt_in, i));
                            break;
                          case SQLITE_FLOAT:
                            sqlite3_bind_double(stmt_out, i + 2,
                                                sqlite3_column_double(stmt_in,
                                                                      i));
                            break;
                          case SQLITE_TEXT:
                            sqlite3_bind_text(stmt_out, i + 2,
                                              (const char *)
                                              sqlite3_column_text(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_BLOB:
                            sqlite3_bind_blob(stmt_out, i + 2,
                                              sqlite3_column_blob(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_NULL:
                          default:
                            sqlite3_bind_null(stmt_out, i + 2);
                            break;
                        };
                    }

                  ret = sqlite3_step(stmt_out);
                  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                    ;
                  else
                    {
                      wxString err =
                        wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
                      wxMessageBox(wxT("[OUT]step error: ") + err,
                                   wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                                   this);
                      goto abort;
                    }
                  ln = ln->Next;
                }
              pg = g->FirstPolygon;
              while (pg)
                {
                  // separating Polygons
                  outGeom = GeomFromPolygon(pg, g->Srid);
                  sqlite3_reset(stmt_out);
                  sqlite3_clear_bindings(stmt_out);
                  sqlite3_bind_int64(stmt_out, 1, id);
                  if (!outGeom)
                    sqlite3_bind_null(stmt_out, geom_idx + 2);
                  else
                    {
                      unsigned char *blob;
                      int size;
                      gaiaToSpatiaLiteBlobWkb(outGeom, &blob, &size);
                      sqlite3_bind_blob(stmt_out, geom_idx + 2, blob, size,
                                        free);
                      gaiaFreeGeomColl(outGeom);
                    }

                  for (i = 0; i < n_columns; i++)
                    {
                      int type = sqlite3_column_type(stmt_in, i);
                      if (i == geom_idx)
                        continue;
                      switch (type)
                        {
                          case SQLITE_INTEGER:
                            sqlite3_bind_int64(stmt_out, i + 2,
                                               sqlite3_column_int(stmt_in, i));
                            break;
                          case SQLITE_FLOAT:
                            sqlite3_bind_double(stmt_out, i + 2,
                                                sqlite3_column_double(stmt_in,
                                                                      i));
                            break;
                          case SQLITE_TEXT:
                            sqlite3_bind_text(stmt_out, i + 2,
                                              (const char *)
                                              sqlite3_column_text(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_BLOB:
                            sqlite3_bind_blob(stmt_out, i + 2,
                                              sqlite3_column_blob(stmt_in, i),
                                              sqlite3_column_bytes(stmt_in, i),
                                              SQLITE_STATIC);
                            break;
                          case SQLITE_NULL:
                          default:
                            sqlite3_bind_null(stmt_out, i + 2);
                            break;
                        };
                    }

                  ret = sqlite3_step(stmt_out);
                  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                    ;
                  else
                    {
                      wxString err =
                        wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
                      wxMessageBox(wxT("[OUT]step error: ") + err,
                                   wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                                   this);
                      goto abort;
                    }
                  pg = pg->Next;
                }
              gaiaFreeGeomColl(g);
            }
          id++;
      } else
        {
          wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          wxMessageBox(wxT("[IN]step error: ") + err, wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          goto abort;
        }
    }
  sqlite3_finalize(stmt_in);
  sqlite3_finalize(stmt_out);

// commits the transaction 
  ret = sqlite3_exec(SqliteHandle, "COMMIT", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("COMMIT error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  return true;

abort:
  if (stmt_in)
    sqlite3_finalize(stmt_in);
  if (stmt_out)
    sqlite3_finalize(stmt_out);
  return false;
}

gaiaGeomCollPtr MyFrame::GeomFromPoint(gaiaPointPtr pt, int srid)
{
// creating a Geometry containing a single Point
  gaiaGeomCollPtr g = NULL;
  switch (pt->DimensionModel)
    {
      case GAIA_XY_Z_M:
        g = gaiaAllocGeomCollXYZM();
        break;
      case GAIA_XY_Z:
        g = gaiaAllocGeomCollXYZ();
        break;
      case GAIA_XY_M:
        g = gaiaAllocGeomCollXYM();
        break;
      default:
        g = gaiaAllocGeomColl();
        break;
    };
  if (!g)
    return NULL;
  g->Srid = srid;
  g->DeclaredType = GAIA_POINT;
  switch (pt->DimensionModel)
    {
      case GAIA_XY_Z_M:
        gaiaAddPointToGeomCollXYZM(g, pt->X, pt->Y, pt->Z, pt->M);
        break;
      case GAIA_XY_Z:
        gaiaAddPointToGeomCollXYZ(g, pt->X, pt->Y, pt->Z);
        break;
      case GAIA_XY_M:
        gaiaAddPointToGeomCollXYM(g, pt->X, pt->Y, pt->M);
        break;
      default:
        gaiaAddPointToGeomColl(g, pt->X, pt->Y);
        break;
    };
  return g;
}

gaiaGeomCollPtr MyFrame::GeomFromLinestring(gaiaLinestringPtr ln, int srid)
{
// creating a Geometry containing a single Linestring
  gaiaGeomCollPtr g = NULL;
  gaiaLinestringPtr ln2;
  int iv;
  double x;
  double y;
  double z;
  double m;
  switch (ln->DimensionModel)
    {
      case GAIA_XY_Z_M:
        g = gaiaAllocGeomCollXYZM();
        break;
      case GAIA_XY_Z:
        g = gaiaAllocGeomCollXYZ();
        break;
      case GAIA_XY_M:
        g = gaiaAllocGeomCollXYM();
        break;
      default:
        g = gaiaAllocGeomColl();
        break;
    };
  if (!g)
    return NULL;
  g->Srid = srid;
  g->DeclaredType = GAIA_LINESTRING;
  ln2 = gaiaAddLinestringToGeomColl(g, ln->Points);
  switch (ln->DimensionModel)
    {
      case GAIA_XY_Z_M:
        for (iv = 0; iv < ln->Points; iv++)
          {
            gaiaGetPointXYZM(ln->Coords, iv, &x, &y, &z, &m);
            gaiaSetPointXYZM(ln2->Coords, iv, x, y, z, m);
          }
        break;
      case GAIA_XY_Z:
        for (iv = 0; iv < ln->Points; iv++)
          {
            gaiaGetPointXYZ(ln->Coords, iv, &x, &y, &z);
            gaiaSetPointXYZ(ln2->Coords, iv, x, y, z);
          }
        break;
      case GAIA_XY_M:
        for (iv = 0; iv < ln->Points; iv++)
          {
            gaiaGetPointXYM(ln->Coords, iv, &x, &y, &m);
            gaiaSetPointXYM(ln2->Coords, iv, x, y, m);
          }
        break;
      default:
        for (iv = 0; iv < ln->Points; iv++)
          {
            gaiaGetPoint(ln->Coords, iv, &x, &y);
            gaiaSetPoint(ln2->Coords, iv, x, y);
          }
        break;
    };
  return g;
}

gaiaGeomCollPtr MyFrame::GeomFromPolygon(gaiaPolygonPtr pg, int srid)
{
// creating a Geometry containing a single Polygon
  gaiaGeomCollPtr g = NULL;
  gaiaPolygonPtr pg2;
  gaiaRingPtr rng;
  gaiaRingPtr rng2;
  int ib;
  int iv;
  double x;
  double y;
  double z;
  double m;
  switch (pg->DimensionModel)
    {
      case GAIA_XY_Z_M:
        g = gaiaAllocGeomCollXYZM();
        break;
      case GAIA_XY_Z:
        g = gaiaAllocGeomCollXYZ();
        break;
      case GAIA_XY_M:
        g = gaiaAllocGeomCollXYM();
        break;
      default:
        g = gaiaAllocGeomColl();
        break;
    };
  if (!g)
    return NULL;
  g->Srid = srid;
  g->DeclaredType = GAIA_POLYGON;
  rng = pg->Exterior;
  pg2 = gaiaAddPolygonToGeomColl(g, rng->Points, pg->NumInteriors);
  rng2 = pg2->Exterior;
  switch (pg->DimensionModel)
    {
      case GAIA_XY_Z_M:
        for (iv = 0; iv < rng->Points; iv++)
          {
            gaiaGetPointXYZM(rng->Coords, iv, &x, &y, &z, &m);
            gaiaSetPointXYZM(rng2->Coords, iv, x, y, z, m);
          }
        for (ib = 0; ib < pg->NumInteriors; ib++)
          {
            rng = pg->Interiors + ib;
            rng2 = gaiaAddInteriorRing(pg2, ib, rng->Points);
            for (iv = 0; iv < rng->Points; iv++)
              {
                gaiaGetPointXYZM(rng->Coords, iv, &x, &y, &z, &m);
                gaiaSetPointXYZM(rng2->Coords, iv, x, y, z, m);
              }
          }
        break;
      case GAIA_XY_Z:
        for (iv = 0; iv < rng->Points; iv++)
          {
            gaiaGetPointXYZ(rng->Coords, iv, &x, &y, &z);
            gaiaSetPointXYZ(rng2->Coords, iv, x, y, z);
          }
        for (ib = 0; ib < pg->NumInteriors; ib++)
          {
            rng = pg->Interiors + ib;
            rng2 = gaiaAddInteriorRing(pg2, ib, rng->Points);
            for (iv = 0; iv < rng->Points; iv++)
              {
                gaiaGetPointXYZ(rng->Coords, iv, &x, &y, &z);
                gaiaSetPointXYZ(rng2->Coords, iv, x, y, z);
              }
          }
        break;
      case GAIA_XY_M:
        for (iv = 0; iv < rng->Points; iv++)
          {
            gaiaGetPointXYM(rng->Coords, iv, &x, &y, &m);
            gaiaSetPointXYM(rng2->Coords, iv, x, y, m);
          }
        for (ib = 0; ib < pg->NumInteriors; ib++)
          {
            rng = pg->Interiors + ib;
            rng2 = gaiaAddInteriorRing(pg2, ib, rng->Points);
            for (iv = 0; iv < rng->Points; iv++)
              {
                gaiaGetPointXYM(rng->Coords, iv, &x, &y, &m);
                gaiaSetPointXYM(rng2->Coords, iv, x, y, m);
              }
          }
        break;
      default:
        for (iv = 0; iv < rng->Points; iv++)
          {
            gaiaGetPoint(rng->Coords, iv, &x, &y);
            gaiaSetPoint(rng2->Coords, iv, x, y);
          }
        for (ib = 0; ib < pg->NumInteriors; ib++)
          {
            rng = pg->Interiors + ib;
            rng2 = gaiaAddInteriorRing(pg2, ib, rng->Points);
            for (iv = 0; iv < rng->Points; iv++)
              {
                gaiaGetPoint(rng->Coords, iv, &x, &y);
                gaiaSetPoint(rng2->Coords, iv, x, y);
              }
          }
        break;
    };
  return g;
}

void MyFrame::InitTableTree()
{
// loads the table TREE list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *createSql;
  char *type;
  wxString tblName;
  wxString sql;
  bool virtualTable = false;
  TableTree->Show(false);
  if (MemoryDatabase == true)
    {
      wxString memory = wxT("MEMORY-DB");
      TableTree->SetPath(memory);
  } else
    TableTree->SetPath(SqlitePath);
  TableTree->FlushAll();
  if (ExistsTopologyMaster())
    {
      //
      wxString column_list;
      GetTopologyColumns(&column_list);
      // fetching topologies
      sql = wxT("SELECT ");
      sql += column_list;
      sql += wxT(" FROM topology_master");
      int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                                  &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          int srid_column = -1;
          int dims_column = -1;
          for (int c = 0; c < columns; c++)
            {
              const char *name = results[c];
              if (strcasecmp(name, "srid") == 0)
                srid_column = c;
              if (strcasecmp(name, "coord_dimension") == 0)
                dims_column = c;
            }
          for (i = 1; i <= rows; i++)
            {
              // adding some Topology
              TopologySet topology;
              for (int c = 0; c < columns; c++)
                {
                  const char *name = results[(i * columns) + c];
                  if (name != NULL)
                    {
                      if (c == srid_column)
                        topology.SetSrid(atoi(name));
                      else if (c == dims_column)
                        topology.SetCoordDims(name);
                      else
                        {
                          bool table;
                          bool view;
                          CheckIfExists(name, &table, &view);
                          if (table == true || view == true)
                            topology.Add(name, table, view);
                        }
                    }
                }
              if (topology.CheckPrefix() == true)
                TableTree->AddTopology(&topology);
            }
        }
      sqlite3_free_table(results);
    }
// fetching persistent tables and views
  sql =
    wxT
    ("SELECT name, sql, type FROM sqlite_master WHERE (type = 'table' OR type = 'view') ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          createSql = results[(i * columns) + 1];
          type = results[(i * columns) + 2];
          if (strstr(createSql, " VIRTUAL ") || strstr(createSql, " virtual "))
            virtualTable = true;
          else
            virtualTable = false;
          tblName = wxString::FromUTF8(name);
          if (strcmp(type, "view") == 0)
            TableTree->AddView(tblName, false);
          else
            TableTree->AddTable(tblName, virtualTable, false);
        }
    }
  sqlite3_free_table(results);
// fetching temporary tables and views
  sql =
    wxT
    ("SELECT name, sql, type FROM sqlite_temp_master WHERE (type = 'table' OR type = 'view') ORDER BY name");
  ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          createSql = results[(i * columns) + 1];
          type = results[(i * columns) + 2];
          if (strstr(createSql, " VIRTUAL ") || strstr(createSql, " virtual "))
            virtualTable = true;
          else
            virtualTable = false;
          tblName = wxString::FromUTF8(name);
          if (strcmp(type, "view") == 0)
            TableTree->AddView(tblName, true);
          else
            TableTree->AddTable(tblName, virtualTable, true);
        }
    }
  sqlite3_free_table(results);
  ListAttached();
  TableTree->ExpandRoot();
  TableTree->Show(true);
}

void MyFrame::InitTableTree(wxString & dbAlias, wxString & path)
{
// loads the table TREE list [ATTACHED DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *createSql;
  char *type;
  wxString tblName;
  wxString sql;
  bool virtualTable = false;
  wxString dbInfos = dbAlias + wxT(": ") + path;
  wxTreeItemId db = TableTree->AppendItem(TableTree->GetRootItem(), dbInfos);
  TableTree->SetItemData(db,
                         (wxTreeItemData *) (new
                                             MyObject(MY_ATTACHED, true,
                                                      dbAlias, path)));
  TableTree->SetItemImage(db, 21);
  wxTreeItemId rootUserData = TableTree->AppendItem(db, wxT("User Data"));
  wxTreeItemId rootIsoMetadata = TableTree->AppendItem(db, wxT("ISO Metadata"));
  wxTreeItemId rootStyling = TableTree->AppendItem(db, wxT("Styling"));
  wxTreeItemId rootTopologies = TableTree->AppendItem(db, wxT("Topologies"));
  wxTreeItemId rootMetadata = TableTree->AppendItem(db, wxT("Metadata"));
  wxTreeItemId rootInternal = TableTree->AppendItem(db, wxT("Internal Data"));
  wxTreeItemId rootSpatialIndex =
    TableTree->AppendItem(db, wxT("Spatial Index"));
  TableTree->SetItemImage(rootUserData, 17);
  TableTree->SetItemImage(rootTopologies, 17);
  TableTree->SetItemImage(rootStyling, 17);
  TableTree->SetItemImage(rootIsoMetadata, 17);
  TableTree->SetItemImage(rootMetadata, 17);
  TableTree->SetItemImage(rootInternal, 17);
  TableTree->SetItemImage(rootSpatialIndex, 17);
  RootNodes nodes =
    RootNodes(dbAlias, rootUserData, rootTopologies, rootStyling,
              rootIsoMetadata, rootMetadata,
              rootInternal,
              rootSpatialIndex);
  if (ExistsTopologyMaster(dbAlias))
    {
      //
      wxString column_list;
      GetTopologyColumns(dbAlias, &column_list);
      // fetching topologies
      sql = wxT("SELECT ");
      sql += column_list;
      sql += wxT(" FROM ") + dbAlias + wxT(".topology_master");
      int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                                  &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          int srid_column = -1;
          int dims_column = -1;
          for (int c = 0; c < columns; c++)
            {
              const char *name = results[c];
              if (strcasecmp(name, "srid") == 0)
                srid_column = c;
              if (strcasecmp(name, "coord_dimension") == 0)
                dims_column = c;
            }
          for (i = 1; i <= rows; i++)
            {
              // adding some Topology
              TopologySet topology;
              for (int c = 0; c < columns; c++)
                {
                  const char *name = results[(i * columns) + c];
                  if (name != NULL)
                    {
                      if (c == srid_column)
                        topology.SetSrid(atoi(name));
                      else if (c == dims_column)
                        topology.SetCoordDims(name);
                      else
                        {
                          bool table;
                          bool view;
                          CheckIfExists(dbAlias, name, &table, &view);
                          if (table == true || view == true)
                            topology.Add(name, table, view);
                        }
                    }
                }
              if (topology.CheckPrefix() == true)
                TableTree->AddTopology(rootTopologies, &topology);
            }
        }
      sqlite3_free_table(results);
    }
// fetching tables and views
  sql = wxT("SELECT name, sql, type FROM ");
  sql += dbAlias;
  sql +=
    wxT(".sqlite_master WHERE (type = 'table' OR type = 'view') ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          createSql = results[(i * columns) + 1];
          type = results[(i * columns) + 2];
          if (strstr(createSql, " VIRTUAL ") || strstr(createSql, " virtual "))
            virtualTable = true;
          else
            virtualTable = false;
          tblName = wxString::FromUTF8(name);
          if (strcmp(type, "view") == 0)
            TableTree->AddView(dbAlias, tblName, &nodes);
          else
            TableTree->AddTable(dbAlias, tblName, virtualTable, &nodes);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::ListAttached()
{
// 
// listing all Attached DBs
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  ret = sqlite3_get_table(GetSqlite(), "PRAGMA database_list", &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          if (strcasecmp("main", results[(i * columns) + 1]) == 0)
            continue;
          if (strcasecmp("temp", results[(i * columns) + 1]) == 0)
            continue;
          wxString dbAlias = wxString::FromUTF8(results[(i * columns) + 1]);
          wxString dbPath = wxString::FromUTF8(results[(i * columns) + 2]);
          InitTableTree(dbAlias, dbPath);
        }
    }
  sqlite3_free_table(results);
}

wxString *MyFrame::GetTables(int *n)
{
// loads the table list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString *tables = NULL;
  wxString sql;
  *n = 0;
  sql =
    wxT("SELECT name FROM sqlite_master WHERE type = 'table' ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return tables;
    }
  if (rows < 1)
    ;
  else
    {
      tables = new wxString[rows];
      *n = rows;
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          tables[i - 1] = wxString::FromUTF8(name);
        }
    }
  sqlite3_free_table(results);
  return tables;
}

void MyFrame::GetTableColumns(wxString & tableName, MyTableInfo * list)
{
// loads the table's column list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *column;
  wxString Name;
  wxString Column;
  bool pKey;
  bool index;
  bool cached;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (atoi(results[(i * columns) + 5]) == 0)
            pKey = false;
          else
            pKey = true;
          Name = wxString::FromUTF8(name);
          list->AddColumn(Name, pKey);
        }
    }
  sqlite3_free_table(results);
  if (CheckMetadata() == true)
    {
      // ok, Spatial MetaData exists; retrieving Geometries and Spatial Indices
      sql =
        wxT
        ("SELECT f_geometry_column, spatial_index_enabled FROM geometry_columns ");
      sql += wxT("WHERE Lower(f_table_name) = Lower('");
      strcpy(xname, tableName.ToUTF8());
      CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      ret =
        sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              column = results[(i * columns) + 0];
              if (atoi(results[(i * columns) + 1]) == 1)
                index = true;
              else
                index = false;
              if (atoi(results[(i * columns) + 1]) == 2)
                cached = true;
              else
                cached = false;
              Column = wxString::FromUTF8(column);
              list->SetGeometry(Column, index, cached);
            }
        }
      sqlite3_free_table(results);

      if (HasVirtsMetadata() == true)
        {
          // may also be some VirtualShape
          sql = wxT("SELECT virt_geometry FROM virts_geometry_columns ");
          sql += wxT("WHERE Lower(virt_name) = Lower('");
          strcpy(xname, tableName.ToUTF8());
          CleanSqlString(xname);
          sql += wxString::FromUTF8(xname);
          sql += wxT("')");
          ret =
            sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results, &rows,
                              &columns, &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              return;
            }
          if (rows < 1)
            ;
          else
            {
              for (i = 1; i <= rows; i++)
                {
                  column = results[(i * columns) + 0];
                  Column = wxString::FromUTF8(column);
                  list->SetGeometry(Column, false, false);
                }
            }
          sqlite3_free_table(results);
        }
    }
}

void MyFrame::GetTableColumns(wxString & dbAlias, wxString & tableName,
                              MyTableInfo * list)
{
// loads the table's column list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *column;
  wxString Name;
  wxString Column;
  bool pKey;
  bool index;
  bool cached;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA ") + dbAlias + wxT(".table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (atoi(results[(i * columns) + 5]) == 0)
            pKey = false;
          else
            pKey = true;
          Name = wxString::FromUTF8(name);
          list->AddColumn(Name, pKey);
        }
    }
  sqlite3_free_table(results);
  if (CheckMetadata(dbAlias) == true)
    {
      // ok, Spatial MetaData exists; retrieving Geometries and Spatial Indices
      sql = wxT("SELECT f_geometry_column, spatial_index_enabled FROM ");
      sql += dbAlias + wxT(".geometry_columns ");
      sql += wxT("WHERE Lower(f_table_name) = Lower('");
      strcpy(xname, tableName.ToUTF8());
      CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      ret =
        sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              column = results[(i * columns) + 0];
              if (atoi(results[(i * columns) + 1]) == 1)
                index = true;
              else
                index = false;
              if (atoi(results[(i * columns) + 1]) == 2)
                cached = true;
              else
                cached = false;
              Column = wxString::FromUTF8(column);
              list->SetGeometry(Column, index, cached);
            }
        }
      sqlite3_free_table(results);

      if (HasVirtsMetadata(dbAlias) == true)
        {
          // may also be some VirtualShape
          sql = wxT("SELECT virt_geometry FROM ");
          sql += dbAlias + wxT(".virts_geometry_columns ");
          sql += wxT("WHERE Lower(virt_name) = Lower('");
          strcpy(xname, tableName.ToUTF8());
          CleanSqlString(xname);
          sql += wxString::FromUTF8(xname);
          sql += wxT("')");
          ret =
            sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results, &rows,
                              &columns, &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              return;
            }
          if (rows < 1)
            ;
          else
            {
              for (i = 1; i <= rows; i++)
                {
                  column = results[(i * columns) + 0];
                  Column = wxString::FromUTF8(column);
                  list->SetGeometry(Column, false, false);
                }
            }
          sqlite3_free_table(results);
        }
    }
}

bool MyFrame::IsPrimaryKey(wxString & tableName, wxString & columnName)
{
// checking if some column is a PRIMARY KEY
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  bool pk = false;
  wxString sql;
  char column[1024];
  char xname[1024];
  strcpy(column, columnName.ToUTF8());
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (strcasecmp(name, column) == 0)
            {
              if (atoi(results[(i * columns) + 5]) != 0)
                pk = true;
            }
        }
    }
  sqlite3_free_table(results);
  return pk;
}

void MyFrame::GetViewColumns(wxString & tableName, MyViewInfo * list)
{
// loads the view's column list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *column;
  wxString Name;
  wxString Column;
  bool index;
  bool cached;
  wxString sql;
  char xname[1024];
  char xsql[4192];
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  int ret = sqlite3_get_table(SqliteHandle, xsql, &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          Name = wxString::FromUTF8(name);
          list->AddColumn(Name);
        }
    }
  sqlite3_free_table(results);
  if (HasViewsMetadata() == true)
    {
      // ok, Spatial MetaData exists; retrieving Geometries and Spatial Indices
      sql = wxT("SELECT a.view_geometry, b.spatial_index_enabled ");
      sql += wxT("FROM views_geometry_columns AS a ");
      sql += wxT("JOIN geometry_columns AS b ON (");
      sql += wxT("Lower(a.f_table_name) = Lower(b.f_table_name) AND ");
      sql += wxT("Lower(a.f_geometry_column) = Lower(b.f_geometry_column)) ");
      sql += wxT("WHERE Lower(view_name) = Lower('");
      strcpy(xname, tableName.ToUTF8());
      CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      strcpy(xsql, sql.ToUTF8());
      ret =
        sqlite3_get_table(SqliteHandle, xsql, &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              column = results[(i * columns) + 0];
              if (atoi(results[(i * columns) + 1]) == 1)
                index = true;
              else
                index = false;
              if (atoi(results[(i * columns) + 1]) == 2)
                cached = true;
              else
                cached = false;
              Column = wxString::FromUTF8(column);
              list->SetGeometry(Column, index, cached);
            }
        }
      sqlite3_free_table(results);
    }
}

void MyFrame::GetViewColumns(wxString & dbAlias, wxString & tableName,
                             MyViewInfo * list)
{
// loads the view's column list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *column;
  wxString Name;
  wxString Column;
  bool index;
  bool cached;
  wxString sql;
  char xname[1024];
  char xsql[4192];
  sql = wxT("PRAGMA ") + dbAlias + wxT(".table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  int ret = sqlite3_get_table(SqliteHandle, xsql, &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          Name = wxString::FromUTF8(name);
          list->AddColumn(Name);
        }
    }
  sqlite3_free_table(results);
  if (HasViewsMetadata(dbAlias) == true)
    {
      // ok, Spatial MetaData exists; retrieving Geometries and Spatial Indices
      sql = wxT("SELECT a.view_geometry, b.spatial_index_enabled ");
      sql += wxT("FROM ") + dbAlias + wxT(".views_geometry_columns AS a ");
      sql += wxT("JOIN ") + dbAlias + wxT(".geometry_columns AS b ON (");
      sql += wxT("Lower(a.f_table_name) = Lower(b.f_table_name) AND ");
      sql += wxT("Lower(a.f_geometry_column) = Lower(b.f_geometry_column)) ");
      sql += wxT("WHERE Lower(view_name) = Lower('");
      strcpy(xname, tableName.ToUTF8());
      CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      strcpy(xsql, sql.ToUTF8());
      ret =
        sqlite3_get_table(SqliteHandle, xsql, &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              column = results[(i * columns) + 0];
              if (atoi(results[(i * columns) + 1]) == 1)
                index = true;
              else
                index = false;
              if (atoi(results[(i * columns) + 1]) == 2)
                cached = true;
              else
                cached = false;
              Column = wxString::FromUTF8(column);
              list->SetGeometry(Column, index, cached);
            }
        }
      sqlite3_free_table(results);
    }
}

void MyFrame::GetViewTriggers(wxString & tableName, MyViewInfo * list)
{
// loads the view's indices list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql =
    wxT
    ("SELECT name FROM sqlite_master WHERE type = 'trigger' AND tbl_name = '");
  strcpy(xname, tableName.ToUTF8());
  CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("' ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          Name = wxString::FromUTF8(name);
          list->AddTrigger(Name);
        }
    }
  sqlite3_free_table(results);
  TableTree->ExpandRoot();
}

void MyFrame::GetViewTriggers(wxString & dbAlias, wxString & tableName,
                              MyViewInfo * list)
{
// loads the view's indices list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql =
    wxT("SELECT name FROM ") + dbAlias +
    wxT(".sqlite_master WHERE type = 'trigger' AND tbl_name = '");
  strcpy(xname, tableName.ToUTF8());
  CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("' ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          Name = wxString::FromUTF8(name);
          list->AddTrigger(Name);
        }
    }
  sqlite3_free_table(results);
  TableTree->ExpandRoot();
}

void MyFrame::GetTableIndices(wxString & tableName, MyTableInfo * list)
{
// loads the table's indices list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA index_list(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          Name = wxString::FromUTF8(name);
          list->AddIndex(Name);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetTableIndices(wxString & dbAlias, wxString & tableName,
                              MyTableInfo * list)
{
// loads the table's indices list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA ") + dbAlias + wxT(".index_list(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          Name = wxString::FromUTF8(name);
          list->AddIndex(Name);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetIndexFields(wxString & indexName, wxTreeItemId & node)
{
// loads the index fields list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA index_info(");
  strcpy(xname, indexName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 2];
          Name = wxString::FromUTF8(name);
          TableTree->AppendItem(node, Name, 3);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetIndexFields(wxString & dbAlias, wxString & indexName,
                             wxTreeItemId & node)
{
// loads the index fields list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA ") + dbAlias + wxT(".index_info(");
  strcpy(xname, indexName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 2];
          Name = wxString::FromUTF8(name);
          TableTree->AppendItem(node, Name, 3);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetPrimaryKeyFields(wxString & indexName, wxTreeItemId & node)
{
// loads the Primary Key fields list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA index_info(");
  strcpy(xname, indexName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 2];
          Name = wxString::FromUTF8(name);
          TableTree->AppendItem(node, Name, 2);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetPrimaryKeyFields(wxString & dbAlias, wxString & indexName,
                                  wxTreeItemId & node)
{
// loads the Primary Key fields list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("PRAGMA ") + dbAlias + wxT(".index_info(");
  strcpy(xname, indexName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 2];
          Name = wxString::FromUTF8(name);
          TableTree->AppendItem(node, Name, 2);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetForeignKeys(wxString & tableName, wxTreeItemId & node)
{
// loads the Foreign Keys list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *ref_table;
  char *column;
  int id;
  int uid = INT_MIN;
  wxString refTable;
  wxString Column;
  wxString sql;
  char xname[1024];
  wxTreeItemId item;
  sql = wxT("PRAGMA foreign_key_list(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          id = atoi(results[(i * columns) + 0]);
          ref_table = results[(i * columns) + 2];
          column = results[(i * columns) + 3];
          refTable = wxString::FromUTF8(ref_table);
          Column = wxString::FromUTF8(column);
          if (i == 1)
            {
              wxString fkName = wxT("FK#");
              sprintf(xname, "%d", id);
              fkName += wxString::FromUTF8(xname);
              fkName += wxT(" ref: ") + refTable;
              item = TableTree->AppendItem(node, fkName, 18);
              uid = id;
          } else if (id != uid)
            {
              wxString fkName = wxT("FK#");
              sprintf(xname, "%d", id);
              fkName += wxString::FromUTF8(xname);
              fkName += wxT(" ref: ") + refTable;
              item = TableTree->AppendItem(node, fkName, 18);
              uid = id;
            }
          TableTree->AppendItem(item, Column, 3);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetForeignKeys(wxString & dbAlias, wxString & tableName,
                             wxTreeItemId & node)
{
// loads the Foreign Keys list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *ref_table;
  char *column;
  int id;
  int uid = INT_MIN;
  wxString refTable;
  wxString Column;
  wxString sql;
  char xname[1024];
  wxTreeItemId item;
  sql = wxT("PRAGMA ") + dbAlias + wxT(".foreign_key_list(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          id = atoi(results[(i * columns) + 0]);
          ref_table = results[(i * columns) + 2];
          column = results[(i * columns) + 3];
          refTable = wxString::FromUTF8(ref_table);
          Column = wxString::FromUTF8(column);
          if (i == 1)
            {
              wxString fkName = wxT("FK#");
              sprintf(xname, "%d", id);
              fkName += wxString::FromUTF8(xname);
              fkName += wxT(" ref: ") + refTable;
              item = TableTree->AppendItem(node, fkName, 18);
              uid = id;
          } else if (id != uid)
            {
              wxString fkName = wxT("FK#");
              sprintf(xname, "%d", id);
              fkName += wxString::FromUTF8(xname);
              fkName += wxT(" ref: ") + refTable;
              item = TableTree->AppendItem(node, fkName, 18);
              uid = id;
            }
          TableTree->AppendItem(item, Column, 3);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::GetTableTriggers(wxString & tableName, MyTableInfo * list)
{
// loads the table's indices list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql =
    wxT
    ("SELECT name FROM sqlite_master WHERE type = 'trigger' AND tbl_name = '");
  strcpy(xname, tableName.ToUTF8());
  CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("' ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          Name = wxString::FromUTF8(name);
          list->AddTrigger(Name);
        }
    }
  sqlite3_free_table(results);
  TableTree->ExpandRoot();
}

void MyFrame::GetTableTriggers(wxString & dbAlias, wxString & tableName,
                               MyTableInfo * list)
{
// loads the table's indices list [Attached DB]
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString Name;
  wxString sql;
  char xname[1024];
  sql = wxT("SELECT name FROM ") + dbAlias;
  sql += wxT(".sqlite_master WHERE type = 'trigger' AND tbl_name = '");
  strcpy(xname, tableName.ToUTF8());
  CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("' ORDER BY name");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          Name = wxString::FromUTF8(name);
          list->AddTrigger(Name);
        }
    }
  sqlite3_free_table(results);
  TableTree->ExpandRoot();
}

wxString *MyFrame::GetColumnNames(wxString & tableName, int *n_cols)
{
// loads the table's column names list
  wxString *cols = NULL;
  int nCols = 0;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  char *column;
  char xname[1024];
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, tableName.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return NULL;
    }
  sql = wxT("");
  if (rows < 1)
    ;
  else
    {
      nCols = rows;
      cols = new wxString[rows];
      for (i = 1; i <= rows; i++)
        {
          column = results[(i * columns) + 1];
          *(cols + i - 1) += wxString::FromUTF8(column);
        }
    }
  sqlite3_free_table(results);
  *n_cols = nCols;
  return cols;
}

int MyFrame::GetCharsetIndex(wxString & charset)
{
// identifies the INDEX for a given charset
  int i;
  for (i = 0; i < CharsetsLen; i++)
    {
      if (*(Charsets + i) == charset)
        return i;
    }
  return wxNOT_FOUND;
}

wxString & MyFrame::GetCharsetName(wxString & charset)
{
// identifies the full name for a given charset code
  int i;
  for (i = 0; i < CharsetsLen; i++)
    {
      if (*(Charsets + i) == charset)
        return *(CharsetsNames + i);
    }
  return charset;
}

void MyFrame::ClearTableTree()
{
// resets the table TREE list to the empty state
  wxString path = wxT("no current DB");
  TableTree->SetPath(path);
  TableTree->FlushAll();
}

void MyFrame::AutoFDOStart()
{
//
// trying to start the FDO-OGR auto-wrapper
//
  int ret;
  const char *name;
  int i;
  char **results;
  int rows;
  int columns;
  char sql[1024];
  int count = 0;
  int len;
  int spatial_type = 0;
  AutoFDOTables tables;
  AutoFDOTable *p;
  wxString fdoNames[5];
  char xname[1024];
  char xname2[1024];
  SpatiaLiteMetadata = false;
  AutoFDOmsg = wxT("");
  strcpy(sql, "SELECT CheckSpatialMetadata()");
  ret = sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    goto error1;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        spatial_type = atoi(results[(i * columns) + 0]);
    }
  sqlite3_free_table(results);
error1:
  if (spatial_type == 1 || spatial_type == 3)
    SpatiaLiteMetadata = true;
  if (spatial_type == 2)
    {
      //
      // ok, creating VirtualFDO tables 
      //
      strcpy(sql, "SELECT DISTINCT f_table_name FROM geometry_columns");
      ret =
        sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
      if (ret != SQLITE_OK)
        goto error;
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 0];
              if (name)
                {
                  len = strlen(name);
                  tables.Add(name, len);
                }
            }
        }
      sqlite3_free_table(results);
      p = tables.GetFirst();
      while (p)
        {
          //
          // destroying the VirtualFDO table [if existing] 
          //
          sprintf(xname, "fdo_%s", p->GetName());
          DoubleQuotedSql(xname);
          sprintf(sql, "DROP TABLE IF EXISTS %s", xname);
          ret = sqlite3_exec(SqliteHandle, sql, NULL, 0, NULL);
          if (ret != SQLITE_OK)
            goto error;
          //
          // creating the VirtualFDO table 
          //
          sprintf(xname, "fdo_%s", p->GetName());
          DoubleQuotedSql(xname);
          strcpy(xname2, p->GetName());
          DoubleQuotedSql(xname2);
          sprintf(sql, "CREATE VIRTUAL TABLE %s USING VirtualFDO(%s)",
                  xname, xname2);
          ret = sqlite3_exec(SqliteHandle, sql, NULL, 0, NULL);
          if (ret != SQLITE_OK)
            goto error;
          if (count < 5)
            fdoNames[count] =
              wxT("- VirtualTable: fdo_") + wxString::FromUTF8(p->GetName());
          else
            fdoNames[4] = wxT("- ... and others ...");
          count++;
          p = p->GetNext();
        }
    error:
      if (count++)
        {
          AutoFDOmsg =
            wxT("FDO-OGR detected; activating FDO-OGR auto-wrapping ...\n\n");
          if (fdoNames[0].Len() > 0)
            AutoFDOmsg += fdoNames[0] + wxT("\n");
          if (fdoNames[1].Len() > 0)
            AutoFDOmsg += fdoNames[1] + wxT("\n");
          if (fdoNames[2].Len() > 0)
            AutoFDOmsg += fdoNames[2] + wxT("\n");
          if (fdoNames[3].Len() > 0)
            AutoFDOmsg += fdoNames[3] + wxT("\n");
          if (fdoNames[4].Len() > 0)
            AutoFDOmsg += fdoNames[4] + wxT("\n");
          AutoFDOmsg +=
            wxT
            ("\nAccessing these fdo_XX tables you can take full advantage of\n");
          AutoFDOmsg += wxT("FDO-OGR auto-wrapping facility\n");
          AutoFDOmsg +=
            wxT
            ("This allows you to access any specific FDO-OGR Geometry as if it\n");
          AutoFDOmsg +=
            wxT
            ("where native SpatiaLite ones in a completely transparent way.\n");
        }
      return;
    }
}

void MyFrame::AutoFDOStop()
{
//
// trying to stop the FDO-OGR auto-wrapper
//
  int ret;
  const char *name;
  int i;
  char **results;
  int rows;
  int columns;
  char sql[1024];
  int count = 0;
  int len;
  int spatial_type = 0;
  char xname[1024];
  AutoFDOTables tables;
  AutoFDOTable *p;
  AutoFDOmsg = wxT("");
  strcpy(sql, "SELECT CheckSpatialMetadata()");
  ret = sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    goto error1;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        spatial_type = atoi(results[(i * columns) + 0]);
    }
  sqlite3_free_table(results);
error1:
  if (spatial_type == 2)
    {
      //
      // ok, destroying VirtualFDO tables 
      //
      strcpy(sql, "SELECT DISTINCT f_table_name FROM geometry_columns");
      ret =
        sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
      if (ret != SQLITE_OK)
        goto error;
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 0];
              if (name)
                {
                  len = strlen(name);
                  tables.Add(name, len);
                }
            }
        }
      sqlite3_free_table(results);
      p = tables.GetFirst();
      while (p)
        {
          //
          // destroying the VirtualFDO table [if existing] 
          //
          sprintf(xname, "fdo_%s", p->GetName());
          DoubleQuotedSql(xname);
          sprintf(sql, "DROP TABLE IF EXISTS %s", xname);
          ret = sqlite3_exec(SqliteHandle, sql, NULL, 0, NULL);
          if (ret != SQLITE_OK)
            goto error;
          count++;
          p = p->GetNext();
        }
    error:
      if (count++)
        AutoFDOmsg = wxT("FDO-OGR auto-wrapping shutdown done");
      return;
    }
}

bool MyFrame::HasVirtsMetadata()
{
//
// testing if the VIRTS_GEOMETRY_COLUMNS table exists
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  bool defined = false;
  sql = wxT("PRAGMA table_info(virts_geometry_columns)");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        defined = true;
    }
  sqlite3_free_table(results);
  return defined;
}

bool MyFrame::HasVirtsMetadata(wxString & dbAlias)
{
//
// testing if the VIRTS_GEOMETRY_COLUMNS table exists [Attached DB]
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  bool defined = false;
  sql = wxT("PRAGMA ") + dbAlias + wxT(".table_info(virts_geometry_columns)");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        defined = true;
    }
  sqlite3_free_table(results);
  return defined;
}

bool MyFrame::HasViewsMetadata()
{
//
// testing if the VIEWS_GEOMETRY_COLUMNS table exists
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  bool defined = false;
  sql = wxT("PRAGMA table_info(views_geometry_columns)");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        defined = true;
    }
  sqlite3_free_table(results);
  return defined;
}

bool MyFrame::HasViewsMetadata(wxString & dbAlias)
{
//
// testing if the VIEWS_GEOMETRY_COLUMNS table exists [Attached DB]
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  bool defined = false;
  sql = wxT("PRAGMA ") + dbAlias + wxT(".table_info(views_geometry_columns)");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        defined = true;
    }
  sqlite3_free_table(results);
  return defined;
}

int MyFrame::TestDotCommand(const char *p_stmt)
{
//
// identifying DOT-COMMANDS [SQL script]
//
  const char *stmt = p_stmt;
  int len;

  if (*p_stmt == '\n')
    stmt = p_stmt + 1;
  len = strlen(stmt);
  if (strncasecmp(stmt, ".loadxl ", 8) == 0)
    return CMD_LOADXL;
  if (len <= 9)
    return CMD_NONE;
  if (strncasecmp(stmt, ".loadshp ", 9) == 0)
    return CMD_LOADSHP;
  if (strncasecmp(stmt, ".loaddbf ", 9) == 0)
    return CMD_LOADDBF;
  if (strncasecmp(stmt, ".dumpshp ", 9) == 0)
    return CMD_DUMPSHP;
  if (strncasecmp(stmt, ".dumpdbf ", 9) == 0)
    return CMD_DUMPDBF;
  if (strncasecmp(stmt, ".sqllog ", 8) == 0)
    return CMD_SQLLOG;
  return CMD_NONE;
}

bool MyFrame::IsDotCommandLoadShp(const char *stmt, char *path,
                                  char *table, char *charset,
                                  char *column, int *srid, bool * coerce2D,
                                  bool * compressed)
{
//
// attempting to parse a .loadshp command [SQL script]
//
  wxString cmd = wxString::FromUTF8(stmt);
  wxString extPath;
  wxString extTable;
  wxString extCharset;
  wxString extColumn = wxT("Geometry");
  long extSrid = 0;
  bool extCoerce2D = false;
  bool extCompressed = false;
  int count = 0;
  wxStringTokenizer tok(cmd);
  while (tok.HasMoreTokens())
    {
      wxString token = tok.GetNextToken();
      switch (count)
        {
          case 0:
            extPath = token;
            break;
          case 1:
            extTable = token;
            break;
          case 2:
            extCharset = token;
            break;
          case 3:
            if (token.ToLong(&extSrid) == false)
              extSrid = 0;
            break;
          case 4:
            extColumn = token;
            break;
          case 5:
            extCoerce2D = true;
            break;
          case 6:
            extCompressed = true;
            break;
        };
      count++;
    }
  if (count == 3 || count == 4 || count == 5 || count == 6 || count == 7)
    {
      strcpy(path, extPath.ToUTF8());
      strcpy(table, extTable.ToUTF8());
      strcpy(charset, extCharset.ToUTF8());
      strcpy(column, extColumn.ToUTF8());
      *srid = extSrid;
      *coerce2D = extCoerce2D;
      *compressed = extCompressed;
      return true;
    }
  return false;
}

bool MyFrame::IsDotCommandLoadDbf(const char *stmt, char *path,
                                  char *table, char *charset)
{
//
// attempting to parse a .loaddbf command [SQL script]
//
  wxString cmd = wxString::FromUTF8(stmt);
  wxString extPath;
  wxString extTable;
  wxString extCharset;
  int count = 0;
  wxStringTokenizer tok(cmd);
  while (tok.HasMoreTokens())
    {
      wxString token = tok.GetNextToken();
      switch (count)
        {
          case 0:
            extPath = token;
            break;
          case 1:
            extTable = token;
            break;
          case 2:
            extCharset = token;
            break;
        };
      count++;
    }
  if (count == 3)
    {
      strcpy(path, extPath.ToUTF8());
      strcpy(table, extTable.ToUTF8());
      strcpy(charset, extCharset.ToUTF8());
      return true;
    }
  return false;
}

bool MyFrame::IsDotCommandLoadXL(const char *stmt, char *path,
                                 char *table, int *worksheetIndex,
                                 int *firstTitle)
{
//
// attempting to parse a .loadxl command [SQL script]
//
  wxString cmd = wxString::FromUTF8(stmt);
  wxString extPath;
  wxString extTable;
  int extWorksheetIndex = 0;
  bool extFirstTitle = false;
  int count = 0;
  char dummy[128];
  wxStringTokenizer tok(cmd);
  while (tok.HasMoreTokens())
    {
      wxString token = tok.GetNextToken();
      switch (count)
        {
          case 0:
            extPath = token;
            break;
          case 1:
            extTable = token;
            break;
          case 2:
            strcpy(dummy, token.ToUTF8());
            extWorksheetIndex = atoi(dummy);
            break;
          case 3:
            strcpy(dummy, token.ToUTF8());
            if (atoi(dummy) == 1)
              extFirstTitle = true;
            break;
        };
      count++;
    }
  if (count == 3 || count == 4 || count == 5)
    {
      if (extFirstTitle == true)
        *firstTitle = 1;
      else
        *firstTitle = 0;
      strcpy(path, extPath.ToUTF8());
      strcpy(table, extTable.ToUTF8());
      *worksheetIndex = extWorksheetIndex;
      return true;
    }
  return false;
}

bool MyFrame::IsViewGeometry(wxString & table, wxString & column)
{
//
// checking if some table.geometry corresponds to a VIEW
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  int count = 0;

  sql =
    wxT
    ("SELECT Count(*) FROM views_geometry_columns WHERE Lower(view_name) = Lower('");
  sql += table;
  sql += wxT("') AND view_geometry = Lower('");
  sql += column;
  sql += wxT("')");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          count = atoi(results[(i * columns) + 0]);
        }
    }
  sqlite3_free_table(results);
  if (count)
    return true;
  return false;
}

bool MyFrame::ExistsTopologyMaster()
{
//
// checking if TOPOLOGY_MASTER exists
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  int count = 0;

  sql = wxT("SELECT Count(*) FROM sqlite_master ");
  sql += wxT("WHERE type = 'table' AND ");
  sql += wxT("tbl_name = 'topology_master'");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          count = atoi(results[(i * columns) + 0]);
        }
    }
  sqlite3_free_table(results);
  if (count)
    return true;
  return false;
}

bool MyFrame::ExistsTopologyMaster(wxString & dbAlias)
{
//
// checking if TOPOLOGY_MASTER exists [Attached DB]
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  wxString sql;
  int count = 0;

  sql = wxT("SELECT Count(*) FROM ") + dbAlias + wxT(".sqlite_master ");
  sql += wxT("WHERE type = 'table' AND ");
  sql += wxT("tbl_name = 'topology_master'");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          count = atoi(results[(i * columns) + 0]);
        }
    }
  sqlite3_free_table(results);
  if (count)
    return true;
  return false;
}

void MyFrame::GetTopologyColumns(wxString * list)
{
//
// identifying TOPOLOGY_MASTER columns
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  const char *name;
  int comma = false;
  wxString col_list;

  *list = col_list;
  int ret =
    sqlite3_get_table(SqliteHandle, "PRAGMA table_info(topology_master)",
                      &results,
                      &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (comma == false)
            comma = true;
          else
            col_list += wxT(", ");
          col_list += wxString::FromUTF8(name);
        }
    }
  sqlite3_free_table(results);
  *list = col_list;
}

void MyFrame::GetTopologyColumns(wxString & dbAlias, wxString * list)
{
//
// identifying TOPOLOGY_MASTER columns [Attached DB]
//
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  const char *name;
  int comma = false;
  wxString col_list;
  wxString sql;

  *list = col_list;
  sql = wxT("PRAGMA ") + dbAlias + wxT(".table_info(topology_master)");
  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(),
                              &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (comma == false)
            comma = true;
          else
            col_list += wxT(", ");
          col_list += wxString::FromUTF8(name);
        }
    }
  sqlite3_free_table(results);
  *list = col_list;
}

void MyFrame::CheckIfExists(const char *name, bool * table, bool * view)
{
//
// checking if a Topology related Table or View actually exists
//
  char dummy[2048];
  wxString sql;
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;

  *table = false;
  *view = false;
  strcpy(dummy, name);
  CleanSqlString(dummy);
  sql = wxT("SELECT type FROM sqlite_master WHERE Lower(name) = Lower('");
  sql += wxString::FromUTF8(dummy);
  sql += wxT("')");

  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          if (strcasecmp(name, "table") == 0)
            *table = true;
          if (strcasecmp(name, "view") == 0)
            *view = true;
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::CheckIfExists(wxString & dbAlias, const char *name, bool * table,
                            bool * view)
{
//
// checking if a Topology related Table or View actually exists [Attached DB]
//
  char dummy[2048];
  wxString sql;
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;

  *table = false;
  *view = false;
  strcpy(dummy, name);
  CleanSqlString(dummy);
  sql =
    wxT("SELECT type FROM ") + dbAlias +
    wxT(".sqlite_master WHERE Lower(name) = Lower('");
  sql += wxString::FromUTF8(dummy);
  sql += wxT("')");

  int ret = sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          if (strcasecmp(name, "table") == 0)
            *table = true;
          if (strcasecmp(name, "view") == 0)
            *view = true;
        }
    }
  sqlite3_free_table(results);
}

int MyFrame::GetMetaDataType()
{
//
// determining the MetaData type
//
  int ret;
  int i;
  char **results;
  int rows;
  int columns;
  char sql[1024];
  int spatial_type = 0;
  strcpy(sql, "SELECT CheckSpatialMetadata()");
  ret = sqlite3_get_table(SqliteHandle, sql, &results, &rows, &columns, NULL);
  if (ret != SQLITE_OK)
    goto error1;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        spatial_type = atoi(results[(i * columns) + 0]);
    }
  sqlite3_free_table(results);
  if (spatial_type == 1)
    return METADATA_LEGACY;
  if (spatial_type == 3)
    return METADATA_CURRENT;
error1:
  return METADATA_UNKNOWN;
}

void MyFrame::InsertIntoLog(wxString & sql_stmt)
{
//
// inserting a row into sql_statements_log
//
  char *clean;
  int ret;
  char *errMsg = NULL;
  wxString sql = wxT("INSERT INTO sql_statements_log ");
  sql += wxT("(id, time_start, user_agent, sql_statement) VALUES (");
  sql += wxT("NULL, strftime('%Y-%m-%dT%H:%M:%fZ', 'now'), ");
  sql += wxT("'spatialite_gui', '");
  clean = gaiaSingleQuotedSql(sql_stmt.ToUTF8());
  sql += wxString::FromUTF8(clean);
  free(clean);
  sql += wxT("')");
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("InsertIntoLog: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      LastSqlLogID = -1;
      return;
    }
  LastSqlLogID = sqlite3_last_insert_rowid(SqliteHandle);
}

void MyFrame::UpdateLog()
{
//
// updating sql_statements_log: success
//
  char dummy[64];
  int ret;
  char *errMsg = NULL;
  wxString sql = wxT("UPDATE sql_statements_log SET ");
  sql += wxT("time_end = strftime('%Y-%m-%dT%H:%M:%fZ', 'now'), ");
  sql += wxT("success = 1, error_cause = 'success' ");
#if defined(_WIN32) || defined(__MINGW32__)
  /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
  sprintf(dummy, "WHERE id = %I64d", LastSqlLogID);
#else
  sprintf(dummy, "WHERE id = %lld", LastSqlLogID);
#endif
  sql += wxString::FromUTF8(dummy);
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("UpdateLog: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
}

void MyFrame::UpdateLog(wxString & error_msg)
{
//
// updating sql_statements_log: failure
//
  char dummy[64];
  char *clean;
  int ret;
  char *errMsg = NULL;
  wxString sql = wxT("UPDATE sql_statements_log SET ");
  sql += wxT("time_end = strftime('%Y-%m-%dT%H:%M:%fZ', 'now'), ");
  sql += wxT("success = 0, error_cause = '");
  clean = gaiaSingleQuotedSql(error_msg.ToUTF8());
  sql += wxString::FromUTF8(clean);
  free(clean);
#if defined(_WIN32) || defined(__MINGW32__)
  /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
  sprintf(dummy, "' WHERE id = %I64d", LastSqlLogID);
#else
  sprintf(dummy, "' WHERE id = %lld", LastSqlLogID);
#endif
  sql += wxString::FromUTF8(dummy);
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("UpdateLog: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
}

void MyFrame::UpdateAbortedLog()
{
//
// updating sql_statements_log: success
//
  char dummy[64];
  int ret;
  char *errMsg = NULL;
  wxString sql = wxT("UPDATE sql_statements_log SET ");
  sql += wxT("time_end = strftime('%Y-%m-%dT%H:%M:%fZ', 'now'), ");
  sql += wxT("success = 0, error_cause = 'aborted by the user' ");
#if defined(_WIN32) || defined(__MINGW32__)
  /* CAVEAT - M$ runtime doesn't supports %lld for 64 bits */
  sprintf(dummy, "WHERE id = %I64d", LastSqlLogID);
#else
  sprintf(dummy, "WHERE id = %lld", LastSqlLogID);
#endif
  sql += wxString::FromUTF8(dummy);
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("UpdateAbortedLog: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
}

void MyFrame::GetNextAttachedSymbol(wxString & symbol)
{
//
// return an unused DB alias name
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  char sym[16];
  bool already_used = false;
  char x;
  char y;
  char z;
  ret = sqlite3_get_table(GetSqlite(), "PRAGMA database_list", &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (x = 'a'; x < 'z'; x++)
        {
          already_used = false;
          sprintf(sym, "%c", x);
          for (i = 1; i <= rows; i++)
            {
              if (strcasecmp(sym, results[(i * columns) + 1]) == 0)
                already_used = true;
            }
          if (already_used == false)
            {
              symbol = wxString::FromUTF8(sym);
              goto stop;
            }
        }
      for (y = 'a'; y < 'z'; y++)
        {
          for (x = 'a'; x < 'z'; x++)
            {
              already_used = false;
              sprintf(sym, "%c%c", y, x);
              for (i = 1; i <= rows; i++)
                {
                  if (strcasecmp(sym, results[(i * columns) + 1]) == 0)
                    already_used = true;
                }
              if (already_used == false)
                {
                  symbol = wxString::FromUTF8(sym);
                  goto stop;
                }
            }
        }
      for (z = 'a'; z < 'z'; z++)
        {
          for (y = 'a'; y < 'z'; y++)
            {
              for (x = 'a'; x < 'z'; x++)
                {
                  already_used = false;
                  sprintf(sym, "%c%c%c", z, y, x);
                  for (i = 1; i <= rows; i++)
                    {
                      if (strcasecmp(sym, results[(i * columns) + 1]) == 0)
                        already_used = true;
                    }
                  if (already_used == false)
                    {
                      symbol = wxString::FromUTF8(sym);
                      goto stop;
                    }
                }
            }
        }
    }
stop:
  sqlite3_free_table(results);
}

bool MyFrame::DoAttachDatabase(wxString & path)
{
//
// attempting to attach another DB
//
  int ret;
  char *errMsg = NULL;
  wxString symbol;
  wxString sql = wxT("ATTACH DATABASE \"");
  sql += path;
  sql += wxT("\" AS ");
  GetNextAttachedSymbol(symbol);
  sql += symbol;
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("AttachDatabase: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  return true;
}

bool MyFrame::IsDotCommandDumpShp(const char *stmt, char *table,
                                  char *column, char *path,
                                  char *charset, char *type)
{
//
// attempting to parse a .dumpshp command [SQL script]
//
  wxString cmd = wxString::FromUTF8(stmt);
  wxString extPath;
  wxString extTable;
  wxString extColumn;
  wxString extCharset;
  wxString extType;
  int count = 0;
  wxStringTokenizer tok(cmd);
  while (tok.HasMoreTokens())
    {
      wxString token = tok.GetNextToken();
      switch (count)
        {
          case 0:
            extTable = token;
            break;
          case 1:
            extColumn = token;
            break;
          case 2:
            extPath = token;
            break;
          case 3:
            extCharset = token;
            break;
          case 4:
            extType = token;
            break;
        };
      count++;
    }
  if (count == 4 || count == 5)
    {
      strcpy(path, extPath.ToUTF8());
      strcpy(table, extTable.ToUTF8());
      strcpy(column, extColumn.ToUTF8());
      strcpy(charset, extCharset.ToUTF8());
      strcpy(type, extType.ToUTF8());
      return true;
    }
  return false;
}

void MyFrame::GetHelp(wxString & html)
{
//
// return the HTML Help
//
  html =
    wxT("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">");
  html += wxT("<html>");
  html += wxT("<head>");
  html +=
    wxT
    ("<meta content=\"text/html; charset=UTF-8\" http-equiv=\"content-type\">");
  html += wxT("<title>SQLite + SpatiaLite quick Help</title>");
  html += wxT("</head>");
  html += wxT("<body bgcolor=\"#e8e8e8\">");
  html += wxT("<h1><a name=\"index\">SQLite + SpatiaLite quick Help</a></h1>");
  html += wxT("<table cellspacing=\"2\" cellpadding=\"2\">");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\">Index of contents</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">1.</td><td bgcolor=\"#f0fff0\"><a href=\"#c1\">SQLite SQL syntax</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">2.</td><td bgcolor=\"#f0fff0\"><a href=\"#c2\">SQLite SQL functions</a><ul>");
  html += wxT("<li><a href=\"#c21\">ordinary functions</a></li>");
  html += wxT("<li><a href=\"#c22\">aggregate functions</a></li>");
  html += wxT("</ul></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">3.</td><td bgcolor=\"#f0fff0\"><a href=\"#c3\">SpatiaLite SQL Spatial functions</a><ul>");
  html +=
    wxT
    ("<li><a href=\"#version\">version info [and build options] functions</a></li>");
  html += wxT("<li><a href=\"#generic\">generic functions</a></li>");
  html += wxT("<li><a href=\"#math\">math functions</a></li>");
  html +=
    wxT
    ("<li><a href=\"#length_cvt\">length/distance unit-conversion functions</a></li>");
  html +=
    wxT("<li><a href=\"#blob\">utility functions for BLOB objects</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c30\">utility functions [non-standard] for geometric objects</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c31\">functions for constructing a geometric object given its Well-known Text Representation</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c32\">functions for constructing a geometric object given its Well-known Binary Representation</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c33\">functions for obtaining the Well-known Text / Well-known Binary Representation of a geometric object</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c33misc\">functions supporting exotic geometric formats</a></li>");
  html += wxT("<li><a href=\"#c34\">functions on type Geometry</a></li>");
  html +=
    wxT
    ("<li><a href=\"#repair\">Functions attempting to repair malformed Geometries</a></li>");
  html +=
    wxT("<li><a href=\"#compress\">Geometry-compression functions</a></li>");
  html += wxT("<li><a href=\"#cast\">Geometry-type casting functions</a></li>");
  html +=
    wxT
    ("<li><a href=\"#dims-cast\">Space-dimensions casting functions</a></li>");
  html += wxT("<li><a href=\"#c35\">functions on type Point</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c361\">functions on type Curve [Linestring or Ring]</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c36\">functions on type Surface [Polygon or Ring]</a></li>");
  html += wxT("<li><a href=\"#c37\">functions on type Polygon</a></li>");
  html += wxT("<li><a href=\"#c38\">functions on type GeomCollection</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c39\">functions testing approximative spatial relationships via MBRs</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c40\">functions testing spatial relationships</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c41\">functions implementing spatial operators</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c42\">functions for coordinate transformations</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c43\">functions for Spatial-MetaData and Spatial-Index handling</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c43style\">functions supporting SLD/SE Styled Layers</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c43isometa\">functions supporting ISO Metadata</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c43fdo\">functions implementing FDO/OGR compatibily</a></li>");
  html +=
    wxT("<li><a href=\"#c44\">functions for MbrCache-based queries</a></li>");
  html +=
    wxT
    ("<li><a href=\"#c45\">functions for R*Tree-based queries (Geometry Callbacks)</a></li>");
  html +=
    wxT("<li><a href=\"#xmlBlob\">SQL functions supporting XmlBLOB</a></li>");
  html += wxT("</ul></td></tr>");
  html += wxT("</table>");
  html += wxT("<h3><a name=\"c1\">SQLite SQL syntax</a></h3>");
  html += wxT("<table cellspacing=\"4\" cellpadding=\"2\"width=\"100%\">");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ALTER TABLE</td><td bgcolor=\"#f0fff0\">sql-statement ::= ALTER TABLE [database-name .] table-name alteration<br>");
  html += wxT("alteration ::= RENAME TO new-table-name<br>");
  html += wxT("alteration ::= ADD [COLUMN] column-def<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ANALYZE</td><td bgcolor=\"#f0fff0\">sql-statement ::= ANALYZE<br>");
  html += wxT("sql-statement ::= ANALYZE database-name<br>");
  html +=
    wxT("sql-statement ::= ANALYZE [database-name .] table-name<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ATTACH DATABASE</td><td bgcolor=\"#f0fff0\">sql-statement ::= ATTACH [DATABASE] database-filename AS database-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BEGIN TRANSACTION</td><td bgcolor=\"#f0fff0\">sql-statement ::= BEGIN [ DEFERRED | IMMEDIATE | EXCLUSIVE ] [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= END [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= COMMIT [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= ROLLBACK [TRANSACTION [name]]<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">COMMIT TRANSACTION</td><td bgcolor=\"#f0fff0\">sql-statement ::= BEGIN [ DEFERRED | IMMEDIATE | EXCLUSIVE ] [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= END [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= COMMIT [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= ROLLBACK [TRANSACTION [name]]<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CREATE INDEX</td><td bgcolor=\"#f0fff0\">sql-statement ::= CREATE [UNIQUE] INDEX [IF NOT EXISTS] [database-name .] index-name<br>");
  html += wxT("ON table-name ( column-name [, column-name]* )<br>");
  html +=
    wxT
    ("column-name ::= name [ COLLATE collation-name] [ ASC | DESC ]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CREATE TABLE</td><td bgcolor=\"#f0fff0\">sql-command ::= CREATE [TEMP | TEMPORARY] TABLE [IF NOT EXISTS] [database-name .] table-name (<br>");
  html += wxT("column-def [, column-def]*<br>");
  html += wxT("[, constraint]*<br>");
  html += wxT(")<br>");
  html +=
    wxT
    ("sql-command ::= CREATE [TEMP | TEMPORARY] TABLE [database-name.] table-name AS select-statement<br>");
  html +=
    wxT
    ("column-def ::= name [type] [[CONSTRAINT name] column-constraint]*<br>");
  html += wxT("type ::= typename |<br>");
  html += wxT("typename ( number ) |<br>");
  html += wxT("typename ( number , number )<br>");
  html += wxT("column-constraint ::= NOT NULL [ conflict-clause ] |<br>");
  html +=
    wxT("PRIMARY KEY [sort-order] [ conflict-clause ] [AUTOINCREMENT] |<br>");
  html += wxT("UNIQUE [ conflict-clause ] |<br>");
  html += wxT("CHECK ( expr ) |<br>");
  html += wxT("DEFAULT value |<br>");
  html += wxT("COLLATE collation-name<br>");
  html +=
    wxT("constraint ::= PRIMARY KEY ( column-list ) [ conflict-clause ] |<br>");
  html += wxT("UNIQUE ( column-list ) [ conflict-clause ] |<br>");
  html += wxT("CHECK ( expr )<br>");
  html += wxT("conflict-clause ::= ON CONFLICT conflict-algorithm</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CREATE TRIGGER</td><td bgcolor=\"#f0fff0\">sql-statement ::= CREATE [TEMP | TEMPORARY] TRIGGER [IF NOT EXISTS] trigger-name [ BEFORE | AFTER ]<br>");
  html += wxT("database-event ON [database-name .] table-name<br>");
  html += wxT("trigger-action<br>");
  html +=
    wxT
    ("sql-statement ::= CREATE [TEMP | TEMPORARY] TRIGGER [IF NOT EXISTS] trigger-name INSTEAD OF<br>");
  html += wxT("database-event ON [database-name .] view-name<br>");
  html += wxT("trigger-action<br>");
  html += wxT("database-event ::= DELETE |<br>");
  html += wxT("INSERT |<br>");
  html += wxT("UPDATE |<br>");
  html += wxT("UPDATE OF column-list<br>");
  html += wxT("trigger-action ::= [ FOR EACH ROW ] [ WHEN expression ]<br>");
  html += wxT("BEGIN<br>");
  html += wxT("trigger-step ; [ trigger-step ; ]*<br>");
  html += wxT("END<br>");
  html += wxT("trigger-step ::= update-statement | insert-statement |<br>");
  html += wxT("delete-statement | select-statement</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CREATE VIEW</td><td bgcolor=\"#f0fff0\">sql-command ::= CREATE [TEMP | TEMPORARY] VIEW [IF NOT EXISTS] [database-name.] view-name AS select-statement</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CREATE VIRTUAL TABLE</td><td bgcolor=\"#f0fff0\">sql-command ::= CREATE VIRTUAL TABLE [database-name .] table-name USING module-name [( arguments )]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DELETE</td><td bgcolor=\"#f0fff0\">sql-statement ::= DELETE FROM [database-name .] table-name [WHERE expr]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DETACH DATABASE</td><td bgcolor=\"#f0fff0\">sql-command ::= DETACH [DATABASE] database-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DROP INDEX</td><td bgcolor=\"#f0fff0\">sql-command ::= DROP INDEX [IF EXISTS] [database-name .] index-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DROP TABLE</td><td bgcolor=\"#f0fff0\">sql-command ::= DROP TABLE [IF EXISTS] [database-name.] table-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DROP TRIGGER</td><td bgcolor=\"#f0fff0\">sql-statement ::= DROP TRIGGER [IF EXISTS] [database-name .] trigger-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DROP VIEW</td><td bgcolor=\"#f0fff0\">sql-command ::= DROP VIEW [IF EXISTS] view-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">END TRANSACTION</td><td bgcolor=\"#f0fff0\">sql-statement ::= BEGIN [ DEFERRED | IMMEDIATE | EXCLUSIVE ] [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= END [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= COMMIT [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= ROLLBACK [TRANSACTION [name]]<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">EXPLAIN</td><td bgcolor=\"#f0fff0\">sql-statement ::= EXPLAIN sql-statement</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">INSERT</td><td bgcolor=\"#f0fff0\">sql-statement ::= INSERT [OR conflict-algorithm] INTO [database-name .] table-name [(column-list)] VALUES(value-list) |<br>");
  html +=
    wxT
    ("INSERT [OR conflict-algorithm] INTO [database-name .] table-name [(column-list)] select-statement</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ON CONFLICT clause</td><td bgcolor=\"#f0fff0\">conflict-clause ::= ON CONFLICT conflict-algorithm<br>");
  html +=
    wxT
    ("conflict-algorithm ::= ROLLBACK | ABORT | FAIL | IGNORE | REPLACE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PRAGMA</td><td bgcolor=\"#f0fff0\">sql-statement ::= PRAGMA name [= value] |<br>");
  html += wxT("PRAGMA function(arg)<hr>");
  html += wxT("PRAGMA auto_vacuum;<br>");
  html +=
    wxT("PRAGMA auto_vacuum = 0 | none | 1 | full | 2 | incremental;<hr>");
  html += wxT("PRAGMA cache_size;<br>");
  html += wxT("PRAGMA cache_size = Number-of-pages;<hr>");
  html += wxT("PRAGMA case_sensitive_like;<br>");
  html += wxT("PRAGMA case_sensitive_like = 0 | 1;<hr>");
  html += wxT("PRAGMA count_changes;<br>");
  html += wxT("PRAGMA count_changes = 0 | 1;<hr>");
  html += wxT("PRAGMA default_cache_size;<br>");
  html += wxT("PRAGMA default_cache_size = Number-of-pages;<hr>");
  html += wxT("PRAGMA empty_result_callbacks;<br>");
  html += wxT("PRAGMA empty_result_callbacks = 0 | 1;<hr>");
  html += wxT("PRAGMA encoding;<br>");
  html += wxT("PRAGMA encoding = \"UTF-8\";<br>");
  html += wxT("PRAGMA encoding = \"UTF-16\";<br>");
  html += wxT("PRAGMA encoding = \"UTF-16le\";<br>");
  html += wxT("PRAGMA encoding = \"UTF-16be\";<hr>");
  html += wxT("PRAGMA foreign_keys;<br>");
  html += wxT("PRAGMA foreign_keys = 0 | 1;<hr>");
  html += wxT("PRAGMA full_column_names;<br>");
  html += wxT("PRAGMA full_column_names = 0 | 1;<hr>");
  html += wxT("PRAGMA fullfsync;<br>");
  html += wxT("PRAGMA fullfsync = 0 | 1;<hr>");
  html += wxT("PRAGMA journal_mode;<br>");
  html += wxT("PRAGMA database.journal_mode;<br>");
  html +=
    wxT("PRAGMA journal_mode = DELETE | TRUNCATE | PERSIST | MEMORY | OFF<br>");
  html +=
    wxT
    ("PRAGMA database.journal_mode = DELETE | TRUNCATE | PERSIST | MEMORY | OFF<hr>");
  html += wxT("PRAGMA journal_size_limit;<br>");
  html += wxT("PRAGMA journal_size_limit = N<hr>");
  html += wxT("PRAGMA legacy_file_format;<br>");
  html += wxT("PRAGMA legacy_file_format = 0 | 1<hr>");
  html += wxT("PRAGMA locking_mode;<br>");
  html += wxT("PRAGMA locking_mode = NORMAL | EXCLUSIVE<hr>");
  html += wxT("PRAGMA page_size;<br>");
  html += wxT("PRAGMA page_size = bytes;<hr>");
  html += wxT("PRAGMA max_page_count;<br>");
  html += wxT("PRAGMA max_page_count = N;<hr>");
  html += wxT("PRAGMA read_uncommitted;<br>");
  html += wxT("PRAGMA read_uncommitted = 0 | 1;<hr>");
  html += wxT("PRAGMA recursive_triggers;<br>");
  html += wxT("PRAGMA recursive_triggers = 0 | 1;<hr>");
  html += wxT("PRAGMA reverse_unordered_selects;<br>");
  html += wxT("PRAGMA reverse_unordered_selects = 0 | 1;<hr>");
  html += wxT("PRAGMA short_column_names;<br>");
  html += wxT("PRAGMA short_column_names = 0 | 1;<hr>");
  html += wxT("PRAGMA synchronous;<br>");
  html += wxT("PRAGMA synchronous = FULL; (2)<br>");
  html += wxT("PRAGMA synchronous = NORMAL; (1)<br>");
  html += wxT("PRAGMA synchronous = OFF; (0)<hr>");
  html += wxT("PRAGMA temp_store;<br>");
  html += wxT("PRAGMA temp_store = DEFAULT; (0)<br>");
  html += wxT("PRAGMA temp_store = FILE; (1)<br>");
  html += wxT("PRAGMA temp_store = MEMORY; (2)<hr>");
  html += wxT("PRAGMA temp_store_directory;<br>");
  html += wxT("PRAGMA temp_store_directory = 'directory-name';<hr>");
  html += wxT("PRAGMA database_list;<hr>");
  html += wxT("PRAGMA foreign_key_list(table-name);<hr>");
  html += wxT("PRAGMA [database].freelist_count;<hr>");
  html += wxT("PRAGMA index_info(index-name);<hr>");
  html += wxT("PRAGMA index_list(table-name);<hr>");
  html += wxT("PRAGMA table_info(table-name);<hr>");
  html += wxT("PRAGMA [database.]schema_version;<br>");
  html += wxT("PRAGMA [database.]schema_version = integer ;<br>");
  html += wxT("PRAGMA [database.]user_version;<br>");
  html += wxT("PRAGMA [database.]user_version = integer ;<hr>");
  html += wxT("PRAGMA integrity_check;<br>");
  html += wxT("PRAGMA integrity_check(integer)<hr>");
  html += wxT("PRAGMA quick_check;<br>");
  html += wxT("PRAGMA quick_check(integer)<hr>");
  html += wxT("PRAGMA parser_trace = ON; (1)<br>");
  html += wxT("PRAGMA parser_trace = OFF; (0)<hr>");
  html += wxT("PRAGMA vdbe_trace = ON; (1)<br>");
  html += wxT("PRAGMA vdbe_trace = OFF; (0)<hr>");
  html += wxT("PRAGMA vdbe_listing = ON; (1)<br>");
  html += wxT("PRAGMA vdbe_listing = OFF; (0)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">REINDEX</td><td bgcolor=\"#f0fff0\">sql-statement ::= REINDEX collation name<br>");
  html +=
    wxT
    ("sql-statement ::= REINDEX [database-name .] table/index-name</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">REPLACE</td><td bgcolor=\"#f0fff0\">sql-statement ::= REPLACE INTO [database-name .] table-name [( column-list )] VALUES ( value-list ) |<br>");
  html +=
    wxT
    ("REPLACE INTO [database-name .] table-name [( column-list )] select-statement</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ROLLBACK TRANSACTION</td><td bgcolor=\"#f0fff0\">sql-statement ::= BEGIN [ DEFERRED | IMMEDIATE | EXCLUSIVE ] [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= END [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= COMMIT [TRANSACTION [name]]<br>");
  html += wxT("sql-statement ::= ROLLBACK [TRANSACTION [name]]<br></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SELECT</td><td bgcolor=\"#f0fff0\">sql-statement ::= SELECT [ALL | DISTINCT] result [FROM table-list]<br>");
  html += wxT("[WHERE expr]<br>");
  html += wxT("[GROUP BY expr-list]<br>");
  html += wxT("[HAVING expr]<br>");
  html += wxT("[compound-op select]*<br>");
  html += wxT("[ORDER BY sort-expr-list]<br>");
  html += wxT("[LIMIT integer [( OFFSET | , ) integer]]<br>");
  html += wxT("result ::= result-column [, result-column]*<br>");
  html +=
    wxT("result-column ::= * | table-name . * | expr [ [AS] string ]<br>");
  html += wxT("table-list ::= table [join-op table join-args]*<br>");
  html += wxT("table ::= table-name [AS alias] |<br>");
  html += wxT("( select ) [AS alias]<br>");
  html +=
    wxT
    ("join-op ::= , | [NATURAL] [LEFT | RIGHT | FULL] [OUTER | INNER | CROSS] JOIN<br>");
  html += wxT("join-args ::= [ON expr] [USING ( id-list )]<br>");
  html +=
    wxT("sort-expr-list ::= expr [sort-order] [, expr [sort-order]]*<br>");
  html += wxT("sort-order ::= [ COLLATE collation-name ] [ ASC | DESC ]<br>");
  html +=
    wxT("compound_op ::= UNION | UNION ALL | INTERSECT | EXCEPT</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">UPDATE</td><td bgcolor=\"#f0fff0\">sql-statement ::= UPDATE [ OR conflict-algorithm ] [database-name .] table-name<br>");
  html += wxT("SET assignment [, assignment]*<br>");
  html += wxT("[WHERE expr]<br>");
  html += wxT("assignment ::= column-name = expr</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">VACUUM</td><td bgcolor=\"#f0fff0\">sql-statement ::= VACUUM</td></tr>");
  html += wxT("</table>");
  html += wxT("<a href=\"#index\">back to index</a>");
  html +=
    wxT
    ("<h3><a align=\"center\" bgcolor=\"#e0ffe0\" name=\"c2\">SQLite SQL functions</a></h3>");
  html += wxT("<table cellspacing=\"4\" cellpadding=\"2\" width=\"100%\">");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c21\">ordinary functions</a><br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">abs(X)</td><td bgcolor=\"#f0fff0\">Return the absolute value of the numeric argument X. ");
  html +=
    wxT
    ("Return NULL if X is NULL. Return 0.0 if X is not a numeric value.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">changes()</td><td bgcolor=\"#f0fff0\">Returns the number of database rows that were changed or inserted or deleted ");
  html +=
    wxT
    ("by the most recently completed INSERT, DELETE, or UPDATE statement, exclusive of statements in lower-level triggers.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">char(X1,X2,...,XN)</td><td bgcolor=\"#f0fff0\">The char(X1,X2,...,XN) function returns a string ");
  html +=
    wxT
    ("composed of characters having the unicode code point values of integers X1 through XN, respectively.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">coalesce(X,Y,...)</td><td bgcolor=\"#f0fff0\">Return a copy of the first non-NULL argument. ");
  html +=
    wxT
    ("If all arguments are NULL then NULL is returned. There must be at least 2 arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">glob(X,Y)</td><td bgcolor=\"#f0fff0\">This function is used to implement the \"X GLOB Y\" syntax of SQLite. ");
  html +=
    wxT
    ("The sqlite3_create_function() interface can be used to override this function and thereby change the operation of the GLOB operator.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ifnull(X,Y)</td><td bgcolor=\"#f0fff0\">Return a copy of the first non-NULL argument. ");
  html +=
    wxT
    ("If both arguments are NULL then NULL is returned. This behaves the same as coalesce().</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">instr(X,Y)</td><td bgcolor=\"#f0fff0\">The instr(X,Y) function finds the first occurrence of string Y within string X and returns the ");
  html +=
    wxT
    ("number of prior characters plus 1, or 0 if Y is nowhere found within X. Or, if X and Y are both BLOBs, then instr(X,Y) returns one more than ");
  html +=
    wxT
    ("the number bytes prior to the first occurrence of Y, or 0 if Y does not occur anywhere within X. If both arguments X and Y to instr(X,Y) ");
  html +=
    wxT
    ("are non-NULL and are not BLOBs then both are interpreted as strings. If either X or Y are NULL in instr(X,Y) then the result is NULL. </td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">hex(X)</td><td bgcolor=\"#f0fff0\">The argument is interpreted as a BLOB. ");
  html +=
    wxT
    ("The result is a hexadecimal rendering of the content of that blob.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">last_insert_rowid()</td><td bgcolor=\"#f0fff0\">Return the ROWID of the last row insert from this connection to the database. ");
  html +=
    wxT
    ("This is the same value that would be returned from the sqlite3_last_insert_rowid() API function.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">length(X)</td><td bgcolor=\"#f0fff0\">Return the string length of X in characters. ");
  html +=
    wxT
    ("If SQLite is configured to support UTF-8, then the number of UTF-8 characters is returned, not the number of bytes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">like(X,Y)<br>like(X,Y,Z)</td><td bgcolor=\"#f0fff0\">This function is used to implement the \"X LIKE Y [ESCAPE Z]\" syntax of SQL.");
  html +=
    wxT
    ("If the optional ESCAPE clause is present, then the user-function is invoked with three arguments. ");
  html += wxT("Otherwise, it is invoked with two arguments only. ");
  html +=
    wxT
    ("The sqlite3_create_function() interface can be used to override this function and thereby change the operation of the LIKE operator. ");
  html +=
    wxT
    ("When doing this, it may be important to override both the two and three argument versions of the like() function. ");
  html +=
    wxT
    ("Otherwise, different code may be called to implement the LIKE operator depending on whether or not an ESCAPE clause was specified.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">load_extension(X)</br>load_extension(X,Y)</td><td bgcolor=\"#f0fff0\">Load SQLite extensions ");
  html +=
    wxT("out of the shared library file named X using the entry point Y. ");
  html +=
    wxT
    ("The result is a NULL. If Y is omitted then the default entry point of sqlite3_extension_init is used. ");
  html +=
    wxT
    ("This function raises an exception if the extension fails to load or initialize correctly.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">lower(X)</td><td bgcolor=\"#f0fff0\">Return a copy of string X will all ASCII characters converted to lower case. ");
  html +=
    wxT
    ("The C library tolower() routine is used for the conversion, which means that this function might not work correctly on non-ASCII UTF-8 characters.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ltrim(X)<br>ltrim(X,Y)</td><td bgcolor=\"#f0fff0\">Return a string formed by removing any and all characters ");
  html +=
    wxT
    ("that appear in Y from the left side of X. If the Y argument is omitted, spaces are removed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">max(X,Y,...)</td><td bgcolor=\"#f0fff0\">Return the argument with the maximum value. ");
  html += wxT("Arguments may be strings in addition to numbers. ");
  html += wxT("The maximum value is determined by the usual sort order. ");
  html +=
    wxT
    ("Note that max() is a simple function when it has 2 or more arguments but converts to an aggregate function if given only a single argument.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">min(X,Y,...)</td><td bgcolor=\"#f0fff0\">Return the argument with the minimum value. ");
  html += wxT("Arguments may be strings in addition to numbers. ");
  html += wxT("The minimum value is determined by the usual sort order. ");
  html +=
    wxT
    ("Note that min() is a simple function when it has 2 or more arguments but converts to an aggregate function if given only a single argument.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">nullif(X,Y)</td><td bgcolor=\"#f0fff0\">Return the first argument if the arguments are different, otherwise return NULL.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">quote(X)</td><td bgcolor=\"#f0fff0\">This routine return a string which is the value of its argument suitable for inclusion ");
  html +=
    wxT
    ("into another SQL statement. Strings are surrounded by single-quotes with escapes on interior quotes as needed. ");
  html +=
    wxT
    ("BLOBs are encoded as hexadecimal literals. The implementation of VACUUM uses this function. ");
  html +=
    wxT
    ("The function is also useful when writing triggers to implement undo/redo functionality.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">random()</td><td bgcolor=\"#f0fff0\">Return a pseudo-random integer between -9223372036854775808 and +9223372036854775807.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">randomblob(N)</td><td bgcolor=\"#f0fff0\">Return an N-byte blob containing pseudo-random bytes. N should be a postive integer.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">replace(X,Y,Z)</td><td bgcolor=\"#f0fff0\">Return a string formed by substituting string Z for every occurrance of string Y in string X. ");
  html +=
    wxT("The BINARY collating sequence is used for comparisons.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">round(X)<br>round(X,Y)</td><td bgcolor=\"#f0fff0\">Round off the number X to Y digits to the right of the decimal point. ");
  html += wxT("If the Y argument is omitted, 0 is assumed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">rtrim(X)<br>rtrim(X,Y)</td><td bgcolor=\"#f0fff0\">Return a string formed by removing any and all characters ");
  html +=
    wxT
    ("that appear in Y from the right side of X. If the Y argument is omitted, spaces are removed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">sqlite_version(X)</td><td bgcolor=\"#f0fff0\">Return the version string for the SQLite library that is running. Example: \"3.5.9\"</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">substr(X,Y,Z)<br>substr(X,Y)</td><td bgcolor=\"#f0fff0\">Return a substring of input string X that begins with the Y-th character ");
  html +=
    wxT
    ("and which is Z characters long. If Z is omitted then all character through the end of the string are returned. ");
  html += wxT("The left-most character of X is number 1. ");
  html +=
    wxT
    ("If Y is negative the the first character of the substring is found by counting from the right rather than the left. ");
  html +=
    wxT
    ("If X is string then characters indices refer to actual UTF-8 characters. If X is a BLOB then the indices refer to bytes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">trim(X)<br>trim(X,Y)</td><td bgcolor=\"#f0fff0\">Return a string formed by removing any and all characters that appear in Y from both ends of X. ");
  html += wxT("If the Y argument is omitted, spaces are removed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">typeof(X)</td><td bgcolor=\"#f0fff0\">Return the type of the expression X. ");
  html +=
    wxT
    ("The only return values are \"null\", \"integer\", \"real\", \"text\", and \"blob\".</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">unicode(X)</td><td bgcolor=\"#f0fff0\">The unicode(X) function returns the numeric unicode code point corresponding to the first ");
  html +=
    wxT
    ("character of the string X. If the argument to unicode(X) is not a string then the result is undefined. </td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">upper(X)</td><td bgcolor=\"#f0fff0\">Return a copy of input string X converted to all upper-case letters. ");
  html +=
    wxT
    ("The implementation of this function uses the C library routine toupper() which means it may not work correctly on non-ASCII UTF-8 strings.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">zeroblob(N)</td><td bgcolor=\"#f0fff0\">Return a BLOB consisting of N bytes of 0x00. ");
  html += wxT("SQLite manages these zeroblobs very efficiently. ");
  html +=
    wxT
    ("Zeroblobs can be used to reserve space for a BLOB that is later written using incremental BLOB I/O.</td></tr> ");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c22\">aggregate functions</a><br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">avg(X)</td><td bgcolor=\"#f0fff0\">Return the average value of all non-NULL X within a group. ");
  html +=
    wxT
    ("String and BLOB values that do not look like numbers are interpreted as 0. ");
  html +=
    wxT
    ("The result of avg() is always a floating point value even if all inputs are integers.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">count(X)<br>count(*)</td><td bgcolor=\"#f0fff0\">The first form return a count of the number of times that X is not NULL in a group. ");
  html +=
    wxT
    ("The second form (with no argument) return the total number of rows in the group.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">group_concat(X)<br>group_concat(X,Y)</td><td bgcolor=\"#f0fff0\">The result is a string which is the concatenation of all non-NULL values of X. ");
  html +=
    wxT
    ("If parameter Y is the separator between instances of X. A comma (\",\") is used as the separator if Y is omitted.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">max(X)</td><td bgcolor=\"#f0fff0\">Return the maximum value of all values in the group. ");
  html +=
    wxT("The usual sort order is used to determine the maximum.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">min(X)</td><td bgcolor=\"#f0fff0\">Return the minimum non-NULL value of all values in the group. ");
  html +=
    wxT
    ("The usual sort order is used to determine the minimum. NULL is only returned if all values in the group are NULL.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">sum(X)<br>total(X)</td><td bgcolor=\"#f0fff0\">Return the numeric sum of all non-NULL values in the group. ");
  html +=
    wxT
    ("If there are no non-NULL input rows then sum() return NULL but total() return 0.0. ");
  html +=
    wxT
    ("NULL is not normally a helpful result for the sum of no rows but the SQL standard requires it and most other SQL ");
  html +=
    wxT
    ("database engines implement sum() that way so SQLite does it in the same way in order to be compatible. ");
  html +=
    wxT
    ("The non-standard total() function is provided as a convenient way to work around this design problem in the SQL language.<br>");
  html +=
    wxT
    ("The result of total() is always a floating point value. The result of sum() is an integer value if all non-NULL inputs are integers. ");
  html +=
    wxT
    ("If any input to sum() is neither an integer or a NULL then sum() return a floating point value which might be an approximation to the true sum.<br>");
  html +=
    wxT
    ("Sum() will throw an \"integer overflow\" exception if all inputs are integers or NULL and an integer overflow occurs at any point during the computation.");
  html += wxT("Total() never throws an exception.</td></tr> ");
  html += wxT("</table>");
  html += wxT("<a href=\"#index\">back to index</a>");
  html += wxT("<h3><a name=\"c3\">SpatiaLite SQL Spatial functions</a></h3>");
  html += wxT("<table cellspacing=\"4\" cellpadding=\"2\" width=\"100%\">");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"version\">SQL version info [and build options] functions</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">spatialite_version( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the current SpatiaLite version</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">proj4_version( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the current PROJ.4 version, or NULL if PROJ.4 is currently unsupported</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">geos_version( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the current GEOS version, or NULL if GEOS is currently unsupported</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">lwgeom_version( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the current LWGEOM version, or NULL if LWGEOM is currently unsupported</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">libxml2_version( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the current LibXML2 version, or NULL if LibXML2 is currently unsupported</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasIconv( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling ICONV</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasMathSQL( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling MATHSQL</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">HasGeoCallbacks( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling GEOCALLBACKS</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasProj( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling PROJ</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasGeos( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling GEOS</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">HasGeosAdvanced( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling GEOSADVANCED</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">HasGeosTrunk( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling GEOSTRUNK</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasLwGeom( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling LWGEOM</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasLibXML2( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling LibXML2</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasEpsg( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling EPSG</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">HasFreeXL( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">TRUE if the underlaying library was built enabling FreeXL</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"generic\">generic SQL functions</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToInteger( value Generic ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the intput value possibly casted to the Integer data-type: NULL if no conversion is possible.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToDouble( value Generic ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the intput value possibly casted to the Double data-type: NULL if no conversion is possible.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToText( value Generic ) : Text<hr>CastToText( value Generic ) : Text</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the intput value possibly casted to the Text data-type: NULL if no conversion is possible.<br>");
  html +=
    wxT
    ("If an optional argument \"zero_pad\" is passed and the input value is of the Integer or Double type, then the returned string will be padded using as much trailing ZEROs so to ensure the required length.</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">CastToBlob( value Generic ) : Blob<hr>");
  html += wxT("(CastToBlob (value Generic , hex_input Boolean ) : Blob</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the intput value possibly casted to the Blob data-type: NULL if no conversion is possible.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ForceAsNull( val1 Generic , val2 Generic ) : Generic</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">if \"val1\" and \"val2\" are equal (and exactly of the same data-type) NULL will be returned; ");
  html +=
    wxT
    ("otherwise \"val1\" will be returned absolutely untouched and still preserving its originale data-type.</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">CreateUUID( void ) : Text</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a Version 4 (random) UUID (<b>Universally unique identifier</b>).</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">MD5Checksum( BLOB | TEXT ) : Text</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the MD5 checksum corresponding to the input value.<br>Will return NULL for non-BLOB input.</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">MD5TotalChecksum( BLOB | TEXT ) : Text</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a cumulative MD5 checksum.<br><b><u>Aggregate function</u></b>.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"math\">SQL math functions</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Abs( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the absolute value of <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Acos( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the arc cosine of <i>x</i>, that is, the value whose cosine is <i>x</i><br>");
  html += wxT("returns NULL if <i>x</i> is not in the range -1 to 1</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Asin( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the arc sine of <i>x</i>, that is, the value whose sine is <i>x</i><br>");
  html += wxT("returns NULL if <i>x</i> is not in the range -1 to 1</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Atan( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the arc tangent of <i>x</i>, that is, the value whose tangent is <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Ceil( x Double precision ) : Double precision<hr>");
  html += wxT("Ceiling( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the smallest integer value not less than <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Cos( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the cosine of <i>x</i>, where <i>x</i> is given in <u>radians</u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Cot( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the cotangent of <i>x</i>, where <i>x</i> is given in <u>radians</u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Degrees( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the argument <i>x</i>, converted from radians to degrees</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Exp( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the value of <b><i>e</i></b> (the base of natural logarithms) raised to the power of <i>x</i><hr>");
  html +=
    wxT
    ("the inverse of this function is <i>Log()</i> (using a single argument only) or <i>Ln()</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Floor( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the largest integer value not greater than <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Ln( x Double precision ) : Double precision<hr>");
  html += wxT("Log( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the natural logarithm of <i>x</i>; that is, the base-<b><i>e</i></b> logarithm of <i>x</i><br>");
  html +=
    wxT
    ("If <i>x</i> is less than or equal to 0, then NULL is returned</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Log( b Double precision, x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the logarithm of <i>x</i> to the base <i>b</i><br>");
  html +=
    wxT
    ("If <i>x</i> is less than or equal to 0, or if <i>b</i> is less than or equal to 1, then NULL is returned<hr>");
  html +=
    wxT
    ("<i>Log(b, x)</i>  is equivalent to <i>Log(x)</i> / <i>Log(b)</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Log2( x Double precision ) : Double precision</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">returns the base-2 logarithm of <i>x</i><hr>");
  html +=
    wxT
    ("<i>Log2(x)</i>  is equivalent to <i>Log(x)</i> / <i>Log(2)</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Log10( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the base-10 logarithm of <i>x</i><hr>");
  html +=
    wxT
    ("<i>Log10(x)</i>  is equivalent to <i>Log(x)</i> / <i>Log(10)</i></td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">PI( void ) : Double precision</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">returns the value of <i>PI</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Pow( x Double precision, y Double precision ) : Double precision<hr>");
  html +=
    wxT
    ("Power( x Double precision, y Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the value of <i>x</i> raised to the power of <i>y</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Radians( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the argument <i>x</i>, converted from degrees to radians</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Round( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the integer value nearest to <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Sign( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the sign of the argument as -1, 0, or 1, ");
  html +=
    wxT
    ("depending on whether <i>x</i> is negative, zero, or positive.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Sin( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the sine of <i>x</i>, where <i>x</i> is given in <u>radians</u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Sqrt( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the square root of a non-negative number <i>x</i></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Stddev_pop( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the population standard deviation of the input values<br>");
  html += wxT("<b><u>aggregate function</u></b></u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Stddev_samp( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the sample standard deviation of the input values<br>");
  html += wxT("<b><u>aggregate function</u></b></u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Tan( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the tangent of <i>x</i>, where <i>x</i> is given in <u>radians</u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Var_pop( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the population variance of the input values (<i>square of the population standard deviation</i>)<br>");
  html += wxT("<b><u>aggregate function</u></b></u></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Var_samp( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the sample variance of the input values (<i>square of the sample standard deviation</i>)<br>");
  html += wxT("<b><u>aggregate function</u></b></u></td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"length_cvt\">SQL length/distance unit-conversion functions</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToKm( x Double precision ) : Double precision<hr>CvtFromKm( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / kilometers</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToDm( x Double precision ) : Double precision<hr>CvtFromDm( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / decimeters</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToCm( x Double precision ) : Double precision<hr>CvtFromCm( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / centimeters</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToMm( x Double precision ) : Double precision<hr>CvtFromMm( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / millimeters</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToKmi( x Double precision ) : Double precision<hr>CvtFromKmi( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">meters / internation nautical miles</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToIn( x Double precision ) : Double precision<hr>CvtFromIn( x Double precision ) : Double precision</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">meters / international inches</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToFt( x Double precision ) : Double precision<hr>CvtFromFt( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / international feet</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToYd( x Double precision ) : Double precision<hr>CvtFromYd( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / international yards</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToMi( x Double precision ) : Double precision<hr>CvtFromMi( x Double precision ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">meters / international statute miles</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToFath( x Double precision ) : Double precision<hr>CvtFromFath( x Double precision ) : Double precision</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">meters / international fathoms</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToCh( x Double precision ) : Double precision<hr>CvtFromCh( x Double precision ) : Double precision</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">meters / international chains</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToLink( x Double precision ) : Double precision<hr>CvtFromLink( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / international links</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToUsIn( x Double precision ) : Double precision<hr>CvtFromUsIn( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / U.S. inches</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToUsFt( x Double precision ) : Double precision<hr>CvtFromUsFt( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / U.S. feet</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToUsYd( x Double precision ) : Double precision<hr>CvtFromUsYd( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / U.S. yards</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToUsMi( x Double precision ) : Double precision<hr>CvtFromUsMi( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / U.S. statute miles</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToUsCh( x Double precision ) : Double precision<hr>CvtFromUsCh( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / U.S. chains</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToIndFt( x Double precision ) : Double precision<hr>CvtFromIndFt( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / indian feet</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToIndYd( x Double precision ) : Double precision<hr>CvtFromIndYd( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / indian yards</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CvtToIndCh( x Double precision ) : Double precision<hr>CvtFromIndCh( x Double precision ) : Double precision</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">meters / indian chains</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"blob\">SQL utility functions for BLOB objects</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsZipBlob( content Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if this BLOB object corresponds to a valid ZIP-compressed file</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsPdfBlob( content Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if this BLOB object corresponds to a valid PDF document</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsGifBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid GIF image</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsPngBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid PNG image</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsTiffBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid TIFF image</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsJpegBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid JPEG image</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsExifBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html += wxT("TRUE if this BLOB object corresponds to a valid EXIF image<br>");
  html +=
    wxT
    ("<u>Please note:</u> any valid EXIF image is a valid JPEG as well</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">IsExifGpsBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid EXIF-GPS image<br>");
  html +=
    wxT
    ("<u>Please note:</u> any valid EXIF-GPS image is a valid EXIF and JPEG as well</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsWebpBlob( image Blob ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this BLOB object corresponds to a valid WebP image</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GetMimeType( payload Blob ) : String</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">The return type is Text.<br>");
  html +=
    wxT
    ("NULL could be returned for an invalid argument or if no valid mime-type is detected.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BlobFromFile( filepath String ) : BLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">If filepath corresponds to some valid pathname, and the corresponding file ");
  html +=
    wxT
    ("can be actually accessed in read mode, then the whole file content will be returned as a BLOB value.<br>");
  html +=
    wxT
    ("Otherwise NULL will be returned.<br><u>Please note:</u> SQLite doesn't support BLOB values bigger than SQLITE_MAX_LENGTH (usually, 1 GB).<hr>");
  html +=
    wxT
    ("<u>Please note</u>: security restrictions apply to this function, which is really supported only when the environment variable \"SPATIALTE_SECURITY=relaxed\" is explicitly set.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BlobToFile( payload BLOB , filepath String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">If payload is of the BLOB-type, and if filepath corresponds to some valid pathname ");
  html +=
    wxT
    ("(accessible in write/create mode), then the corresponding file will be created/overwritten so to contain the payload.<br>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for success, 0 for failure.<hr>");
  html +=
    wxT
    ("<u>Please note</u>: security restrictions apply to this function, which is really supported only when the environment variable \"SPATIALTE_SECURITY=relaxed\" is explicitly set.</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">CountUnsafeTriggers( ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">This SQL function checks if the currently connected DB does contain any potentially malicious Trigger; ");
  html +=
    wxT
    ("carefully checking this conditions is a minimal precaution expected to be always verified before eventually activating the \"SPATIALITE_SECURITY=relaxed\" mode.<hr>");
  html +=
    wxT
    ("The return type is Integer (total count of suspected Triggers); 0 means that the currently connected DB should be considered absolutely safe.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c30\">SQL utility functions [<i>non-standard</i>] for geometric objects</a></a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromExifGpsBlob( image Blob ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a POINT Geometry will be returned representing the GPS long/lat contained within EXIF-GPS <i>metadata</i>	for the BLOB image<br>");
  html +=
    wxT
    ("NULL will be returned if for any reason it's not possible to build such a POINT</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Point( x Double precision , y Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">simply an alias-name for MakePoint() (SRID is never supported).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakePoint( x Double precision , y Double precision [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry will be returned representing the POINT defined by [x y] coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakePointZ( x Double precision , y Double precision , z Double precision [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry will be returned representing the POINT Z defined by [x y z] coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakePointM( x Double precision , y Double precision , m Double precision [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry will be returned representing the POINT M defined by [x y m] coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakePointZM( x Double precision , y Double precision , z Double precision , m Double precision ");
  html += wxT("[ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry will be returned representing the POINT ZM defined by [x y z m] coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeLine( pt1 PointGeometry , pt2 PointGeometry ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Linestring Geometry will be returned representing the segment connecting pt1 to pt2<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeLine( geom PointGeometry ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Linestring Geometry will be returned connecting all the input Points (accordingly to input sequence)<br>");
  html += wxT("<b><u>aggregate function</u></b><br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeLine( geom MultiPointGeometry , direction Boolean ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Linestring Geometry will be returned connecting all the input Points (accordingly to input sequence); \"direction=FALSE\" implies reverse order.<br>");
  html +=
    wxT
    ("<u>Please note</u>: similar to the previuous one, but this one is an ordinary (not aggregate) function; a MultiPoint input is always expected.<hr>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeCircle( cx Double , cy Double , radius Double [ , SRID Integer [ , step Double ] ] ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a closed LINESTRING approximating the Circle defined by \"cx\", \"cy\" and \"radius\".<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the circumference; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeEllipse( cx Double , cy Double , x_axis Double , y_axus Double [ , SRID Integer [ , step Double ] ] ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a closed LINESTRING approximating the Ellipsee defined by \"cx\", \"cy\" and \"x_axis\", \"y_axis\".<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the ellipse; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeArc( cx Double , cy Double , radius Double , start Double , stop Double [ , SRID Integer [ , step Double ] ] ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a LINESTRING approximating the Circular Arc defined by \"cx\", \"cy\" and \"radius\"; the arc's extremities will be defined by \"start\", \"stop\" angles expressed in degrees.<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the circumference; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeEllipticArc( cx Double , cy Double , x_axis Double , y_axis Double, start Double , stop Double [ , SRID Integer [ , step Double ] ] ) : LinestringGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a LINESTRING approximating the Elliptic Arc defined by \"cx\", \"cy\" and \"x_axis\", \"y_axis\"; the arc's extremities will be defined by \"start\", \"stop\" angles expressed in degrees.<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the ellipse; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeCircularSector( cx Double , cy Double , radius Double , start Double , stop Double [ , SRID Integer [ , step Double ] ] ) : PolygonGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a POLYGON approximating the Circular Sector defined by \"cx\", \"cy\" and \"radius\"; the arc's extremities will be defined by \"start\", \"stop\" angles expressed in degrees.<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the circumference; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeEllipticSector( cx Double , cy Double , x_axis Double , y_axis Double, start Double , stop Double [ , SRID Integer [ , step Double ] ] ) : PolygonGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a POLYGON approximating the Elliptic Sector defined by \"cx\", \"cy\" and \"x_axis\", \"y_axis\"; the arc's extremities will be defined by \"start\", \"stop\" angles expressed in degrees.<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the ellipse; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeCircularStripe( cx Double , cy Double , radius_1 Double , radius_2 Double, start Double , stop Double [ , SRID Integer [ , step Double ] ] ) : PolygonGeometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">will return a POLYGON approximating the Circular Stripe delimited by two arcs sharing the same Centre \"cx\", \"cy\" ");
  html +=
    wxT
    ("but having different radii \"radius_1\", \"radius_2\"; the arc's extremities will be defined by \"start\", \"stop\" angles expressed in degrees.<br>");
  html +=
    wxT
    ("The optional argument \"step\" if specified defines how many points will be interpolated on the circumference; a point will be set every \"step\" degrees.<br>");
  html +=
    wxT
    ("The implicit default setting corresponds to a point every 10 degrees.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Collect( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html += wxT("ST_Collect( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a generic Geometry (possibly a GEOMETRYCOLLECTION) will be returned merging geom1 and geom2<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Collect( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_Collect( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a generic Geometry (possibly a GEOMETRYCOLLECTION) will be returned merging input Geometries all together<br>");
  html += wxT("<b><u>aggregate function</u></b><br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BuildArea( geom Geometry  ) : Geometry<hr>");
  html += wxT("ST_BuildArea( geom Geometry  ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a POLYGON or MULTIPOLYGON) will be returned.<br>");
  html += wxT
    ("The input Geometry is expected to represent a LINESTRING or a MULTILINESTRING.<br>The input Geometry can be an arbitrary collection ");
  html += wxT
    ("of sparse Linestrings: this function will then try to (possibly) reassemble them into one (or more) polygon(s).");
  html += wxT
    ("<br>If the second [optional] argument is TRUE then a MULTIPOLYGON will be returned anyway.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Polygonize( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_Polygonize( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Exactly the same as ST_BuildArea, but implemented as an <u>aggregate function</u>.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LineMerge( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_LineMerge( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING or MULTILINESTRING) will be returned.<br>");
  html += wxT
    ("The input Geometry is expected to represent a LINESTRING or a MULTILINESTRING.<br>The input Geometry can be an arbitrary collection ");
  html += wxT
    ("of sparse Linestrings: this function will then try to (possibly) reassemble them into one (or more) polygon(s).");
  html += wxT
    ("<br>If the second [optional] argument is TRUE then a MULTIPOLYGON will be returned anyway.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LinesFromRings( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_LinesFromRings( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING or MULTILINESTRING) will be returned.<br>");
  html +=
    wxT
    ("The input Geometry is expected to be a POLYGON or MULTIPOLYGON; any RING will then be transformed into the corresponding LINESTRING.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LinesCutAtNodes( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html +=
    wxT
    ("ST_LinesCutAtNodes( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING or MULTILINESTRING) will be returned.<br>");
  html +=
    wxT
    ("The first input Geometry is expected to be a LINESTRING or MULTILINESTRING (Lines); ");
  html +=
    wxT
    ("the second input Geometry is expected to be a POINT or MULTIPOINT (Nodes).<br>");
  html +=
    wxT
    ("any Line will then be possibly split in two halves where some vertex exactely intercepts a Node.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RingsCutAtNodes( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_RingsCutAtNodes( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING or MULTILINESTRING) will be returned.<br>");
  html +=
    wxT
    ("The input Geometry is expected to be a POLYGON or MULTIPOLYGON (Rings); ");
  html +=
    wxT
    ("any self-intersection or intersection between Rings is assumed to represent a Node.<br>");
  html +=
    wxT
    ("any Ring will then be possibly split in two halves where some vertex exactely intercepts a Node.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DissolveSegments( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_DissolveSegments( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING, MULTILINESTRING or GEOMETRYCOLLECTION) will be returned.<br>");
  html +=
    wxT
    ("The input Geometry is arbitrary: any POINT will remain unaffected, but any LINESTRING or RING will be dissolved into elementary segments.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DissolvePoints( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_DissolvePoints( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a POINT or MULTIPOINT) will be returned.<br>");
  html +=
    wxT
    ("The input Geometry is arbitrary: any POINT will remain unaffected, but any LINESTRING or RING will be dissolved into elementary Points corresponding to each Vertex.<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CollectionExtract( geom Geometry, type Integer ) : Geometry<hr>");
  html +=
    wxT("ST_CollectionExtract( geom Geometry, type Integer ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Given a GEOMETRYCOLLECTION, returns a MULTI* geometry consisting only of the specified type.<br>");
  html +=
    wxT
    ("Sub-geometries that are not the specified type are ignored.<br>(1=POINT-type, 2=LINESTRING-type, 3=POLYGON-type)<br>");
  html +=
    wxT
    ("NULL will be returned if any error is encountered (or when no item of required type is found)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Locate_Along_Measure( geom Geometry, m_value Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return a derived geometry collection value with elements that match the specified measure.<br>");
  html +=
    wxT
    ("NULL will be returned if any error is encountered (or when no element corresponding to the given measure is found).<br>");
  html +=
    wxT
    ("Please note: NULL will be returned anyway if Geometry doesn't support M-dimension, or if it contains any Polygon, or if it's of the GeometryCollection type.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Locate_Between_Measures( geom Geometry, m_start Double precision , m_end Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return a derived geometry collection value with elements that match the specified range of measures.<br>");
  html +=
    wxT
    ("NULL will be returned if any error is encountered (or when no element corresponding to the given range if measures is found).<br>");
  html +=
    wxT
    ("Please note: NULL will be returned anyway if Geometry doesn't support M-dimension, or if it contains any Polygon, or if it's of the GeometryCollection type.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SquareGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_SquareGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a grid of square cells (having the edge length of size) precisely covering the input Geometry (expected to be a Polygon or MultiPolygon).<br>");
  html +=
    wxT
    ("The returned Geometry will usually be of the MultiPolygon type (a collection of Squares), but will be a MultiLinestring if the optional edges_only argument is set to TRUE<br>");
  html +=
    wxT
    ("If the optional origin argument (expected to be a Point) is not specified then the (0,0) grid origin will be assumed by default.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">TriangularGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_TriangularGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a grid of square cells (having the edge length of size) precisely covering the input Geometry (expected to be a Polygon or MultiPolygon).<br>");
  html +=
    wxT
    ("The returned Geometry will usually be of the MultiPolygon type (a collection of Triangles), but will be a MultiLinestring if the optional edges_only argument is set to TRUE<br>");
  html +=
    wxT
    ("If the optional origin argument (expected to be a Point) is not specified then the (0,0) grid origin will be assumed by default.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">HexagonalGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_HexagonalGrid( geom Geometry [ , size Double precision [ , edges_only Boolean [ , origin Geometry  ]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a grid of square cells (having the edge length of size) precisely covering the input Geometry (expected to be a Polygon or MultiPolygon).<br>");
  html +=
    wxT
    ("The returned Geometry will usually be of the MultiPolygon type (a collection of Hexagons), but will be a MultiLinestring if the optional edges_only argument is set to TRUE<br>");
  html +=
    wxT
    ("If the optional origin argument (expected to be a Point) is not specified then the (0,0) grid origin will be assumed by default.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DelaunayTriangulation( geom Geometry [ , edges_only Boolean [ , tolerance Double precision  ]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_DelaunayTriangulation( geom Geometry [ , edges_only Boolean [ , tolerance Double precision ]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return the Delaunay Triangulation corresponding to the input Geometry.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">VoronojDiagram( geom Geometry [ , edges_only Boolean [ , extra_frame_size Double precision [ , tolerance Double precision ]]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_VoronojDiagram( geom Geometry [ , edges_only Boolean [ , extra_frame_size Double precision [ , tolerance Double precision ]]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return the Voronoj Diagram corresponding to the input Geometry.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ConcaveHull( geom Geometry [ , factor Double precision [ , allow_hols Boolean [ , tolerance Double precision  ]]] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_ConcaveHull( geom Geometry [ , factor Double precision [ , allow_hols Boolean [ , tolerance Double precision  ]]] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return the ConcaveHull corresponding to the input Geometry.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeValid( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_MakeValid( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return a surely valid version of the input Geometry.<br>");
  html +=
    wxT
    ("If the input Geometry was already valid, then it will be returned exactly as it was.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MakeValidDiscarded( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_MakeValidDiscarded( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return all elements that would be eventually discarded by ST_MakeValid() while validating the same input Geometry.<br>");
  html +=
    wxT
    ("NULL will be returned if any error is encountered, or if no discarded item exists.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Segmentize( geom Geometry , dist Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("ST_Segmentize( geom Geometry , dist Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Geometry corresponding to the input Geometry; as much Linestring / Ring vertices ");
  html +=
    wxT
    ("as required will be eventually interpolated so to ensure that no segment will be longer than dist.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Split( geom Geometry , blade Geometry ) : Geometry<hr>");
  html += wxT("ST_Split( geom Geometry , blade Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Geometry collecting all items resulting by splitting the input Geometry by the blade.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SplitLeft( geom Geometry , blade Geometry ) : Geometry<hr>");
  html += wxT("ST_SplitLeft( geom Geometry , blade Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Geometry collecting all items resulting by splitting the input Geometry by the blade and falling on the left side.<br>");
  html +=
    wxT
    ("All items not affected by the split operation (i.e. not intersecting the blade) will be returned into the left collection.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SplitRight( geom Geometry , blade Geometry ) : Geometry<hr>");
  html +=
    wxT("ST_SplitRight( geom Geometry , blade Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Geometry collecting all items resulting by splitting the input Geometry by the blade and falling on the right side.<br>");
  html +=
    wxT
    ("NULL will be returned if any error is encountered (or if the right side is empty).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Azimuth( pt1 Geometry , pt2 Geometry ) : Double precision<hr>");
  html +=
    wxT("ST_Azimuth( pt1 Geometry , pt2 Geometry ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the angle (in radians) from the horizontal of the vector defined by pt1 and pt2.<br>");
  html += wxT("Both pt1 and pt2 are expected to be simple Points.<br>");
  html +=
    wxT
    ("Starting since 4.1.0 if both points supports \"long/lat\" coords the returned Azimuth will be precisely computed on the ellipsoid.<br>");
  html += wxT("NULL will be returned if any error is encountered.<hr>");
  html += wxT("On the clock: 12=0; 3=PI/2; 6=PI; 9=3PI/2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Project( start_point Geometry, distance Double precision, azimuth Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("Project( start_point Geometry, distance Double precision, azimuth Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Point projected from a start point using a bearing and distance.<br>");
  html +=
    wxT
    ("start_point is expected to be simple long/lat Point.<br>distance is expected to be measured in meters; ");
  html +=
    wxT
    ("azimuth (aka bearing or heading) has the same identical meaning as in ST_Azimuth().<br>");
  html += wxT("NULL is returned on failure or on invalid arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SnapToGrid( geom Geometry , size Double precision  ) : Geometry<br>");
  html +=
    wxT
    ("SnapToGrid( geom Geometry , size_x Double precision , size_y Double precision ) : Geometry<br>");
  html +=
    wxT
    ("SnapToGrid( geom Geometry , origin_x Double precision , origin_y Double precision , size_x Double precision , size_y Double precision ) : Geometry<br>");
  html +=
    wxT
    ("SnapToGrid( geom Geometry , origin Geometry , size_x Double precision , size_y Double precision , size_z Double precision , size_m Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("ST_SnapToGrid( geom Geometry , size Double precision  ) : Geometry<br>");
  html +=
    wxT
    ("ST_SnapToGrid( geom Geometry , size_x Double precision , size_y Double precision )  ) : Geometry<br>");
  html +=
    wxT
    ("ST_SnapToGrid( geom Geometry , origin_x Double precision , origin_y Double precision , size_x Double precision , size_y Double precision )  ) : Geometry<br>");
  html +=
    wxT
    ("ST_SnapToGrid( geom Geometry , origin Geometry , size_x Double precision , size_y Double precision, size_z Double precision , size_m Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a new Geometry corresponding to the input Geometry; all points and vertices will be snapped to the grid defined by its origin and size(s).<br>");
  html += wxT("Removes all consecutive points falling on the same cell.<br>");
  html +=
    wxT
    ("All collapsed geometries will be stripped from the returned Geometry.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">GeoHash( geom Geometry ) : String<hr>");
  html += wxT("ST_GeoHash( geom Geometry ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return a GeoHash representation (geohash.org) of the geometry.<br>");
  html +=
    wxT
    ("A GeoHash encodes a point into a text form that is sortable and searchable based on prefixing.<hr>");
  html +=
    wxT
    ("ST_GeoHash will not work with geometries that are not in geographic (lon/lat) coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsX3D( geom Geometry [ , precision Integer [ , options Integer [ , refid String ]]] ) : String<hr>");
  html +=
    wxT
    ("ST_AsX3D( geom Geometry [ , precision Integer [ , options Integer [ , refid String ]]] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Returns a geometry as an X3D XML formatted node element</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MaxDistance( geom1 Geometry , geom2 Geometry ) : Double precision<hr>");
  html +=
    wxT
    ("ST_MaxDistance( geom1 Geometry , geom2 Geometry ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the max distance between geom1 and geom2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_3DDistance( geom1 Geometry , geom2 Geometry ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the 3D-distance between geom1 and geom2 (considering Z coordinates)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Max3DDistance( geom1 Geometry , geom2 Geometry ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the max 3D-distance between geom1 and geom2 (considering Z coordinates)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BuildMbr( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision, [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">[x1 y1] and [x2 y2] are assumed to be Points identifying a line segment;");
  html +=
    wxT
    ("then a Geometry will be returned representing the MBR for this line segment</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BuildCircleMbr( x Double precision , y Double precision , radius Double precision [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">[x y] is assumed to be the center of a circle of given radius; then a Geometry will be returned representing the MBR for this circle</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Extent( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object representing the bounding box that encloses a set of input values<br>");
  html += wxT("<b><u>aggregate function</u></b></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">ToGARS( geom Geometry ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">geom is expected to represent a POINT (longitude and latitude coords); ");
  html +=
    wxT("the corresponding GARS area designation code will be returned.<hr>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GARSMbr( GARS_code String ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">code is assumed to represent a valid GARS area designation code; ");
  html +=
    wxT
    ("a Geometry will be returned representing the MBR for the corresponding GARS area.<hr>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrMinX( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MbrMinX( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MinX( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the x-coordinate for geom MBR's leftmost side as a double precision number.<br>");
  html +=
    wxT("NULL will be returned if geom isn't a valid Geometry.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrMinY( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MbrMinY( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MinY( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the y-coordinate for geom MBR's lowermost side as a double precision number.<br>");
  html +=
    wxT("NULL will be returned if geom isn't a valid Geometry.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrMaxX( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MbrMaxX( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MaxX( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the x-coordinate for geom MBR's rightmost side as a double precision number.<br>");
  html +=
    wxT("NULL will be returned if geom isn't a valid Geometry.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrMaxY( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MbrMaxY( geom Geometry) : Double precision<hr>");
  html += wxT("ST_MaxY( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the y-coordinate for geom MBR's uppermost side as a double precision number.<br>");
  html +=
    wxT("NULL will be returned if geom isn't a valid Geometry.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_MinZ( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the minimum Z-coordinate value for geom as a double precision number.<br>");
  html +=
    wxT
    ("NULL will be returned if geom isn't a valid Geometry or if geom has no Z dimension.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_MaxZ( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the maximum Z-coordinate value for geom as a double precision number.<br>");
  html +=
    wxT
    ("NULL will be returned if geom isn't a valid Geometry or if geom has no Z dimension.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_MinM( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the minimum M-coordinate value for geom as a double precision number.<br>");
  html +=
    wxT
    ("NULL will be returned if geom isn't a valid Geometry or if geom has no M dimension.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_MaxM( geom Geometry) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the maximum M-coordinate value for geom as a double precision number.<br>");
  html +=
    wxT
    ("NULL will be returned if geom isn't a valid Geometry or if geom has no M dimension.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c31\">functions for constructing a geometric object given its Well-known Text Representation</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromText( wkt String [ , SRID Integer] ) : Geometry<hr>");
  html +=
    wxT("ST_GeomFromText( wkt String [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">construct a geometric object given its Well-known text Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_WKTToSQL( wkt String ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">SQL/MM alias name for ST_GeomFromText: SRID=0 is assumed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PointFromText( wktPoint String [ , SRID Integer] ) : Point<hr>");
  html +=
    wxT("ST_PointFromText( wktPoint String [ , SRID Integer] ) : Point</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Point</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LineFromText( wktLineString String [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("ST_LineFromText( wktLineString String [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("LineStringFromText( wktLineString String [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("ST_LineStringFromText( wktLineString String [ , SRID Integer] ) : Linestring</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Linestring</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PolyFromText( wktPolygon String [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT("ST_PolyFromText( wktPolygon String [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT("PolygonFromText( wktPolygon String [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT
    ("ST_PolygonFromText( wktPolygon String [ , SRID Integer] ) : Polygon</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Polygon</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MPointFromText( wktMultiPoint String [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("ST_MPointFromText( wktMultiPoint String [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("MultiPointFromText( wktMultiPoint String [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("ST_MultiPointFromText( wktMultiPoint String [ , SRID Integer] ) : MultiPoint</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiPoint</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MLineFromText( wktMultiLineString String [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("ST_MLineFromText( wktMultiLineString String [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("MultiLineStringFromText( wktMultiLineString String [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("ST_MultiLineStringFromText( wktMultiLineString String [ , SRID Integer] ) : MultiLinestring</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiLinestring</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MPolyFromText( wktMultiPolygon String [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_MPolyFromText( wktMultiPolygon String [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("MultiPolygonFromText( wktMultiPolygon String [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_MultiPolygonFromText( wktMultiPolygon String [ , SRID Integer] ) : MultiPolygon</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiPolygon</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomCollFromText( wktGeometryCollection String [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("ST_GeomCollFromText( wktGeometryCollection String [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("GeometryCollectionFromText( wktGeometryCollection String [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("ST_GeometryCollectionFromText( wktGeometryCollection String [ , SRID Integer] ) : GeometryCollection</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">construct a GeometryCollection</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BdPolyFromText( wktMultilinestring String [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT
    ("ST_BdPolyFromText( wktMultilinestring String [ , SRID Integer] ) : Polygon</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct a Polygon given an arbitrary collection of closed ");
  html +=
    wxT
    ("linestrings as a MultiLineString text representation.<hr>see also: BuildArea(), Polygonize()</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BdMPolyFromText( wktMultilinestring String [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_BdMPolyFromText( wktMultilinestring String [ , SRID Integer] ) : MultiPolygon</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct a MultiPolygon given an arbitrary collection of closed ");
  html +=
    wxT
    ("linestrings as a MultiLineString text representation.<hr>see also: BuildArea(), Polygonize()</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c32\">functions for constructing a geometric object given its Well-known Binary Representation</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromWKB( wkbGeometry Binary [ , SRID Integer] ) : Geometry<hr>");
  html +=
    wxT
    ("ST_GeomFromWKB( wkbGeometry Binary [ , SRID Integer] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">construct a geometric object given its Well-known binary Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_WKBToSQL( wkbGeometry Binary ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">SQL/MM alias name for ST_GeomFromWKB: SRID=0 is assumed.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PointFromWKB( wkbPoint Binary [ , SRID Integer] ) : Point<hr>");
  html +=
    wxT("ST_PointFromWKB( wkbPoint Binary [ , SRID Integer] ) : Point</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Point</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LineFromWKB( wkbLineString Binary [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("ST_LineFromWKB( wkbLineString Binary [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("LineStringFromWKB( wkbLineString Binary [ , SRID Integer] ) : Linestring<hr>");
  html +=
    wxT
    ("ST_LineStringFromWKB ( wkbLineString Binary [ , SRID Integer] ) : Linestring</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Linestring</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PolyFromWKB( wkbPolygon Binary [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT("ST_PolyFromWKB( wkbPolygon Binary [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT("PolygonFromWKB( wkbPolygon Binary [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT
    ("ST_PolygonFromWKB( wkbPolygon Binary [ , SRID Integer] ) : Polygon</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a Polygon</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MPointFromWKB( wkbMultiPoint Binary [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("ST_MPointFromWKB( wkbMultiPoint Binary [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("MultiPointFromWKB( wkbMultiPoint Binary [ , SRID Integer] ) : MultiPoint<hr>");
  html +=
    wxT
    ("ST_MultiPointFromWKB( wkbMultiPoint Binary [ , SRID Integer] ) : MultiPoint</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiPoint</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MLineFromWKB( wkbMultiLineString Binary [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("ST_MLineFromWKB( wkbMultiLineString Binary [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("MultiLineStringFromWKB( wkbMultiLineString Binary [ , SRID Integer] ) : MultiLinestring<hr>");
  html +=
    wxT
    ("ST_MultiLineStringFromWKB( wkbMultiLineString Binary [ , SRID Integer] ) : MultiLinestring</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiLinestring</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MPolyFromWKB( wkbMultiPolygon Binary [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_MPolyFromWKB( wkbMultiPolygon Binary [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("MultiPolygonFromWKB( wkbMultiPolygon Binary [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_MultiPolygonFromWKB( wkbMultiPolygon Binary [ , SRID Integer] ) : MultiPolygon</td>");
  html += wxT("<td bgcolor=\"#f0fff0\">construct a MultiPolygon</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomCollFromWKB( wkbGeometryCollection Binary [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("ST_GeomCollFromWKB( wkbGeometryCollection Binary [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("GeometryCollectionFromWKB( wkbGeometryCollection Binary [ , SRID Integer] ) : GeometryCollection<hr>");
  html +=
    wxT
    ("ST_GeometryCollectionFromWKB( wkbGeometryCollection Binary [ , SRID Integer] ) : GeometryCollection</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">construct a GeometryCollection</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BdPolyFromWKB( wkbMultilinestring Binary [ , SRID Integer] ) : Polygon<hr>");
  html +=
    wxT
    ("ST_BdPolyFromWKB( wkbMultilinestring Binary [ , SRID Integer] ) : Polygon</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct a Polygon given an arbitrary collection of closed ");
  html +=
    wxT
    ("linestrings as a MultiLineString binary representation.<hr>see also: BuildArea(), Polygonize()</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">BdMPolyFromWKB( wkbMultilinestring Binary [ , SRID Integer] ) : MultiPolygon<hr>");
  html +=
    wxT
    ("ST_BdMPolyFromWKB( wkbMultilinestring Binary [ , SRID Integer] ) : MultiPolygon</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct a MultiPolygon given an arbitrary collection of closed ");
  html +=
    wxT
    ("linestrings as a MultiLineString binary representation.<hr>see also: BuildArea(), Polygonize()</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c33\">functions for obtaining the Well-known Text / Well-known Binary Representation of a geometric object</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">AsText( geom Geometry ) : String<hr>");
  html +=
    wxT
    ("ST_AsText( geom Geometry ) : String</td><td bgcolor=\"#f0fff0\">return the Well-known Text representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsWKT( geom Geometry [ , precision Integer ] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the Well-known Text representation<hr>always return strictly conformant 2D WKT</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">AsBinary( geom Geometry ) : Binary<hr>");
  html +=
    wxT
    ("ST_AsBinary( geom Geometry ) : Binary</td><td bgcolor=\"#f0fff0\">return the Well-known Binary representation</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c33misc\">SQL functions supporting exotic geometric formats</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsSVG( geom Geometry, [ , relative Integer [ , precision Integer ] ] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the SVG [<i>Scalable Vector Graphics</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsKML( geom Geometry [ , precision Integer ] ) : String<br>");
  html +=
    wxT
    ("AsKML( name String, description String, geom Geometry [ , precision Integer ] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the KML [<i>Keyhole Markup Language</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromKml( KmlGeometry String ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its KML Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsGML( geom Geometry [ , precision Integer ] ) : String<br>");
  html +=
    wxT
    ("AsGML( version Integer, geom Geometry [ , precision Integer ] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the GML [<i>Geography Markup Language</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromGML( gmlGeometry String ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its GML Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsGeoJSON( geom Geometry [ , precision Integer [ , options Integer ] ] ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns the GeoJSON [<i>Geographic JavaScript Object Notation</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromGeoJSON( GeoJSONGeometry String ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its GeoJSON Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsEWKB( geom Geometry ) : String</td><td bgcolor=\"#f0fff0\">returns the EWKB [<i>Extended Well Known Binary</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromEWKB( ewkbGeometry String ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its EWKB Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsEWKT( geom Geometry ) : String</td><td bgcolor=\"#f0fff0\">returns the EWKT [<i>Extended Well Known Text</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromEWKT( ewktGeometry String ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its EWKT Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AsFGF( geom Geometry ) : String</td><td bgcolor=\"#f0fff0\">returns the FGF [<i>FDO Geometry Binary Format</i>] representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeomFromFGF( fgfGeometry Binary [ , SRID Integer] ) : Geometry</td><td bgcolor=\"#f0fff0\">construct a geometric object given its FGF binary Representation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ExportDXF( out_dir String , filename String , sql_query String , layer_col_name String , ");
  html +=
    wxT
    ("geom_col_name String , label_col_name String , text_height_col_name String , text_rotation_col_name String , geom_filter Geometry [ , precision Integer ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Exports a whole DXF file.<br>Will return 0 (i.e. FALSE) on failure, any other value (i.e. TRUE) on success.<hr>");
  html +=
    wxT
    ("Please note this SQL function open the doors to many potential security issues, and thus is always disabled by default.<br>");
  html +=
    wxT
    ("Explicitly setting the environmente variable SPATIALITE_SECURITY=relaxed is absolutely required in order to effectively enable this function.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c34\">functions on type Geometry</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Dimension( geom Geometry ) : Integer<hr>");
  html += wxT("ST_Dimension( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the dimension of the geometric object, which is less than or equal to the dimension of the coordinate space</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CoordDimension( geom Geometry ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the dimension model used by the geometric object as:<br>'XY', 'XYZ', 'XYM' or 'XYZM'</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">ST_NDims( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the number of dimensions used by the geometric object as:<br>2, 3 or 4</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">ST_Is3D( geom Geometry ) : Integer</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">Checks if geom has the Z dimension.<br>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a ");
  html += wxT("function invocation on invalid arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_IsMeasured( geom Geometry ) : Integer</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">Checks if geom has the M dimension.<br>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a ");
  html += wxT("function invocation on invalid arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeometryType( geom Geometry ) : String<hr>");
  html += wxT("ST_GeometryType( geom Geometry ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the name of the instantiable subtype of Geometry of which this geometric object is a member, as a string</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">SRID( geom Geometry ) : Integer<hr>");
  html += wxT("ST_SRID( geom <i>Geometry</i> ) : <i>Integer</i></td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the Spatial Reference System ID for this geometric object</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SetSRID( geom Geometry , SRID Integer ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">directly sets the Spatial Reference System ID for this geometric object [no reprojection is applied].<br>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a ");
  html += wxT("function invocation on NULL arguments</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsEmpty( geom Geometry ) : Integer<hr>");
  html += wxT("ST_IsEmpty( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if this geometric object corresponds to the empty set</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsSimple( geom Geometry ) : Integer<hr>");
  html += wxT("ST_IsSimple( geom <i>Geometry</i> ) : <i>Integer</i></td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if this geometric object is simple, as defined in the Geometry Model</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">IsValid( geom Geometry ) : Integer<hr>");
  html += wxT("ST_IsValid( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if this geometric object does not contains any topological error</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Boundary( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_Boundary( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the combinatorial boundary of g as defined in the Geometry Model</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Envelope( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_Envelope( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the rectangle bounding g as a Polygon. ");
  html +=
    wxT
    ("The Polygon is defined by the corner points of the bounding box [(MINX, MINY),(MAXX, MINY), (MAXX, MAXY), (MINX, MAXY), (MINX, MINY)].</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Expand( geom Geometry , amount Double ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the rectangle bounding g as a Polygon.<br>");
  html +=
    wxT
    ("The bounding rectangle is expanded in all directions by an amount specified by the second argument.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_NPoints( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the total number of Points (this including any Linestring/Polygon vertex)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_NRings( geom Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the total number of Rings (this including both Exterior and Interior Rings)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Reverse( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a new Geometry [if possible] or NULL<hr>");
  html += wxT("Any Linestring or Ring will be in reverse order.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_ForceLHR( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a new Geometry [if possible] or NULL<hr>");
  html +=
    wxT
    ("Any Polygon will be oriented accordingly to Left Hand Rule (Exterior Ring clockwise, ");
  html += wxT("Interior Rings counter-clockwise).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AddPoint( line Linestring , point Point [ , position Integer ] ) : Linestring<hr>");
  html +=
    wxT
    ("ST_AddPoint( line Linestring , point Point [ , position Integer ] ) : Linestring</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Adds a new Point into the input Linestring immediately before \"position\" (zero-based index)<br>");
  html +=
    wxT
    ("A negative \"position\" (default) means appending the new Point to the end of the input Linestring.<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SetPoint( line Linestring , position Integer , point Point ) : Linestring<hr>");
  html +=
    wxT
    ("ST_SetPoint( line Linestring , position Integer , point Point ) : Linestring</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Replaces the Point into the input Linestring at \"position\" (zero-based index)<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RemovePoint( line Linestring , position Integer ) : Linestring<hr>");
  html +=
    wxT
    ("ST_RemovePoint( line Linestring , position Integer ) : Linestring</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Removes the Point into the input Linestring at \"position\" (zero-based index)<br>");
  html += wxT("NULL will be returned if any error is encountered.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"repair\">Functions attempting to repair malformed Geometries</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SanitizeGeometry( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a (possibly) sanitized Geometry or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"compress\">Geometry-compression functions</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CompressGeometry( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a compressed Geometry or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">UncompressGeometry( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns an uncompressed Geometry or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"cast\">Geometry-type casting functions</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToPoint( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a POINT-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToLinestring( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a LINESTRING-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToPolygon( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a POLYGON-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToMultiPoint( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a MULTIPOINT-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToMultiLinestring( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a MULTILINESTRING-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToMultiPolygon( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a MULTIPOLYGON-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToGeometryCollection( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a GEOMETRYCOLLECTION-type Geometry [if possible] or NULL</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToMulti( geom Geometry ) : geom Geometry<hr>");
  html += wxT("ST_Multi( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a MULTIPOINT-, MULTILINESTRING- or MULTIPOLYGON-type Geometry [if possible]: NULL in any other case</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToSingle( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a POINT-, LINESTRING- or POLYGON-type Geometry [if possible]: NULL in any other case</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"dims-cast\">Space-dimensions casting functions</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToXY( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a Geometry using the [XY] space dimension</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToXYZ( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a Geometry using the [XYZ] space dimension</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToXYM( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a Geometry using the [XYM] space dimension</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CastToXYZM( geom Geometry ) : geom Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">returns a Geometry using the [XYZM] space dimension</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c35\">functions on type Point</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">X( pt Point ) : Double precision<hr>");
  html +=
    wxT
    ("ST_X( pt Point ) : Double precision</td><td bgcolor=\"#f0fff0\">return the x-coordinate of Point p as a double precision number</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Y( pt Point ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Y( pt Point ) : Double precision</td><td bgcolor=\"#f0fff0\">return the y-coordinate of Point p as a double precision number</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">Z( pt Point ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Z( pt Point ) : Double precision</td><td bgcolor=\"#f0fff0\">return the z-coordinate of Point p as a double precision number");
  html += wxT("or NULL if no z-coordinate is available</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">M( pt Point ) : Double precision<hr>");
  html +=
    wxT
    ("ST_M( pt Point ) : Double precision</td><td bgcolor=\"#f0fff0\">return the m-coordinate of Point p as a double precision number<br>");
  html += wxT("or NULL if no m-coordinate is available</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c361\">functions on type Curve [Linestring or Ring]</a></td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">StartPoint( c Curve ) : Point<hr>");
  html +=
    wxT
    ("ST_StartPoint( c Curve ) : Point</td><td bgcolor=\"#f0fff0\">return a Point containing the first Point of c</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">EndPoint( c Curve ) : Point<hr>");
  html +=
    wxT
    ("ST_EndPoint( c Curve ) : Point</td><td bgcolor=\"#f0fff0\">return a Point containing the last Point of c</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">NumPoints( line LineString ) : Integer<hr>");
  html +=
    wxT
    ("ST_NumPoints( line LineString ) : Integer</td><td bgcolor=\"#f0fff0\">return the number of Points in the LineString</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PointN( line LineString , n Integer ) : Point<hr>");
  html +=
    wxT
    ("ST_PointN( line LineString , n Integer ) : Point</td><td bgcolor=\"#f0fff0\">return a Point containing Point n of line</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GLength( c Curve [ , use_ellipsoid Boolean ] ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Length( c Curve [ , use_ellipsoid Boolean ] ) : Double precision</td><td bgcolor=\"#f0fff0\">return the length of c</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Perimeter( s Surface [ , use_ellipsoid Boolean ] ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Perimeter( s Surface [ , use_ellipsoid Boolean ] ) : Double precision</td><td bgcolor=\"#f0fff0\">return the perimeter of s</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeodesicLength( c Curve ) : Double precision</td><td bgcolor=\"#f0fff0\">return the geodesic length of c");
  html +=
    wxT
    (" measured on the Ellipsoid [<i>only for <b>geographic</b> SRIDs</i>]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GreatCircleLength( c Curve ) : Double precision</td><td bgcolor=\"#f0fff0\">return the Great Circle length of c");
  html += wxT(" [<i>only for <b>geographic</b> SRIDs</i>]</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">IsClosed( c Curve ) : Integer<hr>");
  html += wxT("ST_IsClosed( c Curve ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("return TRUE if c is closed, i.e., if StartPoint(c) = EndPoint(c)</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">IsRing( c Curve ) : Integer<hr>");
  html += wxT("ST_IsRing( c Curve ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("return TRUE if c is a ring, i.e., if c is closed and simple. A simple Curve does not pass through the same Point more than once.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PointOnSurface( s Surface/Curve ) : Point<hr>");
  html +=
    wxT
    ("ST_PointOnSurface( s Surface ) : Point</td><td bgcolor=\"#f0fff0\">return a Point guaranteed to lie on the Surface (or Curve)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Simplify( c Curve , tolerance Double precision ) : Curve<hr>");
  html +=
    wxT("ST_Simplify( c Curve , tolerance Double precision ) : Curve<hr>");
  html +=
    wxT("ST_Generalize( c Curve , tolerance Double precision ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object representing a simplified version of c applying the Douglas-Peukert algorithm with given tolerance</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SimplifyPreserveTopology( c Curve , tolerance Double precision ) : Curve<hr>");
  html +=
    wxT
    ("ST_SimplifyPreserveTopology( c Curve , tolerance Double precision ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object representing a simplified version of c ");
  html +=
    wxT
    ("applying the Douglas-Peukert algorithm with given tolerance and respecting topology</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c36\">functions on type Surface [Polygon or Ring]</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">Centroid( s Surface ) : Point<hr>");
  html +=
    wxT
    ("ST_Centroid( s Surface ) : Point</td><td bgcolor=\"#f0fff0\">return the centroid of s, which may lie outside s</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Area( s Surface [ , use_ellipsoid Boolean ] ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Area( s Surface [ , use_ellipsoid Boolean ] ) : Double precision</td><td bgcolor=\"#f0fff0\">return the area of s</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c37\">functions on type Polygon</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ExteriorRing( polyg Polygon ) : LineString<hr>");
  html +=
    wxT
    ("ST_ExteriorRing( polyg Polygon ) : LineString</td><td bgcolor=\"#f0fff0\">return the exteriorRing of p</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">NumInteriorRing( polyg Polygon ) : Integer<hr>NumInteriorRings( polyg Polygon ) : Integer<hr>");
  html += wxT("ST_NumInteriorRing( polyg Polygon ) : Integer</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">return the number of interiorRings</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">InteriorRingN( polyg Polygon , n Integer ) : LineString<hr>");
  html +=
    wxT("ST_InteriorRingN( polyg Polygon , n Integer ) : LineString</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the nth interiorRing. The order of Rings is not geometrically significant.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c38\">functions on type GeomCollection</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">NumGeometries( geom GeomCollection ) : Integer<hr>");
  html +=
    wxT
    ("ST_NumGeometries( geom GeomCollection ) : Integer</td><td bgcolor=\"#f0fff0\">return the number of geometries</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GeometryN( geom GeomCollection , n Integer ) : Geometry<hr>");
  html +=
    wxT("ST_GeometryN( geom GeomCollection , n Integer ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the nth geometric object in the collection. The order of the elements in the collection is not geometrically significant.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c39\">functions testing approximative spatial relationships via MBRs</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrEqual( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html += wxT("TRUE if g1 and g2 have equal MBRs</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrDisjoint( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the intersection of g1 and g2 MBRs is the empty set</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrTouches( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the only Points in common between g1 and g2 MBRs lie in the union of the boundaries of g1 and g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrWithin( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html += wxT("TRUE if g1 MBR is completely contained in g2 MBR</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrOverlaps( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the intersection of g1 and g2 MBRs results in a value of the same dimension as g1 and g2 that is different from both g1 and g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrIntersects( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if the intersection of g1 and g2 MBRs is not empty</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_EnvIntersects( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html +=
    wxT
    ("ST_EnvelopesIntersects( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html +=
    wxT
    ("ST_EnvIntersects( geom1 Geometry , x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision ) : Integer<hr>");
  html +=
    wxT
    ("ST_EnvelopesIntersects( geom1 Geometry , x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The first form simply is an alias name for MbrIntersects; the other form allows to define the second MBR by two extreme points [x1, y1] and [x2, y2].<br>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on invalid arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if the intersection of both MBRs is not empty</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">MbrContains( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if g2 MBR is completely contained in g1 MBR</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c40\">functions testing spatial relationships</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Equals( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Equals( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html += wxT("TRUE if g1 and g2 are equal</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Disjoint( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Disjoint( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT("TRUE if the intersection of g1 and g2 is the empty set</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Touches( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Touches( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the only Points in common between g1 and g2 lie in the union of the boundaries of g1 and g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Within( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Within( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html += wxT("TRUE if g1 is completely contained in g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Overlaps( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Overlaps( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the intersection of g1 and g2 results in a value of the same dimension as g1 and g2 that is different from both g1 and g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Crosses( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Crosses( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("TRUE if the intersection of g1 and g2 results in a value whose dimension is less than the maximum dimension of g1 and g2 ");
  html +=
    wxT
    ("and the intersection value includes Points interior to both g1 and g2, and the intersection value is not equal to either g1 or g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Intersects( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html +=
    wxT("ST_Intersects( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if the intersection of g1 and g2 is not empty</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Contains( geom1 Geometry , geom2 Geometry ) : Integer<hr>");
  html += wxT("ST_Contains( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if g2 is completely contained in g1</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Covers( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT("convenience predicate: TRUE if g1 completely covers g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CoveredBy( geom1 Geometry , geom2 Geometry ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments<br>");
  html +=
    wxT
    ("convenience predicate: TRUE if g1 is completely covered by g2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Relate( geom1 Geometry , geom2 Geometry , patternMatrix String ) : Integer<hr>");
  html +=
    wxT
    ("ST_Relate( geom1 Geometry , geom2 Geometry , patternMatrix String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.<br>");
  html +=
    wxT
    ("return TRUE if the spatial relationship specified by the patternMatrix holds</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Distance( geom1 Geometry , geom2 Geometry [ , use_ellipsoid Boolean ] ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Distance( geom1 Geometry , geom2 Geometry [ , use_ellipsoid Boolean ] ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the distance between geom1 and geom2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">HausdorffDistance( geom1 Geometry , geom2 Geometry ) : Double precision<hr>");
  html +=
    wxT
    ("ST_HausdorffDistance( geom1 Geometry , geom2 Geometry ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the Hausdorff Distance between geom1 and geom2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">PtDistWithin( geom1 Geometry , geom2 Geometry , range Double precision [ , use_spheroid Integer ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">checks if the distance between geom1 and geom2 is within the given range.<br>");
  html +=
    wxT
    ("As a special case if both geoms are simple WGS84 POINTs (SRID=4326) distances are expessed in meters.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c41\">functions implementing spatial operators</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Intersection( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html +=
    wxT("ST_Intersection( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the intersection of geometric objects geom1 and geom2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Difference( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html +=
    wxT("ST_Difference( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the closure of the set difference of geom1 and geom2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GUnion( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html += wxT("ST_Union( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the set union of geom1 and geom2</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">GUnion( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_Union( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the set union of input values<br>");
  html += wxT("<b><u>aggregate function</u></b></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SymDifference( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html +=
    wxT("ST_SymDifference( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the closure of the set symmetric difference of geom1 and geom2 (logical XOR of space)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Buffer( geom Geometry , dist Double precision ) : Geometry<hr>");
  html +=
    wxT("ST_Buffer( geom Geometry , dist Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object defined by buffering a distance d around geom, ");
  html +=
    wxT
    ("where dist is in the distance units for the Spatial Reference of geom</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ConvexHull( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_ConvexHull( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object that is the convex hull of geom</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">OffsetCurve( geom Curve , radius Double precision, left_or_right Integer ) : Curve<hr>");
  html +=
    wxT
    ("ST_OffsetCurve( geom Curve , radius Double precision, left_or_right Integer ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object representing the corresponding left- (or right-sided) offset curve<br>");
  html +=
    wxT
    ("NULL if returned whenever is not possible deriving an offset curve from the original geometry<br>");
  html +=
    wxT("[a single not-closed LINESTRING is expected as input]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SingleSidedBuffer( geom Curve , radius Double precision, left_or_right Integer ) : Curve<hr>");
  html +=
    wxT
    ("ST_SingleSidedBuffer( geom Curve , radius Double precision, left_or_right Integer ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object representing the corresponding left- (or right-sided) single-sided buffer<br>");
  html +=
    wxT
    ("NULL if returned whenever is not possible deriving a single-sided buffer from the original geometry<br>");
  html +=
    wxT("[a single not-closed LINESTRING is expected as input]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SharedPaths( geom1 Geometry , geom2 Geometry ) : Geometry<hr>");
  html +=
    wxT("ST_SharedPaths( geom1 Geometry , geom2 Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object (of the MULTILINESTRING type) representing any common edge shared by both geometries<br>");
  html += wxT("NULL if returned is no common edge exists</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Line_Interpolate_Point( line Curve , fraction Double precision ) : Point<hr>");
  html +=
    wxT
    ("ST_Line_Interpolate_Point( line Curve , fraction Double precision ) : Point</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a point interpolated along a line.<br>Second argument (between 0.0 and 1.0) representing fraction");
  html += wxT("of total length of linestring the point has to be located.<br>");
  html += wxT("NULL if returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Line_Interpolate_Equidistant_Points( line Curve , distance Double precision ) : Point<hr>");
  html +=
    wxT
    ("ST_Line_Interpolate_Equidistant_Points( line Curve , distance Double precision ) : Point</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a set of equidistant points interpolated along a line; the returned geometry ");
  html +=
    wxT
    ("always corresponds to a MULTIPOINT supporting the M coordinate (representing the progressive distance for each interpolated Point).<br>");
  html +=
    wxT
    ("Second argument represents the regular distance between interpolated points.<br>");
  html += wxT("NULL if returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Line_Locatate_Point( line Curve , point Point ) : Double precision<hr>");
  html +=
    wxT
    ("ST_Line_Locate_Point( line Curve , point Point ) : Double precision</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a number (between 0.0 and 1.0) representing the location of the closest point on LineString");
  html += wxT("to the given Point, as a fraction of total 2d line length.<br>");
  html += wxT("NULL if returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Line_Substring( line Curve , start_fraction Double precision , end_fraction Double precision ) : Curve<hr>");
  html +=
    wxT
    ("ST_Line_Substring( line Curve , start_fraction Double precision , end_fraction Double precision ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return a Linestring being a substring of the input one starting and ending at the given fractions of total 2d length.<br>");
  html +=
    wxT
    ("Second and third arguments are expected to be in the range between 0.0 and 1.0.<br>");
  html += wxT("NULL if returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ClosestPoint( geom1 Geometry , geom2 Geometry ) : Point<hr>");
  html +=
    wxT("ST_ClosestPoint( geom1 Geometry , geom2 Geometry ) : Point</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Returns the Point on geom1 that is closest to geom2.<br>");
  html +=
    wxT
    ("NULL is returned for invalid arguments (or if distance is ZERO)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ShortestLine( geom1 Geometry , geom2 Geometry ) : Curve<hr>");
  html +=
    wxT("ST_ShortestLine( geom1 Geometry , geom2 Geometry ) : Curve</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Returns the shortest line between two geometries.<br>");
  html +=
    wxT
    ("NULL is returned for invalid arguments (or if distance is ZERO)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Snap( geom1 Geometry , geom2 Geometry , tolerance Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("ST_Snap( geom1 Geometry , geom2 Geometry , tolerance Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Returns a new Geometry representing a modified geom1, so to \"snap\" vertices ");
  html +=
    wxT
    ("and segments to geom2 vertices; a snap distance tolerance is used to control where snapping is performed.<br>");
  html += wxT("NULL is returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">LineMerge( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_LineMerge( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">a Geometry (actually corresponding to a LINESTRING or MULTILINESTRING) will be returned.");
  html +=
    wxT
    ("The input Geometry is expected to represent a LINESTRING or a MULTILINESTRING.<br>The input Geometry can be an ");
  html +=
    wxT
    ("arbitrary collection of sparse line fragments: this function will then try to (possibly) reassemble them into one");
  html += wxT("(or more) Linestring(s).<br>");
  html += wxT("NULL is returned for invalid arguments</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">UnaryUnion( geom Geometry ) : Geometry<hr>");
  html += wxT("ST_UnaryUnion( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Exactely the same as ST_Union, but applied to a single Geometry.<br>");
  html +=
    wxT
    ("(set union of elementary Geometries within a MULTI- or GEOMETRYCOLLECTION complex Geometry).<br>");
  html += wxT("NULL will be returned if any error is encountered</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c42\">functions for coordinate transformations</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">Transform( geom Geometry , newSRID Integer ) : Geometry<br>");
  html +=
    wxT("ST_Transform( geom Geometry , newSRID Integer ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by reprojecting coordinates into the Reference System identified by newSRID</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SridFromAuthCRS( auth_name String , auth_SRID Integer ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return the internal SRID corresponding to auth_name and auth_SRID<br>-1 will be returned if no such CRS is defined</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ShiftCoords( geom Geometry , shiftX Double precision , shiftY Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("ShiftCoordinates( geom Geometry , shiftX Double precision , shiftY Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by translating coordinates according to shiftX and shiftY values</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Translate( geom Geometry , shiftX Double precision , shiftY Double precision , shiftZ Double Precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by translating coordinates according to shiftX, shiftY and shiftZ values</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ST_Shift_Longitude( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by translating any negative longitude by 360.<hr>");
  html +=
    wxT
    ("Only meaningful for geographic (longitude/latitude) coordinates.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">NormalizeLonLat( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by normalizing any longitude in the range");
  html += wxT("[-180 / +180] and any latitude in the range [-90 / + 90].<hr>");
  html +=
    wxT
    ("Only meaningful for geographic (longitude/latitude) coordinates.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ScaleCoords( geom Geometry , scaleX Double precision [ , scaleY Double precision ] ) : Geometry<hr>");
  html +=
    wxT
    ("ScaleCoordinates( geom Geometry , scaleX Double precision [ , scaleY Double precision ] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by scaling coordinates according to scaleX and scaleY values<br>");
  html +=
    wxT
    ("if only one scale factor is specified, then an isotropic scaling occurs [i.e. the same scale factor is applied to both axis]");
  html +=
    wxT
    ("otherwise an anisotropic scaling occurs [i.e. each axis is scaled according to its own scale factor]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RotateCoords( geom Geometry , angleInDegrees Double precision ) : Geometry<hr>");
  html +=
    wxT
    ("RotateCoordinates( geom Geometry , angleInDegrees Double precision ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by rotating coordinates according to angleInDegrees value<br>");
  html += wxT("Positive angle = clockwise rotation</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">ReflectCoords( geom Geometry , xAxis Integer , yAxis Integer ) : Geometry<hr>");
  html +=
    wxT
    ("ReflectCoordinates( geom Geometry , xAxis Integer , yAxis Integer ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by reflecting coordinates according to xAxis and yAxis switches<br>");
  html +=
    wxT
    ("i.e. if xAxis is 0 (FALSE), then x-coordinates remains untouched; otherwise x-coordinates will be reflected</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SwapCoords( geom Geometry ) : Geometry<hr>SwapCoordinates( geom Geometry ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">return a geometric object obtained by swapping x- and y-coordinates</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c43\">functions for Spatial-MetaData and Spatial-Index handling</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html += wxT
    ("<tr><td bgcolor=\"#fffff0\">InitSpatialMetaData( void ) : Integer<hr>");
  html += wxT("InitSpatialMetaData( transaction Integer ) : Integer<hr>");
  html += wxT("InitSpatialMetaData( mode String ) : Integer<hr>");
  html +=
    wxT
    ("InitSpatialMetaData( transaction Integer , mode String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates the geometry_columns and spatial_ref_sys metadata tables");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE<hr>");
  html +=
    wxT
    ("the first form (no args) will automatically populate spatial_ref_sys inserting any possible ESPG SRID definition<br>");
  html +=
    wxT
    ("if the arg 'WGS84' (alias 'WGS84_ONLY') is specified, then only WGS84-releated EPSG SRIDs will be inserted<br>");
  html +=
    wxT
    ("if the arg 'NONE' (alias 'EMPTY') is specified, no EPSG SRID will be inserted at all</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">InsertEpsgSrid( srid <i>Integer</i> ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Attempts to insert into spatial_ref_sys the EPSG definition uniquely identified by srid<br>");
  html +=
    wxT
    ("[the corresponding EPSG SRID definition will be copied from the inlined dataset defined in libspatialite<hr>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for success or 0 for failure</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AddGeometryColumn( table String , column String , srid Integer , geom_type String , dimension String [ , not_null Integer ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates a new geometry column updating the Spatial Metadata tables and creating any required trigger in order to enforce constraints<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RecoverGeometryColumn( table String , column String , srid Integer , geom_type String , dimension String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Validates an existing ordinary column in order to possibly transform it in a real geometry column, ");
  html +=
    wxT
    ("thus updating the Spatial Metadata tables and creating any required trigger in order to enforce constraints<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DiscardGeometryColumn( table String , column String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Removes a geometry column from Spatial MetaData tables and drops any related trigger<br>");
  html +=
    wxT
    ("the column itself still continues to exist untouched as an ordinary, unconstrained column<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterVirtualGeometry( table String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Registers a VirtualShape table into Spatial MetaData tables; the VirtualShape table should be already created in some previous steo.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DropVirtualGeometry( table String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Removes a VirtualShape table from Spatial MetaData tables, dropping the VirtualShape table as well.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CreateSpatialIndex( table String , column String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Builds an RTree Spatial Index on a geometry column, ");
  html +=
    wxT
    ("creating any required trigger required in order to enforce full data coherency between the main table and Spatial Index<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DisableSpatialIndex( table String , column String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Disables an RTree Spatial Index, removing any related trigger<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">CheckSpatialIndex( void ) : Integer<hr>");
  html +=
    wxT("CheckSpatialIndex( table String , column String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Checks an RTree Spatial Index for validity and consistency<br>");
  html +=
    wxT
    ("- if no arguments are passed, then any RTree defined into geometry_columns will be checked<br>");
  html +=
    wxT
    ("- otherwise only the RTree corresponding to table and column will be checked<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RecoverSpatialIndex( [ no_check Integer ] ) : Integer<hr>");
  html +=
    wxT
    ("RecoverSpatialIndex( table String , column String [ , no_check Integer ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Recovers a (possibly broken) RTree Spatial Index<br>");
  html +=
    wxT
    ("- if no arguments are passed, then any RTree defined into geometry_columns will be recovered<br>");
  html +=
    wxT
    ("- otherwise only the RTree corresponding to table and column will be recovered<br>");
  html +=
    wxT("- the optional argument no_check will be interpreted as follows:<br>");
  html +=
    wxT
    ("&nbsp;&nbsp;* if no_check = FALSE (default) the RTree will be checked first: ");
  html += wxT("and only an invalid RTree will be then actually rebuilt<br>");
  html +=
    wxT
    ("&nbsp;&nbsp;* if no_check = TRUE the RTree will be unconditionally rebuilt from scratch anyway<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">UpdateLayerStatistics( void ) : Integer<hr>");
  html +=
    wxT
    ("UpdateLayerStatistics( table String [, column String ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Updates the internal Layer Statistics [Feature Count and Total Extent]<br>");
  html +=
    wxT
    ("- if no arguments are passed, then internal statics will be updated for any possible Geometry Column ");
  html += wxT("defined in the current DB<br>");
  html +=
    wxT
    ("- otherwise statistics will be updated only for Geometry Columns corresponding to the given table<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GetLayerExtent( table String [ , column String [ , mode Boolean] ] ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return the Envelope corresponding to the Total Extent of some Layer; ");
  html +=
    wxT
    ("if the Table/Layer only contains a single Geometry column passing the column name isn't strictly required.<br>");
  html +=
    wxT
    ("NULL will be returned if any error occurs or if the required table isn't a Layer.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CreateTopologyTables( SRID Integer , dims String ) : Integer<hr>");
  html +=
    wxT
    ("CreateTopologyTables( prefix String , SRID Integer , dims String ) : Integer</td>");
  html +=
    wxT("<td bgcolor=\"#f0fff0\">Creates a set of <b>Topology</b> tables.<br>");
  html +=
    wxT
    ("<ul><li>the SRIDargument is mandatory</li><li>the dims argument must be 'XY' or 'XYZ': 2 or 3 are valid aliases</li>");
  html +=
    wxT
    ("<li>the optionalargument prefix can be used to support more Topology sets on the same DB: ");
  html +=
    wxT
    ("if omitted a \"topo_\" prefix will be assumed by default</li></ul><hr>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE (failure).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CreateRasterCoveragesTable( void ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates the <b>raster_coverages</b> table required by RasterLite-2<hr>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE (failure).</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c43style\">functions supporting SLD/SE Styled Layers</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">CreateStylingTables( ) : Integer<hr>");
  html += wxT("CreateStylingTables( relaxed Integer ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates a set of tables supporting <b>SLD/SE Styled Layers</b>.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterExternalGraphic( xlink_href String , resource BLOB ) : Integer<hr>");
  html +=
    wxT
    ("RagisterExternalGraphic( xlink_href String , resource BLOB , title String , abstract String , file_name String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) an <b>External Graphic Resource</b>.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterVectorStyledLayer( f_table_name String , f_geometry_column String , style BLOB ) : Integer<hr>");
  html +=
    wxT
    ("RegisterVectorStyledLayer( f_table_name String , f_geometry_column String , style_id Integer , style BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) a <b>Vector Styled Layer</b> definition.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterRasterStyledLayer( coverage_name String , style BLOB ) : Integer<hr>");
  html +=
    wxT
    ("RegisterRasterStyledLayer( coverage_name String , style_id Integer , style BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) a <b>Raster Styled Layer</b> definition.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterStyledGroup( group_name String , f_table_name String , f_geometry_colum String , style_id Integer [ , paint_order Integer ] ) : Integer<hr>");
  html +=
    wxT
    ("RegisterStyledGroup( group_name String, coverage_name String , style_id Integer [ , paint_order Integer ] ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) a <b>Styled Group</b> definition.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">SetStyledGroupInfos( group_name String , title String , abstract String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) the descriptive infos associated to a <b>Styled Group</b><br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c43isometa\">functions implementing ISO Metadata</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">CreateIsoMetadataTables( ) : Integer<hr>");
  html += wxT("CreateIsoMetadataTables( relaxed Integer ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates a set of tables supporting <b>ISO Metadata</b>.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RegisterIsoMetadata( scope String , metadata BLOB ) : Integer<hr>");
  html +=
    wxT
    ("RegisterIsoMetadata( scope String , metadata BLOB , id Integer ) : Integer<hr>");
  html +=
    wxT
    ("RagisterIsoMetadata( scope String , metadata BLOB , fileIdentifier String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Inserts (or updates) an <b>ISO Metadata</b> definition.<br>");
  html +=
    wxT
    ("The first form (two arguments only) always performs an INSERT; if one the optional arguments \"id\" or \"fileIdentifier\" an UPDATE could be eventually performed if a corresponding metadata row is already defined.<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE (success) or 0 for FALSE ");
  html +=
    wxT("(failure): -1 will be returned on invalid argumentes.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">GetIsoMetadataId( fileIdentifier String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Return the unique \"id\" corresponding to the ISO Metadata definition identified by <b>fileIdentifier</b>.<hr>");
  html +=
    wxT
    ("If no corresponding ISO Metadata definition exists, this function will always return ZERO; -1 will be returned for invalid arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c43fdo\">functions implementing FDO/OGR compatibily</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">CheckSpatialMetaData( void ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Checks the Spatial Metadata type, then returning:<br>");
  html +=
    wxT
    ("<b>0</b> if the <i>geometry_columns</i> and <i>spatial_ref_sys</i> table does not exist, ");
  html +=
    wxT
    ("or if their actual layout doesn't corresponds to any known implementation<br>");
  html +=
    wxT
    ("<b>1</b> if both tables exist, and their layout is the one used by <i>SpatiaLite legacy</i> (older versions including 3.0.1)<br>");
  html +=
    wxT
    ("<b>2</b> if both tables exist, and their layout is the one used by <i>FDO/OGR</i><br>");
  html +=
    wxT
    ("<b>3</b> if both tables exist, and their layout is the one currently used by <i>SpatiaLite</i> (3.1.0 or any subsequent version)</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">AutoFDOStart( void ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">This function will inspect the Spatial Metadata, then automatically creating/refreshing a <i>VirtualFDO</i>");
  html +=
    wxT
    (" wrapper for each FDO/OGR geometry table<br>the return type is Integer [how many VirtualFDO tables have been created]</td></tr>");
  html += wxT("<tr><td bgcolor=\"#fffff0\">AutoFDOStop( void ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">This function will inspect the Spatial Metadata, then automatically destroying any <i>VirtualFDO</i>");
  html +=
    wxT
    (" wrapper found<br>the return type is Integer [how many VirtualFDO tables have been destroyed]</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">InitFDOSpatialMetaData( void ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates the geometry_columns and spatial_ref_sys metadata tables");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE<br>");
  html +=
    wxT
    ("<u>Please note:</u> Spatial Metadata created using this function will have the FDO/OGR layout, and not the SpatiaLite's own</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">AddFDOGeometryColumn( table String , column String , srid Integer , geom_type String , dimension Integer , geometry_type String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Creates a new geometry column updating the FDO/OGR Spatial Metadata tables<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RecoverFDOGeometryColumn( table String , column String , srid Integer , geom_type String , dimension Integer , geometry_type String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Validates an existing ordinary column in order to possibly transform it in a real geometry column, ");
  html += wxT("thus updating the FDO/OGR Spatial Metadata tables<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">DiscardFDOGeometryColumn( table String , column String ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Removes a geometry column from FDO/OGR Spatial MetaData tables<br>");
  html +=
    wxT
    ("the column itself still continues to exist untouched as an ordinary column<br>");
  html +=
    wxT
    ("the return type is Integer, with a return value of 1 for TRUE or 0 for FALSE</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c44\">functions for MbrCache-based queries</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">FilterMbrWithin( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Retrieves from an MbrCache any entity whose MBR falls within");
  html +=
    wxT
    (" the rectangle identified by extreme points x1 y1 and x2 y2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">FilterMbrContains( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Retrieves from an MbrCache any entity whose MBR contains");
  html +=
    wxT
    (" the rectangle identified by extreme points x1 y1 and x2 y2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">FilterMbrIntersects( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Retrieves from an MbrCache any entity whose MBR intersects");
  html +=
    wxT
    (" the rectangle identified by extreme points x1 y1 and x2 y2</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"c45\">functions for R*Tree-based queries (Geometry Callbacks)</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RTreeWithin( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">*DEPRECATED* (alias-name for RTreeIntersects)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RTreeContains( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">*DEPRECATED* (alias-name for RTreeIntersects)</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RTreeIntersects( x1 Double precision , y1 Double precision , x2 Double precision , y2 Double precision  ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Retrieves from an R*Tree any entity whose MBR intersects");
  html +=
    wxT
    (" the rectangle identified by extreme points x1 y1 and x2 y2</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">RTreeDistWithin( x Double precision , y Double precision , radius Double precision ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Retrieves from an R*Tree any entity whose MBR intersects");
  html +=
    wxT
    (" the square square circumscribed on the given circle  (x y center, radius)</td></tr>");
  html +=
    wxT
    ("<tr><td align=\"center\" bgcolor=\"#e0ffe0\" colspan=\"2\"><a name=\"xmlBlob\">SQL functions supporting XmlBLOB</a>");
  html += wxT("<br><a href=\"#index\">back to index</a></td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_Create( xmlPayload BLOB ) : XmlBLOB<hr>");
  html +=
    wxT("XB_Create( xmlPayload BLOB , compressed Boolean  ) : XmlBLOB<hr>");
  html +=
    wxT
    ("XB_Create( xmlPayload BLOB , compressed Boolean ,  schemaURI Text ) : XmlBLOB<hr>");
  html +=
    wxT
    ("XB_Create( xmlPayload BLOB , compressed Boolean , internalSchemaURI Boolean  ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct an XmlBLOB object starting from an XmlDocument.<hr>");
  html +=
    wxT
    ("NULL will be returned for not well-formed XmlDocuments, or when XML validation is required but XmlDocument fails to pass validation for any reason.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetPayload( xmlObject BLOB [ , indent Integer ]  ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Extracts a generic BLOB from an XmlBLOB object, exactly corresponding to the original XmlDocument and fully preserving the original character encoding.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetDocument( xmlObject BLOB [ , indent Integer ]  ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Extracts an XmlDocument from an XmlBLOB object; the returned XmlDocument will always be UTF-8 encoded (TEXT), irrespectively from the original internal encoding declaration.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_SchemaValidate( xmlObject BLOB , schemaURI Text  [ , compressed Boolean ]  ) : XmlBLOB<hr>");
  html +=
    wxT
    ("XB_SchemaValidate( xmlObject BLOB , internalSchemaURI Boolean  [ , compressed Boolean ]  ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Construct an XML validated XmlBLOB object starting from an XmlDocument.<hr>");
  html +=
    wxT
    ("NULL will be returned if the input XmlBLOB fails to pass validation for any reason.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_Compress( xmlObject BLOB ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">A new compressed XmlBLOB object will be returned.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_Uncompress( xmlObject BLOB ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">A new uncompressed XmlBLOB object will be returned.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsValid( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsCompressed( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsSchemaValidated( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsIsoMetadata( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsSldSeVectorStyle( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsSldSeRasterStyle( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsSvg( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetDocumentSize( xmlObject BLOB ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the size in bytes of the corresponding uncompressed XmlDocument.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetEncoding( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the character encoding internally declared by the XmlDocument corresponding to the input XmlBLOB.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetSchemaURI( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the Schema URI effectively used to validate an XmlBLOB.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetInternalSchemaURI( xmlPayload BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the Schema URI internally declared by the input XmlDocument.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetFileId( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the FileId defined within the XmlBLOB (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_SetFileId( xmlObject BLOB , fileId String ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return a new XmlBLOB by replacing the FileIdentifier value.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_AddFileId( xmlObject BLOB , fileId String , IdNameSpacePrefix String , IdNameSpaceURI String , CsNameSpacePrefix String , CsNameSpaceURI String ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return a new XmlBLOB by inserting a FileIdentifier value.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetParentId( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the ParentId defined within the XmlBLOB (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_SetParentId( xmlObject BLOB , parentId String ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return a new XmlBLOB by replacing the ParentIdentifier value.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_AddParentId( xmlObject BLOB , parentId String , IdNameSpacePrefix String , IdNameSpaceURI String , CsNameSpacePrefix String , CsNameSpaceURI String ) : XmlBLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return a new XmlBLOB by inserting a ParentIdentifier value.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetTitle( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the Title defined within the XmlBLOB (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetAbstract( xmlObject BLOB ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the Abstract defined within the XmlBLOB (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetGeometry( xmlObject BLOB ) : Geometry</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the Geometry defined within the XmlBLOB (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetLastParseError( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the most recent XML parsing error (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetLastValidateError( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the most recent XML validating error (if any).</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_IsValidXPathExpression( expr Text ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">The return type is Integer, with a return value of 1 for TRUE, 0 for FALSE, ");
  html +=
    wxT
    ("and -1 for UNKNOWN corresponding to a function invocation on NULL arguments.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_GetLastXPathError( void ) : String</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Will return the most recent XPath error (if any).</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">XB_CacheFlush( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Reset the internal XML Schema cache to its initial empty state.</td></tr>");
  html +=
    wxT("<tr><td bgcolor=\"#fffff0\">XB_CacheFlush( void ) : Boolean</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">Reset the internal XML Schema cache to its initial empty state.</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_LoadXML( filepath-or-URL String ) : BLOB</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">If \"filepath-or-URL\" corresponds to some valid local pathname, and the corresponding file (expected to contain a well-formed XML Document) ");
  html +=
    wxT
    ("can be actually accessed in read mode, then the whole file content will be returned as a BLOB value.<br>");
  html +=
    wxT
    ("This function is even able to acces a remote XML Document identified by an URL.<br>Otherwise NULL will be returned.<br>");
  html +=
    wxT
    ("<u>Please note:</u> SQLite doesn't support BLOB values bigger than SQLITE_MAX_LENGTH (usually, 1 GB).<hr>");
  html +=
    wxT
    ("<u>Please note well</u>: this SQL function open the doors to many potential security issues, and thus is always disabled by default.<br>");
  html +=
    wxT
    ("Explicitly setting the environmente variable \"SPATIALITE_SECURITY=relaxed\" is absolutely required in order to effectively enable this function.<br>");
  html += wxT("Please see: CountUnsafeTriggers().</td></tr>");
  html +=
    wxT
    ("<tr><td bgcolor=\"#fffff0\">XB_StoreXML( XmlObject XmlBLOB , filepath String ) : Integer<hr>");
  html +=
    wxT
    ("XB_StoreXML( XmlObject XmlBLOB , filepath String , indent Integer ) : Integer</td>");
  html +=
    wxT
    ("<td bgcolor=\"#f0fff0\">If \"XmlObject\" is of the XmlBLOB-type, and if \"filepath\" corresponds to some valid pathname ");
  html +=
    wxT
    ("(accessible in write/create mode), then the corresponding file will be created/overwritten so to ");
  html +=
    wxT
    ("contain the corresponding XML Document (fully preserving the original character encoding).<br>");
  html +=
    wxT
    ("If the optional argument \"indent\" is set to some positive value then the returned XmlDocument will be nicely formatted and properly indented by the required factor; ");
  html +=
    wxT
    ("ZERO will cause the whole XmlDocument to be returned as a single line. (default setting is <i>negative</i> indenting, i.e. not reformatting at all).<hr>");
  html +=
    wxT
    ("The return type is Integer, with a return value of 1 for success, 0 for failure and -1 for invalid arguments.<hr>");
  html +=
    wxT
    ("<u>Please note well</u>: this SQL function open the doors to many potential security issues, and thus is always disabled by default.<br>");
  html +=
    wxT
    ("Explicitly setting the environmente variable \"SPATIALITE_SECURITY=relaxed\" is absolutely required in order to effectively enable this function.<br>");
  html += wxT("Please see: CountUnsafeTriggers().</td></tr>");
  html += wxT("</table>");
  html += wxT("<a href=\"#index\">back to index</a>");
  html += wxT("</body>");
  html += wxT("</html>");
}
