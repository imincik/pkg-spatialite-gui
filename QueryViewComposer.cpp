/*
/ QueryViewComposer.cpp
/ Query/View Composer
/
/ version 1.7, 2013 May 8
/
/ Author: Sandro Furieri a-furieri@lqt.it
/
/ Copyright (C) 2009-2013  Alessandro Furieri
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

void MyFrame::QueryViewComposer()
{
//
// query/view composer invoked
//
  int ret;
  char *errMsg = NULL;
  ComposerDialog dlg;
  dlg.Create(this);
  int metadataType = GetMetaDataType();
  if (dlg.ShowModal() == wxID_OK)
    {
      if (dlg.IsSpatialView() == true)
        {
          // creating a Spatial View
          ::wxBeginBusyCursor();
          ret =
            sqlite3_exec(GetSqlite(), dlg.GetSqlSample().ToUTF8(), NULL, NULL,
                         &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              ::wxEndBusyCursor();
              return;
            }
          if (dlg.IsWritable1() == true)
            {
              // creating Triggers supporting RW operations on the Main Table
              ret =
                sqlite3_exec(GetSqlite(), dlg.GetSqlTriggerInsert().ToUTF8(),
                             NULL, NULL, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  ::wxEndBusyCursor();
                  return;
                }
              ret =
                sqlite3_exec(GetSqlite(), dlg.GetSqlTriggerUpdate().ToUTF8(),
                             NULL, NULL, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  ::wxEndBusyCursor();
                  return;
                }
              ret =
                sqlite3_exec(GetSqlite(), dlg.GetSqlTriggerDelete().ToUTF8(),
                             NULL, NULL, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  ::wxEndBusyCursor();
                  return;
                }
            }
          wxString sql = wxT("INSERT INTO views_geometry_columns ");
          if (metadataType == METADATA_CURRENT)
            {
              // current metadata style >= v.4.0.0
              sql +=
                wxT
                ("(view_name, view_geometry, view_rowid, f_table_name, f_geometry_column, read_only)");
          } else
            {
              // legacy metadata style <= v.3.1.0
              sql +=
                wxT
                ("(view_name, view_geometry, view_rowid, f_table_name, f_geometry_column)");
            }
          sql += wxT(" VALUES (");
          if (metadataType == METADATA_CURRENT)
            {
              // current metadata style >= v.4.0.0
              sql += wxT("Lower('");
              sql += dlg.GetViewName();
              sql += wxT("'), Lower('");
              sql += dlg.GetGeometryColumnAlias();
              sql += wxT("'), Lower('");
              sql += dlg.GetGeometryRowidAlias();
              sql += wxT("'), Lower('");
              if (dlg.IsViewGeomTable2() == true)
                sql += dlg.GetTableName2();
              else
                sql += dlg.GetTableName1();
              sql += wxT("'), Lower('");
              sql += dlg.GetGeometryColumn();
              if (metadataType == METADATA_CURRENT)
                {
                  // current metadata style >= v.4.0.0
                  if (dlg.IsWritable1() == true)
                    sql += wxT("'), 0)");
                  else
                    sql += wxT("'), 1)");
                }
          } else
            {
              // legacy metadata style <= v.3.1.0
              sql += wxT("'");
              sql += dlg.GetViewName();
              sql += wxT("', '");
              sql += dlg.GetGeometryColumnAlias();
              sql += wxT("', '");
              sql += dlg.GetGeometryRowidAlias();
              sql += wxT("', '");
              if (dlg.IsViewGeomTable2() == true)
                sql += dlg.GetTableName2();
              else
                sql += dlg.GetTableName1();
              sql += wxT("', '");
              sql += dlg.GetGeometryColumn();
              sql += wxT("')");
            }
          ret = sqlite3_exec(GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              ::wxEndBusyCursor();
              return;
            }
          if (metadataType == METADATA_CURRENT)
            {
              // current metadata style >= v.4.0.0

              // creating the Spatial View Auth row
              sql = wxT("INSERT INTO views_geometry_columns_auth ");
              sql += wxT("(view_name, view_geometry, hidden)");
              sql += wxT(" VALUES (Lower('");
              sql += dlg.GetViewName();
              sql += wxT("'), Lower('");
              sql += dlg.GetGeometryColumnAlias();
              sql += wxT("'), 0)");
              ret =
                sqlite3_exec(GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  ::wxEndBusyCursor();
                  return;
                }
              // creating the Spatial View Statistics row
              sql = wxT("INSERT INTO views_geometry_columns_statistics ");
              sql += wxT("(view_name, view_geometry)");
              sql += wxT(" VALUES (Lower('");
              sql += dlg.GetViewName();
              sql += wxT("'), Lower('");
              sql += dlg.GetGeometryColumnAlias();
              sql += wxT("'))");
              ret =
                sqlite3_exec(GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  ::wxEndBusyCursor();
                  return;
                }
            }
          ::wxEndBusyCursor();
          wxMessageBox(wxT("The Spatial View ") + dlg.GetViewName() +
                       wxT("\nwas successfully created"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          InitTableTree();
          return;
      } else if (dlg.IsPlainView() == true)
        {
          // creating an ordinary View
          ::wxBeginBusyCursor();
          ret =
            sqlite3_exec(GetSqlite(), dlg.GetSqlSample().ToUTF8(), NULL, NULL,
                         &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              ::wxEndBusyCursor();
              return;
            }
          ::wxEndBusyCursor();
          wxMessageBox(wxT("The View ") + dlg.GetViewName() +
                       wxT("\nwas successfully created"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          InitTableTree();
          return;
      } else
        {
          // executing the SQL statement
          SetSql(dlg.GetSqlSample(), true);
        }
    }
}

bool ComposerDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  IncompleteSql = true;
  Table2Enabled = false;
  TableAlias1 = wxT("a");
  TableAlias2 = wxT("b");
  LeftJoin = false;
  Match2Enabled = false;
  Match3Enabled = false;
  Where1Enabled = false;
  Where2Enabled = false;
  Where3Enabled = false;
  Where1Table2 = false;
  Where2Table2 = false;
  Where3Table2 = false;
  AndOr12 = true;
  AndOr23 = true;
  Order1Enabled = false;
  Order2Enabled = false;
  Order3Enabled = false;
  Order4Enabled = false;
  Order1Table2 = false;
  Order2Table2 = false;
  Order3Table2 = false;
  Order4Table2 = false;
  Order1Desc = false;
  Order2Desc = false;
  Order3Desc = false;
  Order4Desc = false;
  ViewGeomTable2 = false;
  PlainView = false;
  SpatialView = false;
  Writable1 = false;
  Writable2 = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Query / View Composer")) == false)
    return false;
// populates individual controls
  TableList.Populate(MainFrame->GetSqlite());
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  UpdateSqlSample();
  return true;
}

void ComposerDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
// the SQL sample row
  wxBoxSizer *sqlBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(sqlBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticBox *sqlBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("SQL statement"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *sqlSizer = new wxStaticBoxSizer(sqlBox, wxVERTICAL);
  sqlBoxSizer->Add(sqlSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  SqlCtrl =
    new wxTextCtrl(this, ID_QVC_SQL, wxT(""), wxDefaultPosition,
                   wxSize(600, 150),
                   wxTE_READONLY | wxTE_MULTILINE | wxHSCROLL | wxTE_RICH);
  sqlSizer->Add(SqlCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the tab control
  TabCtrl =
    new wxNotebook(this, ID_QVC_TAB, wxDefaultPosition, wxDefaultSize,
                   wxNB_TOP);
  boxSizer->Add(TabCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// tab - Page #1
  Page1 = new ComposerMainPage();
  Page1->Create(this);
  TabCtrl->AddPage(Page1, wxT("Main"), true);
// tab - Page #2
  Page2 = new ComposerFilterPage();
  Page2->Create(this);
  TabCtrl->AddPage(Page2, wxT("Filter"), true);
// tab - Page #3
  Page3 = new ComposerOrderPage();
  Page3->Create(this);
  TabCtrl->AddPage(Page3, wxT("Order"), true);
// tab - Page #4
  Page4 = new ComposerViewPage();
  Page4->Create(this);
  TabCtrl->AddPage(Page4, wxT("View"), true);

  TabCtrl->ChangeSelection(0);

// appending the OK / CANCEL buttons
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *okBtn = new wxButton(this, wxID_OK, wxT("&Ok"));
  btnSizer->Add(okBtn, 0, wxALIGN_LEFT | wxALL, 5);
  wxButton *cancelBtn = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  btnSizer->Add(cancelBtn, 0, wxALIGN_LEFT | wxALL, 5);
// appending the event handlers
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & ComposerDialog::OnOk);
}

void ComposerDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  if (IncompleteSql == true)
    {
      wxMessageBox(wxT
                   ("Your SQL statement isn't yet completed ...\n\nplease, check better and then retry"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (SpatialView == true && GeometryColumn.Len() == 0)
    {
      wxMessageBox(wxT
                   ("You MUST specify a Geometry Column to create a Spatial View ...\n\nplease, check better and then retry"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  PrepareSqlTriggers();
  wxDialog::EndModal(wxID_OK);
}

void ComposerDialog::Table1Status(bool ok)
{
//
// enabling/disabling dependent fields in following pages
//
  Page2->Table1Status(ok);
  Page3->Table1Status(ok);
  Page4->Table1Status(ok);
  UpdateSqlSample();
}

void ComposerDialog::Table2Status(bool ok)
{
//
// enabling/disabling dependent fields in following pages
//
  Page2->Table2Status(ok);
  Page3->Table2Status(ok);
  Page4->Table2Status(ok);
  UpdateSqlSample();
}

void ComposerDialog::SelectGeometryColumn()
{
//
// forcing the SpatialView Geometry Column to be selected anyway
//
  wxString rowid = wxT("ROWID");
  if (GeometryColumn.Len() > 0)
    {
      if (ViewGeomTable2 == true)
        {
          Column2List.SetState(GeometryColumn);
          Page1->SelectGeometryColumn(GeometryColumn, true);
          Column2List.SetState(rowid);
          Page1->SelectGeometryColumn(rowid, true);
      } else
        {
          Column1List.SetState(GeometryColumn);
          Page1->SelectGeometryColumn(GeometryColumn, false);
          Column1List.SetState(rowid);
          Page1->SelectGeometryColumn(rowid, false);
        }
    }
}

bool ComposerDialog::IsDuplicateAlias(wxString & alias)
{
//
// checkink an alias name to be unique
//
  AuxColumn *pC;
  pC = Column1List.GetFirst();
  while (pC)
    {
      if (pC->GetAliasName().CmpNoCase(alias) == 0)
        return true;
      pC = pC->GetNext();
    }
  pC = Column2List.GetFirst();
  while (pC)
    {
      if (pC->GetAliasName().CmpNoCase(alias) == 0)
        return true;
      pC = pC->GetNext();
    }
  return false;
}

void ComposerDialog::SetAliases()
{
//
// setting up column alias names
//
  AuxColumn *pC;
  wxString alias;
  int antiAlias;
  char dummy[64];

// cleaning up the old alias names
  pC = Column1List.GetFirst();
  while (pC)
    {
      pC->SetAliasName(alias);
      pC = pC->GetNext();
    }
  pC = Column2List.GetFirst();
  while (pC)
    {
      pC->SetAliasName(alias);
      pC = pC->GetNext();
    }

// setting up the current alias names
  pC = Column1List.GetFirst();
  while (pC)
    {
      if (pC->IsSelected() == false)
        {
          pC = pC->GetNext();
          continue;
        }
      alias = pC->GetName();
      if (IsDuplicateAlias(alias) == true)
        {
          antiAlias = 1;
          while (1)
            {
              sprintf(dummy, "_%d", antiAlias++);
              alias = pC->GetName() + wxString::FromUTF8(dummy);
              if (IsDuplicateAlias(alias) == false)
                break;
            }
        }
      pC->SetAliasName(alias);
      if (ViewGeomTable2 == false)
        {
          if (pC->GetName().CmpNoCase(GeometryColumn) == 0)
            GeometryColumnAlias = pC->GetAliasName();
          if (pC->GetName() == wxString::FromUTF8("ROWID"))
            GeometryRowidAlias = pC->GetAliasName();
        }
      pC = pC->GetNext();
    }
  pC = Column2List.GetFirst();
  while (pC)
    {
      if (pC->IsSelected() == false)
        {
          pC = pC->GetNext();
          continue;
        }
      alias = pC->GetName();
      if (IsDuplicateAlias(alias) == true)
        {
          antiAlias = 1;
          while (1)
            {
              sprintf(dummy, "_%d", antiAlias++);
              alias = pC->GetName() + wxString::FromUTF8(dummy);
              if (IsDuplicateAlias(alias) == false)
                break;
            }
        }
      pC->SetAliasName(alias);
      if (ViewGeomTable2 == true)
        {
          if (pC->GetName().CmpNoCase(GeometryColumn) == 0)
            GeometryColumnAlias = pC->GetAliasName();
          if (pC->GetName() == wxString::FromUTF8("ROWID"))
            GeometryRowidAlias = pC->GetAliasName();
        }
      pC = pC->GetNext();
    }
}

void ComposerDialog::UpdateSqlSample()
{
//
// composing the SQL statement
//
  AuxColumn *pCol;
  bool comma;
  int markStart[4192];
  int markStop[4192];
  int markStyle[4192];
  int nextMark = 0;
  int base;
  char xname[1024];
  wxString undefined = wxT("~???~");
  wxString selectedColumns;
  IncompleteSql = false;
  SetAliases();
  SqlSample = wxT("");

  if (PlainView == true || SpatialView == true)
    {
      // CREATE VIEW ... AS
      markStyle[nextMark] = 2;
      markStart[nextMark] = 0;
      SqlSample += wxT("CREATE VIEW");
      markStop[nextMark++] = SqlSample.Len();
      if (ViewName.Len() == 0)
        {
          SqlSample += wxT(" ");
          markStyle[nextMark] = 1;
          markStart[nextMark] = SqlSample.Len();
          SqlSample += undefined;
          IncompleteSql = true;
          markStop[nextMark++] = SqlSample.Len();
          SqlSample += wxT(" ");
      } else
        {
          SqlSample += wxT(" ");
          strcpy(xname, ViewName.ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          SqlSample += wxString::FromUTF8(xname);
          SqlSample += wxT(" ");
        }
      markStyle[nextMark] = 2;
      markStart[nextMark] = SqlSample.Len();
      SqlSample += wxT("AS");
      markStop[nextMark++] = SqlSample.Len();
      SqlSample += wxT("\n");
    }

  markStyle[nextMark] = 2;
  markStart[nextMark] = SqlSample.Len();
  int startSelect = SqlSample.Len();
  SqlSample += wxT("SELECT");
  markStop[nextMark++] = SqlSample.Len();
  SqlSample += wxT(" ");
  base = SqlSample.Len();
// SELECT a,b,c [from the main table]
  int selectCount = SqlSample.Len() - startSelect;
  comma = false;
  pCol = Column1List.GetFirst();
  while (pCol)
    {
      if (pCol->IsSelected() == true)
        {
          if (comma)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + selectedColumns.Len();
              selectedColumns += wxT(",");
              markStop[nextMark++] = base + selectedColumns.Len();
              if (selectCount > 50)
                {
                  selectedColumns += wxT("\n    ");
                  selectCount = 4;
              } else
                {
                  selectedColumns += wxT(" ");
                  selectCount += 2;
                }
            }
          wxString col;
          int baseCol = base + selectedColumns.Len();
          if (Table2Enabled == false)
            ;
          else
            {
              if (TableAlias1.Len() == 0)
                {
                  strcpy(xname, TableName1.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  col += wxString::FromUTF8(xname);
                  col += wxT(".");
              } else
                {
                  strcpy(xname, TableAlias1.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  col += wxString::FromUTF8(xname);
                  col += wxT(".");
                }
            }
          strcpy(xname, pCol->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          col += wxString::FromUTF8(xname);
          col += wxT(" ");
          markStyle[nextMark] = 2;
          markStart[nextMark] = baseCol + col.Len();
          col += wxT("AS");
          markStop[nextMark++] = baseCol + col.Len();
          col += wxT(" ");
          strcpy(xname, pCol->GetAliasName().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          col += wxString::FromUTF8(xname);
          selectCount += col.Len();
          selectedColumns += col;
          comma = true;
        }
      pCol = pCol->GetNext();
    }
  if (Table2Enabled == true && Column2List.HasSelectedColumns() == true)
    {
      // SELECT a,b,c [from Table#2]
      pCol = Column2List.GetFirst();
      while (pCol)
        {
          if (pCol->IsSelected() == true)
            {
              if (comma)
                {
                  markStyle[nextMark] = 2;
                  markStart[nextMark] = base + selectedColumns.Len();
                  selectedColumns += wxT(",");
                  markStop[nextMark++] = base + selectedColumns.Len();
                }
              wxString col;
              int baseCol = base + selectedColumns.Len();
              if (selectCount > 50)
                {
                  col += wxT("\n    ");
                  selectCount = 0;
              } else
                {
                  col += wxT(" ");
                  selectCount += 2;
                }
              if (TableAlias2.Len() == 0)
                {
                  strcpy(xname, TableName2.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  col += wxString::FromUTF8(xname);
                  col += wxT(".");
              } else
                {
                  strcpy(xname, TableAlias2.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  col += wxString::FromUTF8(xname);
                  col += wxT(".");
                }
              strcpy(xname, pCol->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              col += wxString::FromUTF8(xname);
              col += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = baseCol + col.Len();
              col += wxT("AS");
              markStop[nextMark++] = baseCol + col.Len();
              col += wxT(" ");
              strcpy(xname, pCol->GetAliasName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              col += wxString::FromUTF8(xname);
              selectCount += col.Len();
              selectedColumns += col;
              comma = true;
            }
          pCol = pCol->GetNext();
        }
    }
  if (selectedColumns.Len() == 0)
    {
      markStyle[nextMark] = 1;
      markStart[nextMark] = SqlSample.Len();
      SqlSample += undefined;
      IncompleteSql = true;
      markStop[nextMark++] = SqlSample.Len();
  } else
    SqlSample += selectedColumns;

// the FROM clause
  SqlSample += wxT("\n");
  markStyle[nextMark] = 2;
  markStart[nextMark] = SqlSample.Len();
  SqlSample += wxT("FROM");
  markStop[nextMark++] = SqlSample.Len();
  SqlSample += wxT(" ");
  if (TableName1.Len() == 0)
    {
      markStyle[nextMark] = 1;
      markStart[nextMark] = SqlSample.Len();
      SqlSample += undefined;
      IncompleteSql = true;
      markStop[nextMark++] = SqlSample.Len();
  } else
    {
      base = SqlSample.Len();
      strcpy(xname, TableName1.ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      wxString from = wxString::FromUTF8(xname);
      if (Table2Enabled == false)
        ;
      else
        {
          if (TableAlias1.Len() == 0)
            ;
          else
            {
              from += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + from.Len();
              from += wxT("AS");
              markStop[nextMark++] = base + from.Len();
              from += wxT(" ");
              strcpy(xname, TableAlias1.ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              from += wxString::FromUTF8(xname);
            }
        }
      SqlSample += from;
    }
  if (Table2Enabled == true)
    {
      SqlSample += wxT("\n");
      int onIniJoin = SqlSample.Len();
      if (LeftJoin == true)
        {
          markStyle[nextMark] = 2;
          markStart[nextMark] = SqlSample.Len();
          SqlSample += wxT("LEFT JOIN");
          markStop[nextMark++] = SqlSample.Len();
      } else
        {
          markStyle[nextMark] = 2;
          markStart[nextMark] = SqlSample.Len();
          SqlSample += wxT("JOIN");
          markStop[nextMark++] = SqlSample.Len();
        }
      SqlSample += wxT(" ");
      if (TableName2.Len() == 0)
        {
          markStyle[nextMark] = 1;
          markStart[nextMark] = SqlSample.Len();
          SqlSample += undefined;
          IncompleteSql = true;
          markStop[nextMark++] = SqlSample.Len();
      } else
        {
          base = SqlSample.Len();
          strcpy(xname, TableName2.ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          wxString join = wxString::FromUTF8(xname);
          if (TableAlias2.Len() == 0)
            ;
          else
            {
              join += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + join.Len();
              join += wxT("AS");
              markStop[nextMark++] = base + join.Len();
              join += wxT(" ");
              strcpy(xname, TableAlias2.ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              join += wxString::FromUTF8(xname);
            }
          int onCount = SqlSample.Len() - onIniJoin;
          int baseMatch = join.Len();
          wxString match;
          bool isUsing = false;
          if (Match2Enabled == false && Match3Enabled == false
              && Match1Table1 == Match1Table2)
            isUsing = true;
          if (Match2Enabled == true && Match3Enabled == false
              && Match1Table1 == Match1Table2 && Match2Table1 == Match2Table2)
            isUsing = true;
          if (Match2Enabled == false && Match3Enabled == true
              && Match1Table1 == Match1Table2 && Match3Table1 == Match3Table2)
            isUsing = true;
          if (Match2Enabled == true && Match3Enabled == true
              && Match1Table1 == Match1Table2 && Match2Table1 == Match2Table2
              && Match3Table1 == Match3Table2)
            isUsing = true;
          if (isUsing == true)
            {
              match = wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + baseMatch + match.Len();
              match += wxT("USING (");
              markStop[nextMark++] = base + baseMatch + match.Len();
              if (Match1Table1.Len() == 0)
                {
                  markStyle[nextMark] = 1;
                  markStart[nextMark] = base + baseMatch + match.Len();
                  match += undefined;
                  IncompleteSql = true;
                  markStop[nextMark++] = base + baseMatch + match.Len();
              } else
                {
                  strcpy(xname, Match1Table1.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  match += wxString::FromUTF8(xname);
                }
              if (Match2Enabled == true)
                {
                  markStyle[nextMark] = 2;
                  markStart[nextMark] = base + baseMatch + match.Len();
                  match += wxT(", ");
                  markStop[nextMark++] = base + baseMatch + match.Len();
                  if (Match2Table1.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] = base + baseMatch + match.Len();
                      match += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] = base + baseMatch + match.Len();
                  } else
                    {
                      strcpy(xname, Match2Table1.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      match += wxString::FromUTF8(xname);
                    }
                }
              if (Match3Enabled == true)
                {
                  markStyle[nextMark] = 2;
                  markStart[nextMark] = base + baseMatch + match.Len();
                  match += wxT(", ");
                  markStop[nextMark++] = base + baseMatch + match.Len();
                  if (Match3Table1.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] = base + baseMatch + match.Len();
                      match += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] = base + baseMatch + match.Len();
                  } else
                    {
                      strcpy(xname, Match3Table1.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      match += wxString::FromUTF8(xname);
                    }
                }
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + baseMatch + match.Len();
              match += wxT(")");
              markStop[nextMark++] = base + baseMatch + match.Len();
          } else
            {
              match = wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + baseMatch + match.Len();
              match += wxT("ON (");
              markStop[nextMark++] = base + baseMatch + match.Len();
              int baseCondition = match.Len();
              wxString condition;
              if (Match1Table1.Len() > 0 || Match1Table2.Len() > 0)
                {
                  if (Match1Table1.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match1Table1.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                  condition += wxT(" = ");
                  if (Match1Table2.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match1Table2.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                  onCount += condition.Len();
                }
              if (Match2Enabled
                  && (Match2Table1.Len() > 0 || Match2Table2.Len() > 0))
                {
                  if (onCount > 30)
                    {
                      condition += wxT("\n    ");
                      onCount = 4;
                  } else
                    condition += wxT(" ");
                  markStyle[nextMark] = 2;
                  markStart[nextMark] =
                    base + baseMatch + baseCondition + condition.Len();
                  condition += wxT("AND");
                  markStop[nextMark++] =
                    base + baseMatch + baseCondition + condition.Len();
                  condition += wxT(" ");
                  if (Match2Table1.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match2Table1.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                  condition += wxT(" = ");
                  if (Match2Table2.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match2Table2.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                  onCount += condition.Len();
                }
              if (Match3Enabled
                  && (Match3Table1.Len() > 0 || Match3Table2.Len() > 0))
                {
                  if (onCount > 30)
                    {
                      condition += wxT("\n    ");
                      onCount = 4;
                  } else
                    condition += wxT(" ");
                  markStyle[nextMark] = 2;
                  markStart[nextMark] =
                    base + baseMatch + baseCondition + condition.Len();
                  condition += wxT("AND");
                  markStop[nextMark++] =
                    base + baseMatch + baseCondition + condition.Len();
                  condition += wxT(" ");
                  if (Match3Table1.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match3Table1.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                  condition += wxT(" = ");
                  if (Match3Table2.Len() == 0)
                    {
                      markStyle[nextMark] = 1;
                      markStart[nextMark] =
                        base + baseMatch + baseCondition + condition.Len();
                      condition += undefined;
                      IncompleteSql = true;
                      markStop[nextMark++] =
                        base + baseMatch + baseCondition + condition.Len();
                  } else
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          condition += wxString::FromUTF8(xname);
                        }
                      condition += wxT(".");
                      strcpy(xname, Match3Table2.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      condition += wxString::FromUTF8(xname);
                    }
                }
              if (condition.Len() == 0)
                {
                  match += undefined;
                  IncompleteSql = true;
              } else
                match += condition;
              markStyle[nextMark] = 2;
              markStart[nextMark] = base + baseMatch + match.Len();
              match += wxT(")");
              markStop[nextMark++] = base + baseMatch + match.Len();
            }
          if (match.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = base + join.Len();
              join += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = base + join.Len();
          } else
            join += match;
          SqlSample += join;
        }
    }
  if (Where1Enabled == true || Where2Enabled == true || Where3Enabled == true)
    {
      // the WHERE clause
      bool openBraket1 = false;
      bool openBraket2 = false;
      bool closeBraket3 = false;
      bool closeBraket4 = false;
      SqlSample += wxT("\n");
      int whereStart = SqlSample.Len();
      markStyle[nextMark] = 2;
      markStart[nextMark] = SqlSample.Len();
      SqlSample += wxT("WHERE");
      markStop[nextMark++] = SqlSample.Len();
      SqlSample += wxT(" ");
      int whereCount = SqlSample.Len() - whereStart;
      if (Where1Enabled == true && Where2Enabled == true
          && Where3Enabled == true)
        {
          if (AndOr12 == false && AndOr23 == true)
            {
              openBraket1 = true;
              closeBraket3 = true;
            }
          if (AndOr12 == true && AndOr23 == false)
            {
              openBraket2 = true;
              closeBraket4 = true;
            }
        }
      if (Where1Enabled == true)
        {
          int clauseBase = SqlSample.Len();
          wxString clause;
          if (openBraket1 == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT("(");
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          if (Where1Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Where1Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  clause += wxString::FromUTF8(xname);
              } else
                {
                  if (Where1Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                        }
                      clause += wxT(".");
                      clause += Where1Column;
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        clause += TableName1;
                      else
                        clause += TableAlias1;
                      clause += wxT(".");
                      clause += Where1Column;
                    }
                }
            }
          clause += wxT(" ");
          if (Where1Operator.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += Where1Operator;
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          if (Where1Operator == wxT("IS NULL")
              || Where1Operator == wxT("IS NOT NULL"))
            ;
          else if (Where1Operator == wxT("IN"))
            {
              clause += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT("(");
              markStop[nextMark++] = clauseBase + clause.Len();
              int listBase = clauseBase + clause.Len();
              wxString cleanList;
              SqlCleanList(Where1Value, cleanList, markStyle, markStart,
                           markStop, &nextMark, listBase);
              clause += cleanList;
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT(")");
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              clause += wxT(" ");
              bool isNumber = false;
              long lngTest;
              double dblTest;
              if (Where1Value.ToLong(&lngTest) == true)
                isNumber = true;
              if (Where1Value.ToDouble(&dblTest) == true)
                isNumber = true;
              bool noQuote;
              wxString cleanSql;
              if (isNumber == false)
                noQuote = SqlCleanString(Where1Value, cleanSql);
              else
                {
                  noQuote = true;
                  cleanSql = Where1Value;
                }
              if (noQuote == false)
                clause += wxT("'");
              clause += cleanSql;
              if (noQuote == false)
                clause += wxT("'");
            }
          whereCount += clause.Len();
          SqlSample += clause;
        }
      if (Where2Enabled == true)
        {
          int clauseBase = SqlSample.Len();
          wxString clause;
          if (whereCount > 30)
            {
              clause = wxT("\n    ");
              whereCount = 4;
          } else
            clause = wxT(" ");
          markStyle[nextMark] = 2;
          markStart[nextMark] = clauseBase + clause.Len();
          if (AndOr12 == true)
            clause += wxT("AND");
          else
            clause += wxT("OR");
          markStop[nextMark++] = clauseBase + clause.Len();
          clause += wxT(" ");
          if (openBraket2 == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT("(");
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          if (Where2Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Where2Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  clause += wxString::FromUTF8(xname);
              } else
                {
                  if (Where2Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                        }
                      clause += wxT(".");
                      strcpy(xname, Where2Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      clause += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                        }
                      clause += wxT(".");
                      strcpy(xname, Where2Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      clause += wxString::FromUTF8(xname);
                    }
                }
            }
          clause += wxT(" ");
          if (Where2Operator.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += Where2Operator;
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          if (Where2Operator == wxT("IS NULL")
              || Where2Operator == wxT("IS NOT NULL"))
            ;
          else
            {
              clause += wxT(" ");
              bool isNumber = false;
              long lngTest;
              double dblTest;
              if (Where2Value.ToLong(&lngTest) == true)
                isNumber = true;
              if (Where2Value.ToDouble(&dblTest) == true)
                isNumber = true;
              bool noQuote;
              wxString cleanSql;
              if (isNumber == false)
                noQuote = SqlCleanString(Where2Value, cleanSql);
              else
                {
                  noQuote = true;
                  cleanSql = Where2Value;
                }
              if (noQuote == false)
                clause += wxT("'");
              clause += cleanSql;
              if (noQuote == false)
                clause += wxT("'");
            }
          if (closeBraket3 == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT(")");
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          whereCount += clause.Len();
          SqlSample += clause;
        }
      if (Where3Enabled == true)
        {
          int clauseBase = SqlSample.Len();
          wxString clause;
          if (whereCount > 30)
            {
              clause = wxT("\n    ");
              whereCount = 4;
          } else
            clause = wxT(" ");
          markStyle[nextMark] = 2;
          markStart[nextMark] = clauseBase + clause.Len();
          if (AndOr23 == true)
            clause += wxT("AND");
          else
            clause += wxT("OR");
          markStop[nextMark++] = clauseBase + clause.Len();
          clause += wxT(" ");
          if (Where3Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Where3Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  clause += wxString::FromUTF8(xname);
              } else
                {
                  if (Where3Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                        }
                      clause += wxT(".");
                      strcpy(xname, Where3Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      clause += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          clause += wxString::FromUTF8(xname);
                        }
                      clause += wxT(".");
                      strcpy(xname, Where3Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      clause += wxString::FromUTF8(xname);
                    }
                }
            }
          clause += wxT(" ");
          if (Where3Operator.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = clauseBase + clause.Len();
          } else
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += Where3Operator;
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          if (Where3Operator == wxT("IS NULL")
              || Where3Operator == wxT("IS NOT NULL"))
            ;
          else
            {
              clause += wxT(" ");
              bool isNumber = false;
              long lngTest;
              double dblTest;
              if (Where3Value.ToLong(&lngTest) == true)
                isNumber = true;
              if (Where3Value.ToDouble(&dblTest) == true)
                isNumber = true;
              bool noQuote;
              wxString cleanSql;
              if (isNumber == false)
                noQuote = SqlCleanString(Where3Value, cleanSql);
              else
                {
                  noQuote = true;
                  cleanSql = Where3Value;
                }
              if (noQuote == false)
                clause += wxT("'");
              clause += cleanSql;
              if (noQuote == false)
                clause += wxT("'");
            }
          if (closeBraket4 == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = clauseBase + clause.Len();
              clause += wxT(")");
              markStop[nextMark++] = clauseBase + clause.Len();
            }
          SqlSample += clause;
        }
    }
  if (Order1Enabled == true || Order2Enabled == true || Order3Enabled == true
      || Order4Enabled == true)
    {
      // the ORDER BY clause
      bool comma = false;
      SqlSample += wxT("\n");
      int orderStart = SqlSample.Len();
      markStyle[nextMark] = 2;
      markStart[nextMark] = SqlSample.Len();
      SqlSample += wxT("ORDER BY");
      markStop[nextMark++] = SqlSample.Len();
      SqlSample += wxT(" ");
      int orderCount = SqlSample.Len() - orderStart;
      if (Order1Enabled == true)
        {
          int orderBase = SqlSample.Len();
          wxString order;
          if (Order1Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = orderBase + order.Len();
              order += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = orderBase + order.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Order1Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  order += wxString::FromUTF8(xname);
              } else
                {
                  if (Order1Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order1Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order1Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                    }
                }
            }
          if (Order1Desc == true)
            {
              order += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT("DESC");
              markStop[nextMark++] = orderBase + order.Len();
            }
          orderCount += order.Len();
          SqlSample += order;
          comma = true;
        }
      if (Order2Enabled == true)
        {
          int orderBase = SqlSample.Len();
          wxString order;
          if (comma == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT(",");
              markStop[nextMark++] = orderBase + order.Len();
            }
          if (orderCount > 40)
            {
              order += wxT("\n    ");
              orderCount = 4;
          } else
            order += wxT(" ");
          if (Order2Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = orderBase + order.Len();
              order += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = orderBase + order.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Order2Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  order += wxString::FromUTF8(xname);
              } else
                {
                  if (Order2Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order2Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order2Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                    }
                }
            }
          if (Order2Desc == true)
            {
              order += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT("DESC");
              markStop[nextMark++] = orderBase + order.Len();
            }
          orderCount += order.Len();
          SqlSample += order;
          comma = true;
        }
      if (Order3Enabled == true)
        {
          int orderBase = SqlSample.Len();
          wxString order;
          if (comma == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT(",");
              markStop[nextMark++] = orderBase + order.Len();
            }
          if (orderCount > 40)
            {
              order += wxT("\n    ");
              orderCount = 4;
          } else
            order += wxT(" ");
          if (Order3Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = orderBase + order.Len();
              order += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = orderBase + order.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Order1Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  order += wxString::FromUTF8(xname);
              } else
                {
                  if (Order3Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order3Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order3Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                    }
                }
            }
          if (Order3Desc == true)
            {
              order += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT("DESC");
              markStop[nextMark++] = orderBase + order.Len();
            }
          orderCount += order.Len();
          SqlSample += order;
          comma = true;
        }
      if (Order4Enabled == true)
        {
          int orderBase = SqlSample.Len();
          wxString order;
          if (comma == true)
            {
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT(",");
              markStop[nextMark++] = orderBase + order.Len();
            }
          if (orderCount > 40)
            {
              order += wxT("\n    ");
              orderCount = 4;
          } else
            order += wxT(" ");
          if (Order4Column.Len() == 0)
            {
              markStyle[nextMark] = 1;
              markStart[nextMark] = orderBase + order.Len();
              order += undefined;
              IncompleteSql = true;
              markStop[nextMark++] = orderBase + order.Len();
          } else
            {
              if (Table2Enabled == false)
                {
                  strcpy(xname, Order1Column.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  order += wxString::FromUTF8(xname);
              } else
                {
                  if (Order4Table2 == true)
                    {
                      if (TableAlias2.Len() == 0)
                        {
                          strcpy(xname, TableName2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias2.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order4Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                  } else
                    {
                      if (TableAlias1.Len() == 0)
                        {
                          strcpy(xname, TableName1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                      } else
                        {
                          strcpy(xname, TableAlias1.ToUTF8());
                          MainFrame->DoubleQuotedSql(xname);
                          order += wxString::FromUTF8(xname);
                        }
                      order += wxT(".");
                      strcpy(xname, Order4Column.ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      order += wxString::FromUTF8(xname);
                    }
                }
            }
          if (Order4Desc == true)
            {
              order += wxT(" ");
              markStyle[nextMark] = 2;
              markStart[nextMark] = orderBase + order.Len();
              order += wxT("DESC");
              markStop[nextMark++] = orderBase + order.Len();
            }
          SqlSample += order;
        }
    }

  SqlCtrl->Hide();
  SqlCtrl->SetValue(SqlSample);
  wxColour bgColor(255, 255, 255);
  if (IncompleteSql == true
      || (SpatialView == true && GeometryColumn.Len() == 0))
    bgColor = wxColor(255, 255, 0);
  wxTextAttr style(wxColour(192, 0, 192), bgColor,
                   wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_NORMAL));
  wxTextAttr style2(wxColour(128, 0, 255), wxColour(255, 192, 0),
                    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                           wxFONTWEIGHT_NORMAL));
  wxTextAttr style3(wxColour(0, 0, 255), bgColor,
                    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                           wxFONTWEIGHT_BOLD));
  SqlCtrl->SetStyle(0, SqlSample.Len(), style);
  int i;
  for (i = 0; i < nextMark; i++)
    {
      if (markStyle[i] == 2)
        SqlCtrl->SetStyle(markStart[i], markStop[i], style3);
      else
        SqlCtrl->SetStyle(markStart[i], markStop[i], style2);
    }
  SqlCtrl->Show();
}

void ComposerDialog::PrepareSqlTriggers()
{
//
// preparing the SQL Triggers enabling a Writable View
//
  AuxColumn *pCol;
  char dummy[8192];
  char xname[1024];
  char xname2[1024];
  char xview[1024];
  char xcol[1024];
  bool comma;
  SqlTriggerInsert = wxT("");
  SqlTriggerUpdate = wxT("");
  SqlTriggerDelete = wxT("");
  if (Writable1 == false)
    return;

// composing the INSERT trigger
  SqlTriggerInsert = wxT("CREATE TRIGGER ");
  strcpy(xview, ViewName.ToUTF8());
  sprintf(dummy, "vw_ins_%s", xview);
  MyFrame::DoubleQuotedSql(dummy);
  SqlTriggerInsert += wxString::FromUTF8(dummy);
  SqlTriggerInsert += wxT(" INSTEAD OF INSERT ON ");
  MyFrame::DoubleQuotedSql(xview);
  SqlTriggerInsert += wxString::FromUTF8(xview);
  SqlTriggerInsert += wxT("\nBEGIN\nINSERT OR REPLACE INTO ");
  if (Writable2 == true && Table2Enabled == true)
    {
      // supporting table #2
      strcpy(xname2, TableName2.ToUTF8());
      MyFrame::DoubleQuotedSql(xname2);
      SqlTriggerInsert += wxString::FromUTF8(xname2);
      SqlTriggerInsert += wxT(" (");
      comma = false;
      strcpy(xcol, Match1Table2.ToUTF8());
      MainFrame->DoubleQuotedSql(xcol);
      SqlTriggerInsert += wxString::FromUTF8(xcol);
      comma = true;
      if (Match2Enabled == true)
        {
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, Match2Table2.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxString::FromUTF8(xcol);
        }
      if (Match3Enabled == true)
        {
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, Match3Table2.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxString::FromUTF8(xcol);
        }
      pCol = Column2List.GetFirst();
      while (pCol)
        {
          if (pCol->IsSelected() == true)
            {
              if (pCol->GetName() == wxT("ROWID"))
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (pCol->GetName() == Match1Table2)
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (Match2Enabled == true)
                {
                  if (pCol->GetName() == Match2Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (Match3Enabled == true)
                {
                  if (pCol->GetName() == Match3Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (comma)
                SqlTriggerInsert += wxT(", ");
              strcpy(xcol, pCol->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xcol);
              SqlTriggerInsert += wxString::FromUTF8(xcol);
              comma = true;
            }
          pCol = pCol->GetNext();
        }
      SqlTriggerInsert += wxT(")\n  VALUES (");
      comma = false;
      strcpy(xcol, Match1Table1.ToUTF8());
      MainFrame->DoubleQuotedSql(xcol);
      SqlTriggerInsert += wxT("NEW.") + wxString::FromUTF8(xcol);
      comma = true;
      if (Match2Enabled == true)
        {
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, Match2Table1.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxT("NEW.") + wxString::FromUTF8(xcol);
        }
      if (Match3Enabled == true)
        {
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, Match3Table1.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxT("NEW.") + wxString::FromUTF8(xcol);
        }
      pCol = Column2List.GetFirst();
      while (pCol)
        {
          if (pCol->IsSelected() == true)
            {
              if (pCol->GetName() == wxT("ROWID"))
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (pCol->GetName() == Match1Table2)
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (Match2Enabled == true)
                {
                  if (pCol->GetName() == Match2Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (Match3Enabled == true)
                {
                  if (pCol->GetName() == Match3Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (comma)
                SqlTriggerInsert += wxT(", ");
              strcpy(xcol, pCol->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xcol);
              SqlTriggerInsert += wxT("NEW.") + wxString::FromUTF8(xcol);
              comma = true;
            }
          pCol = pCol->GetNext();
        }
      SqlTriggerInsert += wxT(");\nINSERT INTO ");
    }
  strcpy(xname, TableName1.ToUTF8());
  MyFrame::DoubleQuotedSql(xname);
  SqlTriggerInsert += wxString::FromUTF8(xname);
  SqlTriggerInsert += wxT(" (");
  comma = false;
  pCol = Column1List.GetFirst();
  while (pCol)
    {
      if (pCol->IsSelected() == true)
        {
          if (pCol->GetName() == wxT("ROWID"))
            {
              pCol = pCol->GetNext();
              continue;
            }
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, pCol->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxString::FromUTF8(xcol);
          comma = true;
        }
      pCol = pCol->GetNext();
    }
  SqlTriggerInsert += wxT(")\n    VALUES (");
  comma = false;
  pCol = Column1List.GetFirst();
  while (pCol)
    {
      if (pCol->IsSelected() == true)
        {
          if (pCol->GetName() == wxT("ROWID"))
            {
              pCol = pCol->GetNext();
              continue;
            }
          if (comma)
            SqlTriggerInsert += wxT(", ");
          strcpy(xcol, pCol->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerInsert += wxT("NEW.") + wxString::FromUTF8(xcol);
          comma = true;
        }
      pCol = pCol->GetNext();
    }
  SqlTriggerInsert += wxT(");\nEND");

// composing the UPDATE trigger
  SqlTriggerUpdate = wxT("CREATE TRIGGER ");
  strcpy(xview, ViewName.ToUTF8());
  sprintf(dummy, "vw_upd_%s", xview);
  MyFrame::DoubleQuotedSql(dummy);
  SqlTriggerUpdate += wxString::FromUTF8(dummy);
  SqlTriggerUpdate += wxT(" INSTEAD OF UPDATE OF\n    ");
  comma = false;
  pCol = Column1List.GetFirst();
  while (pCol)
    {
      if (pCol->IsSelected() == true)
        {
          if (pCol->GetName() == wxT("ROWID"))
            {
              pCol = pCol->GetNext();
              continue;
            }
          if (comma)
            SqlTriggerUpdate += wxT(", ");
          strcpy(xcol, pCol->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate += wxString::FromUTF8(xcol);
          comma = true;
        }
      pCol = pCol->GetNext();
    }
  if (Writable2 == true && Table2Enabled == true)
    {
      // supporting table #2
      pCol = Column2List.GetFirst();
      while (pCol)
        {
          if (pCol->IsSelected() == true)
            {
              if (pCol->GetName() == wxT("ROWID"))
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (comma)
                SqlTriggerUpdate += wxT(", ");
              strcpy(xcol, pCol->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xcol);
              SqlTriggerUpdate += wxString::FromUTF8(xcol);
              comma = true;
            }
          pCol = pCol->GetNext();
        }
    }
  SqlTriggerUpdate += wxT("\n    ON ");
  MyFrame::DoubleQuotedSql(xview);
  SqlTriggerUpdate += wxString::FromUTF8(xview);
  SqlTriggerUpdate += wxT("\nBEGIN\nUPDATE ");
  if (Writable2 == true && Table2Enabled == true)
    {
      // supporting table #2
      strcpy(xname, TableName2.ToUTF8());
      MyFrame::DoubleQuotedSql(xname);
      SqlTriggerUpdate += wxString::FromUTF8(xname);
      SqlTriggerUpdate += wxT(" SET ");
      comma = false;
      pCol = Column2List.GetFirst();
      while (pCol)
        {
          if (pCol->IsSelected() == true)
            {
              if (pCol->GetName() == wxT("ROWID"))
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (pCol->GetName() == Match1Table2)
                {
                  pCol = pCol->GetNext();
                  continue;
                }
              if (Match2Enabled == true)
                {
                  if (pCol->GetName() == Match2Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (Match3Enabled == true)
                {
                  if (pCol->GetName() == Match3Table2)
                    {
                      pCol = pCol->GetNext();
                      continue;
                    }
                }
              if (comma)
                SqlTriggerUpdate += wxT(", ");
              strcpy(xcol, pCol->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xcol);
              SqlTriggerUpdate +=
                wxString::FromUTF8(xcol) + wxT(" = NEW.") +
                wxString::FromUTF8(xcol);
              comma = true;
            }
          pCol = pCol->GetNext();
        }
      SqlTriggerUpdate += wxT("\nWHERE ");
      strcpy(xcol, Match1Table2.ToUTF8());
      MainFrame->DoubleQuotedSql(xcol);
      SqlTriggerUpdate += wxString::FromUTF8(xcol);
      strcpy(xcol, Match1Table1.ToUTF8());
      MainFrame->DoubleQuotedSql(xcol);
      SqlTriggerUpdate += wxT(" = NEW.") + wxString::FromUTF8(xcol);
      if (Match2Enabled == true)
        {
          if (AndOr12 == true)
            SqlTriggerUpdate += wxT(" AND ");
          else
            SqlTriggerUpdate += wxT(" OR ");
          strcpy(xcol, Match2Table2.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate += wxT(" = NEW.") + wxString::FromUTF8(xcol);
          strcpy(xcol, Match2Table1.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate += wxT("NEW.") + wxString::FromUTF8(xcol);
        }
      if (Match3Enabled == true)
        {

          if (AndOr23 == true)
            SqlTriggerUpdate += wxT(" AND ");
          else
            SqlTriggerUpdate += wxT(" OR ");
          strcpy(xcol, Match3Table2.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate += wxT(" = NEW.") + wxString::FromUTF8(xcol);
          strcpy(xcol, Match3Table1.ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate += wxT("NEW.") + wxString::FromUTF8(xcol);
        }
      SqlTriggerUpdate += wxT(";\nUPDATE");
    }
  strcpy(xname, TableName1.ToUTF8());
  MyFrame::DoubleQuotedSql(xname);
  SqlTriggerUpdate += wxString::FromUTF8(xname);
  SqlTriggerUpdate += wxT(" SET ");
  comma = false;
  pCol = Column1List.GetFirst();
  while (pCol)
    {
      if (pCol->IsSelected() == true)
        {
          if (pCol->GetName() == wxT("ROWID"))
            {
              pCol = pCol->GetNext();
              continue;
            }
          if (comma)
            SqlTriggerUpdate += wxT(", ");
          strcpy(xcol, pCol->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xcol);
          SqlTriggerUpdate +=
            wxString::FromUTF8(xcol) + wxT(" = NEW.") +
            wxString::FromUTF8(xcol);
          comma = true;
        }
      pCol = pCol->GetNext();
    }
  SqlTriggerUpdate += wxT("\nWHERE ROWID = OLD.ROWID;\nEND");

// composing the DELETE trigger
  SqlTriggerDelete = wxT("CREATE TRIGGER ");
  strcpy(xview, ViewName.ToUTF8());
  sprintf(dummy, "vw_del_%s", xview);
  MyFrame::DoubleQuotedSql(dummy);
  SqlTriggerDelete += wxString::FromUTF8(dummy);
  SqlTriggerDelete += wxT(" INSTEAD OF DELETE ON ");
  MyFrame::DoubleQuotedSql(xview);
  SqlTriggerDelete += wxString::FromUTF8(xview);
  SqlTriggerDelete += wxT("\nBEGIN\nDELETE FROM ");
  strcpy(xname, TableName1.ToUTF8());
  MyFrame::DoubleQuotedSql(xname);
  SqlTriggerDelete += wxString::FromUTF8(xname);
  SqlTriggerDelete += wxT(" WHERE ROWID = OLD.ROWID;\nEND");
}

bool ComposerDialog::SqlCleanString(wxString & dirty, wxString & clean)
{
// well-formatting a string to be used as an SQL string
  char org[8192];
  char dst[8192];
  int len;
  char *in = org;
  char *out = dst;
  bool ret = false;
  strcpy(org, dirty.ToUTF8());
  len = strlen(org);
  if (len > 2 && *org == '\'' && *(org + (len - 1)) == '\'')
    {
      strcpy(dst, org);
      ret = true;
  } else
    {
      while (*in != '\0')
        {
          if (*in == '\'')
            *out++ = '\'';
          *out++ = *in++;
        }
      *out = '\0';
    }
  clean = wxString::FromUTF8(dst);
  return ret;
}

void ComposerDialog::SqlCleanList(wxString & dirty, wxString & clean,
                                  int *style, int *start, int *stop, int *next,
                                  int base)
{
// well-formatting a string to be used as an SQL values list [IN]
  wxString token[256];
  int nextToken = 0;
  int i;
  bool quotedList;
  char org[8192];
  char dst[8192];
  char *in = org;
  char *out = dst;
  strcpy(org, dirty.ToUTF8());
  while (1)
    {
      if (*in == '\0')
        {
          *out = '\0';
          token[nextToken++] = wxString::FromUTF8(dst);
          break;
        }
      if (*in == ',')
        {
          *out = '\0';
          token[nextToken++] = wxString::FromUTF8(dst);
          in++;
          out = dst;
          continue;
        }
      *out++ = *in++;
    }
  for (i = 0; i < nextToken; i++)
    {
      wxString str = token[i].Trim(false);
      token[i] = str.Trim(true);
    }
  quotedList = false;
  for (i = 0; i < nextToken; i++)
    {
      bool isNumber = false;
      long lngTest;
      double dblTest;
      if (token[i].Len() == 0)
        continue;
      if (token[i].ToLong(&lngTest) == true)
        isNumber = true;
      if (token[i].ToDouble(&dblTest) == true)
        isNumber = true;
      if (isNumber == false)
        quotedList = true;
    }
  if (quotedList == true)
    {
      for (i = 0; i < nextToken; i++)
        {
          if (token[i].Len() == 0)
            continue;
          wxString str;
          SqlCleanString(token[i], str);
          token[i] = str;
        }
    }
  clean = wxT("");
  bool comma = false;
  for (i = 0; i < nextToken; i++)
    {
      if (token[i].Len() == 0)
        continue;
      if (comma == true)
        {
          style[*next] = 2;
          start[*next] = base + clean.Len();
          clean += wxT(", ");
          stop[(*next)++] = base + clean.Len();
        }
      if (quotedList == true)
        clean += wxT("'");
      clean += token[i];
      if (quotedList == true)
        clean += wxT("'");
      comma = true;
    }
}

bool ComposerMainPage::Create(ComposerDialog * parent)
{
//
// creating the dialog
//
  Parent = parent;
  if (wxPanel::Create(Parent->GetTabCtrl()) == false)
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

void ComposerMainPage::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the TABLES row
  wxBoxSizer *tablesSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tablesSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
// the MAIN TABLE pane
  wxBoxSizer *mainTblBoxSizer = new wxBoxSizer(wxVERTICAL);
  tablesSizer->Add(mainTblBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *mainTblBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Main Table"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *mainTblSizer = new wxStaticBoxSizer(mainTblBox, wxVERTICAL);
  mainTblBoxSizer->Add(mainTblSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  int count = Parent->GetTableList()->GetCount();
  wxString *tables = new wxString[count];
  int i = 0;
  AuxTable *pTable = Parent->GetTableList()->GetFirst();
  while (pTable)
    {
      tables[i] = pTable->GetTableName();
      i++;
      pTable = pTable->GetNext();
    }
  Table1NameCtrl =
    new wxComboBox(this, ID_QVC_TABLE_NAME_1, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), count, tables,
                   wxCB_DROPDOWN | wxCB_READONLY);
  mainTblSizer->Add(Table1NameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxBoxSizer *alias1Sizer = new wxBoxSizer(wxHORIZONTAL);
  mainTblSizer->Add(alias1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *aliasTbl1 = new wxStaticText(this, wxID_STATIC, wxT("&Alias:"));
  alias1Sizer->Add(aliasTbl1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);

  Table1AliasCtrl =
    new wxTextCtrl(this, ID_QVC_TABLE_ALIAS_1, Parent->GetTableAlias1(),
                   wxDefaultPosition, wxDefaultSize);
  alias1Sizer->Add(Table1AliasCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Table1ColumnsCtrl = new wxListBox(this, ID_QVC_COLUMNS_1,
                                    wxDefaultPosition, wxSize(130, 165),
                                    0, NULL,
                                    wxLB_HSCROLL | wxLB_NEEDED_SB |
                                    wxLB_EXTENDED);
  mainTblSizer->Add(Table1ColumnsCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// the TABLE#2 pane
  wxBoxSizer *tbl2BoxSizer = new wxBoxSizer(wxVERTICAL);
  tablesSizer->Add(tbl2BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *tbl2Box = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Table #2"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *tbl2Sizer = new wxStaticBoxSizer(tbl2Box, wxVERTICAL);
  tbl2BoxSizer->Add(tbl2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Table2Ctrl = new wxCheckBox(this, ID_QVC_TABLE_2, wxT("Enable"));
  Table2Ctrl->SetValue(false);
  tbl2Sizer->Add(Table2Ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  count = Parent->GetTableList()->GetCount();
  wxString *tables2 = new wxString[count];
  i = 0;
  pTable = Parent->GetTableList()->GetFirst();
  while (pTable)
    {
      tables2[i] = pTable->GetTableName();
      i++;
      pTable = pTable->GetNext();
    }
  Table2NameCtrl =
    new wxComboBox(this, ID_QVC_TABLE_NAME_2, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), count, tables2,
                   wxCB_DROPDOWN | wxCB_READONLY);
  Table2NameCtrl->Enable(false);
  tbl2Sizer->Add(Table2NameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *alias2Sizer = new wxBoxSizer(wxHORIZONTAL);
  tbl2Sizer->Add(alias2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticText *aliasTbl2 = new wxStaticText(this, wxID_STATIC, wxT("&Alias:"));
  alias2Sizer->Add(aliasTbl2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  Table2AliasCtrl =
    new wxTextCtrl(this, ID_QVC_TABLE_ALIAS_2, Parent->GetTableAlias2(),
                   wxDefaultPosition, wxDefaultSize);
  Table2AliasCtrl->Enable(false);
  alias2Sizer->Add(Table2AliasCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Table2ColumnsCtrl = new wxListBox(this, ID_QVC_COLUMNS_2,
                                    wxDefaultPosition, wxSize(130, 80),
                                    0, NULL,
                                    wxLB_HSCROLL | wxLB_NEEDED_SB |
                                    wxLB_EXTENDED);
  Table2ColumnsCtrl->Enable(false);
  tbl2Sizer->Add(Table2ColumnsCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxString join1Modes[2];
  join1Modes[0] = wxT("[&Inner] Join");
  join1Modes[1] = wxT("&Left [Outer] Join");
  JoinModeCtrl = new wxRadioBox(this, ID_QVC_JOIN_MODE,
                                wxT("Join mode"),
                                wxDefaultPosition, wxDefaultSize, 2,
                                join1Modes, 1, wxRA_SPECIFY_COLS);
  JoinModeCtrl->SetSelection(0);
  JoinModeCtrl->Enable(false);
  tbl2Sizer->Add(JoinModeCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// the JOIN condition pane
  wxBoxSizer *joinSizer = new wxBoxSizer(wxVERTICAL);
  tablesSizer->Add(joinSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

// JOIN match #1
  wxStaticBox *match1Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Join match #1"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *match1Sizer = new wxStaticBoxSizer(match1Box, wxHORIZONTAL);
  joinSizer->Add(match1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticBox *match1tbl1Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Main Table column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match1tbl1Sizer = new wxStaticBoxSizer(match1tbl1Box, wxVERTICAL);
  match1Sizer->Add(match1tbl1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match1Table1Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_1_T1, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match1Table1Ctrl->Enable(false);
  match1tbl1Sizer->Add(Match1Table1Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);
  wxStaticBox *match1tbl2Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Table #2 column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match1tbl2Sizer = new wxStaticBoxSizer(match1tbl2Box, wxVERTICAL);
  match1Sizer->Add(match1tbl2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match1Table2Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_1_T2, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match1Table2Ctrl->Enable(false);
  match1tbl2Sizer->Add(Match1Table2Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);

// JOIN match #2
  wxStaticBox *match2Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Join match #2"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *match20Sizer = new wxStaticBoxSizer(match2Box, wxVERTICAL);
  joinSizer->Add(match20Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Match2Ctrl = new wxCheckBox(this, ID_QVC_MATCH_2_ENABLE, wxT("Enable"));
  Match2Ctrl->SetValue(false);
  Match2Ctrl->Enable(false);
  match20Sizer->Add(Match2Ctrl, 0, wxALIGN_LEFT | wxALL, 1);
  wxBoxSizer *match2Sizer = new wxBoxSizer(wxHORIZONTAL);
  match20Sizer->Add(match2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *match2tbl1Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Main Table column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match2tbl1Sizer = new wxStaticBoxSizer(match2tbl1Box, wxVERTICAL);
  match2Sizer->Add(match2tbl1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match2Table1Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_2_T1, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match2Table1Ctrl->Enable(false);
  match2tbl1Sizer->Add(Match2Table1Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);
  wxStaticBox *match2tbl2Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Table #2 column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match2tbl2Sizer = new wxStaticBoxSizer(match2tbl2Box, wxVERTICAL);
  match2Sizer->Add(match2tbl2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match2Table2Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_2_T2, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match2Table2Ctrl->Enable(false);
  match2tbl2Sizer->Add(Match2Table2Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);

// JOIN match #3
  wxStaticBox *match3Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Join match #3"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *match30Sizer = new wxStaticBoxSizer(match3Box, wxVERTICAL);
  joinSizer->Add(match30Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Match3Ctrl = new wxCheckBox(this, ID_QVC_MATCH_3_ENABLE, wxT("Enable"));
  Match3Ctrl->SetValue(false);
  Match3Ctrl->Enable(false);
  match30Sizer->Add(Match3Ctrl, 0, wxALIGN_LEFT | wxALL, 1);
  wxBoxSizer *match3Sizer = new wxBoxSizer(wxHORIZONTAL);
  match30Sizer->Add(match3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *match3tbl1Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Main Table column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match3tbl1Sizer = new wxStaticBoxSizer(match3tbl1Box, wxVERTICAL);
  match3Sizer->Add(match3tbl1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match3Table1Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_3_T1, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match3Table1Ctrl->Enable(false);
  match3tbl1Sizer->Add(Match3Table1Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);
  wxStaticBox *match3tbl2Box = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Table #2 column"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
  wxBoxSizer *match3tbl2Sizer = new wxStaticBoxSizer(match3tbl2Box, wxVERTICAL);
  match3Sizer->Add(match3tbl2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Match3Table2Ctrl =
    new wxComboBox(this, ID_QVC_MATCH_3_T2, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Match3Table2Ctrl->Enable(false);
  match3tbl2Sizer->Add(Match3Table2Ctrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       0);

// adding the event handlers
  Connect(ID_QVC_TABLE_2, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerMainPage::OnTable2Enabled);
  Connect(ID_QVC_TABLE_NAME_1, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnTable1Selected);
  Connect(ID_QVC_TABLE_NAME_2, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnTable2Selected);
  Connect(ID_QVC_TABLE_ALIAS_1, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerMainPage::OnTable1AliasChanged);
  Connect(ID_QVC_TABLE_ALIAS_2, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerMainPage::OnTable2AliasChanged);
  Connect(ID_QVC_COLUMNS_1, wxEVT_COMMAND_LISTBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnColumns1Selected);
  Connect(ID_QVC_COLUMNS_2, wxEVT_COMMAND_LISTBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnColumns2Selected);
  Connect(ID_QVC_JOIN_MODE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnJoinModeChanged);
  Connect(ID_QVC_MATCH_2_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch2Enabled);
  Connect(ID_QVC_MATCH_3_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch3Enabled);
  Connect(ID_QVC_MATCH_1_T1, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch1Table1Selected);
  Connect(ID_QVC_MATCH_1_T2, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch1Table2Selected);
  Connect(ID_QVC_MATCH_2_T1, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch2Table1Selected);
  Connect(ID_QVC_MATCH_2_T2, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch2Table2Selected);
  Connect(ID_QVC_MATCH_3_T1, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch3Table1Selected);
  Connect(ID_QVC_MATCH_3_T2, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerMainPage::OnMatch3Table2Selected);
}

void ComposerMainPage::SelectGeometryColumn(wxString & column, bool table2)
{
// selecting the geometry column
  wxListBox *lb = Table1ColumnsCtrl;
  if (table2 == true)
    lb = Table2ColumnsCtrl;
  int sel = lb->FindString(column);
  if (sel != wxNOT_FOUND)
    lb->Select(sel);
}

void ComposerMainPage::InitializeComboColumns(wxComboBox * ctrl, bool table2)
{
//
// initializing a column list ComboBox
//
  ctrl->Clear();
  AuxColumn *pColumn;
  if (table2 == false)
    pColumn = Parent->GetColumn1List()->GetFirst();
  else
    pColumn = Parent->GetColumn2List()->GetFirst();
  while (pColumn)
    {
      wxString col = pColumn->GetName();
      ctrl->Append(col);
      pColumn = pColumn->GetNext();
    }
}

void ComposerMainPage::InitializeListColumns(wxListBox * ctrl, bool table2)
{
//
// initializing a column list ListBox
//
  ctrl->Clear();
  AuxColumn *pColumn;
  if (table2 == false)
    pColumn = Parent->GetColumn1List()->GetFirst();
  else
    pColumn = Parent->GetColumn2List()->GetFirst();
  while (pColumn)
    {
      wxString col = pColumn->GetName();
      ctrl->Append(col);
      pColumn = pColumn->GetNext();
    }
}

void ComposerMainPage::OnTable2Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Table#2 enabled/disabled
//
  Parent->SetTable2Enabled(Table2Ctrl->GetValue());
  if (Parent->IsTable2Enabled() == true)
    {
      Table2NameCtrl->Enable(true);
      Table2AliasCtrl->Enable(true);
      Table2ColumnsCtrl->Enable(true);
      JoinModeCtrl->Enable(true);
      if (Parent->IsLeftJoin() == false)
        JoinModeCtrl->SetSelection(0);
      else
        JoinModeCtrl->SetSelection(1);
      Match2Ctrl->Enable(true);
      Match3Ctrl->Enable(true);
      InitializeComboColumns(Match1Table1Ctrl, false);
      Match1Table1Ctrl->Enable(true);
      Match1Table2Ctrl->Enable(true);
      if (Parent->IsMatch2Enabled() == true)
        {
          Match2Table1Ctrl->Enable(true);
          Match2Table2Ctrl->Enable(true);
          InitializeComboColumns(Match2Table1Ctrl, false);
      } else
        {
          Match2Table1Ctrl->Enable(false);
          Match2Table2Ctrl->Enable(false);
        }
      if (Parent->IsMatch3Enabled() == true)
        {
          Match3Table1Ctrl->Enable(true);
          Match3Table2Ctrl->Enable(true);
          InitializeComboColumns(Match3Table1Ctrl, false);
      } else
        {
          Match3Table1Ctrl->Enable(false);
          Match3Table2Ctrl->Enable(false);
        }
  } else
    {
      Table2NameCtrl->Enable(false);
      Parent->SetTableName2(wxT(""));
      Table2NameCtrl->SetSelection(wxNOT_FOUND);
      Table2AliasCtrl->Enable(false);
      Table2ColumnsCtrl->Enable(false);
      Parent->GetColumn2List()->Flush();
      Table2ColumnsCtrl->Clear();
      Parent->SetMatch2Enabled(false);
      Parent->SetMatch3Enabled(false);
      Match2Ctrl->SetValue(Parent->IsMatch2Enabled());
      Match3Ctrl->SetValue(Parent->IsMatch3Enabled());
      Match2Ctrl->Enable(false);
      Match3Ctrl->Enable(false);
      JoinModeCtrl->Enable(false);
      Match1Table1Ctrl->Enable(false);
      Match1Table2Ctrl->Enable(false);
      Match2Table1Ctrl->Enable(false);
      Match2Table2Ctrl->Enable(false);
      Match3Table1Ctrl->Enable(false);
      Match3Table2Ctrl->Enable(false);
      Match1Table1Ctrl->Clear();
      Match1Table2Ctrl->Clear();
      Match2Table1Ctrl->Clear();
      Match2Table2Ctrl->Clear();
      Match3Table1Ctrl->Clear();
      Match3Table2Ctrl->Clear();
      Parent->SetMatch1Table1(wxT(""));
      Parent->SetMatch2Table1(wxT(""));
      Parent->SetMatch3Table1(wxT(""));
      Parent->SetMatch1Table2(wxT(""));
      Parent->SetMatch2Table2(wxT(""));
      Parent->SetMatch3Table2(wxT(""));
      Parent->Table2Status(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch2Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Match#2 enabled/disabled
//
  Parent->SetMatch2Enabled(Match2Ctrl->GetValue());
  if (Parent->IsMatch2Enabled() == true)
    {
      Match2Table1Ctrl->Enable(true);
      Match2Table2Ctrl->Enable(true);
      InitializeComboColumns(Match2Table1Ctrl, false);
      InitializeComboColumns(Match2Table2Ctrl, true);
  } else
    {
      Match2Table1Ctrl->Enable(false);
      Match2Table2Ctrl->Enable(false);
      Parent->SetMatch2Table1(wxT(""));
      Parent->SetMatch2Table2(wxT(""));
      Match2Table1Ctrl->Clear();
      Match2Table2Ctrl->Clear();
    }
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch3Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Match#3 enabled/disabled
//
  Parent->SetMatch3Enabled(Match3Ctrl->GetValue());
  if (Parent->IsMatch3Enabled() == true)
    {
      Match3Table1Ctrl->Enable(true);
      Match3Table2Ctrl->Enable(true);
      InitializeComboColumns(Match3Table1Ctrl, false);
      InitializeComboColumns(Match3Table2Ctrl, true);
  } else
    {
      Match3Table1Ctrl->Enable(false);
      Match3Table2Ctrl->Enable(false);
      Parent->SetMatch3Table1(wxT(""));
      Parent->SetMatch3Table2(wxT(""));
      Match3Table1Ctrl->Clear();
      Match3Table2Ctrl->Clear();
    }
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnTable1AliasChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Table#1 alias-name changed
//
  Parent->SetTableAlias1(Table1AliasCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnTable2AliasChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Table#2 alias-name changed
//
  Parent->SetTableAlias2(Table2AliasCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnJoinModeChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Join mode changed
//
  if (JoinModeCtrl->GetSelection() == 1)
    Parent->SetLeftJoin(true);
  else
    Parent->SetLeftJoin(false);
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnTable1Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Table#1 selection changed
//
  bool changed = true;
  if (Table1NameCtrl->GetValue() == Parent->GetTableName1())
    changed = false;
  Parent->SetTableName1(Table1NameCtrl->GetValue());
  Parent->PopulateColumnList1();
  InitializeListColumns(Table1ColumnsCtrl, false);
  InitializeComboColumns(Match1Table1Ctrl, false);
  InitializeComboColumns(Match2Table1Ctrl, false);
  InitializeComboColumns(Match3Table1Ctrl, false);
  Parent->SetMatch1Table1(wxT(""));
  Parent->SetMatch2Table1(wxT(""));
  Parent->SetMatch3Table1(wxT(""));
  if (Parent->GetTableName1().Len() == 0)
    Parent->Table1Status(false);
  else
    {
      if (changed == true)
        Parent->Table1Status(false);
      Parent->Table1Status(true);
    }
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnTable2Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Table#2 selection changed
//
  bool changed = true;
  if (Table2NameCtrl->GetValue() == Parent->GetTableName2())
    changed = false;
  Parent->SetTableName2(Table2NameCtrl->GetValue());
  Parent->PopulateColumnList2();
  Table2ColumnsCtrl->Clear();
  InitializeListColumns(Table2ColumnsCtrl, true);
  InitializeComboColumns(Match1Table2Ctrl, true);
  InitializeComboColumns(Match2Table2Ctrl, true);
  InitializeComboColumns(Match3Table2Ctrl, true);
  Parent->SetMatch1Table2(wxT(""));
  Parent->SetMatch2Table2(wxT(""));
  Parent->SetMatch3Table2(wxT(""));
  if (Parent->GetTableName2().Len() == 0)
    Parent->Table2Status(false);
  else
    {
      if (changed == true)
        Parent->Table2Status(false);
      Parent->Table2Status(true);
    }
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnColumns1Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Table#1 columns selection changed
//
  int i;
  int count = Table1ColumnsCtrl->GetCount();
  for (i = 0; i < count; i++)
    {
      // marking the selected columns
      if (Table1ColumnsCtrl->IsSelected(i) == true)
        Parent->GetColumn1List()->SetState(i, true);
      else
        Parent->GetColumn1List()->SetState(i, false);
    }
  Parent->SelectGeometryColumn();
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnColumns2Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Table#2 columns selection changed
//
  int i;
  int count = Table2ColumnsCtrl->GetCount();
  for (i = 0; i < count; i++)
    {
      // marking the selected columns
      if (Table2ColumnsCtrl->IsSelected(i) == true)
        Parent->GetColumn2List()->SetState(i, true);
      else
        Parent->GetColumn2List()->SetState(i, false);
    }
  Parent->SelectGeometryColumn();
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch1Table1Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #1 table #1 selection changed
//
  Parent->SetMatch1Table1(Match1Table1Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch1Table2Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #1 table #2 selection changed
//
  Parent->SetMatch1Table2(Match1Table2Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch2Table1Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #2 table #1 selection changed
//
  Parent->SetMatch2Table1(Match2Table1Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch2Table2Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #2 table #2 selection changed
//
  Parent->SetMatch2Table2(Match2Table2Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch3Table1Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #3 table #1 selection changed
//
  Parent->SetMatch3Table1(Match3Table1Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerMainPage::OnMatch3Table2Selected(wxCommandEvent & WXUNUSED(event))
{
//
// Match #3 table #2 selection changed
//
  Parent->SetMatch3Table2(Match3Table2Ctrl->GetValue());
  Parent->UpdateSqlSample();
}

bool ComposerFilterPage::Create(ComposerDialog * parent)
{
//
// creating the dialog
//
  Parent = parent;
  if (wxPanel::Create(Parent->GetTabCtrl()) == false)
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

void ComposerFilterPage::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxSize size;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the WHERE clauses row
  wxBoxSizer *whereSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(whereSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the WHERE clause #1 pane
  wxBoxSizer *where1BoxSizer = new wxBoxSizer(wxVERTICAL);
  whereSizer->Add(where1BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *where1Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Filter #1"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *where1Sizer = new wxStaticBoxSizer(where1Box, wxVERTICAL);
  where1BoxSizer->Add(where1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where1EnabledCtrl =
    new wxCheckBox(this, ID_QVC_WHERE_1_ENABLE, wxT("Enable"));
  Where1EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Where1EnabledCtrl->Enable(false);
  where1Sizer->Add(Where1EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString where1Tables[2];
  where1Tables[0] = wxT("&Main table columns");
  where1Tables[1] = wxT("&Table #2 columns");
  Where1TableCtrl = new wxRadioBox(this, ID_QVC_WHERE_1_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   where1Tables, 1, wxRA_SPECIFY_COLS);
  Where1TableCtrl->SetSelection(0);
  Where1TableCtrl->Enable(false);
  where1Sizer->Add(Where1TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxStaticBox *cols1Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Column to be filtered"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *cols1Sizer = new wxStaticBoxSizer(cols1Box, wxVERTICAL);
  where1Sizer->Add(cols1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where1ColumnCtrl =
    new wxComboBox(this, ID_QVC_WHERE_1_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where1ColumnCtrl->Enable(false);
  cols1Sizer->Add(Where1ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *oper1Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Comparison operator"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *oper1Sizer = new wxStaticBoxSizer(oper1Box, wxVERTICAL);
  where1Sizer->Add(oper1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where1OperatorCtrl =
    new wxComboBox(this, ID_QVC_WHERE_1_OPERATOR, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where1OperatorCtrl->Enable(false);
  oper1Sizer->Add(Where1OperatorCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  size = Where1OperatorCtrl->GetSize();
  wxStaticBox *value1Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Value"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *value1Sizer = new wxStaticBoxSizer(value1Box, wxVERTICAL);
  where1Sizer->Add(value1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where1ValueCtrl =
    new wxTextCtrl(this, ID_QVC_WHERE_1_VALUE, wxT(""), wxDefaultPosition,
                   size);
  Where1ValueCtrl->Enable(false);
  value1Sizer->Add(Where1ValueCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

// the CONNECTOR 1-2 pane
  wxString bool12[2];
  bool12[0] = wxT("&AND");
  bool12[1] = wxT("&OR");
  Connector12Ctrl = new wxRadioBox(this, ID_QVC_CONNECTOR_12,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   bool12, 1, wxRA_SPECIFY_COLS);
  Connector12Ctrl->SetSelection(0);
  Connector12Ctrl->Enable(false);
  whereSizer->Add(Connector12Ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);

// the WHERE clause #2 pane
  wxBoxSizer *where2BoxSizer = new wxBoxSizer(wxVERTICAL);
  whereSizer->Add(where2BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *where2Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Filter #2"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *where2Sizer = new wxStaticBoxSizer(where2Box, wxVERTICAL);
  where2BoxSizer->Add(where2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where2EnabledCtrl =
    new wxCheckBox(this, ID_QVC_WHERE_2_ENABLE, wxT("Enable"));
  Where2EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Where2EnabledCtrl->Enable(false);
  where2Sizer->Add(Where2EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString where2Tables[2];
  where2Tables[0] = wxT("&Main table columns");
  where2Tables[1] = wxT("&Table #2 columns");
  Where2TableCtrl = new wxRadioBox(this, ID_QVC_WHERE_2_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   where2Tables, 1, wxRA_SPECIFY_COLS);
  Where2TableCtrl->SetSelection(0);
  Where2TableCtrl->Enable(false);
  where2Sizer->Add(Where2TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxStaticBox *cols2Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Column to be filtered"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *cols2Sizer = new wxStaticBoxSizer(cols2Box, wxVERTICAL);
  where2Sizer->Add(cols2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where2ColumnCtrl =
    new wxComboBox(this, ID_QVC_WHERE_2_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where2ColumnCtrl->Enable(false);
  cols2Sizer->Add(Where2ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *oper2Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Comparison operator"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *oper2Sizer = new wxStaticBoxSizer(oper2Box, wxVERTICAL);
  where2Sizer->Add(oper2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where2OperatorCtrl =
    new wxComboBox(this, ID_QVC_WHERE_2_OPERATOR, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where2OperatorCtrl->Enable(false);
  oper2Sizer->Add(Where2OperatorCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  size = Where2OperatorCtrl->GetSize();
  wxStaticBox *value2Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Value"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *value2Sizer = new wxStaticBoxSizer(value2Box, wxVERTICAL);
  where2Sizer->Add(value2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where2ValueCtrl =
    new wxTextCtrl(this, ID_QVC_WHERE_2_VALUE, wxT(""), wxDefaultPosition,
                   size);
  Where2ValueCtrl->Enable(false);
  value2Sizer->Add(Where2ValueCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

// the CONNECTOR 2-3 pane
  wxString bool23[2];
  bool23[0] = wxT("&AND");
  bool23[1] = wxT("&OR");
  Connector23Ctrl = new wxRadioBox(this, ID_QVC_CONNECTOR_23,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   bool12, 1, wxRA_SPECIFY_COLS);
  Connector23Ctrl->SetSelection(0);
  Connector23Ctrl->Enable(false);
  whereSizer->Add(Connector23Ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);

// the WHERE clause #3 pane
  wxBoxSizer *where3BoxSizer = new wxBoxSizer(wxVERTICAL);
  whereSizer->Add(where3BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *where3Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Filter #3"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *where3Sizer = new wxStaticBoxSizer(where3Box, wxVERTICAL);
  where3BoxSizer->Add(where3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where3EnabledCtrl =
    new wxCheckBox(this, ID_QVC_WHERE_3_ENABLE, wxT("Enable"));
  Where3EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Where3EnabledCtrl->Enable(false);
  where3Sizer->Add(Where3EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString where3Tables[2];
  where3Tables[0] = wxT("&Main table columns");
  where3Tables[1] = wxT("&Table #2 columns");
  Where3TableCtrl = new wxRadioBox(this, ID_QVC_WHERE_3_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   where2Tables, 1, wxRA_SPECIFY_COLS);
  Where3TableCtrl->SetSelection(0);
  Where3TableCtrl->Enable(false);
  where3Sizer->Add(Where3TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxStaticBox *cols3Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Column to be filtered"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *cols3Sizer = new wxStaticBoxSizer(cols3Box, wxVERTICAL);
  where3Sizer->Add(cols3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where3ColumnCtrl =
    new wxComboBox(this, ID_QVC_WHERE_3_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where3ColumnCtrl->Enable(false);
  cols3Sizer->Add(Where3ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *oper3Box = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Comparison operator"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *oper3Sizer = new wxStaticBoxSizer(oper3Box, wxVERTICAL);
  where3Sizer->Add(oper3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where3OperatorCtrl =
    new wxComboBox(this, ID_QVC_WHERE_3_OPERATOR, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Where3OperatorCtrl->Enable(false);
  oper3Sizer->Add(Where3OperatorCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  size = Where3OperatorCtrl->GetSize();
  wxStaticBox *value3Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Value"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *value3Sizer = new wxStaticBoxSizer(value3Box, wxVERTICAL);
  where3Sizer->Add(value3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Where3ValueCtrl =
    new wxTextCtrl(this, ID_QVC_WHERE_3_VALUE, wxT(""), wxDefaultPosition,
                   size);
  Where3ValueCtrl->Enable(false);
  value3Sizer->Add(Where3ValueCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

// adding the event handlers
  Connect(ID_QVC_WHERE_1_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere1Enabled);
  Connect(ID_QVC_WHERE_2_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere2Enabled);
  Connect(ID_QVC_WHERE_3_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere3Enabled);
  Connect(ID_QVC_WHERE_1_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere1TableChanged);
  Connect(ID_QVC_WHERE_2_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere2TableChanged);
  Connect(ID_QVC_WHERE_3_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere3TableChanged);
  Connect(ID_QVC_CONNECTOR_12, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnConnector12Changed);
  Connect(ID_QVC_CONNECTOR_23, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnConnector23Changed);
  Connect(ID_QVC_WHERE_1_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere1ColumnSelected);
  Connect(ID_QVC_WHERE_2_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere2ColumnSelected);
  Connect(ID_QVC_WHERE_3_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere3ColumnSelected);
  Connect(ID_QVC_WHERE_1_OPERATOR, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) &
          ComposerFilterPage::OnWhere1OperatorSelected);
  Connect(ID_QVC_WHERE_2_OPERATOR, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) &
          ComposerFilterPage::OnWhere2OperatorSelected);
  Connect(ID_QVC_WHERE_3_OPERATOR, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) &
          ComposerFilterPage::OnWhere3OperatorSelected);
  Connect(ID_QVC_WHERE_1_VALUE, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere1ValueChanged);
  Connect(ID_QVC_WHERE_2_VALUE, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere2ValueChanged);
  Connect(ID_QVC_WHERE_3_VALUE, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerFilterPage::OnWhere3ValueChanged);
}

void ComposerFilterPage::Table1Status(bool ok)
{
//
// enabling/disabling Table#1 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName2().Len() == 0)
        {
          Where1EnabledCtrl->SetValue(false);
          Parent->SetWhere1Enabled(false);
          Where1EnabledCtrl->Enable(false);
          Where2EnabledCtrl->SetValue(false);
          Parent->SetWhere2Enabled(false);
          Where2EnabledCtrl->Enable(false);
          Where3EnabledCtrl->SetValue(false);
          Parent->SetWhere3Enabled(false);
          Where3EnabledCtrl->Enable(false);
          Where1TableCtrl->SetSelection(0);
          Parent->SetWhere1Table2(false);
          Where1TableCtrl->Enable(false);
          Parent->SetWhere1Column(wxT(""));
          Where1ColumnCtrl->Clear();
          Where1ColumnCtrl->Enable(false);
          Parent->SetWhere1Operator(wxT(""));
          Where1OperatorCtrl->Clear();
          Where1OperatorCtrl->Enable(false);
          Parent->SetWhere1Operator(wxT(""));
          Where1ValueCtrl->SetValue(wxT(""));
          Where1ValueCtrl->Enable(false);
          Where2TableCtrl->SetSelection(0);
          Parent->SetWhere2Table2(false);
          Where2TableCtrl->Enable(false);
          Parent->SetWhere2Column(wxT(""));
          Where2ColumnCtrl->Clear();
          Where2ColumnCtrl->Enable(false);
          Parent->SetWhere2Operator(wxT(""));
          Where2OperatorCtrl->Clear();
          Where2OperatorCtrl->Enable(false);
          Parent->SetWhere2Operator(wxT(""));
          Where2ValueCtrl->SetValue(wxT(""));
          Where2ValueCtrl->Enable(false);
          Where3TableCtrl->SetSelection(0);
          Parent->SetWhere3Table2(false);
          Where3TableCtrl->Enable(false);
          Parent->SetWhere3Column(wxT(""));
          Where3ColumnCtrl->Clear();
          Where3ColumnCtrl->Enable(false);
          Parent->SetWhere3Operator(wxT(""));
          Where3OperatorCtrl->Clear();
          Where3OperatorCtrl->Enable(false);
          Parent->SetWhere3Operator(wxT(""));
          Where3ValueCtrl->SetValue(wxT(""));
          Where3ValueCtrl->Enable(false);
          Parent->SetAndOr12(true);
          Connector12Ctrl->SetSelection(0);
          Connector12Ctrl->Enable(false);
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(false);
      } else
        {
          if (Parent->IsWhere1Table2() == false)
            {
              Where1EnabledCtrl->SetValue(false);
              Parent->SetWhere1Enabled(false);
              Where1TableCtrl->SetSelection(0);
              Parent->SetWhere1Table2(false);
              Where1TableCtrl->Enable(false);
              Parent->SetWhere1Column(wxT(""));
              Where1ColumnCtrl->Clear();
              Where1ColumnCtrl->Enable(false);
              Parent->SetWhere1Operator(wxT(""));
              Where1OperatorCtrl->Clear();
              Where1OperatorCtrl->Enable(false);
              Parent->SetWhere1Operator(wxT(""));
              Where1ValueCtrl->SetValue(wxT(""));
              Where1ValueCtrl->Enable(false);
              Parent->SetAndOr12(true);
              Connector12Ctrl->SetSelection(0);
              Connector12Ctrl->Enable(false);
            }
          if (Parent->IsWhere3Table2() == false)
            {
              Where3EnabledCtrl->SetValue(false);
              Parent->SetWhere3Enabled(false);
              Where3TableCtrl->SetSelection(0);
              Parent->SetWhere3Table2(false);
              Where3TableCtrl->Enable(false);
              Parent->SetWhere3Column(wxT(""));
              Where3ColumnCtrl->Clear();
              Where3ColumnCtrl->Enable(false);
              Parent->SetWhere3Operator(wxT(""));
              Where3OperatorCtrl->Clear();
              Where3OperatorCtrl->Enable(false);
              Where3ValueCtrl->SetValue(wxT(""));
              Where3ValueCtrl->Enable(false);
              Parent->SetAndOr23(true);
              Connector23Ctrl->SetSelection(0);
              Connector23Ctrl->Enable(false);
            }
          if (Parent->IsWhere2Table2() == false)
            {
              Where2EnabledCtrl->SetValue(false);
              Parent->SetWhere2Enabled(false);
              Where2TableCtrl->SetSelection(0);
              Parent->SetWhere2Table2(false);
              Where2TableCtrl->Enable(false);
              Parent->SetWhere2Column(wxT(""));
              Where2ColumnCtrl->Clear();
              Where2ColumnCtrl->Enable(false);
              Parent->SetWhere2Operator(wxT(""));
              Where2OperatorCtrl->Clear();
              Where2OperatorCtrl->Enable(false);
              Parent->SetWhere2Operator(wxT(""));
              Where2ValueCtrl->SetValue(wxT(""));
              Where2ValueCtrl->Enable(false);
              Parent->SetAndOr12(true);
              Connector12Ctrl->SetSelection(0);
              Connector12Ctrl->Enable(false);
              if (Parent->IsWhere3Enabled() == true
                  && Parent->IsWhere1Enabled() == true)
                ;
              else
                {
                  Parent->SetAndOr23(true);
                  Connector23Ctrl->SetSelection(0);
                  Connector23Ctrl->Enable(false);
                }
            }
        }
  } else
    {
      Where1EnabledCtrl->Enable(true);
      Where2EnabledCtrl->Enable(true);
      Where3EnabledCtrl->Enable(true);
    }
}

void ComposerFilterPage::Table2Status(bool ok)
{
//
// enabling/disabling Table#2 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName1().Len() == 0)
        {
          Where1EnabledCtrl->SetValue(false);
          Parent->SetWhere1Enabled(false);
          Where1EnabledCtrl->Enable(false);
          Where2EnabledCtrl->SetValue(false);
          Parent->SetWhere2Enabled(false);
          Where2EnabledCtrl->Enable(false);
          Where3EnabledCtrl->SetValue(false);
          Parent->SetWhere3Enabled(false);
          Where3EnabledCtrl->Enable(false);
          Where1TableCtrl->SetSelection(0);
          Parent->SetWhere1Table2(false);
          Where1TableCtrl->Enable(false);
          Parent->SetWhere1Column(wxT(""));
          Where1ColumnCtrl->Clear();
          Where1ColumnCtrl->Enable(false);
          Parent->SetWhere1Operator(wxT(""));
          Where1OperatorCtrl->Clear();
          Where1OperatorCtrl->Enable(false);
          Parent->SetWhere1Operator(wxT(""));
          Where1ValueCtrl->SetValue(wxT(""));
          Where1ValueCtrl->Enable(false);
          Where2TableCtrl->SetSelection(0);
          Parent->SetWhere2Table2(false);
          Where2TableCtrl->Enable(false);
          Parent->SetWhere2Column(wxT(""));
          Where2ColumnCtrl->Clear();
          Where2ColumnCtrl->Enable(false);
          Parent->SetWhere2Operator(wxT(""));
          Where2OperatorCtrl->Clear();
          Where2OperatorCtrl->Enable(false);
          Parent->SetWhere2Operator(wxT(""));
          Where2ValueCtrl->SetValue(wxT(""));
          Where2ValueCtrl->Enable(false);
          Where3TableCtrl->SetSelection(0);
          Parent->SetWhere3Table2(false);
          Where3TableCtrl->Enable(false);
          Parent->SetWhere3Column(wxT(""));
          Where3ColumnCtrl->Clear();
          Where3ColumnCtrl->Enable(false);
          Parent->SetWhere3Operator(wxT(""));
          Where3OperatorCtrl->Clear();
          Where3OperatorCtrl->Enable(false);
          Parent->SetWhere3Operator(wxT(""));
          Where3ValueCtrl->SetValue(wxT(""));
          Where3ValueCtrl->Enable(false);
          Parent->SetAndOr12(true);
          Connector12Ctrl->SetSelection(0);
          Connector12Ctrl->Enable(false);
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(false);
      } else
        {
          if (Parent->IsWhere1Table2() == true)
            {
              Where1EnabledCtrl->SetValue(false);
              Parent->SetWhere1Enabled(false);
              Where1TableCtrl->SetSelection(0);
              Parent->SetWhere1Table2(false);
              Where1TableCtrl->Enable(false);
              Parent->SetWhere1Column(wxT(""));
              Where1ColumnCtrl->Clear();
              Where1ColumnCtrl->Enable(false);
              Parent->SetWhere1Operator(wxT(""));
              Where1OperatorCtrl->Clear();
              Where1OperatorCtrl->Enable(false);
              Parent->SetWhere1Operator(wxT(""));
              Where1ValueCtrl->SetValue(wxT(""));
              Where1ValueCtrl->Enable(false);
              Parent->SetAndOr12(true);
              Connector12Ctrl->SetSelection(0);
              Connector12Ctrl->Enable(false);
            }
          if (Parent->IsWhere3Table2() == true)
            {
              Where3EnabledCtrl->SetValue(false);
              Parent->SetWhere3Enabled(false);
              Where3TableCtrl->SetSelection(0);
              Parent->SetWhere3Table2(false);
              Where3TableCtrl->Enable(false);
              Parent->SetWhere3Column(wxT(""));
              Where3ColumnCtrl->Clear();
              Where3ColumnCtrl->Enable(false);
              Parent->SetWhere3Operator(wxT(""));
              Where3OperatorCtrl->Clear();
              Where3OperatorCtrl->Enable(false);
              Where3ValueCtrl->SetValue(wxT(""));
              Where3ValueCtrl->Enable(false);
              Parent->SetAndOr23(true);
              Connector23Ctrl->SetSelection(0);
              Connector23Ctrl->Enable(false);
            }
          if (Parent->IsWhere2Table2() == true)
            {
              Where2EnabledCtrl->SetValue(false);
              Parent->SetWhere2Enabled(false);
              Where2TableCtrl->SetSelection(0);
              Parent->SetWhere2Table2(false);
              Where2TableCtrl->Enable(false);
              Parent->SetWhere2Column(wxT(""));
              Where2ColumnCtrl->Clear();
              Where2ColumnCtrl->Enable(false);
              Parent->SetWhere2Operator(wxT(""));
              Where2OperatorCtrl->Clear();
              Where2OperatorCtrl->Enable(false);
              Parent->SetWhere2Operator(wxT(""));
              Where2ValueCtrl->SetValue(wxT(""));
              Where2ValueCtrl->Enable(false);
              Parent->SetAndOr12(true);
              Connector12Ctrl->SetSelection(0);
              Connector12Ctrl->Enable(false);
              if (Parent->IsWhere3Enabled() == true
                  && Parent->IsWhere1Enabled() == true)
                ;
              else
                {
                  Parent->SetAndOr23(true);
                  Connector23Ctrl->SetSelection(0);
                  Connector23Ctrl->Enable(false);
                }
            }
        }
  } else
    {
      Where1EnabledCtrl->Enable(true);
      Where2EnabledCtrl->Enable(true);
      Where3EnabledCtrl->Enable(true);
    }
}

void ComposerFilterPage::InitializeOperators(wxComboBox * ctrl)
{
// initializing a list of boolean operators ComboBox
  ctrl->Clear();
  ctrl->Append(wxT("= {equal to}"));
  ctrl->Append(wxT("<> {not equal}"));
  ctrl->Append(wxT("< {lesser than}"));
  ctrl->Append(wxT("<= {lesser / equal}"));
  ctrl->Append(wxT("> {greather than}"));
  ctrl->Append(wxT(">= {greather / equal}"));
  ctrl->Append(wxT("LIKE {text search}"));
  ctrl->Append(wxT("IN {val1, val2, ...}"));
  ctrl->Append(wxT("IS NULL"));
  ctrl->Append(wxT("IS NOT NULL"));
}

void ComposerFilterPage::InitializeColumns(wxComboBox * ctrl, bool table2)
{
// initializing a columns list ComboBox
  ctrl->Clear();
  AuxColumn *pColumn;
  if (table2 == false)
    pColumn = Parent->GetColumn1List()->GetFirst();
  else
    pColumn = Parent->GetColumn2List()->GetFirst();
  while (pColumn)
    {
      wxString col = pColumn->GetName();
      ctrl->Append(col);
      pColumn = pColumn->GetNext();
    }
}

void ComposerFilterPage::OnWhere1Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Where #1 clause enabled/disabled
//
  Parent->SetWhere1Enabled(Where1EnabledCtrl->GetValue());
  if (Parent->IsWhere1Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsWhere1Table2() == false)
            Where1TableCtrl->SetSelection(0);
          else
            Where1TableCtrl->SetSelection(1);
          Where1TableCtrl->Enable(true);
      } else
        {
          Parent->SetWhere1Table2(false);
          Where1TableCtrl->SetSelection(0);
          Where1TableCtrl->Enable(false);
          Parent->SetWhere1Column(wxT(""));
          Where1ColumnCtrl->Clear();
        }
      Parent->SetWhere1Column(wxT(""));
      InitializeColumns(Where1ColumnCtrl, Parent->IsWhere1Table2());
      Where1ColumnCtrl->Enable(true);
      Parent->SetWhere1Operator(wxT(""));
      InitializeOperators(Where1OperatorCtrl);
      Where1OperatorCtrl->Enable(true);
      Where1ValueCtrl->SetValue(wxT(""));
      Where1ValueCtrl->Enable(true);
      if (Parent->IsWhere2Enabled() == true)
        {
          Parent->SetAndOr12(true);
          Connector12Ctrl->SetSelection(0);
          Connector12Ctrl->Enable(true);
      } else if (Parent->IsWhere3Enabled() == true)
        {
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(true);
        }
  } else
    {
      Where1TableCtrl->SetSelection(0);
      Parent->SetWhere1Table2(false);
      Where1TableCtrl->Enable(false);
      Parent->SetWhere1Column(wxT(""));
      Where1ColumnCtrl->Clear();
      Where1ColumnCtrl->Enable(false);
      Parent->SetWhere1Operator(wxT(""));
      Where1OperatorCtrl->Clear();
      Where1OperatorCtrl->Enable(false);
      Parent->SetWhere1Operator(wxT(""));
      Where1ValueCtrl->SetValue(wxT(""));
      Where1ValueCtrl->Enable(false);
      Parent->SetAndOr12(true);
      Connector12Ctrl->SetSelection(0);
      Connector12Ctrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere2Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Where #2 clause enabled/disabled
//
  Parent->SetWhere2Enabled(Where2EnabledCtrl->GetValue());
  if (Parent->IsWhere2Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsWhere2Table2() == false)
            Where2TableCtrl->SetSelection(0);
          else
            Where2TableCtrl->SetSelection(1);
          Where2TableCtrl->Enable(true);
      } else
        {
          Parent->SetWhere2Table2(false);
          Where2TableCtrl->SetSelection(0);
          Where2TableCtrl->Enable(false);
          Parent->SetWhere2Column(wxT(""));
          Where2ColumnCtrl->Clear();
        }
      Parent->SetWhere2Column(wxT(""));
      InitializeColumns(Where2ColumnCtrl, Parent->IsWhere2Table2());
      Where2ColumnCtrl->Enable(true);
      Parent->SetWhere2Operator(wxT(""));
      InitializeOperators(Where2OperatorCtrl);
      Where2OperatorCtrl->Enable(true);
      Where2ValueCtrl->SetValue(wxT(""));
      Where2ValueCtrl->Enable(true);
      if (Parent->IsWhere1Enabled() == true)
        {
          Parent->SetAndOr12(true);
          Connector12Ctrl->SetSelection(0);
          Connector12Ctrl->Enable(true);
        }
      if (Parent->IsWhere3Enabled() == true)
        {
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(true);
        }
  } else
    {
      Where2TableCtrl->SetSelection(0);
      Parent->SetWhere2Table2(false);
      Where2TableCtrl->Enable(false);
      Parent->SetWhere2Column(wxT(""));
      Where2ColumnCtrl->Clear();
      Where2ColumnCtrl->Enable(false);
      Parent->SetWhere2Operator(wxT(""));
      Where2OperatorCtrl->Clear();
      Where2OperatorCtrl->Enable(false);
      Parent->SetWhere2Operator(wxT(""));
      Where2ValueCtrl->SetValue(wxT(""));
      Where2ValueCtrl->Enable(false);
      Parent->SetAndOr12(true);
      Connector12Ctrl->SetSelection(0);
      Connector12Ctrl->Enable(false);
      if (Parent->IsWhere3Enabled() == true
          && Parent->IsWhere1Enabled() == true)
        ;
      else
        {
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(false);
        }
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere3Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Where #3 clause enabled/disabled
//
  Parent->SetWhere3Enabled(Where3EnabledCtrl->GetValue());
  if (Parent->IsWhere3Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsWhere3Table2() == false)
            Where3TableCtrl->SetSelection(0);
          else
            Where3TableCtrl->SetSelection(1);
          Where3TableCtrl->Enable(true);
      } else
        {
          Parent->SetWhere3Table2(false);
          Where3TableCtrl->SetSelection(0);
          Where3TableCtrl->Enable(false);
          Parent->SetWhere3Column(wxT(""));
          Where3ColumnCtrl->Clear();
        }
      Parent->SetWhere3Column(wxT(""));
      InitializeColumns(Where3ColumnCtrl, Parent->IsWhere3Table2());
      Where3ColumnCtrl->Enable(true);
      Parent->SetWhere3Operator(wxT(""));
      InitializeOperators(Where3OperatorCtrl);
      Where3OperatorCtrl->Enable(true);
      Where3ValueCtrl->SetValue(wxT(""));
      Where3ValueCtrl->Enable(true);
      if (Parent->IsWhere2Enabled() == true
          || Parent->IsWhere1Enabled() == true)
        {
          Parent->SetAndOr23(true);
          Connector23Ctrl->SetSelection(0);
          Connector23Ctrl->Enable(true);
        }
  } else
    {
      Where3TableCtrl->SetSelection(0);
      Parent->SetWhere3Table2(false);
      Where3TableCtrl->Enable(false);
      Parent->SetWhere3Column(wxT(""));
      Where3ColumnCtrl->Clear();
      Where3ColumnCtrl->Enable(false);
      Parent->SetWhere3Operator(wxT(""));
      Where3OperatorCtrl->Clear();
      Where3OperatorCtrl->Enable(false);
      Where3ValueCtrl->SetValue(wxT(""));
      Where3ValueCtrl->Enable(false);
      Parent->SetAndOr23(true);
      Connector23Ctrl->SetSelection(0);
      Connector23Ctrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere1TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #1 table changed
//
  if (Where1TableCtrl->GetSelection() == 1)
    Parent->SetWhere1Table2(true);
  else
    Parent->SetWhere1Table2(false);
  InitializeColumns(Where1ColumnCtrl, Parent->IsWhere1Table2());
}

void ComposerFilterPage::OnWhere2TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #2 table changed
//
  if (Where2TableCtrl->GetSelection() == 1)
    Parent->SetWhere2Table2(true);
  else
    Parent->SetWhere2Table2(false);
  InitializeColumns(Where2ColumnCtrl, Parent->IsWhere2Table2());
}

void ComposerFilterPage::OnWhere3TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #3 table changed
//
  if (Where3TableCtrl->GetSelection() == 1)
    Parent->SetWhere3Table2(true);
  else
    Parent->SetWhere3Table2(false);
  InitializeColumns(Where3ColumnCtrl, Parent->IsWhere3Table2());
}

void ComposerFilterPage::OnConnector12Changed(wxCommandEvent & WXUNUSED(event))
{
//
// Connector 1-2 changed
//
  if (Connector12Ctrl->GetSelection() == 0)
    Parent->SetAndOr12(true);
  else
    Parent->SetAndOr12(false);
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnConnector23Changed(wxCommandEvent & WXUNUSED(event))
{
//
// Connector 2-3 changed
//
  if (Connector23Ctrl->GetSelection() == 0)
    Parent->SetAndOr23(true);
  else
    Parent->SetAndOr23(false);
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere1ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #1 column changed
//
  Parent->SetWhere1Column(Where1ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere2ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #2 column changed
//
  Parent->SetWhere2Column(Where2ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere3ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #3 column changed
//
  Parent->SetWhere3Column(Where3ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere1OperatorSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #1 operator changed
//
  wxString str = Where1OperatorCtrl->GetValue();
  wxString clean = wxT("");
  if (str == wxT("= {equal to}"))
    clean = wxT("=");
  if (str == wxT("<> {not equal}"))
    clean = wxT("<>");
  if (str == wxT("< {lesser than}"))
    clean = wxT("<");
  if (str == wxT("<= {lesser / equal}"))
    clean = wxT("<=");
  if (str == wxT("> {greather than}"))
    clean = wxT(">");
  if (str == wxT(">= {greather / equal}"))
    clean = wxT(">=");
  if (str == wxT("LIKE {text search}"))
    clean = wxT("LIKE");
  if (str == wxT("IN {val1, val2, ...}"))
    clean = wxT("IN");
  if (str == wxT("IS NULL"))
    clean = wxT("IS NULL");
  if (str == wxT("IS NOT NULL"))
    clean = wxT("IS NOT NULL");
  Parent->SetWhere1Operator(clean);
  if (Parent->GetWhere1Operator() == wxT("IS NULL")
      || Parent->GetWhere1Operator() == wxT("IS NOT NULL"))
    {
      Where1ValueCtrl->SetValue(wxT(""));
      Parent->SetWhere1Value(wxT(""));
      Where1ValueCtrl->Enable(false);
  } else
    {
      Where1ValueCtrl->SetValue(Parent->GetWhere1Value());
      Where1ValueCtrl->Enable(true);
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere2OperatorSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #2 operator changed
//
  wxString str = Where2OperatorCtrl->GetValue();
  wxString clean = wxT("");
  if (str == wxT("= {equal to}"))
    clean = wxT("=");
  if (str == wxT("<> {not equal}"))
    clean = wxT("<>");
  if (str == wxT("< {lesser than}"))
    clean = wxT("<");
  if (str == wxT("<= {lesser / equal}"))
    clean = wxT("<=");
  if (str == wxT("> {greather than}"))
    clean = wxT(">");
  if (str == wxT(">= {greather / equal}"))
    clean = wxT(">=");
  if (str == wxT("LIKE {text search}"))
    clean = wxT("LIKE");
  if (str == wxT("IN {val1, val2, ...}"))
    clean = wxT("IN");
  if (str == wxT("IS NULL"))
    clean = wxT("IS NULL");
  if (str == wxT("IS NOT NULL"))
    clean = wxT("IS NOT NULL");
  Parent->SetWhere2Operator(clean);
  if (Parent->GetWhere2Operator() == wxT("IS NULL")
      || Parent->GetWhere2Operator() == wxT("IS NOT NULL"))
    {
      Where2ValueCtrl->SetValue(wxT(""));
      Parent->SetWhere2Value(wxT(""));
      Where2ValueCtrl->Enable(false);
  } else
    {
      Where2ValueCtrl->SetValue(Parent->GetWhere2Value());
      Where2ValueCtrl->Enable(true);
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::
OnWhere3OperatorSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Where #3 operator changed
//
  wxString str = Where3OperatorCtrl->GetValue();
  wxString clean = wxT("");
  if (str == wxT("= {equal to}"))
    clean = wxT("=");
  if (str == wxT("<> {not equal}"))
    clean = wxT("<>");
  if (str == wxT("< {lesser than}"))
    clean = wxT("<");
  if (str == wxT("<= {lesser / equal}"))
    clean = wxT("<=");
  if (str == wxT("> {greather than}"))
    clean = wxT(">");
  if (str == wxT(">= {greather / equal}"))
    clean = wxT(">=");
  if (str == wxT("LIKE {text search}"))
    clean = wxT("LIKE");
  if (str == wxT("IN {val1, val2, ...}"))
    clean = wxT("IN");
  if (str == wxT("IS NULL"))
    clean = wxT("IS NULL");
  if (str == wxT("IS NOT NULL"))
    clean = wxT("IS NOT NULL");
  Parent->SetWhere3Operator(clean);
  if (Parent->GetWhere3Operator() == wxT("IS NULL")
      || Parent->GetWhere3Operator() == wxT("IS NOT NULL"))
    {
      Where3ValueCtrl->SetValue(wxT(""));
      Parent->SetWhere3Value(wxT(""));
      Where3ValueCtrl->Enable(false);
  } else
    {
      Where3ValueCtrl->SetValue(Parent->GetWhere3Value());
      Where3ValueCtrl->Enable(true);
    }
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere1ValueChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #1 value changed
//
  Parent->SetWhere1Value(Where1ValueCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere2ValueChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #2 value changed
//
  Parent->SetWhere2Value(Where2ValueCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerFilterPage::OnWhere3ValueChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Where #3 value changed
//
  Parent->SetWhere3Value(Where3ValueCtrl->GetValue());
  Parent->UpdateSqlSample();
}

bool ComposerOrderPage::Create(ComposerDialog * parent)
{
//
// creating the dialog
//
  Parent = parent;
  if (wxPanel::Create(Parent->GetTabCtrl()) == false)
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

void ComposerOrderPage::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxSize size;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the ORDER BY clauses row
  wxBoxSizer *OrderSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(OrderSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the Order clause #1 pane
  wxBoxSizer *Order1BoxSizer = new wxBoxSizer(wxVERTICAL);
  OrderSizer->Add(Order1BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *Order1Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Order by #1"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *Order1Sizer = new wxStaticBoxSizer(Order1Box, wxVERTICAL);
  Order1BoxSizer->Add(Order1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order1EnabledCtrl =
    new wxCheckBox(this, ID_QVC_ORDER_1_ENABLE, wxT("Enable"));
  Order1EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Order1EnabledCtrl->Enable(false);
  Order1Sizer->Add(Order1EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString Order1Tables[2];
  Order1Tables[0] = wxT("&Main table columns");
  Order1Tables[1] = wxT("&Table #2 columns");
  Order1TableCtrl = new wxRadioBox(this, ID_QVC_ORDER_1_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   Order1Tables, 1, wxRA_SPECIFY_COLS);
  Order1TableCtrl->SetSelection(0);
  Order1TableCtrl->Enable(false);
  Order1Sizer->Add(Order1TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order1ColumnCtrl =
    new wxComboBox(this, ID_QVC_ORDER_1_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Order1ColumnCtrl->Enable(false);
  Order1Sizer->Add(Order1ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxString desc1[2];
  desc1[0] = wxT("&Descending [Z-A]");
  desc1[1] = wxT("&Ascending [A-Z]");
  Order1DescCtrl = new wxRadioBox(this, ID_QVC_ORDER_1_DESC,
                                  wxT("Direction"),
                                  wxDefaultPosition, wxDefaultSize, 2,
                                  desc1, 1, wxRA_SPECIFY_COLS);
  Order1DescCtrl->SetSelection(1);
  Order1DescCtrl->Enable(false);
  Order1Sizer->Add(Order1DescCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// the Order clause #2 pane
  wxBoxSizer *Order2BoxSizer = new wxBoxSizer(wxVERTICAL);
  OrderSizer->Add(Order2BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *Order2Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Order by #2"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *Order2Sizer = new wxStaticBoxSizer(Order2Box, wxVERTICAL);
  Order2BoxSizer->Add(Order2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order2EnabledCtrl =
    new wxCheckBox(this, ID_QVC_ORDER_2_ENABLE, wxT("Enable"));
  Order2EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Order2EnabledCtrl->Enable(false);
  Order2Sizer->Add(Order2EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString Order2Tables[2];
  Order2Tables[0] = wxT("&Main table columns");
  Order2Tables[1] = wxT("&Table #2 columns");
  Order2TableCtrl = new wxRadioBox(this, ID_QVC_ORDER_2_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   Order2Tables, 1, wxRA_SPECIFY_COLS);
  Order2TableCtrl->SetSelection(0);
  Order2TableCtrl->Enable(false);
  Order2Sizer->Add(Order2TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order2ColumnCtrl =
    new wxComboBox(this, ID_QVC_ORDER_2_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Order2ColumnCtrl->Enable(false);
  Order2Sizer->Add(Order2ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxString desc2[2];
  desc2[0] = wxT("&Descending [Z-A]");
  desc2[1] = wxT("&Ascending [A-Z]");
  Order2DescCtrl = new wxRadioBox(this, ID_QVC_ORDER_2_DESC,
                                  wxT("Direction"),
                                  wxDefaultPosition, wxDefaultSize, 2,
                                  desc2, 1, wxRA_SPECIFY_COLS);
  Order2DescCtrl->SetSelection(1);
  Order2DescCtrl->Enable(false);
  Order2Sizer->Add(Order2DescCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// the Order clause #3 pane
  wxBoxSizer *Order3BoxSizer = new wxBoxSizer(wxVERTICAL);
  OrderSizer->Add(Order3BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *Order3Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Order by #3"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *Order3Sizer = new wxStaticBoxSizer(Order3Box, wxVERTICAL);
  Order3BoxSizer->Add(Order3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order3EnabledCtrl =
    new wxCheckBox(this, ID_QVC_ORDER_3_ENABLE, wxT("Enable"));
  Order3EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Order3EnabledCtrl->Enable(false);
  Order3Sizer->Add(Order3EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString Order3Tables[2];
  Order3Tables[0] = wxT("&Main table columns");
  Order3Tables[1] = wxT("&Table #2 columns");
  Order3TableCtrl = new wxRadioBox(this, ID_QVC_ORDER_3_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   Order2Tables, 1, wxRA_SPECIFY_COLS);
  Order3TableCtrl->SetSelection(0);
  Order3TableCtrl->Enable(false);
  Order3Sizer->Add(Order3TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order3ColumnCtrl =
    new wxComboBox(this, ID_QVC_ORDER_3_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Order3ColumnCtrl->Enable(false);
  Order3Sizer->Add(Order3ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxString desc3[2];
  desc3[0] = wxT("&Descending [Z-A]");
  desc3[1] = wxT("&Ascending [A-Z]");
  Order3DescCtrl = new wxRadioBox(this, ID_QVC_ORDER_3_DESC,
                                  wxT("Direction"),
                                  wxDefaultPosition, wxDefaultSize, 2,
                                  desc3, 1, wxRA_SPECIFY_COLS);
  Order3DescCtrl->SetSelection(1);
  Order3DescCtrl->Enable(false);
  Order3Sizer->Add(Order3DescCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// the Order clause #4 pane
  wxBoxSizer *Order4BoxSizer = new wxBoxSizer(wxVERTICAL);
  OrderSizer->Add(Order4BoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *Order4Box = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Order by #4"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *Order4Sizer = new wxStaticBoxSizer(Order4Box, wxVERTICAL);
  Order4BoxSizer->Add(Order4Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order4EnabledCtrl =
    new wxCheckBox(this, ID_QVC_ORDER_4_ENABLE, wxT("Enable"));
  Order4EnabledCtrl->SetValue(false);
  if (Parent->GetTableName1().Len() == 0 && Parent->GetTableName2().Len() == 0)
    Order4EnabledCtrl->Enable(false);
  Order4Sizer->Add(Order4EnabledCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxString Order4Tables[2];
  Order4Tables[0] = wxT("&Main table columns");
  Order4Tables[1] = wxT("&Table #2 columns");
  Order4TableCtrl = new wxRadioBox(this, ID_QVC_ORDER_4_TABLE,
                                   wxT(""),
                                   wxDefaultPosition, wxDefaultSize, 2,
                                   Order2Tables, 1, wxRA_SPECIFY_COLS);
  Order4TableCtrl->SetSelection(0);
  Order4TableCtrl->Enable(false);
  Order4Sizer->Add(Order4TableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  Order4ColumnCtrl =
    new wxComboBox(this, ID_QVC_ORDER_4_COLUMN, wxT(""), wxDefaultPosition,
                   wxSize(130, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  Order4ColumnCtrl->Enable(false);
  Order4Sizer->Add(Order4ColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxString desc4[2];
  desc4[0] = wxT("&Descending [Z-A]");
  desc4[1] = wxT("&Ascending [A-Z]");
  Order4DescCtrl = new wxRadioBox(this, ID_QVC_ORDER_4_DESC,
                                  wxT("Direction"),
                                  wxDefaultPosition, wxDefaultSize, 2,
                                  desc4, 1, wxRA_SPECIFY_COLS);
  Order4DescCtrl->SetSelection(1);
  Order4DescCtrl->Enable(false);
  Order4Sizer->Add(Order4DescCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);

// adding the event handlers
  Connect(ID_QVC_ORDER_1_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder1Enabled);
  Connect(ID_QVC_ORDER_2_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder2Enabled);
  Connect(ID_QVC_ORDER_3_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder3Enabled);
  Connect(ID_QVC_ORDER_4_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder4Enabled);
  Connect(ID_QVC_ORDER_1_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder1TableChanged);
  Connect(ID_QVC_ORDER_2_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder2TableChanged);
  Connect(ID_QVC_ORDER_3_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder3TableChanged);
  Connect(ID_QVC_ORDER_4_TABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder4TableChanged);
  Connect(ID_QVC_ORDER_1_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder1ColumnSelected);
  Connect(ID_QVC_ORDER_2_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder2ColumnSelected);
  Connect(ID_QVC_ORDER_3_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder3ColumnSelected);
  Connect(ID_QVC_ORDER_4_COLUMN, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder4ColumnSelected);
  Connect(ID_QVC_ORDER_1_DESC, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder1DescChanged);
  Connect(ID_QVC_ORDER_2_DESC, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder2DescChanged);
  Connect(ID_QVC_ORDER_3_DESC, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder3DescChanged);
  Connect(ID_QVC_ORDER_4_DESC, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerOrderPage::OnOrder4DescChanged);
}

void ComposerOrderPage::Table1Status(bool ok)
{
//
// enabling/disabling Table#1 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName2().Len() == 0)
        {
          Order1EnabledCtrl->SetValue(false);
          Parent->SetOrder1Enabled(false);
          Order1EnabledCtrl->Enable(false);
          Order2EnabledCtrl->SetValue(false);
          Parent->SetOrder2Enabled(false);
          Order2EnabledCtrl->Enable(false);
          Order3EnabledCtrl->SetValue(false);
          Parent->SetOrder3Enabled(false);
          Order3EnabledCtrl->Enable(false);
          Order4EnabledCtrl->SetValue(false);
          Parent->SetOrder4Enabled(false);
          Order4EnabledCtrl->Enable(false);
          Order1TableCtrl->SetSelection(0);
          Parent->SetOrder1Table2(false);
          Order1TableCtrl->Enable(false);
          Parent->SetOrder1Column(wxT(""));
          Order1ColumnCtrl->Clear();
          Order1ColumnCtrl->Enable(false);
          Order1DescCtrl->SetSelection(1);
          Parent->SetOrder1Desc(false);
          Order1DescCtrl->Enable(false);
          Order2TableCtrl->SetSelection(0);
          Parent->SetOrder2Table2(false);
          Order2TableCtrl->Enable(false);
          Parent->SetOrder2Column(wxT(""));
          Order2ColumnCtrl->Clear();
          Order2ColumnCtrl->Enable(false);
          Order2DescCtrl->SetSelection(1);
          Parent->SetOrder2Desc(false);
          Order2DescCtrl->Enable(false);
          Order3TableCtrl->SetSelection(0);
          Parent->SetOrder3Table2(false);
          Order3TableCtrl->Enable(false);
          Parent->SetOrder3Column(wxT(""));
          Order3ColumnCtrl->Clear();
          Order3ColumnCtrl->Enable(false);
          Order3DescCtrl->SetSelection(1);
          Parent->SetOrder3Desc(false);
          Order3DescCtrl->Enable(false);
          Order4TableCtrl->SetSelection(0);
          Parent->SetOrder4Table2(false);
          Order4TableCtrl->Enable(false);
          Parent->SetOrder4Column(wxT(""));
          Order4ColumnCtrl->Clear();
          Order4ColumnCtrl->Enable(false);
          Order4DescCtrl->SetSelection(1);
          Parent->SetOrder4Desc(false);
          Order4DescCtrl->Enable(false);
      } else
        {
          if (Parent->IsOrder1Table2() == false)
            {
              Order1EnabledCtrl->SetValue(false);
              Parent->SetOrder1Enabled(false);
              Order1TableCtrl->SetSelection(0);
              Parent->SetOrder1Table2(false);
              Order1TableCtrl->Enable(false);
              Parent->SetOrder1Column(wxT(""));
              Order1ColumnCtrl->Clear();
              Order1ColumnCtrl->Enable(false);
              Order1DescCtrl->SetSelection(1);
              Parent->SetOrder1Desc(false);
              Order1DescCtrl->Enable(false);
            }
          if (Parent->IsOrder2Table2() == false)
            {
              Order2EnabledCtrl->SetValue(false);
              Parent->SetOrder2Enabled(false);
              Order2TableCtrl->SetSelection(0);
              Parent->SetOrder2Table2(false);
              Order2TableCtrl->Enable(false);
              Parent->SetOrder2Column(wxT(""));
              Order2ColumnCtrl->Clear();
              Order2ColumnCtrl->Enable(false);
              Order2DescCtrl->SetSelection(1);
              Parent->SetOrder2Desc(false);
              Order2DescCtrl->Enable(false);
            }
          if (Parent->IsOrder3Table2() == false)
            {
              Order3EnabledCtrl->SetValue(false);
              Parent->SetOrder3Enabled(false);
              Order3TableCtrl->SetSelection(0);
              Parent->SetOrder3Table2(false);
              Order3TableCtrl->Enable(false);
              Parent->SetOrder3Column(wxT(""));
              Order3ColumnCtrl->Clear();
              Order3ColumnCtrl->Enable(false);
              Order3DescCtrl->SetSelection(1);
              Parent->SetOrder3Desc(false);
              Order3DescCtrl->Enable(false);
            }
          if (Parent->IsOrder4Table2() == false)
            {
              Order4EnabledCtrl->SetValue(false);
              Parent->SetOrder4Enabled(false);
              Order4TableCtrl->SetSelection(0);
              Parent->SetOrder4Table2(false);
              Order4TableCtrl->Enable(false);
              Parent->SetOrder4Column(wxT(""));
              Order4ColumnCtrl->Clear();
              Order4ColumnCtrl->Enable(false);
              Order4DescCtrl->SetSelection(1);
              Parent->SetOrder4Desc(false);
              Order4DescCtrl->Enable(false);
            }
        }
  } else
    {
      Order1EnabledCtrl->Enable(true);
      Order2EnabledCtrl->Enable(true);
      Order3EnabledCtrl->Enable(true);
      Order4EnabledCtrl->Enable(true);
    }
}

void ComposerOrderPage::Table2Status(bool ok)
{
//
// enabling/disabling Table#2 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName1().Len() == 0)
        {
          Order1EnabledCtrl->SetValue(false);
          Parent->SetOrder1Enabled(false);
          Order1EnabledCtrl->Enable(false);
          Order2EnabledCtrl->SetValue(false);
          Parent->SetOrder2Enabled(false);
          Order2EnabledCtrl->Enable(false);
          Order3EnabledCtrl->SetValue(false);
          Parent->SetOrder3Enabled(false);
          Order3EnabledCtrl->Enable(false);
          Order4EnabledCtrl->SetValue(false);
          Parent->SetOrder4Enabled(false);
          Order4EnabledCtrl->Enable(false);
          Order1TableCtrl->SetSelection(0);
          Parent->SetOrder1Table2(false);
          Order1TableCtrl->Enable(false);
          Parent->SetOrder1Column(wxT(""));
          Order1ColumnCtrl->Clear();
          Order1ColumnCtrl->Enable(false);
          Order1DescCtrl->SetSelection(1);
          Parent->SetOrder1Desc(false);
          Order1DescCtrl->Enable(false);
          Parent->SetOrder2Table2(false);
          Order2TableCtrl->Enable(false);
          Parent->SetOrder2Column(wxT(""));
          Order2ColumnCtrl->Clear();
          Order2ColumnCtrl->Enable(false);
          Order2DescCtrl->SetSelection(1);
          Parent->SetOrder2Desc(false);
          Order2DescCtrl->Enable(false);
          Order3TableCtrl->SetSelection(0);
          Parent->SetOrder3Table2(false);
          Order3TableCtrl->Enable(false);
          Parent->SetOrder3Column(wxT(""));
          Order3ColumnCtrl->Clear();
          Order3ColumnCtrl->Enable(false);
          Order3DescCtrl->SetSelection(1);
          Parent->SetOrder3Desc(false);
          Order3DescCtrl->Enable(false);
          Order4TableCtrl->SetSelection(0);
          Parent->SetOrder4Table2(false);
          Order4TableCtrl->Enable(false);
          Parent->SetOrder4Column(wxT(""));
          Order4ColumnCtrl->Clear();
          Order4ColumnCtrl->Enable(false);
          Order4DescCtrl->SetSelection(1);
          Parent->SetOrder4Desc(false);
          Order4DescCtrl->Enable(false);
      } else
        {
          if (Parent->IsOrder1Table2() == true)
            {
              Order1EnabledCtrl->SetValue(false);
              Parent->SetOrder1Enabled(false);
              Order1TableCtrl->SetSelection(0);
              Parent->SetOrder1Table2(false);
              Order1TableCtrl->Enable(false);
              Parent->SetOrder1Column(wxT(""));
              Order1ColumnCtrl->Clear();
              Order1ColumnCtrl->Enable(false);
              Order1DescCtrl->SetSelection(1);
              Parent->SetOrder1Desc(false);
              Order1DescCtrl->Enable(false);
            }
          if (Parent->IsOrder2Table2() == true)
            {
              Order2EnabledCtrl->SetValue(false);
              Parent->SetOrder2Enabled(false);
              Order2TableCtrl->SetSelection(0);
              Parent->SetOrder2Table2(false);
              Order2TableCtrl->Enable(false);
              Parent->SetOrder2Column(wxT(""));
              Order2ColumnCtrl->Clear();
              Order2ColumnCtrl->Enable(false);
              Order2DescCtrl->SetSelection(1);
              Parent->SetOrder2Desc(false);
              Order2DescCtrl->Enable(false);
            }
          if (Parent->IsOrder3Table2() == true)
            {
              Order3EnabledCtrl->SetValue(false);
              Parent->SetOrder3Enabled(false);
              Order3TableCtrl->SetSelection(0);
              Parent->SetOrder3Table2(false);
              Order3TableCtrl->Enable(false);
              Parent->SetOrder3Column(wxT(""));
              Order3ColumnCtrl->Clear();
              Order3ColumnCtrl->Enable(false);
              Order3DescCtrl->SetSelection(1);
              Parent->SetOrder3Desc(false);
              Order3DescCtrl->Enable(false);
            }
          if (Parent->IsOrder4Table2() == true)
            {
              Order4EnabledCtrl->SetValue(false);
              Parent->SetOrder4Enabled(false);
              Order4TableCtrl->SetSelection(0);
              Parent->SetOrder4Table2(false);
              Order4TableCtrl->Enable(false);
              Parent->SetOrder4Column(wxT(""));
              Order4ColumnCtrl->Clear();
              Order4ColumnCtrl->Enable(false);
              Order4DescCtrl->SetSelection(1);
              Parent->SetOrder4Desc(false);
              Order4DescCtrl->Enable(false);
            }
        }
  } else
    {
      Order1EnabledCtrl->Enable(true);
      Order2EnabledCtrl->Enable(true);
      Order3EnabledCtrl->Enable(true);
      Order4EnabledCtrl->Enable(true);
    }
}

void ComposerOrderPage::OnOrder1Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Order #1 clause enabled/disabled
//
  Parent->SetOrder1Enabled(Order1EnabledCtrl->GetValue());
  if (Parent->IsOrder1Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsOrder1Table2() == false)
            Order1TableCtrl->SetSelection(0);
          else
            Order1TableCtrl->SetSelection(1);
          Order1TableCtrl->Enable(true);
      } else
        {
          Parent->SetOrder1Table2(false);
          Order1TableCtrl->SetSelection(0);
          Order1TableCtrl->Enable(false);
          Parent->SetOrder1Column(wxT(""));
          Order1ColumnCtrl->Clear();
        }
      Parent->SetOrder1Column(wxT(""));
      InitializeColumns(Order1ColumnCtrl, Parent->IsOrder1Table2());
      Order1ColumnCtrl->Enable(true);
      if (Parent->IsOrder1Desc() == true)
        Order1DescCtrl->SetSelection(0);
      else
        Order1DescCtrl->SetSelection(1);
      Order1DescCtrl->Enable(true);
  } else
    {
      Order1TableCtrl->SetSelection(0);
      Parent->SetOrder1Table2(false);
      Order1TableCtrl->Enable(false);
      Parent->SetOrder1Column(wxT(""));
      Order1ColumnCtrl->Clear();
      Order1ColumnCtrl->Enable(false);
      Parent->SetOrder1Desc(false);
      Order1DescCtrl->SetSelection(1);
      Order1DescCtrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder2Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Order #2 clause enabled/disabled
//
  Parent->SetOrder2Enabled(Order2EnabledCtrl->GetValue());
  if (Parent->IsOrder2Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsOrder2Table2() == false)
            Order2TableCtrl->SetSelection(0);
          else
            Order2TableCtrl->SetSelection(1);
          Order2TableCtrl->Enable(true);
      } else
        {
          Parent->SetOrder2Table2(false);
          Order2TableCtrl->SetSelection(0);
          Order2TableCtrl->Enable(false);
          Parent->SetOrder2Column(wxT(""));
          Order2ColumnCtrl->Clear();
        }
      Parent->SetOrder2Column(wxT(""));
      InitializeColumns(Order2ColumnCtrl, Parent->IsOrder2Table2());
      Order2ColumnCtrl->Enable(true);
      if (Parent->IsOrder2Desc() == true)
        Order2DescCtrl->SetSelection(0);
      else
        Order2DescCtrl->SetSelection(1);
      Order2DescCtrl->Enable(true);
  } else
    {
      Order2TableCtrl->SetSelection(0);
      Parent->SetOrder2Table2(false);
      Order2TableCtrl->Enable(false);
      Parent->SetOrder2Column(wxT(""));
      Order2ColumnCtrl->Clear();
      Order2ColumnCtrl->Enable(false);
      Parent->SetOrder2Desc(false);
      Order2DescCtrl->SetSelection(1);
      Order2DescCtrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder3Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Order #3 clause enabled/disabled
//
  Parent->SetOrder3Enabled(Order3EnabledCtrl->GetValue());
  if (Parent->IsOrder3Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsOrder3Table2() == false)
            Order3TableCtrl->SetSelection(0);
          else
            Order3TableCtrl->SetSelection(1);
          Order3TableCtrl->Enable(true);
      } else
        {
          Parent->SetOrder3Table2(false);
          Order3TableCtrl->SetSelection(0);
          Order3TableCtrl->Enable(false);
          Parent->SetOrder3Column(wxT(""));
          Order3ColumnCtrl->Clear();
        }
      Parent->SetOrder3Column(wxT(""));
      InitializeColumns(Order3ColumnCtrl, Parent->IsOrder3Table2());
      Order3ColumnCtrl->Enable(true);
      if (Parent->IsOrder3Desc() == true)
        Order3DescCtrl->SetSelection(0);
      else
        Order3DescCtrl->SetSelection(1);
      Order3DescCtrl->Enable(true);
  } else
    {
      Order3TableCtrl->SetSelection(0);
      Parent->SetOrder3Table2(false);
      Order3TableCtrl->Enable(false);
      Parent->SetOrder3Column(wxT(""));
      Order3ColumnCtrl->Clear();
      Order3ColumnCtrl->Enable(false);
      Parent->SetOrder3Desc(false);
      Order3DescCtrl->SetSelection(1);
      Order3DescCtrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder4Enabled(wxCommandEvent & WXUNUSED(event))
{
//
// Order #4 clause enabled/disabled
//
  Parent->SetOrder4Enabled(Order4EnabledCtrl->GetValue());
  if (Parent->IsOrder4Enabled() == true)
    {
      if (Parent->IsTable2Enabled() == true)
        {
          if (Parent->IsOrder4Table2() == false)
            Order4TableCtrl->SetSelection(0);
          else
            Order4TableCtrl->SetSelection(1);
          Order4TableCtrl->Enable(true);
      } else
        {
          Parent->SetOrder4Table2(false);
          Order4TableCtrl->SetSelection(0);
          Order4TableCtrl->Enable(false);
          Parent->SetOrder4Column(wxT(""));
          Order4ColumnCtrl->Clear();
        }
      Parent->SetOrder4Column(wxT(""));
      InitializeColumns(Order4ColumnCtrl, Parent->IsOrder4Table2());
      Order4ColumnCtrl->Enable(true);
      if (Parent->IsOrder4Desc() == true)
        Order4DescCtrl->SetSelection(0);
      else
        Order4DescCtrl->SetSelection(1);
      Order4DescCtrl->Enable(true);
  } else
    {
      Order4TableCtrl->SetSelection(0);
      Parent->SetOrder4Table2(false);
      Order4TableCtrl->Enable(false);
      Parent->SetOrder4Column(wxT(""));
      Order4ColumnCtrl->Clear();
      Order4ColumnCtrl->Enable(false);
      Parent->SetOrder4Desc(false);
      Order4DescCtrl->SetSelection(1);
      Order4DescCtrl->Enable(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::InitializeColumns(wxComboBox * ctrl, bool table2)
{
// initializing a columns list ComboBox
  ctrl->Clear();
  AuxColumn *pColumn;
  if (table2 == false)
    pColumn = Parent->GetColumn1List()->GetFirst();
  else
    pColumn = Parent->GetColumn2List()->GetFirst();
  while (pColumn)
    {
      wxString col = pColumn->GetName();
      ctrl->Append(col);
      pColumn = pColumn->GetNext();
    }
}

void ComposerOrderPage::OnOrder1TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #1 table changed
//
  if (Order1TableCtrl->GetSelection() == 1)
    Parent->SetOrder1Table2(true);
  else
    Parent->SetOrder1Table2(false);
  InitializeColumns(Order1ColumnCtrl, Parent->IsOrder1Table2());
}

void ComposerOrderPage::OnOrder2TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #2 table changed
//
  if (Order2TableCtrl->GetSelection() == 1)
    Parent->SetOrder2Table2(true);
  else
    Parent->SetOrder2Table2(false);
  InitializeColumns(Order2ColumnCtrl, Parent->IsOrder2Table2());
}

void ComposerOrderPage::OnOrder3TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #3 table changed
//
  if (Order3TableCtrl->GetSelection() == 1)
    Parent->SetOrder3Table2(true);
  else
    Parent->SetOrder3Table2(false);
  InitializeColumns(Order3ColumnCtrl, Parent->IsOrder3Table2());
}

void ComposerOrderPage::OnOrder4TableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #4 table changed
//
  if (Order4TableCtrl->GetSelection() == 1)
    Parent->SetOrder4Table2(true);
  else
    Parent->SetOrder4Table2(false);
  InitializeColumns(Order4ColumnCtrl, Parent->IsOrder4Table2());
}

void ComposerOrderPage::OnOrder1ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Order #1 column changed
//
  Parent->SetOrder1Column(Order1ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder2ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Order #2 column changed
//
  Parent->SetOrder2Column(Order2ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder3ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Order #3 column changed
//
  Parent->SetOrder3Column(Order3ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder4ColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Order #4 column changed
//
  Parent->SetOrder4Column(Order4ColumnCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder1DescChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #1 Asc/Desc changed
//
  if (Order1DescCtrl->GetSelection() == 0)
    Parent->SetOrder1Desc(true);
  else
    Parent->SetOrder1Desc(false);
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder2DescChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #2 Asc/Desc changed
//
  if (Order2DescCtrl->GetSelection() == 0)
    Parent->SetOrder2Desc(true);
  else
    Parent->SetOrder2Desc(false);
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder3DescChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #3 Asc/Desc changed
//
  if (Order3DescCtrl->GetSelection() == 0)
    Parent->SetOrder3Desc(true);
  else
    Parent->SetOrder3Desc(false);
  Parent->UpdateSqlSample();
}

void ComposerOrderPage::OnOrder4DescChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Order #4 Asc/Desc changed
//
  if (Order4DescCtrl->GetSelection() == 0)
    Parent->SetOrder4Desc(true);
  else
    Parent->SetOrder4Desc(false);
  Parent->UpdateSqlSample();
}

bool ComposerViewPage::Create(ComposerDialog * parent)
{
//
// creating the dialog
//
  Parent = parent;
  if (wxPanel::Create(Parent->GetTabCtrl()) == false)
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

void ComposerViewPage::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the VIEw row
  wxBoxSizer *viewMainSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(viewMainSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// the VIEW pane
  wxBoxSizer *viewBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  viewMainSizer->Add(viewBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticBox *viewBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Create View options"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *viewSizer = new wxStaticBoxSizer(viewBox, wxVERTICAL);
  viewBoxSizer->Add(viewSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxString viewModes[3];
  viewModes[0] = wxT("&No View [execute SELECT query]");
  viewModes[1] = wxT("&Create View [ordinary SQL view]");
  viewModes[2] = wxT("&Create Spatial View [could be used as a GIS Layer]");
  ViewTypeCtrl = new wxRadioBox(this, ID_QVC_VIEW_TYPE,
                                wxT("View type"),
                                wxDefaultPosition, wxDefaultSize, 3,
                                viewModes, 1, wxRA_SPECIFY_COLS);
  ViewTypeCtrl->SetSelection(0);
  viewSizer->Add(ViewTypeCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxSize size = ViewTypeCtrl->GetSize();
  wxStaticBox *viewNmBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("View name"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *viewNmSizer = new wxStaticBoxSizer(viewNmBox, wxVERTICAL);
  viewSizer->Add(viewNmSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  ViewNameCtrl =
    new wxTextCtrl(this, ID_QVC_VIEW_NAME, Parent->GetViewName(),
                   wxDefaultPosition, wxSize(size.GetWidth() - 10, 21));
  ViewNameCtrl->Enable(false);
  viewNmSizer->Add(ViewNameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);

// the GEOMETRY pane
  wxStaticBox *geomBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Geometry Column"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *geomSizer = new wxStaticBoxSizer(geomBox, wxVERTICAL);
  viewBoxSizer->Add(geomSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxString geomTables[2];
  geomTables[0] = wxT("&Main table geometries");
  geomTables[1] = wxT("&Table #2 geometries");
  GeomTableCtrl = new wxRadioBox(this, ID_QVC_VIEW_GEOTABLE,
                                 wxT(""),
                                 wxDefaultPosition, wxDefaultSize, 2,
                                 geomTables, 1, wxRA_SPECIFY_COLS);
  GeomTableCtrl->SetSelection(0);
  GeomTableCtrl->Enable(false);
  geomSizer->Add(GeomTableCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  size = GeomTableCtrl->GetSize();
  GeometryColumnCtrl =
    new wxComboBox(this, ID_QVC_VIEW_GEOMETRY, wxT(""), wxDefaultPosition,
                   wxSize(size.GetWidth() - 5, 21), 0, NULL,
                   wxCB_DROPDOWN | wxCB_READONLY);
  GeometryColumnCtrl->Enable(false);
  geomSizer->Add(GeometryColumnCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

// Writable View
  wxStaticBox *writeBox = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Read/Write View"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *writeSizer = new wxStaticBoxSizer(writeBox, wxHORIZONTAL);
  boxSizer->Add(writeSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  Writable1Ctrl =
    new wxCheckBox(this, ID_QVC_WRITABLE_1, wxT("RW Enabled Main Table"));
  Writable1Ctrl->SetValue(false);
  Writable1Ctrl->Enable(false);
  writeSizer->Add(Writable1Ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
  Writable2Ctrl =
    new wxCheckBox(this, ID_QVC_WRITABLE_2, wxT("RW Enabled Table #2"));
  Writable2Ctrl->SetValue(false);
  Writable2Ctrl->Enable(false);
  writeSizer->Add(Writable2Ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

// adding the event handlers
  Connect(ID_QVC_VIEW_TYPE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerViewPage::OnViewTypeChanged);
  Connect(ID_QVC_VIEW_NAME, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & ComposerViewPage::OnViewNameChanged);
  Connect(ID_QVC_VIEW_GEOTABLE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerViewPage::OnGeomTableChanged);
  Connect(ID_QVC_VIEW_GEOMETRY, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & ComposerViewPage::OnGeometryColumnSelected);
  Connect(ID_QVC_WRITABLE_1, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerViewPage::OnWritable1Changed);
  Connect(ID_QVC_WRITABLE_2, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & ComposerViewPage::OnWritable2Changed);
}

void ComposerViewPage::Table1Status(bool ok)
{
//
// enabling/disabling Table#1 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName2().Len() == 0)
        {
          GeomTableCtrl->SetSelection(0);
          Parent->SetViewGeomTable2(false);
          GeometryColumnCtrl->Clear();
          Parent->SetGeometryColumn(wxT(""));
      } else
        {
          if (Parent->IsViewGeomTable2() == false)
            {
              GeomTableCtrl->SetSelection(0);
              Parent->SetViewGeomTable2(false);
              GeometryColumnCtrl->Clear();
              Parent->SetGeometryColumn(wxT(""));
            }
        }
  } else
    {
      if (Parent->IsTable2Enabled() == false)
        {
          GeomTableCtrl->SetSelection(0);
          Parent->SetViewGeomTable2(false);
          GeomTableCtrl->Enable(false);
      } else
        {
          if (Parent->IsSpatialView() == true)
            {
              if (Parent->IsViewGeomTable2() == true)
                GeomTableCtrl->SetSelection(1);
              else
                GeomTableCtrl->SetSelection(0);
              GeomTableCtrl->Enable(true);
              GeometryColumnCtrl->Enable(true);
              InitializeGeometries(Parent->IsViewGeomTable2());
          } else
            {
              GeomTableCtrl->SetSelection(0);
              Parent->SetViewGeomTable2(false);
              GeomTableCtrl->Enable(false);
            }
        }
    }
}

void ComposerViewPage::Table2Status(bool ok)
{
//
// enabling/disabling Table#2 dependent fields 
//
  if (ok == false)
    {
      if (Parent->GetTableName1().Len() == 0)
        {
          GeomTableCtrl->SetSelection(0);
          Parent->SetViewGeomTable2(false);
          GeometryColumnCtrl->Clear();
          Parent->SetGeometryColumn(wxT(""));
      } else
        {
          if (Parent->IsOrder1Table2() == true)
            {
              GeomTableCtrl->SetSelection(0);
              Parent->SetViewGeomTable2(false);
              GeometryColumnCtrl->Clear();
              Parent->SetGeometryColumn(wxT(""));
            }
        }
  } else
    {
      if (Parent->IsSpatialView() == true)
        {
          if (Parent->IsViewGeomTable2() == true)
            GeomTableCtrl->SetSelection(1);
          else
            GeomTableCtrl->SetSelection(0);
          GeomTableCtrl->Enable(true);
          GeometryColumnCtrl->Enable(true);
          InitializeGeometries(Parent->IsViewGeomTable2());
      } else
        {
          GeomTableCtrl->SetSelection(0);
          Parent->SetViewGeomTable2(false);
          GeomTableCtrl->Enable(false);
        }
    }
}

void ComposerViewPage::InitializeGeometries(bool table2)
{
// initializing the Geometry columns list ComboBox
  GeometryColumnCtrl->Clear();
  AuxTable *pTbl = NULL;
  AuxTable *pTable;
  wxString tbl = Parent->GetTableName1();
  if (table2 == true)
    tbl = Parent->GetTableName2();
  pTable = Parent->GetTableList()->GetFirst();
  while (pTable)
    {
      if (pTable->GetTableName() == tbl)
        {
          pTbl = pTable;
          break;
        }
      pTable = pTable->GetNext();
    }
  if (pTbl)
    {
      // initializing Geometry columns
      int i;
      for (i = 0; i < pTable->GetGeometriesCount(); i++)
        GeometryColumnCtrl->Append(pTable->GetGeometryColumn(i));
    }
}

void ComposerViewPage::OnViewTypeChanged(wxCommandEvent & WXUNUSED(event))
{
//
// View type changed
//
  Parent->SetPlainView(false);
  Parent->SetSpatialView(false);
  if (ViewTypeCtrl->GetSelection() == 1)
    Parent->SetPlainView(true);
  if (ViewTypeCtrl->GetSelection() == 2)
    Parent->SetSpatialView(true);
  if (Parent->IsPlainView() == true || Parent->IsSpatialView() == true)
    {
      ViewNameCtrl->Enable(true);
  } else
    {
      ViewNameCtrl->Enable(false);
      ViewNameCtrl->SetValue(wxT(""));
      Parent->SetViewName(wxT(""));
    }
  if (Parent->IsSpatialView() == true)
    {
      InitializeGeometries(Parent->IsViewGeomTable2());
      if (Parent->IsTable2Enabled() == true)
        GeomTableCtrl->Enable(true);
      GeometryColumnCtrl->Enable(true);
      Writable1Ctrl->Enable(true);
      if (Parent->IsTable2Enabled() == true)
        {
          Writable2Ctrl->Enable(true);
          Writable2Ctrl->SetValue(false);
          Parent->SetWritable2(false);
        }
  } else
    {
      GeomTableCtrl->Enable(false);
      GeomTableCtrl->SetSelection(0);
      Parent->SetViewGeomTable2(false);
      GeometryColumnCtrl->Enable(false);
      GeometryColumnCtrl->Clear();
      Parent->SetGeometryColumn(wxT(""));
      Writable1Ctrl->Enable(false);
      Writable2Ctrl->Enable(false);
      Writable1Ctrl->SetValue(false);
      Writable2Ctrl->SetValue(false);
      Parent->SetWritable1(false);
      Parent->SetWritable2(false);
    }
  Parent->UpdateSqlSample();
}

void ComposerViewPage::OnViewNameChanged(wxCommandEvent & WXUNUSED(event))
{
//
// View name changed
//
  Parent->SetViewName(ViewNameCtrl->GetValue());
  Parent->UpdateSqlSample();
}

void ComposerViewPage::OnGeomTableChanged(wxCommandEvent & WXUNUSED(event))
{
//
// View Geom table changed
//
  if (GeomTableCtrl->GetSelection() == 1)
    Parent->SetViewGeomTable2(true);
  else
    Parent->SetViewGeomTable2(false);
  InitializeGeometries(Parent->IsViewGeomTable2());
}

void ComposerViewPage::
OnGeometryColumnSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Geometry column selection changed
//
  Parent->SetGeometryColumn(GeometryColumnCtrl->GetValue());
  Parent->SelectGeometryColumn();
  Parent->UpdateSqlSample();
}

void ComposerViewPage::OnWritable1Changed(wxCommandEvent & WXUNUSED(event))
{
//
// Main Table Read/Write enabled/disabled
//
  Parent->SetWritable1(Writable1Ctrl->GetValue());
  if (Parent->IsWritable1() == false)
    {
      Parent->SetWritable2(false);
      Writable2Ctrl->SetValue(false);
    }
}

void ComposerViewPage::OnWritable2Changed(wxCommandEvent & WXUNUSED(event))
{
//
// Table#2 Read/Write enabled/disabled
//
  Parent->SetWritable2(Writable2Ctrl->GetValue());
  if (Parent->IsWritable2() == true)
    {
      Parent->SetWritable1(true);
      Writable1Ctrl->SetValue(true);
    }
}

AuxTable::AuxTable(wxString & table)
{
// constructor - table item
  TableName = table;
  MaxGeometryIndex = 0;
  Next = NULL;
}

void AuxTable::AddGeometryColumn(wxString & geom)
{
// adding a Geometry column
  if (MaxGeometryIndex < 128)
    Geometries[MaxGeometryIndex++] = geom;
}

wxString & AuxTable::GetGeometryColumn(int ind)
{
// returns a Geometry column
  return Geometries[ind];
}

AuxTableList::AuxTableList()
{
// constructor - table list
  First = NULL;
  Last = NULL;
  Count = 0;
}

AuxTableList::~AuxTableList()
{
// destructor - table list
  Flush();
}

void AuxTableList::Flush()
{
// cleanup
  AuxTable *pT;
  AuxTable *pTn;
  pT = First;
  while (pT)
    {
      pTn = pT->GetNext();
      delete pT;
      pT = pTn;
    }
  First = NULL;
  Last = NULL;
  Count = 0;
}

void AuxTableList::Populate(sqlite3 * handle)
{
// populating a table list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  char *geom;
  wxString tblName;
  wxString geomCol;
  wxString sql;
  AuxTable *pTable;
// retrieving the Table Names
  sql =
    wxT
    ("SELECT name FROM sqlite_master WHERE (type = 'table' OR type = 'view') ORDER BY name");
  int ret = sqlite3_get_table(handle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    return;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          tblName = wxString::FromUTF8(name);
          pTable = new AuxTable(tblName);
          if (!First)
            First = pTable;
          if (Last)
            Last->SetNext(pTable);
          Last = pTable;
          Count++;
        }
    }
  sqlite3_free_table(results);

// retrieving the Geometry Columns
  sql = wxT("SELECT f_table_name, f_geometry_column FROM geometry_columns");
  ret = sqlite3_get_table(handle, sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    return;
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 0];
          geom = results[(i * columns) + 1];
          tblName = wxString::FromUTF8(name);
          geomCol = wxString::FromUTF8(geom);
          pTable = First;
          while (pTable)
            {
              char tbl[1024];
              strcpy(tbl, pTable->GetTableName().ToUTF8());
              if (strcasecmp(tbl, name) == 0)
                {
                  pTable->AddGeometryColumn(geomCol);
                  break;
                }
              pTable = pTable->GetNext();
            }
        }
    }
  sqlite3_free_table(results);
}

AuxColumn::AuxColumn(wxString & name)
{
// constructor - column item
  Name = name;
  Selected = false;
  Next = NULL;
}

AuxColumnList::AuxColumnList()
{
// constructor - column list
  First = NULL;
  Last = NULL;
  Count = 0;
}

AuxColumnList::~AuxColumnList()
{
// destructor - column list
  Flush();
}

void AuxColumnList::Flush()
{
// cleanup
  AuxColumn *pC;
  AuxColumn *pCn;
  pC = First;
  while (pC)
    {
      pCn = pC->GetNext();
      delete pC;
      pC = pCn;
    }
  First = NULL;
  Last = NULL;
  Count = 0;
}

void AuxColumnList::SetState(int ind, bool mode)
{
// setting the selection state for some column identified by its position
  AuxColumn *pC;
  int cnt = 0;
  pC = First;
  while (pC)
    {
      if (cnt == ind)
        {
          pC->SetState(mode);
          return;
        }
      cnt++;
      pC = pC->GetNext();
    }
}

void AuxColumnList::SetState(wxString & column)
{
// forcing the selection state for some column identified by its name
  AuxColumn *pC;
  pC = First;
  while (pC)
    {
      if (pC->GetName() == column)
        {
          pC->SetState(true);
          return;
        }
      pC = pC->GetNext();
    }
}

bool AuxColumnList::HasSelectedColumns()
{
// setting the selection state for some column identified by its position
  AuxColumn *pC;
  int cnt = 0;
  pC = First;
  while (pC)
    {
      cnt++;
      pC = pC->GetNext();
    }
  if (cnt)
    return true;
  return false;
}

void AuxColumnList::Populate(sqlite3 * handle, wxString & table)
{
// populating a column list
  int i;
  char **results;
  int rows;
  int columns;
  char *errMsg = NULL;
  char *name;
  wxString colName;
  wxString sql;
  AuxColumn *pCol;
  char xname[1024];
  Flush();
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, table.ToUTF8());
  MyFrame::DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(handle, sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    return;
  if (rows < 1)
    ;
  else
    {
      // inserting the ROWID
      colName = wxT("ROWID");
      pCol = new AuxColumn(colName);
      if (!First)
        First = pCol;
      if (Last)
        Last->SetNext(pCol);
      Last = pCol;
      Count++;
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (strcasecmp(name, "ROWID") == 0)
            continue;
          colName = wxString::FromUTF8(name);
          pCol = new AuxColumn(colName);
          if (!First)
            First = pCol;
          if (Last)
            Last->SetNext(pCol);
          Last = pCol;
          Count++;
        }
    }
  sqlite3_free_table(results);
}
