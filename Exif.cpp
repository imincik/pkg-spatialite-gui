/*
/ Exif.cpp
/ methods related to EXIF import and XmlBLOB import/export
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

#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#endif
#include <float.h>

#include <wx/filename.h>

#include <spatialite/gg_dxf.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp	_stricmp
#endif

void MyFrame::ImportExifPhotos(wxString & path, bool folder, bool metadata,
                               bool gps_only)
{
//
// trying to import EXIF photos
// 
  int cnt;
  char msg[256];
  ::wxBeginBusyCursor();
  if (CheckExifTables() == false)
    {
      ::wxEndBusyCursor();
      wxMessageBox(wxT
                   ("An EXIF table is already defined, but has incompatibles columns"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
  if (folder == true)
    cnt = ExifLoadDir(path, gps_only, metadata);
  else
    cnt = ExifLoadFile(path, gps_only, metadata);
  ::wxEndBusyCursor();
  sprintf(msg, "%d EXIF photo%s succesfully inserted into the DB\n", cnt,
          (cnt > 1) ? "s where" : " was");
  wxMessageBox(wxString::FromUTF8(msg), wxT("spatialite_gui"),
               wxOK | wxICON_INFORMATION, this);
  InitTableTree();
}

bool MyFrame::CheckExifTables()
{
//
// creates the EXIF DB tables / or checks existing ones for validity 
//
  int ret;
  wxString sql;
  char xsql[1024];
  char *errMsg;
  bool ok_photoId;
  bool ok_photo;
  bool ok_pixelX;
  bool ok_pixelY;
  bool ok_cameraMake;
  bool ok_cameraModel;
  bool ok_shotDateTime;
  bool ok_gpsGeometry;
  bool ok_gpsDirection;
  bool ok_gpsTimestamp;
  bool ok_fromPath;
  bool ok_tagId;
  bool ok_tagName;
  bool ok_gpsTag;
  bool ok_valueType;
  bool ok_typeName;
  bool ok_countValues;
  bool ok_valueIndex;
  bool ok_byteValue;
  bool ok_stringValue;
  bool ok_numValue;
  bool ok_numValueBis;
  bool ok_doubleValue;
  bool ok_humanReadable;
  bool err_pk;
  bool ok_photoIdPk;
  bool ok_tagIdPk;
  bool ok_valueIndexPk;
  bool pKey;
  const char *name;
  int i;
  char **results;
  int rows;
  int columns;
// creating the ExifPhoto table 
  sql = wxT("CREATE TABLE IF NOT EXISTS ExifPhoto (\n");
  sql += wxT("PhotoId INTEGER PRIMARY KEY AUTOINCREMENT,\n");
  sql += wxT("Photo BLOB NOT NULL,\n");
  sql += wxT("PixelX INTEGER,\n");
  sql += wxT("PixelY INTEGER,\n");
  sql += wxT("CameraMake TEXT,\n");
  sql += wxT("CameraModel TEXT,\n");
  sql += wxT("ShotDateTime DOUBLE,\n");
  sql += wxT("GpsGeometry BLOB,\n");
  sql += wxT("GpsDirection DOUBLE, ");
  sql += wxT("GpsSatellites TEXT,\n");
  sql += wxT("GpsTimestamp DOUBLE,\n");
  sql += wxT("FromPath TEXT");
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE TABLE ExifPhoto error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// checking the ExifPhoto table for sanity 
  ok_photoId = false;
  ok_photo = false;
  ok_pixelX = false;
  ok_pixelY = false;
  ok_cameraMake = false;
  ok_cameraModel = false;
  ok_shotDateTime = false;
  ok_gpsGeometry = false;
  ok_gpsDirection = false;
  ok_gpsTimestamp = false;
  ok_fromPath = false;
  ok_photoIdPk = false;
  err_pk = false;
  strcpy(xsql, "PRAGMA table_info(ExifPhoto)");
  ret =
    sqlite3_get_table(SqliteHandle, xsql, &results, &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("PRAGMA table_info(ExifPhoto) error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (atoi(results[(i * columns) + 5]) == 0)
            pKey = false;
          else
            pKey = true;
          if (strcasecmp(name, "PhotoId") == 0)
            ok_photoId = true;
          if (strcasecmp(name, "Photo") == 0)
            ok_photo = true;
          if (strcasecmp(name, "PixelX") == 0)
            ok_pixelX = true;
          if (strcasecmp(name, "PixelY") == 0)
            ok_pixelY = true;
          if (strcasecmp(name, "CameraMake") == 0)
            ok_cameraMake = true;
          if (strcasecmp(name, "CameraModel") == 0)
            ok_cameraModel = true;
          if (strcasecmp(name, "ShotDateTime") == 0)
            ok_shotDateTime = true;
          if (strcasecmp(name, "GpsGeometry") == 0)
            ok_gpsGeometry = true;
          if (strcasecmp(name, "GpsDirection") == 0)
            ok_gpsDirection = true;
          if (strcasecmp(name, "GpsTimestamp") == 0)
            ok_gpsTimestamp = true;
          if (strcasecmp(name, "FromPath") == 0)
            ok_fromPath = true;
          if (pKey == true)
            {
              if (strcasecmp(name, "PhotoId") == 0)
                ok_photoIdPk = true;
              else
                err_pk = true;
            }
        }
    }
  sqlite3_free_table(results);
  if (ok_photoId == true && ok_photo == true && ok_pixelX == true
      && ok_pixelY == true && ok_cameraMake == true && ok_cameraModel == true
      && ok_shotDateTime == true && ok_gpsGeometry == true
      && ok_gpsDirection == true && ok_gpsTimestamp == true
      && ok_fromPath == true && ok_photoIdPk == true && err_pk == false)
    ;
  else
    {
      wxMessageBox(wxT
                   ("ERROR: table ExifPhoto already exists, but has incompatible columns"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// creating the ExifTags table 
  sql = wxT("CREATE TABLE IF NOT EXISTS ExifTags (\n");
  sql += wxT("PhotoId INTEGER NOT NULL,\n");
  sql += wxT("TagId INTEGER NOT NULL,\n");
  sql += wxT("TagName TEXT NOT NULL,\n");
  sql += wxT("GpsTag INTEGER NOT NULL CHECK (GpsTag IN (0, 1)),\n");
  sql +=
    wxT
    ("ValueType INTEGER NOT NULL CHECK (ValueType IN (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12)),\n");
  sql += wxT("TypeName TEXT NOT NULL,\n");
  sql += wxT("CountValues INTEGER NOT NULL,\n");
  sql += wxT("PRIMARY KEY (PhotoId, TagId)");
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE TABLE ExifTags error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// checking the ExifTags table for sanity 
  ok_photoId = false;
  ok_tagId = false;
  ok_tagName = false;
  ok_gpsTag = false;
  ok_valueType = false;
  ok_typeName = false;
  ok_countValues = false;
  ok_photoIdPk = false;
  ok_tagIdPk = false;
  err_pk = false;
  strcpy(xsql, "PRAGMA table_info(ExifTags)");
  ret =
    sqlite3_get_table(SqliteHandle, xsql, &results, &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("PRAGMA table_info(ExifTags) error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (atoi(results[(i * columns) + 5]) == 0)
            pKey = false;
          else
            pKey = true;
          if (strcasecmp(name, "PhotoId") == 0)
            ok_photoId = true;
          if (strcasecmp(name, "TagId") == 0)
            ok_tagId = true;
          if (strcasecmp(name, "TagName") == 0)
            ok_tagName = true;
          if (strcasecmp(name, "GpsTag") == 0)
            ok_gpsTag = true;
          if (strcasecmp(name, "ValueType") == 0)
            ok_valueType = true;
          if (strcasecmp(name, "TypeName") == 0)
            ok_typeName = true;
          if (strcasecmp(name, "CountValues") == 0)
            ok_countValues = true;
          if (pKey == true)
            {
              if (strcasecmp(name, "PhotoId") == 0)
                ok_photoIdPk = true;
              else if (strcasecmp(name, "TagId") == 0)
                ok_tagIdPk = true;
              else
                err_pk = true;
            }
        }
    }
  sqlite3_free_table(results);
  if (ok_photoId == true && ok_tagId == true && ok_tagName == true
      && ok_gpsTag == true && ok_valueType == true && ok_typeName == true
      && ok_countValues == true && ok_photoIdPk == true && ok_tagIdPk == true
      && err_pk == false)
    ;
  else
    {
      wxMessageBox(wxT
                   ("ERROR: table ExifTags already exists, but has incompatible columns"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// creating the ExifValues table 
  sql = wxT("CREATE TABLE IF NOT EXISTS ExifValues (\n");
  sql += wxT("PhotoId INTEGER NOT NULL,\n");
  sql += wxT("TagId INTEGER NOT NULL,\n");
  sql += wxT("ValueIndex INTEGER NOT NULL,\n");
  sql += wxT("ByteValue BLOB,\n");
  sql += wxT("StringValue TEXT,\n");
  sql += wxT("NumValue INTEGER,\n");
  sql += wxT("NumValueBis INTEGER,\n");
  sql += wxT("DoubleValue DOUBLE,\n");
  sql += wxT("HumanReadable TEXT,\n");
  sql += wxT("PRIMARY KEY (PhotoId, TagId, ValueIndex)");
  sql += wxT(")");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE TABLE ExifValues error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// checking the ExifValues table for sanity 
  ok_photoId = false;
  ok_tagId = false;
  ok_valueIndex = false;
  ok_byteValue = false;
  ok_stringValue = false;
  ok_numValue = false;
  ok_numValueBis = false;
  ok_doubleValue = false;
  ok_humanReadable = false;
  ok_photoIdPk = false;
  ok_tagIdPk = false;
  ok_valueIndexPk = false;
  err_pk = false;
  strcpy(xsql, "PRAGMA table_info(ExifValues)");
  ret =
    sqlite3_get_table(SqliteHandle, xsql, &results, &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("PRAGMA table_info(ExifValues) error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (atoi(results[(i * columns) + 5]) == 0)
            pKey = false;
          else
            pKey = true;
          if (strcasecmp(name, "PhotoId") == 0)
            ok_photoId = true;
          if (strcasecmp(name, "TagId") == 0)
            ok_tagId = true;
          if (strcasecmp(name, "ValueIndex") == 0)
            ok_valueIndex = true;
          if (strcasecmp(name, "ByteValue") == 0)
            ok_byteValue = true;
          if (strcasecmp(name, "StringValue") == 0)
            ok_stringValue = true;
          if (strcasecmp(name, "NumValue") == 0)
            ok_numValue = true;
          if (strcasecmp(name, "NumValueBis") == 0)
            ok_numValueBis = true;
          if (strcasecmp(name, "DoubleValue") == 0)
            ok_doubleValue = true;
          if (strcasecmp(name, "HumanReadable") == 0)
            ok_humanReadable = true;
          if (pKey == true)
            {
              if (strcasecmp(name, "PhotoId") == 0)
                ok_photoIdPk = true;
              else if (strcasecmp(name, "TagId") == 0)
                ok_tagIdPk = true;
              else if (strcasecmp(name, "ValueIndex") == 0)
                ok_valueIndexPk = true;
              else
                err_pk = true;
            }
        }
    }
  sqlite3_free_table(results);
  if (ok_photoId == true && ok_tagId == true && ok_valueIndex == true
      && ok_byteValue == true && ok_stringValue == true && ok_numValue == true
      && ok_numValueBis == true && ok_doubleValue == true
      && ok_humanReadable == true && ok_photoIdPk == true && ok_tagIdPk == true
      && ok_valueIndexPk == true && err_pk == false)
    ;
  else
    {
      wxMessageBox(wxT
                   ("ERROR: table ExifValues already exists, but has incompatible columns"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// creating the ExifView view 
  sql = wxT("CREATE VIEW IF NOT EXISTS ExifMetadata AS\n");
  sql += wxT("SELECT p.PhotoId AS PhotoId, ");
  sql += wxT("t.TagId AS TagId, ");
  sql += wxT("t.TagName AS TagName,");
  sql += wxT("t.GpsTag AS GpsTag,\n");
  sql += wxT("t.ValueType AS ValueType,");
  sql += wxT("t.TypeName AS TypeName, ");
  sql += wxT("t.CountValues AS CountValues, ");
  sql += wxT("v.ValueIndex AS ValueIndex,\n");
  sql += wxT("v.ByteValue AS ByteValue, ");
  sql += wxT("v.StringValue AS StringValue, ");
  sql += wxT("v.NumValue AS NumValue, ");
  sql += wxT("v.NumValueBis AS NumValueBis,\n");
  sql += wxT("v.DoubleValue AS DoubleValue, ");
  sql += wxT("v.HumanReadable AS HumanReadable\n");
  sql += wxT("FROM ExifPhoto AS p, ExifTags AS t, ExifValues AS v\n");
  sql +=
    wxT
    ("WHERE t.PhotoId = p.PhotoId AND v.PhotoId = t.PhotoId AND v.TagId = t.TagId");
  strcpy(xsql, sql.ToUTF8());
  ret = sqlite3_exec(SqliteHandle, xsql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE VIEW ExifMetadata error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  return true;
abort:
  return false;
}

int MyFrame::ExifLoadDir(wxString & path, bool gps_only, bool metadata)
{
//
// importing EXIF files from a whole DIRECTORY 
//
#if defined(_WIN32) && !defined(__MINGW32__)
/* Visual Studio .NET */
  struct _finddata_t c_file;
  intptr_t hFile;
  int cnt = 0;
  wxString filePath;
  if (_chdir(path.ToUTF8()) < 0)
    return 0;
  if ((hFile = _findfirst("*.*", &c_file)) == -1L)
    ;
  else
    {
      while (1)
        {
          if ((c_file.attrib & _A_RDONLY) == _A_RDONLY
              || (c_file.attrib & _A_NORMAL) == _A_NORMAL)
            {
              filePath = path;
              filePath += wxT("/") + wxString::FromUTF8(c_file.name);
              cnt += ExifLoadFile(filePath, gps_only, metadata);
            }
          if (_findnext(hFile, &c_file) != 0)
            break;
        };
      _findclose(hFile);
    }
  return cnt;
#else
/* not Visual Studio .NET */
  int cnt = 0;
  wxString filePath;
  struct dirent *entry;
  DIR *dir = opendir(path.ToUTF8());
  if (!dir)
    return 0;
  while (1)
    {
      // scanning dir-entries
      entry = readdir(dir);
      if (!entry)
        break;
      filePath = path;
      filePath += wxT("/") + wxString::FromUTF8(entry->d_name);
      cnt += ExifLoadFile(filePath, gps_only, metadata);
    }
  closedir(dir);
  return cnt;
#endif
}

int MyFrame::ExifLoadFile(wxString & path, bool gps_only, bool metadata)
{
//
// importing a single EXIF file 
//
  FILE *fl;
  int sz = 0;
  int rd;
  int loaded = 0;
  unsigned char *blob = NULL;
  gaiaExifTagListPtr tag_list = NULL;
  fl = fopen(path.ToUTF8(), "rb");
  if (!fl)
    return 0;
  if (fseek(fl, 0, SEEK_END) == 0)
    sz = ftell(fl);
  if (sz > 14)
    {
      blob = (unsigned char *) malloc(sz);
      rewind(fl);
      rd = fread(blob, 1, sz, fl);
      if (rd == sz)
        {
          tag_list = gaiaGetExifTags(blob, sz);
          if (tag_list)
            {
              if (gps_only && IsExifGps(tag_list) == false)
                goto stop;
              if (UpdateExifTables(blob, sz, tag_list, metadata, path) == false)
                goto stop;
              loaded = 1;
            }
        }
    }
stop:
  if (blob)
    free(blob);
  if (tag_list)
    gaiaExifTagsFree(tag_list);
  fclose(fl);
  return loaded;
}

bool MyFrame::IsExifGps(gaiaExifTagListPtr tag_list)
{
//
// checks if this one is a GPS-tagged EXIF 
//
  bool gps_lat = false;
  bool gps_long = false;
  gaiaExifTagPtr pT = tag_list->First;
  while (pT)
    {
      if (pT->Gps && pT->TagId == 0x04)
        gps_long = true;
      if (pT->Gps && pT->TagId == 0x02)
        gps_lat = true;
      if (gps_long == true && gps_lat == true)
        return true;
      pT = pT->Next;
    }
  return false;
}

bool MyFrame::UpdateExifTables(unsigned char *blob, int sz,
                               gaiaExifTagListPtr tag_list, bool metadata,
                               wxString & path)
{
//
// inserting an EXIF photo into the DB 
//
  int i;
  int iv;
  bool ok;
  int xok;
  int ok_human;
  char tag_name[128];
  gaiaExifTagPtr pT;
  int ret;
  char sql[1024];
  char human[1024];
  wxString make;
  wxString model;
  wxString satellites;
  wxString date;
  wxString timestamp;
  char *errMsg = NULL;
  sqlite3_stmt *stmt;
  sqlite3_int64 pk = 0;
  sqlite3_int64 val64;
  double dblval;
  const char *type_desc;
  double longitude;
  double latitude;
  gaiaGeomCollPtr geom;
  unsigned char *geoblob;
  int geosize;
// starts a transaction 
  strcpy(sql, "BEGIN");
  ret = sqlite3_exec(SqliteHandle, sql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("BEGIN error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
// feeding the ExifPhoto table; preparing the SQL statement
  strcpy(sql,
         "INSERT INTO ExifPhoto (PhotoId, Photo, PixelX, PixelY, CameraMake, CameraModel, ");
  strcat(sql,
         "ShotDateTime, GpsGeometry, GpsDirection, GpsSatellites, GpsTimestamp, FromPath) ");
  strcat(sql,
         "VALUES (NULL, ?, ?, ?, ?, ?, JulianDay(?), ?, ?, ?, JulianDay(?), ?)");
  ret = sqlite3_prepare_v2(SqliteHandle, sql, strlen(sql), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("INSERT INTO ExifPhoto error: ") +
                   wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      goto abort;
    }
  sqlite3_bind_blob(stmt, 1, blob, sz, SQLITE_STATIC);
  val64 = GetPixelX(tag_list, &ok);
  if (ok == true)
    sqlite3_bind_int64(stmt, 2, val64);
  else
    sqlite3_bind_null(stmt, 2);
  val64 = GetPixelY(tag_list, &ok);
  if (ok == true)
    sqlite3_bind_int64(stmt, 3, val64);
  else
    sqlite3_bind_null(stmt, 3);
  GetMake(tag_list, make, &ok);
  if (ok == true)
    sqlite3_bind_text(stmt, 4, make.ToUTF8(), make.Len(), SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 4);
  GetModel(tag_list, model, &ok);
  if (ok == true)
    sqlite3_bind_text(stmt, 5, model.ToUTF8(), model.Len(), SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 5);
  GetDate(tag_list, date, &ok);
  if (ok == true)
    sqlite3_bind_text(stmt, 6, date.ToUTF8(), date.Len(), SQLITE_TRANSIENT);
  else
    sqlite3_bind_text(stmt, 6, "0000-00-00 00:00:00", 19, SQLITE_TRANSIENT);
  GetGpsCoords(tag_list, &longitude, &latitude, &ok);
  if (ok == true)
    {
      geom = gaiaAllocGeomColl();
      geom->Srid = 4326;
      gaiaAddPointToGeomColl(geom, longitude, latitude);
      gaiaToSpatiaLiteBlobWkb(geom, &geoblob, &geosize);
      gaiaFreeGeomColl(geom);
      sqlite3_bind_blob(stmt, 7, geoblob, geosize, SQLITE_TRANSIENT);
      free(geoblob);
  } else
    sqlite3_bind_null(stmt, 7);
  dblval = GetGpsDirection(tag_list, &ok);
  if (ok == true)
    sqlite3_bind_double(stmt, 8, dblval);
  else
    sqlite3_bind_null(stmt, 8);
  GetGpsSatellites(tag_list, satellites, &ok);
  if (ok == true)
    sqlite3_bind_text(stmt, 9, satellites.ToUTF8(), satellites.Len(),
                      SQLITE_TRANSIENT);
  else
    sqlite3_bind_null(stmt, 9);
  GetGpsTimestamp(tag_list, timestamp, &ok);
  if (ok == true)
    sqlite3_bind_text(stmt, 10, timestamp.ToUTF8(), timestamp.Len(),
                      SQLITE_TRANSIENT);
  else
    sqlite3_bind_text(stmt, 10, "0000-00-00 00:00:00", 19, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 11, path.ToUTF8(), path.Len(), SQLITE_TRANSIENT);
  ret = sqlite3_step(stmt);
  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
    ;
  else
    {
      wxMessageBox(wxT("sqlite3_step() error: ") +
                   wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_finalize(stmt);
      goto abort;
    }
  sqlite3_finalize(stmt);
  pk = sqlite3_last_insert_rowid(SqliteHandle);
  if (metadata)
    {
      // feeding the ExifTags table; preparing the SQL statement 
      strcpy(sql,
             "INSERT OR IGNORE INTO ExifTags (PhotoId, TagId, TagName, GpsTag, ValueType, ");
      strcat(sql, "TypeName, CountValues) VALUES (?, ?, ?, ?, ?, ?, ?)");
      ret = sqlite3_prepare_v2(SqliteHandle, sql, strlen(sql), &stmt, NULL);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("INSERT INTO ExifTags error: ") +
                       wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto abort;
        }
      for (i = 0; i < gaiaGetExifTagsCount(tag_list); i++)
        {
          pT = gaiaGetExifTagByPos(tag_list, i);
          if (pT)
            {
              gaiaExifTagGetName(pT, tag_name, 128);
              switch (gaiaExifTagGetValueType(pT))
                {
                  case 1:
                    type_desc = "BYTE";
                    break;
                  case 2:
                    type_desc = "STRING";
                    break;
                  case 3:
                    type_desc = "SHORT";
                    break;
                  case 4:
                    type_desc = "LONG";
                    break;
                  case 5:
                    type_desc = "RATIONAL";
                    break;
                  case 6:
                    type_desc = "SBYTE";
                    break;
                  case 7:
                    type_desc = "UNDEFINED";
                    break;
                  case 8:
                    type_desc = "SSHORT";
                    break;
                  case 9:
                    type_desc = "SLONG";
                    break;
                  case 10:
                    type_desc = "SRATIONAL";
                    break;
                  case 11:
                    type_desc = "FLOAT";
                    break;
                  case 12:
                    type_desc = "DOUBLE";
                    break;
                  default:
                    type_desc = "UNKNOWN";
                    break;
                };
              // INSERTing an Exif Tag 
              sqlite3_reset(stmt);
              sqlite3_clear_bindings(stmt);
              sqlite3_bind_int64(stmt, 1, pk);
              sqlite3_bind_int(stmt, 2, gaiaExifTagGetId(pT));
              sqlite3_bind_text(stmt, 3, tag_name, strlen(tag_name),
                                SQLITE_STATIC);
              sqlite3_bind_int(stmt, 4, gaiaIsExifGpsTag(pT));
              sqlite3_bind_int(stmt, 5, gaiaExifTagGetValueType(pT));
              sqlite3_bind_text(stmt, 6, type_desc, strlen(type_desc),
                                SQLITE_STATIC);
              sqlite3_bind_int(stmt, 7, gaiaExifTagGetNumValues(pT));
              ret = sqlite3_step(stmt);
              if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                ;
              else
                {
                  wxMessageBox(wxT("sqlite3_step() error: ") +
                               wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_finalize(stmt);
                  goto abort;
                }
            }
        }
      sqlite3_finalize(stmt);
      // feeding the ExifValues table; preparing the SQL statement
      strcpy(sql,
             "INSERT OR IGNORE INTO ExifValues (PhotoId, TagId, ValueIndex, ByteValue, ");
      strcat(sql,
             "StringValue, NumValue, NumValueBis, DoubleValue, HumanReadable) VALUES ");
      strcat(sql, "(?, ?, ?, ?, ?, ?, ?, ?, ?)");
      ret = sqlite3_prepare_v2(SqliteHandle, sql, strlen(sql), &stmt, NULL);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("NSERT INTO ExifValues error: ") +
                       wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto abort;
        }
      for (i = 0; i < gaiaGetExifTagsCount(tag_list); i++)
        {
          pT = gaiaGetExifTagByPos(tag_list, i);
          if (pT)
            {
              gaiaExifTagGetHumanReadable(pT, human, 1024, &ok_human);
              for (iv = 0; iv < gaiaExifTagGetNumValues(pT); iv++)
                {
                  // INSERTing an Exif Tag
                  sqlite3_reset(stmt);
                  sqlite3_clear_bindings(stmt);
                  sqlite3_bind_int64(stmt, 1, pk);
                  sqlite3_bind_int(stmt, 2, gaiaExifTagGetId(pT));
                  sqlite3_bind_int(stmt, 3, iv);
                  if (gaiaExifTagGetValueType(pT) == 1
                      || gaiaExifTagGetValueType(pT) == 6
                      || gaiaExifTagGetValueType(pT) == 7)
                    {
                      sqlite3_bind_blob(stmt, 4, pT->ByteValue, pT->Count,
                                        SQLITE_STATIC);
                      sqlite3_bind_null(stmt, 5);
                      sqlite3_bind_null(stmt, 6);
                      sqlite3_bind_null(stmt, 7);
                      sqlite3_bind_null(stmt, 8);
                    }
                  if (gaiaExifTagGetValueType(pT) == 2)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_text(stmt, 5, pT->StringValue,
                                        strlen(pT->StringValue), SQLITE_STATIC);
                      sqlite3_bind_null(stmt, 6);
                      sqlite3_bind_null(stmt, 7);
                      sqlite3_bind_null(stmt, 8);
                    }
                  if (gaiaExifTagGetValueType(pT) == 3)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      val64 = gaiaExifTagGetShortValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 6);
                      else
                        sqlite3_bind_int64(stmt, 6, val64);
                      sqlite3_bind_null(stmt, 7);
                      sqlite3_bind_null(stmt, 8);
                    }
                  if (gaiaExifTagGetValueType(pT) == 4)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      val64 = gaiaExifTagGetLongValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 6);
                      else
                        sqlite3_bind_int64(stmt, 6, val64);
                      sqlite3_bind_null(stmt, 7);
                      sqlite3_bind_null(stmt, 8);
                    }
                  if (gaiaExifTagGetValueType(pT) == 5)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      val64 = gaiaExifTagGetRational1Value(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 6);
                      else
                        sqlite3_bind_int64(stmt, 6, val64);
                      val64 = gaiaExifTagGetRational2Value(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 7);
                      else
                        sqlite3_bind_int64(stmt, 7, val64);
                      dblval = gaiaExifTagGetRationalValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 8);
                      else
                        sqlite3_bind_double(stmt, 8, dblval);
                    }
                  if (gaiaExifTagGetValueType(pT) == 9)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      val64 = gaiaExifTagGetSignedLongValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 6);
                      else
                        sqlite3_bind_int64(stmt, 6, val64);
                      sqlite3_bind_null(stmt, 7);
                      sqlite3_bind_null(stmt, 8);
                    }
                  if (gaiaExifTagGetValueType(pT) == 10)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      val64 = gaiaExifTagGetSignedRational1Value(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 6);
                      else
                        sqlite3_bind_int64(stmt, 6, val64);
                      val64 = gaiaExifTagGetSignedRational2Value(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 7);
                      else
                        sqlite3_bind_int64(stmt, 7, val64);
                      dblval = gaiaExifTagGetSignedRationalValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 8);
                      else
                        sqlite3_bind_double(stmt, 8, dblval);
                    }
                  if (gaiaExifTagGetValueType(pT) == 11)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      sqlite3_bind_null(stmt, 6);
                      sqlite3_bind_null(stmt, 7);
                      dblval = gaiaExifTagGetFloatValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 8);
                      else
                        sqlite3_bind_double(stmt, 8, dblval);
                    }
                  if (gaiaExifTagGetValueType(pT) == 12)
                    {
                      sqlite3_bind_null(stmt, 4);
                      sqlite3_bind_null(stmt, 5);
                      sqlite3_bind_null(stmt, 6);
                      sqlite3_bind_null(stmt, 7);
                      dblval = gaiaExifTagGetDoubleValue(pT, iv, &xok);
                      if (!ok)
                        sqlite3_bind_null(stmt, 8);
                      else
                        sqlite3_bind_double(stmt, 8, dblval);
                    }
                  if (!ok_human)
                    sqlite3_bind_null(stmt, 9);
                  else
                    sqlite3_bind_text(stmt, 9, human, strlen(human),
                                      SQLITE_STATIC);
                  ret = sqlite3_step(stmt);
                  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                    ;
                  else
                    {
                      wxMessageBox(wxT("sqlite3_step() error: ") +
                                   wxString::FromUTF8(sqlite3_errmsg
                                                      (SqliteHandle)),
                                   wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                                   this);
                      sqlite3_finalize(stmt);
                      goto abort;
                    }
                  if (gaiaExifTagGetValueType(pT) == 1
                      || gaiaExifTagGetValueType(pT) == 2
                      || gaiaExifTagGetValueType(pT) == 6
                      || gaiaExifTagGetValueType(pT) == 7)
                    break;
                  ok_human = 0;
                }
            }
        }
      sqlite3_finalize(stmt);
    }
// commits the transaction 
  strcpy(sql, "COMMIT");
  ret = sqlite3_exec(SqliteHandle, sql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("COMMIT error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
  return true;
abort:
// rolling back the transaction
  strcpy(sql, "ROLLBACK");
  ret = sqlite3_exec(SqliteHandle, sql, NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("ROLLBACK error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
  return false;
}

sqlite3_int64 MyFrame::GetPixelX(gaiaExifTagListPtr tag_list, bool * ok)
{
//
// trying to retrieve the ExifImageWidth 
//
  *ok = false;
  if (!tag_list)
    return 0;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->TagId == 0xA002)
        {
          // ok, this one is the ExifImageWidth tag 
          if (tag->Type == 3 && tag->Count == 1)
            {
              *ok = true;
              return *(tag->ShortValues + 0);
          } else if (tag->Type == 4 && tag->Count == 1)
            {
              *ok = true;
              return *(tag->LongValues + 0);
            }
        }
      tag = tag->Next;
    }
  return false;
}

sqlite3_int64 MyFrame::GetPixelY(gaiaExifTagListPtr tag_list, bool * ok)
{
//
// trying to retrieve the ExifImageLength 
//
  *ok = false;
  if (!tag_list)
    return 0;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->TagId == 0xA003)
        {
          // ok, this one is the ExifImageLength tag
          if (tag->Type == 3 && tag->Count == 1)
            {
              *ok = true;
              return *(tag->ShortValues + 0);
          } else if (tag->Type == 4 && tag->Count == 1)
            {
              *ok = true;
              return *(tag->LongValues + 0);
            }
        }
      tag = tag->Next;
    }
  return false;
}

void MyFrame::GetMake(gaiaExifTagListPtr tag_list, wxString & str, bool * ok)
{
//
// trying to retrieve the Make 
//
  *ok = false;
  if (!tag_list)
    return;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->TagId == 0x010F)
        {
          // ok, this one is the Make tag 
          if (tag->Type == 2)
            {
              *ok = true;
              str = wxString::FromUTF8(tag->StringValue);
              return;
            }
        }
      tag = tag->Next;
    }
  return;
}

void MyFrame::GetModel(gaiaExifTagListPtr tag_list, wxString & str, bool * ok)
{
//
// trying to retrieve the Model 
//
  *ok = false;
  if (!tag_list)
    return;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->TagId == 0x0110)
        {
          // ok, this one is the Model tag
          if (tag->Type == 2)
            {
              *ok = true;
              str = wxString::FromUTF8(tag->StringValue);
              return;
            }
        }
      tag = tag->Next;
    }
  return;
}

void MyFrame::GetDate(gaiaExifTagListPtr tag_list, wxString & str, bool * ok)
{
//
// trying to retrieve the Date 
//
  *ok = false;
  if (!tag_list)
    return;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->TagId == 0x9003)
        {
          // ok, this one is the DateTimeOriginal tag 
          if (tag->Type == 2)
            {
              *ok = true;
              str = wxString::FromUTF8(tag->StringValue);
              if (str.Len() >= 19)
                {
                  str.SetChar(4, '-');
                  str.SetChar(7, '-');
                }
              return;
            }
        }
      tag = tag->Next;
    }
  return;
}

void MyFrame::GetGpsCoords(gaiaExifTagListPtr tag_list, double *longitude,
                           double *latitude, bool * ok)
{
//
// trying to retrieve the GPS coordinates 
//
  char lat_ref = '\0';
  char long_ref = '\0';
  double lat_degs = DBL_MIN;
  double lat_mins = DBL_MIN;
  double lat_secs = DBL_MIN;
  double long_degs = DBL_MIN;
  double long_mins = DBL_MIN;
  double long_secs = DBL_MIN;
  double dblval;
  double sign;
  double dblLatitude = DBL_MIN;
  double dblLongitude = DBL_MIN;
  int xok;
  wxString str;
  *ok = false;
  if (!tag_list)
    return;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->Gps && tag->TagId == 0x01)
        {
          // ok, this one is the GPSLatitudeRef tag
          if (tag->Type == 2)
            lat_ref = *(tag->StringValue);
        }
      if (tag->Gps && tag->TagId == 0x03)
        {
          // ok, this one is the GPSLongitudeRef tag
          if (tag->Type == 2)
            long_ref = *(tag->StringValue);
        }
      if (tag->Gps && tag->TagId == 0x02)
        {
          // ok, this one is the GPSLatitude tag 
          if (tag->Type == 5 && tag->Count == 3)
            {
              dblval = gaiaExifTagGetRationalValue(tag, 0, &xok);
              if (xok)
                lat_degs = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 1, &xok);
              if (xok)
                lat_mins = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 2, &xok);
              if (xok)
                lat_secs = dblval;
            }
          if (tag->Type == 2)
            {
              str = wxString::FromUTF8(tag->StringValue);
              if (str.ToDouble(&dblval) == true)
                dblLatitude = dblval;
            }
        }
      if (tag->Gps && tag->TagId == 0x04)
        {
          // ok, this one is the GPSLongitude tag
          if (tag->Type == 5 && tag->Count == 3)
            {
              dblval = gaiaExifTagGetRationalValue(tag, 0, &xok);
              if (xok)
                long_degs = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 1, &xok);
              if (xok)
                long_mins = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 2, &xok);
              if (xok)
                long_secs = dblval;
            }
          if (tag->Type == 2)
            {
              str = wxString::FromUTF8(tag->StringValue);
              if (str.ToDouble(&dblval) == true)
                dblLongitude = dblval;
            }
        }
      tag = tag->Next;
    }
  if ((lat_ref == 'N' || lat_ref == 'S' || long_ref == 'E' || long_ref == 'W')
      && lat_degs != DBL_MIN && lat_mins != DBL_MIN && lat_secs != DBL_MIN
      && long_degs != DBL_MIN && long_mins != DBL_MIN && long_secs != DBL_MIN)
    {
      *ok = true;
      if (lat_ref == 'S')
        sign = -1.0;
      else
        sign = 1.0;
      lat_degs = math_round(lat_degs * 1000000.0);
      lat_mins = math_round(lat_mins * 1000000.0);
      lat_secs = math_round(lat_secs * 1000000.0);
      dblval =
        math_round(lat_degs + (lat_mins / 60.0) +
                   (lat_secs / 3600.0)) * (sign / 1000000.0);
      *latitude = dblval;
      if (long_ref == 'W')
        sign = -1.0;
      else
        sign = 1.0;
      long_degs = math_round(long_degs * 1000000.0);
      long_mins = math_round(long_mins * 1000000.0);
      long_secs = math_round(long_secs * 1000000.0);
      dblval =
        math_round(long_degs + (long_mins / 60.0) +
                   (long_secs / 3600.0)) * (sign / 1000000.0);
      *longitude = dblval;
  } else if (dblLatitude != DBL_MIN && dblLongitude != DBL_MIN)
    {
      *ok = true;
      *latitude = dblLatitude;
      *longitude = dblLongitude;
    }
  return;
}

void MyFrame::GetGpsSatellites(gaiaExifTagListPtr tag_list, wxString & str,
                               bool * ok)
{
//
// trying to retrieve the GPSSatellites 
//
  *ok = false;
  if (!tag_list)
    return;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->Gps && tag->TagId == 0x08)
        {
          // ok, this one is the GPSSatellites tag
          if (tag->Type == 2)
            {
              *ok = true;
              str = wxString::FromUTF8(tag->StringValue);
              return;
            }
        }
      tag = tag->Next;
    }
  return;
}

double MyFrame::GetGpsDirection(gaiaExifTagListPtr tag_list, bool * ok)
{
//
// trying to retrieve the GPS direction 
//
  char dir_ref = '\0';
  double direction = DBL_MIN;
  double dblval;
  int xok;
  *ok = false;
  if (!tag_list)
    return direction;
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->Gps && tag->TagId == 0x10)
        {
          // ok, this one is the GPSDirectionRef tag 
          if (tag->Type == 2)
            dir_ref = *(tag->StringValue);
        }
      if (tag->Gps && tag->TagId == 0x11)
        {
          // ok, this one is the GPSDirection tag
          if (tag->Type == 5 && tag->Count == 1)
            {
              dblval = gaiaExifTagGetRationalValue(tag, 0, &xok);
              if (xok)
                direction = dblval;
            }
        }
      tag = tag->Next;
    }
  if ((dir_ref == 'T' || dir_ref == 'M') && direction != DBL_MIN)
    *ok = true;
  return direction;
}

void MyFrame::GetGpsTimestamp(gaiaExifTagListPtr tag_list, wxString & str,
                              bool * ok)
{
//
// trying to retrieve the GPS Timestamp 
//
  char date[16];
  char timestamp[32];
  double hours = DBL_MIN;
  double mins = DBL_MIN;
  double secs = DBL_MIN;
  double dblval;
  int xok;
  int hh;
  int mm;
  int ss;
  int millis;
  *ok = false;
  if (!tag_list)
    return;
  strcpy(date, "0000-00-00");
  gaiaExifTagPtr tag = tag_list->First;
  while (tag)
    {
      if (tag->Gps && tag->TagId == 0x1D)
        {
          // ok, this one is the GPSDateStamp tag 
          if (tag->Type == 2)
            {
              strcpy(date, tag->StringValue);
              date[4] = '-';
              date[7] = '-';
            }
        }
      if (tag->Gps && tag->TagId == 0x07)
        {
          // ok, this one is the GPSTimeStamp tag 
          if (tag->Type == 5 && tag->Count == 3)
            {
              dblval = gaiaExifTagGetRationalValue(tag, 0, &xok);
              if (xok)
                hours = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 1, &xok);
              if (xok)
                mins = dblval;
              dblval = gaiaExifTagGetRationalValue(tag, 2, &xok);
              if (xok)
                secs = dblval;
            }
        }
      tag = tag->Next;
    }
  if (hours != DBL_MIN && mins != DBL_MIN && secs != DBL_MIN)
    {
      *ok = true;
      hh = (int) floor(hours);
      mm = (int) floor(mins);
      ss = (int) floor(secs);
      millis = (int) ((secs - ss) * 1000);
      sprintf(timestamp, "%s %02d:%02d:%02d.%03d", date, hh, mm, ss, millis);
      str = wxString::FromUTF8(timestamp);
    }
  return;
}

void MyFrame::ImportXmlDocuments(wxString & path, bool folder,
                                 wxString & suffix, wxString & table,
                                 wxString & pkName, wxString & xmlColumn,
                                 wxString & inPathColumn,
                                 wxString & schemaUriColumn,
                                 wxString & parseErrColumn,
                                 wxString & validateErrColumn, int compressed,
                                 const char *schemaURI,
                                 bool isInternalSchemaUri)
{
//
// trying to import XML Documents
// 
  int cnt;
  char msg[256];
  wxString sql;
  sqlite3_stmt *stmt;
  char *errMsg;
  char xsql[8192];
  char *qname;
  int ret;
  int failed;
  if (CheckOrCreateXmlTable
      (table, pkName, xmlColumn, inPathColumn, schemaUriColumn, parseErrColumn,
       validateErrColumn) == false)
    return;
// preparing the SQL INSERT statement
  sql = wxT("INSERT OR IGNORE INTO \"");
  strcpy(xsql, table.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\" (\"");
  strcpy(xsql, pkName.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\", \"");
  strcpy(xsql, xmlColumn.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  if (inPathColumn.Len() > 0)
    {
      // appending the InPath column
      sql += wxT("\", \"");
      strcpy(xsql, inPathColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
    }
  if (schemaUriColumn.Len() > 0)
    {
      // appending the SchemaURI column
      sql += wxT("\", \"");
      strcpy(xsql, schemaUriColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
    }
  if (parseErrColumn.Len() > 0)
    {
      // appending the ParseError column
      sql += wxT("\", \"");
      strcpy(xsql, parseErrColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
    }
  if (validateErrColumn.Len() > 0)
    {
      // appending the ValidateError column
      sql += wxT("\", \"");
      strcpy(xsql, validateErrColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
    }
  sql += wxT("\") VALUES (NULL, ?");
  if (inPathColumn.Len() > 0)
    sql += wxT(", ?");
  if (schemaUriColumn.Len() > 0)
    sql += wxT(", ?");
  if (parseErrColumn.Len() > 0)
    sql += wxT(", ?");
  if (validateErrColumn.Len() > 0)
    sql += wxT(", ?");
  sql += wxT(")");
  ret = sqlite3_prepare_v2(SqliteHandle, sql.ToUTF8(), sql.Len(), &stmt, NULL);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("INSERT INTO XML target table error: ") +
                   wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }
// stating a transaction 
  ret = sqlite3_exec(SqliteHandle, "BEGIN", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("BEGIN error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }

  if (folder == true)
    cnt =
      XmlDocumentLoadDir(path, suffix, compressed, schemaURI,
                         isInternalSchemaUri, inPathColumn, parseErrColumn,
                         validateErrColumn, schemaUriColumn, stmt, &failed);
  else
    cnt =
      XmlDocumentLoadFile(path, compressed, schemaURI, isInternalSchemaUri,
                          inPathColumn, schemaUriColumn, parseErrColumn,
                          validateErrColumn, stmt, &failed);

// commits the transaction 
  ret = sqlite3_exec(SqliteHandle, "COMMIT", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("COMMIT error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
    }
  sqlite3_finalize(stmt);
  sprintf(msg, "%d XML Document%s processed\n\n", cnt,
          (cnt > 1) ? "s were" : " was");
  wxString message = wxString::FromUTF8(msg);
  sprintf(msg, "Valid XMLDocuments: %d\n", cnt - failed);
  message += wxString::FromUTF8(msg);
  sprintf(msg, "Failures: %d\n", failed);
  message += wxString::FromUTF8(msg);
  wxMessageBox(message, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
}

bool MyFrame::InsertIntoXmlTable(sqlite3_stmt * stmt, char *blob,
                                 int sz, wxString & inPathColumn,
                                 wxString & path, wxString & schemaUriColumn,
                                 const char *schemaUri,
                                 wxString & parseErrColumn,
                                 const char *parseError,
                                 wxString & validateErrColumn,
                                 const char *validateError)
{
//
// attemping to insert into the XML target table
//
  int ret;
  int i_col = 2;
  sqlite3_reset(stmt);
  sqlite3_clear_bindings(stmt);
  sqlite3_bind_blob(stmt, 1, blob, sz, free);
  if (inPathColumn.Len() > 0)
    {
      // binding the InPath value
      char x_path[1024];
      wxFileName fn = wxFileName(path);
      wxString name = fn.GetName();
      if (fn.GetExt().Len() > 0)
        name += wxT(".") + fn.GetExt();
      strcpy(x_path, name.ToUTF8());
      sqlite3_bind_text(stmt, i_col++, x_path, strlen(x_path),
                        SQLITE_TRANSIENT);
    }
  if (schemaUriColumn.Len() > 0)
    {
      // binding the SchemaURI value
      if (schemaUri == NULL)
        sqlite3_bind_null(stmt, i_col++);
      else
        sqlite3_bind_text(stmt, i_col++, schemaUri, strlen(schemaUri),
                          SQLITE_STATIC);
    }
  if (parseErrColumn.Len() > 0)
    {
      // binding the ParseError value
      if (parseError == NULL)
        sqlite3_bind_null(stmt, i_col++);
      else
        sqlite3_bind_text(stmt, i_col++, parseError, strlen(parseError),
                          SQLITE_STATIC);
    }
  if (validateErrColumn.Len() > 0)
    {
      // binding the ValidateError value
      if (validateError == NULL)
        sqlite3_bind_null(stmt, i_col++);
      else
        sqlite3_bind_text(stmt, i_col++, validateError, strlen(validateError),
                          SQLITE_STATIC);
    }
  ret = sqlite3_step(stmt);
  if (ret == SQLITE_DONE || ret == SQLITE_ROW)
    return true;
  wxMessageBox(wxT("XML-INSERT: sqlite3_step() error: ") +
               wxString::FromUTF8(sqlite3_errmsg(SqliteHandle)),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  return false;
}

bool MyFrame::CheckOrCreateXmlTable(wxString & table, wxString & pkName,
                                    wxString & xmlColumn,
                                    wxString & inPathColumn,
                                    wxString & schemaUriColumn,
                                    wxString & parseErrColumn,
                                    wxString & validateErrColumn)
{
//
// creates the XML DB table / or checks an existing one for validity 
//
  int ret;
  wxString sql;
  char *errMsg;
  char xsql[8192];
  char *qname;
  bool ok_xmlblob = false;
  bool ok_inPath = false;
  bool ok_schemaURI = false;
  bool ok_parseErr = false;
  bool ok_validateErr = false;
  const char *name;
  int i;
  char **results;
  int rows;
  int columns;
  sql = wxT("CREATE TABLE IF NOT EXISTS \"");
  strcpy(xsql, table.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\" (\n\"");
  strcpy(xsql, pkName.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\" INTEGER PRIMARY KEY AUTOINCREMENT,\n\"");
  strcpy(xsql, xmlColumn.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\" BLOB NOT NULL");
  if (inPathColumn.Len() > 0)
    {
      // adding the InPath Column
      sql += wxT("\n,\"");
      strcpy(xsql, inPathColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
      sql += wxT("\" TEXT NOT NULL");
    }
  if (schemaUriColumn.Len() > 0)
    {
      // adding the SchemaURI Column
      sql += wxT("\n,\"");
      strcpy(xsql, schemaUriColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
      sql += wxT("\" TEXT");
    }
  if (parseErrColumn.Len() > 0)
    {
      // adding the ParseError Column
      sql += wxT("\n,\"");
      strcpy(xsql, parseErrColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
      sql += wxT("\" TEXT");
    }
  if (validateErrColumn.Len() > 0)
    {
      // adding the validateError Column
      sql += wxT("\n,\"");
      strcpy(xsql, validateErrColumn.ToUTF8());
      qname = gaiaDoubleQuotedSql(xsql);
      sql += wxString::FromUTF8(qname);
      free(qname);
      sql += wxT("\" TEXT");
    }
  sql += wxT(")");
  ret = sqlite3_exec(SqliteHandle, sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("CREATE XML Target Table error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }

  sql = wxT("PRAGMA table_info(\"");
  strcpy(xsql, table.ToUTF8());
  qname = gaiaDoubleQuotedSql(xsql);
  sql += wxString::FromUTF8(qname);
  free(qname);
  sql += wxT("\")");
  ret =
    sqlite3_get_table(SqliteHandle, sql.ToUTF8(), &results, &rows, &columns,
                      &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("PRAGMA table_info(xmlTargetTable) error: ") +
                   wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  if (rows < 1)
    ;
  else
    {
      strcpy(xsql, xmlColumn.ToUTF8());
      for (i = 1; i <= rows; i++)
        {
          name = results[(i * columns) + 1];
          if (strcasecmp(name, xsql) == 0)
            ok_xmlblob = true;
        }
      if (schemaUriColumn.Len() == 0)
        ok_schemaURI = true;
      else
        {
          strcpy(xsql, schemaUriColumn.ToUTF8());
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              if (strcasecmp(name, xsql) == 0)
                ok_schemaURI = true;
            }
        }
      if (inPathColumn.Len() == 0)
        ok_inPath = true;
      else
        {
          strcpy(xsql, inPathColumn.ToUTF8());
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              if (strcasecmp(name, xsql) == 0)
                ok_inPath = true;
            }
        }
      if (parseErrColumn.Len() == 0)
        ok_parseErr = true;
      else
        {
          strcpy(xsql, parseErrColumn.ToUTF8());
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              if (strcasecmp(name, xsql) == 0)
                ok_parseErr = true;
            }
        }
      if (validateErrColumn.Len() == 0)
        ok_validateErr = true;
      else
        {
          strcpy(xsql, validateErrColumn.ToUTF8());
          for (i = 1; i <= rows; i++)
            {
              name = results[(i * columns) + 1];
              if (strcasecmp(name, xsql) == 0)
                ok_validateErr = true;
            }
        }
    }
  sqlite3_free_table(results);
  if (ok_xmlblob == true && ok_inPath == true && ok_schemaURI == true
      && ok_parseErr == true && ok_validateErr == true)
    ;
  else
    {
      wxMessageBox(wxT
                   ("ERROR: XML target table already exists, but has incompatible columns"),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      goto abort;
    }
  return true;
abort:
  return false;
}

int MyFrame::XmlDocumentLoadDir(wxString & path, wxString & suffix,
                                int compressed, const char *schemaURI,
                                bool isInternalSchemaUri,
                                wxString & inPathColumn,
                                wxString & schemaUriColumn,
                                wxString & parseErrColumn,
                                wxString & validateErrColumn,
                                sqlite3_stmt * stmt, int *failed)
{
//
// importing XML Document files from a whole DIRECTORY 
//
  int fails;
  *failed = 0;
#if defined(_WIN32) && !defined(__MINGW32__)
/* Visual Studio .NET */
  struct _finddata_t c_file;
  intptr_t hFile;
  int cnt = 0;
  wxString filePath;
  if (_chdir(path.ToUTF8()) < 0)
    return 0;
  if ((hFile = _findfirst("*.*", &c_file)) == -1L)
    ;
  else
    {
      while (1)
        {
          if ((c_file.attrib & _A_RDONLY) == _A_RDONLY
              || (c_file.attrib & _A_NORMAL) == _A_NORMAL)
            {
              if (IsValidSuffix(c_file.name, suffix))
                {
                  filePath = path;
                  filePath += wxT("/") + wxString::FromUTF8(c_file.name);
                  cnt +=
                    XmlDocumentLoadFile(filePath, compressed, schemaURI,
                                        isInternalSchemaUri, inPathColumn,
                                        schemaUriColumn, parseErrColumn,
                                        validateErrColumn, stmt, &fails);
                  *failed += fails;
                }
            }
          if (_findnext(hFile, &c_file) != 0)
            break;
        };
      _findclose(hFile);
    }
  return cnt;
#else
/* not Visual Studio .NET */
  int cnt = 0;
  wxString filePath;
  struct dirent *entry;
  DIR *dir = opendir(path.ToUTF8());
  if (!dir)
    return 0;
  while (1)
    {
      // scanning dir-entries
      entry = readdir(dir);
      if (!entry)
        break;
      if (IsValidSuffix(entry->d_name, suffix))
        {
          filePath = path;
          filePath += wxT("/") + wxString::FromUTF8(entry->d_name);
          cnt +=
            XmlDocumentLoadFile(filePath, compressed, schemaURI,
                                isInternalSchemaUri, inPathColumn,
                                schemaUriColumn, parseErrColumn,
                                validateErrColumn, stmt, &fails);
          *failed += fails;
        }
    }
  closedir(dir);
  return cnt;
#endif
}

bool MyFrame::IsValidSuffix(const char *fileName, wxString & suffix)
{
// testing if a FileName ends with the expected suffix
  if (suffix.Len() == 0)
    return true;

  char suf[1024];
  strcpy(suf, suffix.ToUTF8());
  int len1 = strlen(fileName);
  int len2 = strlen(suf);
  int off = len1 - len2;
  if (off >= 1)
    {
      if (strcasecmp(fileName + off, suf) == 0)
        return true;
    }
  return false;
}

int MyFrame::XmlDocumentLoadFile(wxString & path, int compressed,
                                 const char *schemaURI,
                                 bool isInternalSchemaUri,
                                 wxString & inPathColumn,
                                 wxString & schemaUriColumn,
                                 wxString & parseErrColumn,
                                 wxString & validateErrColumn,
                                 sqlite3_stmt * stmt, int *failed)
{
//
// importing a single XML Document file 
//
#ifdef ENABLE_LIBXML2           /* only if LIBXML2 is enabled */
  FILE *fl;
  int sz = 0;
  int rd;
  int loaded = 0;
  unsigned char *blob = NULL;
  char *xml = NULL;
  int xml_size;
  char *parseError = NULL;
  char *validateError = NULL;
  char *p_schemaURI = NULL;
  int len;
  *failed = 0;

  fl = fopen(path.ToUTF8(), "rb");
  if (!fl)
    return 0;
  if (fseek(fl, 0, SEEK_END) == 0)
    sz = ftell(fl);
  blob = (unsigned char *) malloc(sz);
  rewind(fl);
  rd = fread(blob, 1, sz, fl);
  if (rd == sz)
    {
      // attempting to parse (and possibly validate) the XMLDocument
      if (isInternalSchemaUri)
        {
          // attempting to retrieve an internally defined SchemaURI
          char *internalSchemaURI =
            gaiaXmlGetInternalSchemaURI(InternalCache, blob, rd);
          if (internalSchemaURI == NULL)
            {
              // unable to identify the SchemaURI
              xml = NULL;
          } else
            {
              // ok, attempting to validate using the internal SchemaURI
              len = strlen(internalSchemaURI);
              p_schemaURI = (char *) malloc(len + 1);
              strcpy(p_schemaURI, internalSchemaURI);
              gaiaXmlToBlob(InternalCache, blob, rd, compressed,
                            internalSchemaURI, (unsigned char **) (&xml),
                            &xml_size, &parseError, &validateError);
              free(internalSchemaURI);
            }
      } else
        {
          // possibly validating against the externally defined SchemaURI
          if (schemaURI)
            {
              len = strlen(schemaURI);
              p_schemaURI = (char *) malloc(len + 1);
              strcpy(p_schemaURI, schemaURI);
            }
          gaiaXmlToBlob(InternalCache, blob, rd, compressed, schemaURI,
                        (unsigned char **) (&xml), &xml_size, &parseError,
                        &validateError);
        }
      if (xml == NULL)
        {
          /* creating an empty BLOB as a placeholder */
          xml_size = 1;
          xml = (char *) malloc(xml_size);
          *xml = '\0';
          *failed = 1;
        }
      if (InsertIntoXmlTable
          (stmt, xml, xml_size, inPathColumn, path, schemaUriColumn,
           p_schemaURI, parseErrColumn, parseError, validateErrColumn,
           validateError) == false)
        goto stop;
      loaded = 1;
    }
stop:
  if (p_schemaURI)
    free(p_schemaURI);
  if (blob)
    free(blob);
  fclose(fl);
  return loaded;

#else

  wxMessageBox(wxT
               ("Sorry, spatialite_gui was built disabling LIBXML2\n\nUnsupported operation"),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);

#endif /* end LIBXML2 conditionals */
}

void MyFrame::ImportDXFfiles(wxString & path, bool folder, wxString & prefix,
                             wxString & layer, int srid, bool force2d,
                             bool force3d, bool mixed, bool linked,
                             bool unlinked, bool append)
{
//
// trying to import DXF drawing files
// 
  int cnt;
  int failed;
  char msg[256];
  if (folder == true)
    cnt =
      DxfLoadDir(path, prefix, layer, srid, force2d, force3d, mixed, linked,
                 unlinked, append, &failed);
  else
    cnt =
      DxfLoadFile(path, prefix, layer, srid, force2d, force3d, mixed, linked,
                  unlinked, append, &failed);
  wxString message = wxString::FromUTF8(msg);
  sprintf(msg, "Imported DXF files: %d\n", cnt - failed);
  message += wxString::FromUTF8(msg);
  sprintf(msg, "Failures: %d\n", failed);
  message += wxString::FromUTF8(msg);
  wxMessageBox(message, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
}

int MyFrame::DxfLoadDir(wxString & path, wxString & prefix, wxString & layer,
                        int srid, bool force2d, bool force3d, bool mixed,
                        bool linked, bool unlinked, bool append, int *failed)
{
//
// importing DXF files from a whole DIRECTORY 
//
  wxString suffix = wxT(".dxf");
  int fails;
  *failed = 0;
#if defined(_WIN32) && !defined(__MINGW32__)
/* Visual Studio .NET */
  struct _finddata_t c_file;
  intptr_t hFile;
  int cnt = 0;
  wxString filePath;
  if (_chdir(path.ToUTF8()) < 0)
    return 0;
  if ((hFile = _findfirst("*.*", &c_file)) == -1L)
    ;
  else
    {
      while (1)
        {
          if ((c_file.attrib & _A_RDONLY) == _A_RDONLY
              || (c_file.attrib & _A_NORMAL) == _A_NORMAL)
            {
              if (IsValidSuffix(entry->d_name, suffix))
                {
                  filePath = path;
                  filePath += wxT("/") + wxString::FromUTF8(c_file.name);
                  cnt +=
                    DxfLoadFile(filePath, prefix, layer, srid, force2d, force3d,
                                mixed, linked, unlinked, append, &fails);
                  *failed += fails;
                }
            }
          if (_findnext(hFile, &c_file) != 0)
            break;
        };
      _findclose(hFile);
    }
  return cnt;
#else
/* not Visual Studio .NET */
  int cnt = 0;
  wxString filePath;
  struct dirent *entry;
  DIR *dir = opendir(path.ToUTF8());
  if (!dir)
    return 0;
  while (1)
    {
      // scanning dir-entries
      entry = readdir(dir);
      if (!entry)
        break;
      if (IsValidSuffix(entry->d_name, suffix))
        {
          filePath = path;
          filePath += wxT("/") + wxString::FromUTF8(entry->d_name);
          cnt +=
            DxfLoadFile(filePath, prefix, layer, srid, force2d, force3d, mixed,
                        linked, unlinked, append, &fails);
          *failed += fails;
        }
    }
  closedir(dir);
  return cnt;
#endif
}

int MyFrame::DxfLoadFile(wxString & path, wxString & prefix, wxString & layer,
                         int srid, bool force2d, bool force3d, bool mixed,
                         bool linked, bool unlinked, bool append, int *failed)
{
//
// importing a single DXF file 
//
  int force_dims = GAIA_DXF_AUTO_2D_3D;
  int special_rings = GAIA_DXF_RING_NONE;
  char dxf_path[1024];
  char xprefix[1024];
  const char *xxprefix = NULL;
  char selected_layer[1024];
  const char *xselected_layer = NULL;
  gaiaDxfParserPtr dxf;
  bool error = false;

  *failed = 0;
// creating a DXF parser
  if (prefix.Len() > 0)
    {
      strcpy(xprefix, prefix.ToUTF8());
      xxprefix = xprefix;
    }
  if (layer.Len() > 0)
    {
      strcpy(selected_layer, layer.ToUTF8());
      xselected_layer = selected_layer;
    }
  if (force2d)
    force_dims = GAIA_DXF_FORCE_2D;
  else if (force3d)
    force_dims = GAIA_DXF_FORCE_3D;
  if (linked)
    special_rings = GAIA_DXF_RING_LINKED;
  else if (unlinked)
    special_rings = GAIA_DXF_RING_UNLINKED;
  dxf =
    gaiaCreateDxfParser(srid, force_dims, xxprefix, xselected_layer,
                        special_rings);
  if (dxf == NULL)
    {
      *failed = 1;
      goto stop;
    }
// attempting to parse the DXF input file
  strcpy(dxf_path, path.ToUTF8());
  if (gaiaParseDxfFile(dxf, dxf_path))
    {
// loading into the DB 
      int mode = GAIA_DXF_IMPORT_BY_LAYER;
      if (mixed)
        mode = GAIA_DXF_IMPORT_MIXED;
      if (!gaiaLoadFromDxfParser(GetSqlite(), dxf, mode, append))
        {
          wxMessageBox(wxT("DB error while loading: ") + path,
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          *failed = 1;
        }
  } else
    {
      wxMessageBox(wxT("Unable to parse: ") + path, wxT("spatialite_gui"),
                   wxOK | wxICON_ERROR, this);
      *failed = 1;
    }


stop:
// destroying the DXF parser
  gaiaDestroyDxfParser(dxf);
  return 1;
}
