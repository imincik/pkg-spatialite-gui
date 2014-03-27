/*
/ QueryView.cpp
/ a panel to set SQL queries
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

#include "wx/clipbrd.h"
#include "wx/filename.h"

//
// ICONs in XPM format [universally portable]
//
#include "icons/sql_go.xpm"
#include "icons/sql_abort.xpm"
#include "icons/sql_abort_no.xpm"
#include "icons/hs_back.xpm"
#include "icons/hs_back_no.xpm"
#include "icons/hs_forward.xpm"
#include "icons/hs_forward_no.xpm"

MyQueryView::MyQueryView(MyFrame * parent, wxWindowID id):
wxPanel(parent, id, wxDefaultPosition, wxSize(440, 76), wxBORDER_SUNKEN)
{
//
// constructor: a frame for SQL Queries
//
  MainFrame = parent;
  BracketStart = -1;
  BracketEnd = -1;
  IgnoreEvent = false;
// SQL statement
  SqlCtrl =
    new MySqlControl(this, ID_SQL, wxT(""), wxPoint(40, 5),
                     wxSize(20, 20),
                     wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB |
                     wxHSCROLL | wxTE_RICH);
  BtnSqlGo =
    new wxBitmapButton(this, ID_SQL_GO, wxBitmap(sql_go_xpm), wxPoint(340, 5),
                       wxSize(32, 32));
  BtnSqlGo->SetToolTip(wxT("Execute SQL statement"));
  BtnSqlAbort =
    new wxBitmapButton(this, ID_SQL_ABORT, wxBitmap(sql_abort_xpm),
                       wxPoint(340, 38), wxSize(32, 32));
  BtnSqlAbort->SetBitmapDisabled(wxBitmap(sql_abort_no_xpm));
  BtnSqlAbort->SetToolTip(wxT("Abort SQL query"));
  BtnHistoryBack =
    new wxBitmapButton(this, ID_HISTORY_BACK, wxBitmap(hs_back_xpm),
                       wxPoint(5, 5), wxSize(32, 32));
  BtnHistoryBack->SetBitmapDisabled(wxBitmap(hs_back_no_xpm));
  BtnHistoryBack->SetToolTip(wxT("History: previous SQL statement"));
  BtnHistoryForward =
    new wxBitmapButton(this, ID_HISTORY_FORWARD, wxBitmap(hs_forward_xpm),
                       wxPoint(5, 40), wxSize(32, 32));
  BtnHistoryForward->SetBitmapDisabled(wxBitmap(hs_forward_no_xpm));
  BtnHistoryForward->SetToolTip(wxT("History: next SQL statement"));
  SetHistoryStates();
  BtnSqlAbort->Enable(false);
// setting up event handlers
  Connect(ID_SQL_GO, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyQueryView::OnSqlGo);
  Connect(ID_SQL_ABORT, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyQueryView::OnSqlAbort);
  Connect(ID_HISTORY_BACK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyQueryView::OnHistoryBack);
  Connect(ID_HISTORY_FORWARD, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyQueryView::OnHistoryForward);
  Connect(wxID_ANY, wxEVT_SIZE, (wxObjectEventFunction) & MyQueryView::OnSize);
  Connect(wxID_ANY, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & MyQueryView::OnSqlSyntaxColor);
}

void MyQueryView::ShowControls()
{
//
// making all SQL controls to be visible
//
  SqlCtrl->Show(true);
  BtnSqlGo->Show(true);
  BtnSqlAbort->Show(true);
  BtnHistoryBack->Show(true);
  BtnHistoryForward->Show(true);
  SetHistoryStates();
}

void MyQueryView::HideControls()
{
//
// making all controls to be invisible
//
  SqlCtrl->Show(false);
  BtnSqlGo->Show(false);
  BtnSqlAbort->Show(false);
  BtnHistoryBack->Show(false);
  BtnHistoryForward->Show(false);

}

void MyQueryView::AddToHistory(wxString & sql)
{
//
// adds an SQL statement to history
//
  History.Add(sql);
  SetHistoryStates();
}

void MyQueryView::SetHistoryStates()
{
//
// updates the history buttons state
//
  BtnHistoryForward->Enable(History.TestNext());
  BtnHistoryBack->Enable(History.TestPrev());
}

void MyQueryView::SetSql(wxString & sql, bool execute)
{
//
// sets an SQL statement [and maybe executes it]
//
  int metaDataType = MainFrame->GetMetaDataType();
  SqlCtrl->SetValue(sql);
  if (execute == true)
    {
      if (metaDataType == METADATA_CURRENT)
        {
          // current metadata style >= v.4.0.0
          MainFrame->InsertIntoLog(sql);
        }
      if (MainFrame->GetRsView()->ExecuteSqlPre(sql, 0, true) == false)
        {
          if (metaDataType == METADATA_CURRENT)
            {
              // current metadata style >= v.4.0.0
              MainFrame->UpdateLog(MainFrame->GetRsView()->GetSqlErrorMsg());
            }
          wxMessageBox(MainFrame->GetRsView()->GetSqlErrorMsg(),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, MainFrame);
      } else
        {
          if (metaDataType == METADATA_CURRENT)
            {
              // current metadata style >= v.4.0.0
              MainFrame->UpdateLog();
            }
        }
    }
}

void MyQueryView::OnSize(wxSizeEvent & WXUNUSED(event))
{
//
// this window has changed its size
//
  int vert;
  int vertBack;
  wxSize sz = GetClientSize();
// setting the SQL statement pane size
  SqlCtrl->SetSize(sz.GetWidth() - 80, sz.GetHeight() - 10);
// setting the SQL GO button position
  BtnSqlGo->Move(sz.GetWidth() - 35, 5);
// setting the SQL GO button size
  vert = sz.GetHeight() - 45;
  if (vert < 32)
    vert = 32;
  BtnSqlGo->SetSize(32, vert);
// setting the SQL ABORT button position
  BtnSqlAbort->Move(sz.GetWidth() - 35, sz.GetHeight() - 37);
// setting the SQL ABORT button size
  BtnSqlAbort->SetSize(32, 32);
// setting the HISTORY BACK button position
  BtnHistoryBack->Move(5, 5);
// setting the HISTORY BACK button size
  vert = (sz.GetHeight() - 15) / 2;
  if (vert < 32)
    vert = 32;
  BtnHistoryBack->SetSize(32, vert);
  vertBack = 10 + vert;
// setting the HISTORY FORWARD button position
  BtnHistoryForward->Move(5, vertBack);
// setting the HISTORY FORWARD button size
  BtnHistoryForward->SetSize(32, vert);
}

void MyQueryView::OnSqlGo(wxCommandEvent & WXUNUSED(event))
{
//
// executing an SQL statement
//
  int metaDataType = MainFrame->GetMetaDataType();
  wxString sql = SqlCtrl->GetValue();
  if (metaDataType == METADATA_CURRENT)
    {
      // current metadata style >= v.4.0.0
      MainFrame->InsertIntoLog(sql);
    }
  if (MainFrame->GetRsView()->ExecuteSqlPre(sql, 0, true) == false)
    {
      if (metaDataType == METADATA_CURRENT)
        {
          // current metadata style >= v.4.0.0
          MainFrame->UpdateLog(MainFrame->GetRsView()->GetSqlErrorMsg());
        }
      wxMessageBox(MainFrame->GetRsView()->GetSqlErrorMsg(),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, MainFrame);
  } else
    {
      if (metaDataType == METADATA_CURRENT)
        {
          // current metadata style >= v.4.0.0
          MainFrame->UpdateLog();
        }
    }
}

void MyQueryView::OnSqlAbort(wxCommandEvent & WXUNUSED(event))
{
//
// aborting the current SQL query
//
  MainFrame->GetRsView()->AbortRequested();
  if (MainFrame->GetMetaDataType() == METADATA_CURRENT)
    {
      // current metadata style >= v.4.0.0
      MainFrame->UpdateAbortedLog();
    }
}

void MyQueryView::OnHistoryBack(wxCommandEvent & WXUNUSED(event))
{
//
// going backward into the SQL Queries History
//
  MySqlQuery *sql = History.GetPrev();
  if (sql)
    {
      SetSql(sql->GetSql(), false);
      SetHistoryStates();
    }
}

void MyQueryView::OnHistoryForward(wxCommandEvent & WXUNUSED(event))
{
//
// going forward into the SQL Queries History
//
  MySqlQuery *sql = History.GetNext();
  if (sql)
    {
      SetSql(sql->GetSql(), false);
      SetHistoryStates();
    }
}

bool MyQueryView::IsSqlString(wxString & str)
{
// checks if this one is an SQL string constant
  char word[4096];
  strcpy(word, str.ToUTF8());
  int len = strlen(word);
  if (len < 2)
    return false;
  if (word[0] == '\'' && word[len - 1] == '\'')
    return true;
  if (word[0] == '"' && word[len - 1] == '"')
    return true;
  return false;
}

bool MyQueryView::IsSqlNumber(wxString & str)
{
// checks if this one is an SQL numeric constant
  double dbl;
  return str.ToDouble(&dbl);
}

bool MyQueryView::IsSqliteExtra(wxString & str)
{
// checks if this one is an extra SQLite keyword
  if (str.CmpNoCase(wxT("asc")) == 0)
    return true;
  if (str.CmpNoCase(wxT("desc")) == 0)
    return true;
  if (str.CmpNoCase(wxT("null")) == 0)
    return true;
  if (str.CmpNoCase(wxT("trigger")) == 0)
    return true;
  if (str.CmpNoCase(wxT("for")) == 0)
    return true;
  if (str.CmpNoCase(wxT("each")) == 0)
    return true;
  if (str.CmpNoCase(wxT("row")) == 0)
    return true;
  if (str.CmpNoCase(wxT("begin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("end")) == 0)
    return true;
  if (str.CmpNoCase(wxT("before")) == 0)
    return true;
  if (str.CmpNoCase(wxT("after")) == 0)
    return true;
  if (str.CmpNoCase(wxT("virtual")) == 0)
    return true;
  return false;
}

bool MyQueryView::IsSqlFunction(wxString & str, char next_c)
{
// checks if this one is an SQL function
  if (next_c != '(')
    return false;
  if (str.CmpNoCase(wxT("raise")) == 0)
    return true;
  if (str.CmpNoCase(wxT("avg")) == 0)
    return true;
  if (str.CmpNoCase(wxT("count")) == 0)
    return true;
  if (str.CmpNoCase(wxT("group_concat")) == 0)
    return true;
  if (str.CmpNoCase(wxT("max")) == 0)
    return true;
  if (str.CmpNoCase(wxT("min")) == 0)
    return true;
  if (str.CmpNoCase(wxT("sum")) == 0)
    return true;
  if (str.CmpNoCase(wxT("total")) == 0)
    return true;
  if (str.CmpNoCase(wxT("abs")) == 0)
    return true;
  if (str.CmpNoCase(wxT("changes")) == 0)
    return true;
  if (str.CmpNoCase(wxT("char")) == 0)
    return true;
  if (str.CmpNoCase(wxT("coalesce")) == 0)
    return true;
  if (str.CmpNoCase(wxT("glob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ifnull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("instr")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("last_insert_rowid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("length")) == 0)
    return true;
  if (str.CmpNoCase(wxT("load_extension")) == 0)
    return true;
  if (str.CmpNoCase(wxT("lower")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ltrim")) == 0)
    return true;
  if (str.CmpNoCase(wxT("nullif")) == 0)
    return true;
  if (str.CmpNoCase(wxT("quote")) == 0)
    return true;
  if (str.CmpNoCase(wxT("random")) == 0)
    return true;
  if (str.CmpNoCase(wxT("randomblob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("replace")) == 0)
    return true;
  if (str.CmpNoCase(wxT("round")) == 0)
    return true;
  if (str.CmpNoCase(wxT("rtrim")) == 0)
    return true;
  if (str.CmpNoCase(wxT("soundex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("sqlite_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("substr")) == 0)
    return true;
  if (str.CmpNoCase(wxT("trim")) == 0)
    return true;
  if (str.CmpNoCase(wxT("typeof")) == 0)
    return true;
  if (str.CmpNoCase(wxT("unicode")) == 0)
    return true;
  if (str.CmpNoCase(wxT("upper")) == 0)
    return true;
  if (str.CmpNoCase(wxT("zeroblob")) == 0)
    return true;
  return false;
}

bool MyQueryView::IsSqlGeoFunction(wxString & str, char next_c)
{
// checks if this one is an SQL geo-function
  if (next_c != '(')
    return false;
  if (str.CmpNoCase(wxT("spatialite_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("geos_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("proj4_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("lwgeom_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("libxml2_version")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasIconv")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasMathSql")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasGeoCallbacks")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasGeos")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasProj")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasGeosAdvanced")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasGeosTrunk")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasLwGeom")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasEpsg")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasFreeXL")) == 0)
    return true;
  if (str.CmpNoCase(wxT("hasLibXML2")) == 0)
    return true;

  if (str.CmpNoCase(wxT("GeometryConstraints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CheckSpatialMetaData")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AutoFDOStart")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AutoFDOStop")) == 0)
    return true;
  if (str.CmpNoCase(wxT("InitFDOSpatialMetaData")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AddFDOGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RecoverFDOGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DiscardFDOGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("InitSpatialMetaData")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AddGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RecoverGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DiscardGeometryColumn")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterVirtualGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DropVirtualGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("UpdateLayerStatistics")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GetLayerExtent")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateSpatialIndex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateMbrCache")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DisableSpatialIndex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RebuildGeometryTriggers")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CheckSpatialIndex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RecoverSpatialIndex")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateTopologyTables")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateStylingTables")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterExternalGraphic")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterVectorStyledLayer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterRasterStyledLayer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterStyledGroup")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SetStyledGroupInfos")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateIsoMetadataTables")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GetIsoMetadataId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RegisterIsoMetadata")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_LoadXML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_StoreXML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CountUnsafeTriggers")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToInteger")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToDouble")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ForceAsNull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateUUID")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MD5Checksum")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MD5TotalChecksum")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CreateRasterCoveragesTable")) == 0)
    return true;

  if (str.CmpNoCase(wxT("InsertEpsgSrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Abs")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Acos")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Asin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Atan")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Ceil")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Ceiling")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Cos")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Cot")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Degrees")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Exp")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Floor")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Ln")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Log")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Log2")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Log10")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PI")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Pow")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Power")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Radians")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Round")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Sign")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Sin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Sqrt")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Stddev_pop")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Stddev_samp")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Var_pop")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Var_samp")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Tan")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsZipBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsPdfBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsGifBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsPngBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsTiffBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsWaveletBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsJpegBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsExifBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsExifGpsBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsWebpBlob")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GetMimeType")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BlobFromFile")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BlobToFile")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ExportDXF")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakePoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Point")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeLine")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeCircle")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeEllipse")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeArc")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeEllipticArc")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeCircularSector")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeCircularStripe")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeEllipticSector")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BuildMbr")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BuildCircleMbr")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Extent")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrMinX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrMinY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrMaxX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrMaxY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MbrMinX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MbrMinY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MbrMaxX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MbrMaxY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MinX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MinY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MaxX")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MaxY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MinZ")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MinM")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MaxZ")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MaxM")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeomFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_WKTToSQL")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeometryFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LineFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LineFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LineStringFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LineStringFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PolygonFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PolygonFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MPointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MPointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiPointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiPointFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MLineFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MLineFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiLineStringFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiLineStringFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiPolygonFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiPolygonFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomCollFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeomCollFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryCollectionFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BdPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_BdPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BdMPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_BdMPolyFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryCollectionFromText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeomFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_WKBToSQL")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LineFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LineFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LineStringFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LineStringFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PolygonFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PolygonFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MPointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MPointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiPointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiPointFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MLineFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MLineFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiLineStringFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiLineStringFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MultiPolygonFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MultiPolygonFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomCollFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeomCollFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryCollectionFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeometryCollectionFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BdPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_BdPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BdMPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_BdMPolyFromWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_AsText")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsWKT")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsSVG")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsKML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromKML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsGML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromGML")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsGeoJSON")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromGeoJSON")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsFGF")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsBinary")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_AsBinary")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromFGF")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsEWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromEWKB")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsEWKT")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeomFromEWKT")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CompressGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("UncompressGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SanitizeGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToLinestring")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToPolygon")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToMultiPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToMultiLinestring")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToMultiPolygon")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToGeometryCollection")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToMulti")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Multi")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToSingle")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToXY")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToXYZ")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToXYM")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CastToXYZM")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Reverse")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ForceLHR")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Dimension")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Dimension")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CoordDimension")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NDims")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Is3D")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsMeasured")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryType")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryAliasType")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeometryType")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SRID")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SRID")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SetSRID")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ToGARS")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GARSMbr")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsEmpty")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsEmpty")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsSimple")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsSimple")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsValid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsValid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Boundary")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Boundary")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Envelope")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Envelope")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Expand")) == 0)
    return true;
  if (str.CmpNoCase(wxT("X")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_X")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Y")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Y")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Z")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Z")) == 0)
    return true;
  if (str.CmpNoCase(wxT("M")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_M")) == 0)
    return true;
  if (str.CmpNoCase(wxT("StartPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_StartPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("EndPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_EndPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GLength")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Length")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Perimeter")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Perimeter")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsClosed")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsClosed")) == 0)
    return true;
  if (str.CmpNoCase(wxT("IsRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_IsRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Simplify")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Simplify")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Generalize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SimplifyPreserveTopology")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SimplifyPreserveTopology")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeodesicLength")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GreatCircleLength")) == 0)
    return true;
  if (str.CmpNoCase(wxT("NumPoints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NumPoints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PointN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PointN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Centroid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Centroid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PointOnSurface")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_PointOnSurface")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Area")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Area")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ExteriorRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ExteriorRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("NumInteriorRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NumInteriorRing")) == 0)
    return true;
  if (str.CmpNoCase(wxT("NumInteriorRings")) == 0)
    return true;
  if (str.CmpNoCase(wxT("InteriorRingN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_InteriorRingN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("NumGeometries")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NumGeometries")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NPoints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_NRings")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeometryN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeometryN")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AddPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_AddPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RemovePoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_RemovePoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SetPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SetPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrEqual")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrDisjoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrTouches")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrWithin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrOverlaps")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrIntersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_EnvIntersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_EnvelopesIntersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MbrContains")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Equals")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Equals")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Disjoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Disjoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Touches")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Touches")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Within")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Within")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Overlaps")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Overlaps")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Crosses")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Crosses")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Intersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Intersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Contains")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Contains")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Covers")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Covers")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CoveredBy")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_CoveredBy")) == 0)
    return true;
  if (str.CmpNoCase(wxT("OffsetCurve")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_OffsetCurve")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SingleSidedBuffer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SingleSidedBuffer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SharedPaths")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SharedPaths")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Relate")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Relate")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Distance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Distance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("HausdorffDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_HausdorffDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("PtDistWithin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Intersection")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Intersection")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Difference")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Difference")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GUnion")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Union")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SymDifference")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SymDifference")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Buffer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Buffer")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ConvexHull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ConvexHull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Transform")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Transform")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Shift_Longitude")) == 0)
    return true;
  if (str.CmpNoCase(wxT("NormalizeLonLat")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Line_Interpolate_Point")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Line_Interpolate_Point")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Line_Interpolate_Equidistant_Points")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Line_Interpolate_Equidistant_Points")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Line_Locate_Point")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Line_Locate_Point")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Line_Substring")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Line_Substring")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ClosestPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ClosestPoint")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ShortestLine")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ShortestLine")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Snap")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Snap")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Collect")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Collect")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LineMerge")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LineMerge")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BuildArea")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_BuildArea")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Polygonize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Polygonize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("UnaryUnion")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_UnaryUnion")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DissolveSegments")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_DissolveSegments")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DissolvePoints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_DissolvePoints")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LinesFromRings")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LinesFromRings")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RingsCutAtNodes")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_RingsCutAtNodes")) == 0)
    return true;
  if (str.CmpNoCase(wxT("LinesCutAtNodes")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_LinesCutAtNodes")) == 0)
    return true;
  if (str.CmpNoCase(wxT("CollectionExtract")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_CollectionExtract")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Locate_Along_Measure")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Locate_Between_Measures")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SquareGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SquareGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("TriangularGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_TriangularGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("HexagonalGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_HexagonalGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("DelaunayTriangulation")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_DelaunayTriangulation")) == 0)
    return true;
  if (str.CmpNoCase(wxT("VoronojDiagram")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_VoronojDiagram")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ConcaveHull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_ConcaveHull")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeValid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MakeValid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MakeValidDiscarded")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MakeValidDiscarded")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Segmentize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Segmentize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Split")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Split")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SplitLeft")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SplitLeft")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SplitRight")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SplitRight")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Azimuth")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Azimuth")) == 0)
    return true;
  if (str.CmpNoCase(wxT("Project")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Project")) == 0)
    return true;
  if (str.CmpNoCase(wxT("GeoHash")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_GeoHash")) == 0)
    return true;
  if (str.CmpNoCase(wxT("AsX3D")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_AsX3D")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_3DDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_3DMaxDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("MaxDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_MaxDistance")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SnapToGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_SnapToGrid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SridFromAuthCRS")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ShiftCoords")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ShiftCoordinates")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ST_Translate")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ScaleCoords")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ScaleCoordinates")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RotateCoords")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RotateCoordinates")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ReflectCoords")) == 0)
    return true;
  if (str.CmpNoCase(wxT("ReflectCoordinates")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SwapCoords")) == 0)
    return true;
  if (str.CmpNoCase(wxT("SwapCoordinates")) == 0)
    return true;
  if (str.CmpNoCase(wxT("FilterMbrWithin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("FilterMbrContains")) == 0)
    return true;
  if (str.CmpNoCase(wxT("FilterMbrIntersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("BuildMbrFilter")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RTreeWithin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RTreeContains")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RTreeIntersects")) == 0)
    return true;
  if (str.CmpNoCase(wxT("RTreeDistWithin")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_Create")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetPayload")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetDocument")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsValid")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_SchemaValidate")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsCompressed")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsIsoMetadata")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsSldSeVectorStyle")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsSldSeRasterStyle")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsSvg")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_Compress")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_Uncompress")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsSchemaValidated")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetSchemaURI")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetInternalSchemaURI")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetFileId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_SetFileId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_AddFileId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetParentId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_SetParentId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_AddParentId")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetTitle")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetAbstract")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetGeometry")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetDocumentSize")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetEncoding")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetLastParseError")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_IsValidXPathExpression")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetLastValidateError")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_GetLastXPathError")) == 0)
    return true;
  if (str.CmpNoCase(wxT("XB_CacheFlush")) == 0)
    return true;
  return false;
}

void MyQueryView::DoSqlSyntaxColor()
{
//
// evidencing a nice colored SQL syntax 
//
  IgnoreEvent = true;
  SqlCtrl->Hide();
  wxTextAttr normal_style(wxColour(128, 128, 128), wxColour(255, 255, 255),
                          wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                                 wxFONTWEIGHT_NORMAL));
  wxTextAttr sql_style(wxColour(0, 0, 255), wxColour(255, 255, 255),
                       wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_BOLD));
  wxTextAttr const_style(wxColour(255, 0, 255), wxColour(255, 255, 255),
                         wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                                wxFONTWEIGHT_NORMAL));
  wxTextAttr fnct_style(wxColour(192, 128, 0), wxColour(255, 255, 255),
                        wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                               wxFONTWEIGHT_BOLD));
  wxTextAttr bracket_style(wxColour(255, 0, 0), wxColour(192, 192, 192),
                           wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL,
                                  wxFONTWEIGHT_BOLD));
  wxString sql = SqlCtrl->GetValue();
// setting the base style
  SqlCtrl->SetStyle(0, sql.Len(), normal_style);
  wxString right = sql;
  int from;
  int to = 0;
  int i;
  char c;
  char next_c;
  SqlTokenizer tokenizer(sql);
  while (tokenizer.HasMoreTokens())
    {
      wxString token = tokenizer.GetNextToken();
      from = to + right.Find(token);
      to = from + token.Len();
      // extracting the unparsed portion of the SQL string
      right = sql.Mid(to);
      next_c = '\0';
      for (i = 0; i < (int) right.Len(); i++)
        {
          c = right.GetChar(i);
          if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
            continue;
          next_c = c;
          break;
        }
      char word[4096];
      strcpy(word, token.ToUTF8());
      if (gaiaIsReservedSqliteName(word))
        {
          // setting the SQL keyword style
          SqlCtrl->SetStyle(from, to, sql_style);
      } else if (IsSqliteExtra(token))
        {
          // setting the SQL keyword style
          SqlCtrl->SetStyle(from, to, sql_style);
      } else if (IsSqlString(token) == true)
        {
          // setting the SQL string constant style
          SqlCtrl->SetStyle(from, to, const_style);
      } else if (IsSqlNumber(token) == true)
        {
          // setting the SQL numeric constant style
          SqlCtrl->SetStyle(from, to, const_style);
      } else if (IsSqlFunction(token, next_c) == true)
        {
          // setting the SQL function style
          SqlCtrl->SetStyle(from, to, fnct_style);
      } else if (IsSqlGeoFunction(token, next_c) == true)
        {
          // setting the SQL geo-function style
          SqlCtrl->SetStyle(from, to, fnct_style);
        }
    }
  if (BracketStart >= 0)
    {
      // evidencing an opening bracket
      SqlCtrl->SetStyle(BracketStart, BracketStart + 1, bracket_style);
    }
  if (BracketEnd >= 0)
    {
      // evidencing a closing bracket
      SqlCtrl->SetStyle(BracketEnd, BracketEnd + 1, bracket_style);
    }
  SqlCtrl->Show();
  SqlCtrl->SetFocus();
  IgnoreEvent = false;
}

void MyQueryView::OnSqlSyntaxColor(wxCommandEvent & event)
{
//
// EVENT: updating the SQL syntax 
//
  if (IgnoreEvent == true)
    {
      // processing is still in progress; ignoring any internally generated call
      return;
    }
  event.Skip();
  EventBrackets();
}

void MyQueryView::EvidBrackets(int on, int off)
{
// evidencing corresponding brackets [open/close]
  BracketStart = -1;
  BracketEnd = -1;
  if (on >= 0)
    BracketStart = on;
  if (off >= 0)
    BracketEnd = off;
  DoSqlSyntaxColor();
}

void MyQueryView::EventBrackets()
{
//
// evidencing brackets [balancing open-close pairs] 
//
  if (IgnoreEvent == true)
    {
      // processing is still in progress; ignoring any internally generated call
      return;
    }
  int pos = SqlCtrl->GetInsertionPoint();
  int on;
  int off;
  wxString sql = SqlCtrl->GetValue();
  char pre = '\0';
  char post = '\0';
  if (pos > 0)
    pre = sql.GetChar(pos - 1);
  if (pos < (int) sql.Len())
    post = sql.GetChar(pos);
  if (post == '(')
    {
      // positioned before an opening bracket
      if (CheckBrackets(pos, false, &on, &off) == true)
        EvidBrackets(on, off);
      else
        EvidBrackets(pos, -1);
      return;
    }
  if (pre == ')')
    {
      // positioned after a closing bracket
      if (CheckBrackets(pos - 1, true, &on, &off) == true)
        EvidBrackets(on, off);
      else
        EvidBrackets(-1, pos - 1);
      return;
    }
  EvidBrackets(-1, -1);
}

bool MyQueryView::CheckBrackets(int pos, bool reverse_direction, int *on,
                                int *off)
{
// trying to balance a brackets pair [opening/closing]
  int i;
  int len;
  int level = 0;
  char c;
  int single_quoted = 0;
  int double_quoted = 0;
  wxString sql = SqlCtrl->GetValue();
  if (reverse_direction == true)
    {
      // going backward from CLOSE to OPEN
      for (i = pos - 1; i >= 0; i--)
        {
          c = sql.GetChar(i);
          if (c == '\'' && !double_quoted)
            {
              // single quoting start-stop
              if (single_quoted)
                single_quoted = 0;
              else
                single_quoted = 1;
            }
          if (c == '"' && !single_quoted)
            {
              // double quoting start-stop
              if (double_quoted)
                double_quoted = 0;
              else
                double_quoted = 1;
            }
          if (single_quoted || double_quoted)
            continue;
          if (c == ')')
            level++;
          if (c == '(')
            {
              if (level == 0)
                {
                  *on = i;
                  *off = pos;
                  return true;
                }
              level--;
            }
        }
  } else
    {
      // going forward from OPEN to CLOSE
      len = sql.Len();
      for (i = pos + 1; i < len; i++)
        {
          c = sql.GetChar(i);
          if (c == '\'' && !double_quoted)
            {
              // single quoting start-stop
              if (single_quoted)
                single_quoted = 0;
              else
                single_quoted = 1;
            }
          if (c == '"' && !single_quoted)
            {
              // double quoting start-stop
              if (double_quoted)
                double_quoted = 0;
              else
                double_quoted = 1;
            }
          if (single_quoted || double_quoted)
            continue;
          if (c == '(')
            level++;
          if (c == ')')
            {
              if (level == 0)
                {
                  *on = pos;
                  *off = i;
                  return true;
                }
              level--;
            }
        }
    }
  return false;
}

MySqlControl::MySqlControl(MyQueryView * parent, wxWindowID id, const wxString & value, const wxPoint & pos, const wxSize & size, long style):
wxTextCtrl(parent, id, value, pos, size,
           style)
{
//
// constructor: SQL text control
//
  Parent = parent;
  Connect(wxID_ANY, wxEVT_LEFT_DOWN,
          (wxObjectEventFunction) & MySqlControl::OnSqlMousePosition);
  Connect(wxID_ANY, wxEVT_KEY_UP,
          (wxObjectEventFunction) & MySqlControl::OnSqlArrowPosition);
}

void MySqlControl::OnSqlMousePosition(wxMouseEvent & event)
{
//
// intercepting mouse clicks
//
  if (Parent->IsIgnoreEvent() == true)
    return;
  event.Skip();
  Parent->EventBrackets();
}

void MySqlControl::OnSqlArrowPosition(wxKeyEvent & event)
{
//
// intercepting arrow keys
//
  if (Parent->IsIgnoreEvent() == true)
    return;
  event.Skip();
  int key_code = event.GetKeyCode();
  switch (key_code)
    {
      case WXK_DELETE:
      case WXK_HOME:
      case WXK_LEFT:
      case WXK_UP:
      case WXK_RIGHT:
      case WXK_DOWN:
      case WXK_PAGEUP:
      case WXK_PAGEDOWN:
      case WXK_NUMPAD_DELETE:
      case WXK_NUMPAD_HOME:
      case WXK_NUMPAD_LEFT:
      case WXK_NUMPAD_UP:
      case WXK_NUMPAD_RIGHT:
      case WXK_NUMPAD_DOWN:
      case WXK_NUMPAD_PAGEUP:
      case WXK_NUMPAD_PAGEDOWN:
        Parent->EventBrackets();
        break;
      default:
        break;
    };
}

SqlTokenizer::SqlTokenizer(wxString & sql)
{
// breaking tokens from an SQL expression
  Block = 1024;
  Max = Block;
  int i;
  char c;
  int single_quoted = 0;
  int double_quoted = 0;
  int white_space = 0;
  int start = -1;
  int len;
// initial allocation for the token list
  TokenList = new wxString *[Max];
  for (i = 0; i < Max; i++)
    TokenList[i] = NULL;
  Index = 0;
  for (i = 0; i < (int) sql.Len(); i++)
    {
      // scanning the SQL statement
      c = sql.GetChar(i);
      if (c == '\'' && !double_quoted)
        {
          if (single_quoted)
            {
              single_quoted = 0;
              len = i - start;
              len++;
              wxString *token = new wxString(sql.Mid(start, len));
              Insert(token);
              start = -1;
          } else
            {
              single_quoted = 1;
              start = i;
            }
          continue;
        }
      if (c == '"' && !single_quoted)
        {
          if (double_quoted)
            {
              double_quoted = 0;
              len = i - start;
              len++;
              wxString *token = new wxString(sql.Mid(start, len));
              Insert(token);
              start = -1;
          } else
            {
              double_quoted = 1;
              start = i;
            }
          continue;
        }
      if (single_quoted || double_quoted)
        continue;
      if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '('
          || c == ')' || c == ';' || c == ',')
        {
          if (white_space)
            continue;
          if (start >= 0)
            {
              // ok, we have a valid SQL token
              len = i - start;
              wxString *token = new wxString(sql.Mid(start, len));
              Insert(token);
            }
          start = -1;
          white_space = 1;
          continue;
        }
      white_space = 0;
      if (start < 0)
        start = i;
    }
  if (start >= 0)
    {
      // fetching the last token
      i = sql.Len();
      len = i - start;
      wxString *token = new wxString(sql.Mid(start, len));
      Insert(token);
    }
  Index = 0;
}

SqlTokenizer::~SqlTokenizer()
{
// destructor
  wxString *token;
  Index = 0;
  while (1)
    {
      token = TokenList[Index];
      if (token == NULL)
        break;
      delete token;
      Index++;
    }
  delete[]TokenList;
}

void SqlTokenizer::Expand()
{
// expanding the token list
  int newSize = Max + Block;
  int i;
  wxString **newList = new wxString *[newSize];
  for (i = 0; i < newSize; i++)
    newList[i] = NULL;
  for (i = 0; i < Max; i++)
    newList[i] = TokenList[i];
  delete[]TokenList;
  TokenList = newList;
  Max = newSize;
}

void SqlTokenizer::Insert(wxString * token)
{
// inserting a new token
  if (Index == (Max - 1))
    Expand();
  TokenList[Index++] = token;
}

bool SqlTokenizer::HasMoreTokens()
{
  wxString *token = TokenList[Index];
  if (token == NULL)
    return false;
  return true;
}

wxString & SqlTokenizer::GetNextToken()
{
// return the next token
  wxString *token = TokenList[Index];
  Index++;
  CurrentToken = *token;
  return CurrentToken;
}
