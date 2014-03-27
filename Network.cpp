/*
/ Network.cpp
/ methods related to Network building
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
#include <float.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp	_stricmp
#endif

#define MAX_BLOCK	1048576

void
  MyFrame::BuildNetwork(wxString & table, wxString & from, wxString & to,
                        wxString & geometry, wxString & name, bool cost_length,
                        wxString & cost, bool bidirectional, bool one_way,
                        wxString & one_way_from_to, wxString & one_way_to_from,
                        bool aStarSupported)
{
//
// trying to build a Network
// 
  int ret;
  sqlite3_stmt *stmt;
  Network *p_graph = NULL;
  wxString sql;
  char xsql[2048];
  char **results;
  int n_rows;
  int n_columns;
  int i;
  char *errMsg = NULL;
  char *col_name;
  int type;
  bool ok_from_column = false;
  bool ok_to_column = false;
  bool ok_cost_column = false;
  bool ok_geom_column = false;
  bool ok_name_column = false;
  bool ok_oneway_tofrom = false;
  bool ok_oneway_fromto = false;
  bool from_null = false;
  bool from_int = false;
  bool from_double = false;
  bool from_text = false;
  bool from_blob = false;
  bool to_null = false;
  bool to_int = false;
  bool to_double = false;
  bool to_text = false;
  bool to_blob = false;
  bool cost_null = false;
  bool cost_text = false;
  bool cost_blob = false;
  bool tofrom_null = false;
  bool tofrom_double = false;
  bool tofrom_text = false;
  bool tofrom_blob = false;
  bool fromto_null = false;
  bool fromto_double = false;
  bool fromto_text = false;
  bool fromto_blob = false;
  bool geom_null = false;
  bool geom_not_linestring = false;
  int col_n;
  int fromto_n = 0;
  int tofrom_n = 0;
  sqlite3_int64 rowid;
  sqlite3_int64 id_from = -1;
  sqlite3_int64 id_to = -1;
  char code_from[1024];
  char code_to[1024];
  double node_from_x;
  double node_from_y;
  double node_to_x;
  double node_to_y;
  double cost_val;
  int fromto;
  int tofrom;
  wxString endMsg;
  wxString msg;
  bool wr;
  bool aStarLength;
  double a_star_length;
  double a_star_coeff;
  double min_a_star_coeff = DBL_MAX;
  char xname[1024];
  ::wxBeginBusyCursor();
// checking for table existence
  sql =
    wxT("SELECT tbl_name FROM sqlite_master WHERE Lower(tbl_name) = Lower('");
  strcpy(xname, table.ToUTF8());
  CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("') AND type = 'table'");
  strcpy(xsql, sql.ToUTF8());
  ret =
    sqlite3_get_table(SqliteHandle, xsql, &results, &n_rows, &n_columns,
                      &errMsg);
  if (ret != SQLITE_OK)
    {
// some error occurred 
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (n_rows == 0)
    {
      // required table does not exists 
      wxMessageBox(wxT("ERROR: table ") + table + wxT(" does not exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      goto abort;
  } else
    sqlite3_free_table(results);
// checking for columns existence
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  ret =
    sqlite3_get_table(SqliteHandle, xsql, &results, &n_rows, &n_columns,
                      &errMsg);
  if (ret != SQLITE_OK)
    {
// some error occurred 
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (n_rows > 1)
    {
      for (i = 1; i <= n_rows; i++)
        {
          char xcol[256];
          col_name = results[(i * n_columns) + 1];
          strcpy(xcol, from.ToUTF8());
          if (strcasecmp(xcol, col_name) == 0)
            ok_from_column = true;
          strcpy(xcol, to.ToUTF8());
          if (strcasecmp(xcol, col_name) == 0)
            ok_to_column = true;
          if (cost_length == false)
            {
              strcpy(xcol, cost.ToUTF8());
              if (strcasecmp(xcol, col_name) == 0)
                ok_cost_column = true;
            }
          strcpy(xcol, geometry.ToUTF8());
          if (strcasecmp(xcol, col_name) == 0)
            ok_geom_column = true;
          if (name.Len() > 0)
            {
              strcpy(xcol, name.ToUTF8());
              if (strcasecmp(xcol, col_name) == 0)
                ok_name_column = true;
            }
          if (one_way == true)
            {
              strcpy(xcol, one_way_from_to.ToUTF8());
              if (strcasecmp(xcol, col_name) == 0)
                ok_oneway_tofrom = true;
            }
          if (one_way == true)
            {
              strcpy(xcol, one_way_to_from.ToUTF8());
              if (strcasecmp(xcol, col_name) == 0)
                ok_oneway_fromto = true;
            }
        }
      sqlite3_free_table(results);
    }
  if (ok_from_column == true && ok_to_column == true && ok_geom_column == true)
    ;
  else
    goto abort;
  if (name.Len() > 0 && ok_name_column == false)
    goto abort;
  if (cost_length == false && ok_cost_column == false)
    goto abort;
  if (one_way == true && ok_oneway_tofrom == false)
    goto abort;
  if (one_way == true && ok_oneway_fromto == false)
    goto abort;
// checking column types
  p_graph = new Network();
  strcpy(xname, from.ToUTF8());
  DoubleQuotedSql(xname);
  sql = wxT("SELECT ") + wxString::FromUTF8(xname);
  strcpy(xname, to.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxT(", ") + wxString::FromUTF8(xname);
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxT(", GeometryType(") + wxString::FromUTF8(xname) + wxT(")");
  col_n = 3;
  if (cost_length == false)
    {
      strcpy(xname, cost.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxT(", ") + wxString::FromUTF8(xname);
      col_n++;
    }
  if (one_way == true)
    {
      strcpy(xname, one_way_to_from.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxT(", ") + wxString::FromUTF8(xname);
      tofrom_n = col_n;
      col_n++;
      strcpy(xname, one_way_from_to.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxT(", ") + wxString::FromUTF8(xname);
      fromto_n = col_n;
      col_n++;
    }
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxT(" FROM ") + wxString::FromUTF8(xname);
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_prepare_v2(SqliteHandle, xsql, strlen(xsql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      wxMessageBox(wxT("SQL error: ") + err, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      goto abort;
    }
  n_columns = sqlite3_column_count(stmt);
  while (1)
    {
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        break;
      if (ret == SQLITE_ROW)
        {
          // the NodeFrom type 
          type = sqlite3_column_type(stmt, 0);
          if (type == SQLITE_NULL)
            from_null = true;
          if (type == SQLITE_INTEGER)
            {
              from_int = true;
              id_from = sqlite3_column_int(stmt, 0);
              p_graph->InsertNode(id_from);
            }
          if (type == SQLITE_FLOAT)
            from_double = true;
          if (type == SQLITE_TEXT)
            {
              from_text = true;
              strcpy(code_from, (char *) sqlite3_column_text(stmt, 0));
              p_graph->InsertNode(code_from);
            }
          if (type == SQLITE_BLOB)
            from_blob = true;
          // the NodeTo type 
          type = sqlite3_column_type(stmt, 1);
          if (type == SQLITE_NULL)
            to_null = true;
          if (type == SQLITE_INTEGER)
            {
              to_int = true;
              id_to = sqlite3_column_int(stmt, 1);
              p_graph->InsertNode(id_to);
            }
          if (type == SQLITE_FLOAT)
            to_double = true;
          if (type == SQLITE_TEXT)
            {
              to_text = true;
              strcpy(code_to, (char *) sqlite3_column_text(stmt, 1));
              p_graph->InsertNode(code_to);
            }
          if (type == SQLITE_BLOB)
            to_blob = true;
          // the Geometry type 
          type = sqlite3_column_type(stmt, 2);
          if (type == SQLITE_NULL)
            geom_null = true;
          else if (strcmp("LINESTRING", (char *) sqlite3_column_text(stmt, 2))
                   != 0)
            geom_not_linestring = true;
          col_n = 3;
          if (cost_length == false)
            {
              // the Cost type 
              type = sqlite3_column_type(stmt, col_n);
              col_n++;
              if (type == SQLITE_NULL)
                cost_null = true;
              if (type == SQLITE_TEXT)
                cost_text = true;
              if (type == SQLITE_BLOB)
                cost_blob = true;
            }
          if (one_way == true)
            {
              // the FromTo type
              type = sqlite3_column_type(stmt, col_n);
              col_n++;
              if (type == SQLITE_NULL)
                fromto_null = true;
              if (type == SQLITE_FLOAT)
                fromto_double = true;
              if (type == SQLITE_TEXT)
                fromto_text = true;
              if (type == SQLITE_BLOB)
                fromto_blob = true;
              // the ToFrom type 
              type = sqlite3_column_type(stmt, col_n);
              col_n++;
              if (type == SQLITE_NULL)
                tofrom_null = true;
              if (type == SQLITE_FLOAT)
                tofrom_double = true;
              if (type == SQLITE_TEXT)
                tofrom_text = true;
              if (type == SQLITE_BLOB)
                tofrom_blob = true;
            }
      } else
        {
          wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          wxMessageBox(wxT("sqlite3_step error: ") + err, wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_finalize(stmt);
          goto abort;
        }
    }
  sqlite3_finalize(stmt);
  ret = 1;
  if (from_null == true)
    ret = 0;
  if (from_blob == true)
    ret = 0;
  if (from_double == true)
    ret = 0;
  if (to_null == true)
    ret = 0;
  if (to_blob == true)
    ret = 0;
  if (to_double == true)
    ret = 0;
  if (geom_null == true)
    ret = 0;
  if (geom_not_linestring == true)
    ret = 0;
  if (cost_length == false)
    {
      if (cost_null == true)
        ret = 0;
      if (cost_blob == true)
        ret = 0;
      if (cost_text == true)
        ret = 0;
    }
  if (one_way == true)
    {
      if (fromto_null == true)
        ret = 0;
      if (fromto_blob == true)
        ret = 0;
      if (fromto_text == true)
        ret = 0;
      if (fromto_double == true)
        ret = 0;
      if (tofrom_null == true)
        ret = 0;
      if (tofrom_blob == true)
        ret = 0;
      if (tofrom_text == true)
        ret = 0;
      if (tofrom_double == true)
        ret = 0;
    }
  if (!ret)
    goto abort;
  if (from_int == true && to_int == true)
    {
      // each node is identified by an INTEGER id 
      p_graph->SetNodeCode(false);
  } else if (from_text == true && to_text == true)
    {
      // each node is identified by a TEXT code
      p_graph->SetNodeCode(true);
  } else
    goto abort;
  p_graph->InitNodes();
// checking topologic consistency 
  strcpy(xname, from.ToUTF8());
  DoubleQuotedSql(xname);
  sql = wxT("SELECT ROWID, ") + wxString::FromUTF8(xname);
  strcpy(xname, to.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxT(", ") + wxString::FromUTF8(xname) + wxT(", ");
  strcpy(xname, geometry.ToUTF8());
  DoubleQuotedSql(xname);
  sql +=
    wxT("X(StartPoint(") + wxString::FromUTF8(xname) + wxT(")), Y(StartPoint(");
  sql += wxString::FromUTF8(xname) + wxT(")), ");
  sql +=
    wxT("X(EndPoint(") + wxString::FromUTF8(xname) + wxT(")), Y(EndPoint(");
  sql += wxString::FromUTF8(xname) + wxT("))");
  if (aStarSupported == true)
    {
      // supporting A* algorithm
      if (cost_length == false)
        {
          strcpy(xname, cost.ToUTF8());
          DoubleQuotedSql(xname);
          sql += wxT(", ") + wxString::FromUTF8(xname);
          strcpy(xname, geometry.ToUTF8());
          DoubleQuotedSql(xname);
          sql += wxT(", GLength(") + wxString::FromUTF8(xname) + wxT(")");
          col_n = 9;
          aStarLength = true;
      } else
        {
          strcpy(xname, geometry.ToUTF8());
          DoubleQuotedSql(xname);
          sql += wxT(", GLength(") + wxString::FromUTF8(xname) + wxT(")");
          col_n = 8;
          aStarLength = false;
          min_a_star_coeff = 1.0;
        }
  } else
    {
      // A* algorithm unsupported
      if (cost_length == false)
        {
          strcpy(xname, cost.ToUTF8());
          DoubleQuotedSql(xname);
          sql += wxT(", ") + wxString::FromUTF8(xname);
      } else
        {
          strcpy(xname, geometry.ToUTF8());
          DoubleQuotedSql(xname);
          sql += wxT(", GLength(") + wxString::FromUTF8(xname) + wxT(")");
        }
      col_n = 8;
      aStarLength = false;
    }
  if (one_way == true)
    {
      strcpy(xname, one_way_to_from.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxT(", ") + wxString::FromUTF8(xname);
      tofrom_n = col_n;
      col_n++;
      strcpy(xname, one_way_from_to.ToUTF8());
      DoubleQuotedSql(xname);
      sql += wxT(", ") + wxString::FromUTF8(xname);
      fromto_n = col_n;
      col_n++;
    }
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxT(" FROM ") + wxString::FromUTF8(xname);
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_prepare_v2(SqliteHandle, xsql, strlen(xsql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      wxMessageBox(wxT("SQL error: ") + err, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      goto abort;
    }
  n_columns = sqlite3_column_count(stmt);
  while (1)
    {
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        break;
      if (ret == SQLITE_ROW)
        {
          fromto = true;
          tofrom = true;
          if (p_graph->IsNodeCode() == true)
            {
              id_from = -1;
              id_to = -1;
          } else
            {
              *code_from = '\0';
              *code_to = '\0';
            }
          // fetching the ROWID 
          rowid = sqlite3_column_int64(stmt, 0);
          // fetching the NodeFrom value
          if (p_graph->IsNodeCode() == true)
            strcpy(code_from, (char *) sqlite3_column_text(stmt, 1));
          else
            id_from = sqlite3_column_int64(stmt, 1);
          // fetching the NodeTo value
          if (p_graph->IsNodeCode() == true)
            strcpy(code_to, (char *) sqlite3_column_text(stmt, 2));
          else
            id_to = sqlite3_column_int64(stmt, 2);
          // fetching the NodeFromX value
          node_from_x = sqlite3_column_double(stmt, 3);
          // fetching the NodeFromY value 
          node_from_y = sqlite3_column_double(stmt, 4);
          // fetching the NodeFromX value 
          node_to_x = sqlite3_column_double(stmt, 5);
          // fetching the NodeFromY value 
          node_to_y = sqlite3_column_double(stmt, 6);
          // fetching the Cost value 
          cost_val = sqlite3_column_double(stmt, 7);
          if (one_way == true)
            {
              // fetching the OneWay-FromTo value
              fromto = sqlite3_column_int(stmt, fromto_n);
              // fetching the OneWay-ToFrom value
              tofrom = sqlite3_column_int(stmt, tofrom_n);
            }
          if (cost_val <= 0.0)
            p_graph->SetError();
          if (aStarLength == true)
            {
              // supporting A* - fetching the arc length
              a_star_length = sqlite3_column_double(stmt, 8);
              a_star_coeff = cost_val / a_star_length;
              if (a_star_coeff < min_a_star_coeff)
                min_a_star_coeff = a_star_coeff;
            }
          if (one_way == true)
            {
              // fetching the OneWay-FromTo value
              fromto = sqlite3_column_int(stmt, fromto_n);
              // fetching the OneWay-ToFrom value
              tofrom = sqlite3_column_int(stmt, tofrom_n);
            }
          if (bidirectional == true)
            {
              if (fromto)
                {
                  if (p_graph->IsNodeCode() == true)
                    p_graph->AddArc(rowid, code_from, code_to, node_from_x,
                                    node_from_y, node_to_x, node_to_y,
                                    cost_val);
                  else
                    p_graph->AddArc(rowid, id_from, id_to, node_from_x,
                                    node_from_y, node_to_x, node_to_y,
                                    cost_val);
                }
              if (tofrom)
                {
                  if (p_graph->IsNodeCode() == true)
                    p_graph->AddArc(rowid, code_to, code_from, node_to_x,
                                    node_to_y, node_from_x, node_from_y,
                                    cost_val);
                  else
                    p_graph->AddArc(rowid, id_to, id_from, node_to_x, node_to_y,
                                    node_from_x, node_from_y, cost_val);
                }
          } else
            {
              if (p_graph->IsNodeCode() == true)
                p_graph->AddArc(rowid, code_from, code_to, node_from_x,
                                node_from_y, node_to_x, node_to_y, cost_val);
              else
                p_graph->AddArc(rowid, id_from, id_to, node_from_x, node_from_y,
                                node_to_x, node_to_y, cost_val);
            }
          if (p_graph->IsError() == true)
            {
              sqlite3_finalize(stmt);
              goto abort;
            }
      } else
        {
          wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          wxMessageBox(wxT("sqlite3_step error: ") + err, wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_finalize(stmt);
          goto abort;
        }
    }
  sqlite3_finalize(stmt);
  ::wxEndBusyCursor();
  wr =
    CreateNetwork(p_graph, table, from, to, geometry, name, aStarSupported,
                  min_a_star_coeff);
  if (wr == true)
    {
      endMsg =
        wxT("OK: VirtualNetwork table '") + table +
        wxT("_net' successfully created");
      wxMessageBox(endMsg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                   this);
  } else
    {
      endMsg =
        wxT("DB ERROR: VirtualNetwork table '") + table +
        wxT("_net' was not created");
      wxMessageBox(endMsg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
    }
  if (p_graph)
    delete p_graph;
  InitTableTree();
  return;
abort:
  ::wxEndBusyCursor();
  msg =
    wxT
    ("It's impossible to build a Network using the given configuration;\nsome fatal error occurred\n\n");
  msg += wxT("please note: using the 'spatialite_network' command-line tool\n");
  msg +=
    wxT
    ("you can obtain a full detailed report explaining causes for this failure");
  wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  if (p_graph)
    delete p_graph;
}

void
  MyFrame::OutputNetNode(unsigned char *auxbuf, int *size, int ind,
                         bool node_code, int max_node_length, NetNode * pN,
                         int endian_arch, bool aStarSupported)
{
//
// exporting a Node into NETWORK-DATA
//
  int n_star;
  int i;
  NetArc **arc_array;
  NetArc *pA;
  unsigned char *out = auxbuf;
  *out++ = GAIA_NET_NODE;
  gaiaExport32(out, ind, 1, endian_arch); // the Node internal index 
  out += 4;
  if (node_code)
    {
      // Nodes are identified by a TEXT Code 
      memset(out, '\0', max_node_length);
      strcpy((char *) out, pN->GetCode().ToUTF8());
      out += max_node_length;
  } else
    {
      // Nodes are identified by an INTEGER Id 
      gaiaExportI64(out, pN->GetId(), 1, endian_arch);
      out += 8;
    }
  if (aStarSupported)
    {
      // in order to support the A* algorithm [X,Y] are required for each node
      gaiaExport64(out, pN->GetX(), 1, endian_arch);
      out += 8;
      gaiaExport64(out, pN->GetY(), 1, endian_arch);
      out += 8;
    }
  arc_array = pN->PrepareOutcomings(&n_star);
  gaiaExport16(out, n_star, 1, endian_arch);  // # of outcoming arcs
  out += 2;
  for (i = 0; i < n_star; i++)
    {
      // exporting the outcoming arcs 
      pA = *(arc_array + i);
      *out++ = GAIA_NET_ARC;
      gaiaExportI64(out, pA->GetRowId(), 1, endian_arch); // the Arc rowid
      out += 8;
      gaiaExport32(out, pA->GetTo()->GetInternalIndex(), 1, endian_arch); // the ToNode internal index
      out += 4;
      gaiaExport64(out, pA->GetCost(), 1, endian_arch); // the Arc Cost 
      out += 8;
      *out++ = GAIA_NET_END;
    }
  if (arc_array)
    delete[]arc_array;
  *out++ = GAIA_NET_END;
  *size = out - auxbuf;
}

bool MyFrame::CreateNetwork(Network * p_graph, wxString & table,
                            wxString & from, wxString & to, wxString & geometry,
                            wxString & name, bool aStarSupported,
                            double aStarCoeff)
{
//
// creates the NETWORK-DATA table 
//
  int ret;
  wxString sql;
  char xsql[1024];
  char *errMsg = NULL;
  unsigned char *auxbuf = new unsigned char[MAX_BLOCK];
  unsigned char *buf = new unsigned char[MAX_BLOCK];
  unsigned char *out;
  sqlite3_stmt *stmt;
  int i;
  int size;
  int endian_arch = gaiaEndianArch();
  NetNode *pN;
  int pk = 0;
  int nodes_cnt = 0;
  int len;
  bool net_data_exists = false;
  bool net_exists = false;
  bool delete_existing = false;
  char xname[1024];
  wxString data_table = table + wxT("_net_data");
  wxString net_table = table + wxT("_net");
  net_data_exists = TableAlreadyExists(data_table);
  net_exists = TableAlreadyExists(net_table);
  if (net_data_exists == true || net_exists == true)
    {
      // asking permission to overwrite existing tables
      wxString msg;
      if (net_data_exists == true)
        msg += wxT("A table named '") + data_table + wxT("' already exists\n");
      if (net_exists == true)
        msg += wxT("A table named '") + net_table + wxT("' already exists\n");
      msg += wxT("\nDo you allow DROPping existing table(s) ?");
      wxMessageDialog confirm(this, msg, wxT("Confirm overwrite"),
                              wxYES_NO | wxICON_QUESTION);
      ret = confirm.ShowModal();
      if (ret == wxID_YES)
        delete_existing = true;
    }
  ::wxBeginBusyCursor();
  for (i = 0; i < p_graph->GetNumNodes(); i++)
    {
      // setting the internal index to each Node 
      pN = p_graph->GetSortedNode(i);
      pN->SetInternalIndex(i);
    }
// starts a transaction 
  ret = sqlite3_exec(SqliteHandle, "BEGIN", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("BEGIN error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (delete_existing == true)
    {
      strcpy(xname, net_table.ToUTF8());
      DoubleQuotedSql(xname);
      sql = wxT("DROP TABLE IF EXISTS ") + wxString::FromUTF8(xname);
      strcpy(xsql, sql.ToUTF8());
      ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("DROP TABLE error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto abort;
        }
      strcpy(xname, data_table.ToUTF8());
      DoubleQuotedSql(xname);
      sql = wxT("DROP TABLE IF EXISTS ") + wxString::FromUTF8(xname);
      strcpy(xsql, sql.ToUTF8());
      ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("DROP TABLE error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto abort;
        }
    }
// creating the NETWORK-DATA table 
  strcpy(xname, data_table.ToUTF8());
  DoubleQuotedSql(xname);
  sql = wxT("CREATE TABLE ") + wxString::FromUTF8(xname);
  sql += wxT(" (Id INTEGER PRIMARY KEY, NetworkData BLOB NOT NULL)");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE TABLE error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// preparing the SQL statement
  strcpy(xname, data_table.ToUTF8());
  DoubleQuotedSql(xname);
  sql =
    wxT("INSERT INTO ") + wxString::FromUTF8(xname) +
    wxT(" (Id, NetworkData) VALUES (?, ?)");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_prepare_v2(SqliteHandle, xsql, strlen(xsql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
      wxMessageBox(wxT("INSERT error: ") + err, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      goto abort;
    }
  if (pk == 0)
    {
      // preparing the HEADER block 
      out = buf;
      if (aStarSupported)
        *out++ = GAIA_NET64_A_STAR_START;
      else
        *out++ = GAIA_NET64_START;
      *out++ = GAIA_NET_HEADER;
      gaiaExport32(out, p_graph->GetNumNodes(), 1, endian_arch);  // how many Nodes are there
      out += 4;
      if (p_graph->IsNodeCode() == true)
        *out++ = GAIA_NET_CODE; // Nodes are identified by a TEXT code 
      else
        *out++ = GAIA_NET_ID;   // Nodes are identified by an INTEGER id 
      if (p_graph->IsNodeCode() == true)
        *out++ = p_graph->GetMaxCodeLength(); // max TEXT code length
      else
        *out++ = 0x00;
      // inserting the main Table name 
      *out++ = GAIA_NET_TABLE;
      len = table.Len() + 1;
      gaiaExport16(out, len, 1, endian_arch); // the Table Name length, including last '\0'
      out += 2;
      memset(out, '\0', len);
      strcpy((char *) out, table.ToUTF8());
      out += len;
      // inserting the NodeFrom column name 
      *out++ = GAIA_NET_FROM;
      len = from.Len() + 1;
      gaiaExport16(out, len, 1, endian_arch); // the NodeFrom column Name length, including last '\0'
      out += 2;
      memset(out, '\0', len);
      strcpy((char *) out, from.ToUTF8());
      out += len;
      // inserting the NodeTo column name
      *out++ = GAIA_NET_TO;
      len = to.Len() + 1;
      gaiaExport16(out, len, 1, endian_arch); // the NodeTo column Name length, including last '\0'
      out += 2;
      memset(out, '\0', len);
      strcpy((char *) out, to.ToUTF8());
      out += len;
      // inserting the Geometry column name
      *out++ = GAIA_NET_GEOM;
      len = geometry.Len() + 1;
      gaiaExport16(out, len, 1, endian_arch); // the Geometry column Name length, including last '\0'
      out += 2;
      memset(out, '\0', len);
      strcpy((char *) out, geometry.ToUTF8());
      out += len;
      // inserting the Name column name - may be empty
      *out++ = GAIA_NET_NAME;
      if (name.Len() == 0)
        len = 1;
      else
        len = name.Len() + 1;
      gaiaExport16(out, len, 1, endian_arch); // the Name column Name length, including last '\0'
      out += 2;
      memset(out, '\0', len);
      if (name.Len() > 0)
        strcpy((char *) out, name.ToUTF8());
      out += len;
      if (aStarSupported)
        {
          // inserting the A* Heuristic Coeff
          *out++ = GAIA_NET_A_STAR_COEFF;
          gaiaExport64(out, aStarCoeff, 1, endian_arch);
          out += 8;
        }
      *out++ = GAIA_NET_END;
      // INSERTing the Header block 
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_int64(stmt, 1, pk);
      sqlite3_bind_blob(stmt, 2, buf, out - buf, SQLITE_STATIC);
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE || ret == SQLITE_ROW)
        ;
      else
        {
          wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          wxMessageBox(wxT("sqlite3_step error: ") + err, wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_finalize(stmt);
          goto abort;
        }
      pk++;
      // preparing a new block 
      out = buf;
      *out++ = GAIA_NET_BLOCK;
      gaiaExport16(out, 0, 1, endian_arch); // how many Nodes are into this block 
      out += 2;
      nodes_cnt = 0;
    }
  for (i = 0; i < p_graph->GetNumNodes(); i++)
    {
      // looping on each Node 
      pN = p_graph->GetSortedNode(i);
      OutputNetNode(auxbuf, &size, i, p_graph->IsNodeCode(),
                    p_graph->GetMaxCodeLength(), pN, endian_arch,
                    aStarSupported);
      if (size >= (MAX_BLOCK - (out - buf)))
        {
          // inserting the last block 
          gaiaExport16(buf + 1, nodes_cnt, 1, endian_arch); // how many Nodes are into this block
          sqlite3_reset(stmt);
          sqlite3_clear_bindings(stmt);
          sqlite3_bind_int64(stmt, 1, pk);
          sqlite3_bind_blob(stmt, 2, buf, out - buf, SQLITE_STATIC);
          ret = sqlite3_step(stmt);
          if (ret == SQLITE_DONE || ret == SQLITE_ROW)
            ;
          else
            {
              wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
              wxMessageBox(wxT("sqlite3_step error: ") + err,
                           wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
              sqlite3_finalize(stmt);
              goto abort;
            }
          pk++;
          // preparing a new block 
          out = buf;
          *out++ = GAIA_NET_BLOCK;
          gaiaExport16(out, 0, 1, endian_arch); // how many Nodes are into this block
          out += 2;
          nodes_cnt = 0;
        }
      // inserting the current Node into the block 
      nodes_cnt++;
      memcpy(out, auxbuf, size);
      out += size;
    }
  if (nodes_cnt)
    {
      // inserting the last block
      gaiaExport16(buf + 1, nodes_cnt, 1, endian_arch); // how many Nodes are into this block
      sqlite3_reset(stmt);
      sqlite3_clear_bindings(stmt);
      sqlite3_bind_int64(stmt, 1, pk);
      sqlite3_bind_blob(stmt, 2, buf, out - buf, SQLITE_STATIC);
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE || ret == SQLITE_ROW)
        ;
      else
        {
          wxString err = wxString::FromUTF8(sqlite3_errmsg(SqliteHandle));
          wxMessageBox(wxT("sqlite3_step error: ") + err, wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_finalize(stmt);
          goto abort;
        }
    }
  sqlite3_finalize(stmt);
// creating the VirtualNetwork NET-table
  strcpy(xname, net_table.ToUTF8());
  DoubleQuotedSql(xname);
  sql = wxT("CREATE VIRTUAL TABLE ") + wxString::FromUTF8(xname);
  sql += wxT(" USING VirtualNetwork(");
  strcpy(xname, data_table.ToUTF8());
  DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname) + wxT(")");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE VIRTUAL TABLE error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// commits the transaction 
  ret = sqlite3_exec(SqliteHandle, "COMMIT", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("COMMIT error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (buf)
    delete[]buf;
  if (auxbuf)
    delete[]auxbuf;
  ::wxEndBusyCursor();
  return true;
abort:
  ::wxEndBusyCursor();
  if (buf)
    delete[]buf;
  if (auxbuf)
    delete[]auxbuf;
  return true;
  return false;
}

int cmp_prenodes_code(const void *p1, const void *p2)
{
//
// compares two preliminary nodes  by CODE [for QSORT] 
//
  NetNodePre *pP1 = *((NetNodePre **) p1);
  NetNodePre *pP2 = *((NetNodePre **) p2);
  return pP1->GetCode().Cmp(pP2->GetCode());
}

int cmp_prenodes_id(const void *p1, const void *p2)
{
//
// compares two preliminary nodes  by ID [for QSORT] 
//
  NetNodePre *pP1 = *((NetNodePre **) p1);
  NetNodePre *pP2 = *((NetNodePre **) p2);
  if (pP1->GetId() == pP2->GetId())
    return 0;
  if (pP1->GetId() > pP2->GetId())
    return 1;
  return -1;
}

int cmp_nodes2_code(const void *p1, const void *p2)
{
//
// compares two nodes  by CODE [for BSEARCH] 
//
  NetNode *pN1 = (NetNode *) p1;
  NetNode *pN2 = *((NetNode **) p2);
  return pN1->GetCode().Cmp(pN2->GetCode());
}

int cmp_nodes2_id(const void *p1, const void *p2)
{
//
// compares two nodes  by ID [for BSEARCH] 
//
  NetNode *pN1 = (NetNode *) p1;
  NetNode *pN2 = *((NetNode **) p2);
  if (pN1->GetId() == pN2->GetId())
    return 0;
  if (pN1->GetId() > pN2->GetId())
    return 1;
  return -1;
}

int cmp_nodes1_code(const void *p1, const void *p2)
{
//
// compares two nodes  by CODE [for QSORT] 
//
  NetNode *pN1 = *((NetNode **) p1);
  NetNode *pN2 = *((NetNode **) p2);
  return pN1->GetCode().Cmp(pN2->GetCode());
}

int cmp_nodes1_id(const void *p1, const void *p2)
{
//
// compares two nodes  by ID [for QSORT ]
//
  NetNode *pN1 = *((NetNode **) p1);
  NetNode *pN2 = *((NetNode **) p2);
  if (pN1->GetId() == pN2->GetId())
    return 0;
  if (pN1->GetId() > pN2->GetId())
    return 1;
  return -1;
}

NetNodePre::NetNodePre(sqlite3_int64 id)
{
//
// Network Node [preliminary] constructor
//
  Id = id;
  Code = wxT("");
  Next = NULL;
}

NetNodePre::NetNodePre(const char *code)
{
//
// Network Node [preliminary] constructor
//
  Id = -1;
  Code = wxString::FromUTF8(code);
  Code.Truncate(30);
  Next = NULL;
}

NetNode::NetNode(sqlite3_int64 id)
{
//
// Network Node [final] constructor
//
  InternalIndex = -1;
  Id = id;
  Code = wxT("");
  X = DBL_MAX;
  Y = DBL_MAX;
  First = NULL;
  Last = NULL;
  Next = NULL;
}

NetNode::NetNode(wxString & code)
{
//
// Network Node [final] constructor
//
  InternalIndex = -1;
  Id = -1;
  Code = code;
  X = DBL_MAX;
  Y = DBL_MAX;
  First = NULL;
  Last = NULL;
  Next = NULL;
}

NetNode::~NetNode()
{
// Network Node [final] destructor
  NetArcRef *pAR;
  NetArcRef *pARn;
  pAR = First;
  while (pAR)
    {
      pARn = pAR->GetNext();
      delete pAR;
      pAR = pARn;
    }
}

void NetNode::AddOutcoming(NetArc * pA)
{
//
// adds an outcoming Arc to a Node 
//
  NetArcRef *pAR = new NetArcRef(pA);
  if (!First)
    First = pAR;
  if (Last)
    Last->SetNext(pAR);
  Last = pAR;
}

NetArc **NetNode::PrepareOutcomings(int *count)
{
//
// preparing the outcoming arc array 
//
  NetArc **arc_array;
  int n = 0;
  int i;
  bool ok;
  NetArcRef *pAR;
  NetArc *pA0;
  NetArc *pA1;
  pAR = First;
  while (pAR)
    {
      // counting how many outcoming arcs are there 
      n++;
      pAR = pAR->GetNext();
    }
  if (!n)
    {
      *count = 0;
      return NULL;
    }
  arc_array = new NetArc *[n];
  i = 0;
  pAR = First;
  while (pAR)
    {
      // populating the arcs array 
      *(arc_array + i++) = pAR->GetReference();
      pAR = pAR->GetNext();
    }
  ok = true;
  while (ok == true)
    {
      // bubble sorting the arcs by Cost 
      ok = false;
      for (i = 1; i < n; i++)
        {
          pA0 = *(arc_array + i - 1);
          pA1 = *(arc_array + i);
          if (pA0->GetCost() > pA1->GetCost())
            {
              // swapping the arcs 
              *(arc_array + i - 1) = pA1;
              *(arc_array + i) = pA0;
              ok = true;
            }
        }
    }
  *count = n;
  return arc_array;
}

NetArc::NetArc(sqlite3_int64 rowid, NetNode * from, NetNode * to, double cost)
{
//
// Network Arc constructor
//
  RowId = rowid;
  From = from;
  To = to;
  Cost = cost;
  Next = NULL;
}

Network::Network()
{
//
// Network constructor
//
  FirstPre = NULL;
  LastPre = NULL;
  NumPreNodes = 0;
  SortedPreNodes = NULL;
  NumPreNodes = 0;
  SortedPreNodes = NULL;
  FirstArc = NULL;
  LastArc = NULL;
  FirstNode = NULL;
  LastNode = NULL;
  NumNodes = 0;
  SortedNodes = NULL;
  Error = false;
  NodeCode = false;
  MaxCodeLength = 0;
}

Network::~Network()
{
//
// Network destructor
//
  NetArc *pA;
  NetArc *pAn;
  NetNode *pN;
  NetNode *pNn;
  CleanPreNodes();
  pA = FirstArc;
  while (pA)
    {
      pAn = pA->GetNext();
      delete pA;
      pA = pAn;
    }
  pN = FirstNode;
  while (pN)
    {
      pNn = pN->GetNext();
      delete pN;
      pN = pNn;
    }
  if (SortedNodes)
    delete[]SortedNodes;
}

void Network::CleanPreNodes()
{
//
// cleaning up the preliminary Nodes list
//
  NetNodePre *pP;
  NetNodePre *pPn;
  pP = FirstPre;
  while (pP)
    {
      pPn = pP->GetNext();
      delete pP;
      pP = pPn;
    }
  FirstPre = NULL;
  LastPre = NULL;
  NumPreNodes = 0;
  if (SortedPreNodes)
    delete[]SortedPreNodes;
  SortedPreNodes = NULL;
}

void Network::InsertNode(sqlite3_int64 id)
{
//
// inserts a Node into the preliminary list 
//
  NetNodePre *pP = new NetNodePre(id);
  if (!FirstPre)
    FirstPre = pP;
  if (LastPre)
    LastPre->SetNext(pP);
  LastPre = pP;
}

void Network::InsertNode(const char *code)
{
//
// inserts a Node into the preliminary list 
//
  NetNodePre *pP = new NetNodePre(code);
  if (!FirstPre)
    FirstPre = pP;
  if (LastPre)
    LastPre->SetNext(pP);
  LastPre = pP;
}

void Network::AddNode(sqlite3_int64 id)
{
//
// inserts a Node into the final list 
//
  NetNode *pN = new NetNode(id);
  if (!FirstNode)
    FirstNode = pN;
  if (LastNode)
    LastNode->SetNext(pN);
  LastNode = pN;
}

void Network::AddNode(wxString & code)
{
//
// inserts a Node into the final list 
//
  int len;
  NetNode *pN = new NetNode(code);
  len = pN->GetCode().Len() + 1;
  if (len > MaxCodeLength)
    MaxCodeLength = len;
  if (!FirstNode)
    FirstNode = pN;
  if (LastNode)
    LastNode->SetNext(pN);
  LastNode = pN;
}

NetNode *Network::ProcessNode(sqlite3_int64 id, double x, double y,
                              NetNode ** pOther)
{
//
// inserts a new node or retrieves an already defined one 
//
  NetNode *pN = Find(id);
  *pOther = NULL;
  if (pN)
    {
      // this Node already exists into the sorted list
      if (pN->GetX() == DBL_MAX && pN->GetY() == DBL_MAX)
        {
          pN->SetX(x);
          pN->SetY(y);
      } else
        {
          if (pN->GetX() == x && pN->GetY() == y)
            ;
          else
            *pOther = pN;
        }
      return pN;
    }
// unexpected error; undefined Node
  return NULL;
}

NetNode *Network::ProcessNode(wxString & code, double x, double y,
                              NetNode ** pOther)
{
//
// inserts a new node or retrieves an already defined one 
//
  NetNode *pN = Find(code);
  *pOther = NULL;
  if (pN)
    {
      // this Node already exists into the sorted list
      if (pN->GetX() == DBL_MAX && pN->GetY() == DBL_MAX)
        {
          pN->SetX(x);
          pN->SetY(y);
      } else
        {
          if (pN->GetX() == x && pN->GetY() == y)
            ;
          else
            *pOther = pN;
        }
      return pN;
    }
// unexpected error; undefined Node
  return NULL;
}

void
  Network::AddArc(sqlite3_int64 rowid, sqlite3_int64 id_from,
                  sqlite3_int64 id_to, double node_from_x, double node_from_y,
                  double node_to_x, double node_to_y, double cost)
{
//
// inserting an arc into the memory structures 
//
  NetNode *pFrom;
  NetNode *pTo;
  NetNode *pN2;
  NetArc *pA;
  pFrom = ProcessNode(id_from, node_from_x, node_from_y, &pN2);
  if (pN2)
    Error = true;
  pTo = ProcessNode(id_to, node_to_x, node_to_y, &pN2);
  if (pN2)
    Error = true;
  if (!pFrom)
    Error = true;
  if (!pTo)
    Error = true;
  if (pFrom == pTo)
    Error = true;
  if (Error == true)
    return;
  pA = new NetArc(rowid, pFrom, pTo, cost);
  if (!FirstArc)
    FirstArc = pA;
  if (LastArc)
    LastArc->SetNext(pA);
  LastArc = pA;
// updating Node connections 
  pFrom->AddOutcoming(pA);
}

void
  Network::AddArc(sqlite3_int64 rowid, const char *code_from,
                  const char *code_to, double node_from_x, double node_from_y,
                  double node_to_x, double node_to_y, double cost)
{
//
// inserting an arc into the memory structures 
//
  NetNode *pFrom;
  NetNode *pTo;
  NetNode *pN2;
  NetArc *pA;
  wxString stCode = wxString::FromUTF8(code_from);
  pFrom = ProcessNode(stCode, node_from_x, node_from_y, &pN2);
  if (pN2)
    Error = true;
  stCode = wxString::FromUTF8(code_to);
  pTo = ProcessNode(stCode, node_to_x, node_to_y, &pN2);
  if (pN2)
    Error = true;
  if (!pFrom)
    Error = true;
  if (!pTo)
    Error = true;
  if (pFrom == pTo)
    Error = true;
  if (Error == true)
    return;
  pA = new NetArc(rowid, pFrom, pTo, cost);
  if (!FirstArc)
    FirstArc = pA;
  if (LastArc)
    LastArc->SetNext(pA);
  LastArc = pA;
// updating Node connections 
  pFrom->AddOutcoming(pA);
}

void Network::Sort()
{
//
// updating the Nodes sorted list 
//
  int i;
  NetNode *pN;
  NumNodes = 0;
  if (SortedNodes)
    {
      // we must free the already existent sorted list
      delete[]SortedNodes;
    }
  SortedNodes = NULL;
  pN = FirstNode;
  while (pN)
    {
      NumNodes++;
      pN = pN->GetNext();
    }
  if (!NumNodes)
    return;
  SortedNodes = new NetNode *[NumNodes];
  i = 0;
  pN = FirstNode;
  while (pN)
    {
      *(SortedNodes + i++) = pN;
      pN = pN->GetNext();
    }
  if (NodeCode == true)
    {
      // Nodes are identified by a TEXT code 
      qsort(SortedNodes, NumNodes, sizeof(NetNode *), cmp_nodes1_code);
  } else
    {
      // Nodes are identified by an INTEGER id 
      qsort(SortedNodes, NumNodes, sizeof(NetNode *), cmp_nodes1_id);
    }
}

NetNode *Network::GetSortedNode(sqlite3_int64 x)
{
//
// return a sorted Node [by position]
//
  if (x >= 0 && x < NumNodes)
    return *(SortedNodes + x);
  return NULL;
}

NetNode *Network::Find(sqlite3_int64 id)
{
//
// searching a Node into the sorted list 
//
  NetNode **ret;
  NetNode pN(id);
  if (!SortedNodes)
    return NULL;
// Nodes are identified by an INTEGER id
  ret =
    (NetNode **) bsearch(&pN, SortedNodes, NumNodes, sizeof(NetNode *),
                         cmp_nodes2_id);
  if (!ret)
    return NULL;
  return *ret;
}

NetNode *Network::Find(wxString & code)
{
//
// searching a Node into the sorted list 
//
  NetNode **ret;
  NetNode pN(code);
  if (!SortedNodes)
    return NULL;
// Nodes are identified by a TEXT code
  ret =
    (NetNode **) bsearch(&pN, SortedNodes, NumNodes, sizeof(NetNode *),
                         cmp_nodes2_code);
  if (!ret)
    return NULL;
  return *ret;
}

void Network::InitNodes()
{
//
// prepares the final Nodes list 
//
  sqlite3_int64 last_id;
  wxString last_code;
  int i;
  NetNodePre *pP;
  NumPreNodes = 0;
// sorting preliminary nodes 
  if (SortedPreNodes)
    {
      // we must free the already existent sorted list
      delete[]SortedPreNodes;
    }
  SortedPreNodes = NULL;
  pP = FirstPre;
  while (pP)
    {
      NumPreNodes++;
      pP = pP->GetNext();
    }
  if (!NumPreNodes)
    return;
  SortedPreNodes = new NetNodePre *[NumPreNodes];
  i = 0;
  pP = FirstPre;
  while (pP)
    {
      *(SortedPreNodes + i++) = pP;
      pP = pP->GetNext();
    }
  if (NodeCode == true)
    {
      // Nodes are identified by a TEXT code
      qsort(SortedPreNodes, NumPreNodes, sizeof(NetNodePre *),
            cmp_prenodes_code);
  } else
    {
      // Nodes are identified by an INTEGER id 
      qsort(SortedPreNodes, NumPreNodes, sizeof(NetNodePre *), cmp_prenodes_id);
    }
// creating the final Nodes linked list 
  last_id = -1;
  last_code = wxT("");
  for (i = 0; i < NumPreNodes; i++)
    {
      pP = *(SortedPreNodes + i);
      if (NodeCode == true)
        {
          // Nodes are identified by a TEXT code
          if (pP->GetCode().Cmp(last_code) != 0)
            AddNode(pP->GetCode());
      } else
        {
          // Nodes are identified by an INTEGER id
          if (pP->GetId() != last_id)
            AddNode(pP->GetId());
        }
      last_id = pP->GetId();
      last_code = pP->GetCode();
    }
// sorting the final Nodes list 
  Sort();
// cleaning up the preliminary Nodes structs 
  CleanPreNodes();
}
