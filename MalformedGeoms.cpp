/*
/ MalformedGeoms.cpp
/ identifying malformed geometries
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
#include "wx/colordlg.h"

#if defined(_WIN32) || defined (__MINGW32__)
#define FORMAT_64	"%I64d"
#else
#define FORMAT_64	"%lld"
#endif

MalformedGeom::MalformedGeom(sqlite3_int64 rowid, int severity, bool canFix,
                             wxString & error)
{
//
// constructor
//
  RowId = rowid;
  Severity = severity;
  CanFix = canFix;
  Error = error;
  GeosMsg = wxT("");
  Next = NULL;
}

MalformedGeom::MalformedGeom(sqlite3_int64 rowid, int severity, bool canFix,
                             wxString & error, wxString & geosMsg)
{
//
// constructor
//
  RowId = rowid;
  Severity = severity;
  CanFix = canFix;
  Error = error;
  GeosMsg = geosMsg;
  Next = NULL;
}

MalformedGeomsList::~MalformedGeomsList()
{
//
// destructor
//
  MalformedGeom *pM;
  MalformedGeom *pMn;
  pM = First;
  while (pM)
    {
      pMn = pM->GetNext();
      delete pM;
      pM = pMn;
    }
}

void MalformedGeomsList::AddEntity(sqlite3_int64 rowid, int severity,
                                   bool canFix, wxString & error)
{
//
// inserting a malformed geometry into the list
//
  MalformedGeom *p = new MalformedGeom(rowid, severity, canFix, error);
  if (!First)
    First = p;
  if (Last)
    Last->SetNext(p);
  Last = p;
}

void MalformedGeomsList::AddEntity(sqlite3_int64 rowid, int severity,
                                   bool canFix, wxString & error,
                                   wxString & geosMsg)
{
//
// inserting a malformed geometry into the list
//
  MalformedGeom *p = new MalformedGeom(rowid, severity, canFix, error, geosMsg);
  if (!First)
    First = p;
  if (Last)
    Last->SetNext(p);
  Last = p;
}

void MyFrame::PreRepairPolygons(wxString & table,
                                wxString & geometry, int *count)
{
//
// identifying candidates for Repair Polygons
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  char *value;
  char xname[1024];

  *count = 0;
  sql = wxT("SELECT Count(*) FROM ");
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" WHERE ST_GeometryType(");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(") IN ('MULTIPOLYGON', 'MULTIPOLYGON Z', ");
  sql += wxT("'MULTIPOLYGON M', 'MULTIPOLYGON ZM') AND ");
  sql += wxT("ST_IsValid(");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(") = 0");
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
          *count = atoi(value);
        }
    }
  sqlite3_free_table(results);
}

void MyFrame::RepairPolygons(wxString & table, wxString & geometry, int *count)
{
//
// actual implementation for Repair Polygons
//
  int ret;
  char *errMsg = NULL;
  wxString sql;
  char xname[1024];

  *count = 0;
  sql = wxT("UPDATE ");
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" SET ");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" = CastToMulti(ST_BuildArea(ST_RingsCutAtNodes(");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("))) WHERE ST_GeometryType(");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(") IN ('MULTIPOLYGON', 'MULTIPOLYGON Z', ");
  sql += wxT("'MULTIPOLYGON M', 'MULTIPOLYGON ZM') AND ");
  sql += wxT("ST_IsValid(");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(") = 0");
  ret = sqlite3_exec(GetSqlite(), sql.ToUTF8(), NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  *count = sqlite3_changes(GetSqlite());
}

MalformedGeomsList *MyFrame::FindMalformedGeoms(wxString & table,
                                                wxString & geometry,
                                                bool allowRepair)
{
//
// identifying malformed geometries
//
  int ret;
  sqlite3_stmt *stmt;
  wxString errCause;
  wxString msg;
  char xname[1024];
  MalformedGeomsList *list = new MalformedGeomsList();
  wxString sql = wxT("SELECT ROWID, ");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" FROM ");
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  ret = sqlite3_prepare_v2(SqliteHandle, sql.ToUTF8(), sql.Len(), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      msg = wxT("SQL error: ");
      msg += errCause;
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return list;
    }

  ::wxBeginBusyCursor();

  while (1)
    {
      //
      // fetching the result set rows 
      //
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          bool ok_rowid = false;
          bool ok_geom = false;
          sqlite3_int64 int_value;
          unsigned char *blob;
          int blobSize;
          gaiaGeomCollPtr geom;
          if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
            {
              int_value = sqlite3_column_int64(stmt, 0);
              ok_rowid = true;
            }
          if (sqlite3_column_type(stmt, 1) == SQLITE_BLOB)
            {
              blob = (unsigned char *) sqlite3_column_blob(stmt, 1);
              blobSize = sqlite3_column_bytes(stmt, 1);
              geom = gaiaFromSpatiaLiteBlobWkb(blob, blobSize);
              if (geom)
                ok_geom = true;
            }
          if (ok_rowid == true && ok_geom == true)
            {
              // ok, we have a valid geometry ... going to check
              gaiaLinestringPtr ln;
              gaiaPolygonPtr pg;
              gaiaRingPtr rng;
              int iv;
              int ib;
              int points;
              double x;
              double y;
              double z;
              double m;
              double prev_x;
              double prev_y;
              double prev_z;
              double first_x;
              double first_y;
              double first_z;
              bool can_be_fixed = false;
              bool not_a_line = false;
              bool not_a_ring = false;
              bool unclosed_ring = false;
              bool repeated = false;
              ln = geom->FirstLinestring;
              while (ln)
                {
                  // checking LINESTRINGs
                  if (ln->Points < 2)
                    not_a_line = true;
                  else
                    {
                      for (iv = 0; iv < ln->Points; iv++)
                        {
                          z = 0.0;
                          m = 0.0;
                          if (ln->DimensionModel == GAIA_XY_Z)
                            {
                              gaiaGetPointXYZ(ln->Coords, iv, &x, &y, &z);
                          } else if (ln->DimensionModel == GAIA_XY_M)
                            {
                              gaiaGetPointXYM(ln->Coords, iv, &x, &y, &m);
                          } else if (ln->DimensionModel == GAIA_XY_Z_M)
                            {
                              gaiaGetPointXYZM(ln->Coords, iv, &x, &y, &z, &m);
                          } else
                            {
                              gaiaGetPoint(ln->Coords, iv, &x, &y);
                            }
                          if (iv > 0)
                            {
                              if (x == prev_x && y == prev_y && z == prev_z)
                                repeated = true;
                              else
                                points++;
                            }
                          if (iv == 0)
                            points = 1;
                          prev_x = x;
                          prev_y = y;
                          prev_z = z;
                        }
                      if (points >= 2)
                        can_be_fixed = true;
                    }
                  ln = ln->Next;
                }
              pg = geom->FirstPolygon;
              while (pg)
                {
                  // checking POLYGONs
                  rng = pg->Exterior;
                  if (rng->Points < 3)
                    not_a_ring = true;
                  else
                    {
                      for (iv = 0; iv < rng->Points; iv++)
                        {
                          z = 0.0;
                          m = 0.0;
                          if (rng->DimensionModel == GAIA_XY_Z)
                            {
                              gaiaGetPointXYZ(rng->Coords, iv, &x, &y, &z);
                          } else if (rng->DimensionModel == GAIA_XY_M)
                            {
                              gaiaGetPointXYM(rng->Coords, iv, &x, &y, &m);
                          } else if (rng->DimensionModel == GAIA_XY_Z_M)
                            {
                              gaiaGetPointXYZM(rng->Coords, iv, &x, &y, &z, &m);
                          } else
                            {
                              gaiaGetPoint(rng->Coords, iv, &x, &y);
                            }
                          if (iv > 0)
                            {
                              if (x == prev_x && y == prev_y && z == prev_z)
                                repeated = true;
                              else
                                points++;
                            }
                          if (iv == 0)
                            {
                              first_x = x;
                              first_y = y;
                              first_z = z;
                              points = 0;
                            }
                          prev_x = x;
                          prev_y = y;
                          prev_z = z;
                        }
                      if (first_x == prev_x && first_y == prev_y
                          && first_z == prev_z)
                        {
                          if (rng->Points == 3)
                            not_a_ring = true;
                      } else
                        unclosed_ring = true;
                      if (points >= 4)
                        can_be_fixed = true;
                    }
                  for (ib = 0; ib < pg->NumInteriors; ib++)
                    {
                      rng = pg->Interiors + ib;
                      if (rng->Points < 3)
                        not_a_ring = true;
                      else
                        {
                          for (iv = 0; iv < rng->Points; iv++)
                            {
                              z = 0.0;
                              m = 0.0;
                              if (rng->DimensionModel == GAIA_XY_Z)
                                {
                                  gaiaGetPointXYZ(rng->Coords, iv, &x, &y, &z);
                              } else if (rng->DimensionModel == GAIA_XY_M)
                                {
                                  gaiaGetPointXYM(rng->Coords, iv, &x, &y, &m);
                              } else if (rng->DimensionModel == GAIA_XY_Z_M)
                                {
                                  gaiaGetPointXYZM(rng->Coords, iv, &x, &y, &z,
                                                   &m);
                              } else
                                {
                                  gaiaGetPoint(rng->Coords, iv, &x, &y);
                                }
                              if (iv > 0)
                                {
                                  if (x == prev_x && y == prev_y && z == prev_z)
                                    repeated = true;
                                  else
                                    points++;
                                }
                              if (iv == 0)
                                {
                                  first_x = x;
                                  first_y = y;
                                  first_z = z;
                                  points = 1;
                                }
                              prev_x = x;
                              prev_y = y;
                              prev_z = z;
                            }
                          if (first_x == prev_x && first_y == prev_y
                              && first_z == prev_z)
                            {
                              if (rng->Points == 3)
                                not_a_ring = true;
                          } else
                            unclosed_ring = true;
                          if (points >= 4)
                            can_be_fixed = true;
                        }
                    }
                  pg = pg->Next;
                }
              if (allowRepair == false)
                can_be_fixed = false;
              if (not_a_line == true)
                {
                  msg = wxT("Not a valid Linestring [< 2 vertices]");
                  list->AddEntity(int_value, 2, false, msg);
                }
              if (not_a_ring == true)
                {
                  msg = wxT("Not a valid Ring [< 4 vertices]");
                  list->AddEntity(int_value, 2, false, msg);
                }
              if (unclosed_ring == true)
                {
                  msg = wxT("Unclosed Ring");
                  list->AddEntity(int_value, 2, true, msg);
                }
              if (repeated == true)
                {
                  msg = wxT("Repeated vertex");
                  list->AddEntity(int_value, 0, can_be_fixed, msg);
                }
              if (not_a_line == false && not_a_ring == false
                  && unclosed_ring == false)
                {
                  if (!gaiaIsValid(geom))
                    {
                      wxString geos_error =
                        wxString::FromUTF8(gaiaGetGeosErrorMsg());
                      if (geos_error.Len() == 0)
                        geos_error =
                          wxString::FromUTF8(gaiaGetGeosWarningMsg());
                      msg = wxT("GEOS invalid Geometry");
                      list->AddEntity(int_value, 1, false, msg, geos_error);
                  } else
                    {
                      wxString geos_warning =
                        wxString::FromUTF8(gaiaGetGeosWarningMsg());
                      if (geos_warning.Len() > 0)
                        {
                          msg = wxT("GEOS warning");
                          list->AddEntity(int_value, 1, false, msg,
                                          geos_warning);
                        }
                    }
                }
            }
          if (geom)
            gaiaFreeGeomColl(geom);

      } else
        {
          errCause = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          msg = wxT("SQL error: ");
          msg += errCause;
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto error;
        }
    }
error:
  sqlite3_finalize(stmt);
  ::wxEndBusyCursor();

  return list;
}

bool MalformedGeomsDialog::Create(MyFrame * parent, wxString & table,
                                  wxString & geom, MalformedGeomsList * list)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Geometry = geom;
  List = list;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Malformed Geometries")) == false)
    return false;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void MalformedGeomsDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  char dummy[128];
  wxString cell;
  MalformedGeom *pM;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// Table and GeometryColumn
  wxBoxSizer *headBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(headBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *headBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Spatial Table"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *myHeadSizer = new wxStaticBoxSizer(headBox, wxHORIZONTAL);
  headBoxSizer->Add(myHeadSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxBoxSizer *tblSizer = new wxBoxSizer(wxHORIZONTAL);
  myHeadSizer->Add(tblSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table:"));
  tblSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_ANY,
                                         Table, wxDefaultPosition,
                                         wxSize(200, 22), wxTE_READONLY);
  tblSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 2);
  wxBoxSizer *geoSizer = new wxBoxSizer(wxHORIZONTAL);
  myHeadSizer->Add(geoSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticText *geomLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Geometry:"));
  geoSizer->Add(geomLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxTextCtrl *geomCtrl = new wxTextCtrl(this, wxID_ANY,
                                        Geometry, wxDefaultPosition,
                                        wxSize(150, 22), wxTE_READONLY);
  geoSizer->Add(geomCtrl, 0, wxALIGN_RIGHT | wxALL, 2);

  wxBoxSizer *statSizer = new wxBoxSizer(wxHORIZONTAL);
  myHeadSizer->Add(statSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticText *statLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Status:"));
  statSizer->Add(statLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxString tableStatus = wxT("clean");
  wxColour backStatus(255, 255, 255);
  int maxSeverity = -1;
  pM = List->GetFirst();
  while (pM)
    {
      switch (pM->GetSeverity())
        {
          case 0:
            if (maxSeverity < 0)
              {
                tableStatus = wxT("Notice");
                backStatus = wxColour(192, 255, 192);
                maxSeverity = 0;
              }
            break;
          case 1:
            if (maxSeverity < 1)
              {
                tableStatus = wxT("Warning");
                backStatus = wxColour(255, 255, 192);
                maxSeverity = 1;
              }
            break;
          case 2:
            if (maxSeverity < 2)
              {
                tableStatus = wxT("Error");
                backStatus = wxColour(255, 192, 192);
                maxSeverity = 2;
              }
            break;
        };
      pM = pM->GetNext();
    }
  wxTextCtrl *statCtrl = new wxTextCtrl(this, wxID_ANY,
                                        tableStatus, wxDefaultPosition,
                                        wxSize(60, 22), wxTE_READONLY);
  statCtrl->SetBackgroundColour(backStatus);
  statSizer->Add(statCtrl, 0, wxALIGN_RIGHT | wxALL, 2);
// a GRID to show results
  wxBoxSizer *gridBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(gridBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *gridBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Malformed Geometries"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *myGridSizer = new wxStaticBoxSizer(gridBox, wxVERTICAL);
  gridBoxSizer->Add(myGridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *gridSizer = new wxBoxSizer(wxHORIZONTAL);
  myGridSizer->Add(gridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  GridCtrl =
    new wxGrid(this, ID_MALFORMED_GRID, wxDefaultPosition, wxSize(550, 300));
  int count = 0;
  pM = List->GetFirst();
  while (pM)
    {
      // counting how many lines are there
      count++;
      pM = pM->GetNext();
    }
  GridCtrl->CreateGrid(count, 5);
  GridCtrl->SetColLabelValue(0, wxT("ROWID"));
  GridCtrl->SetColLabelValue(1, wxT("Severity"));
  GridCtrl->SetColLabelValue(2, wxT("Repair"));
  GridCtrl->SetColLabelValue(3, wxT("Description"));
  GridCtrl->SetColLabelValue(4, wxT("GEOS report"));
  GridCtrl->SetRowLabelValue(0, wxT("1"));
  count = 0;
  pM = List->GetFirst();
  while (pM)
    {
      // feeding grid rows
      sprintf(dummy, "%d", count + 1);
      cell = wxString::FromUTF8(dummy);
      GridCtrl->SetRowLabelValue(count, cell);
      sprintf(dummy, FORMAT_64, pM->GetRowId());
      cell = wxString::FromUTF8(dummy);
      GridCtrl->SetCellValue(count, 0, cell);
      switch (pM->GetSeverity())
        {
          case 0:
            GridCtrl->SetCellValue(count, 1, wxT("Notice"));
            if (pM->CanBeFixed() == true)
              {
                GridCtrl->SetCellValue(count, 2, wxT("Yes"));
                GridCtrl->SetCellBackgroundColour(count, 2,
                                                  wxColour(192, 255, 192));
              }
            break;
          case 1:
            GridCtrl->SetCellValue(count, 1, wxT("Warning"));
            GridCtrl->SetCellBackgroundColour(count, 0,
                                              wxColour(255, 255, 192));
            GridCtrl->SetCellBackgroundColour(count, 1,
                                              wxColour(255, 255, 192));
            if (pM->CanBeFixed() == true)
              {
                GridCtrl->SetCellValue(count, 2, wxT("Yes"));
                GridCtrl->SetCellBackgroundColour(count, 2,
                                                  wxColour(192, 255, 192));
              }
            GridCtrl->SetCellBackgroundColour(count, 3,
                                              wxColour(255, 255, 192));
            GridCtrl->SetCellBackgroundColour(count, 4,
                                              wxColour(255, 255, 192));
            break;
          case 2:
            GridCtrl->SetCellValue(count, 1, wxT("Error"));
            GridCtrl->SetCellBackgroundColour(count, 0,
                                              wxColour(255, 192, 192));
            GridCtrl->SetCellBackgroundColour(count, 1,
                                              wxColour(255, 192, 192));
            if (pM->CanBeFixed() == true)
              {
                GridCtrl->SetCellValue(count, 2, wxT("Yes"));
                GridCtrl->SetCellBackgroundColour(count, 2,
                                                  wxColour(192, 255, 192));
              }
            GridCtrl->SetCellBackgroundColour(count, 3,
                                              wxColour(255, 192, 192));
            GridCtrl->SetCellBackgroundColour(count, 4,
                                              wxColour(255, 192, 192));
            break;
        };
      GridCtrl->SetCellValue(count, 3, pM->GetError());
      GridCtrl->SetCellValue(count, 4, pM->GetGeosMsg());
      count++;
      pM = pM->GetNext();
    }
  GridCtrl->SetRowLabelSize(wxGRID_AUTOSIZE);
  GridCtrl->AutoSize();
  GridCtrl->EnableEditing(false);
  gridSizer->Add(GridCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// CLOSE button
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *close = new wxButton(this, ID_MALFORMED_CLOSE, wxT("&Close"));
  okCancelBox->Add(close, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  okCancelBox->AddSpacer(20);
  okCancelBox->AddSpacer(20);
  okCancelBox->AddSpacer(20);
  bool can_repair = false;
  pM = List->GetFirst();
  while (pM)
    {
      if (pM->CanBeFixed() == true)
        can_repair = true;
      pM = pM->GetNext();
    }
  wxButton *repair = new wxButton(this, ID_MALFORMED_REPAIR, wxT("&Repair"));
  repair->Enable(can_repair);
  okCancelBox->Add(repair, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(ID_MALFORMED_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnClose);
  Connect(ID_MALFORMED_REPAIR, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnRepair);
  Connect(ID_MALFORMED_GRID, wxEVT_GRID_CELL_RIGHT_CLICK,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnRightClick);
  Connect(ID_MALFORMED_CLEAR, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdClearSelection);
  Connect(ID_MALFORMED_ALL, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdSelectAll);
  Connect(ID_MALFORMED_ROW, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdSelectRow);
  Connect(ID_MALFORMED_COLUMN, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdSelectColumn);
  Connect(ID_MALFORMED_COPY, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdCopy);
  Connect(ID_MALFORMED_BLOB, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MalformedGeomsDialog::OnCmdBlob);
}

void MalformedGeomsDialog::OnRightClick(wxGridEvent & event)
{
//
// right click on some cell [mouse action]
//
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxPoint pt = event.GetPosition();
  CurrentEvtRow = event.GetRow();
  CurrentEvtColumn = event.GetCol();
  menuItem = new wxMenuItem(menu, ID_MALFORMED_BLOB, wxT("BLOB &explore"));
  menu->Append(menuItem);
  menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, ID_MALFORMED_CLEAR, wxT("&Clear selection"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_MALFORMED_ALL, wxT("Select &all"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_MALFORMED_ROW, wxT("Select &row"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_MALFORMED_COLUMN, wxT("&Select column"));
  menu->Append(menuItem);
  menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, ID_MALFORMED_COPY, wxT("&Copy"));
  menu->Append(menuItem);
  GridCtrl->PopupMenu(menu, pt);
}

void MalformedGeomsDialog::OnCmdBlob(wxCommandEvent & WXUNUSED(event))
{
//
// exploring some BLOB value
//
  int ret;
  sqlite3_stmt *stmt;
  wxString errCause;
  wxString msg;
  unsigned char *blob = NULL;
  unsigned char *db_blob;
  int blobSize = 0;
  char xname[1024];
  wxString value = GridCtrl->GetCellValue(CurrentEvtRow, 0);
  wxString sql = wxT("SELECT ");
  strcpy(xname, Geometry.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" FROM ");
  strcpy(xname, Table.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" WHERE ROWID = ");
  sql += value;
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), sql.ToUTF8(), sql.Len(), &stmt,
                       NULL);
  if (ret != SQLITE_OK)
    {
      errCause = wxString::FromUTF8(sqlite3_errmsg(MainFrame->GetSqlite()));
      msg = wxT("SQL error: ");
      msg += errCause;
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }

  while (1)
    {
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          if (sqlite3_column_type(stmt, 0) == SQLITE_BLOB)
            {
              db_blob = (unsigned char *) sqlite3_column_blob(stmt, 0);
              blobSize = sqlite3_column_bytes(stmt, 0);
              blob = new unsigned char[blobSize];
              memcpy(blob, db_blob, blobSize);
            }

      } else
        {
          errCause = wxString::FromUTF8(sqlite3_errmsg(MainFrame->GetSqlite()));
          msg = wxT("SQL error: ");
          msg += errCause;
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto error;
        }
    }
error:
  sqlite3_finalize(stmt);

  if (!blob)
    return;
  BlobExplorerDialog dlg;
  dlg.Create(MainFrame, blobSize, blob);
  dlg.ShowModal();
  delete[]blob;
}

void MalformedGeomsDialog::OnCmdClearSelection(wxCommandEvent & WXUNUSED(event))
{
//
// clearing current selection
//
  GridCtrl->ClearSelection();
}

void MalformedGeomsDialog::OnCmdSelectAll(wxCommandEvent & WXUNUSED(event))
{
//
// selecting all
//
  GridCtrl->SelectAll();
}

void MalformedGeomsDialog::OnCmdSelectRow(wxCommandEvent & WXUNUSED(event))
{
//
// selecting the current row
//
  GridCtrl->SelectRow(CurrentEvtRow);
}

void MalformedGeomsDialog::OnCmdSelectColumn(wxCommandEvent & WXUNUSED(event))
{
//
// selecting column
//
  GridCtrl->SelectCol(CurrentEvtColumn);

}

void MalformedGeomsDialog::OnCmdCopy(wxCommandEvent & WXUNUSED(event))
{
//
// copying the selection into the clipboard
//
  wxString copyData;
  int row;
  int col;
  bool newRow;
  bool firstRow = true;
  for (row = 0; row < GridCtrl->GetNumberRows(); row++)
    {
      newRow = true;
      for (col = 0; col < GridCtrl->GetNumberCols(); col++)
        {
          if (GridCtrl->IsInSelection(row, col) == true)
            {
              // ok, this cell is included into the selection to copy
              if (firstRow == true)
                {
                  newRow = false;
                  firstRow = false;
              } else if (newRow == true)
                {
                  newRow = false;
                  copyData += wxT("\n");
              } else
                copyData += wxT("\t");
              copyData += GridCtrl->GetCellValue(row, col);
            }
        }
    }
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(copyData));
      wxTheClipboard->Close();
    }
}

void MalformedGeomsDialog::OnRepair(wxCommandEvent & WXUNUSED(event))
{
//
// repairing malformed geometries
//
  int ret;
  char *errMsg = NULL;
  sqlite3_stmt *stmt;
  wxString errCause;
  wxString msg;
  wxString sql;
  MalformedGeom *pM;
  char xname[1024];

  ::wxBeginBusyCursor();

  ret = sqlite3_exec(MainFrame->GetSqlite(), "BEGIN", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQL error:") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto clean_up;
    }

  sql = wxT("UPDATE ");
  strcpy(xname, Table.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" SET ");
  strcpy(xname, Geometry.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" = SanitizeGeometry(");
  strcpy(xname, Geometry.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(") WHERE ROWID = ?");

  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), sql.ToUTF8(), sql.Len(), &stmt,
                       NULL);
  if (ret != SQLITE_OK)
    {
      errCause = wxString::FromUTF8(sqlite3_errmsg(MainFrame->GetSqlite()));
      msg = wxT("SQL error: ");
      msg += errCause;
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      goto clean_up;
    }

  pM = List->GetFirst();
  while (pM)
    {
      if (pM->CanBeFixed() == true)
        {
          // binding query params
          sqlite3_reset(stmt);
          sqlite3_clear_bindings(stmt);
          sqlite3_bind_int64(stmt, 1, pM->GetRowId());
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE || ret == SQLITE_ROW)
            ;
          else
            {
              wxString err =
                wxString::FromUTF8(sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQL error:") + err, wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_finalize(stmt);
              goto clean_up;
            }
        }
      pM = pM->GetNext();
    }

  sqlite3_finalize(stmt);

  ret = sqlite3_exec(MainFrame->GetSqlite(), "COMMIT", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQL error:") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto clean_up;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT("Repair completed succesfully"),
               wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
  wxDialog::EndModal(wxID_OK);
  return;
clean_up:
  ret = sqlite3_exec(MainFrame->GetSqlite(), "EOLLBACK", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQL error:") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("Repair aborted\n\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
}

void MalformedGeomsDialog::OnClose(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

DbStatusDialog::DbStatusDialog()
{
// constructor
  MainFrame = NULL;
  DynamicIds = NULL;
  DynamicModes = NULL;
  RefreshTimer = NULL;
}

DbStatusDialog::~DbStatusDialog()
{
// destructor
  if (RefreshTimer)
    {
      RefreshTimer->Stop();
      delete RefreshTimer;
    }
  if (DynamicIds)
    delete[]DynamicIds;
  if (DynamicModes)
    delete[]DynamicModes;
}

bool DbStatusDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("DB Status Infos")) == false)
    return false;
  DynamicIds = new int[20];
  DynamicModes = new int[20];
  for (int i = 0; i < 20; i++)
    {
      DynamicIds[i] = -1;
      DynamicModes[i] = ModeNone;
    }
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  RefreshTimer = new wxTimer(this, ID_DB_STATUS_TIMER);
  RefreshTimer->Start(250, wxTIMER_ONE_SHOT);
  return true;
}

void DbStatusDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  sqlite3 *sqlite = MainFrame->GetSqlite();
  char dummy[128];
  int iCur;
  int iHiwtr;
  wxString cell;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// a GRID to show results
  wxBoxSizer *gridBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(gridBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *gridBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("DB Status Infos"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *myGridSizer = new wxStaticBoxSizer(gridBox, wxVERTICAL);
  gridBoxSizer->Add(myGridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *gridSizer = new wxBoxSizer(wxHORIZONTAL);
  myGridSizer->Add(gridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  GridCtrl =
    new wxGrid(this, ID_DB_STATUS_GRID, wxDefaultPosition, wxSize(630, 400));
  GridCtrl->CreateGrid(20, 4);
  GridCtrl->SetColLabelValue(0, wxT(""));
  GridCtrl->SetColLabelValue(1, wxT("Current"));
  GridCtrl->SetColLabelValue(2, wxT("Max"));
  GridCtrl->SetColLabelValue(3, wxT("Notes"));

  GridCtrl->SetRowLabelValue(0, wxT("1"));
  GridCtrl->SetCellValue(0, 0, wxT("Memory Used"));
  DynamicModes[0] = ModeStatusBothBytes;
  DynamicIds[0] = SQLITE_STATUS_MEMORY_USED;
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_MEMORY_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(0, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 0, 1);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(0, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 0, 2);
  GridCtrl->SetCellValue(0, 3,
                         wxT
                         ("amount of memory checked out using sqlite3_malloc()"));
  GridCtrl->SetCellBackgroundColour(0, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(1, wxT("2"));
  DynamicModes[1] = ModeStatusSecondBytes;
  DynamicIds[1] = SQLITE_STATUS_MALLOC_SIZE;
  GridCtrl->SetCellValue(1, 0, wxT("Largest Allocation"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_MALLOC_SIZE, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(1, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 1, 2);
  GridCtrl->SetCellValue(1, 3, wxT("largest memory allocation request handed"));
  GridCtrl->SetCellBackgroundColour(1, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(2, wxT("3"));
  DynamicModes[2] = ModeStatusBoth;
  DynamicIds[2] = SQLITE_STATUS_MALLOC_COUNT;
  GridCtrl->SetCellValue(2, 0, wxT("Number of Outstanding Allocations"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_MALLOC_COUNT, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(2, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 2, 1);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(2, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 2, 2);
  GridCtrl->SetCellValue(2, 3,
                         wxT
                         ("number of separate memory allocations currently checked out"));
  GridCtrl->SetCellBackgroundColour(2, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(3, wxT("4"));
  DynamicModes[3] = ModeStatusBoth;
  DynamicIds[3] = SQLITE_STATUS_PAGECACHE_USED;
  GridCtrl->SetCellValue(3, 0, wxT("Number of Page Cache Pages Used"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_PAGECACHE_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(3, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 3, 1);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(3, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 3, 2);
  GridCtrl->SetCellValue(3, 3,
                         wxT
                         ("number of pages used out of the pagecache memory allocato"));
  GridCtrl->SetCellBackgroundColour(3, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(4, wxT("5"));
  DynamicModes[4] = ModeStatusBothBytes;
  DynamicIds[4] = SQLITE_STATUS_PAGECACHE_OVERFLOW;
  GridCtrl->SetCellValue(4, 0, wxT("Number of Page Cache Overflow Bytes"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_PAGECACHE_OVERFLOW, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(4, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 4, 1);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(4, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 4, 2);
  GridCtrl->SetCellValue(4, 3,
                         wxT
                         ("page cache allocations which could not be satisfied and were forced to overflow"));
  GridCtrl->SetCellBackgroundColour(4, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(5, wxT("6"));
  DynamicModes[5] = ModeStatusSecondBytes;
  DynamicIds[5] = SQLITE_STATUS_PAGECACHE_SIZE;
  GridCtrl->SetCellValue(5, 0, wxT("Largest Page Cache Allocation"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_PAGECACHE_SIZE, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(5, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 5, 2);
  GridCtrl->SetCellValue(5, 3,
                         wxT
                         ("largest memory allocation request handed to pagecache memory allocator"));
  GridCtrl->SetCellBackgroundColour(5, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(6, wxT("7"));
  DynamicModes[6] = ModeStatusBothBytes;
  DynamicIds[6] = SQLITE_STATUS_SCRATCH_USED;
  GridCtrl->SetCellValue(6, 0, wxT("Number of Scratch Allocations Used"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_SCRATCH_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(6, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 6, 1);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(6, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 6, 2);
  GridCtrl->SetCellValue(6, 3,
                         wxT
                         ("number of allocations used out of the scratch memory allocator"));
  GridCtrl->SetCellBackgroundColour(6, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(7, wxT("8"));
  DynamicModes[7] = ModeStatusBothBytes;
  DynamicIds[7] = SQLITE_STATUS_SCRATCH_OVERFLOW;
  GridCtrl->SetCellValue(7, 0, wxT("Number of Scratch Overflow Bytes"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_SCRATCH_OVERFLOW, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(7, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 7, 1);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(7, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 7, 2);
  GridCtrl->SetCellValue(7, 3,
                         wxT
                         ("scratch memory allocations which could not be satisfied and were forced to overflow"));
  GridCtrl->SetCellBackgroundColour(7, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(8, wxT("9"));
  DynamicModes[8] = ModeStatusSecondBytes;
  DynamicIds[8] = SQLITE_STATUS_SCRATCH_SIZE;
  GridCtrl->SetCellValue(8, 0, wxT("Largest Scratch Allocation"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_SCRATCH_SIZE, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(8, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 8, 2);
  GridCtrl->SetCellValue(8, 3,
                         wxT
                         ("largest memory allocation request handed to scratch memory allocator"));
  GridCtrl->SetCellBackgroundColour(8, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(9, wxT("10"));
  DynamicModes[9] = ModeStatusBoth;
  DynamicIds[9] = SQLITE_STATUS_PARSER_STACK;
  GridCtrl->SetCellValue(9, 0, wxT("Deepest Parser Stack"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_PARSER_STACK, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(9, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 9, 1);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(9, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 9, 2);
  GridCtrl->SetCellValue(9, 3, wxT("deepest parser stack"));
  GridCtrl->SetCellBackgroundColour(9, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(10, wxT("11"));
  GridCtrl->SetCellValue(10, 0, wxT("Lookaside Slots Used"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_USED
  DynamicModes[10] = ModeDbStatusBoth;
  DynamicIds[10] = SQLITE_DBSTATUS_LOOKASIDE_USED;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(10, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 10, 1);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(10, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 10, 2);
#else
  GridCtrl->SetCellValue(10, 1, wxT("n.a."));
  GridCtrl->SetCellValue(10, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(10, 3,
                         wxT
                         ("number of lookaside memory slots currently checked out"));
  GridCtrl->SetCellBackgroundColour(10, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(11, wxT("12"));
  GridCtrl->SetCellValue(11, 0, wxT("Successful lookaside attempts"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_HIT
  DynamicModes[11] = ModeDbStatusSecond;
  DynamicIds[11] = SQLITE_DBSTATUS_LOOKASIDE_HIT;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_HIT, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(11, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 11, 2);
#else
  GridCtrl->SetCellValue(11, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(11, 3,
                         wxT
                         ("number malloc attempts that were satisfied using lookaside memory"));
  GridCtrl->SetCellBackgroundColour(11, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(12, wxT("13"));
  GridCtrl->SetCellValue(12, 0, wxT("Lookaside failures due to size"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
  DynamicModes[12] = ModeDbStatusSecond;
  DynamicIds[12] = SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE, &iCur, &iHiwtr,
                    0);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(12, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 12, 2);
#else
  GridCtrl->SetCellValue(12, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(12, 3,
                         wxT
                         ("malloc attempts failed due to the amount larger than the lookaside slot size"));
  GridCtrl->SetCellBackgroundColour(12, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(13, wxT("14"));
  GridCtrl->SetCellValue(13, 0, wxT("Lookaside failures due to OOM"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
  DynamicModes[13] = ModeDbStatusSecond;
  DynamicIds[13] = SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL, &iCur, &iHiwtr,
                    0);
  sprintf(dummy, "%d", iHiwtr);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(13, 2, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 13, 2);
#else
  GridCtrl->SetCellValue(13, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(13, 3,
                         wxT
                         ("malloc attempts failed due to all lookaside memory already being in use"));
  GridCtrl->SetCellBackgroundColour(13, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(14, wxT("15"));
  DynamicModes[14] = ModeStatusFirst;
  DynamicIds[14] = SQLITE_STATUS_PARSER_STACK;
  GridCtrl->SetCellValue(14, 0, wxT("Pager Heap Usage"));
  iHiwtr = iCur = -1;
  sqlite3_status(SQLITE_STATUS_PARSER_STACK, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(14, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 14, 1);
  GridCtrl->SetCellValue(14, 3,
                         wxT
                         ("heap memory used by all pager caches associated with the database connection"));
  GridCtrl->SetCellBackgroundColour(14, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(15, wxT("16"));
  GridCtrl->SetCellValue(15, 0, wxT("Schema Heap Usage"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_SCHEMA_USED
  DynamicModes[15] = ModeDbStatusFirstBytes;
  DynamicIds[15] = SQLITE_DBSTATUS_SCHEMA_USED;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_SCHEMA_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(15, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 15, 1);
#else
  GridCtrl->SetCellValue(15, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(15, 3,
                         wxT
                         ("heap memory used to store the schema for all databases associated with the connection"));
  GridCtrl->SetCellBackgroundColour(15, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(16, wxT("17"));
  GridCtrl->SetCellValue(16, 0, wxT("Statement Heap/Lookaside Usage"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_STMT_USED
  DynamicModes[16] = ModeDbStatusFirstBytes;
  DynamicIds[16] = SQLITE_DBSTATUS_STMT_USED;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_STMT_USED, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d bytes", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(16, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 16, 1);
#else
  GridCtrl->SetCellValue(16, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(16, 3,
                         wxT
                         ("heap and lookaside memory used by all prepared statements associated with the database connection"));
  GridCtrl->SetCellBackgroundColour(16, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(17, wxT("18"));
  GridCtrl->SetCellValue(17, 0, wxT("Page cache hits"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_HIT
  DynamicModes[17] = ModeDbStatusFirst;
  DynamicIds[17] = SQLITE_DBSTATUS_CACHE_HIT;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_HIT, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(17, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 17, 1);
#else
  GridCtrl->SetCellValue(17, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(17, 3,
                         wxT("number of pager cache hits that have occurred"));
  GridCtrl->SetCellBackgroundColour(17, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(18, wxT("19"));
  GridCtrl->SetCellValue(18, 0, wxT("Page cache misses"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_MISS
  DynamicModes[18] = ModeDbStatusFirst;
  DynamicIds[18] = SQLITE_DBSTATUS_CACHE_MISS;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_MISS, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(18, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 18, 1);
#else
  GridCtrl->SetCellValue(18, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(18, 3,
                         wxT
                         ("number of pager cache misses that have occurred"));
  GridCtrl->SetCellBackgroundColour(18, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelValue(19, wxT("20"));
  GridCtrl->SetCellValue(19, 0, wxT("Page cache writes"));
  iHiwtr = iCur = -1;
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_WRITE
  DynamicModes[19] = ModeDbStatusFirst;
  DynamicIds[19] = SQLITE_DBSTATUS_CACHE_WRITE;
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_WRITE, &iCur, &iHiwtr, 0);
  sprintf(dummy, "%d", iCur);
  cell = wxString::FromUTF8(dummy);
  GridCtrl->SetCellValue(19, 1, cell);
  GridCtrl->SetCellAlignment(wxALIGN_RIGHT, 19, 1);
#else
  GridCtrl->SetCellValue(19, 2, wxT("n.a."));
#endif
  GridCtrl->SetCellValue(19, 3,
                         wxT
                         ("number of dirty cache entries that have been written to disk"));
  GridCtrl->SetCellBackgroundColour(19, 3, wxColour(208, 208, 208));

  GridCtrl->SetRowLabelSize(wxGRID_AUTOSIZE);
  GridCtrl->AutoSize();
  GridCtrl->EnableEditing(false);
  gridSizer->Add(GridCtrl, 0, wxALIGN_RIGHT | wxALL, 0);

// CLOSE button
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *close = new wxButton(this, ID_DB_STATUS_CLOSE, wxT("&Close"));
  okCancelBox->Add(close, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  okCancelBox->AddSpacer(20);
  okCancelBox->AddSpacer(20);
  okCancelBox->AddSpacer(20);
  wxButton *reset =
    new wxButton(this, ID_DB_STATUS_RESET, wxT("&Reset Statistics"));
  okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// appends event handler for OK button
  Connect(ID_DB_STATUS_CLOSE, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DbStatusDialog::OnClose);
  Connect(ID_DB_STATUS_RESET, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DbStatusDialog::OnReset);
  Connect(ID_DB_STATUS_GRID, wxEVT_GRID_CELL_RIGHT_CLICK,
          (wxObjectEventFunction) & DbStatusDialog::OnRightClick);
  Connect(ID_DB_STATUS_CLEAR, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & DbStatusDialog::OnCmdClearSelection);
  Connect(ID_DB_STATUS_ALL, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & DbStatusDialog::OnCmdSelectAll);
  Connect(ID_DB_STATUS_ROW, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & DbStatusDialog::OnCmdSelectRow);
  Connect(ID_DB_STATUS_COLUMN, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & DbStatusDialog::OnCmdSelectColumn);
  Connect(ID_DB_STATUS_COPY, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & DbStatusDialog::OnCmdCopy);
//
// setting up a Timer event handler for Refresh
//
  Connect(ID_DB_STATUS_TIMER, wxEVT_TIMER,
          wxTimerEventHandler(DbStatusDialog::OnRefreshTimer), NULL, this);
}

void DbStatusDialog::OnRightClick(wxGridEvent & event)
{
//
// right click on some cell [mouse action]
//
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxPoint pt = event.GetPosition();
  CurrentEvtRow = event.GetRow();
  CurrentEvtColumn = event.GetCol();
  menuItem = new wxMenuItem(menu, ID_DB_STATUS_CLEAR, wxT("&Clear selection"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_DB_STATUS_ALL, wxT("Select &all"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_DB_STATUS_ROW, wxT("Select &row"));
  menu->Append(menuItem);
  menuItem = new wxMenuItem(menu, ID_DB_STATUS_COLUMN, wxT("&Select column"));
  menu->Append(menuItem);
  menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, ID_DB_STATUS_COPY, wxT("&Copy"));
  menu->Append(menuItem);
  GridCtrl->PopupMenu(menu, pt);
}

void DbStatusDialog::OnCmdClearSelection(wxCommandEvent & WXUNUSED(event))
{
//
// clearing current selection
//
  GridCtrl->ClearSelection();
}

void DbStatusDialog::OnCmdSelectAll(wxCommandEvent & WXUNUSED(event))
{
//
// selecting all
//
  GridCtrl->SelectAll();
}

void DbStatusDialog::OnCmdSelectRow(wxCommandEvent & WXUNUSED(event))
{
//
// selecting the current row
//
  GridCtrl->SelectRow(CurrentEvtRow);
}

void DbStatusDialog::OnCmdSelectColumn(wxCommandEvent & WXUNUSED(event))
{
//
// selecting column
//
  GridCtrl->SelectCol(CurrentEvtColumn);
}

void DbStatusDialog::OnCmdCopy(wxCommandEvent & WXUNUSED(event))
{
//
// copying the selection into the clipboard
//
  wxString copyData;
  int row;
  int col;
  bool newRow;
  bool firstRow = true;
  for (row = 0; row < GridCtrl->GetNumberRows(); row++)
    {
      newRow = true;
      for (col = 0; col < GridCtrl->GetNumberCols(); col++)
        {
          if (GridCtrl->IsInSelection(row, col) == true)
            {
              // ok, this cell is included into the selection to copy
              if (firstRow == true)
                {
                  newRow = false;
                  firstRow = false;
              } else if (newRow == true)
                {
                  newRow = false;
                  copyData += wxT("\n");
              } else
                copyData += wxT("\t");
              copyData += GridCtrl->GetCellValue(row, col);
            }
        }
    }
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(copyData));
      wxTheClipboard->Close();
    }
}

void DbStatusDialog::OnReset(wxCommandEvent & WXUNUSED(event))
{
//
// resetting the DB Status counters
//
  sqlite3 *sqlite = MainFrame->GetSqlite();
  int iCur;
  int iHiwtr;
  sqlite3_status(SQLITE_STATUS_MEMORY_USED, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_MALLOC_SIZE, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_MALLOC_COUNT, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_PAGECACHE_USED, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_PAGECACHE_OVERFLOW, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_PAGECACHE_SIZE, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_SCRATCH_USED, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_SCRATCH_OVERFLOW, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_SCRATCH_SIZE, &iCur, &iHiwtr, 1);
  sqlite3_status(SQLITE_STATUS_PARSER_STACK, &iCur, &iHiwtr, 1);
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_USED
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_USED, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_HIT
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_HIT, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_MISS_SIZE, &iCur, &iHiwtr,
                    1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_LOOKASIDE_MISS_FULL, &iCur, &iHiwtr,
                    1);
#endif
  sqlite3_db_status(sqlite, SQLITE_STATUS_PARSER_STACK, &iCur, &iHiwtr, 1);
#ifdef HAVE_DECL_SQLITE_DBSTATUS_SCHEMA_USED
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_SCHEMA_USED, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_STMT_USED
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_STMT_USED, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_HIT
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_HIT, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_MISS
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_MISS, &iCur, &iHiwtr, 1);
#endif
#ifdef HAVE_DECL_SQLITE_DBSTATUS_CACHE_WRITE
  sqlite3_db_status(sqlite, SQLITE_DBSTATUS_CACHE_WRITE, &iCur, &iHiwtr, 1);
#endif
}

void DbStatusDialog::OnRefreshTimer(wxTimerEvent & WXUNUSED(event))
{
//
// Refreshing the Graph and the Statistics
//
  sqlite3 *sqlite = MainFrame->GetSqlite();
  int iCur;
  int iHiwtr;
  char dummy[128];
  wxString cell;
  for (int i = 0; i < 20; i++)
    {
      // updating Statistics
      if (DynamicIds[i] > 0)
        {
          switch (DynamicModes[i])
            {
              case ModeStatusBoth:
              case ModeStatusFirst:
              case ModeStatusSecond:
              case ModeStatusBothBytes:
              case ModeStatusFirstBytes:
              case ModeStatusSecondBytes:
                sqlite3_status(DynamicIds[i], &iCur, &iHiwtr, 0);
                break;
              case ModeDbStatusBoth:
              case ModeDbStatusFirst:
              case ModeDbStatusSecond:
              case ModeDbStatusBothBytes:
              case ModeDbStatusFirstBytes:
              case ModeDbStatusSecondBytes:
                sqlite3_db_status(sqlite, DynamicIds[i], &iCur, &iHiwtr, 0);
                break;
            };
          switch (DynamicModes[i])
            {
              case ModeStatusBoth:
              case ModeDbStatusBoth:
                sprintf(dummy, "%d", iCur);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 1, cell);
                sprintf(dummy, "%d", iHiwtr);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 2, cell);
                break;
              case ModeStatusFirst:
              case ModeDbStatusFirst:
                sprintf(dummy, "%d", iCur);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 1, cell);
                break;
              case ModeStatusSecond:
              case ModeDbStatusSecond:
                sprintf(dummy, "%d", iHiwtr);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 2, cell);
                break;
              case ModeStatusBothBytes:
              case ModeDbStatusBothBytes:
                sprintf(dummy, "%d bytes", iCur);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 1, cell);
                sprintf(dummy, "%d bytes", iHiwtr);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 2, cell);
                break;
              case ModeStatusFirstBytes:
              case ModeDbStatusFirstBytes:
                sprintf(dummy, "%d bytes", iCur);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 1, cell);
                break;
              case ModeStatusSecondBytes:
              case ModeDbStatusSecondBytes:
                sprintf(dummy, "%d bytes", iHiwtr);
                cell = wxString::FromUTF8(dummy);
                GridCtrl->SetCellValue(i, 2, cell);
                break;
            };
        }
    }
  //
  // restarting the refresh timer
  //
  if (!RefreshTimer)
    RefreshTimer = new wxTimer(this, ID_DB_STATUS_TIMER);
  else
    RefreshTimer->Stop();
  RefreshTimer->Start(250, wxTIMER_ONE_SHOT);
}

void DbStatusDialog::OnClose(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  //wxDialog::EndModal(wxID_OK);
  Destroy();
}
