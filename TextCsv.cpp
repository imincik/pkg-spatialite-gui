/*
/ TextCsv.cpp
/ methods related to CSV/TXT loading 
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

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp	_stricmp
#endif

static void text_clean_integer(char *value)
{
/* cleaning an integer value */
  char last;
  char buffer[35536];
  int len = strlen(value);
  last = value[len - 1];
  if (last == '-' || last == '+')
    {
      /* trailing sign; transforming into a leading sign */
      *buffer = last;
      strcpy(buffer + 1, value);
      buffer[len - 1] = '\0';
      strcpy(value, buffer);
    }
}

static void text_clean_double(char *value)
{
/* cleaning an integer value */
  char *p;
  char last;
  char buffer[35536];
  int len = strlen(value);
  last = value[len - 1];
  if (last == '-' || last == '+')
    {
      /* trailing sign; transforming into a leading sign */
      *buffer = last;
      strcpy(buffer + 1, value);
      buffer[len - 1] = '\0';
      strcpy(value, buffer);
    }
  p = value;
  while (*p != '\0')
    {
      /* transforming COMMAs into POINTs */
      if (*p == ',')
        *p = '.';
      p++;
    }
}

void
  MyFrame::LoadText(wxString & path, wxString & table, wxString & charset,
                    bool first_titles, const char decimal_separator,
                    const char separator, const char text_separator)
{
//
// loading a CSV/TXT as a new DB table
//
  gaiaTextReaderPtr text = NULL;
  int seed;
  int dup;
  int idup;
  char dummy[65536];
  char dummyName[4096];
  char **col_name = NULL;
  int i;
  char sql[65536];
  int len;
  int ret;
  int rows = 0;
  char *errMsg = NULL;
  bool sqlError = false;
  char xname[1024];
  int current_row;
  wxString msg;
  char buf[4096];
  int type;
  const char *value;
//
// performing some checks before starting
//
  if (TableAlreadyExists(table) == true)
    {
      wxMessageBox(wxT("a table named '") + table + wxT("' already exists"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
  text = gaiaTextReaderAlloc(path.ToUTF8(), separator,
                             text_separator, decimal_separator,
                             first_titles, charset.ToUTF8());
  if (text)
    {
      if (gaiaTextReaderParse(text) == 0)
        {
          gaiaTextReaderDestroy(text);
          text = NULL;
        }
    }
  if (!text)
    return;
  ::wxBeginBusyCursor();
//
// checking for duplicate / illegal column names and antialising them 
//
  col_name = (char **) malloc(sizeof(char *) * text->max_fields);
  seed = 0;
  for (i = 0; i < text->max_fields; i++)
    {
      strcpy(dummyName, text->columns[i].name);
      dup = 0;
      for (idup = 0; idup < i; idup++)
        {
          if (strcasecmp(dummyName, *(col_name + idup)) == 0)
            dup = 1;
        }
      if (strcasecmp(dummyName, "PK_UID") == 0)
        dup = 1;
      if (dup)
        sprintf(dummyName, "DUPCOL_%d", seed++);
      len = strlen(dummyName);
      *(col_name + i) = (char *) malloc(len + 1);
      strcpy(*(col_name + i), dummyName);
    }
//
// starting a transaction
//
  ret = sqlite3_exec(SqliteHandle, "BEGIN", NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("load CSV/TXT error:") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      sqlError = true;
      goto clean_up;
    }
//
// creating the Table 
//
  strcpy(xname, table.ToUTF8());
  DoubleQuotedSql(xname);
  sprintf(sql, "CREATE TABLE %s", xname);
  strcat(sql, " (\nPK_UID INTEGER PRIMARY KEY AUTOINCREMENT");
  for (i = 0; i < text->max_fields; i++)
    {
      strcat(sql, ",\n");
      strcpy(xname, *(col_name + i));
      DoubleQuotedSql(xname);
      strcat(sql, xname);
      if (text->columns[i].type == VRTTXT_INTEGER)
        strcat(sql, " INTEGER");
      else if (text->columns[i].type == VRTTXT_DOUBLE)
        strcat(sql, " DOUBLE");
      else
        strcat(sql, " TEXT");
    }
  strcat(sql, ")");
  ret = sqlite3_exec(SqliteHandle, sql, NULL, 0, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("load text error:") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      sqlError = true;
      goto clean_up;
    }
  current_row = 0;
  while (current_row < text->num_rows)
    {
      //
      // inserting rows from CSV/TXT
      //
      if (!gaiaTextReaderGetRow(text, current_row))
        break;
      strcpy(xname, table.ToUTF8());
      DoubleQuotedSql(xname);
      sprintf(sql, "INSERT INTO %s (\nPK_UID", xname);
      for (i = 0; i < text->max_fields; i++)
        {
          // columns corresponding to some CSV/TXT column
          strcat(sql, ",");
          strcpy(xname, *(col_name + i));
          DoubleQuotedSql(xname);
          strcat(sql, xname);
        }
      strcat(sql, ")\nVALUES (");
      sprintf(dummy, "%d", current_row);
      strcat(sql, dummy);
      for (i = 0; i < text->max_fields; i++)
        {
          // column values
          strcat(sql, ",");
          if (!gaiaTextReaderFetchField(text, i, &type, &value))
            strcat(sql, "NULL");
          else
            {
              if (type == VRTTXT_INTEGER)
                {
                  strcpy(buf, value);
                  text_clean_integer(buf);
#if defined(_WIN32) || defined(__MINGW32__)
/* CAVEAT - M$ runtime has non-standard functions for 64 bits */
                  sprintf(dummy, "%I64d", _atoi64(buf));
#else
                  sprintf(dummy, "%lld", atoll(buf));
#endif
                  strcat(sql, dummy);
              } else if (type == VRTTXT_DOUBLE)
                {
                  strcpy(buf, value);
                  text_clean_double(buf);
                  sprintf(dummy, "%1.6f", atof(buf));
                  strcat(sql, dummy);
              } else if (type == VRTTXT_TEXT)
                {
                  void *ptr = (void *) value;
                  strcpy(dummy, value);
                  free(ptr);
                  CleanSqlString(dummy);
                  strcat(sql, "'");
                  strcat(sql, dummy);
                  strcat(sql, "'");
              } else
                strcat(sql, "NULL");
            }
        }
      strcat(sql, ")");
      ret = sqlite3_exec(SqliteHandle, sql, NULL, 0, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("load text error:") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          sqlError = true;
          goto clean_up;
        }
      rows++;
      current_row++;
    }
clean_up:
  if (col_name)
    {
      // releasing memory allocation for column names 
      for (i = 0; i < text->max_fields; i++)
        free(*(col_name + i));
      free(col_name);
    }
  free(text);
  if (sqlError == true)
    {
      // some error occurred - ROLLBACK 
      ret = sqlite3_exec(SqliteHandle, "ROLLBACK", NULL, 0, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("load text error:") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
        }
      ::wxEndBusyCursor();
      msg =
        wxT("CSV/TXT not loaded\n\n\na ROLLBACK was automatically performed");
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
  } else
    {
      // ok - confirming pending transaction - COMMIT 
      ret = sqlite3_exec(SqliteHandle, "COMMIT", NULL, 0, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("load text error:") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      ::wxEndBusyCursor();
      sprintf(dummy, "CSV/TXT loaded\n\n%d inserted rows", rows);
      msg = wxString::FromUTF8(dummy);
      wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
      InitTableTree();
    }
}
