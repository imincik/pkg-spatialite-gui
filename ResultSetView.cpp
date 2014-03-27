/*
/ ResultSetView.cpp
/ a panel to show SQL query results
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

#include "gaiagraphics.h"

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <io.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

//
// ICONs in XPM format [universally portable]
//
#include "icons/rs_first.xpm"
#include "icons/rs_last.xpm"
#include "icons/rs_next.xpm"
#include "icons/rs_previous.xpm"
#include "icons/refresh.xpm"

#if defined(_WIN32) || defined (__MINGW32__)
#define FORMAT_64	"%I64d"
#else
#define FORMAT_64	"%lld"
#endif

MyResultSetView::MyResultSetView(MyFrame * parent, wxWindowID id):
wxPanel(parent, id, wxDefaultPosition, wxSize(440, 480), wxBORDER_SUNKEN)
{
//
// constructor: a frame for SQL Result Sets
//
  RsBlock = 500;                // the ResultSet block size
  RowIds = new sqlite3_int64[RsBlock];
  ReadOnly = true;
  InsertRow = NULL;
  MainFrame = parent;
  BtnRsFirst =
    new wxBitmapButton(this, ID_RS_FIRST, wxBitmap(rs_first_xpm),
                       wxPoint(5, 400), wxSize(32, 32));
  BtnRsFirst->SetToolTip(wxT("ResultSet: go to first row"));
  BtnRsPrevious =
    new wxBitmapButton(this, ID_RS_PREVIOUS, wxBitmap(rs_previous_xpm),
                       wxPoint(55, 400), wxSize(32, 32));
  BtnRsPrevious->SetToolTip(wxT("ResultSet: go to previous block"));
  BtnRefresh =
    new wxBitmapButton(this, ID_REFRESH, wxBitmap(refresh_xpm),
                       wxPoint(55, 400), wxSize(32, 32));
  BtnRefresh->SetToolTip(wxT("ResultSet: refresh"));
  BtnRsNext =
    new wxBitmapButton(this, ID_RS_NEXT, wxBitmap(rs_next_xpm),
                       wxPoint(105, 400), wxSize(32, 32));
  BtnRsNext->SetToolTip(wxT("ResultSet: go to next block"));
  BtnRsLast =
    new wxBitmapButton(this, ID_RS_LAST, wxBitmap(rs_last_xpm),
                       wxPoint(155, 400), wxSize(32, 32));
  BtnRsLast->SetToolTip(wxT("ResultSet: go to last row"));
  RsCurrentBlock =
    new wxStaticText(this, ID_RS_BLOCK, wxT(""), wxPoint(210, 400),
                     wxSize(200, 18));
  TableView = NULL;
  TableBlobs = NULL;
  TableValues = NULL;
  CurrentBlob = NULL;
// setting up event handlers
  Connect(ID_RS_FIRST, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnRsFirst);
  Connect(ID_RS_PREVIOUS, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnRsPrevious);
  Connect(ID_RS_NEXT, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnRsNext);
  Connect(ID_RS_LAST, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnRsLast);
  Connect(ID_REFRESH, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnRefresh);
  Connect(ID_RS_THREAD_FINISHED, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnThreadFinished);
  Connect(ID_RS_STATS_UPDATE, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MyResultSetView::OnStatsUpdate);
  Connect(wxID_ANY, wxEVT_SIZE,
          (wxObjectEventFunction) & MyResultSetView::OnSize);
  Connect(wxID_ANY, wxEVT_GRID_SELECT_CELL,
          (wxObjectEventFunction) & MyResultSetView::OnCellSelected);
  Connect(wxID_ANY, wxEVT_GRID_CELL_RIGHT_CLICK,
          (wxObjectEventFunction) & MyResultSetView::OnRightClick);
  Connect(wxID_ANY, wxEVT_GRID_CELL_CHANGE,
          (wxObjectEventFunction) & MyResultSetView::OnCellChanged);
  Connect(Grid_Delete, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdDelete);
  Connect(Grid_Insert, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdInsert);
  Connect(Grid_Abort, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdAbort);
  Connect(Grid_Clear, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdClearSelection);
  Connect(Grid_All, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdSelectAll);
  Connect(Grid_Row, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdSelectRow);
  Connect(Grid_Column, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdSelectColumn);
  Connect(Grid_Copy, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdCopy);
  Connect(Grid_Blob, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdBlob);
  Connect(Grid_BlobIn, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdBlobIn);
  Connect(Grid_BlobOut, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdBlobOut);
  Connect(Grid_BlobNull, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdBlobNull);
  Connect(Grid_XmlBlobIn, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdXmlBlobIn);
  Connect(Grid_XmlBlobOut, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdXmlBlobOut);
  Connect(Grid_XmlBlobOutIndented, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdXmlBlobOutIndented);
  Connect(Grid_ExpTxtTab, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpTxtTab);
  Connect(Grid_ExpCsv, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpCsv);
  Connect(Grid_ExpHtml, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpHtml);
  Connect(Grid_ExpShp, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpShp);
  Connect(Grid_ExpDif, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpDif);
  Connect(Grid_ExpSylk, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpSylk);
  Connect(Grid_ExpDbf, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyResultSetView::OnCmdExpDbf);
}

MyResultSetView::~MyResultSetView()
{
// destructor
  if (RowIds)
    delete[]RowIds;
  if (InsertRow)
    delete InsertRow;
  if (TableBlobs)
    delete TableBlobs;
  if (TableValues)
    delete TableValues;
}

void MyResultSetView::ShowControls()
{
//
// making all ResultSet controls to be visible
//
  BtnRsFirst->Show(true);
  BtnRsPrevious->Show(true);
  BtnRsNext->Show(true);
  BtnRsLast->Show(true);
  BtnRefresh->Show(true);
}

void MyResultSetView::HideControls()
{
//
// making all controls to be invisible
//
  BtnRsFirst->Show(false);
  BtnRsPrevious->Show(false);
  BtnRsNext->Show(false);
  BtnRsLast->Show(false);
  BtnRefresh->Show(false);
}

void MyResultSetView::EditTable(wxString & sql, int *primaryKeys, int *blobCols,
                                wxString & table)
{
//
// starting the edit table sequence
//
  int i;
  for (i = 0; i < 1024; i++)
    {
      PrimaryKeys[i] = *(primaryKeys + i);
      BlobColumns[i] = *(blobCols + i);
    }
  ReadOnly = false;
  TableName = table;
  MainFrame->GetQueryView()->GetSqlCtrl()->SetValue(sql);
  if (ExecuteSqlPre(sql, 0, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::FormatElapsedTime(double seconds, char *elapsed,
                                        bool simple)
{
/* well formatting elapsed time */
  int int_time = (int) seconds;
  int millis = (int) ((seconds - (double) int_time) * 1000.0);
  int secs = int_time % 60;
  int_time /= 60;
  int mins = int_time % 60;
  int_time /= 60;
  int hh = int_time;
  if (simple == true)
    {
      if (hh == 0 && mins == 0)
        sprintf(elapsed, "%d.%03d", secs, millis);
      else if (hh == 0)
        sprintf(elapsed, "%d:%02d.%03d", mins, secs, millis);
      else
        sprintf(elapsed, "%d:%02d:%02d.%03d", hh, mins, secs, millis);
  } else
    sprintf(elapsed, "%02d:%02d:%02d.%03d", hh, mins, secs, millis);
}

void SqlThreadParams::Reset(void)
{
//
// thread params: resetting to empty state
//
  Mother = NULL;
  Sql = wxT("");
  Stmt = NULL;
  FromRow = 0;
  EndRow = 0;
  MaxRow = 0;
  List.Reset();
  Sqlite = NULL;
  Start = 0;
  LastGuiUpdate = 0;
  Error = false;
  FetchedRows = 0;
  StatFullscanStep = 0;
  StatSort = 0;
  StatAutoindex = 0;
  ElapsedTime = 0.0;
  AbortRequested = false;
  Valid = false;
}

void SqlThreadParams::Initialize(MyResultSetView * mother, wxString & sql,
                                 sqlite3_stmt * stmt, int from,
                                 sqlite3 * sqlite, clock_t start)
{
//
// thread params: initializing
//
  Reset();
  Mother = mother;
  Sql = sql;
  Stmt = stmt;
  FromRow = from;
  EndRow = 0;
  MaxRow = 0;
  Sqlite = sqlite;
  Start = start;
  LastGuiUpdate = start;
  Error = false;
  FetchedRows = 0;
  StatFullscanStep = 0;
  StatSort = 0;
  StatAutoindex = 0;
  ElapsedTime = 0.0;
  AbortRequested = false;
  Valid = true;
}

void SqlThreadParams::UpdateStats(int fullscan, int sort, int autoindex,
                                  clock_t now)
{
//
// updading Stats (SQL query progress handler callback)
//
  StatFullscanStep = fullscan;
  StatSort = sort;
  StatAutoindex = autoindex;
  ElapsedTime = (double) (now - Start) / (double) CLOCKS_PER_SEC;
}

bool SqlThreadParams::GuiHasToBeUpdated(clock_t now, int millisecs)
{
//
// checking if a GUI update is required (Stats)
//
  double interval = (double) (now - LastGuiUpdate) / (double) CLOCKS_PER_SEC;
  if (interval >= ((double) millisecs / 1000.0))
    return true;
  return false;
}

int SqlProgressCallback(void *arg)
{
//
// SQL query progress handler callback function
//
  int fullscan;
  int sort;
  int autoindex;
  clock_t clock_end;
  wxString currentBlock;
  SqlThreadParams *params = (SqlThreadParams *) arg;

  if (params->IsValid() == false)
    return 0;

  if (params->IsAbortRequested() == true)
    {
      // aborting the SQL query
      sqlite3_interrupt(params->GetSqlite());
      return 0;
    }
// updating query stats
  fullscan =
    sqlite3_stmt_status(params->GetStmt(), SQLITE_STMTSTATUS_FULLSCAN_STEP, 0);
  sort = sqlite3_stmt_status(params->GetStmt(), SQLITE_STMTSTATUS_SORT, 0);
#ifdef OMIT_SQLITE_STMTSTATUS_AUTOINXED
  autoindex = -1;
#else
  autoindex =
    sqlite3_stmt_status(params->GetStmt(), SQLITE_STMTSTATUS_AUTOINDEX, 0);
#endif
  clock_end = clock();
  params->UpdateStats(fullscan, sort, autoindex, clock_end);
  if (params->GuiHasToBeUpdated(clock_end, 500) == true)
    {
      // requesting a GUI update
      wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_RS_STATS_UPDATE);
      params->GetMother()->GetEventHandler()->AddPendingEvent(event);
      params->SetLastGuiUpdate(clock_end);
    }
  return 0;
}

#ifdef _WIN32
DWORD WINAPI DoExecuteSqlThread(void *arg)
#else
void *DoExecuteSqlThread(void *arg)
#endif
{
//
// threaded function: processing an SQL query
//
  int ret;
  int i_col;
  int i_row = 0;
  int end_row = 0;
  int columns;
  char err_msg[2048];

// setting up the sqlite's progress handler callback
  SqlThreadParams *params = (SqlThreadParams *) arg;
  sqlite3_progress_handler(params->GetSqlite(), 1000, SqlProgressCallback, arg);

  while (1)
    {
      //
      // fetching the result set rows 
      //
      if (params->IsValid() == false)
        goto error;
      ret = sqlite3_step(params->GetStmt());
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          //
          // fetching a row
          //
          if (params->IsAbortRequested() == true)
            {
              // aborting the SQL query
              sqlite3_interrupt(params->GetSqlite());
              continue;
            }

          params->FetchedRow();
          if ((params->GetFetchedRows() % 1000) == 0)
            SqlProgressCallback(params);
          if (i_row < params->GetFromRow())
            {
              i_row++;
              continue;
            }
          if ((i_row - params->GetFromRow()) >=
              params->GetMother()->GetRsBlock())
            {
              i_row++;
              continue;
            }
          end_row = i_row;
          columns = sqlite3_column_count(params->GetStmt());
          MyRowVariant *rowVariant = params->GetList()->Add(columns);
          for (i_col = 0; i_col < columns; i_col++)
            {
              sqlite3_int64 int_value;
              double dbl_value;
              const unsigned char *txt_value;
              const void *blob_value;
              int blobSize;
              params->GetList()->SetColumnName(i_col,
                                               sqlite3_column_name
                                               (params->GetStmt(), i_col));
              switch (sqlite3_column_type(params->GetStmt(), i_col))
                {
                  case SQLITE_INTEGER:
                    int_value = sqlite3_column_int64(params->GetStmt(), i_col);
                    rowVariant->Set(i_col, int_value);
                    break;
                  case SQLITE_FLOAT:
                    dbl_value = sqlite3_column_double(params->GetStmt(), i_col);
                    rowVariant->Set(i_col, dbl_value);
                    break;
                  case SQLITE_TEXT:
                    txt_value = sqlite3_column_text(params->GetStmt(), i_col);
                    rowVariant->Set(i_col, txt_value);
                    break;
                  case SQLITE_BLOB:
                    blob_value = sqlite3_column_blob(params->GetStmt(), i_col);
                    blobSize = sqlite3_column_bytes(params->GetStmt(), i_col);
                    rowVariant->Set(i_col, blob_value, blobSize);
                    break;
                  case SQLITE_NULL:
                  default:
                    break;
                };
            }
          i_row++;
      } else
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(params->GetSqlite()));
          sqlite3_finalize(params->GetStmt());
          wxString msg = wxString::FromUTF8(err_msg);
          params->GetMother()->SetSqlErrorMsg(msg);
          goto error;
        }
    }
  sqlite3_finalize(params->GetStmt());
  params->SetEndRow(end_row);
  params->SetMaxRow(i_row);
  goto ok;
error:
  params->SetError();
ok:
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_RS_THREAD_FINISHED);
  params->GetMother()->GetEventHandler()->AddPendingEvent(event);
#ifdef _WIN32
  return 0;
#else
  pthread_exit(NULL);
#endif
}

void MyResultSetView::AbortRequested()
{
//
// attempting to abort the current SQL query
//
  if (ThreadParams.IsValid() == true)
    ThreadParams.Abort();
}

bool MyResultSetView::ExecuteSqlPre(wxString & sql, int from, bool read_only)
{
//
// executing some SQL statement
//
  ReadOnly = read_only;
  char *xSql = NULL;
  char err_msg[2048];
  int i_row;
  clock_t clock_start;
  sqlite3_stmt *stmt;
  sqlite3 *sqlite = MainFrame->GetSqlite();
  SqlErrorMsg = wxT("");
  if (TableView)
    TableView->Destroy();
  TableView = NULL;
  if (TableBlobs)
    delete TableBlobs;
  if (TableValues)
    delete TableValues;
  TableBlobs = NULL;
  TableValues = NULL;
  CurrentBlob = NULL;
  HideControls();
  RsBeginRow = 0;
  RsEndRow = 0;
  RsMaxRow = 0;
#ifdef _WIN32
  HANDLE thread_handle;
  DWORD dwThreadId;
#else
  pthread_t thread_id;
#endif
  for (i_row = 0; i_row < RsBlock; i_row++)
    RowIds[i_row] = -1;
  i_row = 0;
  xSql = new char[65536];
  strcpy(xSql, sql.ToUTF8());

  clock_start = clock();
  int ret = sqlite3_prepare_v2(sqlite, xSql, strlen(xSql), &stmt, NULL);
  delete[]xSql;
  if (ret != SQLITE_OK)
    {
      sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(sqlite));
      SqlErrorMsg = wxString::FromUTF8(err_msg);
      ::wxEndBusyCursor();
      return false;
    }
  CreateStatsGrid();
  ::wxBeginBusyCursor();

  ThreadParams.Initialize(this, sql, stmt, from, sqlite, clock_start);
#ifdef _WIN32
  thread_handle =
    CreateThread(NULL, 0, DoExecuteSqlThread, &ThreadParams, 0, &dwThreadId);
  SetThreadPriority(thread_handle, THREAD_PRIORITY_IDLE);
#else
  int ok_prior = 0;
  int policy;
  int min_prio;
  pthread_attr_t attr;
  struct sched_param sp;
  pthread_attr_init(&attr);
  if (pthread_attr_setschedpolicy(&attr, SCHED_RR) == 0)
    {
      // attempting to set the lowest priority  
      if (pthread_attr_getschedpolicy(&attr, &policy) == 0)
        {
          min_prio = sched_get_priority_min(policy);
          sp.sched_priority = min_prio;
          if (pthread_attr_setschedparam(&attr, &sp) == 0)
            {
              // ok, setting the lowest priority  
              ok_prior = 1;
              pthread_create(&thread_id, &attr, DoExecuteSqlThread,
                             &ThreadParams);
            }
        }
    }
  if (!ok_prior)
    {
      // failure: using standard priority
      pthread_create(&thread_id, NULL, DoExecuteSqlThread, &ThreadParams);
    }
#endif
  MainFrame->GetQueryView()->EnableAbortButton();
  return true;
}

bool MyResultSetView::ExecuteSqlPost()
{
  char dummy[1024];
  int i_col;
  int i_row;
  double seconds;
  clock_t clock_end;
  char elapsed[64];
  MyRowVariant *row;
  MyVariant *value;
  wxString blobType;
  int type;
  wxString cellValue;
  wxString currentBlock;
  MyVariantList *list;

  MainFrame->GetQueryView()->DisableAbortButton();
  if (TableView)
    TableView->Destroy();
  TableView = NULL;
  if (TableBlobs)
    delete TableBlobs;
  if (TableValues)
    delete TableValues;
  TableBlobs = NULL;
  TableValues = NULL;
  CurrentBlob = NULL;

  if (ThreadParams.IsValid() == false)
    {
      if (SqlErrorMsg.Len() > 0)
        {
          wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                       MainFrame);
          ::wxEndBusyCursor();
          ThreadParams.Reset();
          return true;
        }
      goto error;
    }
  if (ThreadParams.IsError() == true)
    {
      if (SqlErrorMsg.Len() > 0)
        {
          wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                       MainFrame);
          ::wxEndBusyCursor();
          ThreadParams.Reset();
          return true;
        }
      goto error;
    }

  list = ThreadParams.GetList();
  clock_end = clock();
  seconds =
    (double) (clock_end - ThreadParams.GetStart()) / (double) CLOCKS_PER_SEC;
  RsBeginRow = ThreadParams.GetFromRow();
  RsEndRow = ThreadParams.GetEndRow();
  RsMaxRow = ThreadParams.GetMaxRow();
  if (list->GetRows() == 0)
    {
      //
      // this one is an EMPTY Result Set
      //
      if (ReadOnly == false)
        {
          // preparing the insert row
          int numCols = 0;
          wxString *colNames = MainFrame->GetColumnNames(TableName, &numCols);
          CreateGrid(0, numCols + 1);
          TableView->SetColLabelValue(0, wxT("ROWID"));
          for (i_col = 0; i_col < numCols; i_col++)
            TableView->SetColLabelValue(i_col + 1, *(colNames + i_col));
          TableView->EnableEditing(true);
          delete[]colNames;
      } else
        {
          // simply showing a warning message
          CreateGrid(1, 1);
          TableView->SetColLabelValue(0, wxT("Message"));
          TableView->SetRowLabelValue(0, wxT("Message"));
          TableView->SetCellValue(0, 0,
                                  wxT
                                  ("SQL query returned an empty ResultSet\n\nThis is not an error"));
        }
  } else
    {
      //
      // preparing the Grid to show the result set
      //
      CreateGrid(list->GetRows(), list->GetColumns());
      if (ReadOnly == true)
        TableView->EnableEditing(false);
      else
        TableView->EnableEditing(true);
      for (i_col = 0; i_col < list->GetColumns(); i_col++)
        TableView->SetColLabelValue(i_col, list->GetColumnName(i_col));
      if (ReadOnly == false)
        TableView->SetColLabelValue(0, wxT("ROWID"));
      i_row = 0;
      row = list->GetFirst();
      while (row)
        {
          sprintf(dummy, "%d", i_row + RsBeginRow + 1);
          cellValue = wxString::FromUTF8(dummy);
          TableView->SetRowLabelValue(i_row, cellValue);
          if (ReadOnly == false)
            {
              // storing the ROWID value into the RowIds array
              value = row->GetColumn(0);
              if (value->GetType() == MY_INT_VARIANT)
                RowIds[i_row] = value->GetIntValue();
            }
          for (i_col = 0; i_col < row->GetNumCols(); i_col++)
            {
              value = row->GetColumn(i_col);
              if (value)
                {
                  switch (value->GetType())
                    {
                      case MY_INT_VARIANT:
                        sprintf(dummy, FORMAT_64, value->GetIntValue());
                        cellValue = wxString::FromUTF8(dummy);
                        TableView->SetCellValue(i_row, i_col, cellValue);
                        if (ReadOnly == false)
                          TableValues->SetValue(i_row, i_col,
                                                value->GetIntValue());
                        TableView->SetCellAlignment(i_row, i_col, wxALIGN_RIGHT,
                                                    wxALIGN_TOP);
                        break;
                      case MY_DBL_VARIANT:
                        sprintf(dummy, "%1.6f", value->GetDblValue());
                        cellValue = wxString::FromUTF8(dummy);
                        TableView->SetCellValue(i_row, i_col, cellValue);
                        if (ReadOnly == false)
                          TableValues->SetValue(i_row, i_col,
                                                value->GetDblValue());
                        TableView->SetCellAlignment(i_row, i_col, wxALIGN_RIGHT,
                                                    wxALIGN_TOP);
                        break;
                      case MY_TXT_VARIANT:
                        TableView->SetCellValue(i_row, i_col,
                                                value->GetTxtValue());
                        if (ReadOnly == false)
                          TableValues->SetValue(i_row, i_col,
                                                value->GetTxtValue());
                        break;
                      case MY_BLOB_VARIANT:
                        blobType = wxT("UNKNOWN type");
                        type =
                          gaiaGuessBlobType(value->GetBlob(),
                                            value->GetBlobSize());
                        switch (type)
                          {
                            case GAIA_GEOMETRY_BLOB:
                              blobType = wxT("GEOMETRY");
                              break;
                            case GAIA_XML_BLOB:
                              blobType = wxT("XmlBLOB");
                              break;
                            case GAIA_JPEG_BLOB:
                              blobType = wxT("JPEG image");
                              break;
                            case GAIA_EXIF_BLOB:
                              blobType = wxT("JPEG-EXIF image");
                              break;
                            case GAIA_EXIF_GPS_BLOB:
                              blobType = wxT("JPEG-EXIF-GPS image");
                              break;
                            case GAIA_PNG_BLOB:
                              blobType = wxT("PNG image");
                              break;
                            case GAIA_GIF_BLOB:
                              blobType = wxT("GIF image");
                              break;
                            case GAIA_TIFF_BLOB:
                              blobType = wxT("TIFF image");
                              break;
                            case GAIA_PDF_BLOB:
                              blobType = wxT("PDF document");
                              break;
                            case GAIA_ZIP_BLOB:
                              blobType = wxT("ZIP archive");
                              break;
                            default:
                              if (gGraphIsRawImage
                                  (value->GetBlob(),
                                   value->GetBlobSize()) == GGRAPH_OK)
                                blobType = wxT("RasterLite RAW image");
                              break;
                          };
                        if (type == GAIA_XML_BLOB)
                          {
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */

                            int doc_size =
                              gaiaXmlBlobGetDocumentSize(value->GetBlob(),
                                                         value->GetBlobSize());
                            if (gaiaIsSvgXmlBlob
                                (value->GetBlob(), value->GetBlobSize()))
                              sprintf(dummy, "XmlBLOB-SVG sz=%d (XMLsz=%d) ",
                                      value->GetBlobSize(), doc_size);
                            else if (gaiaIsIsoMetadataXmlBlob
                                     (value->GetBlob(), value->GetBlobSize()))
                              sprintf(dummy,
                                      "XmlBLOB-ISOmetadata sz=%d (XMLsz=%d) ",
                                      value->GetBlobSize(), doc_size);
                            else
                              if (gaiaIsSldSeVectorStyleXmlBlob
                                  (value->GetBlob(), value->GetBlobSize()))
                              sprintf(dummy,
                                      "XmlBLOB-VectorStyle sz=%d (XMLsz=%d) ",
                                      value->GetBlobSize(), doc_size);
                            else
                              if (gaiaIsSldSeRasterStyleXmlBlob
                                  (value->GetBlob(), value->GetBlobSize()))
                              sprintf(dummy,
                                      "XmlBLOB-RasterStyle sz=%d (XMLsz=%d) ",
                                      value->GetBlobSize(), doc_size);
                            else
                              sprintf(dummy, "XmlBLOB sz=%d (XMLsz=%d) ",
                                      value->GetBlobSize(), doc_size);
                            cellValue = wxString::FromUTF8(dummy);
                            if (gaiaIsSchemaValidatedXmlBlob
                                (value->GetBlob(), value->GetBlobSize()))
                              cellValue += wxT(" SchemaValidated");

#endif /* end LIBXML2 conditionals */
                        } else
                          {
                            sprintf(dummy, "BLOB sz=%d ", value->GetBlobSize());
                            cellValue = wxString::FromUTF8(dummy);
                            cellValue += blobType;
                          }
                        TableView->SetCellValue(i_row, i_col, cellValue);
                        TableView->SetReadOnly(i_row, i_col);
                        TableBlobs->SetBlob(i_row, i_col, value);
                        break;
                      case MY_NULL_VARIANT:
                      default:
                        TableView->SetCellValue(i_row, i_col, wxT("NULL"));
                        break;
                    };
              } else
                TableView->SetCellValue(i_row, i_col, wxT("NULL"));
              if (ReadOnly == false)
                {
                  if (IsPrimaryKey(i_col) == true)
                    TableView->SetReadOnly(i_row, i_col);
                  if (IsBlobColumn(i_col) == true)
                    TableView->SetReadOnly(i_row, i_col);
                }
            }
          i_row++;
          row = row->GetNext();
        }
    }
  if (ReadOnly == false)
    {
      // prepearing the insert row
      TableView->SetRowLabelValue(TableView->GetNumberRows() - 1,
                                  wxT("Insert row"));
      InsertPending = false;
      for (i_col = 0; i_col < TableView->GetNumberCols(); i_col++)
        {
          TableView->SetCellValue(TableView->GetNumberRows() - 1, i_col,
                                  wxT(""));
          TableView->SetCellBackgroundColour(TableView->GetNumberRows() - 1,
                                             i_col, wxColour(0, 0, 0));
          TableView->SetReadOnly(TableView->GetNumberRows() - 1, i_col);
        }
    }
  TableView->SetRowLabelSize(wxGRID_AUTOSIZE);
  TableView->AutoSize();
  ResizeView();
  FormatElapsedTime(seconds, elapsed);
  sprintf(dummy, "current block: %d / %d [%d rows]    [fetched in %s]",
          RsBeginRow + 1, RsEndRow + 1, RsMaxRow, elapsed);
  currentBlock = wxString::FromUTF8(dummy);
  RsCurrentBlock->SetLabel(currentBlock);
  ShowControls();
  MainFrame->GetQueryView()->AddToHistory(ThreadParams.GetSql());
  ThreadParams.Reset();
  ::wxEndBusyCursor();
  return true;
error:
  ::wxEndBusyCursor();
  ThreadParams.Reset();
  return false;
}

bool MyResultSetView::IsPrimaryKey(int column)
{
//
// checks if this column is a Primary Key one
//
  int i;
  for (i = 0; i < 1024; i++)
    {
      if (PrimaryKeys[i] == column)
        return true;
    }
  return false;
}

bool MyResultSetView::IsBlobColumn(int column)
{
//
// checks if this column is a BLOB-type column
//
  int i;
  for (i = 0; i < 1024; i++)
    {
      if (BlobColumns[i] == column)
        return true;
    }
  return false;
}

void MyResultSetView::CreateGrid(int rows, int cols)
{
//
// creating a new Grid to show the result set
//
  int extra = 0;
  if (ReadOnly == false)
    extra = 1;
  TableView = new wxGrid(this, wxID_ANY, wxPoint(5, 5), wxSize(200, 200));
  TableView->Show(false);
  TableView->CreateGrid(rows + extra, cols);
  TableBlobs = new MyBlobs(rows, cols);
  if (ReadOnly == false)
    TableValues = new MyValues(rows, cols);
}

void MyResultSetView::CreateStatsGrid()
{
//
// creating a new Grid to show the SQL query stats (progress handler)
//
  wxSize sz = GetClientSize();
  TableView = new wxGrid(this, wxID_ANY, wxPoint(5, 5), wxSize(200, 200));
  TableView->Show(false);
  TableView->CreateGrid(5, 2);
  TableView->EnableEditing(false);
  TableView->SetColLabelValue(0, wxT("Progress Counter"));
  TableView->SetColLabelValue(1, wxT("Current Value"));
  TableView->SetCellValue(0, 0, wxT("FullscanStep"));
  TableView->SetCellValue(1, 0, wxT("Sort"));
  TableView->SetCellValue(2, 0, wxT("Autoindex"));
  TableView->SetCellValue(3, 0, wxT("FetchedRows"));
  TableView->SetCellValue(4, 0, wxT("ElapsedTime"));
  TableView->SetCellBackgroundColour(4, 0, wxColour(128, 255, 128));
  TableView->SetCellValue(0, 1, wxT("0"));
  TableView->SetCellAlignment(0, 1, wxALIGN_RIGHT, wxALIGN_CENTRE);
  TableView->SetCellValue(1, 1, wxT("0"));
  TableView->SetCellAlignment(1, 1, wxALIGN_RIGHT, wxALIGN_CENTRE);
  TableView->SetCellValue(2, 1, wxT("0"));
  TableView->SetCellAlignment(2, 1, wxALIGN_RIGHT, wxALIGN_CENTRE);
  TableView->SetCellValue(3, 1, wxT("0"));
  TableView->SetCellAlignment(3, 1, wxALIGN_RIGHT, wxALIGN_CENTRE);
  TableView->SetCellValue(4, 1, wxT("0"));
  TableView->SetCellAlignment(4, 1, wxALIGN_RIGHT, wxALIGN_CENTRE);
  TableView->SetRowLabelSize(wxGRID_AUTOSIZE);
  TableView->AutoSize();
  TableView->SetSize(sz.GetWidth() - 10, sz.GetHeight() - 45);
  TableView->Show(true);
  TableView->Disable();
  wxString msg = wxT("... wait please: SQL query in progress ...");
  RsCurrentBlock->SetLabel(msg);
}

void MyResultSetView::ResizeView()
{
//
// resizing the Grid to show the result set
//
  wxSize sz = GetClientSize();
  if (TableView)
    {
      TableView->SetSize(sz.GetWidth() - 10, sz.GetHeight() - 45);
      TableView->Show(true);
    }
}

void MyResultSetView::OnSize(wxSizeEvent & WXUNUSED(event))
{
//
// this window has changed its size
//
  wxSize sz = GetClientSize();
  if (TableView)
    TableView->SetSize(sz.GetWidth() - 10, sz.GetHeight() - 45);
  BtnRsFirst->Move(5, sz.GetHeight() - 35);
  BtnRsPrevious->Move(40, sz.GetHeight() - 35);
  BtnRefresh->Move(75, sz.GetHeight() - 35);
  BtnRsNext->Move(110, sz.GetHeight() - 35);
  BtnRsLast->Move(145, sz.GetHeight() - 35);
  RsCurrentBlock->Move(180, sz.GetHeight() - 25);
}

void MyResultSetView::OnRsFirst(wxCommandEvent & WXUNUSED(event))
{
//
// scrolling to the result set beginning
//
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  if (ExecuteSqlPre(sql, 0, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::OnRsPrevious(wxCommandEvent & WXUNUSED(event))
{
//
// scrolling to the result set previous block
//
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  int start = RsBeginRow - RsBlock;
  if (start < 0)
    start = 0;
  if (ExecuteSqlPre(sql, start, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::OnRsNext(wxCommandEvent & WXUNUSED(event))
{
//
// scrolling to the result set next block
//
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  int start = RsEndRow + 1;
  if (ExecuteSqlPre(sql, start, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::OnRsLast(wxCommandEvent & WXUNUSED(event))
{
//
// scrolling to the result set ending
//
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  int start = RsMaxRow - RsBlock;
  if (start < 0)
    start = 0;
  if (ExecuteSqlPre(sql, start, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::OnRefresh(wxCommandEvent & WXUNUSED(event))
{
//
// refreshing the result set
//
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  int start = RsBeginRow;
  if (ExecuteSqlPre(sql, start, ReadOnly) == false)
    wxMessageBox(SqlErrorMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                 MainFrame);
}

void MyResultSetView::OnThreadFinished(wxCommandEvent & WXUNUSED(event))
{
//
// the SQL thread signals termination
//
  if (ExecuteSqlPost() == false)
    wxMessageBox(wxT("An error occurred while showing the ResultSet"),
                 wxT("spatialite_gui"), wxOK | wxICON_ERROR, MainFrame);
}

void MyResultSetView::OnStatsUpdate(wxCommandEvent & WXUNUSED(event))
{
//
// updating SQL stats (progress handler)
//
  char elapsed[64];
  char dummy[1024];
  wxString cellValue;
  if (ThreadParams.IsValid() == false)
    return;
  FormatElapsedTime(ThreadParams.GetElapsedTime(), elapsed, true);
  sprintf(dummy, "%d", ThreadParams.GetStatFullscanStep());
  cellValue = wxString::FromUTF8(dummy);
  TableView->SetCellValue(0, 1, cellValue);
  sprintf(dummy, "%d", ThreadParams.GetStatSort());
  cellValue = wxString::FromUTF8(dummy);
  TableView->SetCellValue(1, 1, cellValue);
  sprintf(dummy, "%d", ThreadParams.GetStatAutoindex());
  cellValue = wxString::FromUTF8(dummy);
  TableView->SetCellValue(2, 1, cellValue);
  sprintf(dummy, "%d", ThreadParams.GetFetchedRows());
  cellValue = wxString::FromUTF8(dummy);
  TableView->SetCellValue(3, 1, cellValue);
  sprintf(dummy, "%s", elapsed);
  cellValue = wxString::FromUTF8(dummy);
  TableView->SetCellValue(4, 1, cellValue);
  TableView->ForceRefresh();
}

void MyResultSetView::OnRightClick(wxGridEvent & event)
{
//
// right click on some cell [mouse action]
//
  int blob_type;
  MyVariant *blobVar;
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxPoint pt = event.GetPosition();
  if (ReadOnly == false && event.GetRow() == TableView->GetNumberRows() - 1)
    {
      // this is the INSERT ROW
      if (InsertPending == true)
        {
          menuItem =
            new wxMenuItem(menu, Grid_Insert, wxT("&Confirm insertion"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Grid_Abort, wxT("&Abort insertion"));
          menu->Append(menuItem);
      } else
        {
          menuItem = new wxMenuItem(menu, Grid_Insert, wxT("&Insert new row"));
          menu->Append(menuItem);
        }
      TableView->PopupMenu(menu, pt);
      return;
    }
  CurrentEvtRow = event.GetRow();
  CurrentEvtColumn = event.GetCol();
  blobVar = TableBlobs->GetBlob(CurrentEvtRow, CurrentEvtColumn);
  if (blobVar)
    {
      // this one is a BLOB cell
      if (ReadOnly == false)
        {
          MyRowVariant *varRow = TableValues->GetRow(CurrentEvtRow);
          if (varRow->IsDeleted() == false)
            {
              menuItem = new wxMenuItem(menu, Grid_Delete, wxT("&Delete row"));
              menu->Append(menuItem);
              menuItem =
                new wxMenuItem(menu, Grid_Insert, wxT("&Insert new row"));
              menu->Append(menuItem);
              menu->AppendSeparator();
            }
        }
      menuItem = new wxMenuItem(menu, Grid_Blob, wxT("BLOB &explore"));
      menu->Append(menuItem);
      blob_type = gaiaGuessBlobType(blobVar->GetBlob(), blobVar->GetBlobSize());
      if (blob_type == GAIA_GEOMETRY_BLOB)
        ;
      else if (blob_type == GAIA_XML_BLOB)
        {
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Grid_XmlBlobIn, wxT("XmlBLOB &import"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Grid_XmlBlobOut,
                           wxT("XmlBLOB &export (not indented)"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Grid_XmlBlobOutIndented,
                           wxT("XmlBLOB export (i&ndented)"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Grid_BlobNull, wxT("Set BLOB as &NULL"));
          menu->Append(menuItem);
      } else
        {
          menu->AppendSeparator();
          menuItem = new wxMenuItem(menu, Grid_BlobIn, wxT("BLOB &import"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Grid_BlobOut, wxT("BLOB &export"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Grid_BlobNull, wxT("Set BLOB as &NULL"));
          menu->Append(menuItem);
        }
      CurrentBlob = blobVar;
  } else
    {
      // this one is an ordinary cell
      CurrentBlob = NULL;
      if (ReadOnly == false)
        {
          MyRowVariant *varRow = TableValues->GetRow(CurrentEvtRow);
          if (varRow->IsDeleted() == false)
            {
              menuItem = new wxMenuItem(menu, Grid_Delete, wxT("&Delete row"));
              menu->Append(menuItem);
              menuItem =
                new wxMenuItem(menu, Grid_Insert, wxT("&Insert new row"));
              menu->Append(menuItem);
              if (IsBlobColumn(CurrentEvtColumn) == true)
                {
                  menu->AppendSeparator();
                  menuItem =
                    new wxMenuItem(menu, Grid_BlobIn, wxT("BLOB &import"));
                  menu->Append(menuItem);
                }
              menu->AppendSeparator();
            }
        }
      menuItem = new wxMenuItem(menu, Grid_Clear, wxT("&Clear selection"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Grid_All, wxT("Select &all"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Grid_Row, wxT("Select &row"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Grid_Column, wxT("&Select column"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Grid_Copy, wxT("&Copy"));
      menu->Append(menuItem);
      if (TableView->IsSelection() == false)
        menuItem->Enable(false);
    }
  menu->AppendSeparator();
  wxMenu *exportMenu = new wxMenu();
  menuItem = new wxMenuItem(exportMenu, Grid_ExpTxtTab, wxT("as &Txt/Tab"));
  exportMenu->Append(menuItem);
  menuItem = new wxMenuItem(exportMenu, Grid_ExpCsv, wxT("as &CSV"));
  exportMenu->Append(menuItem);
  menuItem = new wxMenuItem(exportMenu, Grid_ExpHtml, wxT("as &HTML"));
  exportMenu->Append(menuItem);
  menuItem = new wxMenuItem(exportMenu, Grid_ExpShp, wxT("as &Shapefile"));
  exportMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(exportMenu, Grid_ExpDif, wxT("as &DIF spreadsheet"));
  exportMenu->Append(menuItem);
  menuItem =
    new wxMenuItem(exportMenu, Grid_ExpSylk, wxT("as &SYLK spreadsheet"));
  exportMenu->Append(menuItem);
  menuItem = new wxMenuItem(exportMenu, Grid_ExpDbf, wxT("as &DBF archive"));
  exportMenu->Append(menuItem);
  menu->AppendSubMenu(exportMenu, wxT("&Export ResultSet"));
  TableView->PopupMenu(menu, pt);
}

void MyResultSetView::OnCellSelected(wxGridEvent & event)
{
//
// cell selection changed
//
  if (InsertPending == true)
    {
      // an INSERT row is still pending
      if (event.GetRow() != TableView->GetNumberRows() - 1)
        DoInsert(false);
    }
  event.Skip();
}

void MyResultSetView::OnCellChanged(wxGridEvent & event)
{
//
// user changed value in some cell
//
  MyVariant *oldValue;
  MyVariant *insValue;
  wxString value;
  wxString numValue;
  wxString newValue = wxT("NULL");
  wxString sql;
  wxString rowid;
  char *errMsg = NULL;
  bool error = false;
  sqlite3_int64 int64_value;
  long long_value;
  bool okIntValue = false;
  double dbl_value;
  bool okDblValue = false;
  int ret;
  char dummy[256];
  int row = event.GetRow();
  int column = event.GetCol();
  char xname[1024];
  value = TableView->GetCellValue(row, column);
  if (InsertPending == true)
    {
      // an INSERT row is still pending
      insValue = InsertRow->GetColumn(column);
      numValue = value;
      numValue.Replace(wxT(","), wxT("."));
      okIntValue = numValue.ToLong(&long_value);
      okDblValue = numValue.ToDouble(&dbl_value);
      if (okIntValue == true)
        {
          int64_value = long_value;
          insValue->Set(int64_value);
      } else if (okDblValue == true)
        insValue->Set(dbl_value);
      else
        insValue->Set(value);
      if (row != TableView->GetNumberRows() - 1)
        DoInsert(false);
      return;
    }
  if (value.Len() > 0)
    {
      numValue = value;
      numValue.Replace(wxT(","), wxT("."));
      okIntValue = numValue.ToLong(&long_value);
      okDblValue = numValue.ToDouble(&dbl_value);
      if (okIntValue == true)
        {
          int64_value = long_value;
          sprintf(dummy, FORMAT_64, int64_value);
          newValue = wxString::FromUTF8(dummy);
      } else if (okDblValue == true)
        {
          sprintf(dummy, "%1.6f", dbl_value);
          newValue = wxString::FromUTF8(dummy);
      } else
        {
          value.Replace(wxT("'"), wxT("''"));
          newValue = wxT("'") + value + wxT("'");
        }
    }
  oldValue = TableValues->GetValue(row, 0);
  sprintf(dummy, FORMAT_64, oldValue->GetIntValue());
  rowid = wxString::FromUTF8(dummy);
  sql = wxT("UPDATE ");
  strcpy(xname, TableName.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" SET ");
  strcpy(xname, TableView->GetColLabelValue(column).ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" = ") + newValue + wxT(" WHERE ROWID = ") + rowid;
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      error = true;
    }
  oldValue = TableValues->GetValue(row, column);
  if (error == true)
    {
      // update failed; restorig old cell value
      value = wxT("NULL");
      if (oldValue)
        {
          if (oldValue->GetType() == MY_INT_VARIANT)
            {
              sprintf(dummy, FORMAT_64, oldValue->GetIntValue());
              value = wxString::FromUTF8(dummy);
            }
          if (oldValue->GetType() == MY_DBL_VARIANT)
            {
              sprintf(dummy, "%1.6f", oldValue->GetDblValue());
              value = wxString::FromUTF8(dummy);
            }
          if (oldValue->GetType() == MY_TXT_VARIANT)
            value = oldValue->GetTxtValue();
        }
      TableView->SetCellValue(row, column, value);
      TableView->ForceRefresh();
  } else
    {
      // marking cell as modified
      TableView->SetCellTextColour(row, column, wxColour(0, 0, 192));
      TableView->SetCellBackgroundColour(row, column, wxColour(255, 255, 204));
      TableView->ForceRefresh();
    }
}

void MyResultSetView::DoInsert(bool confirmed)
{
//
// performing actual row INSERT
//
  int i;
  int ret;
  MyVariant *var;
  wxString value;
  wxString strValue;
  wxString sql;
  char dummy[256];
  char *errMsg = NULL;
  char xname[1024];
  if (confirmed == false)
    {
      ret =
        wxMessageBox(wxT("A new row is ready for insertion\n\nConfirm ?"),
                     wxT("spatialite_gui"), wxYES_NO | wxICON_QUESTION, this);
      if (ret != wxYES)
        goto close_insert;
    }
  sql = wxT("INSERT INTO ");
  strcpy(xname, TableName.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" (");
  for (i = 1; i < TableView->GetNumberCols(); i++)
    {
      if (i > 1)
        sql += wxT(", ");
      strcpy(xname, TableView->GetColLabelValue(i).ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
    }
  sql += wxT(") VALUES (");
  for (i = 1; i < InsertRow->GetNumCols(); i++)
    {
      if (i > 1)
        sql += wxT(", ");
      var = InsertRow->GetColumn(i);
      value = wxT("NULL");
      if (var->GetType() == MY_INT_VARIANT)
        {
          sprintf(dummy, FORMAT_64, var->GetIntValue());
          value = wxString::FromUTF8(dummy);
        }
      if (var->GetType() == MY_DBL_VARIANT)
        {
          sprintf(dummy, "%1.6f", var->GetDblValue());
          value = wxString::FromUTF8(dummy);
        }
      if (var->GetType() == MY_TXT_VARIANT)
        {
          strValue = var->GetTxtValue();
          strValue.Replace(wxT("'"), wxT("''"));
          value = wxT("'") + strValue + wxT("'");
        }
      sql += value;
    }
  sql += wxT(")");
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
close_insert:
//
// closing insert row
//
  InsertPending = false;
  delete InsertRow;
  InsertRow = NULL;
  for (i = 0; i < TableView->GetNumberCols(); i++)
    {
      TableView->SetCellValue(TableView->GetNumberRows() - 1, i, wxT(""));
      TableView->SetCellBackgroundColour(TableView->GetNumberRows() - 1, i,
                                         wxColour(0, 0, 0));
      TableView->SetReadOnly(TableView->GetNumberRows() - 1, i);
    }
}

void MyResultSetView::OnCmdDelete(wxCommandEvent & WXUNUSED(event))
{
//
// user required row deletion
//
  char *errMsg = NULL;
  bool error = false;
  int ret;
  int i;
  wxString sql;
  wxString rowid;
  MyVariant *value;
  char dummy[256];
  char xname[1024];
  MyRowVariant *varRow = TableValues->GetRow(CurrentEvtRow);
  if (varRow->IsDeleted() == true)
    return;
  value = TableValues->GetValue(CurrentEvtRow, 0);
  sprintf(dummy, FORMAT_64, value->GetIntValue());
  rowid = wxString::FromUTF8(dummy);
  ret =
    wxMessageBox(wxT("Requested deletion for row identified by RowId = ")
                 + rowid + wxT("\n\nConfirm ?"), wxT("spatialite_gui"),
                 wxYES_NO | wxICON_QUESTION, this);
  if (ret != wxYES)
    return;
  strcpy(xname, TableName.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql =
    wxT("DELETE FROM ") + wxString::FromUTF8(xname) + wxT(" WHERE ROWID = ") +
    rowid;
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      error = true;
    }
  if (error == false)
    {
      // marking row as deleted
      varRow->SetDeleted();
      for (i = 0; i < TableView->GetNumberCols(); i++)
        {
          TableView->SetCellTextColour(CurrentEvtRow, i, wxColour(0, 0, 0));
          TableView->SetCellBackgroundColour(CurrentEvtRow, i,
                                             wxColour(128, 128, 128));
          TableView->SetReadOnly(CurrentEvtRow, i);
        }
      TableView->ForceRefresh();
    }
}

void MyResultSetView::OnCmdInsert(wxCommandEvent & WXUNUSED(event))
{
//
// user required row insertion
//
  int i;
  if (ReadOnly == true)
    return;
  if (InsertPending == true)
    {
      // an INSERT is still pending, and the user required actual insertion
      DoInsert(true);
      return;
    }
  InsertRow = new MyRowVariant(TableView->GetNumberCols());
  TableView->MakeCellVisible(TableView->GetNumberRows() - 1, 1);
  InsertPending = true;
  for (i = 1; i < TableView->GetNumberCols(); i++)
    {
      TableView->SetCellValue(TableView->GetNumberRows() - 1, i, wxT(""));
      TableView->SetCellBackgroundColour(TableView->GetNumberRows() - 1, i,
                                         wxColour(255, 255, 255));
      TableView->SetReadOnly(TableView->GetNumberRows() - 1, i, false);
    }
}

void MyResultSetView::OnCmdAbort(wxCommandEvent & WXUNUSED(event))
{
//
// user cancelled current row insertion
//
  int i;
  if (InsertPending)
    {
      InsertPending = false;
      delete InsertRow;
      InsertRow = NULL;
      for (i = 0; i < TableView->GetNumberCols(); i++)
        {
          TableView->SetCellValue(TableView->GetNumberRows() - 1, i, wxT(""));
          TableView->SetCellBackgroundColour(TableView->GetNumberRows() - 1, i,
                                             wxColour(0, 0, 0));
          TableView->SetReadOnly(TableView->GetNumberRows() - 1, i);
        }
    }
}

void MyResultSetView::OnCmdClearSelection(wxCommandEvent & WXUNUSED(event))
{
//
// clearing current selection
//
  TableView->ClearSelection();
}

void MyResultSetView::OnCmdSelectAll(wxCommandEvent & WXUNUSED(event))
{
//
// selecting all
//
  TableView->SelectAll();
}

void MyResultSetView::OnCmdSelectRow(wxCommandEvent & WXUNUSED(event))
{
//
// selecting the current row
//
  TableView->SelectRow(CurrentEvtRow);
}

void MyResultSetView::OnCmdSelectColumn(wxCommandEvent & WXUNUSED(event))
{
//
// selecting column
//
  TableView->SelectCol(CurrentEvtColumn);
}

void MyResultSetView::OnCmdCopy(wxCommandEvent & WXUNUSED(event))
{
//
// copying the selection into the clipboard
//
  wxString copyData;
  int row;
  int col;
  bool newRow;
  bool firstRow = true;
  for (row = 0; row < TableView->GetNumberRows(); row++)
    {
      newRow = true;
      for (col = 0; col < TableView->GetNumberCols(); col++)
        {
          if (TableView->IsInSelection(row, col) == true)
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
              copyData += TableView->GetCellValue(row, col);
            }
        }
    }
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(copyData));
      wxTheClipboard->Close();
    }
}

void MyResultSetView::OnCmdBlob(wxCommandEvent & WXUNUSED(event))
{
//
// exploring some BLOB value
//
  if (!CurrentBlob)
    return;
  BlobExplorerDialog dlg;
  dlg.Create(MainFrame, CurrentBlob->GetBlobSize(), CurrentBlob->GetBlob());
  dlg.ShowModal();
}

void MyResultSetView::OnCmdBlobIn(wxCommandEvent & WXUNUSED(event))
{
// importing an external file into a BLOB-value
  FILE *in = NULL;
  char path[2048];
  int rd;
  int maxSize = 1024 * 1024;    // limit BLOB size to 1MB
  wxString fileList;
  wxString rowid;
  wxString sql;
  wxString blobValue;
  wxString hex;
  MyVariant *value;
  char dummy[1024];
  bool error = false;
  unsigned char *buffer = NULL;
  int ret;
  char *errMsg = NULL;
  wxString lastDir;
  char xname[1024];
  fileList =
    wxT
    ("BLOB Document (*.jpg;*.jpeg;*.png;*.gif;*.tif;*.pdf;*.zip)|*.jpg;*.jpeg;*.png;*.gif;*.tif;*.pdf;*.zip|");
  fileList +=
    wxT
    ("Image (*.jpg;*.jpeg;*.png;*.gif;*.tif)|*.jpg;*.jpeg;*.png;*.gif;*.tif|");
  fileList +=
    wxT
    ("JPEG Image (*.jpg;*.jpeg)|*.jpg;*.jpeg|PNG Image (*.png)|*.png|GIF Image (*.gif)|*.gif|TIFF Image (*.tif)|*.tif|");
  fileList +=
    wxT("PDF Document (*.pdf)|*.pdf|ZIP Archive|(*.zip)|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("loading a BLOB value"),
                          wxT(""), wxT(""), fileList,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(path, fileDialog.GetPath().ToUTF8());
      in = fopen(path, "rb");
      if (!in)
        {
          wxMessageBox(wxT("Cannot open '") + fileDialog.GetPath() +
                       wxT("' for reading"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          return;
        }
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      ::wxBeginBusyCursor();
      buffer = new unsigned char[maxSize];
      rd = fread(buffer, 1, maxSize, in);
      if (rd == maxSize && !(feof(in)))
        {
          // exceding 1MB; it's too big for a BLOB
          wxMessageBox(wxT
                       ("Selected file excedes 1MB; cowardly refusing to load it as a BLOB value ..."),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto end;
        }
      if (ferror(in))
        {
          // I/O error
          wxMessageBox(wxT("an I/O error occurred"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          goto end;
        }
      //
      // preparing  the SQL UPDATE statement
      //
      value = TableValues->GetValue(CurrentEvtRow, 0);
      sprintf(dummy, FORMAT_64, value->GetIntValue());
      rowid = wxString::FromUTF8(dummy);
      HexBlobValue(buffer, rd, hex);
      sql = wxT("UPDATE ");
      strcpy(xname, TableName.ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(" SET ");
      strcpy(xname, TableView->GetColLabelValue(CurrentEvtColumn).ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(" = ") + hex + wxT(" WHERE ROWID = ") + rowid;
      ret =
        sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          error = true;
        }
      value = TableValues->GetValue(CurrentEvtRow, CurrentEvtColumn);
      if (error == false)
        {
          // updating the Grid cell
          sprintf(dummy, "BLOB sz=%d ", rd);
          blobValue = wxString::FromUTF8(dummy);
          switch (gaiaGuessBlobType(buffer, rd))
            {
              case GAIA_JPEG_BLOB:
                blobValue += wxT("JPEG image");
                break;
              case GAIA_EXIF_BLOB:
                blobValue += wxT("JPEG-EXIF image");
                break;
              case GAIA_EXIF_GPS_BLOB:
                blobValue += wxT("JPEG-EXIF-GPS image");
                break;
              case GAIA_PNG_BLOB:
                blobValue += wxT("PNG image");
                break;
              case GAIA_GIF_BLOB:
                blobValue += wxT("GIF image");
                break;
              case GAIA_TIFF_BLOB:
                blobValue += wxT("TIFF image");
                break;
              case GAIA_PDF_BLOB:
                blobValue += wxT("PDF document");
                break;
              case GAIA_ZIP_BLOB:
                blobValue += wxT("ZIP archive");
                break;
              default:
                blobValue += wxT("UNKNOWN type");
                break;
            };
          TableView->SetCellValue(CurrentEvtRow, CurrentEvtColumn, blobValue);
          TableView->SetCellTextColour(CurrentEvtRow, CurrentEvtColumn,
                                       wxColour(0, 0, 192));
          TableView->SetCellBackgroundColour(CurrentEvtRow, CurrentEvtColumn,
                                             wxColour(255, 255, 204));
          TableView->ForceRefresh();
        }
    }
end:
// clean-up
  ::wxEndBusyCursor();
  if (in)
    fclose(in);
  if (buffer)
    delete[]buffer;
}

void MyResultSetView::HexBlobValue(unsigned char *blob, int size,
                                   wxString & hex)
{
//
// builds the HEX BLOB as X'01234567890abcdef'
//
  int i;
  char digit[16];
  hex = wxT("X'");
  for (i = 0; i < size; i++)
    {
      sprintf(digit, "%02x", *(blob + i));
      hex += wxString::FromUTF8(digit);
    }
  hex += wxT("'");
}

void MyResultSetView::OnCmdBlobOut(wxCommandEvent & WXUNUSED(event))
{
// exporting to external file a BLOB-value
  int blobType;
  wxString fileName;
  wxString fileType;
  int ret;
  wxString path;
  FILE *out = NULL;
  char xpath[2048];
  int wr;
  wxString lastDir;
  if (!CurrentBlob)
    return;
  blobType =
    gaiaGuessBlobType(CurrentBlob->GetBlob(), CurrentBlob->GetBlobSize());
  switch (blobType)
    {
      case GAIA_JPEG_BLOB:
      case GAIA_EXIF_BLOB:
      case GAIA_EXIF_GPS_BLOB:
        fileName = wxT("image.jpg");
        fileType = wxT("File JPEG (*.jpg;*.jpeg)|*.jpg");
        break;
      case GAIA_PNG_BLOB:
        fileName = wxT("image.png");
        fileType = wxT("File PNG (*.png)|*.png");
        break;
      case GAIA_GIF_BLOB:
        fileName = wxT("image.gif");
        fileType = wxT("File GIF (*.gif)|*.gif");
        break;
      case GAIA_TIFF_BLOB:
        fileName = wxT("image.tiff");
        fileType = wxT("File TIFF (*.tif)|*.tif");
        break;
      case GAIA_PDF_BLOB:
        fileName = wxT("document.pdf");
        fileType = wxT("PDF document (*.pdf)|*.pdf");
        break;
      case GAIA_ZIP_BLOB:
        fileName = wxT("archive.zip");
        fileType = wxT("ZIP Archive (*.zip)|*.zip");
        break;
      default:
        fileName = wxT("file");
    };
  fileType += wxT("|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("exporting a BLOB value to file"),
                          wxT(""), fileName, fileType,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      switch (blobType)
        {
          case GAIA_JPEG_BLOB:
          case GAIA_EXIF_BLOB:
          case GAIA_EXIF_GPS_BLOB:
            path += wxT(".jpg");
            break;
          case GAIA_PNG_BLOB:
            path += wxT(".png");
            break;
          case GAIA_GIF_BLOB:
            path += wxT(".gif");
            break;
          case GAIA_TIFF_BLOB:
            path += wxT(".tif");
            break;
          case GAIA_PDF_BLOB:
            path += wxT(".pdf");
            break;
          case GAIA_ZIP_BLOB:
            path += wxT(".zip");
            break;
          default:
            path += file.GetExt();
        };
      strcpy(xpath, path.ToUTF8());
      out = fopen(xpath, "wb");
      if (!out)
        {
          wxMessageBox(wxT("Cannot open '") + path + wxT("' for writing"),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          return;
        }
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      ::wxBeginBusyCursor();
      wr = fwrite(CurrentBlob->GetBlob(), 1, CurrentBlob->GetBlobSize(), out);
      if (wr != CurrentBlob->GetBlobSize())
        {
          wxMessageBox(wxT("an I/O error occurred"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
        }
      fclose(out);
      ::wxEndBusyCursor();
    }
}

void MyResultSetView::OnCmdXmlBlobIn(wxCommandEvent & WXUNUSED(event))
{
// importing an external XMLDocument into an XmlBLOB-value
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */
  FILE *in = NULL;
  char path[2048];
  int rd;
  int maxSize = 1024 * 1024;    // limit XmlBLOB size to 1MB
  wxString fileList;
  wxString rowid;
  wxString sql;
  wxString blobValue;
  wxString hex;
  MyVariant *value;
  char dummy[1024];
  bool error = false;
  unsigned char *buffer = NULL;
  int ret;
  char *errMsg = NULL;
  wxString lastDir;
  char xname[1024];
  unsigned char *xml = NULL;
  int xml_size;
  int compressed;
  char *schemaURI;
  char xschema[8192];
  fileList = wxT("XML Document (*.xml)|*.xml|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, wxT("loading an XmlBLOB value"),
                          wxT(""), wxT(""), fileList,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(path, fileDialog.GetPath().ToUTF8());
      in = fopen(path, "rb");
      if (!in)
        {
          wxMessageBox(wxT("Cannot open '") + fileDialog.GetPath() +
                       wxT("' for reading"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          return;
        }
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      ::wxBeginBusyCursor();
      buffer = new unsigned char[maxSize];
      rd = fread(buffer, 1, maxSize, in);
      if (rd == maxSize && !(feof(in)))
        {
          // exceding 1MB; it's too big for a BLOB
          wxMessageBox(wxT
                       ("Selected file excedes 1MB; cowardly refusing to load it as an XmlBLOB value ..."),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto end;
        }
      if (ferror(in))
        {
          // I/O error
          wxMessageBox(wxT("an I/O error occurred"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          goto end;
        }

      LoadXmlDialog dlg;
      wxString inPath = fileDialog.GetPath();
      dlg.Create(MainFrame, inPath);
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          if (dlg.IsCompressed() == true)
            compressed = 1;
          else
            compressed = 0;
          wxString schema = dlg.GetSchemaURI();
          if (schema.Len() == 0)
            schemaURI = NULL;
          else
            {
              strcpy(xschema, schema.ToUTF8());
              schemaURI = xschema;
            }
      } else
        goto end;

// attempting to parse (and possibly validate) the XML
      gaiaXmlToBlob(MainFrame->GetInternalCache(), buffer, rd, compressed,
                    schemaURI, &xml, &xml_size, NULL, NULL);
      if (xml == NULL)
        {
          wxMessageBox(wxT("Invalid XML ... unable to parse or validate"),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto end;
        }
//
// preparing  the SQL UPDATE statement
//
      value = TableValues->GetValue(CurrentEvtRow, 0);
      sprintf(dummy, FORMAT_64, value->GetIntValue());
      rowid = wxString::FromUTF8(dummy);
      HexBlobValue(xml, xml_size, hex);
      sql = wxT("UPDATE ");
      strcpy(xname, TableName.ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(" SET ");
      strcpy(xname, TableView->GetColLabelValue(CurrentEvtColumn).ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(" = ") + hex + wxT(" WHERE ROWID = ") + rowid;
      ret =
        sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          error = true;
        }
      value = TableValues->GetValue(CurrentEvtRow, CurrentEvtColumn);
      if (error == false)
        {
          // updating the Grid cell
          int doc_size = gaiaXmlBlobGetDocumentSize(xml, xml_size);
          sprintf(dummy, "XmlBLOB sz=%d (XMLsz=%d) ", xml_size, doc_size);
          blobValue = wxString::FromUTF8(dummy);
          if (gaiaIsSchemaValidatedXmlBlob(xml, xml_size))
            blobValue += wxT(" SchemaValidated");
          TableView->SetCellValue(CurrentEvtRow, CurrentEvtColumn, blobValue);
          TableView->SetCellTextColour(CurrentEvtRow, CurrentEvtColumn,
                                       wxColour(0, 0, 192));
          TableView->SetCellBackgroundColour(CurrentEvtRow, CurrentEvtColumn,
                                             wxColour(255, 255, 204));
          TableView->ForceRefresh();
        }
    }
end:
// clean-up
  ::wxEndBusyCursor();
  if (in)
    fclose(in);
  if (buffer)
    delete[]buffer;
  if (xml)
    free(xml);

#else

  wxMessageBox(wxT
               ("Sorry, spatialite_gui was built disabling LIBXML2\n\nUnsupported operation"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);

#endif /* end LIBXML2 conditionals */
}

void MyResultSetView::XmlBlobOut(bool indented)
{
// exporting to external file an XmlBLOB-value
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */
  wxString fileName;
  wxString fileType;
  int ret;
  wxString path;
  FILE *out = NULL;
  char xpath[2048];
  int wr;
  wxString lastDir;
  unsigned char *xml;
  int xml_size;
  wxString title = wxT("exporting an XmlBLOB value to file ");
  if (!CurrentBlob)
    return;
  if (indented)
    {
      title += wxT("(not indented)");
      gaiaXmlFromBlob(CurrentBlob->GetBlob(), CurrentBlob->GetBlobSize(), 0,
                      &xml, &xml_size);
  } else
    {
      title += wxT("(indented)");
      gaiaXmlFromBlob(CurrentBlob->GetBlob(), CurrentBlob->GetBlobSize(), 1,
                      &xml, &xml_size);
    }
  if (xml == NULL)
    return;
  fileName = wxT("document.xml");
  fileType = wxT("XML Document (*.xml)|*.xml");
  fileType += wxT("|All files (*.*)|*.*");
  wxFileDialog fileDialog(this, title,
                          wxT(""), fileName, fileType,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".xml");
      strcpy(xpath, path.ToUTF8());
      out = fopen(xpath, "wb");
      if (!out)
        {
          wxMessageBox(wxT("Cannot open '") + path + wxT("' for writing"),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          free(xml);
          return;
        }
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      ::wxBeginBusyCursor();
      wr = fwrite(xml, 1, xml_size, out);
      if (wr != xml_size)
        {
          wxMessageBox(wxT("an I/O error occurred"), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
        }
      fclose(out);
      ::wxEndBusyCursor();
    }
  free(xml);

#else

  wxMessageBox(wxT
               ("Sorry, spatialite_gui was built disabling LIBXML2\n\nUnsupported operation"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);

#endif /* end LIBXML2 conditionals */
}

void MyResultSetView::OnCmdXmlBlobOut(wxCommandEvent & WXUNUSED(event))
{
// exporting to external file an XmlBLOB-value (not indented)
  XmlBlobOut(false);
}

void MyResultSetView::OnCmdXmlBlobOutIndented(wxCommandEvent & WXUNUSED(event))
{
// exporting to external file an XmlBLOB-value (indented)
  XmlBlobOut(true);
}

void MyResultSetView::OnCmdBlobNull(wxCommandEvent & WXUNUSED(event))
{
// setting to NULL a BLOB-value
  wxString rowid;
  wxString sql;
  int ret;
  char *errMsg = NULL;
  bool error = false;
  MyVariant *value;
  char dummy[256];
  char xname[1024];
  value = TableValues->GetValue(CurrentEvtRow, 0);
  sprintf(dummy, FORMAT_64, value->GetIntValue());
  rowid = wxString::FromUTF8(dummy);
  sql = wxT("UPDATE ");
  strcpy(xname, TableName.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" SET ");
  strcpy(xname, TableView->GetColLabelValue(CurrentEvtColumn).ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(" = NULL WHERE ROWID = ") + rowid;
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      error = true;
    }
  if (error == false)
    {
      // updating the Grid cell
      TableView->SetCellValue(CurrentEvtRow, CurrentEvtColumn, wxT("NULL"));
      TableView->SetCellTextColour(CurrentEvtRow, CurrentEvtColumn,
                                   wxColour(0, 0, 192));
      TableView->SetCellBackgroundColour(CurrentEvtRow, CurrentEvtColumn,
                                         wxColour(255, 255, 204));
      TableView->ForceRefresh();
    }
}

void MyResultSetView::OnCmdExpTxtTab(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as TXT-TAB
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  wxFileDialog fileDialog(this, wxT("Exporting the ResultSet as Txt/Tab file"),
                          wxT(""), wxT("result_set.txt"),
                          wxT("Txt/Tab file (*.txt)|*.txt|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".txt");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsTxtTab(path, sql,
                                             MainFrame->GetDefaultCharset());
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("TXT / TAB");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsTxtTab(path, sql, dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpCsv(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as CSV
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  wxFileDialog fileDialog(this, wxT("Exporting the ResultSet as CSV"),
                          wxT(""), wxT("result_set.csv"),
                          wxT("CSV file (*.csv)|*.csv|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".csv");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsCsv(path, sql,
                                          MainFrame->GetDefaultCharset());
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("CSV");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsCsv(path, sql, dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpHtml(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as HTML
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  wxFileDialog fileDialog(this, wxT("Exporting the ResultSet as HTML"),
                          wxT(""), wxT("result_set.html"),
                          wxT
                          ("HTML web page (*.html)|*.html|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".html");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsHtml(path, sql,
                                           MainFrame->GetSqlitePath(),
                                           MainFrame->GetDefaultCharset());
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("HTML");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsHtml(path, sql,
                                               MainFrame->GetSqlitePath(),
                                               dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpShp(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as Shapefile
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  wxFileDialog fileDialog(this, wxT("Exporting the ResultSet as Shapefile"),
                          wxT(""), wxT("shapefile.shp"),
                          wxT
                          ("Shapefile (*.shp)|*.shp|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {

      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsShp(path, sql,
                                          MainFrame->GetDefaultCharset());
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("Shapefile");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsShp(path, sql, dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpDif(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as DIF spreadsheet
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
// asking Decimal Point / Date-Times params
  DumpSpreadsheetDialog sheet_dlg;
  char decimal_point;
  bool date_times;
  sheet_dlg.Create(MainFrame);
  ret = sheet_dlg.ShowModal();
  if (ret == wxID_OK)
    {
      decimal_point = sheet_dlg.GetDecimalPoint();
      date_times = sheet_dlg.IsDateTimes();
  } else
    return;
  wxFileDialog fileDialog(this,
                          wxT("Exporting the ResultSet as DIF spreadsheet"),
                          wxT(""), wxT("spreadsheet.dif"),
                          wxT
                          ("DIF spreadsheet document (*.dif)|*.dif|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {

      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".dif");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsDif(path, sql,
                                          MainFrame->GetDefaultCharset(),
                                          decimal_point, date_times);
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("DIF spreadsheet");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsDif(path, sql, dlg.GetCharset(),
                                              decimal_point, date_times);
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpSylk(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as SYLK spreadsheet
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
// asking Decimal Point / Date-Times params
  DumpSpreadsheetDialog sheet_dlg;
  bool date_times;
  sheet_dlg.Create(MainFrame);
  ret = sheet_dlg.ShowModal();
  if (ret == wxID_OK)
    date_times = sheet_dlg.IsDateTimes();
  else
    return;
  wxFileDialog fileDialog(this,
                          wxT("Exporting the ResultSet as SYLK spreadsheet"),
                          wxT(""), wxT("spreadsheet.slk"),
                          wxT
                          ("SYLK spreadsheet document (*.slk)|*.slk|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {

      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".slk");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsSylk(path, sql,
                                           MainFrame->GetDefaultCharset(),
                                           date_times);
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("SYLK spreadsheet");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsSylk(path, sql, dlg.GetCharset(),
                                               date_times);
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyResultSetView::OnCmdExpDbf(wxCommandEvent & WXUNUSED(event))
{
//
// exporting the ResultSet as DBF archive
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  wxString sql = MainFrame->GetQueryView()->GetSqlCtrl()->GetValue();
  wxFileDialog fileDialog(this, wxT("Exporting the ResultSet as DBF archive"),
                          wxT(""), wxT("archive.dbf"),
                          wxT
                          ("DBF archive (*.dbf)|*.dbf|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {

      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".dbf");
      lastDir = file.GetPath();
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->ExportResultSetAsDbf(path, sql,
                                          MainFrame->GetDefaultCharset());
          ::wxEndBusyCursor();
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("DBF archive");
          dlg.Create(MainFrame, path, target, MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              MainFrame->ExportResultSetAsDbf(path, sql, dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}
