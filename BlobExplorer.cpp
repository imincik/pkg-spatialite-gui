/*
/ BlobExplorer.cpp
/ a dialog to explore a BLOB value
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

#include "wx/mstream.h"
#include "wx/clipbrd.h"
#include "wx/tokenzr.h"
#include "wx/spinctrl.h"

#include "gaiagraphics.h"

BlobExplorerDialog::BlobExplorerDialog(MyFrame * parent, int blob_size,
                                       unsigned char *blob)
{
//
// constructor; just calls Create()
//
  Create(parent, blob_size, blob);
}

bool BlobExplorerDialog::Create(MyFrame * parent, int blob_size,
                                unsigned char *blob)
{
//
// creating the dialog
//
  MainFrame = parent;
  BlobSize = blob_size;
  Blob = blob;
  BlobType = gaiaGuessBlobType(Blob, BlobSize);
  Geometry = NULL;
  IsSVG = false;
  XMLDocument = wxT("");
  XMLIndented = wxT("");
  Image = NULL;
  SVGrelative = true;
  SVGprecision = -1;
  KMLprecision = -1;
  GMLv2v3 = true;
  GMLprecision = -1;
  GeoJSONoptions = 0;
  GeoJSONprecision = -1;
  const void *img = NULL;
  unsigned char *rgbaArray = NULL;
  int width;
  int height;
  bool isRasterLiteRaw = false;

  if (BlobType == GAIA_GEOMETRY_BLOB)
    Geometry = gaiaFromSpatiaLiteBlobWkb(Blob, BlobSize);
  else if (BlobType == GAIA_XML_BLOB)
    {
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */

      char *xml = gaiaXmlTextFromBlob(Blob, BlobSize, -1);
      if (xml != NULL)
        {
          XMLDocument = wxString::FromUTF8(xml);
          free(xml);
        }
      xml = gaiaXmlTextFromBlob(Blob, BlobSize, 4);
      if (xml != NULL)
        {
          XMLIndented = wxString::FromUTF8(xml);
          free(xml);
        }
      if (gaiaIsSvgXmlBlob(Blob, BlobSize))
        {
          /* handling the SVG preview */
          char *svg = gaiaXmlTextFromBlob(Blob, BlobSize, 0);
          int svg_sz = strlen(svg);
          IsSVG = true;
          SvgSize = svg_sz;
          void *svg_handle;
          img = NULL;
          if (gGraphCreateSVG((const unsigned char *) svg, svg_sz, &svg_handle)
              == GGRAPH_OK)

            {
              gGraphGetSVGDims(svg_handle, &SvgWidth, &SvgHeight);
              double w = SvgWidth;
              double h = SvgHeight;
              while (w > 560.0 || h > 300.0)
                {
                  /* rescaling */
                  w *= 0.9;
                  h *= 0.9;
                }
              double sz = w;
              if (h > sz)
                sz = h;
              if (gGraphImageFromSVG(svg_handle, sz, &img) != GGRAPH_OK)
                img = NULL;
              gGraphFreeSVG(svg_handle);
            }
          free(svg);
        }
#endif /* end LIBXML2 conditionals */
  } else
    {
      switch (BlobType)
        {
          case GAIA_JPEG_BLOB:
          case GAIA_EXIF_BLOB:
          case GAIA_EXIF_GPS_BLOB:
            if (gGraphImageFromMemBuf
                (Blob, BlobSize, GGRAPH_IMAGE_JPEG, &img, 1) != GGRAPH_OK)
              img = NULL;
            break;
          case GAIA_PNG_BLOB:
            if (gGraphImageFromMemBuf(Blob, BlobSize, GGRAPH_IMAGE_PNG, &img, 1)
                != GGRAPH_OK)
              img = NULL;
            break;
          case GAIA_GIF_BLOB:
            if (gGraphImageFromMemBuf(Blob, BlobSize, GGRAPH_IMAGE_GIF, &img, 1)
                != GGRAPH_OK)
              img = NULL;
            break;
          case GAIA_TIFF_BLOB:
            if (gGraphImageFromMemBuf
                (Blob, BlobSize, GGRAPH_IMAGE_TIFF, &img, 1) != GGRAPH_OK)
              img = NULL;
            break;
          default:
            if (gGraphIsRawImage(Blob, BlobSize) == GGRAPH_OK)
              {
                /* this one is a RasterLite RAW image */
                if (gGraphImageFromRawMemBuf(Blob, BlobSize, &img) != GGRAPH_OK)
                  img = NULL;
                else
                  isRasterLiteRaw = true;
              }
            break;
        };
    }
  if (img)
    {
      if (gGraphGetImageDims(img, &width, &height) == GGRAPH_OK)
        {
          if (gGraphImageBufferReferenceRGBA(img, &rgbaArray) != GGRAPH_OK)
            rgbaArray = NULL;
        }
    }
  if (rgbaArray)
    {
      // creating the Image from RGB array
      int x;
      int y;
      Image = new wxImage(width, height);
      unsigned char *p = rgbaArray;
      Image->SetAlpha();
      for (y = 0; y < height; y++)
        {
          for (x = 0; x < width; x++)
            {
              unsigned char r = *p++;
              unsigned char g = *p++;
              unsigned char b = *p++;
              unsigned char alpha = *p++;
              Image->SetRGB(x, y, r, g, b);
              Image->SetAlpha(x, y, alpha);
            }
        }
      free(rgbaArray);
  } else
    {
      // creating a default BLACK Image
      int x;
      int y;
      Image = new wxImage(128, 128);
      for (y = 0; y < 128; y++)
        {
          for (x = 0; x < 128; x++)
            Image->SetRGB(x, y, 0, 0, 0);
        }
    }
  if (img)
    gGraphDestroyImage(img);

  if (wxPropertySheetDialog::Create(parent, wxID_ANY, wxT("BLOB explorer")) ==
      false)
    return false;
  wxBookCtrlBase *book = GetBookCtrl();
// creates individual panels
  wxPanel *hexadecimal = CreateHexadecimalPage(book);
  book->AddPage(hexadecimal, wxT("Hexadecimal dump"), true);
  if (BlobType == GAIA_GEOMETRY_BLOB)
    {
      wxPanel *geometry = CreateGeometryPage(book);
      book->AddPage(geometry, wxT("Geometry explorer"), false);
      wxPanel *wkt = CreateWKTPage(book);
      book->AddPage(wkt, wxT("WKT"), false);
      wxPanel *ewkt = CreateEWKTPage(book);
      book->AddPage(ewkt, wxT("EWKT"), false);
      wxPanel *svg = CreateSVGPage(book);
      book->AddPage(svg, wxT("SVG"), false);
      wxPanel *kml = CreateKMLPage(book);
      book->AddPage(kml, wxT("KML"), false);
      wxPanel *gml = CreateGMLPage(book);
      book->AddPage(gml, wxT("GML"), false);
      wxPanel *geoJson = CreateGeoJSONPage(book);
      book->AddPage(geoJson, wxT("GeoJSON"), false);
    }

  if (BlobType == GAIA_XML_BLOB)
    {
      wxPanel *xml_document = CreateXmlDocumentPage(book);
      book->AddPage(xml_document, wxT("XML Document"), false);
      wxPanel *xml_indented = CreateXmlIndentedPage(book);
      book->AddPage(xml_indented, wxT("XML (indented)"), false);
    }

  if (BlobType == GAIA_JPEG_BLOB || BlobType == GAIA_EXIF_BLOB
      || BlobType == GAIA_EXIF_GPS_BLOB || BlobType == GAIA_PNG_BLOB
      || BlobType == GAIA_GIF_BLOB || BlobType == GAIA_TIFF_BLOB
      || isRasterLiteRaw == true || IsSVG == true)
    {
      wxPanel *image = CreateImagePage(book);
      book->AddPage(image, wxT("Image"), false);
    }
  CreateButtons(wxOK);
  LayoutDialog();
// appends event handler for TAB/PAGE changing
  Connect(wxID_ANY, wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnPageChanged);
// appends event handler for OK button
  Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnOk);
// centers the dialog window
  Centre();
  UpdateHexadecimalPage();
  return true;
}

wxPanel *BlobExplorerDialog::CreateHexadecimalPage(wxWindow * parent)
{
//
// creating the HEXADECIMAL page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_HEXADECIMAL);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the hexadecimal dump
  wxBoxSizer *hexSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(hexSizer, 0, wxALIGN_LEFT | wxALL, 0);
  MyHexList *hexCtrl = new MyHexList(this, Blob, BlobSize, panel,
                                     ID_HEX, wxDefaultPosition,
                                     wxSize(620, 320),
                                     wxLC_REPORT | wxLC_VIRTUAL);
  wxFont font(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  hexCtrl->SetFont(font);
  wxListItem column0;
  hexCtrl->InsertColumn(0, wxT("Address"));
  hexCtrl->SetColumnWidth(0, 90);
  hexCtrl->InsertColumn(1, wxT("Hexadecimal"));
  hexCtrl->SetColumnWidth(1, 370);
  hexCtrl->InsertColumn(2, wxT("ASCII"));
  hexCtrl->SetColumnWidth(2, 130);
  hexSizer->Add(hexCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
  return panel;
}

wxPanel *BlobExplorerDialog::CreateGeometryPage(wxWindow * parent)
{
//
// creating the GEOMETRY page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_HEXADECIMAL);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the geometry as a text table
  wxBoxSizer *geomSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(geomSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *geomCtrl = new wxTextCtrl(panel, ID_GEOM_TABLE, wxT(""),
                                        wxDefaultPosition, wxSize(270,
                                                                  320),
                                        wxTE_MULTILINE | wxTE_RICH |
                                        wxTE_READONLY | wxHSCROLL);
  geomSizer->Add(geomCtrl, 0, wxALIGN_LEFT | wxALL, 5);
// creating a control to show the geometry in a graphical fashion
  wxStaticBox *exBox = new wxStaticBox(panel, wxID_STATIC,
                                       wxT("Geometry preview"),
                                       wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *exampleSizer = new wxStaticBoxSizer(exBox, wxHORIZONTAL);
  geomSizer->Add(exampleSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  DrawGeometry(300, 300);
  GraphicsGeometry *geomGraph = new GraphicsGeometry(this, panel, ID_GEOM_GRAPH,
                                                     GeomPreview, wxSize(300,
                                                                         300));
  exampleSizer->Add(geomGraph, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
  return panel;
}

wxPanel *BlobExplorerDialog::CreateWKTPage(wxWindow * parent)
{
//
// creating the WKT page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_WKT);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the WKT notation
  wxBoxSizer *wktSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(wktSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *wktCtrl = new wxTextCtrl(panel, ID_WKT_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 300),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wktCtrl->SetFont(font);
  wktSizer->Add(wktCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *btnCopy = new wxButton(panel, ID_WKT_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_WKT_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyWKT);
  return panel;
}

void BlobExplorerDialog::OnCopyWKT(wxCommandEvent & WXUNUSED(event))
{
//
// Copying WKT to the Clipboard 
//
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(WKTstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateEWKTPage(wxWindow * parent)
{
//
// creating the EWKT page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_EWKT);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the EWKT notation
  wxBoxSizer *ewktSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(ewktSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *ewktCtrl = new wxTextCtrl(panel, ID_EWKT_TABLE, wxT(""),
                                        wxDefaultPosition, wxSize(600,
                                                                  300),
                                        wxTE_MULTILINE | wxTE_READONLY |
                                        wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  ewktCtrl->SetFont(font);
  ewktSizer->Add(ewktCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *btnCopy = new wxButton(panel, ID_EWKT_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_EWKT_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyEWKT);
  return panel;
}

void BlobExplorerDialog::OnCopyEWKT(wxCommandEvent & WXUNUSED(event))
{
//
// Copying EWKT to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(EWKTstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateSVGPage(wxWindow * parent)
{
//
// creating the SVG page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_SVG);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the SVG notation
  wxBoxSizer *svgSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(svgSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *svgCtrl = new wxTextCtrl(panel, ID_SVG_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 270),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  svgCtrl->SetFont(font);
  svgSizer->Add(svgCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *optBox = new wxStaticBox(panel, wxID_STATIC,
                                        wxT("SVG options"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxHORIZONTAL);
  btnSizer->Add(optSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxBoxSizer *absRelSizer = new wxBoxSizer(wxHORIZONTAL);
  optSizer->Add(absRelSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString absrel[2];
  absrel[0] = wxT("&Relative");
  absrel[1] = wxT("&Absolute");
  wxRadioBox *absRelBox = new wxRadioBox(panel, ID_SVG_RELATIVE,
                                         wxT("&Coord Mode"),
                                         wxDefaultPosition,
                                         wxDefaultSize, 2,
                                         absrel, 2,
                                         wxRA_SPECIFY_COLS);
  absRelSizer->Add(absRelBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *precisionLabel =
    new wxStaticText(panel, wxID_STATIC, wxT("&Precision:"));
  optSizer->Add(precisionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *precisionCtrl =
    new wxSpinCtrl(panel, ID_SVG_PRECISION, wxEmptyString,
                   wxDefaultPosition, wxSize(50, 20),
                   wxSP_ARROW_KEYS,
                   -1, 15, -1);
  optSizer->Add(precisionCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *btnCopy = new wxButton(panel, ID_SVG_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_SVG_RELATIVE, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnSVGRelative);
  Connect(ID_SVG_PRECISION, wxEVT_COMMAND_SPINCTRL_UPDATED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnSVGPrecision);
  Connect(ID_SVG_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopySVG);
  return panel;
}

void BlobExplorerDialog::OnSVGRelative(wxCommandEvent & WXUNUSED(event))
{
//
// SVG Relative/Absolute selection changed
//
  wxRadioBox *absRelCtrl = (wxRadioBox *) FindWindow(ID_SVG_RELATIVE);
  switch (absRelCtrl->GetSelection())
    {
      case 0:
        SVGrelative = true;
        break;
      case 1:
        SVGrelative = false;
        break;
    };
  SVGstring = wxT("");
  wxTextCtrl *svgCtrl = (wxTextCtrl *) FindWindow(ID_SVG_TABLE);
  svgCtrl->SetValue(wxT(""));
  UpdateSVGPage();
}

void BlobExplorerDialog::OnSVGPrecision(wxCommandEvent & WXUNUSED(event))
{
//
// SVG Precision selection changed
//
  wxSpinCtrl *precisionCtrl = (wxSpinCtrl *) FindWindow(ID_SVG_PRECISION);
  SVGprecision = precisionCtrl->GetValue();
  SVGstring = wxT("");
  wxTextCtrl *svgCtrl = (wxTextCtrl *) FindWindow(ID_SVG_TABLE);
  svgCtrl->SetValue(wxT(""));
  UpdateSVGPage();
}

void BlobExplorerDialog::OnCopySVG(wxCommandEvent & WXUNUSED(event))
{
//
// Copying SVG to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(SVGstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateKMLPage(wxWindow * parent)
{
//
// creating the KML page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_KML);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the KML notation
  wxBoxSizer *kmlSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(kmlSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *kmlCtrl = new wxTextCtrl(panel, ID_KML_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 280),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  kmlCtrl->SetFont(font);
  kmlSizer->Add(kmlCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *optBox = new wxStaticBox(panel, wxID_STATIC,
                                        wxT("KML options"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxHORIZONTAL);
  btnSizer->Add(optSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxStaticText *precisionLabel =
    new wxStaticText(panel, wxID_STATIC, wxT("&Precision:"));
  optSizer->Add(precisionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *precisionCtrl =
    new wxSpinCtrl(panel, ID_KML_PRECISION, wxEmptyString,
                   wxDefaultPosition, wxSize(50, 20),
                   wxSP_ARROW_KEYS,
                   -1, 18, -1);
  optSizer->Add(precisionCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *btnCopy = new wxButton(panel, ID_KML_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_KML_PRECISION, wxEVT_COMMAND_SPINCTRL_UPDATED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnKMLPrecision);
  Connect(ID_KML_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyKML);
  return panel;
}

void BlobExplorerDialog::OnKMLPrecision(wxCommandEvent & WXUNUSED(event))
{
//
// KML Precision selection changed
//
  wxSpinCtrl *precisionCtrl = (wxSpinCtrl *) FindWindow(ID_KML_PRECISION);
  KMLprecision = precisionCtrl->GetValue();
  KMLstring = wxT("");
  wxTextCtrl *kmlCtrl = (wxTextCtrl *) FindWindow(ID_KML_TABLE);
  kmlCtrl->SetValue(wxT(""));
  UpdateKMLPage();
}

void BlobExplorerDialog::OnCopyKML(wxCommandEvent & WXUNUSED(event))
{
//
// Copying KML to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(KMLstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateGMLPage(wxWindow * parent)
{
//
// creating the GML page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_GML);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the GML notation
  wxBoxSizer *gmlSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(gmlSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *gmlCtrl = new wxTextCtrl(panel, ID_GML_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 270),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  gmlCtrl->SetFont(font);
  gmlSizer->Add(gmlCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *optBox = new wxStaticBox(panel, wxID_STATIC,
                                        wxT("GML options"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxHORIZONTAL);
  btnSizer->Add(optSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxBoxSizer *versionSizer = new wxBoxSizer(wxHORIZONTAL);
  optSizer->Add(versionSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString version[2];
  version[0] = wxT("&GML v2");
  version[1] = wxT("&GML v3");
  wxRadioBox *versionBox = new wxRadioBox(panel, ID_GML_V2_V3,
                                          wxT("&GML version"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 2,
                                          version, 2,
                                          wxRA_SPECIFY_COLS);
  versionSizer->Add(versionBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *precisionLabel =
    new wxStaticText(panel, wxID_STATIC, wxT("&Precision:"));
  optSizer->Add(precisionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *precisionCtrl =
    new wxSpinCtrl(panel, ID_GML_PRECISION, wxEmptyString,
                   wxDefaultPosition, wxSize(50, 20),
                   wxSP_ARROW_KEYS,
                   -1, 18, -1);
  optSizer->Add(precisionCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *btnCopy = new wxButton(panel, ID_GML_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_GML_V2_V3, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnGMLv2v3);
  Connect(ID_GML_PRECISION, wxEVT_COMMAND_SPINCTRL_UPDATED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnGMLPrecision);
  Connect(ID_GML_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyGML);
  return panel;
}

void BlobExplorerDialog::OnGMLv2v3(wxCommandEvent & WXUNUSED(event))
{
//
// GML Version selection changed
//
  wxRadioBox *versionCtrl = (wxRadioBox *) FindWindow(ID_GML_V2_V3);
  switch (versionCtrl->GetSelection())
    {
      case 0:
        GMLv2v3 = true;
        break;
      case 1:
        GMLv2v3 = false;
        break;
    };
  GMLstring = wxT("");
  wxTextCtrl *gmlCtrl = (wxTextCtrl *) FindWindow(ID_GML_TABLE);
  gmlCtrl->SetValue(wxT(""));
  UpdateGMLPage();
}

void BlobExplorerDialog::OnGMLPrecision(wxCommandEvent & WXUNUSED(event))
{
//
// GML Precision selection changed
//
  wxSpinCtrl *precisionCtrl = (wxSpinCtrl *) FindWindow(ID_GML_PRECISION);
  GMLprecision = precisionCtrl->GetValue();
  GMLstring = wxT("");
  wxTextCtrl *gmlCtrl = (wxTextCtrl *) FindWindow(ID_GML_TABLE);
  gmlCtrl->SetValue(wxT(""));
  UpdateGMLPage();
}

void BlobExplorerDialog::OnCopyGML(wxCommandEvent & WXUNUSED(event))
{
//
// Copying GML to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(GMLstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateGeoJSONPage(wxWindow * parent)
{
//
// creating the GeoJSON page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_SVG);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the GeoJSON notation
  wxBoxSizer *geoJsonSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(geoJsonSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *geoJsonCtrl = new wxTextCtrl(panel, ID_GEOJSON_TABLE, wxT(""),
                                           wxDefaultPosition, wxSize(600,
                                                                     270),
                                           wxTE_MULTILINE | wxTE_READONLY |
                                           wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  geoJsonCtrl->SetFont(font);
  geoJsonSizer->Add(geoJsonCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxStaticBox *optBox = new wxStaticBox(panel, wxID_STATIC,
                                        wxT("GeoJSON options"),
                                        wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *optSizer = new wxStaticBoxSizer(optBox, wxHORIZONTAL);
  btnSizer->Add(optSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxBoxSizer *optionsSizer = new wxBoxSizer(wxHORIZONTAL);
  optSizer->Add(optionsSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxString options[6];
  options[0] = wxT("&none");
  options[1] = wxT("&BBOX");
  options[2] = wxT("&shortCRS");
  options[3] = wxT("&shortCRS + BBOX");
  options[4] = wxT("&longCRS");
  options[5] = wxT("&longCRS + BBOX");
  wxRadioBox *optionsBox = new wxRadioBox(panel, ID_GEOJSON_OPTIONS,
                                          wxT("&BBOX - CRS options"),
                                          wxDefaultPosition,
                                          wxDefaultSize, 6,
                                          options, 2,
                                          wxRA_SPECIFY_ROWS);
  optionsSizer->Add(optionsBox, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  wxStaticText *precisionLabel =
    new wxStaticText(panel, wxID_STATIC, wxT("&Precision:"));
  optSizer->Add(precisionLabel, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxSpinCtrl *precisionCtrl =
    new wxSpinCtrl(panel, ID_GEOJSON_PRECISION, wxEmptyString,
                   wxDefaultPosition, wxSize(50, 20),
                   wxSP_ARROW_KEYS,
                   -1, 18, -1);
  optSizer->Add(precisionCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  wxButton *btnCopy = new wxButton(panel, ID_GEOJSON_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_GEOJSON_OPTIONS, wxEVT_COMMAND_RADIOBOX_SELECTED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnGeoJSONOptions);
  Connect(ID_GEOJSON_PRECISION, wxEVT_COMMAND_SPINCTRL_UPDATED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnGeoJSONPrecision);
  Connect(ID_GEOJSON_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyGeoJSON);
  return panel;
}

void BlobExplorerDialog::OnGeoJSONOptions(wxCommandEvent & WXUNUSED(event))
{
//
// GeoJSON Options selection changed
//
  wxRadioBox *optionsCtrl = (wxRadioBox *) FindWindow(ID_GEOJSON_OPTIONS);
  GeoJSONoptions = optionsCtrl->GetSelection();
  GMLstring = wxT("");
  wxTextCtrl *gmlCtrl = (wxTextCtrl *) FindWindow(ID_GEOJSON_TABLE);
  gmlCtrl->SetValue(wxT(""));
  UpdateGeoJSONPage();
}

void BlobExplorerDialog::OnGeoJSONPrecision(wxCommandEvent & WXUNUSED(event))
{
//
// GeoJSON Precision selection changed
//
  wxSpinCtrl *precisionCtrl = (wxSpinCtrl *) FindWindow(ID_GEOJSON_PRECISION);
  GeoJSONprecision = precisionCtrl->GetValue();
  GeoJSONstring = wxT("");
  wxTextCtrl *geoJsonCtrl = (wxTextCtrl *) FindWindow(ID_GEOJSON_TABLE);
  geoJsonCtrl->SetValue(wxT(""));
  UpdateGeoJSONPage();
}

void BlobExplorerDialog::OnCopyGeoJSON(wxCommandEvent & WXUNUSED(event))
{
//
// Copying GeoJSON to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(GeoJSONstring));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateXmlDocumentPage(wxWindow * parent)
{
//
// creating the XMLDocument page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_WKT);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the WKT notation
  wxBoxSizer *wktSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(wktSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *wktCtrl = new wxTextCtrl(panel, ID_XML_DOCUMENT_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 300),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wktCtrl->SetFont(font);
  wktSizer->Add(wktCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *btnCopy = new wxButton(panel, ID_XML_DOCUMENT_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_XML_DOCUMENT_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyXmlDocument);
  return panel;
}

void BlobExplorerDialog::OnCopyXmlDocument(wxCommandEvent & WXUNUSED(event))
{
//
// Copying XMLDocument to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(XMLDocument));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateXmlIndentedPage(wxWindow * parent)
{
//
// creating the XMLIndented page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_WKT);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the WKT notation
  wxBoxSizer *wktSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(wktSizer, 0, wxALIGN_LEFT | wxALL, 0);
  wxTextCtrl *wktCtrl = new wxTextCtrl(panel, ID_XML_INDENTED_TABLE, wxT(""),
                                       wxDefaultPosition, wxSize(600,
                                                                 300),
                                       wxTE_MULTILINE | wxTE_READONLY |
                                       wxTE_RICH | wxVSCROLL);
  wxFont font =
    wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  wktCtrl->SetFont(font);
  wktSizer->Add(wktCtrl, 0, wxALIGN_LEFT | wxALL, 5);
  wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
  boxSizer->Add(btnSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 0);
  wxButton *btnCopy = new wxButton(panel, ID_XML_INDENTED_COPY, wxT("&Copy"));
  btnSizer->Add(btnCopy, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
// appends event handlers
  Connect(ID_XML_INDENTED_COPY, wxEVT_COMMAND_BUTTON_CLICKED,
          (wxObjectEventFunction) & BlobExplorerDialog::OnCopyXmlIndented);
  return panel;
}

void BlobExplorerDialog::OnCopyXmlIndented(wxCommandEvent & WXUNUSED(event))
{
//
// Copying XMLDocument to the Clipboard 
//

  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxTextDataObject(XMLIndented));
      wxTheClipboard->Close();
    }
}

wxPanel *BlobExplorerDialog::CreateImagePage(wxWindow * parent)
{
//
// creating the IMAGE page
//
  wxPanel *panel = new wxPanel(parent, ID_PANE_IMAGE);
  wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
  panel->SetSizer(topSizer);
  wxBoxSizer *boxSizer = new wxBoxSizer(wxVERTICAL);
  topSizer->Add(boxSizer, 0, wxALIGN_CENTER | wxALL, 5);
// creating a control to show the image title
  wxBoxSizer *imgSizer = new wxBoxSizer(wxVERTICAL);
  boxSizer->Add(imgSizer, 0, wxALIGN_TOP | wxALL, 0);
  wxStaticText *imageTitle = new wxStaticText(panel, ID_IMAGE_TITLE,
                                              wxT("Image"),
                                              wxDefaultPosition,
                                              wxSize(560,
                                                     10));
  imgSizer->Add(imageTitle, 0, wxALIGN_LEFT | wxALL, 5);
// creating a control to show the image
  wxStaticBox *exBox = new wxStaticBox(panel, ID_IMG_BOX,
                                       wxT("Image preview"),
                                       wxDefaultPosition, wxDefaultSize);
  wxBoxSizer *exampleSizer = new wxStaticBoxSizer(exBox, wxHORIZONTAL);
  imgSizer->Add(exampleSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  ImageShow *imgShow = new ImageShow(this, panel, ID_IMAGE,
                                     wxBitmap(), wxSize(560, 300));
  exampleSizer->Add(imgShow, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0);
  panel->SetSizer(topSizer);
  topSizer->Fit(panel);
  return panel;
}

void BlobExplorerDialog::OnPageChanged(wxNotebookEvent & event)
{
//
// TAB/PAGE selection changed
//
  switch (event.GetSelection())
    {
      case 0:
        UpdateHexadecimalPage();
        break;
      case 1:
        if (BlobType == GAIA_GEOMETRY_BLOB)
          UpdateGeometryPage();
        else if (BlobType == GAIA_XML_BLOB)
          UpdateXmlDocumentPage();
        else
          UpdateImagePage();
        break;
      case 2:
        if (BlobType == GAIA_XML_BLOB)
          UpdateXmlIndentedPage();
        else
          UpdateWKTPage();
        break;
      case 3:
        if (IsSVG == true)
          UpdateImagePage();
        else
          UpdateEWKTPage();
        break;
      case 4:
        UpdateSVGPage();
        break;
      case 5:
        UpdateKMLPage();
        break;
      case 6:
        UpdateGMLPage();
        break;
      case 7:
        UpdateGeoJSONPage();
        break;
    };
}

void BlobExplorerDialog::UpdateHexadecimalPage()
{
//
// updating the HEXADECIMAL page
//
  MyHexList *hexCtrl = (MyHexList *) FindWindow(ID_HEX);
  hexCtrl->EnsureVisible(0);
}

void BlobExplorerDialog::UpdateGeometryPage()
{
//
// updating the GEOMETRY page
//
  gaiaPointPtr pt;
  gaiaLinestringPtr ln;
  gaiaPolygonPtr pg;
  gaiaRingPtr rng;
  int points = 0;
  int linestrings = 0;
  int polygons = 0;
  int ib;
  wxString strValue;
  char dummy[1024];
  wxTextAttr attrBold(wxColour(0, 0, 0), wxColour(255, 255, 255),
                      wxFont(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                             wxFONTWEIGHT_BOLD));
  wxTextAttr attrNorm(wxColour(0, 0, 0), wxColour(255, 255, 255),
                      wxFont(9, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,
                             wxFONTWEIGHT_NORMAL));
  wxTextCtrl *geomCtrl = (wxTextCtrl *) FindWindow(ID_GEOM_TABLE);
  if (geomCtrl->GetValue().Len() < 1)
    {
      ::wxBeginBusyCursor();
      pt = Geometry->FirstPoint;
      while (pt)
        {
          // counting how many points are into this Geometry
          points++;
          pt = pt->Next;
        }
      ln = Geometry->FirstLinestring;
      while (ln)
        {
          // counting how many linestrings are into this Geometry
          linestrings++;
          ln = ln->Next;
        }
      pg = Geometry->FirstPolygon;
      while (pg)
        {
          // counting how many polygons are into this Geometry
          polygons++;
          pg = pg->Next;
        }
      // determining the Geometry type
      geomCtrl->SetDefaultStyle(attrNorm);
      geomCtrl->AppendText(wxT("SRID: "));
      geomCtrl->SetDefaultStyle(attrBold);
      sprintf(dummy, "%d", Geometry->Srid);
      strValue = wxString::FromUTF8(dummy);
      geomCtrl->AppendText(strValue);
      geomCtrl->SetDefaultStyle(attrNorm);
      geomCtrl->AppendText(wxT("\n\n"));
      strValue = wxT("UNKNOWN GEOMETRY TYPE");
      if (points == 1 && linestrings == 0 && polygons == 0)
        {
          if (Geometry->DeclaredType == GAIA_MULTIPOINT)
            strValue = wxT("MULTIPOINT");
          else if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("POINT");
      } else if (points == 0 && linestrings == 1 && polygons == 0)
        {
          if (Geometry->DeclaredType == GAIA_MULTILINESTRING)
            strValue = wxT("MULTILINESTRING");
          else if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("LINESTRING");
      } else if (points == 0 && linestrings == 0 && polygons == 1)
        {
          if (Geometry->DeclaredType == GAIA_MULTIPOLYGON)
            strValue = wxT("MULTIPOLYGON");
          else if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("POLYGON");
      } else if (points > 1 && linestrings == 0 && polygons == 0)
        {
          if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("MULTIPOINT");
      } else if (points == 0 && linestrings > 1 && polygons == 0)
        {
          if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("MULTILINESTRING");
      } else if (points == 0 && linestrings == 0 && polygons > 1)
        {
          if (Geometry->DeclaredType == GAIA_GEOMETRYCOLLECTION)
            strValue = wxT("GEOMETRYCOLLECTION");
          else
            strValue = wxT("MULTIPOLYGON");
      } else
        strValue = wxT("GEOMETRYCOLLECTION");
      geomCtrl->SetDefaultStyle(attrNorm);
      geomCtrl->AppendText(wxT("Geometry type: "));
      geomCtrl->SetDefaultStyle(attrBold);
      geomCtrl->AppendText(strValue);
      geomCtrl->SetDefaultStyle(attrNorm);
      geomCtrl->AppendText(wxT("\n\n"));
      if (points)
        {
          // printing the Points list
          sprintf(dummy, "#%d POINT", points);
          strValue = wxString::FromUTF8(dummy);
          if (points > 1)
            strValue += wxT("s:");
          else
            strValue += wxT(":");
          geomCtrl->SetDefaultStyle(attrBold);
          geomCtrl->AppendText(strValue);
          geomCtrl->SetDefaultStyle(attrNorm);
          pt = Geometry->FirstPoint;
          points = 0;
          while (pt)
            {
              // printing each Point
              points++;
              sprintf(dummy, "\n  %d) ", points);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->SetDefaultStyle(attrBold);
              geomCtrl->AppendText(strValue);
              geomCtrl->SetDefaultStyle(attrNorm);
              sprintf(dummy, "%1.4f  %1.4f", pt->X, pt->Y);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->AppendText(strValue);
              pt = pt->Next;
            }
          geomCtrl->AppendText(wxT("\n\n\n"));
        }
      if (linestrings)
        {
          // printing the Linestrings list
          sprintf(dummy, "#%d LINESTRING", linestrings);
          strValue = wxString::FromUTF8(dummy);
          if (linestrings > 1)
            strValue += wxT("s:");
          else
            strValue += wxT(":");
          geomCtrl->SetDefaultStyle(attrBold);
          geomCtrl->AppendText(strValue);
          geomCtrl->SetDefaultStyle(attrNorm);
          ln = Geometry->FirstLinestring;
          linestrings = 0;
          while (ln)
            {
              // printing each Linestring
              linestrings++;
              sprintf(dummy, "\n  %d) ", linestrings);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->SetDefaultStyle(attrBold);
              geomCtrl->AppendText(strValue);
              geomCtrl->SetDefaultStyle(attrNorm);
              sprintf(dummy, "%d vertices", ln->Points);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->AppendText(strValue);
              ln = ln->Next;
            }
          geomCtrl->AppendText(wxT("\n\n\n"));
        }
      if (polygons)
        {
          // printing the Polygons list
          sprintf(dummy, "#%d POLYGON", polygons);
          strValue = wxString::FromUTF8(dummy);
          if (polygons > 1)
            strValue += wxT("s:");
          else
            strValue += wxT(":");
          geomCtrl->SetDefaultStyle(attrBold);
          geomCtrl->AppendText(strValue);
          geomCtrl->SetDefaultStyle(attrNorm);
          pg = Geometry->FirstPolygon;
          polygons = 0;
          while (pg)
            {
              // printing each Polygon
              polygons++;
              sprintf(dummy, "\n  %d)    exterior ring", polygons);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->SetDefaultStyle(attrBold);
              geomCtrl->AppendText(strValue);
              geomCtrl->SetDefaultStyle(attrNorm);
              rng = pg->Exterior;
              sprintf(dummy, ": %d vertices", rng->Points);
              strValue = wxString::FromUTF8(dummy);
              geomCtrl->AppendText(strValue);
              for (ib = 0; ib < pg->NumInteriors; ib++)
                {
                  // printing each interior ring
                  sprintf(dummy, "\n  %d.%d) ", polygons, ib + 1);
                  strValue = wxString::FromUTF8(dummy);
                  geomCtrl->SetDefaultStyle(attrBold);
                  geomCtrl->AppendText(strValue);
                  geomCtrl->SetDefaultStyle(attrNorm);
                  rng = pg->Interiors + ib;
                  sprintf(dummy, " interior ring: %d vertices", rng->Points);
                  strValue = wxString::FromUTF8(dummy);
                  geomCtrl->AppendText(strValue);
                }
              pg = pg->Next;
            }
          geomCtrl->AppendText(wxT("\n\n\n"));
        }
      ::wxEndBusyCursor();
    }
  GraphicsGeometry *geomGraph = (GraphicsGeometry *) FindWindow(ID_GEOM_GRAPH);
  geomGraph->SetBitmap(GeomPreview);
}

void BlobExplorerDialog::UpdateImagePage()
{
//
// updating the IMAGE page
//
  double horz;
  double vert;
  wxImage scaledImg;
  wxSize sz;
  wxSize box;
  int boxX;
  int boxY;
  int posX;
  int posY;
  char latlong[1024];
  char dummy[1024];
  wxString ll;
  wxString title = wxT("Invalid Image");
  wxStaticBox *imgBox = (wxStaticBox *) FindWindow(ID_IMG_BOX);
  ImageShow *imgShow = (ImageShow *) FindWindow(ID_IMAGE);
  wxStaticText *imageTitle = (wxStaticText *) FindWindow(ID_IMAGE_TITLE);
  if (Image)
    {
      ::wxBeginBusyCursor();
      if (Image->IsOk() == true)
        {
          horz = Image->GetWidth();
          vert = Image->GetHeight();
          sz = imgShow->GetSize();
          box = imgBox->GetSize();
          while (horz > sz.GetWidth() || vert > sz.GetHeight())
            {
              horz *= 0.9;
              vert *= 0.9;
            }
          if (horz == Image->GetWidth() && vert == Image->GetHeight())
            scaledImg = Image->Copy();
          else
            scaledImg =
              Image->Scale((int) horz, (int) vert, wxIMAGE_QUALITY_HIGH);
          wxBitmap bmp(scaledImg);
          imgBox->GetPosition(&boxX, &boxY);
          posX = (box.GetWidth() - (int) horz) / 2;
          posY = (box.GetHeight() - (int) vert) / 2;
          imgShow->SetSize(boxX + posX, boxY + posY, (int) horz, (int) vert);
          imgShow->SetBitmap(bmp);
          imgShow->Show(true);
          switch (BlobType)
            {
              case GAIA_JPEG_BLOB:
                sprintf(dummy,
                        "JPEG image     resolution: %d x %d          %d bytes",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                break;
              case GAIA_EXIF_BLOB:
                sprintf(dummy,
                        "EXIF image     resolution: %d x %d          %d bytes",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                break;
              case GAIA_EXIF_GPS_BLOB:
                if (gaiaGetGpsLatLong(Blob, BlobSize, latlong, 1024))
                  ll = wxString::FromUTF8(latlong);
                else
                  ll = wxT("NOT AVAILABLE");
                sprintf(dummy,
                        "EXIF-GPS image     resolution: %d x %d          %d bytes    GPS: ",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                title += ll;
                break;
              case GAIA_PNG_BLOB:
                sprintf(dummy,
                        "PNG image     resolution: %d x %d          %d bytes",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                break;
              case GAIA_GIF_BLOB:
                sprintf(dummy,
                        "GIF image     resolution: %d x %d          %d bytes",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                break;
              case GAIA_TIFF_BLOB:
                sprintf(dummy,
                        "TIFF image     resolution: %d x %d          %d bytes",
                        Image->GetWidth(), Image->GetHeight(), BlobSize);
                title = wxString::FromUTF8(dummy);
                break;
              default:
                if (gGraphIsRawImage(Blob, BlobSize) == GGRAPH_OK)
                  {
                    sprintf(dummy,
                            "RasterLite RAW image     resolution: %d x %d          %d bytes",
                            Image->GetWidth(), Image->GetHeight(), BlobSize);
                    title = wxString::FromUTF8(dummy);
                } else if (IsSVG == true)
                  {
                    sprintf(dummy,
                            "SVG image     resolution: %1.2f x %1.2f          %d bytes",
                            SvgWidth, SvgHeight, SvgSize);
                    title = wxString::FromUTF8(dummy);
                  }
                break;
            }
        }
      ::wxEndBusyCursor();
    }
  imageTitle->SetLabel(title);
}

void BlobExplorerDialog::FormatWKT(wxTextCtrl * txtCtrl, wxString & in,
                                   wxString & out)
{
//
// splitting WKT into rows
//
  int page_width;
  int h;
  wxClientDC *dc = new wxClientDC(txtCtrl);
  int row_sz = 0;
  wxSize spSz = dc->GetTextExtent(wxT(","));
  int sp_sz = spSz.GetWidth();

  out = wxT("");
  txtCtrl->GetSize(&page_width, &h);
  page_width -= page_width / 4;
  wxStringTokenizer tkz(in, wxT(","));
  while (tkz.HasMoreTokens())
    {
      wxString token = tkz.GetNextToken();
      wxSize sz = dc->GetTextExtent(token);
      if (row_sz == 0)
        {
          out += token;
          row_sz = sz.GetWidth();
      } else
        {
          if (row_sz + sp_sz + sz.GetWidth() < page_width)
            {
              out += wxT(",") + token;
              row_sz += sp_sz + sz.GetWidth();
          } else
            {
              out += wxT(",\n") + token;
              row_sz = sz.GetWidth();
            }
        }
    }
  delete dc;
}

void BlobExplorerDialog::UpdateWKTPage()
{
//
// updating the WKT page
//
  wxTextCtrl *wktCtrl = (wxTextCtrl *) FindWindow(ID_WKT_TABLE);
  if (wktCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT ST_AsText(?)";
      ::wxBeginBusyCursor();
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_blob(stmt, 1, Blob, BlobSize, SQLITE_STATIC);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                WKTstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString wkt;
      FormatWKT(wktCtrl, WKTstring, wkt);
      wktCtrl->WriteText(wkt);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::UpdateEWKTPage()
{
//
// updating the EWKT page
//
  wxTextCtrl *ewktCtrl = (wxTextCtrl *) FindWindow(ID_EWKT_TABLE);
  if (ewktCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT AsEWKT(?)";
      ::wxBeginBusyCursor();
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_blob(stmt, 1, Blob, BlobSize, SQLITE_STATIC);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                EWKTstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString ewkt;
      FormatWKT(ewktCtrl, EWKTstring, ewkt);
      ewktCtrl->WriteText(ewkt);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::FormatSVG(wxTextCtrl * txtCtrl, wxString & in,
                                   wxString & out)
{
//
// splitting SVG into rows
//
  int page_width;
  int h;
  wxClientDC *dc = new wxClientDC(txtCtrl);
  int row_sz = 0;
  wxSize spSz = dc->GetTextExtent(wxT(" "));
  int sp_sz = spSz.GetWidth();

  out = wxT("");
  txtCtrl->GetSize(&page_width, &h);
  page_width -= page_width / 4;
  wxStringTokenizer tkz(in, wxT(" "));
  while (tkz.HasMoreTokens())
    {
      wxString token = tkz.GetNextToken();
      wxSize sz = dc->GetTextExtent(token);
      if (row_sz == 0)
        {
          out += token;
          row_sz = sz.GetWidth();
      } else
        {
          if (row_sz + sp_sz + sz.GetWidth() < page_width)
            {
              out += wxT(" ") + token;
              row_sz += sp_sz + sz.GetWidth();
          } else
            {
              out += wxT("\n") + token;
              row_sz = sz.GetWidth();
            }
        }
    }
  delete dc;
}

void BlobExplorerDialog::UpdateSVGPage()
{
//
// updating the SVG page
//
  wxTextCtrl *svgCtrl = (wxTextCtrl *) FindWindow(ID_SVG_TABLE);
  if (svgCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT AsSVG(?, ?)";
      ::wxBeginBusyCursor();
      if (SVGprecision >= 0)
        sql = "SELECT AsSVG(?, ?, ?)";
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_blob(stmt, 1, Blob, BlobSize, SQLITE_STATIC);
      if (SVGrelative == true)
        sqlite3_bind_int(stmt, 2, 1);
      else
        sqlite3_bind_int(stmt, 2, 0);
      if (SVGprecision >= 0)
        sqlite3_bind_int(stmt, 3, SVGprecision);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                SVGstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString svg;
      FormatSVG(svgCtrl, SVGstring, svg);
      svgCtrl->WriteText(svg);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::UpdateKMLPage()
{
//
// updating the KML page
//
  wxTextCtrl *kmlCtrl = (wxTextCtrl *) FindWindow(ID_KML_TABLE);
  if (kmlCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT AsKML(?)";
      if (KMLprecision >= 0)
        sql = "SELECT AsKML(?, ?)";
      ::wxBeginBusyCursor();
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_blob(stmt, 1, Blob, BlobSize, SQLITE_STATIC);
      if (KMLprecision >= 0)
        sqlite3_bind_int(stmt, 2, KMLprecision);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                KMLstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString kml;
      FormatSVG(kmlCtrl, KMLstring, kml);
      kmlCtrl->WriteText(kml);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::UpdateGMLPage()
{
//
// updating the GML page
//
  wxTextCtrl *gmlCtrl = (wxTextCtrl *) FindWindow(ID_GML_TABLE);
  if (gmlCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT AsGML(?, ?)";
      if (GMLprecision >= 0)
        sql = "SELECT AsGML(?, ?, ?)";
      ::wxBeginBusyCursor();
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      if (GMLv2v3 == true)
        sqlite3_bind_int(stmt, 1, 2);
      else
        sqlite3_bind_int(stmt, 1, 3);
      sqlite3_bind_blob(stmt, 2, Blob, BlobSize, SQLITE_STATIC);
      if (GMLprecision >= 0)
        sqlite3_bind_int(stmt, 3, GMLprecision);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                GMLstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString gml;
      FormatSVG(gmlCtrl, GMLstring, gml);
      gmlCtrl->WriteText(gml);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::UpdateGeoJSONPage()
{
//
// updating the GeoJSON page
//
  wxTextCtrl *geoJsonCtrl = (wxTextCtrl *) FindWindow(ID_GEOJSON_TABLE);
  if (geoJsonCtrl->GetValue().Len() < 1)
    {
      sqlite3_stmt *stmt = NULL;
      char err_msg[2048];
      const char *sql = "SELECT AsGeoJSON(?)";
      if (GeoJSONprecision >= 0)
        sql = "SELECT AsGeoJSON(?, ?)";
      if (GeoJSONoptions > 0)
        sql = "SELECT AsGeoJSON(?, ?, ?)";
      ::wxBeginBusyCursor();
      int ret =
        sqlite3_prepare_v2(MainFrame->GetSqlite(), sql, strlen(sql), &stmt,
                           NULL);
      if (ret != SQLITE_OK)
        {
          sprintf(err_msg, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(err_msg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          ::wxEndBusyCursor();
          return;
        }
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_blob(stmt, 1, Blob, BlobSize, SQLITE_STATIC);
      if (GeoJSONoptions > 0)
        {
          if (GeoJSONprecision >= 0)
            sqlite3_bind_int(stmt, 2, GeoJSONprecision);
          else
            sqlite3_bind_int(stmt, 2, 15);
          sqlite3_bind_int(stmt, 3, GeoJSONoptions);
      } else if (GeoJSONprecision >= 0)
        sqlite3_bind_int(stmt, 2, GeoJSONprecision);
      while (1)
        {
          //
          // fetching the result set row 
          //
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE)
            break;              // end of result set
          if (ret == SQLITE_ROW)
            {
              //
              // fetching a row
              //
              if (sqlite3_column_type(stmt, 0) == SQLITE_TEXT)
                GeoJSONstring =
                  wxString::FromUTF8((const char *)
                                     sqlite3_column_text(stmt, 0));
          } else
            {
              sqlite3_finalize(stmt);
              sprintf(err_msg, "SQL error: %s",
                      sqlite3_errmsg(MainFrame->GetSqlite()));
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              ::wxEndBusyCursor();
              return;
            }
        }
      sqlite3_finalize(stmt);
      wxString geoJson;
      FormatWKT(geoJsonCtrl, GeoJSONstring, geoJson);
      geoJsonCtrl->WriteText(geoJson);
      ::wxEndBusyCursor();
    }
}

void BlobExplorerDialog::UpdateXmlDocumentPage()
{
//
// updating the XmlDocument page
//
  wxTextCtrl *xmlCtrl = (wxTextCtrl *) FindWindow(ID_XML_DOCUMENT_TABLE);
  xmlCtrl->WriteText(XMLDocument);
}

void BlobExplorerDialog::UpdateXmlIndentedPage()
{
//
// updating the XmlIndented page
//
  wxTextCtrl *xmlCtrl = (wxTextCtrl *) FindWindow(ID_XML_INDENTED_TABLE);
  xmlCtrl->WriteText(XMLIndented);
}

void BlobExplorerDialog::OnOk(wxCommandEvent & WXUNUSED(event))
{
//
// all done: exiting
//
  wxDialog::EndModal(wxID_OK);
}

void BlobExplorerDialog::DrawGeometry(int horz, int vert)
{
//
// drawing graphic representation for current Geometry
//
  gaiaPointPtr pt;
  gaiaLinestringPtr ln;
  gaiaPolygonPtr pg;
  gaiaRingPtr rng;
  double minx;
  double miny;
  double maxx;
  double maxy;
  double ext_x;
  double ext_y;
  double cx;
  double cy;
  double pixelRatioX;
  double pixelRatioY;
  double pixelRatio;
  double span_x;
  double span_y;
  double baseX;
  double baseY;
  double x;
  double y;
  double z;
  double m;
  double xx;
  double yy;
  int iv;
  int ib;
  int pts;
  int *borders;
  wxPoint *points;
  GeomPreview.Create(horz, vert);
  wxMemoryDC dc(GeomPreview);
//
// background filling
//
  dc.SetBrush(wxBrush(wxColour(255, 255, 255)));
  dc.DrawRectangle(0, 0, horz, vert);
//
// prepearing the drawing pen and brush
//
  dc.SetBrush(wxBrush(wxColour(240, 240, 192)));
  dc.SetPen(wxPen(wxColour(64, 64, 192), 1));
//
// computing the pixel ratio, center position and so on
//
  minx = Geometry->MinX;
  miny = Geometry->MinY;
  maxx = Geometry->MaxX;
  maxy = Geometry->MaxY;
  ext_x = maxx - minx;
  ext_y = maxy - miny;
  minx = Geometry->MinX - (ext_x / 20.0);
  miny = Geometry->MinY - (ext_y / 20.0);
  maxx = Geometry->MaxX + (ext_x / 20.0);
  maxy = Geometry->MaxY + (ext_y / 20.0);
  ext_x = maxx - minx;
  ext_y = maxy - miny;
  cx = minx + (ext_x / 2.0);
  cy = miny + (ext_y / 2.0);
  pixelRatioX = ext_x / horz;
  pixelRatioY = ext_y / vert;
  if (pixelRatioX > pixelRatioY)
    pixelRatio = pixelRatioX;
  else
    pixelRatio = pixelRatioY;
//
// centering the Y axis
//
  span_y = vert * pixelRatio;
  baseY = cy - (span_y / 2.0);
//
// centering the X axis
//
  span_x = horz * pixelRatio;
  baseX = cx - (span_x / 2.0);
  pg = Geometry->FirstPolygon;
  while (pg)
    {
      //
      // drawing polygons
      //
      pts = pg->Exterior->Points;
      for (ib = 0; ib < pg->NumInteriors; ib++)
        {
          rng = pg->Interiors + ib;
          pts += rng->Points;
        }
      borders = new int[pg->NumInteriors + 1];
      points = new wxPoint[pts];
      pts = 0;
      rng = pg->Exterior;
      borders[0] = rng->Points;
      for (iv = 0; iv < rng->Points; iv++)
        {
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
          xx = (x - baseX) / pixelRatio;
          yy = (y - baseY) / pixelRatio;
          yy = vert - yy;
          points[pts].x = (int) xx;
          points[pts].y = (int) yy;
          pts++;
        }
      for (ib = 0; ib < pg->NumInteriors; ib++)
        {
          rng = pg->Interiors + ib;
          borders[1 + ib] = rng->Points;
          for (iv = 0; iv < rng->Points; iv++)
            {
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
              xx = (x - baseX) / pixelRatio;
              yy = (y - baseY) / pixelRatio;
              yy = vert - yy;
              points[pts].x = (int) xx;
              points[pts].y = (int) yy;
              pts++;
            }
        }
      dc.DrawPolyPolygon(pg->NumInteriors + 1, borders, points);
      delete[]points;
      delete[]borders;
      pg = pg->Next;
    }
  ln = Geometry->FirstLinestring;
  while (ln)
    {
      //
      // drawing linestrings
      //
      points = new wxPoint[ln->Points];
      for (iv = 0; iv < ln->Points; iv++)
        {
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
          xx = (x - baseX) / pixelRatio;
          yy = (y - baseY) / pixelRatio;
          yy = vert - yy;
          points[iv].x = (int) xx;
          points[iv].y = (int) yy;
        }
      dc.DrawLines(ln->Points, points);
      delete[]points;
      ln = ln->Next;
    }
  pt = Geometry->FirstPoint;
  while (pt)
    {
      //
      // drawing points
      //
      xx = (pt->X - baseX) / pixelRatio;
      yy = (pt->Y - baseY) / pixelRatio;
      yy = vert - yy;
      dc.DrawCircle((int) xx, (int) yy, 2);
      pt = pt->Next;
    }
}

GraphicsGeometry::GraphicsGeometry(BlobExplorerDialog * parent,
                                   wxWindow * panel, wxWindowID id,
                                   const wxBitmap & bmp,
                                   const wxSize & size):wxStaticBitmap(panel,
                                                                       id, bmp,
                                                                       wxDefaultPosition,
                                                                       size)
{
  Parent = parent;
}

ImageShow::ImageShow(BlobExplorerDialog * parent, wxWindow * panel,
                     wxWindowID id, const wxBitmap & bmp,
                     const wxSize & size):wxStaticBitmap(panel, id, bmp,
                                                         wxDefaultPosition,
                                                         size, wxBORDER_SIMPLE)
{
  Parent = parent;
// appends event handler 
  Connect(ID_IMAGE, wxEVT_RIGHT_DOWN,
          (wxObjectEventFunction) & ImageShow::OnRightClick);
  Connect(Image_Copy, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & ImageShow::OnCmdCopy);
}

void ImageShow::OnRightClick(wxMouseEvent & event)
{
//
// right click on the Image
//
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxImage *Image = Parent->GetImage();
  if (Image)
    {
      if (Image->IsOk() == true)
        {
          wxPoint pt = event.GetPosition();
          menuItem = new wxMenuItem(menu, Image_Copy, wxT("&Copy"));
          menu->Append(menuItem);
          PopupMenu(menu, pt);
        }
    }
}

void ImageShow::OnCmdCopy(wxCommandEvent & WXUNUSED(event))
{
//
// copying the Image into the clipboard
//
  wxImage *Image = Parent->GetImage();
  if (wxTheClipboard->Open())
    {
      wxTheClipboard->SetData(new wxBitmapDataObject(*Image));
      wxTheClipboard->Close();
    }
}

MyHexList::MyHexList(BlobExplorerDialog * parent, unsigned char *blob,
                     int blob_size, wxWindow * panel, wxWindowID id,
                     const wxPoint & pos, const wxSize & size,
                     long style):wxListCtrl(panel, id, pos, size, style)
{
// constructor - the blob hexadecimal dump
  int i = 0;
  int rows = 0;
  Parent = parent;
  Blob = blob;
  BlobSize = blob_size;
  while (i < BlobSize)
    {
      // counting how many rows are there
      rows++;
      i += 16;
    }
  SetItemCount(rows);
}

MyHexList::~MyHexList()
{
// does nothing
}

wxString MyHexList::OnGetItemText(long item, long column) const
{
// return a column value
  int i;
  int c;
  int base = item * 16;
  wxString value;
  char dummy[64];
  wxString hex;
  if (column == 0)
    {
      sprintf(dummy, "%08xd", base);
      value = wxString::FromUTF8(dummy);
  } else if (column == 1)
    {
      // prepearing the hex-dump
      c = 0;
      for (i = base; i < BlobSize; i++)
        {
          if (c >= 16)
            break;
          sprintf(dummy, "%02x", *(Blob + i));
          hex = wxString::FromUTF8(dummy);
          if (c == 8)
            value += wxT("   ");
          else
            value += wxT(" ");
          value += hex;
          c++;
        }
  } else
    {
      // prepearing the ascii dump
      c = 0;
      for (i = base; i < BlobSize; i++)
        {
          if (c >= 16)
            break;
          if (isprint(*(Blob + i)))
            {
              sprintf(dummy, "%c", *(Blob + i));
              hex = wxString::FromUTF8(dummy);
          } else
            hex = wxT(".");
          value += hex;
          c++;
        }
    }
  return value;
}
