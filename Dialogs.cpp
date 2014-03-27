/*
/ Dialogs.cpp
/ various dialog classes
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

#include "wx/spinctrl.h"
#include "wx/listctrl.h"
#include "wx/html/htmlwin.h"

bool SanitizeAllGeometriesDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Sanitize All Geometries")) ==
      false)
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

void SanitizeAllGeometriesDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *msgSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(msgSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxString msg =
    wxT
    ("Do you really intend attempting to sanitize all invalid Geometries ?\n\n");
  msg +=
    wxT("This will imply evaluating ST_MakeValid() for each single invalid\n");
  msg += wxT("Geometry eventually stored within any \"layer\" defined in \n");
  msg +=
    wxT
    ("\"geometry_columns\", and could possibly require a substantial time\n");
  msg += wxT("for a huge DB\n\n");
  msg += wxT("A HTML diagnostic report will be created.");
  wxStaticText *msgLabel = new wxStaticText(this, wxID_STATIC, msg);
  msgSizer->Add(msgLabel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: TMP_PREFIX
  wxBoxSizer *tmpSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tmpSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tmpLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Prefix for Temporary Tables:"));
  tmpSizer->Add(tmpLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tmpCtrl = new wxTextCtrl(this, ID_SANEGEOM_PREFIX, wxT("tmp_"),
                                       wxDefaultPosition, wxSize(150, 22));
  tmpSizer->Add(tmpCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// YES - NO buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *yes = new wxButton(this, wxID_YES, wxT("&YES"));
  okCancelBox->Add(yes, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *no = new wxButton(this, wxID_NO, wxT("&NO"));
  okCancelBox->Add(no, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handlers
  Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SanitizeAllGeometriesDialog::OnYes);
  Connect(wxID_NO, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SanitizeAllGeometriesDialog::OnNo);
}

void SanitizeAllGeometriesDialog::OnNo(wxCommandEvent & WXUNUSED(event))
{
  wxDialog::EndModal(wxNO);
}

void SanitizeAllGeometriesDialog::OnYes(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tmpCtrl = (wxTextCtrl *) FindWindow(ID_SANEGEOM_PREFIX);
  TmpPrefix = tmpCtrl->GetValue();
  if (TmpPrefix.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some Prefix !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxDialog::EndModal(wxYES);
}

bool CheckGeometryDialog::Create(MyFrame * parent, wxString & table,
                                 wxString & geom)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Check Geometry Column")) == false)
    return false;
  Table = table;
  Geometry = geom;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void CheckGeometryDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: Layer selection (Table/Column)
  wxBoxSizer *msgSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(msgSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString msg =
    wxT
    ("Do you really intend checking for validity this Geometry Column ?\n\n");
  msg +=
    wxT
    ("This will imply evaluating ST_IsValid() for each single Geometry stored\n");
  msg +=
    wxT
    ("within the select Column, and could require a substantial time for a\n");
  msg += wxT("huge Table\n\n");
  msg += wxT("A HTML diagnostic report will be created.");
  wxStaticText *msgLabel = new wxStaticText(this, wxID_STATIC, msg);
  msgSizer->Add(msgLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *lyrSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(lyrSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *lyr1Sizer = new wxBoxSizer(wxHORIZONTAL);
  lyrSizer->Add(lyr1Sizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tblLabel = new wxStaticText(this, wxID_STATIC, wxT("Table:"));
  lyr1Sizer->Add(tblLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tblCtrl = new wxTextCtrl(this, wxID_ANY, Table,
                                       wxDefaultPosition, wxSize(150, 22),
                                       wxTE_READONLY);
  lyr1Sizer->Add(tblCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
  wxBoxSizer *lyr2Sizer = new wxBoxSizer(wxHORIZONTAL);
  lyrSizer->Add(lyr2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *geoLabel = new wxStaticText(this, wxID_STATIC, wxT("Geomety:"));
  lyr2Sizer->Add(geoLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *geoCtrl = new wxTextCtrl(this, wxID_ANY, Geometry,
                                       wxDefaultPosition, wxSize(150, 22),
                                       wxTE_READONLY);
  lyr2Sizer->Add(geoCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// YES - NO buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *yes = new wxButton(this, wxID_YES, wxT("&YES"));
  okCancelBox->Add(yes, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *no = new wxButton(this, wxID_NO, wxT("&NO"));
  okCancelBox->Add(no, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handlers
  Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & CheckGeometryDialog::OnYes);
  Connect(wxID_NO, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & CheckGeometryDialog::OnNo);
}

void CheckGeometryDialog::OnNo(wxCommandEvent & WXUNUSED(event))
{
  wxDialog::EndModal(wxNO);
}

void CheckGeometryDialog::OnYes(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxYES);
}

bool SanitizeGeometryDialog::Create(MyFrame * parent, wxString & table,
                                    wxString & geom)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Sanitize Geometry Column")) ==
      false)
    return false;
  Table = table;
  Geometry = geom;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void SanitizeGeometryDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: Layer selection (Table/Column)
  wxBoxSizer *msgSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(msgSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString msg =
    wxT
    ("Do you really intend attempting to sanitize all invalid Geometries for this Geometry Column ?\n\n");
  msg +=
    wxT
    ("This will imply evaluating ST_MakeValid() for each single invalid Geometry eventually\n");
  msg +=
    wxT
    ("stored within the selected column, and could possibly require a substantial time\n");
  msg += wxT("for a huge Table\n\n");
  msg += wxT("A HTML diagnostic report will be created.");
  wxStaticText *msgLabel = new wxStaticText(this, wxID_STATIC, msg);
  msgSizer->Add(msgLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *lyrSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(lyrSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *lyr1Sizer = new wxBoxSizer(wxHORIZONTAL);
  lyrSizer->Add(lyr1Sizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tblLabel = new wxStaticText(this, wxID_STATIC, wxT("Table:"));
  lyr1Sizer->Add(tblLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tblCtrl = new wxTextCtrl(this, wxID_ANY, Table,
                                       wxDefaultPosition, wxSize(150, 22),
                                       wxTE_READONLY);
  lyr1Sizer->Add(tblCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
  wxBoxSizer *lyr2Sizer = new wxBoxSizer(wxHORIZONTAL);
  lyrSizer->Add(lyr2Sizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *geoLabel = new wxStaticText(this, wxID_STATIC, wxT("Geomety:"));
  lyr2Sizer->Add(geoLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *geoCtrl = new wxTextCtrl(this, wxID_ANY, Geometry,
                                       wxDefaultPosition, wxSize(150, 22),
                                       wxTE_READONLY);
  lyr2Sizer->Add(geoCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// Second row: TMP_PREFIX
  wxBoxSizer *tmpSizer = new wxBoxSizer(wxHORIZONTAL);
  lyrSizer->Add(tmpSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tmpLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Prefix for Temporary Tables:"));
  tmpSizer->Add(tmpLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tmpCtrl = new wxTextCtrl(this, ID_SANEGEOM_PREFIX, wxT("tmp_"),
                                       wxDefaultPosition, wxSize(150, 22));
  tmpSizer->Add(tmpCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// YES - NO buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *yes = new wxButton(this, wxID_YES, wxT("&YES"));
  okCancelBox->Add(yes, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *no = new wxButton(this, wxID_NO, wxT("&NO"));
  okCancelBox->Add(no, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handlers
  Connect(wxID_YES, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SanitizeGeometryDialog::OnYes);
  Connect(wxID_NO, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SanitizeGeometryDialog::OnNo);
}

void SanitizeGeometryDialog::OnNo(wxCommandEvent & WXUNUSED(event))
{
  wxDialog::EndModal(wxNO);
}

void SanitizeGeometryDialog::OnYes(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tmpCtrl = (wxTextCtrl *) FindWindow(ID_SANEGEOM_PREFIX);
  TmpPrefix = tmpCtrl->GetValue();
  if (TmpPrefix.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some Prefix !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxDialog::EndModal(wxYES);
}

bool GisLayerAuthDialog::Create(MyFrame * parent, wxString & table,
                                wxString & geom, bool rdOnly, bool hidden)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Geometry = geom;
  ReadOnly = rdOnly;
  Hidden = hidden;
  if (Hidden == true)
    ReadOnly = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("GIS Layer authorizations")) ==
      false)
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

void GisLayerAuthDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_ANY, Table,
                                         wxDefaultPosition, wxSize(350, 22),
                                         wxTE_READONLY);
  tableCtrl->Enable(false);
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Geometry name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_ANY, Geometry,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: HIDDEN / VISIBLE
  wxBoxSizer *authSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(authSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString hidden[2];
  hidden[0] = wxT("&Visible [ordinary GIS Layer]");
  hidden[1] = wxT("&Hidden [not usable as a GIS Layer");
  wxRadioBox *hiddenBox = new wxRadioBox(this, ID_AUTH_HIDDEN,
                                         wxT("&GIS Layer status"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         hidden, 1,
                                         wxRA_SPECIFY_COLS);
  authSizer->Add(hiddenBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  if (Hidden == true)
    hiddenBox->SetSelection(1);
  else
    hiddenBox->SetSelection(0);
// fourth row: READ-ONLY
  wxString rdOnly[2];
  rdOnly[0] = wxT("&ReadWrite [allows editing]");
  rdOnly[1] = wxT("&ReadOnly [not editable]");
  wxRadioBox *rdOnlyBox = new wxRadioBox(this, ID_AUTH_RDONLY,
                                         wxT("&Editing mode"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         rdOnly, 1,
                                         wxRA_SPECIFY_COLS);
  authSizer->Add(rdOnlyBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  if (ReadOnly == true)
    rdOnlyBox->SetSelection(1);
  else
    rdOnlyBox->SetSelection(0);
  if (Hidden == true)
    rdOnlyBox->Enable(false);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handlers
  Connect(ID_AUTH_HIDDEN, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & GisLayerAuthDialog::OnHiddenChanged);
  Connect(ID_AUTH_RDONLY, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & GisLayerAuthDialog::OnReadOnlyChanged);
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & GisLayerAuthDialog::OnOk);
}

void GisLayerAuthDialog::OnHiddenChanged(wxCommandEvent & WXUNUSED(event))
{
//
// HIDDEN / VISIBLE selection changed
//
  wxRadioBox *hiddenCtrl = (wxRadioBox *) FindWindow(ID_AUTH_HIDDEN);
  wxRadioBox *rdOnlyCtrl = (wxRadioBox *) FindWindow(ID_AUTH_RDONLY);
  switch (hiddenCtrl->GetSelection())
    {
      case 0:
        Hidden = false;
        break;
      case 1:
        Hidden = true;
        break;
    };
  if (Hidden == true)
    {
      ReadOnly = false;
      rdOnlyCtrl->SetSelection(0);
      rdOnlyCtrl->Enable(false);
  } else
    {
      if (ReadOnly == false)
        rdOnlyCtrl->SetSelection(0);
      else
        rdOnlyCtrl->SetSelection(1);
      rdOnlyCtrl->Enable(true);
    }
}

void GisLayerAuthDialog::OnReadOnlyChanged(wxCommandEvent & WXUNUSED(event))
{
//
// READ-ONLY selection changed
//
  wxRadioBox *rdOnlyCtrl = (wxRadioBox *) FindWindow(ID_AUTH_RDONLY);
  switch (rdOnlyCtrl->GetSelection())
    {
      case 0:
        ReadOnly = false;
        break;
      case 1:
        ReadOnly = true;
        break;
    };
}

void GisLayerAuthDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool VirtualShpDialog::Create(MyFrame * parent, wxString & path,
                              wxString & table, wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Creating Virtual Shapefile")) ==
      false)
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

void VirtualShpDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the Shapefile path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_VIRTSHP_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: SRID
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(sridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  int srid = 0;
  if (MainFrame->GetMetaDataType() != METADATA_CURRENT)
    srid = -1;                  // old default SRID
  wxSpinCtrl *sridCtrl = new wxSpinCtrl(this, ID_VIRTSHP_SRID, wxEmptyString,
                                        wxDefaultPosition, wxSize(80, 20),
                                        wxSP_ARROW_KEYS,
                                        -1, 1000000, srid);
  sridSizer->Add(sridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// third row: CHARSET
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  sridSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_VIRTSHP_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & VirtualShpDialog::OnOk);
}

void VirtualShpDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_VIRTSHP_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxSpinCtrl *sridCtrl = (wxSpinCtrl *) FindWindow(ID_VIRTSHP_SRID);
  Srid = sridCtrl->GetValue();
  if (Srid <= 0)
    ;
  else if (MainFrame->SridNotExists(Srid) == true)
    {
      wxMessageBox(wxT("invalid SRID value"), wxT("spatialite_gui"),
                   wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_VIRTSHP_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxDialog::EndModal(wxID_OK);
}

bool VirtualTxtDialog::Create(MyFrame * parent, wxString & path,
                              wxString & table, wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Creating Virtual CSV/TXT")) ==
      false)
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

void VirtualTxtDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  FirstLineTitles = true;
  Separator = '\t';
  TextSeparator = '"';
  DecimalPointIsComma = false;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the CSV/TXT path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_VIRTTXT_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: OPTIONS
  wxBoxSizer *optSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *modeSizer = new wxBoxSizer(wxVERTICAL);
  optSizer->Add(modeSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxBoxSizer *mode1Sizer = new wxBoxSizer(wxHORIZONTAL);
  modeSizer->Add(mode1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *titleCtrl = new wxCheckBox(this, ID_VIRTTXT_TITLES,
                                         wxT
                                         ("First line contains column names"),
                                         wxDefaultPosition, wxDefaultSize);
  titleCtrl->SetValue(true);
  mode1Sizer->Add(titleCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxStaticBox *sepBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Separators"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *sepSizer = new wxStaticBoxSizer(sepBox, wxVERTICAL);
  modeSizer->Add(sepSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxBoxSizer *mode2Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode2Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString quotes[3];
  quotes[0] = wxT("&Double \"");
  quotes[1] = wxT("&Single '");
  quotes[2] = wxT("&None");
  wxRadioBox *textSeparatorBox = new wxRadioBox(this, ID_VIRTTXT_QUOTE,
                                                wxT("&Text separator: quotes"),
                                                wxDefaultPosition,
                                                wxDefaultSize, 3,
                                                quotes, 3,
                                                wxRA_SPECIFY_COLS);
  mode2Sizer->Add(textSeparatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode3Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode3Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString separators[6];
  separators[0] = wxT("&Tab");
  separators[1] = wxT("&Space");
  separators[2] = wxT("Co&mma ,");
  separators[3] = wxT("&Colon :");
  separators[4] = wxT("S&emicolon ;");
  separators[5] = wxT("&Other");
  wxRadioBox *separatorBox = new wxRadioBox(this, ID_VIRTTXT_SEPARATOR,
                                            wxT("&Column separator"),
                                            wxDefaultPosition, wxDefaultSize,
                                            6,
                                            separators, 2,
                                            wxRA_SPECIFY_COLS);
  mode3Sizer->Add(separatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode4Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode4Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *charSeparatorLabel = new wxStaticText(this, wxID_STATIC,
                                                      wxT("&Custom separator:"),
                                                      wxDefaultPosition,
                                                      wxDefaultSize, 0);
  mode4Sizer->Add(charSeparatorLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *dummy =
    new wxTextCtrl(this, ID_VIRTTXT_CHARSEPARATOR, wxT("Dummy Text pq"),
                   wxDefaultPosition,
                   wxDefaultSize, 0);
  wxSize sz = dummy->GetSize();
  delete dummy;
  sz.SetWidth(40);
  wxTextCtrl *charSeparatorCtrl =
    new wxTextCtrl(this, ID_VIRTTXT_CHARSEPARATOR, wxT("TAB"),
                   wxDefaultPosition, sz, 0);
  charSeparatorCtrl->SetMaxLength(1);
  charSeparatorCtrl->Enable(false);
  mode4Sizer->Add(charSeparatorCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode5Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode5Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString points[2];
  points[0] = wxT("&Point .");
  points[1] = wxT("&Comma ,");
  wxRadioBox *decimalSeparatorBox = new wxRadioBox(this, ID_VIRTTXT_POINT,
                                                   wxT("&Decimal separator"),
                                                   wxDefaultPosition,
                                                   wxDefaultSize, 2,
                                                   points, 2,
                                                   wxRA_SPECIFY_COLS);
  mode5Sizer->Add(decimalSeparatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// third row: CHARSET
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  optSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_VIRTTXT_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & VirtualTxtDialog::OnOk);
// appends event handler for radioboxes
  Connect(ID_VIRTTXT_QUOTE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & VirtualTxtDialog::OnQuote);
  Connect(ID_VIRTTXT_SEPARATOR, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & VirtualTxtDialog::OnSeparator);
  Connect(ID_VIRTTXT_POINT, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & VirtualTxtDialog::OnDecimalSeparator);
}

void VirtualTxtDialog::OnDecimalSeparator(wxCommandEvent & WXUNUSED(event))
{
//
// POINT selection changed
//
  wxRadioBox *decimalSeparatorCtrl =
    (wxRadioBox *) FindWindow(ID_VIRTTXT_POINT);
  switch (decimalSeparatorCtrl->GetSelection())
    {
      case 0:
        DecimalPointIsComma = false;
        break;
      case 1:
        DecimalPointIsComma = true;
        break;
    };
}

void VirtualTxtDialog::OnQuote(wxCommandEvent & WXUNUSED(event))
{
//
// QUOTE selection changed
//
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_VIRTTXT_QUOTE);
  switch (separatorCtrl->GetSelection())
    {
      case 0:
        TextSeparator = '"';
        break;
      case 1:
        TextSeparator = '\'';
        break;
      case 2:
        TextSeparator = '\0';
        break;
    };
}

void VirtualTxtDialog::OnSeparator(wxCommandEvent & WXUNUSED(event))
{
//
// SEPARATOR selection changed
//
  wxTextCtrl *charSeparatorCtrl =
    (wxTextCtrl *) FindWindow(ID_VIRTTXT_CHARSEPARATOR);
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_VIRTTXT_SEPARATOR);
  switch (separatorCtrl->GetSelection())
    {
      case 0:
        Separator = '\t';
        charSeparatorCtrl->SetValue(wxT("TAB"));
        break;
      case 1:
        Separator = ' ';
        charSeparatorCtrl->SetValue(wxT("SP"));
        break;
      case 2:
        Separator = ',';
        charSeparatorCtrl->SetValue(wxT(","));
        break;
      case 3:
        Separator = ':';
        charSeparatorCtrl->SetValue(wxT(":"));
        break;
      case 4:
        Separator = ';';
        charSeparatorCtrl->SetValue(wxT(";"));
        break;
      case 5:
        Separator = '|';
        charSeparatorCtrl->SetValue(wxT("|"));
        break;
    };
  if (separatorCtrl->GetSelection() == 5)
    charSeparatorCtrl->Enable(true);
  else
    charSeparatorCtrl->Enable(false);
}

void VirtualTxtDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxCheckBox *firstLineCtrl = (wxCheckBox *) FindWindow(ID_VIRTTXT_TITLES);
  FirstLineTitles = firstLineCtrl->IsChecked();
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_VIRTTXT_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_VIRTTXT_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_VIRTTXT_SEPARATOR);
  if (separatorCtrl->GetSelection() == 5)
    {
      wxTextCtrl *charSeparatorCtrl =
        (wxTextCtrl *) FindWindow(ID_VIRTTXT_CHARSEPARATOR);
      wxString separator = charSeparatorCtrl->GetValue();
      if (separator.Len() != 1)
        {
          wxMessageBox(wxT
                       ("you must specificy a single char as Custom Column Separator"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      char dummy[64];
      strcpy(dummy, separator.ToUTF8());
      Separator = *dummy;
    }
  wxDialog::EndModal(wxID_OK);
}

bool VirtualDbfDialog::Create(MyFrame * parent, wxString & path,
                              wxString & table, wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Creating Virtual DBF")) == false)
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

void VirtualDbfDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the DBF path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_VIRTDBF_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: CHARSET
  wxBoxSizer *optSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  optSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_VIRTDBF_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & VirtualDbfDialog::OnOk);
}

void VirtualDbfDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_VIRTDBF_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_VIRTDBF_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxDialog::EndModal(wxID_OK);
}

bool VirtualXLDialog::Create(MyFrame * parent, wxString & path,
                             wxString & table)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  FirstLineTitles = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Creating Virtual XL")) == false)
    return false;
  GetWorksheets();
  WorksheetIndex = 0;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void VirtualXLDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the XL path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_VIRTXL_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: Worksheets
  wxBoxSizer *optSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *sheetBox = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Worksheet Selection"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *sheetSizer = new wxStaticBoxSizer(sheetBox, wxHORIZONTAL);
  optSizer->Add(sheetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *sheetCtrl = new wxListBox(this, ID_VIRTXL_WORKSHEET,
                                       wxDefaultPosition, wxSize(400, 100),
                                       WorksheetCount, Worksheets,
                                       wxLB_SINGLE | wxLB_HSCROLL);
  sheetCtrl->SetFont(wxFont
                     (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                      wxFONTWEIGHT_NORMAL));
  sheetCtrl->SetSelection(0);
  sheetSizer->Add(sheetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// fourth row: First Line Titles
  wxCheckBox *titleCtrl = new wxCheckBox(this, ID_VIRTXL_TITLES,
                                         wxT
                                         ("First line contains column names"),
                                         wxDefaultPosition, wxDefaultSize);
  titleCtrl->SetValue(false);
  optSizer->Add(titleCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & VirtualXLDialog::OnOk);
}

void VirtualXLDialog::GetWorksheets()
{
// attempting to build the Worksheet List
#ifndef OMIT_FREEXL             /* only if FreeXL is enabled */
  Invalid = true;
  Worksheets = NULL;
  WorksheetCount = 0;
  int ret;
  const void *xl_handle;
  char dummy[1024];
  unsigned int info;
  unsigned int idx;
  const char *utf8_string;

// opening the .XLS file [Workbook] 
  strcpy(dummy, Path.ToUTF8());
  ret = freexl_open_info(dummy, &xl_handle);
  if (ret != FREEXL_OK)
    goto error;
// checking if Password protected
  ret = freexl_get_info(xl_handle, FREEXL_BIFF_PASSWORD, &info);
  if (ret != FREEXL_OK)
    goto error;
  if (info != FREEXL_BIFF_PLAIN)
    goto error;
// Worksheet entries
  ret = freexl_get_info(xl_handle, FREEXL_BIFF_SHEET_COUNT, &info);
  if (ret != FREEXL_OK)
    goto error;
  if (info == 0)
    goto error;

  Worksheets = new wxString[info];
  WorksheetCount = info;
  for (idx = 0; idx < info; idx++)
    {
      // fetching Worksheets entries
      unsigned int rows;
      unsigned short columns;

      ret = freexl_get_worksheet_name(xl_handle, idx, &utf8_string);
      if (ret != FREEXL_OK)
        goto error;
      ret = freexl_select_active_worksheet(xl_handle, idx);
      if (ret != FREEXL_OK)
        goto error;
      ret = freexl_worksheet_dimensions(xl_handle, &rows, &columns);
      if (ret != FREEXL_OK)
        goto error;
      if (utf8_string == NULL)
        sprintf(dummy, "%3u] NULL (unnamed) [%urows / %ucols]\n", idx, rows,
                columns);
      else
        sprintf(dummy, "%3u] %s [%urows / %ucols]\n", idx, utf8_string, rows,
                columns);
      Worksheets[idx] = wxString::FromUTF8(dummy);
    }
  Invalid = false;
  freexl_close(xl_handle);
  return;

error:
  freexl_close(xl_handle);
  if (Worksheets)
    delete[]Worksheets;
  Worksheets = new wxString[1];
  Worksheets[0] = wxT("ERROR: no Worksheet found");
  WorksheetCount = 1;
#endif /* end FreeXL conditional support */
}

void VirtualXLDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  if (Invalid)
    {
      wxMessageBox(wxT("You cannot confirm: INVALID XL datasource !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_VIRTXL_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *sheetCtrl = (wxListBox *) FindWindow(ID_VIRTXL_WORKSHEET);
  int idSel = sheetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Worksheet from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  WorksheetIndex = idSel;
  wxCheckBox *firstLineCtrl = (wxCheckBox *) FindWindow(ID_VIRTXL_TITLES);
  FirstLineTitles = firstLineCtrl->IsChecked();
  wxDialog::EndModal(wxID_OK);
}

bool LoadXLDialog::Create(MyFrame * parent, wxString & path, wxString & table)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  FirstLineTitles = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Load XL spreadsheet")) == false)
    return false;
  GetWorksheets();
  WorksheetIndex = 0;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void LoadXLDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the XL path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_LDXL_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: Worksheets
  wxBoxSizer *optSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *sheetBox = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Worksheet Selection"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *sheetSizer = new wxStaticBoxSizer(sheetBox, wxHORIZONTAL);
  optSizer->Add(sheetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *sheetCtrl = new wxListBox(this, ID_LDXL_WORKSHEET,
                                       wxDefaultPosition, wxSize(400, 100),
                                       WorksheetCount, Worksheets,
                                       wxLB_SINGLE | wxLB_HSCROLL);
  sheetCtrl->SetFont(wxFont
                     (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                      wxFONTWEIGHT_NORMAL));
  sheetCtrl->SetSelection(0);
  sheetSizer->Add(sheetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// fourth row: First Line Titles
  wxCheckBox *titleCtrl = new wxCheckBox(this, ID_LDXL_TITLES,
                                         wxT
                                         ("First line contains column names"),
                                         wxDefaultPosition, wxDefaultSize);
  titleCtrl->SetValue(false);
  optSizer->Add(titleCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & LoadXLDialog::OnOk);
}

void LoadXLDialog::GetWorksheets()
{
// attempting to build the Worksheet List
#ifndef OMIT_FREEXL             /* only if FreeXL is enabled */
  Invalid = true;
  Worksheets = NULL;
  WorksheetCount = 0;
  int ret;
  const void *xl_handle;
  char dummy[1024];
  unsigned int info;
  unsigned int idx;
  const char *utf8_string;

// opening the .XLS file [Workbook] 
  strcpy(dummy, Path.ToUTF8());
  ret = freexl_open_info(dummy, &xl_handle);
  if (ret != FREEXL_OK)
    goto error;
// checking if Password protected
  ret = freexl_get_info(xl_handle, FREEXL_BIFF_PASSWORD, &info);
  if (ret != FREEXL_OK)
    goto error;
  if (info != FREEXL_BIFF_PLAIN)
    goto error;
// Worksheet entries
  ret = freexl_get_info(xl_handle, FREEXL_BIFF_SHEET_COUNT, &info);
  if (ret != FREEXL_OK)
    goto error;
  if (info == 0)
    goto error;

  Worksheets = new wxString[info];
  WorksheetCount = info;
  for (idx = 0; idx < info; idx++)
    {
      // fetching Worksheets entries
      unsigned int rows;
      unsigned short columns;

      ret = freexl_get_worksheet_name(xl_handle, idx, &utf8_string);
      if (ret != FREEXL_OK)
        goto error;
      ret = freexl_select_active_worksheet(xl_handle, idx);
      if (ret != FREEXL_OK)
        goto error;
      ret = freexl_worksheet_dimensions(xl_handle, &rows, &columns);
      if (ret != FREEXL_OK)
        goto error;
      if (utf8_string == NULL)
        sprintf(dummy, "%3u] NULL (unnamed) [%urows / %ucols]\n", idx, rows,
                columns);
      else
        sprintf(dummy, "%3u] %s [%urows / %ucols]\n", idx, utf8_string, rows,
                columns);
      Worksheets[idx] = wxString::FromUTF8(dummy);
    }
  Invalid = false;
  freexl_close(xl_handle);
  return;

error:
  freexl_close(xl_handle);
  if (Worksheets)
    delete[]Worksheets;
  Worksheets = new wxString[1];
  Worksheets[0] = wxT("ERROR: no Worksheet found");
  WorksheetCount = 1;
#endif /* end FreeXL conditional support */
}

void LoadXLDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  if (Invalid)
    {
      wxMessageBox(wxT("You cannot confirm: INVALID XL datasource !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_LDXL_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *sheetCtrl = (wxListBox *) FindWindow(ID_LDXL_WORKSHEET);
  int idSel = sheetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Worksheet from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  WorksheetIndex = idSel;
  wxCheckBox *firstLineCtrl = (wxCheckBox *) FindWindow(ID_LDXL_TITLES);
  FirstLineTitles = firstLineCtrl->IsChecked();
  wxDialog::EndModal(wxID_OK);
}

bool LoadXmlDialog::Create(MyFrame * parent, wxString & path)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Compressed = true;
  Validate = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Loading an XML Document")) ==
      false)
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

void LoadXmlDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the Xml path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: Compressed XmlBLOB
  wxBoxSizer *comprSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(comprSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *compressedCtrl = new wxCheckBox(this, ID_LDXML_COMPRESSED,
                                              wxT("Compressed XML BLOB"),
                                              wxDefaultPosition, wxDefaultSize);
  compressedCtrl->SetValue(true);
  comprSizer->Add(compressedCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// third row: Schema Validation
  wxStaticBox *schemaBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Schema Validation"),
                                           wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *validateSizer = new wxStaticBoxSizer(schemaBox, wxVERTICAL);
  boxSizer->Add(validateSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *valSizer = new wxBoxSizer(wxHORIZONTAL);
  validateSizer->Add(valSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxCheckBox *validateCtrl = new wxCheckBox(this, ID_LDXML_VALIDATE,
                                            wxT("Apply Schema Validation"),
                                            wxDefaultPosition, wxDefaultSize);
  validateCtrl->SetValue(false);
  valSizer->Add(validateCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *schemaUriSizer = new wxBoxSizer(wxHORIZONTAL);
  validateSizer->Add(schemaUriSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticText *schemaLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Schema URI:"));
  schemaUriSizer->Add(schemaLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *schemaUriValue = new wxTextCtrl(this, ID_LDXML_SCHEMA_URI,
                                              wxT(""), wxDefaultPosition,
                                              wxSize(350, 22));
  schemaUriValue->Enable(false);
  schemaUriSizer->Add(schemaUriValue, 0, wxALIGN_RIGHT | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & LoadXmlDialog::OnOk);
  Connect(ID_LDXML_COMPRESSED, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & LoadXmlDialog::OnCompressionChanged);
  Connect(ID_LDXML_VALIDATE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & LoadXmlDialog::OnValidationChanged);
}

void LoadXmlDialog::OnCompressionChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Compressed (on/off): 
//
  wxCheckBox *compressedCtrl = (wxCheckBox *) FindWindow(ID_LDXML_COMPRESSED);
  if (compressedCtrl->IsChecked() == true)
    Compressed = true;
  else
    Compressed = false;
}

void LoadXmlDialog::OnValidationChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Schema Validation (on/off): 
//
  wxCheckBox *validateCtrl = (wxCheckBox *) FindWindow(ID_LDXML_VALIDATE);
  wxTextCtrl *schemaUriCtrl = (wxTextCtrl *) FindWindow(ID_LDXML_SCHEMA_URI);
  if (validateCtrl->IsChecked())
    schemaUriCtrl->Enable(true);
  else
    {
      SchemaURI = wxT("");
      schemaUriCtrl->SetValue(SchemaURI);
      schemaUriCtrl->Enable(false);
    }
}

void LoadXmlDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxCheckBox *validateCtrl = (wxCheckBox *) FindWindow(ID_LDXML_VALIDATE);
  if (validateCtrl->IsChecked())
    {
      wxTextCtrl *schemaUriCtrl =
        (wxTextCtrl *) FindWindow(ID_LDXML_SCHEMA_URI);
      wxString schemaUri = schemaUriCtrl->GetValue().Trim(false);
      SchemaURI = schemaUri.Trim(true);
      if (SchemaURI.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some Schema URI !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  wxDialog::EndModal(wxID_OK);
}

bool LoadTxtDialog::Create(MyFrame * parent, wxString & path, wxString & table,
                           wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Load CSV/TXT")) == false)
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

void LoadTxtDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  FirstLineTitles = true;
  Separator = '\t';
  TextSeparator = '"';
  DecimalPointIsComma = false;
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the CSV/TXT path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_LDTXT_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: OPTIONS
  wxBoxSizer *optSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *modeSizer = new wxBoxSizer(wxVERTICAL);
  optSizer->Add(modeSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxBoxSizer *mode1Sizer = new wxBoxSizer(wxHORIZONTAL);
  modeSizer->Add(mode1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *titleCtrl = new wxCheckBox(this, ID_LDTXT_TITLES,
                                         wxT
                                         ("First line contains column names"),
                                         wxDefaultPosition, wxDefaultSize);
  titleCtrl->SetValue(true);
  mode1Sizer->Add(titleCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxStaticBox *sepBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Separators"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *sepSizer = new wxStaticBoxSizer(sepBox, wxVERTICAL);
  modeSizer->Add(sepSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxBoxSizer *mode2Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode2Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString quotes[3];
  quotes[0] = wxT("&Double \"");
  quotes[1] = wxT("&Single '");
  quotes[2] = wxT("&None");
  wxRadioBox *textSeparatorBox = new wxRadioBox(this, ID_LDTXT_QUOTE,
                                                wxT("&Text separator: quotes"),
                                                wxDefaultPosition,
                                                wxDefaultSize, 3,
                                                quotes, 3,
                                                wxRA_SPECIFY_COLS);
  mode2Sizer->Add(textSeparatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode3Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode3Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString separators[6];
  separators[0] = wxT("&Tab");
  separators[1] = wxT("&Space");
  separators[2] = wxT("Co&mma ,");
  separators[3] = wxT("&Colon :");
  separators[4] = wxT("S&emicolon ;");
  separators[5] = wxT("&Other");
  wxRadioBox *separatorBox = new wxRadioBox(this, ID_LDTXT_SEPARATOR,
                                            wxT("&Column separator"),
                                            wxDefaultPosition, wxDefaultSize,
                                            6,
                                            separators, 2,
                                            wxRA_SPECIFY_COLS);
  mode3Sizer->Add(separatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode4Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode4Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *charSeparatorLabel = new wxStaticText(this, wxID_STATIC,
                                                      wxT("&Custom separator:"),
                                                      wxDefaultPosition,
                                                      wxDefaultSize, 0);
  mode4Sizer->Add(charSeparatorLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *dummy =
    new wxTextCtrl(this, ID_LDTXT_CHARSEPARATOR, wxT("Dummy Text pq"),
                   wxDefaultPosition,
                   wxDefaultSize, 0);
  wxSize sz = dummy->GetSize();
  delete dummy;
  sz.SetWidth(40);
  wxTextCtrl *charSeparatorCtrl =
    new wxTextCtrl(this, ID_LDTXT_CHARSEPARATOR, wxT("TAB"),
                   wxDefaultPosition, sz, 0);
  charSeparatorCtrl->SetMaxLength(1);
  charSeparatorCtrl->Enable(false);
  mode4Sizer->Add(charSeparatorCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *mode5Sizer = new wxBoxSizer(wxHORIZONTAL);
  sepSizer->Add(mode5Sizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString points[2];
  points[0] = wxT("&Point .");
  points[1] = wxT("&Comma ,");
  wxRadioBox *decimalSeparatorBox = new wxRadioBox(this, ID_LDTXT_POINT,
                                                   wxT("&Decimal separator"),
                                                   wxDefaultPosition,
                                                   wxDefaultSize, 2,
                                                   points, 2,
                                                   wxRA_SPECIFY_COLS);
  mode5Sizer->Add(decimalSeparatorBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// third row: CHARSET
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  optSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_LDTXT_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & LoadTxtDialog::OnOk);
// appends event handler for radioboxes
  Connect(ID_LDTXT_QUOTE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadTxtDialog::OnQuote);
  Connect(ID_LDTXT_SEPARATOR, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadTxtDialog::OnSeparator);
  Connect(ID_LDTXT_POINT, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadTxtDialog::OnDecimalSeparator);
}

void LoadTxtDialog::OnDecimalSeparator(wxCommandEvent & WXUNUSED(event))
{
//
// POINT selection changed
//
  wxRadioBox *decimalSeparatorCtrl = (wxRadioBox *) FindWindow(ID_LDTXT_POINT);
  switch (decimalSeparatorCtrl->GetSelection())
    {
      case 0:
        DecimalPointIsComma = false;
        break;
      case 1:
        DecimalPointIsComma = true;
        break;
    };
}

void LoadTxtDialog::OnQuote(wxCommandEvent & WXUNUSED(event))
{
//
// QUOTE selection changed
//
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_LDTXT_QUOTE);
  switch (separatorCtrl->GetSelection())
    {
      case 0:
        TextSeparator = '"';
        break;
      case 1:
        TextSeparator = '\'';
        break;
      case 2:
        TextSeparator = '\0';
        break;
    };
}

void LoadTxtDialog::OnSeparator(wxCommandEvent & WXUNUSED(event))
{
//
// SEPARATOR selection changed
//
  wxTextCtrl *charSeparatorCtrl =
    (wxTextCtrl *) FindWindow(ID_LDTXT_CHARSEPARATOR);
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_LDTXT_SEPARATOR);
  switch (separatorCtrl->GetSelection())
    {
      case 0:
        Separator = '\t';
        charSeparatorCtrl->SetValue(wxT("TAB"));
        break;
      case 1:
        Separator = ' ';
        charSeparatorCtrl->SetValue(wxT("SP"));
        break;
      case 2:
        Separator = ',';
        charSeparatorCtrl->SetValue(wxT(","));
        break;
      case 3:
        Separator = ':';
        charSeparatorCtrl->SetValue(wxT(":"));
        break;
      case 4:
        Separator = ';';
        charSeparatorCtrl->SetValue(wxT(";"));
        break;
      case 5:
        Separator = '|';
        charSeparatorCtrl->SetValue(wxT("|"));
        break;
    };
  if (separatorCtrl->GetSelection() == 5)
    charSeparatorCtrl->Enable(true);
  else
    charSeparatorCtrl->Enable(false);
}

void LoadTxtDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxCheckBox *firstLineCtrl = (wxCheckBox *) FindWindow(ID_LDTXT_TITLES);
  FirstLineTitles = firstLineCtrl->IsChecked();
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_LDTXT_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_LDTXT_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxRadioBox *separatorCtrl = (wxRadioBox *) FindWindow(ID_LDTXT_SEPARATOR);
  if (separatorCtrl->GetSelection() == 5)
    {
      wxTextCtrl *charSeparatorCtrl =
        (wxTextCtrl *) FindWindow(ID_LDTXT_CHARSEPARATOR);
      wxString separator = charSeparatorCtrl->GetValue();
      if (separator.Len() != 1)
        {
          wxMessageBox(wxT
                       ("you must specificy a single char as Custom Column Separator"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      char dummy[64];
      strcpy(dummy, separator.ToUTF8());
      Separator = *dummy;
    }
  wxDialog::EndModal(wxID_OK);
}

LoadDbfDialog::~LoadDbfDialog()
{
// destructor
  if (PKFields != NULL)
    delete[]PKFields;
  if (PKFieldsEx != NULL)
    delete[]PKFieldsEx;
}


bool LoadDbfDialog::Create(MyFrame * parent, wxString & path, wxString & table,
                           wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Default = defCs;
  PKCount = 0;
  PKFields = NULL;
  PKFieldsEx = NULL;
  LoadPKFields();
  if (wxDialog::Create(parent, wxID_ANY, wxT("Load DBF")) == false)
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

void LoadDbfDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the DBF path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_LDDBF_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: CHARSET
  wxBoxSizer *optSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  optSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_LDDBF_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// User Defined Primary Key   
  wxBoxSizer *userdefBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(userdefBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *userPKeyBox = new wxStaticBox(this, wxID_STATIC,
                                             wxT("Primary Key Column"),
                                             wxDefaultPosition,
                                             wxDefaultSize);
  wxBoxSizer *gPKeySizer = new wxStaticBoxSizer(userPKeyBox, wxVERTICAL);
  userdefBox->Add(gPKeySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxString pkey_selection[2];
  pkey_selection[0] = wxT("Automatic");
  pkey_selection[1] = wxT("User specified");
  wxRadioBox *gpkey_sel = new wxRadioBox(this, ID_LDDBF_USER_PKEY,
                                         wxT("&Mode"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         pkey_selection, 2,
                                         wxRA_SPECIFY_COLS);
  gpkey_sel->SetSelection(0);
  gPKeySizer->Add(gpkey_sel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxComboBox *pkeyList =
    new wxComboBox(this, ID_LDDBF_PKCOL, wxT(""), wxDefaultPosition,
                   wxSize(180, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  pkeyList->Enable(false);
  gPKeySizer->Add(pkeyList, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & LoadDbfDialog::OnOk);
  Connect(ID_LDDBF_USER_PKEY, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadDbfDialog::OnUserPKey);
}


void LoadDbfDialog::LoadPKFields()
{
// loading the Primary Key Fields list
  char dbf_path[1024];
  char charset[1024];
  char dummy[1024];
  wxString dummyName;
  int cnt;
  int seed;
  int dup;
  int idup;
  wxString *charsets = MainFrame->GetCharsets();
  wxString cs;
  int idSel = MainFrame->GetCharsetIndex(Default);
  gaiaDbfPtr dbf = NULL;
  gaiaDbfFieldPtr dbf_field;
  if (PKFields != NULL)
    delete[]PKFields;
  PKFields = NULL;
  PKCount = 0;

// opening the DBF in order to fetch field names 
  dbf = gaiaAllocDbf();
  strcpy(dbf_path, Path.ToUTF8());
  cs = *(charsets + idSel);
  strcpy(charset, cs.ToUTF8());
  gaiaOpenDbfRead(dbf, dbf_path, charset, "UTF-8");
  if (!(dbf->Valid))
    {
      gaiaFreeDbf(dbf);
      return;
    }
/* checking for duplicate / illegal column names and antialising them */
  dbf_field = dbf->Dbf->First;
  while (dbf_field)
    {
      /* counting DBF fields */
      PKCount++;
      dbf_field = dbf_field->Next;
    }
  PKFields = new wxString[PKCount];
  PKFieldsEx = new wxString[PKCount];
  cnt = 0;
  seed = 0;
  dbf_field = dbf->Dbf->First;
  while (dbf_field)
    {
      /* preparing column names */
      dummyName = wxString::FromUTF8(dbf_field->Name);
      dup = 0;
      for (idup = 0; idup < cnt; idup++)
        {
          if (dummyName == PKFields[idup])
            dup = 1;
        }
      if (dummyName == wxString::FromUTF8("PK_UID"))
        dup = 1;
      if (dup)
        {
          sprintf(dummy, "COL_%d", seed++);
          dummyName = wxString::FromUTF8(dummy);
        }
      PKFields[cnt] = dummyName;
      PKFieldsEx[cnt] = dummyName;
      dummyName = wxT(" [TEXT]");
      switch (dbf_field->Type)
        {
          case 'N':
            if (dbf_field->Decimals)
              dummyName = wxT(" [DOUBLE]");
            else
              {
                if (dbf_field->Length <= 18)
                  dummyName = wxT(" [INTEGER]");
                else
                  dummyName = wxT(" [DOUBLE]");
              }
            break;
          case 'D':
            dummyName = wxT(" [DOUBLE]");
            break;
          case 'F':
            dummyName = wxT(" [DOUBLE]");
            break;
          case 'L':
            dummyName = wxT(" [INTEGER]");
            break;
        };
      PKFieldsEx[cnt] += dummyName;
      cnt++;
      dbf_field = dbf_field->Next;
    }

// memory cleanup
  gaiaFreeDbf(dbf);
}

void LoadDbfDialog::OnUserPKey(wxCommandEvent & WXUNUSED(event))
{
//
// the mode of user defined Primary Key Column changed
//
  wxRadioBox *radioCtrl = (wxRadioBox *) FindWindow(ID_LDDBF_USER_PKEY);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_LDDBF_PKCOL);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      comboCtrl->Clear();
      comboCtrl->Enable(false);
  } else
    {
      // user specified
      int i;
      for (i = 0; i < PKCount; i++)
        {
          // setting PK Fields
          wxString str = PKFieldsEx[i];
          comboCtrl->Append(str);
        }
      comboCtrl->Enable(true);
      comboCtrl->SetSelection(wxNOT_FOUND);
    }
}


void LoadDbfDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_LDDBF_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_LDDBF_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxRadioBox *radioCtrl = (wxRadioBox *) FindWindow(ID_LDDBF_USER_PKEY);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_LDDBF_PKCOL);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      UserDefinedPKey = false;
      PKColumn = wxT("");
  } else
    {
      // user specified
      idSel = comboCtrl->GetSelection();
      if (idSel == wxNOT_FOUND)
        {
          wxMessageBox(wxT
                       ("you must select some User defined Primary Key Column from the list"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      UserDefinedPKey = true;
      PKColumn = PKFields[idSel];
    }
  wxDialog::EndModal(wxID_OK);
}

LoadShpDialog::~LoadShpDialog()
{
// destructor
  if (PKFields != NULL)
    delete[]PKFields;
  if (PKFieldsEx != NULL)
    delete[]PKFieldsEx;
}

bool LoadShpDialog::Create(MyFrame * parent, wxString & path, wxString & table,
                           int srid, wxString & column, wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Srid = srid;
  Column = column;
  Default = defCs;
  Coerce2D = false;
  Compressed = false;
  SpatialIndex = false;
  PKCount = 0;
  PKFields = NULL;
  PKFieldsEx = NULL;
  LoadPKFields();
  if (wxDialog::Create(parent, wxID_ANY, wxT("Load Shapefile")) == false)
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

void LoadShpDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
// first row: the Shapefile path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 1);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_LDSHP_TABLE, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 1);
// third row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&GeomColumn name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, ID_LDSHP_COLUMN, Column,
                                       wxDefaultPosition, wxSize(350, 22));
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 1);
// fourth row: SRID
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(sridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
  wxSpinCtrl *sridCtrl = new wxSpinCtrl(this, ID_LDSHP_SRID, wxEmptyString,
                                        wxDefaultPosition, wxSize(80, 20),
                                        wxSP_ARROW_KEYS,
                                        -1, 1000000, Srid);
  sridSizer->Add(sridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
// fifth row: CHARSET
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  sridSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *charsetCtrl = new wxListBox(this, ID_LDSHP_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// 2D + Compress
  wxBoxSizer *compressBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(compressBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *storageBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Geometry storage"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *storageSizer = new wxStaticBoxSizer(storageBox, wxHORIZONTAL);
  compressBox->Add(storageSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxCheckBox *coerce2DCtrl = new wxCheckBox(this, ID_LDSHP_COERCE_2D,
                                            wxT("Coerce 2D geometries [x,y]"),
                                            wxDefaultPosition, wxDefaultSize);
  coerce2DCtrl->SetValue(Coerce2D);
  storageSizer->Add(coerce2DCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxCheckBox *compressCtrl = new wxCheckBox(this, ID_LDSHP_COMPRESSED,
                                            wxT("Apply geometry compression"),
                                            wxDefaultPosition, wxDefaultSize);
  compressCtrl->SetValue(Compressed);
  storageSizer->Add(compressCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxCheckBox *rtreeCtrl = new wxCheckBox(this, ID_LDSHP_RTREE,
                                         wxT("With Spatial Index (R*Tree)"),
                                         wxDefaultPosition, wxDefaultSize);
  rtreeCtrl->SetValue(SpatialIndex);
  storageSizer->Add(rtreeCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxStaticBox *userGTypeBox = new wxStaticBox(this, wxID_STATIC,
                                              wxT("Geometry Type"),
                                              wxDefaultPosition,
                                              wxDefaultSize);
// User Defined Geometry Type 
  wxBoxSizer *userdefBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(userdefBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *gTypeSizer = new wxStaticBoxSizer(userGTypeBox, wxVERTICAL);
  userdefBox->Add(gTypeSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxString gtype_selection[2];
  gtype_selection[0] = wxT("Automatic");
  gtype_selection[1] = wxT("User specified");
  wxRadioBox *gtyp_sel = new wxRadioBox(this, ID_LDSHP_USER_GTYPE,
                                        wxT("&Mode"),
                                        wxDefaultPosition,
                                        wxDefaultSize, 2,
                                        gtype_selection, 2,
                                        wxRA_SPECIFY_COLS);
  gtyp_sel->SetSelection(0);
  gTypeSizer->Add(gtyp_sel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxComboBox *typeList =
    new wxComboBox(this, ID_LDSHP_GTYPE, wxT(""), wxDefaultPosition,
                   wxSize(180, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  typeList->Enable(false);
  gTypeSizer->Add(typeList, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// User Defined Primary Key 
  wxStaticBox *userPKeyBox = new wxStaticBox(this, wxID_STATIC,
                                             wxT("Primary Key Column"),
                                             wxDefaultPosition,
                                             wxDefaultSize);
  wxBoxSizer *gPKeySizer = new wxStaticBoxSizer(userPKeyBox, wxVERTICAL);
  userdefBox->Add(gPKeySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxString pkey_selection[2];
  pkey_selection[0] = wxT("Automatic");
  pkey_selection[1] = wxT("User specified");
  wxRadioBox *gpkey_sel = new wxRadioBox(this, ID_LDSHP_USER_PKEY,
                                         wxT("&Mode"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         pkey_selection, 2,
                                         wxRA_SPECIFY_COLS);
  gpkey_sel->SetSelection(0);
  gPKeySizer->Add(gpkey_sel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxComboBox *pkeyList =
    new wxComboBox(this, ID_LDSHP_PKCOL, wxT(""), wxDefaultPosition,
                   wxSize(180, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  pkeyList->Enable(false);
  gPKeySizer->Add(pkeyList, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & LoadShpDialog::OnOk);
  Connect(ID_LDSHP_USER_GTYPE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadShpDialog::OnUserGType);
  Connect(ID_LDSHP_USER_PKEY, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & LoadShpDialog::OnUserPKey);
}

void LoadShpDialog::LoadPKFields()
{
// loading the Primary Key Fields list
  char dbf_path[1024];
  char charset[1024];
  char dummy[1024];
  wxString dummyName;
  int cnt;
  int seed;
  int dup;
  int idup;
  wxString *charsets = MainFrame->GetCharsets();
  wxString cs;
  int idSel = MainFrame->GetCharsetIndex(Default);
  gaiaDbfPtr dbf = NULL;
  gaiaDbfFieldPtr dbf_field;
  if (PKFields != NULL)
    delete[]PKFields;
  PKFields = NULL;
  PKCount = 0;

// opening the DBF in order to fetch field names 
  dbf = gaiaAllocDbf();
  strcpy(dbf_path, Path.ToUTF8());
  strcat(dbf_path, ".dbf");
  cs = *(charsets + idSel);
  strcpy(charset, cs.ToUTF8());
  gaiaOpenDbfRead(dbf, dbf_path, charset, "UTF-8");
  if (!(dbf->Valid))
    {
      gaiaFreeDbf(dbf);
      return;
    }
/* checking for duplicate / illegal column names and antialising them */
  dbf_field = dbf->Dbf->First;
  while (dbf_field)
    {
      /* counting DBF fields */
      PKCount++;
      dbf_field = dbf_field->Next;
    }
  PKFields = new wxString[PKCount];
  PKFieldsEx = new wxString[PKCount];
  cnt = 0;
  seed = 0;
  dbf_field = dbf->Dbf->First;
  while (dbf_field)
    {
      /* preparing column names */
      dummyName = wxString::FromUTF8(dbf_field->Name);
      dup = 0;
      for (idup = 0; idup < cnt; idup++)
        {
          if (dummyName == PKFields[idup])
            dup = 1;
        }
      if (dummyName == wxString::FromUTF8("PK_UID"))
        dup = 1;
      if (dup)
        {
          sprintf(dummy, "COL_%d", seed++);
          dummyName = wxString::FromUTF8(dummy);
        }
      PKFields[cnt] = dummyName;
      PKFieldsEx[cnt] = dummyName;
      dummyName = wxT(" [TEXT]");
      switch (dbf_field->Type)
        {
          case 'N':
            if (dbf_field->Decimals)
              dummyName = wxT(" [DOUBLE]");
            else
              {
                if (dbf_field->Length <= 18)
                  dummyName = wxT(" [INTEGER]");
                else
                  dummyName = wxT(" [DOUBLE]");
              }
            break;
          case 'D':
            dummyName = wxT(" [DOUBLE]");
            break;
          case 'F':
            dummyName = wxT(" [DOUBLE]");
            break;
          case 'L':
            dummyName = wxT(" [INTEGER]");
            break;
        };
      PKFieldsEx[cnt] += dummyName;
      cnt++;
      dbf_field = dbf_field->Next;
    }

// memory cleanup
  gaiaFreeDbf(dbf);
}

void LoadShpDialog::OnUserGType(wxCommandEvent & WXUNUSED(event))
{
//
// the mode of user defined Geometry Type changed
//
  wxRadioBox *radioCtrl = (wxRadioBox *) FindWindow(ID_LDSHP_USER_GTYPE);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_LDSHP_GTYPE);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      comboCtrl->Clear();
      comboCtrl->Enable(false);
  } else
    {
      // user specified
      wxString str = wxT("LINESTRING");
      comboCtrl->Append(str);
      str = wxT("LINESTRINGZ");
      comboCtrl->Append(str);
      str = wxT("LINESTRINGM");
      comboCtrl->Append(str);
      str = wxT("LINESTRINGZM");
      comboCtrl->Append(str);
      str = wxT("MULTILINESTRING");
      comboCtrl->Append(str);
      str = wxT("MULTILINESTRINGZ");
      comboCtrl->Append(str);
      str = wxT("MULTILINESTRINGM");
      comboCtrl->Append(str);
      str = wxT("MULTILINESTRINGZM");
      comboCtrl->Append(str);
      str = wxT("POLYGON");
      comboCtrl->Append(str);
      str = wxT("POLYGONZ");
      comboCtrl->Append(str);
      str = wxT("POLYGONM");
      comboCtrl->Append(str);
      str = wxT("POLYGONZM");
      comboCtrl->Append(str);
      str = wxT("MULTIPOLYGON");
      comboCtrl->Append(str);
      str = wxT("MULTIPOLYGONZ");
      comboCtrl->Append(str);
      str = wxT("MULTIPOLYGONM");
      comboCtrl->Append(str);
      str = wxT("MULTIPOLYGONZM");
      comboCtrl->Append(str);
      comboCtrl->Enable(true);
      comboCtrl->SetSelection(wxNOT_FOUND);
    }
}

void LoadShpDialog::OnUserPKey(wxCommandEvent & WXUNUSED(event))
{
//
// the mode of user defined Primary Key Column changed
//
  wxRadioBox *radioCtrl = (wxRadioBox *) FindWindow(ID_LDSHP_USER_PKEY);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_LDSHP_PKCOL);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      comboCtrl->Clear();
      comboCtrl->Enable(false);
  } else
    {
      // user specified
      int i;
      for (i = 0; i < PKCount; i++)
        {
          // setting PK Fields
          wxString str = PKFieldsEx[i];
          comboCtrl->Append(str);
        }
      comboCtrl->Enable(true);
      comboCtrl->SetSelection(wxNOT_FOUND);
    }
}

void LoadShpDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_LDSHP_TABLE);
  Table = tableCtrl->GetValue();
  if (Table.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (MainFrame->TableAlreadyExists(Table) == true)
    {
      wxMessageBox(wxT("a table name '") + Table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxTextCtrl *columnCtrl = (wxTextCtrl *) FindWindow(ID_LDSHP_COLUMN);
  Column = columnCtrl->GetValue();
  if (Column.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the GEOMETRY COLUMN NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxSpinCtrl *sridCtrl = (wxSpinCtrl *) FindWindow(ID_LDSHP_SRID);
  Srid = sridCtrl->GetValue();
  if (MainFrame->SridNotExists(Srid) == true)
    {
      wxMessageBox(wxT("invalid SRID value"), wxT("spatialite_gui"),
                   wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_LDSHP_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxCheckBox *coerce2DCtrl = (wxCheckBox *) FindWindow(ID_LDSHP_COERCE_2D);
  Coerce2D = coerce2DCtrl->GetValue();
  wxCheckBox *compressCtrl = (wxCheckBox *) FindWindow(ID_LDSHP_COMPRESSED);
  Compressed = compressCtrl->GetValue();
  wxCheckBox *rtreeCtrl = (wxCheckBox *) FindWindow(ID_LDSHP_RTREE);
  SpatialIndex = rtreeCtrl->GetValue();
  wxRadioBox *radioCtrl = (wxRadioBox *) FindWindow(ID_LDSHP_USER_GTYPE);
  wxComboBox *comboCtrl = (wxComboBox *) FindWindow(ID_LDSHP_GTYPE);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      UserDefinedGType = false;
      GeometryType = wxT("");
  } else
    {
      // user specified
      idSel = comboCtrl->GetSelection();
      if (idSel == wxNOT_FOUND)
        {
          wxMessageBox(wxT
                       ("you must select some User defined Geometry Type from the list"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      UserDefinedGType = true;
      GeometryType = comboCtrl->GetValue();
    }
  radioCtrl = (wxRadioBox *) FindWindow(ID_LDSHP_USER_PKEY);
  comboCtrl = (wxComboBox *) FindWindow(ID_LDSHP_PKCOL);
  if (radioCtrl->GetSelection() == 0)
    {
      // automatic
      UserDefinedPKey = false;
      PKColumn = wxT("");
  } else
    {
      // user specified
      idSel = comboCtrl->GetSelection();
      if (idSel == wxNOT_FOUND)
        {
          wxMessageBox(wxT
                       ("you must select some User defined Primary Key Column from the list"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      UserDefinedPKey = true;
      PKColumn = PKFields[idSel];
    }
  wxDialog::EndModal(wxID_OK);
}

bool DumpShpDialog::Create(MyFrame * parent, wxString & path, wxString & table,
                           wxString & column, wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Table = table;
  Column = column;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Dump Shapefile")) == false)
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

void DumpShpDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the Shapefile path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_STATIC, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableCtrl->Enable(false);
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&GeomColumn name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_STATIC, Column,
                                       wxDefaultPosition, wxSize(350, 22));
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// fourth row: CHARSET
  wxBoxSizer *csSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(csSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  csSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_DMPSHP_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DumpShpDialog::OnOk);
}

void DumpShpDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_DMPSHP_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxDialog::EndModal(wxID_OK);
}

bool DumpTxtDialog::Create(MyFrame * parent, wxString & path, wxString & target,
                           wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Dump ") + target) == false)
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

void DumpTxtDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the Shapefile path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: CHARSET
  wxBoxSizer *csSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(csSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  csSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_DMPTXT_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DumpTxtDialog::OnOk);
}

void DumpTxtDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_DMPTXT_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxDialog::EndModal(wxID_OK);
}

bool DumpKmlDialog::Create(MyFrame * parent, wxString & table,
                           wxString & column)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Column = column;
  isNameConst = true;
  isDescConst = true;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Dump KML")) == false)
    return false;
  ColumnList.Populate(MainFrame->GetSqlite(), Table);
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  Centre();
  return true;
}

void DumpKmlDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_STATIC, Table,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableCtrl->Enable(false);
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&GeomColumn name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_STATIC, Column,
                                       wxDefaultPosition, wxSize(350, 22));
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);

// precision
  wxBoxSizer *precBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(precBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *precBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Precision"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *precSizer = new wxStaticBoxSizer(precBox, wxVERTICAL);
  precBoxSizer->Add(precSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  PrecisionCtrl = new wxSpinCtrl(this, ID_KML_PRECISION, wxEmptyString,
                                 wxDefaultPosition, wxSize(80, 20),
                                 wxSP_ARROW_KEYS, 1, 18, 8);
  precSizer->Add(PrecisionCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);

// the NAME pane
  wxBoxSizer *nameSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(nameSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *nameBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Name"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *name2Sizer = new wxStaticBoxSizer(nameBox, wxHORIZONTAL);
  boxSizer->Add(name2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticBox *nameColBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("from Column"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *nameColSizer = new wxStaticBoxSizer(nameColBox, wxVERTICAL);
  name2Sizer->Add(nameColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  NameCtrl =
    new wxComboBox(this, ID_KML_NAME, wxT(""), wxDefaultPosition,
                   wxSize(150, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  InitializeComboColumns(NameCtrl);
  nameColSizer->Add(NameCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  wxStaticBox *nameConstBox = new wxStaticBox(this, wxID_STATIC,
                                              wxT("Constant value"),
                                              wxDefaultPosition,
                                              wxDefaultSize);
  wxBoxSizer *nameConstSizer = new wxStaticBoxSizer(nameConstBox, wxVERTICAL);
  name2Sizer->Add(nameConstSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  NameConstCtrl =
    new wxTextCtrl(this, ID_KML_NAME_K, wxT(""),
                   wxDefaultPosition, wxSize(250, 22));
  nameConstSizer->Add(NameConstCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// the DESCRIPTION pane
  wxBoxSizer *descSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(descSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *descBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Description"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *desc2Sizer = new wxStaticBoxSizer(descBox, wxHORIZONTAL);
  boxSizer->Add(desc2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 2);
  wxStaticBox *descColBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("from Column"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *descColSizer = new wxStaticBoxSizer(descColBox, wxVERTICAL);
  desc2Sizer->Add(descColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  DescCtrl =
    new wxComboBox(this, ID_KML_DESC, wxT(""), wxDefaultPosition,
                   wxSize(150, 21), 0, NULL, wxCB_DROPDOWN | wxCB_READONLY);
  InitializeComboColumns(DescCtrl);
  descColSizer->Add(DescCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

  wxStaticBox *descConstBox = new wxStaticBox(this, wxID_STATIC,
                                              wxT("Constant value"),
                                              wxDefaultPosition,
                                              wxDefaultSize);
  wxBoxSizer *descConstSizer = new wxStaticBoxSizer(descConstBox, wxVERTICAL);
  desc2Sizer->Add(descConstSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  DescConstCtrl =
    new wxTextCtrl(this, ID_KML_DESC_K, wxT(""),
                   wxDefaultPosition, wxSize(250, 22));
  descConstSizer->Add(DescConstCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);

// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DumpKmlDialog::OnOk);
  Connect(ID_KML_NAME, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & DumpKmlDialog::OnNameSelected);
  Connect(ID_KML_DESC, wxEVT_COMMAND_COMBOBOX_SELECTED,
          (wxObjectEventFunction) & DumpKmlDialog::OnDescSelected);
}

void DumpKmlDialog::InitializeComboColumns(wxComboBox * ctrl)
{
//
// initializing a column list ComboBox
//
  ctrl->Clear();
  AuxColumn *pColumn;
  wxString col = wxT("none, use Constant");
  ctrl->Append(col);
  pColumn = ColumnList.GetFirst();
  while (pColumn)
    {
      if (pColumn->GetName() != Column)
        {
          col = pColumn->GetName();
          ctrl->Append(col);
        }
      pColumn = pColumn->GetNext();
    }
  ctrl->Select(0);
}

void DumpKmlDialog::OnNameSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Name-column selection changed
//
  if (NameCtrl->GetSelection() == 0)
    {
      // using a Constant value
      isNameConst = true;
      NameConstCtrl->Enable(true);
      return;
    }
  isNameConst = false;
  NameConstCtrl->Enable(false);
  NameConstCtrl->SetValue(wxT(""));
}

void DumpKmlDialog::OnDescSelected(wxCommandEvent & WXUNUSED(event))
{
//
// Desc-column selection changed
//
  if (DescCtrl->GetSelection() == 0)
    {
      // using a Constant value
      isDescConst = true;
      DescConstCtrl->Enable(true);
      return;
    }
  isDescConst = false;
  DescConstCtrl->Enable(false);
  DescConstCtrl->SetValue(wxT(""));
}

void DumpKmlDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  Name = wxT("");
  Desc = wxT("");
  Precision = PrecisionCtrl->GetValue();
  if (isNameConst == true)
    Name = NameConstCtrl->GetValue();
  else
    Name = NameCtrl->GetValue();
  if (Name.Len() == 0)
    {
      wxMessageBox(wxT("you must specify a NAME (Column or Constant)"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (isDescConst == true)
    Desc = DescConstCtrl->GetValue();
  else
    Desc = DescCtrl->GetValue();
  if (Desc.Len() == 0)
    {
      wxMessageBox(wxT("you must specify a DESCRIPTION (Column or Constant)"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxDialog::EndModal(wxID_OK);
}

bool DumpSpreadsheetDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  DecimalPoint = '.';
  DateTimes = true;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Dump Spreadsheet")) == false)
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

void DumpSpreadsheetDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: Decimal Point
  wxBoxSizer *row = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(row, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *pt = new wxBoxSizer(wxHORIZONTAL);
  row->Add(pt, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxString decimal_points[2];
  decimal_points[0] = wxT("POINT [.]");
  decimal_points[1] = wxT("COMMA [,]");
  wxRadioBox *Point = new wxRadioBox(this, ID_SHEET_DECIMAL_POINT,
                                     wxT("&Decimal Point"),
                                     wxDefaultPosition,
                                     wxDefaultSize, 2,
                                     decimal_points, 2,
                                     wxRA_SPECIFY_ROWS);
  Point->SetSelection(0);
  pt->Add(Point, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// second row: Dates and Times
  wxBoxSizer *dt = new wxBoxSizer(wxHORIZONTAL);
  row->Add(dt, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxString dates[2];
  dates[0] = wxT("Don't care");
  dates[1] = wxT("Convert to SpreadSheet format");
  wxRadioBox *Dates = new wxRadioBox(this, ID_SHEET_DATE_TIME,
                                     wxT("&Dates, Data-Times and Times"),
                                     wxDefaultPosition,
                                     wxDefaultSize, 2,
                                     dates, 2,
                                     wxRA_SPECIFY_ROWS);
  Dates->SetSelection(1);
  dt->Add(Dates, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DumpSpreadsheetDialog::OnOk);
  Connect(ID_SHEET_DECIMAL_POINT, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) &
          DumpSpreadsheetDialog::OnDecimalPointSelected);
  Connect(ID_SHEET_DATE_TIME, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & DumpSpreadsheetDialog::OnDateTimesSelected);
}

void DumpSpreadsheetDialog::
OnDecimalPointSelected(wxCommandEvent & WXUNUSED(event))
{
//
// DECIMAL POINT radio box
//
  wxRadioBox *Point = (wxRadioBox *) FindWindow(ID_SHEET_DECIMAL_POINT);
  if (DecimalPoint == '.')
    {
      DecimalPoint = ',';
      Point->SetSelection(1);
  } else
    {
      DecimalPoint = '.';
      Point->SetSelection(0);
    }
}

void DumpSpreadsheetDialog::
OnDateTimesSelected(wxCommandEvent & WXUNUSED(event))
{
//
// DATE/TIMES radio box
//
  wxRadioBox *Dates = (wxRadioBox *) FindWindow(ID_SHEET_DATE_TIME);
  if (DateTimes == true)
    {
      DateTimes = false;
      Dates->SetSelection(0);
  } else
    {
      DateTimes = true;
      Dates->SetSelection(1);
    }
}

void DumpSpreadsheetDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool SqlScriptDialog::Create(MyFrame * parent, wxString & path,
                             wxString & defCs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Default = defCs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("SQL script execute")) == false)
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

void SqlScriptDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the SQL script path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, wxID_STATIC, wxT("&Path:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pathValue = new wxTextCtrl(this, wxID_STATIC,
                                         Path, wxDefaultPosition,
                                         wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(pathValue, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: CHARSET
  wxBoxSizer *csSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(csSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Charset Encoding"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxHORIZONTAL);
  csSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_SCRIPT_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Default);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SqlScriptDialog::OnOk);
}

void SqlScriptDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_SCRIPT_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT("you must select some Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxDialog::EndModal(wxID_OK);
}

bool DefaultCharsetDialog::Create(MyFrame * parent, wxString & charset,
                                  bool ask)
{
//
// creating the dialog
//
  MainFrame = parent;
  Charset = charset;
  AskCharset = ask;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Charset settings")) == false)
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

void DefaultCharsetDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the DEFAULT CHARSET
  wxBoxSizer *csSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(csSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *charsetBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Default Output Charset"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
  wxBoxSizer *charsetSizer = new wxStaticBoxSizer(charsetBox, wxVERTICAL);
  csSizer->Add(charsetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxListBox *charsetCtrl = new wxListBox(this, ID_DFLT_CHARSET,
                                         wxDefaultPosition, wxDefaultSize,
                                         MainFrame->GetCharsetsLen(),
                                         MainFrame->GetCharsetsNames(),
                                         wxLB_SINGLE | wxLB_HSCROLL);
  charsetCtrl->SetFont(wxFont
                       (8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_NORMAL));
  int idSel = MainFrame->GetCharsetIndex(Charset);
  if (idSel != wxNOT_FOUND)
    charsetCtrl->SetSelection(idSel);
  charsetSizer->Add(charsetCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// second row: the ASK CHARSET
  wxBoxSizer *askSizer = new wxBoxSizer(wxHORIZONTAL);
  charsetSizer->Add(askSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *askCtrl = new wxCheckBox(this, ID_DFLT_ASK,
                                       wxT("Ask output charset every time"),
                                       wxDefaultPosition, wxDefaultSize);
  askCtrl->SetValue(AskCharset);
  askSizer->Add(askCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DefaultCharsetDialog::OnOk);
}

void DefaultCharsetDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxListBox *charsetCtrl = (wxListBox *) FindWindow(ID_DFLT_CHARSET);
  int idSel = charsetCtrl->GetSelection();
  if (idSel == wxNOT_FOUND)
    {
      wxMessageBox(wxT
                   ("you must select some Default Charset Encoding from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxString *charsets = MainFrame->GetCharsets();
  Charset = *(charsets + idSel);
  wxCheckBox *askCtrl = (wxCheckBox *) FindWindow(ID_DFLT_ASK);
  AskCharset = askCtrl->GetValue();
  wxDialog::EndModal(wxID_OK);
}

bool RecoverDialog::Create(MyFrame * parent, wxString & table,
                           wxString & column)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Column = column;
  Srid = 0;
  Dimension = wxT("XY");
  if (wxDialog::Create(parent, wxID_ANY, wxT("Recover Geometry Column")) ==
      false)
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

void RecoverDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_ANY, Table,
                                         wxDefaultPosition, wxSize(350, 22),
                                         wxTE_READONLY);
  tableCtrl->Enable(false);
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Column name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_ANY, Column,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: SRID
  wxBoxSizer *triSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(triSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *sridDimsSizer = new wxBoxSizer(wxVERTICAL);
  triSizer->Add(sridDimsSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  sridDimsSizer->Add(sridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *sridCtrl = new wxSpinCtrl(this, ID_RCVR_SRID, wxEmptyString,
                                        wxDefaultPosition, wxSize(80, 20),
                                        wxSP_ARROW_KEYS,
                                        -1, 1000000, Srid);
  sridSizer->Add(sridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxStaticBox *dimsBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Dims"),
                                         wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *dimsSizer = new wxStaticBoxSizer(dimsBox, wxHORIZONTAL);
  sridDimsSizer->Add(dimsSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxString dims[4];
  dims[0] = wxT("XY");
  dims[1] = wxT("XYM");
  dims[2] = wxT("XYZ");
  dims[3] = wxT("XYZM");
  wxListBox *coordDims = new wxListBox(this, ID_RCVR_DIMS,
                                       wxDefaultPosition, wxDefaultSize, 4,
                                       dims, wxLB_SINGLE | wxLB_HSCROLL);
  dimsSizer->Add(coordDims, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// fourth row: GEOMETRY TYPE
  wxStaticBox *typeBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Geometry Type"),
                                         wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *typeSizer = new wxStaticBoxSizer(typeBox, wxHORIZONTAL);
  triSizer->Add(typeSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxString types[8];
  types[0] = wxT("POINT");
  types[1] = wxT("MULTIPOINT");
  types[2] = wxT("LINESTRING");
  types[3] = wxT("MULTILINESTRING");
  types[4] = wxT("POLYGON");
  types[5] = wxT("MULTIPOLYGON");
  types[6] = wxT("GEOMETRYCOLLECTION");
  types[7] = wxT("GEOMETRY");
  wxListBox *geomType = new wxListBox(this, ID_RCVR_TYPE,
                                      wxDefaultPosition, wxDefaultSize, 8,
                                      types, wxLB_SINGLE | wxLB_HSCROLL);
  typeSizer->Add(geomType, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & RecoverDialog::OnOk);
}

void RecoverDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxSpinCtrl *sridCtrl = (wxSpinCtrl *) FindWindow(ID_RCVR_SRID);
  wxListBox *geomType = (wxListBox *) FindWindow(ID_RCVR_TYPE);
  wxListBox *coordDims = (wxListBox *) FindWindow(ID_RCVR_DIMS);
  Srid = sridCtrl->GetValue();
  if (MainFrame->SridNotExists(Srid) == true)
    {
      wxMessageBox(wxT("invalid SRID value"), wxT("spatialite_gui"),
                   wxOK | wxICON_WARNING, this);
      return;
    }
  Type = geomType->GetStringSelection();
  if (Type.Len() < 1)
    {
      wxMessageBox(wxT("you must select some Geometry Type from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  Dimension = coordDims->GetStringSelection();
  if (Dimension.Len() < 1)
    {
      wxMessageBox(wxT("you must select some Coord Dimension from the list"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxDialog::EndModal(wxID_OK);
}

bool ElementaryGeomsDialog::Create(MyFrame * parent, wxString & table,
                                   wxString & geometry)
{
//
// creating the dialog
//
  MainFrame = parent;
  InTable = table;
  Geometry = geometry;
  MainFrame->ElementaryGeoms(InTable, Geometry, OutTable, PrimaryKey, MultiID,
                             Type, &Srid, CoordDims, &SpatialIndex);
  if (wxDialog::Create(parent, wxID_ANY, wxT("Elementary Geometries")) == false)
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

void ElementaryGeomsDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: input TABLE name
  wxStaticBox *inputBox = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Input Table"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *inputSizer = new wxStaticBoxSizer(inputBox, wxVERTICAL);
  boxSizer->Add(inputSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *inTblSizer = new wxBoxSizer(wxHORIZONTAL);
  inputSizer->Add(inTblSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *inTblLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  inTblSizer->Add(inTblLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *inTblCtrl = new wxTextCtrl(this, wxID_ANY, InTable,
                                         wxDefaultPosition, wxSize(350, 22),
                                         wxTE_READONLY);
  inTblCtrl->Enable(false);
  inTblSizer->Add(inTblCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  inputSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Geometry name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_ANY, Geometry,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: output TABLE name
  wxStaticBox *outputBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Output Table"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *outputSizer = new wxStaticBoxSizer(outputBox, wxVERTICAL);
  boxSizer->Add(outputSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *outTblSizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(outTblSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *outTblLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  outTblSizer->Add(outTblLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *outTblCtrl = new wxTextCtrl(this, ID_ELEMGEOM_TABLE, OutTable,
                                          wxDefaultPosition, wxSize(350,
                                                                    22));
  outTblSizer->Add(outTblCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// fourth row: output Primary Key
  wxBoxSizer *pKeySizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(pKeySizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pKeyLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Primary Key name:"));
  pKeySizer->Add(pKeyLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *pKeyCtrl = new wxTextCtrl(this, ID_ELEMGEOM_PKEY, PrimaryKey,
                                        wxDefaultPosition, wxSize(350,
                                                                  22));
  pKeySizer->Add(pKeyCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// fifth row: MULTI ID
  wxBoxSizer *multiSizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(multiSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *multiLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&MultiID name:"));
  multiSizer->Add(multiLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *multiCtrl = new wxTextCtrl(this, ID_ELEMGEOM_MULTI_ID, MultiID,
                                         wxDefaultPosition, wxSize(350, 22));
  multiSizer->Add(multiCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// sixth row: GEOMETRY TYPE
  wxBoxSizer *typeSizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(typeSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tpLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Geometry Type:"));
  typeSizer->Add(tpLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *typeCtrl = new wxTextCtrl(this, ID_ELEMGEOM_TYPE, Type,
                                        wxDefaultPosition, wxSize(350, 22),
                                        wxTE_READONLY);
  typeCtrl->Enable(false);
  typeSizer->Add(typeCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// seventh row: SRID
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(sridSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  char xdummy[64];
  sprintf(xdummy, "%d", Srid);
  wxString dummy = wxString::FromUTF8(xdummy);
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *sridCtrl = new wxTextCtrl(this, wxID_ANY, dummy,
                                        wxDefaultPosition, wxSize(350, 22),
                                        wxTE_READONLY);
  sridCtrl->Enable(false);
  sridSizer->Add(sridCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// heigth row: COORD DIMENSION
  wxBoxSizer *dimSizer = new wxBoxSizer(wxHORIZONTAL);
  outputSizer->Add(dimSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *dimLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Coord Dimension:"));
  dimSizer->Add(dimLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *dimCtrl = new wxTextCtrl(this, wxID_ANY, CoordDims,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  dimCtrl->Enable(false);
  dimSizer->Add(dimCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// nineth row: Spatial Index
  wxCheckBox *rtreeCtrl = new wxCheckBox(this, ID_ELEMGEOM_RTREE,
                                         wxT("With Spatial Index (R*Tree)"),
                                         wxDefaultPosition, wxDefaultSize);
  rtreeCtrl->SetValue(SpatialIndex);
  outputSizer->Add(rtreeCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & ElementaryGeomsDialog::OnOk);
}

void ElementaryGeomsDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *textCtrl = (wxTextCtrl *) FindWindow(ID_ELEMGEOM_TABLE);
  OutTable = textCtrl->GetValue();
  textCtrl = (wxTextCtrl *) FindWindow(ID_ELEMGEOM_PKEY);
  PrimaryKey = textCtrl->GetValue();
  textCtrl = (wxTextCtrl *) FindWindow(ID_ELEMGEOM_MULTI_ID);
  MultiID = textCtrl->GetValue();
  wxCheckBox *boxCtrl = (wxCheckBox *) FindWindow(ID_ELEMGEOM_RTREE);
  if (boxCtrl->IsChecked())
    SpatialIndex = true;
  else
    SpatialIndex = false;
  wxDialog::EndModal(wxID_OK);
}

bool SetSridDialog::Create(MyFrame * parent, wxString & table,
                           wxString & column)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Column = column;
  OldSrid = 0;
  Srid = 0;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Changing SRID")) == false)
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

void SetSridDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, wxID_ANY, Table,
                                         wxDefaultPosition, wxSize(350, 22),
                                         wxTE_READONLY);
  tableCtrl->Enable(false);
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: GEOMETRY COLUMN name
  wxBoxSizer *colSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(colSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *colLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Column name:"));
  colSizer->Add(colLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *colCtrl = new wxTextCtrl(this, wxID_ANY, Column,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  colCtrl->Enable(false);
  colSizer->Add(colCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: SRID
  wxBoxSizer *mySizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(mySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *oldSridSizer = new wxBoxSizer(wxHORIZONTAL);
  mySizer->Add(oldSridSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticText *oldSridLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&old SRID:"));
  oldSridSizer->Add(oldSridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *oldSridCtrl = new wxSpinCtrl(this, ID_SRID_OLD, wxEmptyString,
                                           wxDefaultPosition, wxSize(80,
                                                                     20),
                                           wxSP_ARROW_KEYS,
                                           -1, 1000000, OldSrid);
  oldSridSizer->Add(oldSridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// fourth row: SRID
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  mySizer->Add(sridSizer, 0, wxALIGN_RIGHT | wxLEFT, 20);
  wxStaticText *sridLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&new SRID:"));
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *sridCtrl = new wxSpinCtrl(this, ID_SRID_SRID, wxEmptyString,
                                        wxDefaultPosition, wxSize(80, 20),
                                        wxSP_ARROW_KEYS,
                                        -1, 1000000, Srid);
  sridSizer->Add(sridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SetSridDialog::OnOk);
}

void SetSridDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxSpinCtrl *oldSridCtrl = (wxSpinCtrl *) FindWindow(ID_SRID_OLD);
  wxSpinCtrl *sridCtrl = (wxSpinCtrl *) FindWindow(ID_SRID_SRID);
  OldSrid = oldSridCtrl->GetValue();
  Srid = sridCtrl->GetValue();
  wxDialog::EndModal(wxID_OK);
}

bool SearchSridDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  SearchBySrid = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Searching a SRID by its name")) ==
      false)
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

void SearchSridDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// First row: SEARCH string
  wxBoxSizer *searchSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(searchSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *searchLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Search:"));
  searchSizer->Add(searchLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *searchCtrl = new wxTextCtrl(this, ID_SEARCH, wxT(""),
                                          wxDefaultPosition, wxSize(150,
                                                                    22));
  searchSizer->Add(searchCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// Second row: by SRID / by Name
  wxBoxSizer *bySridSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(bySridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString opts[2];
  opts[0] = wxT("by EPSG &SRID code");
  opts[1] = wxT("by &name");
  wxRadioBox *bySridSel = new wxRadioBox(this, ID_BY_SRID,
                                         wxT("&Search mode"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         opts, 2,
                                         wxRA_SPECIFY_ROWS);
  bySridSel->SetSelection(1);
  bySridSizer->Add(bySridSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & SearchSridDialog::OnOk);
  Connect(ID_BY_SRID, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & SearchSridDialog::OnSearchBySrid);
}

void SearchSridDialog::OnSearchBySrid(wxCommandEvent & WXUNUSED(event))
{
//
// BY SRID radio box
//
  wxRadioBox *bySridSel = (wxRadioBox *) FindWindow(ID_BY_SRID);
  if (SearchBySrid == true)
    {
      SearchBySrid = false;
      bySridSel->SetSelection(1);
  } else
    {
      SearchBySrid = true;
      bySridSel->SetSelection(0);
    }
}

void SearchSridDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *searchCtrl = (wxTextCtrl *) FindWindow(ID_SEARCH);
  wxString txt = searchCtrl->GetValue();
  if (SearchBySrid == true)
    {
      // searching by SRID
      long srid;
      if (txt.ToLong(&srid) == false)
        {
          wxMessageBox(wxT("You must specify a numeric code to search for !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      Srid = srid;
  } else
    {
      // searching by NAME
      String = txt;
      if (String.Len() < 1)
        {
          wxMessageBox(wxT("You must specify a string to search for !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  wxDialog::EndModal(wxID_OK);
}

bool HelpDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  if (wxDialog::Create(parent, wxID_ANY, wxT("SQLite + SpatiaLite help"),
                       wxDefaultPosition, wxDefaultSize,
                       wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) == false)
    return false;
// populates individual controls
  CreateControls();
// sets dialog sizer
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
// centers the dialog window
  MainFrame->OpenHelpPane();
  Centre();
// setting up an event handler [dialog closing]
  Connect(wxID_ANY, wxEVT_CLOSE_WINDOW,
          (wxObjectEventFunction) & HelpDialog::OnClose);
  return true;
}

void HelpDialog::OnClose(wxCloseEvent & WXUNUSED(event))
{
//
// this window has been closed
//
  MainFrame->CloseHelpPane();
  Destroy();
}

void HelpDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxHtmlWindow *helpWin = new wxHtmlWindow(this, ID_HELP_HTML,
                                           wxDefaultPosition, wxSize(400,
                                                                     200));
  wxString html;
  MainFrame->GetHelp(html);
  helpWin->SetPage(html);
  topSizer->Add(helpWin, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// setting up the event handlers
  Connect(wxID_ANY, wxEVT_SIZE, (wxObjectEventFunction) & HelpDialog::OnSize);
}

void HelpDialog::OnSize(wxSizeEvent & WXUNUSED(event))
{
//
// this window has changed its size
//
  wxSize sz = GetClientSize();
  wxHtmlWindow *helpWin = (wxHtmlWindow *) FindWindow(ID_HELP_HTML);
  helpWin->SetSize(sz.GetWidth() - 6, sz.GetHeight() - 6);
}

bool NetworkDialog::Create(MyFrame * parent)
{
//
// creating the dialog
//
  MainFrame = parent;
  FromColumn = wxT("");
  ToColumn = wxT("");
  GeomColumn = wxT("");
  GeomLength = true;
  CostColumn = wxT("");
  Bidirectional = true;
  OneWays = false;
  NameEnabled = false;
  AStarSupported = true;
  OneWayFromTo = wxT("");
  OneWayToFrom = wxT("");
  NameColumn = wxT("");
  if (wxDialog::Create(parent, wxID_ANY, wxT("Build Network")) == false)
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

void NetworkDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *row0Sizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(row0Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// the Main TABLE
  wxBoxSizer *tableSizer = new wxBoxSizer(wxHORIZONTAL);
  row0Sizer->Add(tableSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 0);
  wxStaticBox *tableBox = new wxStaticBox(this, wxID_STATIC,
                                          wxT("Base Table [graph]"),
                                          wxDefaultPosition,
                                          wxDefaultSize);
  wxBoxSizer *tableNameSizer = new wxStaticBoxSizer(tableBox, wxVERTICAL);
  tableSizer->Add(tableNameSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  int nTables;
  wxString *tables = MainFrame->GetTables(&nTables);
  wxSize size = wxSize(150, 300);
  wxListBox *tableCtrl = new wxListBox();
  tableNameSizer->Add(tableCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  tableCtrl->Create(this, ID_NET_TABLE, wxDefaultPosition, size, nTables,
                    tables, wxLB_SINGLE | wxLB_HSCROLL);
  size = wxSize(100, 80);
// the NodeFrom COLUMN
  wxBoxSizer *netSizer = new wxBoxSizer(wxHORIZONTAL);
  row0Sizer->Add(netSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *netBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Network configuration"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *colSizer = new wxStaticBoxSizer(netBox, wxVERTICAL);
  netSizer->Add(colSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxBoxSizer *row1Sizer = new wxBoxSizer(wxHORIZONTAL);
  colSizer->Add(row1Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxBoxSizer *fromSizer = new wxBoxSizer(wxHORIZONTAL);
  row1Sizer->Add(fromSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *fromBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("NodeFrom Column"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *fromColSizer = new wxStaticBoxSizer(fromBox, wxVERTICAL);
  fromSizer->Add(fromColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *fromCtrl = new wxListBox(this, ID_NET_FROM,
                                      wxDefaultPosition, size,
                                      0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  fromCtrl->Enable(false);
  fromColSizer->Add(fromCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// the NodeTo COLUMN
  wxBoxSizer *toSizer = new wxBoxSizer(wxHORIZONTAL);
  row1Sizer->Add(toSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *toBox = new wxStaticBox(this, wxID_STATIC,
                                       wxT("NodeTo Column"),
                                       wxDefaultPosition,
                                       wxDefaultSize);
  wxBoxSizer *toColSizer = new wxStaticBoxSizer(toBox, wxVERTICAL);
  toSizer->Add(toColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *toCtrl = new wxListBox(this, ID_NET_TO,
                                    wxDefaultPosition, size,
                                    0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  toCtrl->Enable(false);
  toColSizer->Add(toCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// the Geometry COLUMN
  wxBoxSizer *geoSizer = new wxBoxSizer(wxHORIZONTAL);
  row1Sizer->Add(geoSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *geoBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Geometry Column"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *geoColSizer = new wxStaticBoxSizer(geoBox, wxVERTICAL);
  toSizer->Add(geoColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *geoCtrl = new wxListBox(this, ID_NET_GEOM,
                                     wxDefaultPosition, size,
                                     0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  geoCtrl->Enable(false);
  geoColSizer->Add(geoCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// UNIDIRECTIONAL / BIDIRECTIONAL
  wxBoxSizer *row2Sizer = new wxBoxSizer(wxHORIZONTAL);
  colSizer->Add(row2Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *row22Sizer = new wxBoxSizer(wxVERTICAL);
  row2Sizer->Add(row22Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *row222Sizer = new wxBoxSizer(wxHORIZONTAL);
  row22Sizer->Add(row222Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString dirs[2];
  dirs[0] = wxT("&Uni-Directional");
  dirs[1] = wxT("&Bi-Directional");
  wxRadioBox *dirSel = new wxRadioBox(this, ID_NET_BIDIR,
                                      wxT("&Arc connections"),
                                      wxDefaultPosition,
                                      wxDefaultSize, 2,
                                      dirs, 2,
                                      wxRA_SPECIFY_ROWS);
  dirSel->SetSelection(1);
  dirSel->Enable(false);
  row222Sizer->Add(dirSel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// the COST is Lenth selection
  wxString costs[2];
  costs[0] = wxT("&Using Length as Cost");
  costs[1] = wxT("&Using Cost Column");
  wxRadioBox *costSel = new wxRadioBox(this, ID_NET_LENGTH,
                                       wxT("&Cost type"),
                                       wxDefaultPosition,
                                       wxDefaultSize, 2,
                                       costs, 2,
                                       wxRA_SPECIFY_ROWS);
  costSel->SetSelection(0);
  costSel->Enable(false);
  row222Sizer->Add(costSel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 1);
// enabling A* support
  wxBoxSizer *row223Sizer = new wxBoxSizer(wxHORIZONTAL);
  row22Sizer->Add(row223Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxString algorithms[2];
  algorithms[0] = wxT("&A* + Dijkstra");
  algorithms[1] = wxT("&Dijkstra only");
  wxRadioBox *aStarSel = new wxRadioBox(this, ID_NET_A_STAR,
                                        wxT
                                        ("&Supported Shortest Path Algorithms"),
                                        wxDefaultPosition,
                                        wxDefaultSize, 2,
                                        algorithms, 2,
                                        wxRA_SPECIFY_COLS);
  aStarSel->SetSelection(0);
  aStarSel->Enable(false);
  row223Sizer->Add(aStarSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// the COST COLUMN    
  wxBoxSizer *costSizer = new wxBoxSizer(wxHORIZONTAL);
  row2Sizer->Add(costSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *costBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Cost Column"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *costColSizer = new wxStaticBoxSizer(costBox, wxVERTICAL);
  costSizer->Add(costColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *costCtrl = new wxListBox(this, ID_NET_COST,
                                      wxDefaultPosition, size,
                                      0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  costCtrl->Enable(false);
  costColSizer->Add(costCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// One Ways
  wxBoxSizer *row3Sizer = new wxBoxSizer(wxHORIZONTAL);
  colSizer->Add(row3Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxBoxSizer *oneWaySizer = new wxBoxSizer(wxHORIZONTAL);
  row3Sizer->Add(oneWaySizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticBox *oneWayBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("OneWay Columns"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *colWaySizer = new wxStaticBoxSizer(oneWayBox, wxVERTICAL);
  oneWaySizer->Add(colWaySizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
// OneWay Enable
  wxCheckBox *oneWayEnable =
    new wxCheckBox(this, ID_NET_ONEWAY, wxT("Enable &OneWays"));
  colWaySizer->Add(oneWayEnable, 0, wxALIGN_LEFT | wxALL, 5);
// the OneWay FromTo COLUMN
  wxBoxSizer *fromToSizer = new wxBoxSizer(wxHORIZONTAL);
  colWaySizer->Add(fromToSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticBox *fromToBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("From -> To"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *fromToColSizer = new wxStaticBoxSizer(fromToBox, wxVERTICAL);
  fromToSizer->Add(fromToColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
  wxListBox *fromToCtrl = new wxListBox(this, ID_NET_FROM_TO,
                                        wxDefaultPosition, size,
                                        0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  fromToCtrl->Enable(false);
  fromToColSizer->Add(fromToCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// the OneWay To From COLUMN
  wxStaticBox *toFromBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("To -> From"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *toFromColSizer = new wxStaticBoxSizer(toFromBox, wxVERTICAL);
  fromToSizer->Add(toFromColSizer, 0, wxALIGN_RIGHT | wxALL, 1);
  wxListBox *toFromCtrl = new wxListBox(this, ID_NET_TO_FROM,
                                        wxDefaultPosition, size,
                                        0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  toFromCtrl->Enable(false);
  toFromColSizer->Add(toFromCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// Name
  wxBoxSizer *nameSizer = new wxBoxSizer(wxHORIZONTAL);
  row3Sizer->Add(nameSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticBox *nameBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Name Column"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *nameColSizer = new wxStaticBoxSizer(nameBox, wxVERTICAL);
  nameSizer->Add(nameColSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 1);
// Name Enable
  wxCheckBox *nameEnable =
    new wxCheckBox(this, ID_NET_NAME_ENABLE, wxT("Enable &Name"));
  nameColSizer->Add(nameEnable, 0, wxALIGN_LEFT | wxALL, 5);
// the Name COLUMN
  wxListBox *nameCtrl = new wxListBox(this, ID_NET_NAME,
                                      wxDefaultPosition, size,
                                      0, NULL, wxLB_SINGLE | wxLB_HSCROLL);
  nameCtrl->Enable(false);
  nameColSizer->Add(nameCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & NetworkDialog::OnOk);
// appends event handlers for radio buttons etc
  Connect(ID_NET_TABLE, wxEVT_COMMAND_LISTBOX_SELECTED,
          (wxObjectEventFunction) & NetworkDialog::OnTable);
  Connect(ID_NET_BIDIR, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & NetworkDialog::OnDirection);
  Connect(ID_NET_LENGTH, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & NetworkDialog::OnCost);
  Connect(ID_NET_ONEWAY, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & NetworkDialog::OnOneWay);
  Connect(ID_NET_NAME_ENABLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & NetworkDialog::OnNameEnabled);
}

void NetworkDialog::OnTable(wxCommandEvent & WXUNUSED(event))
{
//
// TABLE selection changed
//
  wxListBox *tableCtrl = (wxListBox *) FindWindow(ID_NET_TABLE);
  TableName = tableCtrl->GetStringSelection();
  int n_cols;
  wxString *columns = MainFrame->GetColumnNames(TableName, &n_cols);
  wxListBox *fromCtrl = (wxListBox *) FindWindow(ID_NET_FROM);
  fromCtrl->Clear();
  fromCtrl->InsertItems(n_cols, columns, 0);
  fromCtrl->Enable(true);
  wxListBox *toCtrl = (wxListBox *) FindWindow(ID_NET_TO);
  toCtrl->Clear();
  toCtrl->InsertItems(n_cols, columns, 0);
  toCtrl->Enable(true);
  wxListBox *geomCtrl = (wxListBox *) FindWindow(ID_NET_GEOM);
  geomCtrl->Clear();
  geomCtrl->InsertItems(n_cols, columns, 0);
  geomCtrl->Enable(true);
  wxListBox *costCtrl = (wxListBox *) FindWindow(ID_NET_COST);
  costCtrl->Clear();
  costCtrl->InsertItems(n_cols, columns, 0);
  costCtrl->Enable(false);
  wxListBox *fromToCtrl = (wxListBox *) FindWindow(ID_NET_FROM_TO);
  fromToCtrl->Clear();
  fromToCtrl->InsertItems(n_cols, columns, 0);
  fromToCtrl->Enable(false);
  wxListBox *toFromCtrl = (wxListBox *) FindWindow(ID_NET_TO_FROM);
  toFromCtrl->Clear();
  toFromCtrl->InsertItems(n_cols, columns, 0);
  toFromCtrl->Enable(false);
  wxRadioBox *dirSel = (wxRadioBox *) FindWindow(ID_NET_BIDIR);
  Bidirectional = true;
  dirSel->SetSelection(1);
  dirSel->Enable(true);
  wxRadioBox *costSel = (wxRadioBox *) FindWindow(ID_NET_LENGTH);
  GeomLength = true;
  costSel->SetSelection(0);
  costSel->Enable(true);
  wxCheckBox *oneWaySel = (wxCheckBox *) FindWindow(ID_NET_ONEWAY);
  OneWays = false;
  oneWaySel->SetValue(false);
  oneWaySel->Enable(true);
  wxListBox *nameCtrl = (wxListBox *) FindWindow(ID_NET_NAME);
  nameCtrl->Clear();
  nameCtrl->InsertItems(n_cols, columns, 0);
  nameCtrl->Enable(false);
  wxCheckBox *nameEnable = (wxCheckBox *) FindWindow(ID_NET_NAME_ENABLE);
  NameEnabled = false;
  nameEnable->SetValue(false);
  nameEnable->Enable(true);
  wxRadioBox *aStarSel = (wxRadioBox *) FindWindow(ID_NET_A_STAR);
  aStarSel->Enable(true);
}

void NetworkDialog::OnDirection(wxCommandEvent & WXUNUSED(event))
{
//
// BIDIRECTIONAL radio box
//
  wxCheckBox *oneWaySel = (wxCheckBox *) FindWindow(ID_NET_ONEWAY);
  wxRadioBox *dirSel = (wxRadioBox *) FindWindow(ID_NET_BIDIR);
  wxListBox *fromToCtrl = (wxListBox *) FindWindow(ID_NET_FROM_TO);
  wxListBox *toFromCtrl = (wxListBox *) FindWindow(ID_NET_TO_FROM);
  if (Bidirectional == true)
    {
      Bidirectional = false;
      dirSel->SetSelection(0);
      OneWays = false;
      oneWaySel->SetValue(false);
      oneWaySel->Enable(false);
      fromToCtrl->Enable(false);
      toFromCtrl->Enable(false);
  } else
    {
      Bidirectional = true;
      dirSel->SetSelection(1);
      OneWays = false;
      oneWaySel->SetValue(false);
      oneWaySel->Enable(true);
      fromToCtrl->Enable(false);
      toFromCtrl->Enable(false);
    }
}

void NetworkDialog::OnCost(wxCommandEvent & WXUNUSED(event))
{
//
// COST radio box
//
  wxRadioBox *costSel = (wxRadioBox *) FindWindow(ID_NET_LENGTH);
  wxListBox *costCtrl = (wxListBox *) FindWindow(ID_NET_COST);
  if (GeomLength == true)
    {
      GeomLength = false;
      costSel->SetSelection(1);
      costCtrl->Enable(true);
  } else
    {
      GeomLength = true;
      costSel->SetSelection(0);
      costCtrl->Enable(false);
    }
}

void NetworkDialog::OnOneWay(wxCommandEvent & WXUNUSED(event))
{
//
// OneWay check box
//
  wxCheckBox *oneWaySel = (wxCheckBox *) FindWindow(ID_NET_ONEWAY);
  wxListBox *fromToCtrl = (wxListBox *) FindWindow(ID_NET_FROM_TO);
  wxListBox *toFromCtrl = (wxListBox *) FindWindow(ID_NET_TO_FROM);
  if (OneWays == true)
    {
      OneWays = false;
      oneWaySel->SetValue(false);
      fromToCtrl->Enable(false);
      toFromCtrl->Enable(false);
  } else
    {
      OneWays = true;
      oneWaySel->SetValue(true);
      fromToCtrl->Enable(true);
      toFromCtrl->Enable(true);
    }
}

void NetworkDialog::OnNameEnabled(wxCommandEvent & WXUNUSED(event))
{
//
// Name check box
//
  wxCheckBox *nameEnable = (wxCheckBox *) FindWindow(ID_NET_NAME_ENABLE);
  wxListBox *nameCtrl = (wxListBox *) FindWindow(ID_NET_NAME);
  if (NameEnabled == true)
    {
      NameEnabled = false;
      nameEnable->SetValue(false);
      nameCtrl->Enable(false);
  } else
    {
      NameEnabled = true;
      nameEnable->SetValue(true);
      nameCtrl->Enable(true);
    }
}

void NetworkDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxListBox *tableCtrl = (wxListBox *) FindWindow(ID_NET_TABLE);
  TableName = tableCtrl->GetStringSelection();
  if (TableName.Len() < 1)
    {
      wxMessageBox(wxT("You must select some TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *fromCtrl = (wxListBox *) FindWindow(ID_NET_FROM);
  FromColumn = fromCtrl->GetStringSelection();
  if (FromColumn.Len() < 1)
    {
      wxMessageBox(wxT("You must select some 'NodeFrom' COLUMN !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *toCtrl = (wxListBox *) FindWindow(ID_NET_TO);
  ToColumn = toCtrl->GetStringSelection();
  if (ToColumn.Len() < 1)
    {
      wxMessageBox(wxT("You must select some 'NodeTo' COLUMN !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxListBox *geomCtrl = (wxListBox *) FindWindow(ID_NET_GEOM);
  GeomColumn = geomCtrl->GetStringSelection();
  if (GeomColumn.Len() < 1)
    {
      wxMessageBox(wxT("You must select some 'Geometry' COLUMN !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (GeomLength == true)
    CostColumn = wxT("");
  else
    {
      wxListBox *costCtrl = (wxListBox *) FindWindow(ID_NET_COST);
      CostColumn = costCtrl->GetStringSelection();
      if (CostColumn.Len() < 1)
        {
          wxMessageBox(wxT("You must select some 'Cost' COLUMN !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  if (Bidirectional == true && OneWays == true)
    {
      wxListBox *fromToCtrl = (wxListBox *) FindWindow(ID_NET_FROM_TO);
      OneWayFromTo = fromToCtrl->GetStringSelection();
      if (OneWayFromTo.Len() < 1)
        {
          wxMessageBox(wxT("You must select some 'OneWay From->To' COLUMN !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
      wxListBox *toFromCtrl = (wxListBox *) FindWindow(ID_NET_TO_FROM);
      OneWayToFrom = toFromCtrl->GetStringSelection();
      if (OneWayToFrom.Len() < 1)
        {
          wxMessageBox(wxT("You must select some 'OneWay To->From' COLUMN !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    {
      OneWayFromTo = wxT("");
      OneWayToFrom = wxT("");
    }
  if (NameEnabled == true)
    {
      wxListBox *nameCtrl = (wxListBox *) FindWindow(ID_NET_NAME);
      NameColumn = nameCtrl->GetStringSelection();
      if (NameColumn.Len() < 1)
        {
          wxMessageBox(wxT("You must select some 'Name' COLUMN !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    NameColumn = wxT("");
  wxRadioBox *aStarSel = (wxRadioBox *) FindWindow(ID_NET_A_STAR);
  if (aStarSel->GetSelection() == 1)
    AStarSupported = false;
  else
    AStarSupported = true;
  wxDialog::EndModal(wxID_OK);
}

bool ExifDialog::Create(MyFrame * parent, wxString & dir_path,
                        wxString & img_path)
{
//
// creating the dialog
//
  MainFrame = parent;
  DirPath = dir_path;
  ImgPath = img_path;
  Folder = false;
  Metadata = true;
  GpsOnly = false;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Import EXIF Photos")) == false)
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

void ExifDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *row0Sizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(row0Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the path and file/folder selection
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  row0Sizer->Add(pathSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 0);
  wxStaticBox *pathBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Import from source"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *pthSizer = new wxStaticBoxSizer(pathBox, wxVERTICAL);
  pathSizer->Add(pthSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxStaticText *pathLabel = new wxStaticText(this, ID_EXIF_PATH, ImgPath);
  pthSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxString imgFolder[2];
  imgFolder[0] = wxT("Import &selected image only");
  imgFolder[1] = wxT("Import &any EXIF from selected folder");
  wxRadioBox *imgFolderSel = new wxRadioBox(this, ID_EXIF_FOLDER,
                                            wxT("&Source selection"),
                                            wxDefaultPosition,
                                            wxDefaultSize, 2,
                                            imgFolder, 2,
                                            wxRA_SPECIFY_ROWS);
  imgFolderSel->SetSelection(0);
  pthSizer->Add(imgFolderSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// second row: the metadata and gps-only selection
  wxBoxSizer *modeSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(modeSizer, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 0);
  wxString metadata[2];
  metadata[0] = wxT("Feed full EXIF &Metadata");
  metadata[1] = wxT("&Skip EXIF Metadata");
  wxRadioBox *metadataSel = new wxRadioBox(this, ID_EXIF_METADATA,
                                           wxT("&EXIF Metadata tables"),
                                           wxDefaultPosition,
                                           wxDefaultSize, 2,
                                           metadata, 2,
                                           wxRA_SPECIFY_ROWS);
  metadataSel->SetSelection(0);
  modeSizer->Add(metadataSel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxString gpsOnly[2];
  gpsOnly[0] = wxT("Import any EXIF file");
  gpsOnly[1] = wxT("Import EXIF only if containing &GPS tags");
  wxRadioBox *gpsOnlySel = new wxRadioBox(this, ID_EXIF_GPS_ONLY,
                                          wxT("&GPS position"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 2,
                                          gpsOnly, 2,
                                          wxRA_SPECIFY_ROWS);
  gpsOnlySel->SetSelection(0);
  modeSizer->Add(gpsOnlySel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & ExifDialog::OnOk);
// appends event handlers for radio buttons etc
  Connect(ID_EXIF_FOLDER, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ExifDialog::OnFolder);
  Connect(ID_EXIF_METADATA, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ExifDialog::OnMetadata);
  Connect(ID_EXIF_GPS_ONLY, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & ExifDialog::OnGpsOnly);
}

void ExifDialog::OnFolder(wxCommandEvent & WXUNUSED(event))
{
//
// File/Folder radio box
//
  wxRadioBox *folderSel = (wxRadioBox *) FindWindow(ID_EXIF_FOLDER);
  wxStaticText *pathLabel = (wxStaticText *) FindWindow(ID_EXIF_PATH);
  if (Folder == true)
    {
      Folder = false;
      folderSel->SetSelection(0);
      pathLabel->SetLabel(ImgPath);
  } else
    {
      Folder = true;
      folderSel->SetSelection(1);
      pathLabel->SetLabel(DirPath);
    }
}

void ExifDialog::OnMetadata(wxCommandEvent & WXUNUSED(event))
{
//
// Metadata radio box
//
  wxRadioBox *metadataSel = (wxRadioBox *) FindWindow(ID_EXIF_METADATA);
  if (Metadata == true)
    {
      Metadata = false;
      metadataSel->SetSelection(1);
  } else
    {
      Metadata = true;
      metadataSel->SetSelection(0);
    }
}

void ExifDialog::OnGpsOnly(wxCommandEvent & WXUNUSED(event))
{
//
// GpsOnly radio box
//
  wxRadioBox *gpsOnlySel = (wxRadioBox *) FindWindow(ID_EXIF_GPS_ONLY);
  if (GpsOnly == true)
    {
      GpsOnly = false;
      gpsOnlySel->SetSelection(0);
  } else
    {
      GpsOnly = true;
      gpsOnlySel->SetSelection(1);
    }
}

void ExifDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool XmlDocumentsDialog::Create(MyFrame * parent, wxString & dir_path,
                                wxString & xml_path)
{
//
// creating the dialog
//
  MainFrame = parent;
  DirPath = dir_path;
  XmlPath = xml_path;
  Folder = false;
  Compressed = true;
  Validated = false;
  InternalSchema = false;
  OkSuffix = false;
  OkSchemaColumn = true;
  OkInPathColumn = true;
  OkParseErrorColumn = true;
  OkValidateErrorColumn = true;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Import XML Documents")) == false)
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

void XmlDocumentsDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *row0Sizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(row0Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// first row: the path and file/folder selection
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  row0Sizer->Add(pathSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 0);
  wxStaticBox *pathBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Import from source"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *pthSizer = new wxStaticBoxSizer(pathBox, wxVERTICAL);
  pathSizer->Add(pthSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, ID_XML_PATH, XmlPath);
  pthSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxString xmlFolder[2];
  xmlFolder[0] = wxT("Import &selected XML Document only");
  xmlFolder[1] = wxT("Import &any XML Document from selected folder");
  wxRadioBox *xmlFolderSel = new wxRadioBox(this, ID_XML_FOLDER,
                                            wxT("&Source selection"),
                                            wxDefaultPosition,
                                            wxDefaultSize, 2,
                                            xmlFolder, 2,
                                            wxRA_SPECIFY_ROWS);
  xmlFolderSel->SetSelection(0);
  pthSizer->Add(xmlFolderSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *sufSizer = new wxBoxSizer(wxHORIZONTAL);
  pthSizer->Add(sufSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *okSufCtrl = new wxCheckBox(this, ID_XML_OK_SUFFIX,
                                         wxT(""),
                                         wxDefaultPosition, wxDefaultSize);
  sufSizer->Add(okSufCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okSufCtrl->SetValue(false);
  okSufCtrl->Enable(false);
  wxStaticText *sufLabel =
    new wxStaticText(this, wxID_STATIC, wxT("File &suffix:"));
  sufSizer->Add(sufLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *SufValue = new wxTextCtrl(this, ID_XML_SUFFIX,
                                        wxT(""), wxDefaultPosition,
                                        wxSize(50, 22));
  SufValue->Enable(false);
  sufSizer->Add(SufValue, 0, wxALIGN_RIGHT | wxALL, 0);
// second row: Compressed XmlBLOB
  wxBoxSizer *comprSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(comprSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxCheckBox *compressedCtrl = new wxCheckBox(this, ID_XML_COMPRESSED,
                                              wxT
                                              ("Compressed XML Documents [DEFLATE-zip]"),
                                              wxDefaultPosition, wxDefaultSize);
  compressedCtrl->SetValue(true);
  comprSizer->Add(compressedCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// third row: Schema Validation
  wxStaticBox *schemaBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Schema Validation"),
                                           wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *validateSizer = new wxStaticBoxSizer(schemaBox, wxVERTICAL);
  boxSizer->Add(validateSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *valSizer = new wxBoxSizer(wxHORIZONTAL);
  validateSizer->Add(valSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxCheckBox *validateCtrl = new wxCheckBox(this, ID_XML_VALIDATED,
                                            wxT("Apply Schema Validation"),
                                            wxDefaultPosition, wxDefaultSize);
  validateCtrl->SetValue(false);
  valSizer->Add(validateCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxString internalSchema[2];
  internalSchema[0] = wxT("Use &External SchemaURI");
  internalSchema[1] = wxT("Use &Internal SchemaURI");
  wxRadioBox *internalBox = new wxRadioBox(this, ID_XML_INTERNAL_SCHEMA,
                                           wxT("&SchemaURI mode"),
                                           wxDefaultPosition,
                                           wxDefaultSize, 2,
                                           internalSchema, 2,
                                           wxRA_SPECIFY_ROWS);
  valSizer->Add(internalBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  internalBox->SetSelection(0);
  internalBox->Enable(false);
  wxBoxSizer *schemaUriSizer = new wxBoxSizer(wxHORIZONTAL);
  validateSizer->Add(schemaUriSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticText *schemaLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Schema URI:"));
  schemaUriSizer->Add(schemaLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *schemaUriValue = new wxTextCtrl(this, ID_XML_SCHEMA_URI,
                                              wxT(""), wxDefaultPosition,
                                              wxSize(350, 22));
  schemaUriValue->Enable(false);
  schemaUriSizer->Add(schemaUriValue, 0, wxALIGN_RIGHT | wxALL, 0);
// fourth row: target Table / Column
  wxStaticBox *targetBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Destination: Table/Columns"),
                                           wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *targetSizer = new wxStaticBoxSizer(targetBox, wxVERTICAL);
  boxSizer->Add(targetSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *tblSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(tblSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tblLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Table name:"));
  tblSizer->Add(tblLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *tableValue = new wxTextCtrl(this, ID_XML_TARGET_TABLE,
                                          wxT(""), wxDefaultPosition,
                                          wxSize(250, 22));
  tblSizer->Add(tableValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *pkSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(pkSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pkLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Primary Key Column:"));
  pkSizer->Add(pkLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *pkColValue = new wxTextCtrl(this, ID_XML_PK_NAME,
                                          wxT("pk_uid"), wxDefaultPosition,
                                          wxSize(250, 22));
  pkSizer->Add(pkColValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *xmlSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(xmlSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *xmlLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&XMLDocument Column:"));
  xmlSizer->Add(xmlLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *xmlColValue = new wxTextCtrl(this, ID_XML_BLOB_COLUMN,
                                           wxT("xml_document"),
                                           wxDefaultPosition,
                                           wxSize(250, 22));
  xmlSizer->Add(xmlColValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *skSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(skSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxCheckBox *okSkCtrl = new wxCheckBox(this, ID_XML_OK_SCHEMA_URI,
                                        wxT(""),
                                        wxDefaultPosition, wxDefaultSize);
  skSizer->Add(okSkCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okSkCtrl->SetValue(true);
  wxStaticText *skLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&SchemaURI Column:"));
  skSizer->Add(skLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *SchemaColValue = new wxTextCtrl(this, ID_XML_SCHEMA_URI_COLUMN,
                                              wxT("schema_uri"),
                                              wxDefaultPosition,
                                              wxSize(250, 22));
  skSizer->Add(SchemaColValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *inPthSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(inPthSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxCheckBox *okPthCtrl = new wxCheckBox(this, ID_XML_OK_PATH,
                                         wxT(""),
                                         wxDefaultPosition, wxDefaultSize);
  inPthSizer->Add(okPthCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okPthCtrl->SetValue(true);
  wxStaticText *pthLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&InputPath Column:"));
  inPthSizer->Add(pthLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *PathColValue = new wxTextCtrl(this, ID_XML_PATH_COLUMN,
                                            wxT("file_name"), wxDefaultPosition,
                                            wxSize(250, 22));
  inPthSizer->Add(PathColValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *parseSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(parseSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxCheckBox *okParseCtrl = new wxCheckBox(this, ID_XML_OK_PARSE_ERR,
                                           wxT(""),
                                           wxDefaultPosition, wxDefaultSize);
  parseSizer->Add(okParseCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okParseCtrl->SetValue(true);
  wxStaticText *parseLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Parse Errors Column:"));
  parseSizer->Add(parseLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *ParseColValue = new wxTextCtrl(this, ID_XML_PARSE_ERR_COLUMN,
                                             wxT("parse_errors"),
                                             wxDefaultPosition,
                                             wxSize(250, 22));
  parseSizer->Add(ParseColValue, 0, wxALIGN_RIGHT | wxALL, 0);
  wxBoxSizer *validSizer = new wxBoxSizer(wxHORIZONTAL);
  targetSizer->Add(validSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxCheckBox *okValidateCtrl = new wxCheckBox(this, ID_XML_OK_VALIDATE_ERR,
                                              wxT(""),
                                              wxDefaultPosition, wxDefaultSize);
  validSizer->Add(okValidateCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okValidateCtrl->SetValue(true);
  wxStaticText *validateLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Validate Errors Column:"));
  validSizer->Add(validateLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxTextCtrl *ValidateColValue =
    new wxTextCtrl(this, ID_XML_VALIDATE_ERR_COLUMN,
                   wxT("validate_errors"), wxDefaultPosition,
                   wxSize(250, 22));
  validSizer->Add(ValidateColValue, 0, wxALIGN_RIGHT | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnOk);
// appends event handlers for radio buttons etc
  Connect(ID_XML_FOLDER, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnFolder);
  Connect(ID_XML_COMPRESSED, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnCompressionChanged);
  Connect(ID_XML_VALIDATED, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnValidationChanged);
  Connect(ID_XML_INTERNAL_SCHEMA, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) &
          XmlDocumentsDialog::OnInternalSchemaChanged);
  Connect(ID_XML_OK_SUFFIX, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnSuffixChanged);
  Connect(ID_XML_OK_SCHEMA_URI, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnSchemaColumnChanged);
  Connect(ID_XML_OK_PATH, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & XmlDocumentsDialog::OnInPathColumnChanged);
  Connect(ID_XML_OK_PARSE_ERR, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) &
          XmlDocumentsDialog::OnParseErrorColumnChanged);
  Connect(ID_XML_OK_VALIDATE_ERR, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) &
          XmlDocumentsDialog::OnValidateErrorColumnChanged);
}

void XmlDocumentsDialog::OnFolder(wxCommandEvent & WXUNUSED(event))
{
//
// File/Folder radio box
//
  wxRadioBox *folderSel = (wxRadioBox *) FindWindow(ID_XML_FOLDER);
  wxStaticText *pathLabel = (wxStaticText *) FindWindow(ID_XML_PATH);
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_SUFFIX);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_SUFFIX);
  if (Folder == true)
    {
      Folder = false;
      folderSel->SetSelection(0);
      pathLabel->SetLabel(XmlPath);
      OkSuffix = false;
      okCtrl->SetValue(false);
      okCtrl->Enable(false);
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
  } else
    {
      Folder = true;
      folderSel->SetSelection(1);
      pathLabel->SetLabel(DirPath);
      OkSuffix = true;
      okCtrl->SetValue(true);
      okCtrl->Enable(true);
      wxString val = wxT(".xml");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
    }
}

void XmlDocumentsDialog::OnCompressionChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Compressed (on/off): 
//
  wxCheckBox *compressedCtrl = (wxCheckBox *) FindWindow(ID_XML_COMPRESSED);
  if (compressedCtrl->IsChecked() == true)
    Compressed = true;
  else
    Compressed = false;
}

void XmlDocumentsDialog::OnValidationChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Schema Validation (on/off): 
//
  wxCheckBox *validateCtrl = (wxCheckBox *) FindWindow(ID_XML_VALIDATED);
  wxTextCtrl *schemaUriCtrl = (wxTextCtrl *) FindWindow(ID_XML_SCHEMA_URI);
  wxRadioBox *internalCtrl = (wxRadioBox *) FindWindow(ID_XML_INTERNAL_SCHEMA);
  if (validateCtrl->IsChecked())
    {
      schemaUriCtrl->Enable(true);
      internalCtrl->Enable(true);
      internalCtrl->SetSelection(0);
  } else
    {
      SchemaURI = wxT("");
      schemaUriCtrl->SetValue(SchemaURI);
      schemaUriCtrl->Enable(false);
      internalCtrl->Enable(false);
      internalCtrl->SetSelection(0);
    }
}

void XmlDocumentsDialog::
OnInternalSchemaChanged(wxCommandEvent & WXUNUSED(event))
{
//
// InternalSchemaURI radio box
//
  wxRadioBox *internalCtrl = (wxRadioBox *) FindWindow(ID_XML_INTERNAL_SCHEMA);
  wxTextCtrl *schemaUriCtrl = (wxTextCtrl *) FindWindow(ID_XML_SCHEMA_URI);
  if (internalCtrl->GetSelection() == 0)
    {
      InternalSchema = false;
      SchemaURI = wxT("");
      schemaUriCtrl->SetValue(SchemaURI);
      schemaUriCtrl->Enable(true);
  } else
    {
      InternalSchema = true;
      SchemaURI = wxT("");
      schemaUriCtrl->SetValue(SchemaURI);
      schemaUriCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::OnSuffixChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Suffix (on/off): 
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_SUFFIX);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_SUFFIX);
  if (okCtrl->IsChecked() == true)
    {
      OkSuffix = true;
      wxString val = wxT(".xml");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
  } else
    {
      OkSuffix = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::OnSchemaColumnChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed Schema Column (on/off): 
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_SCHEMA_URI);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_SCHEMA_URI_COLUMN);
  if (okCtrl->IsChecked() == true)
    {
      OkSchemaColumn = true;
      wxString val = wxT("schema_uri");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
  } else
    {
      OkSchemaColumn = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::OnInPathColumnChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed InPath Column (on/off): 
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_PATH);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_PATH_COLUMN);
  if (okCtrl->IsChecked() == true)
    {
      OkInPathColumn = true;
      wxString val = wxT("file_name");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
  } else
    {
      OkInPathColumn = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::
OnParseErrorColumnChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed ParseError Column (on/off): 
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_PARSE_ERR);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_PARSE_ERR_COLUMN);
  if (okCtrl->IsChecked() == true)
    {
      OkParseErrorColumn = true;
      wxString val = wxT("parse_errors");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
  } else
    {
      OkParseErrorColumn = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::
OnValidateErrorColumnChanged(wxCommandEvent & WXUNUSED(event))
{
//
// changed ValidateError Column (on/off): 
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_XML_OK_VALIDATE_ERR);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_VALIDATE_ERR_COLUMN);
  if (okCtrl->IsChecked() == true)
    {
      OkValidateErrorColumn = true;
      wxString val = wxT("validate_errors");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
  } else
    {
      OkValidateErrorColumn = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
    }
}

void XmlDocumentsDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxCheckBox *validateCtrl = (wxCheckBox *) FindWindow(ID_XML_VALIDATED);
  if (validateCtrl->IsChecked() && InternalSchema == false)
    {
      wxTextCtrl *schemaUriCtrl = (wxTextCtrl *) FindWindow(ID_XML_SCHEMA_URI);
      wxString schemaUri = schemaUriCtrl->GetValue().Trim(false);
      SchemaURI = schemaUri.Trim(true);
      if (SchemaURI.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some Schema URI !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_XML_TARGET_TABLE);
  TargetTable = tableCtrl->GetValue();
  if (TargetTable.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some Table name !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxTextCtrl *pkCtrl = (wxTextCtrl *) FindWindow(ID_XML_PK_NAME);
  PkName = pkCtrl->GetValue();
  if (PkName.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some PrimaryKeyColumn name !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxTextCtrl *columnCtrl = (wxTextCtrl *) FindWindow(ID_XML_BLOB_COLUMN);
  XmlColumn = columnCtrl->GetValue();
  if (XmlColumn.Len() < 1)
    {
      wxMessageBox(wxT("You must specify some XMLColumn name !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  if (Folder == true && OkSuffix == true)
    {
      wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_SUFFIX);
      Suffix = colCtrl->GetValue();
  } else
    Suffix = wxT("");
  if (OkInPathColumn == true)
    {
      wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_PATH_COLUMN);
      InPathColumn = colCtrl->GetValue();
      if (InPathColumn.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some FileNameColumn name !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    InPathColumn = wxT("");
  if (OkSchemaColumn == true)
    {
      wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_SCHEMA_URI_COLUMN);
      SchemaUriColumn = colCtrl->GetValue();
      if (SchemaUriColumn.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some SchemaURIColumn name !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    SchemaUriColumn = wxT("");
  if (OkParseErrorColumn == true)
    {
      wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_XML_PARSE_ERR_COLUMN);
      ParseErrorColumn = colCtrl->GetValue();
      if (ParseErrorColumn.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some ParseErrorColumn name !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    ParseErrorColumn = wxT("");
  if (OkValidateErrorColumn == true)
    {
      wxTextCtrl *colCtrl =
        (wxTextCtrl *) FindWindow(ID_XML_VALIDATE_ERR_COLUMN);
      ValidateErrorColumn = colCtrl->GetValue();
      if (ValidateErrorColumn.Len() < 1)
        {
          wxMessageBox(wxT
                       ("You must specify some ValidateErrorColumn name !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
  } else
    ValidateErrorColumn = wxT("");
  wxDialog::EndModal(wxID_OK);
}

bool AutoSaveDialog::Create(MyFrame * parent, wxString & path, int secs)
{
//
// creating the dialog
//
  MainFrame = parent;
  Path = path;
  Seconds = secs;
  if (wxDialog::Create(parent, wxID_ANY, wxT("MEMORY-DB AutoSave settings")) ==
      false)
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

void AutoSaveDialog::CreateControls()
{
//
// creating individual controls and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: export path
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(pathSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *pathLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Save as:"));
  pathSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxString pth = Path;
  if (pth.Len() == 0)
    pth = wxT("*** not set: AutoSave IS DISABLED ***");
  PathCtrl = new wxTextCtrl(this, ID_AUTO_SAVE_PATH, pth, wxDefaultPosition,
                            wxSize(350, 22), wxTE_READONLY);
  pathSizer->Add(PathCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: CHANGE PATH button
  wxBoxSizer *changeBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(changeBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *change =
    new wxButton(this, ID_AUTO_SAVE_CHANGE_PATH, wxT("&Set the export path"));
  changeBox->Add(change, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// third row: INTERVAL
  wxBoxSizer *modeSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(modeSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString modes[6];
  modes[0] = wxT("&Disable AutoSaving");
  modes[1] = wxT("Every &30 seconds");
  modes[2] = wxT("Every &minute");
  modes[3] = wxT("Every &2 minutes");
  modes[4] = wxT("Every &5 minutes");
  modes[5] = wxT("Every &10 minutes");
  IntervalCtrl = new wxRadioBox(this, ID_AUTO_SAVE_INTERVAL,
                                wxT("&AutoSave interval"),
                                wxDefaultPosition, wxDefaultSize, 6, modes, 6,
                                wxRA_SPECIFY_ROWS);
  if (Seconds <= 0)
    IntervalCtrl->SetSelection(0);
  else if (Seconds <= 30)
    IntervalCtrl->SetSelection(1);
  else if (Seconds <= 60)
    IntervalCtrl->SetSelection(2);
  else if (Seconds <= 120)
    IntervalCtrl->SetSelection(3);
  else if (Seconds <= 300)
    IntervalCtrl->SetSelection(4);
  else
    IntervalCtrl->SetSelection(5);
  modeSizer->Add(IntervalCtrl, 0, wxALIGN_RIGHT | wxALL, 5);

// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & AutoSaveDialog::OnOk);
  Connect(ID_AUTO_SAVE_CHANGE_PATH, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & AutoSaveDialog::OnChangePath);
  Connect(ID_AUTO_SAVE_INTERVAL, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & AutoSaveDialog::OnIntervalChanged);
}

void AutoSaveDialog::OnChangePath(wxCommandEvent & WXUNUSED(event))
{
//
//  exporting the MEMORY-DB into an external DB 
//
  int retdlg;
  wxString lastDir;
  wxFileDialog fileDialog(this, wxT("Saving the MEMORY-DB"), wxT(""),
                          wxT("db.sqlite"),
                          wxT
                          ("SQLite DB (*.sqlite)|*.sqlite|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  retdlg = fileDialog.ShowModal();
  if (retdlg == wxID_OK)
    {
      // exporting the external DB
      wxString pth = fileDialog.GetPath();
      wxString extPth = wxT("*** not set: AutoSave IS DISABLED ***");
      MainFrame->SetExternalSqlitePath(pth);
      if (MainFrame->MemoryDbSave() == true)
        {
          wxMessageBox(wxT("Ok, MEMORY-DB was succesfully saved"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
          wxFileName file(fileDialog.GetPath());
          lastDir = file.GetPath();
          MainFrame->SetLastDirectory(lastDir);
          extPth = MainFrame->GetExternalSqlitePath();
          PathCtrl->SetValue(extPth);
      } else
        PathCtrl->SetValue(extPth);
    }
}

void AutoSaveDialog::OnIntervalChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Interval selection changed
//
  if (IntervalCtrl->GetSelection() == 0)
    Seconds = 0;
  else if (IntervalCtrl->GetSelection() == 1)
    Seconds = 30;
  else if (IntervalCtrl->GetSelection() == 2)
    Seconds = 60;
  else if (IntervalCtrl->GetSelection() == 3)
    Seconds = 120;
  else if (IntervalCtrl->GetSelection() == 4)
    Seconds = 300;
  else
    Seconds = 600;
}

void AutoSaveDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool DumpPostGISDialog::Create(MyFrame * parent, wxString & table)
{
//
// creating the dialog
//
  MainFrame = parent;
  SchemaName = wxT("");
  TableName = table;
  Lowercase = true;
  CreateTable = true;
  SpatialIndex = true;
  if (wxDialog::Create(parent, wxID_ANY, wxT("SQL Dump for PostGIS")) == false)
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

void DumpPostGISDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: SCHEMA name
  wxBoxSizer *schemaSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(schemaSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *schemaLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&PostGIS Schema name:"));
  schemaSizer->Add(schemaLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *schemaCtrl = new wxTextCtrl(this, ID_POSTGIS_SCHEMA, SchemaName,
                                          wxDefaultPosition, wxSize(350,
                                                                    22));
  schemaSizer->Add(schemaCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// second row: TABLE name
  wxBoxSizer *tableSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(tableSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *tableLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&PostGIS Table name:"));
  tableSizer->Add(tableLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxTextCtrl *tableCtrl = new wxTextCtrl(this, ID_POSTGIS_TABLE, TableName,
                                         wxDefaultPosition, wxSize(350,
                                                                   22));
  tableSizer->Add(tableCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
// third row: Lowercase
  wxBoxSizer *lowerSizer = new wxBoxSizer(wxHORIZONTAL);
  tableSizer->Add(lowerSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxCheckBox *lowerCtrl = new wxCheckBox(this, ID_POSTGIS_LOWER,
                                         wxT("Lowercase column names"),
                                         wxDefaultPosition, wxDefaultSize);
  lowerCtrl->SetValue(true);
  lowerSizer->Add(lowerCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// fourth row: Create Table
  wxBoxSizer *createSizer = new wxBoxSizer(wxHORIZONTAL);
  tableSizer->Add(createSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxCheckBox *createCtrl = new wxCheckBox(this, ID_POSTGIS_CREATE,
                                          wxT("Create PostGIS Table"),
                                          wxDefaultPosition, wxDefaultSize);
  createCtrl->SetValue(true);
  createSizer->Add(createCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// fifth row: Spatial Index
  wxBoxSizer *spindexSizer = new wxBoxSizer(wxHORIZONTAL);
  tableSizer->Add(spindexSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxCheckBox *spindexCtrl = new wxCheckBox(this, ID_POSTGIS_SPINDEX,
                                           wxT("Create PostGIS Spatial Index"),
                                           wxDefaultPosition, wxDefaultSize);
  spindexCtrl->SetValue(true);
  spindexSizer->Add(spindexCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(ID_POSTGIS_LOWER, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & DumpPostGISDialog::OnLowercase);
  Connect(ID_POSTGIS_CREATE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & DumpPostGISDialog::OnCreateTable);
  Connect(ID_POSTGIS_SPINDEX, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & DumpPostGISDialog::OnSpatialIndex);
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DumpPostGISDialog::OnOk);
}

void DumpPostGISDialog::OnLowercase(wxCommandEvent & WXUNUSED(event))
{
//
// Lowercase check box
//
  wxCheckBox *lowerCtrl = (wxCheckBox *) FindWindow(ID_POSTGIS_LOWER);
  if (Lowercase == true)
    Lowercase = false;
  else
    Lowercase = true;
  lowerCtrl->SetValue(Lowercase);
}

void DumpPostGISDialog::OnCreateTable(wxCommandEvent & WXUNUSED(event))
{
//
// Create Table check box
//
  wxCheckBox *createCtrl = (wxCheckBox *) FindWindow(ID_POSTGIS_CREATE);
  wxCheckBox *spindexCtrl = (wxCheckBox *) FindWindow(ID_POSTGIS_SPINDEX);
  if (CreateTable == true)
    {
      CreateTable = false;
      SpatialIndex = false;
      spindexCtrl->SetValue(false);
      spindexCtrl->Enable(false);
  } else
    {
      CreateTable = true;
      SpatialIndex = true;
      spindexCtrl->SetValue(true);
      spindexCtrl->Enable(true);
    }
  createCtrl->SetValue(CreateTable);
}

void DumpPostGISDialog::OnSpatialIndex(wxCommandEvent & WXUNUSED(event))
{
//
// Spatial Index check box
//
  wxCheckBox *spindexCtrl = (wxCheckBox *) FindWindow(ID_POSTGIS_SPINDEX);
  if (SpatialIndex == true)
    SpatialIndex = false;
  else
    SpatialIndex = true;
  spindexCtrl->SetValue(SpatialIndex);
}

void DumpPostGISDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxTextCtrl *schemaCtrl = (wxTextCtrl *) FindWindow(ID_POSTGIS_SCHEMA);
  SchemaName = schemaCtrl->GetValue();
  wxTextCtrl *tableCtrl = (wxTextCtrl *) FindWindow(ID_POSTGIS_TABLE);
  TableName = tableCtrl->GetValue();
  if (TableName.Len() < 1)
    {
      wxMessageBox(wxT("You must specify the PostGIS TABLE NAME !!!"),
                   wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
      return;
    }
  wxDialog::EndModal(wxID_OK);
}

bool DxfDialog::Create(MyFrame * parent, wxString & dir_path,
                       wxString & dxf_path)
{
//
// creating the dialog
//
  MainFrame = parent;
  DirPath = dir_path;
  DxfPath = dxf_path;
  Folder = false;
  OkPrefix = false;
  OkSingle = false;
  Force2D = false;
  Force3D = false;
  LinkedRings = false;
  UnlinkedRings = false;
  ImportMixed = false;
  AppendMode = true;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Import DXF Drawings")) == false)
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

void DxfDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxBoxSizer *row0Sizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(row0Sizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
// first row: the path and file/folder selection
  wxBoxSizer *pathSizer = new wxBoxSizer(wxHORIZONTAL);
  row0Sizer->Add(pathSizer, 0, wxALIGN_CENTRE_VERTICAL | wxALL, 0);
  wxStaticBox *pathBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Import from source"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *pthSizer = new wxStaticBoxSizer(pathBox, wxVERTICAL);
  pathSizer->Add(pthSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *pathLabel = new wxStaticText(this, ID_DXF_PATH, DxfPath);
  pthSizer->Add(pathLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxString dxfFolder[2];
  dxfFolder[0] = wxT("Import &selected DXF drawing file only");
  dxfFolder[1] = wxT("Import &any DXF drawing file from selected folder");
  wxRadioBox *dxfFolderSel = new wxRadioBox(this, ID_DXF_FOLDER,
                                            wxT("&Source selection"),
                                            wxDefaultPosition,
                                            wxDefaultSize, 2,
                                            dxfFolder, 2,
                                            wxRA_SPECIFY_ROWS);
  dxfFolderSel->SetSelection(0);
  pthSizer->Add(dxfFolderSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// second row: SRID / Append
  wxBoxSizer *sridSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(sridSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticText *sridLabel = new wxStaticText(this, wxID_STATIC, wxT("&SRID:"));
  sridSizer->Add(sridLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *sridCtrl = new wxSpinCtrl(this, ID_DXF_SRID, wxEmptyString,
                                        wxDefaultPosition, wxSize(80, 20),
                                        wxSP_ARROW_KEYS,
                                        -1, 1000000, -1);
  sridSizer->Add(sridCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxCheckBox *AppendCtrl = new wxCheckBox(this, ID_DXF_APPEND,
                                          wxT
                                          ("Append to already exixing tables"),
                                          wxDefaultPosition, wxDefaultSize);
  sridSizer->Add(AppendCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  AppendCtrl->SetValue(true);
// third row: table-name prefix
  wxStaticBox *prefixBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT("Prefix for DB Table names"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *prefSizer = new wxStaticBoxSizer(prefixBox, wxHORIZONTAL);
  boxSizer->Add(prefSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxCheckBox *okPrefCtrl = new wxCheckBox(this, ID_DXF_OK_PREFIX,
                                          wxT(""),
                                          wxDefaultPosition, wxDefaultSize);
  prefSizer->Add(okPrefCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okPrefCtrl->SetValue(false);
  wxTextCtrl *PrefValue = new wxTextCtrl(this, ID_DXF_PREFIX,
                                         wxT(""), wxDefaultPosition,
                                         wxSize(250, 22));
  PrefValue->Enable(false);
  prefSizer->Add(PrefValue, 0, wxALIGN_RIGHT | wxALL, 0);
// fourth row: single selected DXF layer
  wxStaticBox *singleBox = new wxStaticBox(this, wxID_STATIC,
                                           wxT
                                           ("Select a single DXF layer to be imported"),
                                           wxDefaultPosition,
                                           wxDefaultSize);
  wxBoxSizer *selSizer = new wxStaticBoxSizer(singleBox, wxHORIZONTAL);
  boxSizer->Add(selSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxCheckBox *okSingleCtrl = new wxCheckBox(this, ID_DXF_OK_SINGLE,
                                            wxT(""),
                                            wxDefaultPosition, wxDefaultSize);
  selSizer->Add(okSingleCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 2);
  okSingleCtrl->SetValue(false);
  wxTextCtrl *SingleValue = new wxTextCtrl(this, ID_DXF_SINGLE,
                                           wxT(""), wxDefaultPosition,
                                           wxSize(250, 22));
  SingleValue->Enable(false);
  selSizer->Add(SingleValue, 0, wxALIGN_RIGHT | wxALL, 0);
// fifth row: dimensions
  wxString dxfDims[3];
  dxfDims[0] = wxT("&authomatic 2D/3D");
  dxfDims[1] = wxT("always force 2D");
  dxfDims[2] = wxT("always force 3D");
  wxRadioBox *dxfDimsSel = new wxRadioBox(this, ID_DXF_DIMS,
                                          wxT("&Dimensions"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 3,
                                          dxfDims, 3,
                                          wxRA_SPECIFY_COLS);
  dxfDimsSel->SetSelection(0);
  boxSizer->Add(dxfDimsSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// sixth row: import mode
  wxString dxfMode[3];
  dxfMode[0] = wxT("preserve &distinct DXF layers");
  dxfMode[1] = wxT("&mixed layers (distinct by type)");
  wxRadioBox *dxfModeSel = new wxRadioBox(this, ID_DXF_MIXED,
                                          wxT("&Import mode"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 2,
                                          dxfMode, 2,
                                          wxRA_SPECIFY_COLS);
  dxfModeSel->SetSelection(0);
  boxSizer->Add(dxfModeSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// seventh row: import mode
  wxString dxfRings[3];
  dxfRings[0] = wxT("&none");
  dxfRings[1] = wxT("&linked rings");
  dxfRings[2] = wxT("&unlinked rings");
  wxRadioBox *dxfRingsSel = new wxRadioBox(this, ID_DXF_RINGS,
                                           wxT
                                           ("&Special Rings handling (holes)"),
                                           wxDefaultPosition,
                                           wxDefaultSize, 3,
                                           dxfRings, 3,
                                           wxRA_SPECIFY_COLS);
  dxfRingsSel->SetSelection(0);
  boxSizer->Add(dxfRingsSel, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// OK - CANCEL buttons
  wxBoxSizer *okCancelBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(okCancelBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *ok = new wxButton(this, wxID_OK, wxT("&OK"));
  okCancelBox->Add(ok, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *cancel = new wxButton(this, wxID_CANCEL, wxT("&Cancel"));
  okCancelBox->Add(cancel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & DxfDialog::OnOk);
// appends event handlers for radio buttons etc
  Connect(ID_DXF_FOLDER, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & DxfDialog::OnFolder);
  Connect(ID_DXF_OK_PREFIX, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & DxfDialog::OnPrefixChanged);
  Connect(ID_DXF_OK_SINGLE, wxEVT_COMMAND_CHECKBOX_CLICKED,
          (wxObjectEventFunction) & DxfDialog::OnSingleLayerChanged);
  Connect(ID_DXF_DIMS, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & DxfDialog::OnDimensionChanged);
  Connect(ID_DXF_MIXED, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & DxfDialog::OnModeChanged);
  Connect(ID_DXF_RINGS, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & DxfDialog::OnRingsChanged);
}

void DxfDialog::OnFolder(wxCommandEvent & WXUNUSED(event))
{
//
// File/Folder radio box
//
  wxRadioBox *folderSel = (wxRadioBox *) FindWindow(ID_DXF_FOLDER);
  wxStaticText *pathLabel = (wxStaticText *) FindWindow(ID_DXF_PATH);
  if (Folder == true)
    {
      Folder = false;
      folderSel->SetSelection(0);
      pathLabel->SetLabel(DxfPath);
  } else
    {
      Folder = true;
      folderSel->SetSelection(1);
      pathLabel->SetLabel(DirPath);
    }
}

void DxfDialog::OnDimensionChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Dimensions radio box
//
  wxRadioBox *dimsSel = (wxRadioBox *) FindWindow(ID_DXF_DIMS);
  Force2D = false;
  Force3D = false;
  if (dimsSel->GetSelection() == 1)
    {
      Force2D = true;
      Force3D = false;
    }
  if (dimsSel->GetSelection() == 2)
    {
      Force2D = false;
      Force3D = true;
    }
}

void DxfDialog::OnRingsChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Rings radio box
//
  wxRadioBox *ringsSel = (wxRadioBox *) FindWindow(ID_DXF_RINGS);
  LinkedRings = false;
  UnlinkedRings = false;
  if (ringsSel->GetSelection() == 1)
    {
      LinkedRings = true;
      UnlinkedRings = false;
    }
  if (ringsSel->GetSelection() == 2)
    {
      LinkedRings = false;
      UnlinkedRings = true;
    }
}

void DxfDialog::OnModeChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Import mode radio box
//
  wxRadioBox *modeSel = (wxRadioBox *) FindWindow(ID_DXF_MIXED);
  ImportMixed = false;
  if (modeSel->GetSelection() == 1)
    ImportMixed = true;
}

void DxfDialog::OnPrefixChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Prefix check-box
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_DXF_OK_PREFIX);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_DXF_PREFIX);
  if (okCtrl->IsChecked() == false)
    {
      OkPrefix = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
  } else
    {
      OkPrefix = true;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
    }
}

void DxfDialog::OnSingleLayerChanged(wxCommandEvent & WXUNUSED(event))
{
//
// selected Single Layer check-box
//
  wxCheckBox *okCtrl = (wxCheckBox *) FindWindow(ID_DXF_OK_SINGLE);
  wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_DXF_SINGLE);
  if (okCtrl->IsChecked() == false)
    {
      OkSingle = false;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(false);
  } else
    {
      OkSingle = true;
      wxString val = wxT("");
      colCtrl->SetValue(val);
      colCtrl->Enable(true);
    }
}

void DxfDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  if (OkPrefix == true)
    {
      wxTextCtrl *prefixCtrl = (wxTextCtrl *) FindWindow(ID_DXF_PREFIX);
      Prefix = prefixCtrl->GetValue();
      if (Prefix.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some DB Table-name Prefix !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  if (OkSingle == true)
    {
      wxTextCtrl *singleCtrl = (wxTextCtrl *) FindWindow(ID_DXF_SINGLE);
      SingleLayer = singleCtrl->GetValue();
      if (SingleLayer.Len() < 1)
        {
          wxMessageBox(wxT("You must specify some DXF Layer name !!!"),
                       wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
          return;
        }
    }
  if (OkPrefix == true)
    {
      wxTextCtrl *colCtrl = (wxTextCtrl *) FindWindow(ID_DXF_PREFIX);
      Prefix = colCtrl->GetValue();
  } else
    Prefix = wxT("");
  wxSpinCtrl *sridCtrl = (wxSpinCtrl *) FindWindow(ID_DXF_SRID);
  Srid = sridCtrl->GetValue();
  if (Srid <= 0)
    ;
  else if (MainFrame->SridNotExists(Srid) == true)
    {
      wxMessageBox(wxT("invalid SRID value"), wxT("spatialite_gui"),
                   wxOK | wxICON_WARNING, this);
      return;
    }
  wxCheckBox *appendCtrl = (wxCheckBox *) FindWindow(ID_DXF_APPEND);
  AppendMode = appendCtrl->IsChecked();
  wxDialog::EndModal(wxID_OK);
}
