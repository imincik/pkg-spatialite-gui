/*
/ Wfs.cpp
/ WFS load data
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

bool WfsDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Load data from WFS datasource"))
      == false)
    return false;
  CurrentEvtRow = -1;
  CurrentEvtColumn = -1;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  ProgressTimer = new wxTimer(this, ID_WFS_TIMER);
  ProgressTimer->Stop();
  return true;
}

void WfsDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// URL group box
  wxStaticBox *urlBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("WFS URL - GetCapabilities"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *urlBoxSizer = new wxStaticBoxSizer(urlBox, wxVERTICAL);
  boxSizer->Add(urlBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// First row: GetCapabilities URL
  wxBoxSizer *urlSizer = new wxBoxSizer(wxVERTICAL);
  urlBoxSizer->Add(urlSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxTextCtrl *urlCtrl = new wxTextCtrl(this, ID_WFS_URL, wxT("http://"),
                                       wxDefaultPosition, wxSize(680, 22));
  urlSizer->Add(urlCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// command buttons
  wxBoxSizer *url2Sizer = new wxBoxSizer(wxHORIZONTAL);
  urlSizer->Add(url2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *catBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("WFS Catalog"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *catBoxSizer = new wxStaticBoxSizer(catBox, wxVERTICAL);
  url2Sizer->Add(catBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  catBoxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *query = new wxButton(this, ID_WFS_CATALOG, wxT("&Load"));
  okCancelBox->Add(query, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxButton *reset = new wxButton(this, ID_WFS_RESET, wxT("&Reset"));
  reset->Enable(false);
  okCancelBox->Add(reset, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxButton *quit = new wxButton(this, wxID_CANCEL, wxT("&Quit"));
  okCancelBox->Add(quit, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
// Keywords group box
  wxStaticBox *keyBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Filter WFS Layers by Keyword"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *keyBoxSizer = new wxStaticBoxSizer(keyBox, wxVERTICAL);
  url2Sizer->Add(keyBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *keySizer = new wxBoxSizer(wxHORIZONTAL);
  keyBoxSizer->Add(keySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxComboBox *keyList =
    new wxComboBox(this, ID_WFS_KEYWORD, wxT(""), wxDefaultPosition,
                   wxSize(200, 21), 0, NULL,
                   wxCB_DROPDOWN);
  keyList->Enable(false);
  keySizer->Add(keyList, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxButton *filter = new wxButton(this, ID_WFS_KEYFILTER, wxT("&Apply"));
  filter->Enable(false);
  keySizer->Add(filter, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  wxButton *keyReset = new wxButton(this, ID_WFS_KEYRESET, wxT("&Reset"));
  keyReset->Enable(false);
  keySizer->Add(keyReset, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
// the Catalog Grid
  wxBoxSizer *gridSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(gridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  WfsView = new wxGrid(this, wxID_ANY, wxPoint(5, 5), wxSize(700, 250));
  WfsView->CreateGrid(1, 3);
  WfsView->EnableEditing(false);
  WfsView->SetColLabelValue(0, wxT("Name"));
  WfsView->SetColLabelValue(1, wxT("Title"));
  WfsView->SetColLabelValue(2, wxT("Abstract"));
  gridSizer->Add(WfsView, 0, wxALIGN_RIGHT | wxALL, 5);
// seleted Layer group box
  wxStaticBox *lyrBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Selected WFS Layer"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *lyrBoxSizer = new wxStaticBoxSizer(lyrBox, wxVERTICAL);
  boxSizer->Add(lyrBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// First row: WFS options
  wxBoxSizer *wfsSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrBoxSizer->Add(wfsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString ver[2];
  ver[0] = wxT("WFS &1.0.0");
  ver[1] = wxT("WFS &1.1.0");
  wxRadioBox *versionBox = new wxRadioBox(this, ID_WFS_VERSION,
                                          wxT("WFS &Version"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 2,
                                          ver, 2,
                                          wxRA_SPECIFY_ROWS);
  versionBox->Enable(false);
  versionBox->SetSelection(1);
  wfsSizer->Add(versionBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxString mode[2];
  mode[0] = wxT("&single WFS request");
  mode[1] = wxT("&using WFS paging");
  wxRadioBox *pagingBox = new wxRadioBox(this, ID_WFS_PAGING,
                                         wxT("WFS &request"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         mode, 2,
                                         wxRA_SPECIFY_ROWS);
  pagingBox->Enable(false);
  pagingBox->SetSelection(0);
  wfsSizer->Add(pagingBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticBox *pageBox = new wxStaticBox(this, ID_WFS_PAGE,
                                         wxT("Monolithic WFS Request"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *pageBoxSizer = new wxStaticBoxSizer(pageBox, wxHORIZONTAL);
  wfsSizer->Add(pageBoxSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticText *maxLabel =
    new wxStaticText(this, ID_WFS_LABEL, wxT("Max &Features limit:"),
                     wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
  pageBoxSizer->Add(maxLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *maxCtrl = new wxTextCtrl(this, ID_WFS_MAX, wxT("-1"),
                                       wxDefaultPosition, wxSize(60, 22),
                                       wxTE_RIGHT);
  maxCtrl->Enable(false);
  pageBoxSizer->Add(maxCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticBox *swapBox = new wxStaticBox(this, wxID_ANY,
                                         wxT("Swap Axes"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *swapBoxSizer = new wxStaticBoxSizer(swapBox, wxHORIZONTAL);
  wfsSizer->Add(swapBoxSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxCheckBox *swapCtrl = new wxCheckBox(this, ID_WFS_SWAP,
                                        wxT("Swap Y,X"),
                                        wxDefaultPosition, wxDefaultSize);
  swapCtrl->SetValue(false);
  swapCtrl->Enable(false);
  swapBoxSizer->Add(swapCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// Second row: Layer and Srid
  wxBoxSizer *nameSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrBoxSizer->Add(nameSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *nameLabel =
    new wxStaticText(this, wxID_STATIC, wxT("WFS &Name:"));
  nameSizer->Add(nameLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *nameCtrl = new wxTextCtrl(this, ID_WFS_NAME, wxT(""),
                                        wxDefaultPosition, wxSize(400, 22),
                                        wxTE_READONLY);
  nameSizer->Add(nameCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  nameSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxComboBox *sridList =
    new wxComboBox(this, ID_WFS_SRID, wxT(""), wxDefaultPosition,
                   wxSize(100, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  sridList->Enable(false);
  nameSizer->Add(sridList, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// third row: extra URL args
  wxBoxSizer *extraSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrBoxSizer->Add(extraSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticText *extraLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&URL extra options:"));
  extraSizer->Add(extraLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *extraCtrl = new wxTextCtrl(this, ID_WFS_EXTRA, wxT(""),
                                         wxDefaultPosition, wxSize(550, 22));
  extraCtrl->Enable(false);
  extraSizer->Add(extraCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// fourth row: DB Table [target] 
  wxBoxSizer *dbSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrBoxSizer->Add(dbSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table:"));
  dbSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_WFS_TABLE, wxT(""),
                                         wxDefaultPosition, wxSize(150, 22));
  tableCtrl->Enable(false);
  dbSizer->Add(tableCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticText *pkLabel =
    new wxStaticText(this, wxID_STATIC, wxT("Primary &Key:"));
  dbSizer->Add(pkLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxComboBox *pkList =
    new wxComboBox(this, ID_WFS_PK, wxT(""), wxDefaultPosition,
                   wxSize(250, 21), 0, NULL, wxCB_DROPDOWN);
  pkList->Enable(false);
  dbSizer->Add(pkList, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxCheckBox *rtreeCtrl = new wxCheckBox(this, ID_WFS_RTREE,
                                         wxT("Spatial Index"),
                                         wxDefaultPosition, wxDefaultSize);
  rtreeCtrl->SetValue(false);
  rtreeCtrl->Enable(false);
  dbSizer->Add(rtreeCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// fifth row:  status and start button
  wxBoxSizer *statusSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrBoxSizer->Add(statusSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxButton *load = new wxButton(this, ID_WFS_LOAD, wxT("&Load data"));
  load->Enable(false);
  statusSizer->Add(load, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  Progress =
    new wxGauge(this, wxID_ANY, 20, wxDefaultPosition, wxSize(200, 21));
  statusSizer->Add(Progress, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticText *statusLabel = new wxStaticText(this, ID_WFS_STATUS, wxT(""),
                                               wxDefaultPosition, wxSize(300,
                                                                         21));
  statusSizer->Add(statusLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handlers
  Connect(wxID_ANY, wxEVT_GRID_CELL_LEFT_CLICK,
          (wxObjectEventFunction) & WfsDialog::OnLeftClick);
  Connect(wxID_ANY, wxEVT_GRID_CELL_RIGHT_CLICK,
          (wxObjectEventFunction) & WfsDialog::OnRightClick);
  Connect(ID_WFS_PAGING, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & WfsDialog::OnPagingChanged);
  Connect(ID_WFS_CATALOG, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnCatalog);
  Connect(ID_WFS_RESET, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnReset);
  Connect(ID_WFS_KEYFILTER, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnKeyFilter);
  Connect(ID_WFS_KEYRESET, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnKeyReset);
  Connect(ID_WFS_LOAD, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnLoadFromWfs);
  Connect(wxID_CANCEL, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnQuit);
  Connect(Wfs_Copy, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & WfsDialog::OnCmdCopy);
  Connect(Wfs_Layer, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & WfsDialog::OnCmdSelectLayer);
  Connect(ID_WFS_THREAD_FINISHED, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & WfsDialog::OnThreadFinished);
//
// setting up a Timer event handler for Refresh
//
  Connect(ID_WFS_TIMER, wxEVT_TIMER,
          wxTimerEventHandler(WfsDialog::OnRefreshTimer), NULL, this);
}

void WfsDialog::OnRefreshTimer(wxTimerEvent & WXUNUSED(event))
{
//
// Refreshing the Progress status
//
  if (Params.GetProgressCount() > Params.GetLastProgressCount())
    {
      Params.SetLastProgressCount(Params.GetProgressCount());
      ProgressUpdate(Params.GetProgressCount());
    }
  Progress->Show(true);
  Progress->Pulse();

  //
  // restarting the timer
  //
  ProgressTimer->Start(500, wxTIMER_ONE_SHOT);
}

void WfsDialog::OnCmdCopy(wxCommandEvent & WXUNUSED(event))
{
//
// copying all WFS Layer definitions into the clipboard
//
  wxString copyData;
  int row;
  int col;
  for (row = 0; row < WfsView->GetNumberRows(); row++)
    {
      for (col = 0; col < WfsView->GetNumberCols(); col++)
        {
          if (col != 0)
            copyData += wxT("\t");
          copyData += WfsView->GetCellValue(row, col);
        }
      copyData += wxT("\n");
    }
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(copyData));
      wxTheClipboard->Close();
    }
}

void WfsDialog::OnCmdSelectLayer(wxCommandEvent & WXUNUSED(event))
{
//
// setting the currently selected WFS Layer
//
  SelectLayer();
}

void WfsDialog::SelectLayer()
{
//
// setting the currently selected WFS Layer
//
  WfsView->Show(false);
  WfsView->ClearSelection();
  WfsView->SelectRow(CurrentEvtRow);
  WfsView->Show(true);
  wxString name = WfsView->GetCellValue(CurrentEvtRow, 0);
  gaiaWFSitemPtr layer = FindLayerByName(name);
  if (layer == NULL)
    return;
  wxTextCtrl *nameCtrl = (wxTextCtrl *) FindWindow(ID_WFS_NAME);
  wxString lyr_name;
  const char *x_name = get_wfs_item_name(layer);
  lyr_name = wxString::FromUTF8(x_name);
  nameCtrl->SetValue(lyr_name);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_SRID);
  comboCtrl->Clear();
  int maxSrid = get_wfs_layer_srid_count(layer);
  for (int s = 0; s < maxSrid; s++)
    {
      wxString str;
      str.Printf(wxT("%d"), get_wfs_layer_srid(layer, s));
      comboCtrl->Append(str);
    }
  comboCtrl->SetSelection(0);
  comboCtrl->Enable(true);
  wxRadioBox *versionBox = (wxRadioBox *) FindWindow(ID_WFS_VERSION);
  versionBox->Enable(true);
  versionBox->SetSelection(1);
  wxTextCtrl *maxCtrl = (wxTextCtrl *) FindWindow(ID_WFS_MAX);
  maxCtrl->SetValue(wxT("100"));
  maxCtrl->Enable(true);
  wxStaticText *maxLabel = (wxStaticText *) FindWindow(ID_WFS_LABEL);
  maxLabel->SetLabel(wxT("FeaturesPerPage"));
  wxStaticBox *pageBox = (wxStaticBox *) FindWindow(ID_WFS_PAGE);
  pageBox->SetLabel(wxT("Multiple WFS Paged Requests"));
  wxRadioBox *pagingCtrl = (wxRadioBox *) FindWindow(ID_WFS_PAGING);
  pagingCtrl->SetSelection(1);
  pagingCtrl->Enable(true);
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_WFS_TABLE);
  tableCtrl->SetValue(wxT(""));
  tableCtrl->Enable(true);
  wxTextCtrl *extraCtrl = (wxTextCtrl *) FindWindow(ID_WFS_EXTRA);
  extraCtrl->SetValue(wxT(""));
  extraCtrl->Enable(true);
  wxComboBox *pkList = (wxComboBox *) FindWindow(ID_WFS_PK);
  pkList->Clear();
  pkList->Append(wxT(""));
  char *url = get_wfs_describe_url(Catalog, x_name, NULL);
  if (url != NULL)
    {
      gaiaWFSschemaPtr schema = create_wfs_schema(url, (char *) x_name, NULL);
      if (schema != NULL)
        {
          int maxCol = get_wfs_schema_column_count(schema);
          for (int c = 0; c < maxCol; c++)
            {
              gaiaWFScolumnPtr column = get_wfs_schema_column(schema, c);
              if (column != NULL)
                {
                  const char *name;
                  int type;
                  int nillable;
                  if (get_wfs_schema_column_info
                      (column, &name, &type, &nillable) != 0)
                    {
                      wxString str = wxString::FromUTF8(name);
                      pkList->Append(str);
                    }
                }
            }
          destroy_wfs_schema(schema);
        }
      free(url);
    }
  pkList->SetSelection(0);
  pkList->Enable(true);
  wxCheckBox *rtreeCtrl = (wxCheckBox *) FindWindow(ID_WFS_RTREE);
  rtreeCtrl->SetValue(true);
  rtreeCtrl->Enable(true);
  wxCheckBox *swapCtrl = (wxCheckBox *) FindWindow(ID_WFS_SWAP);
  swapCtrl->SetValue(false);
  swapCtrl->Enable(true);
  wxButton *load = (wxButton *) FindWindow(ID_WFS_LOAD);
  load->Enable(true);
}

gaiaWFSitemPtr WfsDialog::FindLayerByName(wxString & name)
{
//
// retrieving a WFS Layer from the Catalog by its name
//
  int nLayers = get_wfs_catalog_count(Catalog);
  for (int i = 0; i < nLayers; i++)
    {
      gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
      wxString lyr_name;
      const char *x_name = get_wfs_item_name(layer);
      lyr_name = wxString::FromUTF8(x_name);
      if (name.Cmp(lyr_name) == 0)
        return layer;
    }
  return NULL;
}

void WfsDialog::OnLeftClick(wxGridEvent & event)
{
//
// left click on some cell [mouse action]
//
  int previous = CurrentEvtRow;
  CurrentEvtRow = event.GetRow();
  CurrentEvtColumn = event.GetCol();
  if (CurrentEvtRow != previous)
    SelectLayer();
}

void WfsDialog::OnRightClick(wxGridEvent & event)
{
//
// right click on some cell [mouse action]
//
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxPoint pt = event.GetPosition();
  CurrentEvtRow = event.GetRow();
  CurrentEvtColumn = event.GetCol();
  menuItem =
    new wxMenuItem(menu, Wfs_Layer, wxT("Select as the current WFS &Layer"));
  menu->Append(menuItem);
  menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, Wfs_Copy, wxT("&Copy the whole WFS Catalog"));
  menu->Append(menuItem);
  WfsView->PopupMenu(menu, pt);
}

void WfsDialog::OnPagingChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Monolithic / Paged selection changed
//
  wxRadioBox *pagingCtrl = (wxRadioBox *) FindWindow(ID_WFS_PAGING);
  wxTextCtrl *maxCtrl = (wxTextCtrl *) FindWindow(ID_WFS_MAX);
  wxStaticText *maxLabel = (wxStaticText *) FindWindow(ID_WFS_LABEL);
  wxStaticBox *pageBox = (wxStaticBox *) FindWindow(ID_WFS_PAGE);
  switch (pagingCtrl->GetSelection())
    {
      case 0:
        maxCtrl->SetValue(wxT("-1"));
        maxLabel->SetLabel(wxT("Max &Features limit:"));
        pageBox->SetLabel(wxT("Monolithic WFS Request"));
        break;
      case 1:
        maxCtrl->SetValue(wxT("100"));
        maxLabel->SetLabel(wxT("FeaturesPerPage:"));
        pageBox->SetLabel(wxT("Multiple WFS Paged Requests"));
        break;
    };
}

void WfsDialog::ResetProgress()
{
// resetting the Progress label
  wxStaticText *status = (wxStaticText *) FindWindow(ID_WFS_STATUS);
  status->SetLabel(wxT(""));
  status->Refresh();
  status->Update();
}

void WfsDialog::ProgressWait()
{
// monolithic download
  wxStaticText *status = (wxStaticText *) FindWindow(ID_WFS_STATUS);
  status->SetLabel(wxT("    Please wait ... WFS download in progress"));
  status->Refresh();
  status->Update();
}

void WfsDialog::ProgressUpdate(int rows)
{
// updating the current status (WFS paging)
  wxStaticText *status = (wxStaticText *) FindWindow(ID_WFS_STATUS);
  wxString msg;
  msg.Printf(wxT("    WFS Features loaded since now: %d"), rows);
  status->SetLabel(msg);
  status->Refresh();
  status->Update();
}

void WfsCallback(int rows, void *ptr)
{
// progress callback supporting WFS Paging
  int *p = (int *) ptr;
  *p = rows;
}

#ifdef _WIN32
DWORD WINAPI DoExecuteWfs(void *arg)
#else
void *DoExecuteWfs(void *arg)
#endif
{
//
// threaded function: processing a WFS download
//
  WfsParams *params = (WfsParams *) arg;
  char *err_msg = NULL;
  int rows;
  char url[8196];
  char alt_describe[4192];
  char layer_name[1024];
  char table[1024];
  char pk[1024];
  char *pk_ref = pk;
  wxString wUrl = params->GetUrl();
  wxString altDescribe = params->GetAltDescribeUri();

  if (params->GetExtra().Len() > 0)
    {
      // appending extra arguments to URL */
      if (wUrl.EndsWith(wxT("&")) == true
          || params->GetExtra().StartsWith(wxT("&")) == true)
        wUrl += params->GetExtra();
      else
        wUrl += wxT("&") + params->GetExtra();
    }

  strcpy(url, wUrl.ToUTF8());
  strcpy(alt_describe, altDescribe.ToUTF8());
  strcpy(layer_name, params->GetLayerName().ToUTF8());
  strcpy(table, params->GetTable().ToUTF8());
  wxString pkey = params->GetPrimaryKey();
  if (pkey.Len() == 0)
    pk_ref = NULL;
  else
    strcpy(pk, pkey.ToUTF8());
  int ret =
    load_from_wfs_paged(params->GetSqlite(), url, alt_describe, layer_name,
                        params->GetSwapAxes(), table,
                        pk_ref, params->GetSpatialIndex(),
                        params->GetPageSize(), &rows, &err_msg,
                        params->GetCallback(),
                        params->GetProgressCountPtr());
  params->SetRet(ret);
  params->SetErrMsg(err_msg);
  params->SetRows(rows);
  wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, ID_WFS_THREAD_FINISHED);
  params->GetMother()->GetEventHandler()->AddPendingEvent(event);
#ifdef _WIN32
  return 0;
#else
  pthread_exit(NULL);
#endif
}

void WfsDialog::OnLoadFromWfs(wxCommandEvent & WXUNUSED(event))
{
//
// attempting to load data from WFS
//
#ifdef _WIN32
  HANDLE thread_handle;
  DWORD dwThreadId;
#else
  pthread_t thread_id;
#endif
  wxTextCtrl *nameCtrl = (wxTextCtrl *) FindWindow(ID_WFS_NAME);
  wxTextCtrl *maxCtrl = (wxTextCtrl *) FindWindow(ID_WFS_MAX);
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_WFS_TABLE);
  wxString name = nameCtrl->GetValue();
  wxTextCtrl *extraCtrl = (wxTextCtrl *) FindWindow(ID_WFS_EXTRA);
  wxString extra = extraCtrl->GetValue();
  wxRadioBox *pagingCtrl = (wxRadioBox *) FindWindow(ID_WFS_PAGING);
  long max = -1;
  long page = 100;
  if (pagingCtrl->GetSelection() == 1)
    {
      max = -1;
      if (maxCtrl->GetValue().ToLong(&page) == false)
        page = 100;
  } else
    {
      page = -1;
      if (maxCtrl->GetValue().ToLong(&max) == false)
        max = -1;
    }
  long srid = -1;
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_SRID);
  int idSel = comboCtrl->GetSelection();
  if (idSel != wxNOT_FOUND)
    {
      if (comboCtrl->GetString(idSel).ToLong(&srid) == false)
        srid = -1;
    }
  wxRadioBox *versionCtrl = (wxRadioBox *) FindWindow(ID_WFS_VERSION);
  const char *version = "1.1.0";
  if (versionCtrl->GetSelection() == 0)
    version = "1.0.0";
  wxString table = tableCtrl->GetValue();
  if (table.Len() < 1)
    {
      wxMessageBox(wxT
                   ("You must specify some DB Table name [destination target] !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxComboBox *pkList = (wxComboBox *) FindWindow(ID_WFS_PK);
  wxString pk = pkList->GetValue();
  char xname[1024];
  strcpy(xname, name.ToUTF8());
  int rtree = 0;
  wxCheckBox *rtreeCtrl = (wxCheckBox *) FindWindow(ID_WFS_RTREE);
  if (rtreeCtrl->GetValue())
    rtree = 1;
  int swap_axes = 0;
  wxCheckBox *swapCtrl = (wxCheckBox *) FindWindow(ID_WFS_SWAP);
  if (swapCtrl->GetValue())
    swap_axes = 1;
  char *xurl = get_wfs_request_url(Catalog, xname, version, srid, max);
  wxString url = wxString::FromUTF8(xurl);
  free(xurl);
  char *xalt_describe = get_wfs_describe_url(Catalog, xname, version);
  wxString alt_describe = wxString::FromUTF8(xalt_describe);
  free(xalt_describe);
  char *err_msg = NULL;
  int rows;
  ::wxBeginBusyCursor();
  ProgressWait();
  Params.Initialize(this, MainFrame->GetSqlite(), url, alt_describe, name,
                    swap_axes, table, pk, rtree, page, extra, WfsCallback);
  ProgressTimer->Start(500, wxTIMER_ONE_SHOT);
  Enable(false);

#ifdef _WIN32
  thread_handle = CreateThread(NULL, 0, DoExecuteWfs, &Params, 0, &dwThreadId);
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
              pthread_create(&thread_id, &attr, DoExecuteWfs, &Params);
            }
        }
    }
  if (!ok_prior)
    {
      // failure: using standard priority
      pthread_create(&thread_id, NULL, DoExecuteWfs, &Params);
    }
#endif
}

void WfsDialog::OnThreadFinished(wxCommandEvent & WXUNUSED(event))
{
// resuming execution when WFS thread quits
  char url[1024];
  char xtable[1024];
  int ret = Params.GetRet();
  char *err_msg = Params.GetErrMsg();
  int rows = Params.GetRows();
  strcpy(url, Params.GetUrl().ToUTF8());
  strcpy(xtable, Params.GetTable().ToUTF8());

  Enable(true);
  ProgressTimer->Stop();
  Progress->SetValue(0);
  Progress->Hide();
  ResetProgress();
  ::wxEndBusyCursor();
  if (ret == 0)
    {
      char *xmsg;
      if (err_msg == NULL)
        xmsg = sqlite3_mprintf("Unable to load data from WFS:\nUnkwnon cause");
      else
        xmsg = sqlite3_mprintf("Unable to load data from WFS:\n%s", err_msg);
      wxString msg = wxString::FromUTF8(xmsg);
      sqlite3_free(xmsg);
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  } else
    {
      MainFrame->InitTableTree();
      char *xmsg =
        sqlite3_mprintf("inserted %d rows from WFS into table \"%s\"", rows,
                        xtable);
      wxString msg = wxString::FromUTF8(xmsg);
      sqlite3_free(xmsg);
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
    }
  if (err_msg)
    free(err_msg);
}

void WfsDialog::OnCatalog(wxCommandEvent & WXUNUSED(event))
{
//
// attempting to create a WFS Catalog from GetCapabilities
//
  wxTextCtrl *urlCtrl = (wxTextCtrl *) FindWindow(ID_WFS_URL);
  wxButton *catalogBtn = (wxButton *) FindWindow(ID_WFS_CATALOG);
  wxButton *resetBtn = (wxButton *) FindWindow(ID_WFS_RESET);
  wxString url = urlCtrl->GetValue();
  if (url.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some WFS GetCapabilities URL !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  char xurl[1024];
  char *err_msg;
  strcpy(xurl, url.ToUTF8());
  Catalog = create_wfs_catalog(xurl, &err_msg);
  if (Catalog == NULL)
    {
      wxString msg = wxString::FromUTF8(err_msg);
      wxMessageBox(wxT("unable to get a WFS Catalog\n\n") + msg,
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  } else
    {
      int nLayers = get_wfs_catalog_count(Catalog);
      WfsView->Show(false);
      WfsView->ClearSelection();
      CurrentEvtRow = -1;
      CurrentEvtColumn = -1;
      if (nLayers > 1)
        WfsView->AppendRows(nLayers - 1);
      for (int i = 0; i < nLayers; i++)
        {
          // populating the WFS Catalog
          gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
          wxString name;
          wxString title;
          wxString abstract;
          const char *x_name = get_wfs_item_name(layer);
          const char *x_title = get_wfs_item_title(layer);
          const char *x_abstract = get_wfs_item_abstract(layer);
          name = wxString::FromUTF8(x_name);
          if (x_title != NULL)
            title = wxString::FromUTF8(x_title);
          if (x_abstract != NULL)
            abstract = wxString::FromUTF8(x_abstract);
          WfsView->SetCellValue(i, 0, name);
          WfsView->SetCellValue(i, 1, title);
          WfsView->SetCellValue(i, 2, abstract);
        }
      WfsView->SetRowLabelSize(wxGRID_AUTOSIZE);
      WfsView->AutoSize();
      WfsView->SetSize(690, 240);
      WfsView->Show(true);
      urlCtrl->Enable(false);
      catalogBtn->Enable(false);
      resetBtn->Enable(true);
      Keywords = new WfsKeywords();
      for (int i = 0; i < nLayers; i++)
        {
          // populating the Keywords dictionary
          gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
          int kw = get_wfs_keyword_count(layer);
          for (int k = 0; k < kw; k++)
            Keywords->Add(get_wfs_keyword(layer, k));
        }
      Keywords->Sort();
      wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_KEYWORD);
      comboCtrl->Clear();
      comboCtrl->Append(wxT(""));
      int maxKey = Keywords->GetMaxSorted();
      for (int i = 0; i < maxKey; i++)
        {
          WfsKey *key = Keywords->GetKey(i);
          if (key != NULL)
            {
              // populating the Keywords ComboBox
              comboCtrl->Append(key->GetKeyword());
            }
        }
      comboCtrl->Enable(true);
      wxButton *filterBtn = (wxButton *) FindWindow(ID_WFS_KEYFILTER);
      filterBtn->Enable(true);
      wxButton *keyResetBtn = (wxButton *) FindWindow(ID_WFS_KEYRESET);
      keyResetBtn->Enable(true);
    }
  if (err_msg != NULL)
    free(err_msg);
}

void WfsDialog::OnReset(wxCommandEvent & WXUNUSED(event))
{
//
// resetting to initial empty state
//
  wxTextCtrl *urlCtrl = (wxTextCtrl *) FindWindow(ID_WFS_URL);
  wxButton *catalogBtn = (wxButton *) FindWindow(ID_WFS_CATALOG);
  wxButton *resetBtn = (wxButton *) FindWindow(ID_WFS_RESET);
  if (Catalog != NULL)
    destroy_wfs_catalog(Catalog);
  Catalog = NULL;
  urlCtrl->Enable(true);
  catalogBtn->Enable(true);
  resetBtn->Enable(false);
  WfsView->DeleteRows(1, WfsView->GetNumberRows() - 1);
  WfsView->Show(false);
  WfsView->ClearSelection();
  WfsView->SetCellValue(0, 0, wxT(""));
  WfsView->SetCellValue(0, 1, wxT(""));
  WfsView->SetCellValue(0, 2, wxT(""));
  WfsView->SetRowLabelSize(wxGRID_AUTOSIZE);
  WfsView->AutoSize();
  WfsView->SetSize(690, 240);
  WfsView->Show(true);
  CurrentEvtRow = -1;
  CurrentEvtColumn = -1;
  if (Keywords != NULL)
    delete Keywords;
  Keywords = NULL;
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_KEYWORD);
  comboCtrl->Enable(false);
  wxButton *filterBtn = (wxButton *) FindWindow(ID_WFS_KEYFILTER);
  filterBtn->Enable(false);
  wxButton *keyResetBtn = (wxButton *) FindWindow(ID_WFS_KEYRESET);
  keyResetBtn->Enable(false);
  wxTextCtrl *nameCtrl = (wxTextCtrl *) FindWindow(ID_WFS_NAME);
  nameCtrl->SetValue(wxT(""));
  comboCtrl = (wxComboBox *) FindWindow(ID_WFS_SRID);
  comboCtrl->Clear();
  comboCtrl->SetSelection(wxNOT_FOUND);
  comboCtrl->Enable(false);
  wxRadioBox *versionBox = (wxRadioBox *) FindWindow(ID_WFS_VERSION);
  versionBox->Enable(false);
  wxTextCtrl *maxCtrl = (wxTextCtrl *) FindWindow(ID_WFS_MAX);
  maxCtrl->SetValue(wxT("-1"));
  maxCtrl->Enable(false);
  wxStaticText *maxLabel = (wxStaticText *) FindWindow(ID_WFS_LABEL);
  maxLabel->SetLabel(wxT("Max &Features limit:"));
  wxStaticBox *pageBox = (wxStaticBox *) FindWindow(ID_WFS_PAGE);
  pageBox->SetLabel(wxT("Monolithic WFS Request"));
  wxRadioBox *pagingCtrl = (wxRadioBox *) FindWindow(ID_WFS_PAGING);
  pagingCtrl->SetSelection(0);
  pagingCtrl->Enable(false);
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_WFS_TABLE);
  tableCtrl->SetValue(wxT(""));
  tableCtrl->Enable(false);
  comboCtrl = (wxComboBox *) FindWindow(ID_WFS_PK);
  comboCtrl->Clear();
  comboCtrl->SetSelection(wxNOT_FOUND);
  comboCtrl->Enable(false);
  wxCheckBox *rtreeCtrl = (wxCheckBox *) FindWindow(ID_WFS_RTREE);
  rtreeCtrl->SetValue(false);
  rtreeCtrl->Enable(false);
  wxCheckBox *swapCtrl = (wxCheckBox *) FindWindow(ID_WFS_SWAP);
  swapCtrl->SetValue(false);
  swapCtrl->Enable(false);
  wxButton *load = (wxButton *) FindWindow(ID_WFS_LOAD);
  load->Enable(false);
  ResetProgress();
}

void WfsDialog::OnKeyFilter(wxCommandEvent & WXUNUSED(event))
{
//
// filtering by Keywords
//
  wxString key;
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_KEYWORD);
  key = comboCtrl->GetValue();
  if (key.Len() == 0)
    {
      wxMessageBox(wxT("You must specify some Keyword to be searched !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
// resetting the Grid
  WfsView->Show(false);
  WfsView->ClearSelection();
  WfsView->DeleteRows(1, WfsView->GetNumberRows() - 1);
  WfsView->SetCellValue(0, 0, wxT(""));
  WfsView->SetCellValue(0, 1, wxT(""));
  WfsView->SetCellValue(0, 2, wxT(""));
  WfsView->Show(true);
  CurrentEvtRow = -1;
  CurrentEvtColumn = -1;
  int filtered_rows = 0;
  int nLayers = get_wfs_catalog_count(Catalog);
  for (int i = 0; i < nLayers; i++)
    {
      // searching matching Keywords - pass #1
      gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
      bool match = false;
      int kw = get_wfs_keyword_count(layer);
      for (int k = 0; k < kw; k++)
        {
          wxString str = wxString::FromUTF8(get_wfs_keyword(layer, k));
          if (key.Len() > 0 && str.Cmp(key) == 0)
            match = true;
        }
      if (match)
        filtered_rows++;
    }
  if (filtered_rows > 1)
    WfsView->AppendRows(filtered_rows - 1);
  int irow = 0;
  for (int i = 0; i < nLayers; i++)
    {
      // searching matching Keywords - pass #2
      gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
      bool match = false;
      int kw = get_wfs_keyword_count(layer);
      for (int k = 0; k < kw; k++)
        {
          wxString str = wxString::FromUTF8(get_wfs_keyword(layer, k));
          if (key.Len() > 0 && str.Cmp(key) == 0)
            match = true;
        }
      if (match)
        {
          // inserting a filterd layer
          wxString name;
          wxString title;
          wxString abstract;
          const char *x_name = get_wfs_item_name(layer);
          const char *x_title = get_wfs_item_title(layer);
          const char *x_abstract = get_wfs_item_abstract(layer);
          name = wxString::FromUTF8(x_name);
          if (x_title != NULL)
            title = wxString::FromUTF8(x_title);
          if (x_abstract != NULL)
            abstract = wxString::FromUTF8(x_abstract);
          WfsView->SetCellValue(irow, 0, name);
          WfsView->SetCellValue(irow, 1, title);
          WfsView->SetCellValue(irow, 2, abstract);
          irow++;
        }
    }
  WfsView->SetRowLabelSize(wxGRID_AUTOSIZE);
  WfsView->AutoSize();
  WfsView->SetSize(690, 240);
  WfsView->Show(true);
}

void WfsDialog::OnKeyReset(wxCommandEvent & WXUNUSED(event))
{
//
// resetting any filter by Keywords
//
  WfsView->Show(false);
  WfsView->DeleteRows(1, WfsView->GetNumberRows() - 1);
  WfsView->SetCellValue(0, 0, wxT(""));
  WfsView->SetCellValue(0, 1, wxT(""));
  WfsView->SetCellValue(0, 2, wxT(""));
  int nLayers = get_wfs_catalog_count(Catalog);
  WfsView->Show(false);
  CurrentEvtRow = -1;
  CurrentEvtColumn = -1;
  if (nLayers > 1)
    WfsView->AppendRows(nLayers - 1);
  for (int i = 0; i < nLayers; i++)
    {
      // populating the WFS Catalog
      gaiaWFSitemPtr layer = get_wfs_catalog_item(Catalog, i);
      wxString name;
      wxString title;
      wxString abstract;
      const char *x_name = get_wfs_item_name(layer);
      const char *x_title = get_wfs_item_title(layer);
      const char *x_abstract = get_wfs_item_abstract(layer);
      name = wxString::FromUTF8(x_name);
      if (x_title != NULL)
        title = wxString::FromUTF8(x_title);
      if (x_abstract != NULL)
        abstract = wxString::FromUTF8(x_abstract);
      WfsView->SetCellValue(i, 0, name);
      WfsView->SetCellValue(i, 1, title);
      WfsView->SetCellValue(i, 2, abstract);
    }
  WfsView->SetRowLabelSize(wxGRID_AUTOSIZE);
  WfsView->AutoSize();
  WfsView->SetSize(690, 240);
  WfsView->Show(true);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_WFS_KEYWORD);
  comboCtrl->SetSelection(wxNOT_FOUND);
  wxTextCtrl *nameCtrl = (wxTextCtrl *) FindWindow(ID_WFS_NAME);
  nameCtrl->SetValue(wxT(""));
  comboCtrl = (wxComboBox *) FindWindow(ID_WFS_SRID);
  comboCtrl->Clear();
  comboCtrl->SetSelection(wxNOT_FOUND);
  comboCtrl->Enable(false);
  wxRadioBox *versionBox = (wxRadioBox *) FindWindow(ID_WFS_VERSION);
  versionBox->Enable(false);
  wxTextCtrl *maxCtrl = (wxTextCtrl *) FindWindow(ID_WFS_MAX);
  maxCtrl->SetValue(wxT("-1"));
  maxCtrl->Enable(false);
  wxStaticText *maxLabel = (wxStaticText *) FindWindow(ID_WFS_LABEL);
  maxLabel->SetLabel(wxT("Max &Features limit:"));
  wxStaticBox *pageBox = (wxStaticBox *) FindWindow(ID_WFS_PAGE);
  pageBox->SetLabel(wxT("Monolithic WFS Request"));
  wxRadioBox *pagingCtrl = (wxRadioBox *) FindWindow(ID_WFS_PAGING);
  pagingCtrl->SetSelection(0);
  pagingCtrl->Enable(false);
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_WFS_TABLE);
  tableCtrl->SetValue(wxT(""));
  tableCtrl->Enable(false);
  comboCtrl = (wxComboBox *) FindWindow(ID_WFS_PK);
  comboCtrl->Clear();
  comboCtrl->SetSelection(wxNOT_FOUND);
  comboCtrl->Enable(false);
  wxCheckBox *rtreeCtrl = (wxCheckBox *) FindWindow(ID_WFS_RTREE);
  rtreeCtrl->SetValue(false);
  rtreeCtrl->Enable(false);
  wxCheckBox *swapCtrl = (wxCheckBox *) FindWindow(ID_WFS_SWAP);
  swapCtrl->SetValue(false);
  swapCtrl->Enable(false);
  wxButton *load = (wxButton *) FindWindow(ID_WFS_LOAD);
  load->Enable(false);
  ResetProgress();
}

void WfsDialog::OnQuit(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_CANCEL);
}

WfsKeywords::~WfsKeywords()
{
// destructor
  WfsKey *key;
  WfsKey *n_key;
  if (SortedArray != NULL)
    delete[]SortedArray;
  key = First;
  while (key != NULL)
    {
      n_key = key->GetNext();
      delete key;
      key = n_key;
    }
}

void WfsKeywords::Add(const char *key)
{
// adding a new Keyword into the dictionary (if not already defined)
  if (key == NULL)
    return;
  wxString keyword = wxString::FromUTF8(key);
  WfsKey *pKey = First;
  while (pKey != NULL)
    {
      if (keyword.CmpNoCase(pKey->GetKeyword()) == 0)
        return;
      pKey = pKey->GetNext();
    }
// inserting a new Keyword
  pKey = new WfsKey(keyword);
  if (First == NULL)
    First = pKey;
  if (Last != NULL)
    Last->SetNext(pKey);
  Last = pKey;
}

void WfsKeywords::Sort()
{
// creating the sorted array of Keywords 
  int count = 0;
  WfsKey *key;
  WfsKey *key_prev;
  bool ok = true;
  if (SortedArray != NULL)
    delete[]SortedArray;
  SortedArray = NULL;
  key = First;
  while (key != NULL)
    {
      // counting how many Keywords are there
      count++;
      key = key->GetNext();
    }
  MaxSorted = count;
  if (MaxSorted == 0)
    return;
  SortedArray = new WfsKey *[MaxSorted];
  count = 0;
  key = First;
  while (key != NULL)
    {
      // inserting pointers into the Array
      *(SortedArray + count++) = key;
      key = key->GetNext();
    }
  while (ok)
    {
      // bubble sorting
      ok = false;
      for (count = 1; count < MaxSorted; count++)
        {
          key_prev = *(SortedArray + count - 1);
          key = *(SortedArray + count);
          if (key_prev->GetKeyword().CmpNoCase(key->GetKeyword()) > 0)
            {
              // swapping pointers
              *(SortedArray + count) = key_prev;
              *(SortedArray + count - 1) = key;
              ok = true;
            }
        }
    }
}

WfsKey *WfsKeywords::GetKey(int index)
{
// returning the Nth sorted Keyword
  if (SortedArray == NULL)
    return NULL;
  if (index >= 0 && index < MaxSorted)
    return *(SortedArray + index);
  return NULL;
}
