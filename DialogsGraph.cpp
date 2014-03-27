/*
/ DialogsGraph.cpp
/ graphics dialog classes
/
/ version 1.7, 2013 May 8
/
/ Author: Sandro Furieri a-furieri@lqt.it
/
/ Copyright (C) 2010-2013  Alessandro Furieri
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
#include "wx/filedlg.h"
#include "wx/filename.h"
#include "wx/colordlg.h"
#include "wx/clipbrd.h"

#include <gaiagraphics.h>

#if defined(_WIN32) || defined (__MINGW32__)
#define FORMAT_64	"%I64d"
#else
#define FORMAT_64	"%lld"
#endif

bool ColumnStatsDialog::Create(MyFrame * parent, wxString & table,
                               wxString & column, int null_count,
                               int text_count, int integer_count,
                               int real_count, int blob_count, double min,
                               double max, double avg, double stddev_pop,
                               double stddev_samp, double var_pop,
                               double var_samp, int distinct_values)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Column = column;
  NullValues = null_count;
  TextValues = text_count;
  IntegerValues = integer_count;
  RealValues = real_count;
  BlobValues = blob_count;
  Min = min;
  Max = max;
  Avg = avg;
  StdDevPop = stddev_pop;
  StdDevSamp = stddev_samp;
  VarPop = var_pop;
  VarSamp = var_samp;
  DistinctValues = distinct_values;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Column statistic snapshot")) ==
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

void ColumnStatsDialog::CreateControls()
{
//
// creating individual control and setting initial values
//
  char dummy[256];
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
// second row: COLUMN name
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
// third row: total rows
  wxBoxSizer *rowSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(rowSizer, 0, wxALIGN_RIGHT | wxALL, 0);
  wxStaticText *rowLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Total values:"));
  rowSizer->Add(rowLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  sprintf(dummy, "%d",
          NullValues + TextValues + IntegerValues + RealValues + BlobValues);
  wxString rows = wxString::FromUTF8(dummy);
  wxTextCtrl *rowCtrl = new wxTextCtrl(this, wxID_ANY, rows,
                                       wxDefaultPosition, wxSize(350, 22),
                                       wxTE_READONLY);
  rowCtrl->Enable(false);
  rowSizer->Add(rowCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
  if (NullValues > 0)
    {
      // NULL values count
      wxBoxSizer *nullSizer = new wxBoxSizer(wxHORIZONTAL);
      boxSizer->Add(nullSizer, 0, wxALIGN_RIGHT | wxALL, 0);
      wxStaticText *nullLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&NULL values:"));
      nullSizer->Add(nullLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%d", NullValues);
      wxString nulls = wxString::FromUTF8(dummy);
      wxTextCtrl *nullCtrl = new wxTextCtrl(this, wxID_ANY, nulls,
                                            wxDefaultPosition, wxSize(350, 22),
                                            wxTE_READONLY);
      nullCtrl->Enable(false);
      nullSizer->Add(nullCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
  if (TextValues > 0)
    {
      // TEXT values count
      wxBoxSizer *textSizer = new wxBoxSizer(wxHORIZONTAL);
      boxSizer->Add(textSizer, 0, wxALIGN_RIGHT | wxALL, 0);
      wxStaticText *textLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&TEXT values:"));
      textSizer->Add(textLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%d", TextValues);
      wxString texts = wxString::FromUTF8(dummy);
      wxTextCtrl *textCtrl = new wxTextCtrl(this, wxID_ANY, texts,
                                            wxDefaultPosition, wxSize(350, 22),
                                            wxTE_READONLY);
      textCtrl->Enable(false);
      textSizer->Add(textCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
  if (IntegerValues > 0)
    {
      // INTEGER values count
      wxBoxSizer *intSizer = new wxBoxSizer(wxHORIZONTAL);
      boxSizer->Add(intSizer, 0, wxALIGN_RIGHT | wxALL, 0);
      wxStaticText *intLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&INTEGER values:"));
      intSizer->Add(intLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%d", IntegerValues);
      wxString ints = wxString::FromUTF8(dummy);
      wxTextCtrl *intCtrl = new wxTextCtrl(this, wxID_ANY, ints,
                                           wxDefaultPosition, wxSize(350, 22),
                                           wxTE_READONLY);
      intCtrl->Enable(false);
      intSizer->Add(intCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
  if (RealValues > 0)
    {
      // REAL values count
      wxBoxSizer *realSizer = new wxBoxSizer(wxHORIZONTAL);
      boxSizer->Add(realSizer, 0, wxALIGN_RIGHT | wxALL, 0);
      wxStaticText *realLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&REAL values:"));
      realSizer->Add(realLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%d", RealValues);
      wxString reals = wxString::FromUTF8(dummy);
      wxTextCtrl *realCtrl = new wxTextCtrl(this, wxID_ANY, reals,
                                            wxDefaultPosition, wxSize(350, 22),
                                            wxTE_READONLY);
      realCtrl->Enable(false);
      realSizer->Add(realCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
  if (BlobValues > 0)
    {
      // BLOB values count
      wxBoxSizer *blobSizer = new wxBoxSizer(wxHORIZONTAL);
      boxSizer->Add(blobSizer, 0, wxALIGN_RIGHT | wxALL, 0);
      wxStaticText *blobLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&BLOB values:"));
      blobSizer->Add(blobLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%d", BlobValues);
      wxString blobs = wxString::FromUTF8(dummy);
      wxTextCtrl *blobCtrl = new wxTextCtrl(this, wxID_ANY, blobs,
                                            wxDefaultPosition, wxSize(350, 22),
                                            wxTE_READONLY);
      blobCtrl->Enable(false);
      blobSizer->Add(blobCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
// statistical group box
  wxStaticBox *statBox = new wxStaticBox(this, wxID_STATIC,
                                         wxT("Data distribution snapshot"),
                                         wxDefaultPosition,
                                         wxDefaultSize);
  wxBoxSizer *statSizer = new wxStaticBoxSizer(statBox, wxVERTICAL);
  boxSizer->Add(statSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
// distinct values count
  wxBoxSizer *distSizer = new wxBoxSizer(wxHORIZONTAL);
  statSizer->Add(distSizer, 0, wxALIGN_CENTER | wxALL, 0);
  wxStaticText *distLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&DISTINCT values:"));
  distSizer->Add(distLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  sprintf(dummy, "%d", DistinctValues);
  wxString dists = wxString::FromUTF8(dummy);
  wxTextCtrl *distCtrl = new wxTextCtrl(this, wxID_ANY, dists,
                                        wxDefaultPosition, wxSize(150, 22),
                                        wxTE_READONLY);
  distCtrl->Enable(false);
  distSizer->Add(distCtrl, 0, wxALIGN_RIGHT | wxALL, 5);

  if ((IntegerValues + RealValues) > 1)
    {
      // statistic analysis: MinMax range 
      wxStaticBox *rangeBox = new wxStaticBox(this, wxID_STATIC,
                                              wxT("Data range"),
                                              wxDefaultPosition,
                                              wxDefaultSize);
      wxBoxSizer *rngSizer = new wxStaticBoxSizer(rangeBox, wxVERTICAL);
      statSizer->Add(rngSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
      wxBoxSizer *minMaxSizer = new wxBoxSizer(wxHORIZONTAL);
      rngSizer->Add(minMaxSizer, 0, wxALIGN_CENTER | wxALL, 0);
      wxStaticText *minLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Min:"));
      minMaxSizer->Add(minLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.18f", Min);
      CleanDecimals(dummy);
      wxString min = wxString::FromUTF8(dummy);
      wxTextCtrl *minCtrl = new wxTextCtrl(this, wxID_ANY, min,
                                           wxDefaultPosition, wxSize(150, 22),
                                           wxTE_READONLY);
      minCtrl->Enable(false);
      minMaxSizer->Add(minCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      wxStaticText *maxLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Max:"));
      minMaxSizer->Add(maxLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.18f", Max);
      CleanDecimals(dummy);
      wxString max = wxString::FromUTF8(dummy);
      wxTextCtrl *maxCtrl = new wxTextCtrl(this, wxID_ANY, max,
                                           wxDefaultPosition, wxSize(150, 22),
                                           wxTE_READONLY);
      maxCtrl->Enable(false);
      minMaxSizer->Add(maxCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      // statistic analysis: Average
      wxBoxSizer *avgSizer = new wxBoxSizer(wxHORIZONTAL);
      statSizer->Add(avgSizer, 0, wxALIGN_CENTER | wxALL, 0);
      wxStaticText *avgLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Average:"));
      avgSizer->Add(avgLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.4f", Avg);
      CleanDecimals(dummy);
      wxString avg = wxString::FromUTF8(dummy);
      wxTextCtrl *avgCtrl = new wxTextCtrl(this, wxID_ANY, avg,
                                           wxDefaultPosition, wxSize(150, 22),
                                           wxTE_READONLY);
      avgCtrl->Enable(false);
      avgSizer->Add(avgCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      // statistic analysis: Standard Deviation
      wxStaticBox *stdDevBox = new wxStaticBox(this, wxID_STATIC,
                                               wxT("Standard deviation"),
                                               wxDefaultPosition,
                                               wxDefaultSize);
      wxBoxSizer *stdSizer = new wxStaticBoxSizer(stdDevBox, wxVERTICAL);
      statSizer->Add(stdSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
      wxBoxSizer *stdDevSizer = new wxBoxSizer(wxHORIZONTAL);
      stdSizer->Add(stdDevSizer, 0, wxALIGN_CENTER | wxALL, 0);
      wxStaticText *devPopLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Pop:"));
      stdDevSizer->Add(devPopLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.4f", StdDevPop);
      CleanDecimals(dummy);
      wxString stdPop = wxString::FromUTF8(dummy);
      wxTextCtrl *stdPopCtrl = new wxTextCtrl(this, wxID_ANY, stdPop,
                                              wxDefaultPosition, wxSize(150,
                                                                        22),
                                              wxTE_READONLY);
      stdPopCtrl->Enable(false);
      stdDevSizer->Add(stdPopCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      wxStaticText *stdSampLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Samp:"));
      stdDevSizer->Add(stdSampLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.4f", StdDevSamp);
      CleanDecimals(dummy);
      wxString stdSamp = wxString::FromUTF8(dummy);
      wxTextCtrl *stdSampCtrl = new wxTextCtrl(this, wxID_ANY, stdSamp,
                                               wxDefaultPosition, wxSize(150,
                                                                         22),
                                               wxTE_READONLY);
      stdSampCtrl->Enable(false);
      stdDevSizer->Add(stdSampCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      // statistic analysis: Variance
      wxStaticBox *varBox = new wxStaticBox(this, wxID_STATIC,
                                            wxT("Variance"),
                                            wxDefaultPosition,
                                            wxDefaultSize);
      wxBoxSizer *varianceSizer = new wxStaticBoxSizer(varBox, wxVERTICAL);
      statSizer->Add(varianceSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
      wxBoxSizer *varSizer = new wxBoxSizer(wxHORIZONTAL);
      varianceSizer->Add(varSizer, 0, wxALIGN_CENTER | wxALL, 0);
      wxStaticText *varPopLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Pop:"));
      varSizer->Add(varPopLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.4f", VarPop);
      CleanDecimals(dummy);
      wxString varPop = wxString::FromUTF8(dummy);
      wxTextCtrl *varPopCtrl = new wxTextCtrl(this, wxID_ANY, varPop,
                                              wxDefaultPosition, wxSize(150,
                                                                        22),
                                              wxTE_READONLY);
      varPopCtrl->Enable(false);
      varSizer->Add(varPopCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
      wxStaticText *varSampLabel =
        new wxStaticText(this, wxID_STATIC, wxT("&Samp:"));
      varSizer->Add(varSampLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
      sprintf(dummy, "%1.4f", VarSamp);
      CleanDecimals(dummy);
      wxString varSamp = wxString::FromUTF8(dummy);
      wxTextCtrl *varSampCtrl = new wxTextCtrl(this, wxID_ANY, varSamp,
                                               wxDefaultPosition, wxSize(150,
                                                                         22),
                                               wxTE_READONLY);
      varSampCtrl->Enable(false);
      varSizer->Add(varSampCtrl, 0, wxALIGN_RIGHT | wxALL, 5);
    }
// buttons
  wxBoxSizer *buttonBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *chart = new wxButton(this, ID_STAT_CHART, wxT("&Show chart"));
  buttonBox->Add(chart, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *exit = new wxButton(this, wxID_OK, wxT("&Exit"));
  buttonBox->Add(exit, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(ID_STAT_CHART, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & ColumnStatsDialog::OnShowChart);
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & ColumnStatsDialog::OnExit);
}

void ColumnStatsDialog::CleanDecimals(char *str)
{
// suppressing not significative decimal digits
  int i;
  int len = strlen(str);
  for (i = len - 1; i >= 0; i--)
    {
      if (str[i] == '0')
        str[i] = '\0';
      else
        break;
    }
  len = strlen(str);
  if (str[len - 1] == '.')
    str[len - 1] = '\0';
}

void ColumnStatsDialog::OnShowChart(wxCommandEvent & WXUNUSED(event))
{
//
// showing a chart: 
//
  bool numeric = false;
  StatsChartDialog dlg;
  if (IntegerValues > 0 || RealValues > 0)
    numeric = true;
  dlg.Create(this, MainFrame, Table, Column, numeric, Min, Max);
  dlg.ShowModal();
}

void ColumnStatsDialog::OnExit(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool StatsChartDialog::Create(ColumnStatsDialog * parent, MyFrame * granny,
                              wxString & table, wxString & column, bool numeric,
                              double min, double max)
{
//
// creating the dialog
//
  MainFrame = granny;
  Table = table;
  Column = column;
  NumericData = numeric;
  Min = min;
  Max = max;
  if (wxDialog::Create(parent, wxID_ANY, wxT("Chart preview")) == false)
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

void StatsChartDialog::CreateControls()
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
// second row: COLUMN name
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
// third row: Chart params
  wxStaticBox *optBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Chart settings"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxVERTICAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *chartSizer = new wxBoxSizer(wxHORIZONTAL);
  optSizer->Add(chartSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString type[3];
  type[0] = wxT("&Histogram");
  type[1] = wxT("&Line chart");
  type[2] = wxT("&Pie chart");
  TypeCtrl = new wxRadioBox(this, ID_CHART_TYPE,
                            wxT("&Type"),
                            wxDefaultPosition,
                            wxDefaultSize, 3, type, 1, wxRA_SPECIFY_COLS);
  chartSizer->Add(TypeCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  TypeCtrl->SetSelection(0);
  Histogram = true;
  LineChart = false;
  PieChart = false;
  wxString size[5];
  size[0] = wxT("&256");
  size[1] = wxT("&512");
  size[2] = wxT("&1024");
  size[3] = wxT("&2048");
  size[4] = wxT("&4196");
  SizeCtrl = new wxRadioBox(this, ID_CHART_SIZE,
                            wxT("&Dimension [pixels]"),
                            wxDefaultPosition,
                            wxDefaultSize, 5, size, 1, wxRA_SPECIFY_COLS);
  chartSizer->Add(SizeCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  SizeCtrl->SetSelection(0);
  wxBoxSizer *mixSizer = new wxBoxSizer(wxVERTICAL);
  chartSizer->Add(mixSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString mode[2];
  mode[0] = wxT("&by intervals");
  mode[1] = wxT("&by unique values");
  ModeCtrl = new wxRadioBox(this, ID_CHART_MODE,
                            wxT("&Mode"),
                            wxDefaultPosition,
                            wxDefaultSize, 2, mode, 1, wxRA_SPECIFY_COLS);
  mixSizer->Add(ModeCtrl, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  if (NumericData == true)
    {
      ModeCtrl->SetSelection(0);
      ByInterval = true;
  } else
    {
      ModeCtrl->SetSelection(1);
      ModeCtrl->Enable(false);
      ByInterval = false;
    }
  wxBoxSizer *classSizer = new wxBoxSizer(wxHORIZONTAL);
  mixSizer->Add(classSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *classLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Classes:"));
  classSizer->Add(classLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  ClassCtrl = new wxSpinCtrl(this, ID_CHART_CLASS, wxT("10"),
                             wxDefaultPosition, wxSize(80, 20),
                             wxSP_ARROW_KEYS, 2, 1000, 10);
  classSizer->Add(ClassCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  Classes = 10;

// creating a control to show the Chart preview
  wxStaticBox *exBox = new wxStaticBox(this, wxID_ANY,
                                       wxT("Chart preview"),
                                       wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *showSizer = new wxStaticBoxSizer(exBox, wxHORIZONTAL);
  boxSizer->Add(showSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  ChartShow = new wxStaticBitmap(this, wxID_ANY,
                                 wxBitmap(), wxDefaultPosition, wxSize(256,
                                                                       256),
                                 wxBORDER_SUNKEN);
  showSizer->Add(ChartShow, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

// buttons
  wxStaticBox *btnBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Export as"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *btnSizer = new wxStaticBoxSizer(btnBox, wxVERTICAL);
  showSizer->Add(btnSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *buttonBox = new wxBoxSizer(wxVERTICAL);
  btnSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxButton *copy = new wxButton(this, ID_CHART_COPY, wxT("&Copy"));
  buttonBox->Add(copy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *png = new wxButton(this, ID_CHART_PNG, wxT("&PNG"));
  buttonBox->Add(png, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *svg = new wxButton(this, ID_CHART_SVG, wxT("&SVG"));
  buttonBox->Add(svg, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *pdf = new wxButton(this, ID_CHART_PDF, wxT("&PDF"));
  buttonBox->Add(pdf, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *exitBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(exitBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *exit = new wxButton(this, wxID_OK, wxT("&Exit"));
  exitBox->Add(exit, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(ID_CHART_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartCopy);
  Connect(ID_CHART_PNG, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartPng);
  Connect(ID_CHART_SVG, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartSvg);
  Connect(ID_CHART_PDF, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartPdf);
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & StatsChartDialog::OnExit);
  Connect(ID_CHART_TYPE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartTypeChanged);
  Connect(ID_CHART_MODE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartModeChanged);
  Connect(ID_CHART_CLASS, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & StatsChartDialog::OnChartClassesChanged);
// showing the current preview
  ReloadData();
  UpdatePreview();
}

void StatsChartDialog::OnChartTypeChanged(wxCommandEvent & WXUNUSED(event))
{
// chart type radiobox changed
  switch (TypeCtrl->GetSelection())
    {
      case 0:
        Histogram = true;
        LineChart = false;
        PieChart = false;
        break;
      case 1:
        Histogram = false;
        LineChart = true;
        PieChart = false;
        break;
      case 2:
        Histogram = false;
        LineChart = false;
        PieChart = true;
        break;
    };
  UpdatePreview();
}

void StatsChartDialog::OnChartModeChanged(wxCommandEvent & WXUNUSED(event))
{
// chart mode radiobox changed
  switch (ModeCtrl->GetSelection())
    {
      case 0:
        ByInterval = true;
        break;
      case 1:
        ByInterval = false;
        break;
    };
  ReloadData();
  UpdatePreview();
}

void StatsChartDialog::OnChartClassesChanged(wxCommandEvent & WXUNUSED(event))
{
//
// Classes selection changed
//
  Classes = ClassCtrl->GetValue();
  ReloadData();
  UpdatePreview();
}

void StatsChartDialog::UpdatePreview()
{
// showing an empty preview chart
  wxBitmap bmp;
  ChartShow->SetBitmap(bmp);
  if (ChartData.IsValid() != true)
    return;

// updating the Chart Preview
  if (ByInterval == true)
    {
      if (Histogram == true)
        DoIntervalHistogram(256, 256, CHART_TARGET_IS_PREVIEW, 8);
      if (LineChart == true)
        DoIntervalLineChart(256, 256, CHART_TARGET_IS_PREVIEW, 8);
      if (PieChart == true)
        DoIntervalPieChart(256, 256, CHART_TARGET_IS_PREVIEW, 8);
  } else
    {
      if (Histogram == true)
        DoUniqueHistogram(256, 256, CHART_TARGET_IS_PREVIEW, 8);
      if (LineChart == true)
        DoUniqueLineChart(256, 256, CHART_TARGET_IS_PREVIEW, 8);
      if (PieChart == true)
        DoUniquePieChart(256, 256, CHART_TARGET_IS_PREVIEW, 8);
    }
}

void StatsChartDialog::ReloadData()
{
// reloading Char Data
  if (ChartData.Check(ByInterval, Classes) == true)
    return;                     /* still valid Data */

  ::wxBeginBusyCursor();
  ChartData.CleanData();
  if (ByInterval == true)
    PrepareDataByInterval(Classes);
  else
    PrepareDataByUniqueValue(Classes);
  ::wxEndBusyCursor();

  if (ChartData.IsValid() != true)
    {
      wxMessageBox(wxT("Unable to retrieve column data"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
}

void StatsChartDialog::OnChartCopy(wxCommandEvent & WXUNUSED(event))
{
// copying the current Chart to the Clipboard 
  int hsize;
  int vsize;
  int font_size;
  if (ChartData.IsValid() != true)
    return;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        font_size = 8;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        font_size = 8;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        font_size = 10;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        font_size = 12;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        font_size = 12;
        break;
    };

// exporting the Chart as COPY
  if (ByInterval == true)
    {
      if (Histogram == true)
        DoIntervalHistogram(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
      if (LineChart == true)
        DoIntervalLineChart(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
      if (PieChart == true)
        DoIntervalPieChart(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
  } else
    {
      if (Histogram == true)
        DoUniqueHistogram(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
      if (LineChart == true)
        DoUniqueLineChart(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
      if (PieChart == true)
        DoUniquePieChart(hsize, vsize, CHART_TARGET_IS_COPY, font_size);
    }
}

void StatsChartDialog::OnChartPng(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Chart as PNG
  int hsize;
  int vsize;
  int font_size;
  if (ChartData.IsValid() != true)
    return;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        font_size = 8;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        font_size = 8;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        font_size = 10;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        font_size = 12;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        font_size = 12;
        break;
    };

// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("PNG Image (*.png)|*.png");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Chart as PNG"),
                          wxT(""), wxT("ChartExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".png");
      ExportPath = path;
  } else
    return;


// exporting the Chart as PNG
  if (ByInterval == true)
    {
      if (Histogram == true)
        DoIntervalHistogram(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
      if (LineChart == true)
        DoIntervalLineChart(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
      if (PieChart == true)
        DoIntervalPieChart(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
  } else
    {
      if (Histogram == true)
        DoUniqueHistogram(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
      if (LineChart == true)
        DoUniqueLineChart(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
      if (PieChart == true)
        DoUniquePieChart(hsize, vsize, CHART_TARGET_IS_PNG, font_size);
    }
}

void StatsChartDialog::OnChartSvg(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Chart as SVG 
  int hsize;
  int vsize;
  int font_size;
  if (ChartData.IsValid() != true)
    return;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        font_size = 8;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        font_size = 8;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        font_size = 10;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        font_size = 12;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        font_size = 12;
        break;
    };

// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("SVG Vector Image (*.svg)|*.svg");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Chart as SVG"),
                          wxT(""), wxT("ChartExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".svg");
      ExportPath = path;
  } else
    return;


// exporting the Chart as SVG
  if (ByInterval == true)
    {
      if (Histogram == true)
        DoIntervalHistogram(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
      if (LineChart == true)
        DoIntervalLineChart(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
      if (PieChart == true)
        DoIntervalPieChart(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
  } else
    {
      if (Histogram == true)
        DoUniqueHistogram(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
      if (LineChart == true)
        DoUniqueLineChart(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
      if (PieChart == true)
        DoUniquePieChart(hsize, vsize, CHART_TARGET_IS_SVG, font_size);
    };
}

void StatsChartDialog::OnChartPdf(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Chart as PDF 
  int hsize = (int) (12.8 * 300.0);
  int vsize = (int) (9.0 * 300.0);
  hsize -= 100;                 // margin
  vsize -= 100;                 // margin
  if (ChartData.IsValid() != true)
    return;

// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("PDF Document (*.pdf)|*.pdf");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Chart as PDF"),
                          wxT(""), wxT("ChartExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".pdf");
      ExportPath = path;
  } else
    return;


// exporting the Chart as PDF
  if (ByInterval == true)
    {
      if (Histogram == true)
        DoIntervalHistogram(hsize, vsize, CHART_TARGET_IS_PDF, 120);
      if (LineChart == true)
        DoIntervalLineChart(hsize, vsize, CHART_TARGET_IS_PDF, 120);
      if (PieChart == true)
        DoIntervalPieChart(hsize, vsize, CHART_TARGET_IS_PDF, 120);
  } else
    {
      if (Histogram == true)
        DoUniqueHistogram(hsize, vsize, CHART_TARGET_IS_PDF, 120);
      if (LineChart == true)
        DoUniqueLineChart(hsize, vsize, CHART_TARGET_IS_PDF, 120);
      if (PieChart == true)
        DoUniquePieChart(hsize, vsize, CHART_TARGET_IS_PDF, 120);
    };
}

void StatsChartDialog::PrepareDataByInterval(int classes)
{
// feeding class data by intervals
  char sql[4196];
  char table[1024];
  char column[1024];
  int ret;
  char err_msg[2048];
  sqlite3_stmt *stmt;
  sqlite3 *sqlite = MainFrame->GetSqlite();

  ChartData.Create(Min, Max, classes);

  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(sql, "SELECT %s FROM %s", column, table);
  ret = sqlite3_prepare_v2(sqlite, sql, strlen(sql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(sqlite));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
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
          //
          // fetching a row
          //
          double value;
          if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
            {
              sqlite3_int64 intval = sqlite3_column_int64(stmt, 0);
              value = intval;
              ChartData.Add(value);
            }
          if (sqlite3_column_type(stmt, 0) == SQLITE_FLOAT)
            {
              value = sqlite3_column_double(stmt, 0);
              ChartData.Add(value);
            }
      } else
        {
          sqlite3_finalize(stmt);
          sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(sqlite));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          return;
        }
    }
  sqlite3_finalize(stmt);
  ChartData.SetValid();
}

void StatsChartDialog::PrepareDataByUniqueValue(int classes)
{
// feeding class data by unique values
  char sql[4196];
  char table[1024];
  char column[1024];
  int ret;
  char err_msg[2048];
  sqlite3_stmt *stmt;
  sqlite3 *sqlite = MainFrame->GetSqlite();
  ChartData.Create(classes);

  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(sql, "SELECT %s, Count(*) FROM %s GROUP BY %s", column, table,
          column);
  strcat(sql, " ORDER BY 2 DESC");
  ret = sqlite3_prepare_v2(sqlite, sql, strlen(sql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(sqlite));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
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
          //
          // fetching a row
          //
          bool valid = false;
          wxString value;
          char dummy[128];
          if (sqlite3_column_type(stmt, 0) == SQLITE_INTEGER)
            {
              sqlite3_int64 intval = sqlite3_column_int64(stmt, 0);
              sprintf(dummy, FORMAT_64, intval);
              value = wxString::FromUTF8(dummy);
              valid = true;
            }
          if (sqlite3_column_type(stmt, 0) == SQLITE_FLOAT)
            {
              double dblval = sqlite3_column_double(stmt, 0);
              sprintf(dummy, "%1.18f", dblval);
              CleanDecimals(dummy);
              value = wxString::FromUTF8(dummy);
              valid = true;
            }
          if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
            {
              const char *txtval = (const char *) sqlite3_column_text(stmt, 0);
              value = wxString::FromUTF8(txtval);
              valid = true;
            }
          if (valid == true)
            {
              int count = sqlite3_column_int(stmt, 1);
              ChartData.Add(value, count);
            }

      } else
        {
          sqlite3_finalize(stmt);
          sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(sqlite));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          return;
        }
    }
  sqlite3_finalize(stmt);
  ChartData.SetValid();
}

void StatsChartDialog::DoIntervalHistogram(int hsize, int vsize, int target,
                                           int font_size)
{
// generating an Histogram (Interval values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  int start_x;
  int end_x;
  int start_y;
  int end_y;
  double base_x;
  double step_x;
  double base_y;
  double vspan;
  double height;
  char title[1024];
  char text[1024];
  char table[1024];
  char column[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  double labelWidth = 0.0;
  double labelBase;
  double labelStep;
  wxColour colors[8];
  int color_idx;
  MyChartScaleLabels scaleLabels;
  MyChartScaleLabel *pLab;
  double scaleWidth = 0.0;

// color palette
  colors[0] = wxColour(255, 255, 240);
  colors[1] = wxColour(255, 240, 255);
  colors[2] = wxColour(240, 255, 255);
  colors[3] = wxColour(240, 240, 240);
  colors[4] = wxColour(255, 192, 192);
  colors[5] = wxColour(192, 255, 192);
  colors[6] = wxColour(192, 192, 255);
  colors[7] = wxColour(192, 192, 192);

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [interval values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
// measuring class labels
  gGraphSetFont(gr, font);
  sprintf(text, "%1.4f", Min);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 4.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 2.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + (((Max - Min) / 4.0) * 3.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Max);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;

  start_y = vsize - 10 - (int) labelWidth;
  end_y = 10 + (int) titleHeight;
  vspan = start_y - end_y;
  base_y = start_y;

// building and measuring  the 'scale' labels
  scaleLabels.Initialize(vspan, ChartData.GetMaxFreq());
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > scaleWidth)
        scaleWidth = txtWidth;
      pLab = pLab->GetNext();
    }

  start_x = 10 + (int) scaleWidth;
  end_x = hsize - 10;
  base_x = start_x;
  step_x = (double) (end_x - start_x) / (double) (ChartData.GetNumClasses());
  labelBase = base_x + (step_x / 2.0);
  labelStep = (end_x - start_x - step_x) / 4;

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);
// class labels output
  gGraphSetFont(gr, font);
  sprintf(text, "%1.4f", Min);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 4.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 2.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + (((Max - Min) / 4.0) * 3.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Max);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));

  for (idx = 0; idx < ChartData.GetNumClasses(); idx++)
    {
      // drawing bars
      MyChartIntervalClass *p = ChartData.GetClass(idx);
      gGraphSetPen(gr, 0, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      gGraphDrawRectangle(gr, base_x, base_y - height, step_x, height);
      base_x += step_x;
    }
// drawing 'scale' labels
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                     base_y - pLab->GetPosition() + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 128, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, start_x, base_y - pLab->GetPosition(), end_x,
                       base_y - pLab->GetPosition());
      pLab = pLab->GetNext();
    }
// marking the ZERO baseline
  gGraphSetFont(gr, font);
  strcpy(text, "0");
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                 base_y + (txtHeight / 2.0), 0.0);
  gGraphSetPen(gr, 255, 128, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
  gGraphStrokeLine(gr, start_x, base_y, end_x, base_y);

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::DoIntervalLineChart(int hsize, int vsize, int target,
                                           int font_size)
{
// generating a Line Chart (Interval values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  int start_x;
  int end_x;
  int start_y;
  int end_y;
  double base_x;
  double step_x;
  double base_y;
  double vspan;
  double height;
  char title[1024];
  char table[1024];
  char column[1024];
  char text[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  double labelWidth = 0.0;
  double labelBase;
  double labelStep;
  MyChartScaleLabels scaleLabels;
  MyChartScaleLabel *pLab;
  double scaleWidth = 0.0;

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [interval values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
// measuring class labels
  gGraphSetFont(gr, font);
  sprintf(text, "%1.4f", Min);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 4.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 2.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Min + (((Max - Min) / 4.0) * 3.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;
  sprintf(text, "%1.4f", Max);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  if (txtWidth > labelWidth)
    labelWidth = txtWidth;

  start_y = vsize - 10 - (int) labelWidth;
  end_y = 10 + (int) titleHeight;
  vspan = start_y - end_y;
  base_y = start_y;

// building and measuring  the 'scale' labels
  scaleLabels.Initialize(vspan, ChartData.GetMaxFreq());
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > scaleWidth)
        scaleWidth = txtWidth;
      pLab = pLab->GetNext();
    }

  start_x = 10 + (int) scaleWidth;
  end_x = hsize - 10;
  base_x = start_x;
  step_x = (double) (end_x - start_x) / (double) (ChartData.GetNumClasses());
  labelBase = base_x + (step_x / 2.0);
  labelStep = (end_x - start_x - step_x) / 4;

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);
// class labels output
  gGraphSetFont(gr, font);
  sprintf(text, "%1.4f", Min);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 4.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + ((Max - Min) / 2.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Min + (((Max - Min) / 4.0) * 3.0));
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
  labelBase += labelStep;
  sprintf(text, "%1.4f", Max);
  CleanDecimals(text);
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, labelBase + txtHeight,
                 vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));

  for (idx = 0; idx < ChartData.GetNumClasses(); idx++)
    {
      MyChartIntervalClass *p = ChartData.GetClass(idx);
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      gGraphSetPen(gr, 192, 192, 192, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, base_x + (step_x / 2.0), start_y,
                       base_x + (step_x / 2.0), base_y - height);
      base_x += step_x;
    }
  base_x = start_x;
  for (idx = 0; idx < ChartData.GetNumClasses(); idx++)
    {
      MyChartIntervalClass *p = ChartData.GetClass(idx);
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      if (idx == 0)
        gGraphMoveToPoint(gr, base_x + (step_x / 2.0), base_y - height);
      else
        gGraphAddLineToPath(gr, base_x + (step_x / 2.0), base_y - height);
      base_x += step_x;
    }
  gGraphSetPen(gr, 255, 0, 0, 255, 2, GGRAPH_PENSTYLE_SOLID);
  gGraphStrokePath(gr, GGRAPH_CLEAR_PATH);

// drawing 'scale' labels
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                     base_y - pLab->GetPosition() + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 128, 255, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, start_x, base_y - pLab->GetPosition(), end_x,
                       base_y - pLab->GetPosition());
      pLab = pLab->GetNext();
    }
// marking the ZERO baseline
  gGraphSetFont(gr, font);
  strcpy(text, "0");
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                 base_y + (txtHeight / 2.0), 0.0);
  gGraphSetPen(gr, 128, 255, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
  gGraphStrokeLine(gr, start_x, base_y, end_x, base_y);

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::DoIntervalPieChart(int hsize, int vsize, int target,
                                          int font_size)
{
// generating a Pie Chart (Interval values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  double radius_x;
  double radius_y;
  double radius;
  double cx;
  double cy;
  double from;
  double step;
  char title[1024];
  char table[1024];
  char column[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  wxColour colors[8];
  int color_idx;
  MyPieChartLabels labels;
  MyPieChartLabel *pLab;
  char text[1024];
  double lx;
  double ly;
  double base_y;
  double step_y;
  double labelWidth = 0.0;

// color palette
  colors[0] = wxColour(255, 255, 240);
  colors[1] = wxColour(255, 240, 255);
  colors[2] = wxColour(240, 255, 255);
  colors[3] = wxColour(240, 240, 240);
  colors[4] = wxColour(255, 192, 192);
  colors[5] = wxColour(192, 255, 192);
  colors[6] = wxColour(192, 192, 255);
  colors[7] = wxColour(192, 192, 192);

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [interval values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
  for (idx = 0; idx < ChartData.GetNumClasses(); idx++)
    {
      // measuring labels
      MyChartIntervalClass *p = ChartData.GetClass(idx);
      gGraphSetFont(gr, font);
      sprintf(text, "%1.4f", p->GetMin() + ((p->GetMax() - p->GetMin()) / 2.0));
      CleanDecimals(text);
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
    }

  cx = (double) hsize / 2.0;
  cy = (double) vsize / 2.0;
  cy += (double) (titleHeight + 10) / 2.0;
  radius_x = (double) (hsize - 20) / 2.0;
  radius_x -= labelWidth + 10;
  radius_y = (double) (vsize - titleHeight - 20) / 2.0;
  if (radius_x <= radius_y)
    radius = radius_x;
  else
    radius = radius_y;

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);

  from = 0.0;
  for (idx = 0; idx < ChartData.GetNumClasses(); idx++)
    {
      MyChartIntervalClass *p = ChartData.GetClass(idx);
      gGraphSetPen(gr, 0, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      step =
        (M_PI * 2.0) *
        ((double) (p->GetCount() / (double) (ChartData.GetTotFreq())));
      gGraphDrawCircleSector(gr, cx, cy, radius, from, from + step);
      sprintf(text, "%1.4f", p->GetMin() + ((p->GetMax() - p->GetMin()) / 2.0));
      CleanDecimals(text);
      lx = cx + (radius * 0.90) * cos(from + (step / 2.0));
      ly = cy + (radius * 0.90) * sin(from + (step / 2.0));
      labels.Add(text, lx, ly);
      from += step;
    }
// printing class labels
  labels.Sort(cx);
  step_y = (double) (vsize - 50) / (double) (labels.GetNumLeftLabels() - 1);
  base_y = 25.0;
  for (idx = 0; idx < labels.GetNumLeftLabels(); idx++)
    {
      // printing Left labels
      pLab = labels.GetLeftLabel(idx);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, 10, base_y + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      gGraphStrokeLine(gr, 10 + txtWidth, base_y, pLab->GetX(), pLab->GetY());
      base_y += step_y;
    }
  step_y = (double) (vsize - 50) / (double) (labels.GetNumRightLabels() - 1);
  base_y = 25.0;
  for (idx = 0; idx < labels.GetNumRightLabels(); idx++)
    {
      // printing Right labels
      pLab = labels.GetRightLabel(idx);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, hsize - txtWidth - 10,
                     base_y + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      gGraphStrokeLine(gr, hsize - txtWidth - 10, base_y, pLab->GetX(),
                       pLab->GetY());
      base_y += step_y;
    }

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::DoUniqueHistogram(int hsize, int vsize, int target,
                                         int font_size)
{
// generating an Histogram (Unique values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  MyChartUniqueClass *p;
  int start_x;
  int end_x;
  int start_y;
  int end_y;
  double base_x;
  double step_x;
  double base_y;
  double vspan;
  double height;
  char title[1024];
  char table[1024];
  char column[1024];
  char text[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  double labelWidth = 0.0;
  int others = 0;
  wxColour colors[8];
  int color_idx;
  MyChartScaleLabels scaleLabels;
  MyChartScaleLabel *pLab;
  double scaleWidth = 0.0;

// color palette
  colors[0] = wxColour(255, 255, 240);
  colors[1] = wxColour(255, 240, 255);
  colors[2] = wxColour(240, 255, 255);
  colors[3] = wxColour(240, 240, 240);
  colors[4] = wxColour(255, 192, 192);
  colors[5] = wxColour(192, 255, 192);
  colors[6] = wxColour(192, 192, 255);
  colors[7] = wxColour(192, 192, 192);

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

  if (ChartData.GetOtherUniquesFreq() > 0)
    others = 1;
// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [unique values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
// measuring class labels
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetFont(gr, font);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
      p = p->GetNext();
    }
  if (others)
    {
      // including the 'any other' class
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, "AnyOther", &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
    }

  start_y = vsize - 10 - (int) labelWidth;
  end_y = 10 + (int) titleHeight;
  vspan = start_y - end_y;
  base_y = start_y;

// building and measuring  the 'scale' labels
  scaleLabels.Initialize(vspan, ChartData.GetMaxFreq());
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > scaleWidth)
        scaleWidth = txtWidth;
      pLab = pLab->GetNext();
    }

  start_x = 10 + (int) scaleWidth;
  end_x = hsize - 10;
  base_x = start_x;
  step_x =
    (double) (end_x - start_x) / (double) (ChartData.GetNumClasses() + others);

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);
// class labels output
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetFont(gr, font);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, base_x + (txtHeight / 2.0) + (step_x / 2.0),
                     vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
      base_x += step_x;
      p = p->GetNext();
    }
  if (others)
    {
      // including the 'any other' class
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, "AnyOther", &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, "AnyOther",
                     base_x + (txtHeight / 2.0) + (step_x / 2.0),
                     vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
    }

  idx = 0;
  base_x = start_x;
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetPen(gr, 0, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      gGraphDrawRectangle(gr, base_x, base_y - height, step_x, height);
      base_x += step_x;
      idx++;
      p = p->GetNext();
    }
  if (others)
    {
      // other unclassified values
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      height =
        vspan *
        ((double)
         (ChartData.GetOtherUniquesFreq() / (double) (ChartData.GetMaxFreq())));
      gGraphDrawRectangle(gr, base_x, base_y - height, step_x, height);
    }
// drawing 'scale' labels
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                     base_y - pLab->GetPosition() + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 128, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, start_x, base_y - pLab->GetPosition(), end_x,
                       base_y - pLab->GetPosition());
      pLab = pLab->GetNext();
    }
// marking the ZERO baseline
  gGraphSetFont(gr, font);
  strcpy(text, "0");
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                 base_y + (txtHeight / 2.0), 0.0);
  gGraphSetPen(gr, 255, 128, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
  gGraphStrokeLine(gr, start_x, base_y, end_x, base_y);

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::DoUniqueLineChart(int hsize, int vsize, int target,
                                         int font_size)
{
// generating a Line Chart (Unique values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  MyChartUniqueClass *p;
  int start_x;
  int end_x;
  int start_y;
  int end_y;
  double base_x;
  double step_x;
  double base_y;
  double vspan;
  double height;
  char title[1024];
  char table[1024];
  char column[1024];
  char text[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  double labelWidth = 0.0;
  int others = 0;
  MyChartScaleLabels scaleLabels;
  MyChartScaleLabel *pLab;
  double scaleWidth = 0.0;

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

  if (ChartData.GetOtherUniquesFreq() > 0)
    others = 1;
// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [unique values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
// measuring class labels
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetFont(gr, font);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
      p = p->GetNext();
    }
  if (others)
    {
      // including the 'any other' class
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, "AnyOther", &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
    }

  start_y = vsize - 10 - (int) labelWidth;
  end_y = 10 + (int) titleHeight;
  vspan = start_y - end_y;
  base_y = start_y;

// building and measuring  the 'scale' labels
  scaleLabels.Initialize(vspan, ChartData.GetMaxFreq());
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > scaleWidth)
        scaleWidth = txtWidth;
      pLab = pLab->GetNext();
    }

  start_x = 10 + (int) scaleWidth;
  end_x = hsize - 10;
  base_x = start_x;
  step_x =
    (double) (end_x - start_x) / (double) (ChartData.GetNumClasses() + others);

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);
// class labels output
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetFont(gr, font);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, base_x + (txtHeight / 2.0) + (step_x / 2.0),
                     vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
      base_x += step_x;
      p = p->GetNext();
    }
  if (others)
    {
      // including the 'any other' class
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, "AnyOther", &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, "AnyOther",
                     base_x + (txtHeight / 2.0) + (step_x / 2.0),
                     vsize - labelWidth + txtWidth - 5, M_PI + (M_PI / 2.0));
    }

  idx = 0;
  base_x = start_x;
  p = ChartData.GetFirst();
  while (p)
    {
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      gGraphSetPen(gr, 192, 192, 192, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, base_x + (step_x / 2.0), start_y,
                       base_x + (step_x / 2.0), base_y - height);
      base_x += step_x;
      idx++;
      p = p->GetNext();
    }
  if (others)
    {
      // other unclassified values
      height =
        vspan *
        ((double)
         (ChartData.GetOtherUniquesFreq() / (double) (ChartData.GetMaxFreq())));
      gGraphStrokeLine(gr, base_x + (step_x / 2.0), start_y,
                       base_x + (step_x / 2.0), base_y - height);
    }
  idx = 0;
  base_x = start_x;
  p = ChartData.GetFirst();
  while (p)
    {
      height =
        vspan * ((double) (p->GetCount() / (double) (ChartData.GetMaxFreq())));
      if (idx == 0)
        gGraphMoveToPoint(gr, base_x + (step_x / 2.0), base_y - height);
      else
        gGraphAddLineToPath(gr, base_x + (step_x / 2.0), base_y - height);
      base_x += step_x;
      idx++;
      p = p->GetNext();
    }
  if (others)
    {
      // other unclassified values
      height =
        vspan *
        ((double)
         (ChartData.GetOtherUniquesFreq() / (double) (ChartData.GetMaxFreq())));
      gGraphAddLineToPath(gr, base_x + (step_x / 2.0), base_y - height);
    }
  gGraphSetPen(gr, 255, 0, 0, 255, 2, GGRAPH_PENSTYLE_SOLID);
  gGraphStrokePath(gr, GGRAPH_CLEAR_PATH);

// drawing 'scale' labels
  pLab = scaleLabels.GetFirst();
  while (pLab)
    {
      gGraphSetFont(gr, font);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                     base_y - pLab->GetPosition() + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 128, 255, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
      gGraphStrokeLine(gr, start_x, base_y - pLab->GetPosition(), end_x,
                       base_y - pLab->GetPosition());
      pLab = pLab->GetNext();
    }
// marking the ZERO baseline
  gGraphSetFont(gr, font);
  strcpy(text, "0");
  gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight, &post_x,
                      &post_y);
  gGraphDrawText(gr, text, scaleWidth - txtWidth + 5,
                 base_y + (txtHeight / 2.0), 0.0);
  gGraphSetPen(gr, 128, 255, 128, 255, 1, GGRAPH_PENSTYLE_DOT);
  gGraphStrokeLine(gr, start_x, base_y, end_x, base_y);

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::DoUniquePieChart(int hsize, int vsize, int target,
                                        int font_size)
{
// generating a Pie Chart (Unique values)
  unsigned char *rgb_array = NULL;
  const void *gr = NULL;
  const void *font = NULL;
  const void *font_big = NULL;
  int idx;
  MyChartUniqueClass *p;
  double radius_x;
  double radius_y;
  double radius;
  double cx;
  double cy;
  double from;
  double step;
  char title[1024];
  char table[1024];
  char column[1024];
  double txtWidth;
  double txtHeight;
  double titleHeight;
  double pre_x;
  double pre_y;
  double post_x;
  double post_y;
  int title_x;
  int title_y;
  wxColour colors[8];
  int color_idx;
  MyPieChartLabels labels;
  MyPieChartLabel *pLab;
  char text[1024];
  double lx;
  double ly;
  double base_y;
  double step_y;
  double labelWidth = 0.0;
  int others = 0;

// color palette
  colors[0] = wxColour(255, 255, 240);
  colors[1] = wxColour(255, 240, 255);
  colors[2] = wxColour(240, 255, 255);
  colors[3] = wxColour(240, 240, 240);
  colors[4] = wxColour(255, 192, 192);
  colors[5] = wxColour(192, 255, 192);
  colors[6] = wxColour(192, 192, 255);
  colors[7] = wxColour(192, 192, 192);

// graphics initialization
  if (target == CHART_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == CHART_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// font setup
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_NORMAL,
                   &font);
  gGraphFontSetColor(font, 0, 0, 0, 255);
  gGraphCreateFont(font_size, GGRAPH_FONTSTYLE_NORMAL, GGRAPH_FONTWEIGHT_BOLD,
                   &font_big);
  gGraphFontSetColor(font_big, 0, 0, 0, 255);

  if (ChartData.GetOtherUniquesFreq() > 0)
    others = 1;
// computing TEXT sizes
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(title, "Dataset: %s.%s     [unique values]", table, column);
  gGraphSetFont(gr, font_big);
  gGraphGetTextExtent(gr, title, &pre_x, &pre_y, &txtWidth, &titleHeight,
                      &post_x, &post_y);
  title_x = (hsize - (int) txtWidth) / 2;
  title_y = 5 + (int) titleHeight;
  p = ChartData.GetFirst();
  while (p)
    {
      // measuring labels
      gGraphSetFont(gr, font);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
      p = p->GetNext();
    }
  if (others)
    {
      gGraphGetTextExtent(gr, "AnyOther", &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      if (txtWidth > labelWidth)
        labelWidth = txtWidth;
    }

  cx = (double) hsize / 2.0;
  cy = (double) vsize / 2.0;
  cy += (double) (titleHeight + 10) / 2.0;
  radius_x = (double) (hsize - 20) / 2.0;
  radius_x -= labelWidth + 10;
  radius_y = (double) (vsize - titleHeight - 20) / 2.0;
  if (radius_x <= radius_y)
    radius = radius_x;
  else
    radius = radius_y;

// title output
  gGraphSetFont(gr, font_big);
  gGraphDrawText(gr, title, title_x, title_y, 0.0);

  from = 0.0;
  idx = 0;
  p = ChartData.GetFirst();
  while (p)
    {
      gGraphSetPen(gr, 0, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      step =
        (M_PI * 2.0) *
        ((double) (p->GetCount() / (double) (ChartData.GetTotFreq())));
      gGraphDrawCircleSector(gr, cx, cy, radius, from, from + step);
      strcpy(text, p->GetValue().ToUTF8());
      if (strlen(text) > 10)
        {
          text[10] = '.';
          text[11] = '.';
          text[12] = '\0';      // truncating to max 10 chars
        }
      lx = cx + (radius * 0.90) * cos(from + (step / 2.0));
      ly = cy + (radius * 0.90) * sin(from + (step / 2.0));
      labels.Add(text, lx, ly);
      from += step;
      idx++;
      p = p->GetNext();
    }
  if (others)
    {
      // other unclassified values
      color_idx = idx % 8;
      gGraphSetBrush(gr, colors[color_idx].Red(), colors[color_idx].Green(),
                     colors[color_idx].Blue(), 255);
      step =
        (M_PI * 2.0) *
        ((double)
         (ChartData.GetOtherUniquesFreq() / (double) (ChartData.GetTotFreq())));
      gGraphDrawCircleSector(gr, cx, cy, radius, from, from + step);
      lx = cx + (radius * 0.90) * cos(from + (step / 2.0));
      ly = cy + (radius * 0.90) * sin(from + (step / 2.0));
      labels.Add("AnyOther", lx, ly);
    }
// printing class labels
  labels.Sort(cx);
  step_y = (double) (vsize - 50) / (double) (labels.GetNumLeftLabels() - 1);
  base_y = 25.0;
  for (idx = 0; idx < labels.GetNumLeftLabels(); idx++)
    {
      // printing Left labels
      pLab = labels.GetLeftLabel(idx);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, 10, base_y + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      gGraphStrokeLine(gr, 10 + txtWidth, base_y, pLab->GetX(), pLab->GetY());
      base_y += step_y;
    }
  step_y = (double) (vsize - 50) / (double) (labels.GetNumRightLabels() - 1);
  base_y = 25.0;
  for (idx = 0; idx < labels.GetNumRightLabels(); idx++)
    {
      // printing Right labels
      pLab = labels.GetRightLabel(idx);
      strcpy(text, pLab->GetLabel().ToUTF8());
      gGraphSetFont(gr, font);
      gGraphGetTextExtent(gr, text, &pre_x, &pre_y, &txtWidth, &txtHeight,
                          &post_x, &post_y);
      gGraphDrawText(gr, text, hsize - txtWidth - 10,
                     base_y + (txtHeight / 2.0), 0.0);
      gGraphSetPen(gr, 255, 0, 0, 255, 1, GGRAPH_PENSTYLE_SOLID);
      gGraphStrokeLine(gr, hsize - txtWidth - 10, base_y, pLab->GetX(),
                       pLab->GetY());
      base_y += step_y;
    }

// graphics finalization
  gGraphDestroyFont(font);
  gGraphDestroyFont(font_big);
  if (target == CHART_TARGET_IS_COPY || target == CHART_TARGET_IS_PNG
      || target == CHART_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == CHART_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == CHART_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == CHART_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Char as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == CHART_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          ChartShow->SetBitmap(bmp);
        }
    }
  if (target == CHART_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void StatsChartDialog::CleanDecimals(char *str)
{
// suppressing not significative decimal digits
  int i;
  int len = strlen(str);
  for (i = len - 1; i >= 0; i--)
    {
      if (str[i] == '0')
        str[i] = '\0';
      else
        break;
    }
  len = strlen(str);
  if (str[len - 1] == '.')
    str[len - 1] = '\0';
}

void StatsChartDialog::OnExit(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}

bool MapPreviewDialog::Create(MyFrame * parent, wxString & table,
                              wxString & column, double minx, double miny,
                              double maxx, double maxy)
{
//
// creating the dialog
//
  MainFrame = parent;
  Table = table;
  Column = column;
  MinX = minx;
  MinY = miny;
  MaxX = maxx;
  MaxY = maxy;
  LineColor = wxColour(0, 0, 0);
  FillColor = wxColour(192, 192, 192);
  if (wxDialog::Create(parent, wxID_ANY, wxT("Map preview")) == false)
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

void MapPreviewDialog::CreateControls()
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
// second row: COLUMN name
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
// third row: Graphics params
  wxStaticBox *optBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Graphics"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxVERTICAL);
  boxSizer->Add(optSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *graphSizer = new wxBoxSizer(wxHORIZONTAL);
  optSizer->Add(graphSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString size[5];
  size[0] = wxT("&256");
  size[1] = wxT("&512");
  size[2] = wxT("&1024");
  size[3] = wxT("&2048");
  size[4] = wxT("&4196");
  SizeCtrl = new wxRadioBox(this, ID_CHART_SIZE,
                            wxT("&Dimension [pixels]"),
                            wxDefaultPosition,
                            wxDefaultSize, 5, size, 1, wxRA_SPECIFY_COLS);
  graphSizer->Add(SizeCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  SizeCtrl->SetSelection(0);
  wxBoxSizer *mixSizer = new wxBoxSizer(wxVERTICAL);
  graphSizer->Add(mixSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBoxSizer *ptSizer = new wxBoxSizer(wxHORIZONTAL);
  mixSizer->Add(ptSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString symbol[2];
  symbol[0] = wxT("&Circle");
  symbol[1] = wxT("&Square");
  SymbolCtrl = new wxRadioBox(this, ID_MAP_SYMBOL,
                              wxT("&Point Symbol"),
                              wxDefaultPosition,
                              wxDefaultSize, 2, symbol, 1, wxRA_SPECIFY_ROWS);
  ptSizer->Add(SymbolCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  SymbolCtrl->SetSelection(0);
  wxBoxSizer *szSizer = new wxBoxSizer(wxHORIZONTAL);
  ptSizer->Add(szSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *szLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Symbol size:"));
  szSizer->Add(szLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  SymSizeCtrl = new wxSpinCtrl(this, ID_MAP_SYM_SIZE, wxT("3"),
                               wxDefaultPosition, wxSize(50, 20),
                               wxSP_ARROW_KEYS, 1, 32, 3);
  szSizer->Add(SymSizeCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *lnpgSizer = new wxBoxSizer(wxHORIZONTAL);
  mixSizer->Add(lnpgSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxString fill[2];
  fill[0] = wxT("&Yes");
  fill[1] = wxT("&No");
  FillCtrl = new wxRadioBox(this, ID_MAP_FILL,
                            wxT("&Interior filling"),
                            wxDefaultPosition,
                            wxDefaultSize, 2, fill, 1, wxRA_SPECIFY_ROWS);
  lnpgSizer->Add(FillCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  FillCtrl->SetSelection(0);
  wxBoxSizer *thkSizer = new wxBoxSizer(wxHORIZONTAL);
  lnpgSizer->Add(thkSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *thickLabel =
    new wxStaticText(this, wxID_STATIC, wxT("&Line thickness:"));
  thkSizer->Add(thickLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  ThicknessCtrl = new wxSpinCtrl(this, ID_MAP_THICKNESS, wxT("1"),
                                 wxDefaultPosition, wxSize(50, 20),
                                 wxSP_ARROW_KEYS, 1, 32, 1);
  thkSizer->Add(ThicknessCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *colorSizer = new wxBoxSizer(wxHORIZONTAL);
  mixSizer->Add(colorSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxBitmap lineBmp;
  GetButtonBitmap(LineColor, lineBmp);
  LineColorCtrl = new wxBitmapButton(this, ID_MAP_LINE_COL, lineBmp);
  colorSizer->Add(LineColorCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBitmap fillBmp;
  GetButtonBitmap(FillColor, fillBmp);
  FillColorCtrl = new wxBitmapButton(this, ID_MAP_FILL_COL, fillBmp);
  colorSizer->Add(FillColorCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

// creating a control to show the Map preview
  wxStaticBox *exBox = new wxStaticBox(this, wxID_ANY,
                                       wxT("Map preview"),
                                       wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *showSizer = new wxStaticBoxSizer(exBox, wxHORIZONTAL);
  boxSizer->Add(showSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  MapShow = new wxStaticBitmap(this, wxID_ANY,
                               wxBitmap(), wxDefaultPosition, wxSize(256, 256),
                               wxBORDER_SUNKEN);
  showSizer->Add(MapShow, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

// buttons
  wxStaticBox *btnBox = new wxStaticBox(this, wxID_STATIC,
                                        wxT("Export as"),
                                        wxDefaultPosition,
                                        wxDefaultSize);
  wxBoxSizer *btnSizer = new wxStaticBoxSizer(btnBox, wxVERTICAL);
  showSizer->Add(btnSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *buttonBox = new wxBoxSizer(wxVERTICAL);
  btnSizer->Add(buttonBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
  wxButton *copy = new wxButton(this, ID_MAP_COPY, wxT("&Copy"));
  buttonBox->Add(copy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *png = new wxButton(this, ID_MAP_PNG, wxT("&PNG"));
  buttonBox->Add(png, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *svg = new wxButton(this, ID_MAP_SVG, wxT("&SVG"));
  buttonBox->Add(svg, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *pdf = new wxButton(this, ID_MAP_PDF, wxT("&PDF"));
  buttonBox->Add(pdf, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxBoxSizer *exitBox = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(exitBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *exit = new wxButton(this, wxID_OK, wxT("&Exit"));
  exitBox->Add(exit, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
// appends event handler for OK button
  Connect(ID_MAP_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnMapCopy);
  Connect(ID_MAP_PNG, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnMapPng);
  Connect(ID_MAP_SVG, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnMapSvg);
  Connect(ID_MAP_PDF, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnMapPdf);
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnExit);
  Connect(ID_MAP_LINE_COL, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnLineColor);
  Connect(ID_MAP_FILL_COL, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & MapPreviewDialog::OnFillColor);
  Connect(ID_MAP_SYMBOL, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & MapPreviewDialog::OnSymbolTypeChanged);
  Connect(ID_MAP_FILL, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & MapPreviewDialog::OnLineThicknessChanged);
  Connect(ID_MAP_THICKNESS, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & MapPreviewDialog::OnLineThicknessChanged);
  Connect(ID_MAP_SIZE, wxEVT_COMMAND_TEXT_UPDATED,
          (wxObjectEventFunction) & MapPreviewDialog::OnSymbolSizeChanged);
// showing the current preview
  UpdatePreview();
}

void MapPreviewDialog::GetButtonBitmap(wxColour & color, wxBitmap & bmp)
{
// creating a Bitmap representing some Color
  bmp = wxBitmap(64, 24);
  wxMemoryDC *dc = new wxMemoryDC(bmp);
  dc->SetBrush(wxBrush(color));
  dc->DrawRectangle(-1, -1, 66, 26);
  delete dc;
}

void MapPreviewDialog::OnLineColor(wxCommandEvent & WXUNUSED(event))
{
// color selection
  wxColourData initColor;
  int ret;
  initColor.SetChooseFull(false);
  initColor.SetColour(LineColor);
  wxColourDialog colorDialog(this, &initColor);
  ret = colorDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxColourData colorData = colorDialog.GetColourData();
      wxColour color = colorData.GetColour();
      LineColor = wxColour(color.Red(), color.Green(), color.Blue());
      wxBitmap bmp;
      GetButtonBitmap(LineColor, bmp);
      LineColorCtrl->SetBitmapLabel(bmp);
    }
  UpdatePreview();
}

void MapPreviewDialog::OnFillColor(wxCommandEvent & WXUNUSED(event))
{
// color selection
  wxColourData initColor;
  int ret;
  initColor.SetChooseFull(false);
  initColor.SetColour(FillColor);
  wxColourDialog colorDialog(this, &initColor);
  ret = colorDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxColourData colorData = colorDialog.GetColourData();
      wxColour color = colorData.GetColour();
      FillColor = wxColour(color.Red(), color.Green(), color.Blue());
      wxBitmap bmp;
      GetButtonBitmap(FillColor, bmp);
      FillColorCtrl->SetBitmapLabel(bmp);
    }
  UpdatePreview();
}

void MapPreviewDialog::OnSymbolTypeChanged(wxCommandEvent & WXUNUSED(event))
{
// symbol type radiobox changed
  UpdatePreview();
}

void MapPreviewDialog::OnFillModeChanged(wxCommandEvent & WXUNUSED(event))
{
// fill mode radiobox changed
  UpdatePreview();
}

void MapPreviewDialog::OnSymbolSizeChanged(wxCommandEvent & WXUNUSED(event))
{
// symbol type spincontrol changed
  UpdatePreview();
}

void MapPreviewDialog::OnLineThicknessChanged(wxCommandEvent & WXUNUSED(event))
{
// line thickness spincontrol changed
  UpdatePreview();
}

void MapPreviewDialog::UpdatePreview()
{
// showing an empty preview map
  wxBitmap bmp;
  MapShow->SetBitmap(bmp);

// updating the Map Preview
  DoMap(256, 256, MAP_TARGET_IS_PREVIEW);
}

void MapPreviewDialog::DoMap(int hsize, int vsize, int target)
{
// drawing the map preview
  const void *gr = NULL;
  unsigned char *rgb_array = NULL;
  char table[1024];
  char column[1024];
  char sql[4196];
  int ret;
  char err_msg[2048];
  sqlite3_stmt *stmt;
  int horz;
  int vert;
  int h_shift;
  int v_shift;
  double ratio;
  double ext_x = MaxX - MinX;
  double ext_y = MaxY - MinY;
  bool circle;
  int symbol_size;
  bool fill;
  int thickness;

// computing the rendering ratio
  if (ext_x > ext_y)
    {
      ratio = ext_x / (double) (hsize - 20);
      if ((ext_y / ratio) > vsize)
        ratio = ext_y / (double) (vsize - 20);
  } else
    {
      ratio = ext_y / (double) (vsize - 20);
      if ((ext_x / ratio) > hsize)
        ratio = ext_x / (double) (hsize - 20);
    }
  horz = (int) (ext_x / ratio);
  vert = (int) (ext_y / ratio);
  horz += 20;
  vert += 20;
  h_shift = (hsize - horz) / 2;
  v_shift = (vsize - vert) / 2;

// retrieving current graphic settings
  switch (SymbolCtrl->GetSelection())
    {
      case 0:
        circle = true;
        break;
      case 1:
        circle = false;
        break;
    };
  switch (FillCtrl->GetSelection())
    {
      case 0:
        fill = true;
        break;
      case 1:
        fill = false;
        break;
    };
  symbol_size = SymSizeCtrl->GetValue();
  thickness = ThicknessCtrl->GetValue();

// graphics initialization
  if (target == MAP_TARGET_IS_SVG)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreateSvgContext(xpath, hsize, vsize, &gr);
  } else if (target == MAP_TARGET_IS_PDF)
    {
      char xpath[2024];
      strcpy(xpath, ExportPath.ToUTF8());
      gGraphCreatePdfContext(xpath, hsize + 100, vsize + 100, hsize, vsize,
                             &gr);
  } else
    gGraphCreateContext(hsize, vsize, &gr);

// background initialization
  gGraphSetBrush(gr, 255, 255, 255, 255);
  gGraphDrawRectangle(gr, -1, -1, hsize + 2, vsize + 2);

// setting standard graphic setting
  gGraphSetPen(gr, LineColor.Red(), LineColor.Green(), LineColor.Blue(), 255,
               thickness, GGRAPH_PENSTYLE_SOLID);
  gGraphSetBrush(gr, FillColor.Red(), FillColor.Green(), FillColor.Blue(), 255);

  ::wxBeginBusyCursor();
  strcpy(table, Table.ToUTF8());
  strcpy(column, Column.ToUTF8());
  MainFrame->DoubleQuotedSql(table);
  MainFrame->DoubleQuotedSql(column);
  sprintf(sql, "SELECT %s FROM %s", column, table);
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(err_msg, "SQL error: %s", sqlite3_errmsg(MainFrame->GetSqlite()));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      ::wxEndBusyCursor();
      return;
    }
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
          //
          // fetching a row
          //
          gaiaGeomCollPtr geom = NULL;
          if (sqlite3_column_type(stmt, 0) == SQLITE_BLOB)
            {
              // fetching Geometry
              int x;
              int y;
              const void *blob = sqlite3_column_blob(stmt, 0);
              int blob_size = sqlite3_column_bytes(stmt, 0);
              geom =
                gaiaFromSpatiaLiteBlobWkb((const unsigned char *) blob,
                                          blob_size);
              if (geom)
                {
                  // drawing the geometric entity
                  gaiaPolygonPtr pg = geom->FirstPolygon;
                  while (pg)
                    {
                      // drawing a POLYGON
                      int iv;
                      double dx;
                      double dy;
                      double z;
                      double m;
                      int x;
                      int y;
                      int lastX = 0;
                      int lastY = 0;
                      int ib;
                      gaiaRingPtr ring = pg->Exterior;
                      // exterior border
                      for (iv = 0; iv < ring->Points; iv++)
                        {
                          if (ring->DimensionModel == GAIA_XY_Z)
                            {
                              gaiaGetPointXYZ(ring->Coords, iv, &dx, &dy, &z);
                          } else if (ring->DimensionModel == GAIA_XY_M)
                            {
                              gaiaGetPointXYM(ring->Coords, iv, &dx, &dy, &m);
                          } else if (ring->DimensionModel == GAIA_XY_Z_M)
                            {
                              gaiaGetPointXYZM(ring->Coords, iv, &dx, &dy, &z,
                                               &m);
                          } else
                            {
                              gaiaGetPoint(ring->Coords, iv, &dx, &dy);
                            }
                          x = (int) ((dx - MinX) / ratio);
                          y = vert - (int) ((dy - MinY) / ratio);
                          x += h_shift;
                          y += v_shift;
                          if (iv == 0)
                            {
                              gGraphMoveToPoint(gr, x, y);
                              lastX = x;
                              lastY = y;
                          } else
                            {
                              if (x == lastX && y == lastY)
                                ;
                              else
                                {
                                  gGraphAddLineToPath(gr, x, y);
                                  lastX = x;
                                  lastY = y;
                                }
                            }
                        }
                      gGraphCloseSubpath(gr);
                      for (ib = 0; ib < pg->NumInteriors; ib++)
                        {
                          // interior borders
                          ring = pg->Interiors + ib;
                          for (iv = 0; iv < ring->Points; iv++)
                            {
                              if (ring->DimensionModel == GAIA_XY_Z)
                                {
                                  gaiaGetPointXYZ(ring->Coords, iv, &dx, &dy,
                                                  &z);
                              } else if (ring->DimensionModel == GAIA_XY_M)
                                {
                                  gaiaGetPointXYM(ring->Coords, iv, &dx, &dy,
                                                  &m);
                              } else if (ring->DimensionModel == GAIA_XY_Z_M)
                                {
                                  gaiaGetPointXYZM(ring->Coords, iv, &dx, &dy,
                                                   &z, &m);
                              } else
                                {
                                  gaiaGetPoint(ring->Coords, iv, &dx, &dy);
                                }
                              x = (int) ((dx - MinX) / ratio);
                              y = vert - (int) ((dy - MinY) / ratio);
                              x += h_shift;
                              y += v_shift;
                              if (iv == 0)
                                {
                                  gGraphMoveToPoint(gr, x, y);
                                  lastX = x;
                                  lastY = y;
                              } else
                                {
                                  if (x == lastX && y == lastY)
                                    ;
                                  else
                                    {
                                      gGraphAddLineToPath(gr, x, y);
                                      lastX = x;
                                      lastY = y;
                                    }
                                }
                            }
                          gGraphCloseSubpath(gr);
                        }
                      if (fill == true)
                        gGraphFillPath(gr, GGRAPH_PRESERVE_PATH);
                      gGraphStrokePath(gr, GGRAPH_CLEAR_PATH);
                      pg = pg->Next;
                    }
                  gaiaLinestringPtr ln = geom->FirstLinestring;
                  while (ln)
                    {
                      // drawing a LINESTRING
                      int iv;
                      double dx;
                      double dy;
                      double z;
                      double m;
                      int x;
                      int y;
                      int lastX = 0;
                      int lastY = 0;
                      for (iv = 0; iv < ln->Points; iv++)
                        {
                          if (ln->DimensionModel == GAIA_XY_Z)
                            {
                              gaiaGetPointXYZ(ln->Coords, iv, &dx, &dy, &z);
                          } else if (ln->DimensionModel == GAIA_XY_M)
                            {
                              gaiaGetPointXYM(ln->Coords, iv, &dx, &dy, &m);
                          } else if (ln->DimensionModel == GAIA_XY_Z_M)
                            {
                              gaiaGetPointXYZM(ln->Coords, iv, &dx, &dy, &z,
                                               &m);
                          } else
                            {
                              gaiaGetPoint(ln->Coords, iv, &dx, &dy);
                            }
                          x = (int) ((dx - MinX) / ratio);
                          y = vert - (int) ((dy - MinY) / ratio);
                          x += h_shift;
                          y += v_shift;
                          if (iv == 0)
                            {
                              gGraphMoveToPoint(gr, x, y);
                              lastX = x;
                              lastY = y;
                          } else
                            {
                              if (x == lastX && y == lastY)
                                ;
                              else
                                {
                                  gGraphAddLineToPath(gr, x, y);
                                  lastX = x;
                                  lastY = y;
                                }
                            }
                        }
                      gGraphStrokePath(gr, GGRAPH_CLEAR_PATH);
                      ln = ln->Next;
                    }
                  gaiaPointPtr pt = geom->FirstPoint;
                  while (pt)
                    {
                      // drawing a POINT
                      x = (int) ((pt->X - MinX) / ratio);
                      y = vsize - (int) ((pt->Y - MinY) / ratio);
                      x += h_shift;
                      y += v_shift;
                      if (circle == false)
                        gGraphDrawRectangle(gr, x - symbol_size,
                                            y - symbol_size, symbol_size * 2,
                                            symbol_size * 2);
                      else
                        gGraphDrawEllipse(gr, x - symbol_size, y - symbol_size,
                                          symbol_size * 2, symbol_size * 2);
                      pt = pt->Next;
                    }
                  gaiaFreeGeomColl(geom);
                }
            }
      } else
        {
          sqlite3_finalize(stmt);
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
    }
  sqlite3_finalize(stmt);
  ::wxEndBusyCursor();

// graphics finalization
  if (target == MAP_TARGET_IS_COPY || target == MAP_TARGET_IS_PNG
      || target == MAP_TARGET_IS_PREVIEW)
    gGraphGetContextRgbArray(gr, &rgb_array);
  if (target == MAP_TARGET_IS_SVG)
    gGraphDestroySvgContext(gr);
  else if (target == MAP_TARGET_IS_PDF)
    gGraphDestroyPdfContext(gr);
  else
    gGraphDestroyContext(gr);

  if (target == MAP_TARGET_IS_PNG)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          const void *img =
            gGraphCreateRgbImageFromBitmap(rgb_array, hsize, vsize);
          if (img)
            {
              char xpath[2024];
              strcpy(xpath, ExportPath.ToUTF8());
              if (gGraphImageToPngFile(img, xpath, 6, 0, 0) != GGRAPH_OK)
                {
                  wxString msg =
                    wxT
                    ("An error occurred while saving\nthe current Map as PNG");
                  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               MainFrame);
                }
              gGraphDestroyImage(img);
            }
        }
    }
  if (target == MAP_TARGET_IS_PREVIEW)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          wxBitmap bmp(img);
          MapShow->SetBitmap(bmp);
        }
    }
  if (target == MAP_TARGET_IS_COPY)
    {
      if (rgb_array)
        {
          // creating the Image from RGB array
          wxImage img(hsize, vsize);
          img.SetData(rgb_array);
          if (wxTheClipboard->Open())
            {
              wxTheClipboard->SetData(new wxBitmapDataObject(img));
              wxTheClipboard->Close();
            }
        }
    }
}

void MapPreviewDialog::OnMapCopy(wxCommandEvent & WXUNUSED(event))
{
// copying the current Map to the Clipboard 
  int hsize;
  int vsize;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        break;
    };

// exporting the Map as COPY
  DoMap(hsize, vsize, MAP_TARGET_IS_COPY);
}

void MapPreviewDialog::OnMapPng(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Chart as PNG
  int hsize;
  int vsize;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        break;
    };

// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("PNG Image (*.png)|*.png");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Map as PNG"),
                          wxT(""), wxT("MapExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".png");
      ExportPath = path;
  } else
    return;

// exporting the Map as PNG
  DoMap(hsize, vsize, MAP_TARGET_IS_PNG);
}

void MapPreviewDialog::OnMapSvg(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Map as SVG 
  int hsize;
  int vsize;
  switch (SizeCtrl->GetSelection())
    {
      case 0:
        hsize = 256;
        vsize = 256;
        break;
      case 1:
        hsize = 512;
        vsize = 512;
        break;
      case 2:
        hsize = 1024;
        vsize = 1024;
        break;
      case 3:
        hsize = 2048;
        vsize = 2048;
        break;
      case 4:
        hsize = 4196;
        vsize = 4196;
        break;
    };

// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("SVG Vector Image (*.svg)|*.svg");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Map as SVG"),
                          wxT(""), wxT("MapExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".svg");
      ExportPath = path;
  } else
    return;


// exporting the Map as SVG
  DoMap(hsize, vsize, MAP_TARGET_IS_SVG);
}

void MapPreviewDialog::OnMapPdf(wxCommandEvent & WXUNUSED(event))
{
// exporting the current Map as PDF
  double ext_x = MaxX - MinX;
  double ext_y = MaxY - MinY;
  int hsize = (int) (12.8 * 300.0);
  int vsize = (int) (9.0 * 300.0);
  hsize -= 100;                 // margin
  vsize -= 100;                 // margin

  if (ext_y > ext_x)
    {
      // portrait, not landscape orientation
      int swap = hsize;
      hsize = vsize;
      vsize = swap;
    }
// asking an export path
  int ret;
  wxString lastDir;
  wxString fileList = wxT("PDF Document (*.pdf)|*.pdf");
  wxFileDialog fileDialog(MainFrame, wxT("saving the current Map as PDF"),
                          wxT(""), wxT("MapExport"), fileList,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                          wxDefaultPosition, wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxFileName file(fileDialog.GetPath());
      lastDir = file.GetPath();
      MainFrame->SetLastDirectory(lastDir);
      wxString path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".pdf");
      ExportPath = path;
  } else
    return;


// exporting the Map as PDF
  DoMap(hsize, vsize, MAP_TARGET_IS_PDF);
}

void MapPreviewDialog::OnExit(wxCommandEvent & WXUNUSED(event))
{
//
// all done: 
//
  wxDialog::EndModal(wxID_OK);
}
