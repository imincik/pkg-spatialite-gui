/*
/ Classdef.h
/ class definitions for spatialite_gui  - a SQLite /SpatiaLite GUI tool
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

#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/treectrl.h"
#include "wx/grid.h"
#include "wx/listctrl.h"
#include "wx/textctrl.h"
#include "wx/propdlg.h"
#include "wx/generic/propdlg.h"
#include "wx/timer.h"

#include "config.h"

#ifdef SPATIALITE_AMALGAMATION
#include <spatialite/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <spatialite/gaiaaux.h>
#include <spatialite/gaiaexif.h>
#include <spatialite/gaiageo.h>
#include <spatialite.h>
#include <spatialite/gg_wfs.h>

#include <freexl.h>

//
// functions for QSORT / BSEARCH
//
int cmp_prenodes_code(const void *p1, const void *p2);
int cmp_prenodes_id(const void *p1, const void *p2);
int cmp_nodes2_code(const void *p1, const void *p2);
int cmp_nodes2_id(const void *p1, const void *p2);
int cmp_nodes1_code(const void *p1, const void *p2);
int cmp_nodes1_id(const void *p1, const void *p2);

//
// functions for threaded queries
//
int SqlProgressCallback(void *arg);
#ifdef _WIN32
DWORD WINAPI DoExecuteSqlThread(void *arg);
#else
void *DoExecuteSqlThread(void *arg);
#endif

//
// functions for threaded WFS download
//
void WfsCallback(int rows, void *ptr);
#ifdef _WIN32
DWORD WINAPI DoExecuteWfs(void *arg);
#else
void *DoExecuteWfs(void *arg);
#endif

enum
{
// control IDs for main window and tree list control
  ID_Connect = 1,
  ID_CreateNew,
  ID_Disconnect,
  ID_MemoryDbLoad,
  ID_MemoryDbNew,
  ID_MemoryDbClock,
  ID_MemoryDbSave,
  ID_Vacuum,
  ID_SqlScript,
  ID_QueryViewComposer,
  ID_LoadShp,
  ID_LoadTxt,
  ID_LoadDbf,
  ID_LoadXL,
  ID_VirtualShp,
  ID_VirtualTxt,
  ID_VirtualDbf,
  ID_VirtualXL,
  ID_Network,
  ID_Exif,
  ID_LoadXml,
  ID_Srids,
  ID_Charset,
  ID_Help,
  ID_Attach,
  ID_SqlLog,
  ID_DbStatus,
  ID_CheckGeom,
  ID_SaneGeom,
  ID_WFS,
  ID_DXF,
  Tree_NewTable,
  Tree_NewView,
  Tree_NewIndex,
  Tree_NewTrigger,
  Tree_NewColumn,
  Tree_QueryViewComposer,
  Tree_Show,
  Tree_Drop,
  Tree_Rename,
  Tree_Select,
  Tree_Refresh,
  Tree_SpatialIndex,
  Tree_CheckSpatialIndex,
  Tree_RecoverSpatialIndex,
  Tree_MbrCache,
  Tree_RebuildTriggers,
  Tree_ShowSql,
  Tree_Recover,
  Tree_CheckGeometry,
  Tree_Extent,
  Tree_UpdateLayerStatistics,
  Tree_UpdateLayerStatisticsAll,
  Tree_ElementaryGeoms,
  Tree_MalformedGeometries,
  Tree_RepairPolygons,
  Tree_SetSrid,
  Tree_DumpShp,
  Tree_DumpKml,
  Tree_DumpTxtTab,
  Tree_DumpCsv,
  Tree_DumpHtml,
  Tree_DumpDif,
  Tree_DumpSylk,
  Tree_DumpDbf,
  Tree_DumpPostGIS,
  Tree_Edit,
  Tree_DropColumn,
  Tree_RenameColumn,
  Tree_GisLayerAuth,
  Tree_ColumnStats,
  Tree_MapPreview,
  Tree_CheckDuplicates,
  Tree_RemoveDuplicates,
  Tree_Detach,
  Tree_CheckGeom,
  Tree_SaneGeom,
  Grid_Clear,
  Grid_All,
  Grid_Column,
  Grid_Row,
  Grid_Copy,
  Grid_Blob,
  Grid_Delete,
  Grid_Insert,
  Grid_Abort,
  Grid_BlobIn,
  Grid_BlobOut,
  Grid_BlobNull,
  Grid_XmlBlobIn,
  Grid_XmlBlobOut,
  Grid_XmlBlobOutIndented,
  Grid_ExpTxtTab,
  Grid_ExpCsv,
  Grid_ExpHtml,
  Grid_ExpShp,
  Grid_ExpDif,
  Grid_ExpSylk,
  Grid_ExpDbf,
  Image_Copy,
  Wfs_Copy,
  Wfs_Layer
};

enum
{
// control IDs for dialogs
  ID_SQL = 10000,
  ID_SQL_GO,
  ID_SQL_ABORT,
  ID_HISTORY_BACK,
  ID_HISTORY_FORWARD,
  ID_RS_FIRST,
  ID_RS_LAST,
  ID_RS_NEXT,
  ID_RS_PREVIOUS,
  ID_REFRESH,
  ID_RS_BLOCK,
  ID_RS_THREAD_FINISHED,
  ID_RS_STATS_UPDATE,
  ID_PANE_HEXADECIMAL,
  ID_PANE_GEOMETRY,
  ID_PANE_WKT,
  ID_PANE_EWKT,
  ID_PANE_SVG,
  ID_PANE_KML,
  ID_PANE_GML,
  ID_PANE_GEOJSON,
  ID_PANE_IMAGE,
  ID_HEX,
  ID_GEOM_TABLE,
  ID_WKT_TABLE,
  ID_WKT_COPY,
  ID_EWKT_TABLE,
  ID_EWKT_COPY,
  ID_SVG_TABLE,
  ID_SVG_RELATIVE,
  ID_SVG_PRECISION,
  ID_SVG_COPY,
  ID_KML_TABLE,
  ID_KML_PRECISION,
  ID_KML_COPY,
  ID_GML_TABLE,
  ID_GML_V2_V3,
  ID_GML_PRECISION,
  ID_GML_COPY,
  ID_GEOJSON_TABLE,
  ID_GEOJSON_OPTIONS,
  ID_GEOJSON_PRECISION,
  ID_GEOJSON_COPY,
  ID_GEOM_GRAPH,
  ID_GEOM_BOX,
  ID_IMAGE_TITLE,
  ID_IMG_BOX,
  ID_IMAGE,
  ID_XML_DOCUMENT_TABLE,
  ID_XML_DOCUMENT_COPY,
  ID_XML_INDENTED_TABLE,
  ID_XML_INDENTED_COPY,
  ID_VIRTSHP_TABLE,
  ID_VIRTSHP_SRID,
  ID_VIRTSHP_CHARSET,
  ID_VIRTTXT_TABLE,
  ID_VIRTTXT_CHARSET,
  ID_VIRTTXT_TITLES,
  ID_VIRTTXT_SEPARATOR,
  ID_VIRTTXT_CHARSEPARATOR,
  ID_VIRTTXT_QUOTE,
  ID_VIRTTXT_POINT,
  ID_VIRTDBF_TABLE,
  ID_VIRTDBF_CHARSET,
  ID_VIRTXL_TABLE,
  ID_VIRTXL_WORKSHEET,
  ID_VIRTXL_TITLES,
  ID_LDSHP_TABLE,
  ID_LDSHP_COLUMN,
  ID_LDSHP_SRID,
  ID_LDSHP_CHARSET,
  ID_LDSHP_COERCE_2D,
  ID_LDSHP_COMPRESSED,
  ID_LDSHP_RTREE,
  ID_LDSHP_USER_GTYPE,
  ID_LDSHP_GTYPE,
  ID_LDSHP_USER_PKEY,
  ID_LDSHP_PKCOL,
  ID_LDXL_TABLE,
  ID_LDXL_WORKSHEET,
  ID_LDXL_TITLES,
  ID_LDXML_COMPRESSED,
  ID_LDXML_VALIDATE,
  ID_LDXML_INTERNAL_SCHEMA,
  ID_LDXML_SCHEMA_URI,
  ID_LDTXT_TABLE,
  ID_LDTXT_CHARSET,
  ID_LDTXT_TITLES,
  ID_LDTXT_SEPARATOR,
  ID_LDTXT_CHARSEPARATOR,
  ID_LDTXT_QUOTE,
  ID_LDTXT_POINT,
  ID_LDDBF_TABLE,
  ID_LDDBF_CHARSET,
  ID_LDDBF_USER_PKEY,
  ID_LDDBF_PKCOL,
  ID_DMPSHP_CHARSET,
  ID_DMPTXT_CHARSET,
  ID_NET_TABLE,
  ID_NET_FROM,
  ID_NET_TO,
  ID_NET_GEOM,
  ID_NET_LENGTH,
  ID_NET_COST,
  ID_NET_BIDIR,
  ID_NET_ONEWAY,
  ID_NET_FROM_TO,
  ID_NET_TO_FROM,
  ID_NET_NAME_ENABLE,
  ID_NET_NAME,
  ID_NET_A_STAR,
  ID_EXIF_PATH,
  ID_EXIF_FOLDER,
  ID_EXIF_METADATA,
  ID_EXIF_GPS_ONLY,
  ID_XML_OK_SUFFIX,
  ID_XML_SUFFIX,
  ID_XML_PATH,
  ID_XML_FOLDER,
  ID_XML_TARGET_TABLE,
  ID_XML_PK_NAME,
  ID_XML_BLOB_COLUMN,
  ID_XML_OK_PATH,
  ID_XML_PATH_COLUMN,
  ID_XML_OK_SCHEMA_URI,
  ID_XML_SCHEMA_URI_COLUMN,
  ID_XML_OK_PARSE_ERR,
  ID_XML_PARSE_ERR_COLUMN,
  ID_XML_OK_VALIDATE_ERR,
  ID_XML_VALIDATE_ERR_COLUMN,
  ID_XML_COMPRESSED,
  ID_XML_VALIDATED,
  ID_XML_INTERNAL_SCHEMA,
  ID_XML_SCHEMA_URI,
  ID_DXF_OK_PREFIX,
  ID_DXF_PREFIX,
  ID_DXF_PATH,
  ID_DXF_FOLDER,
  ID_DXF_OK_SINGLE,
  ID_DXF_SINGLE,
  ID_DXF_DIMS,
  ID_DXF_SRID,
  ID_DXF_RINGS,
  ID_DXF_MIXED,
  ID_DXF_APPEND,
  ID_DFLT_CHARSET,
  ID_DFLT_ASK,
  ID_SCRIPT_CHARSET,
  ID_RCVR_SRID,
  ID_RCVR_TYPE,
  ID_RCVR_DIMS,
  ID_SRID_OLD,
  ID_SRID_SRID,
  ID_SEARCH,
  ID_BY_SRID,
  ID_HELP_HTML,
  ID_AUTO_SAVE_PATH,
  ID_AUTO_SAVE_INTERVAL,
  ID_AUTO_SAVE_CHANGE_PATH,
  ID_QVC_SQL,
  ID_QVC_TAB,
  ID_QVC_TABLE_2,
  ID_QVC_TABLE_NAME_1,
  ID_QVC_TABLE_NAME_2,
  ID_QVC_TABLE_ALIAS_1,
  ID_QVC_TABLE_ALIAS_2,
  ID_QVC_COLUMNS_1,
  ID_QVC_COLUMNS_2,
  ID_QVC_JOIN_MODE,
  ID_QVC_MATCH_1_T1,
  ID_QVC_MATCH_1_T2,
  ID_QVC_MATCH_2_ENABLE,
  ID_QVC_MATCH_2_T1,
  ID_QVC_MATCH_2_T2,
  ID_QVC_MATCH_3_ENABLE,
  ID_QVC_MATCH_3_T1,
  ID_QVC_MATCH_3_T2,
  ID_QVC_WHERE_1_ENABLE,
  ID_QVC_WHERE_1_TABLE,
  ID_QVC_WHERE_1_COLUMN,
  ID_QVC_WHERE_1_OPERATOR,
  ID_QVC_WHERE_1_VALUE,
  ID_QVC_WHERE_2_ENABLE,
  ID_QVC_WHERE_2_TABLE,
  ID_QVC_WHERE_2_COLUMN,
  ID_QVC_WHERE_2_OPERATOR,
  ID_QVC_WHERE_2_VALUE,
  ID_QVC_WHERE_3_ENABLE,
  ID_QVC_WHERE_3_TABLE,
  ID_QVC_WHERE_3_COLUMN,
  ID_QVC_WHERE_3_OPERATOR,
  ID_QVC_WHERE_3_VALUE,
  ID_QVC_CONNECTOR_12,
  ID_QVC_CONNECTOR_23,
  ID_QVC_ORDER_1_ENABLE,
  ID_QVC_ORDER_1_TABLE,
  ID_QVC_ORDER_1_COLUMN,
  ID_QVC_ORDER_1_DESC,
  ID_QVC_ORDER_2_ENABLE,
  ID_QVC_ORDER_2_TABLE,
  ID_QVC_ORDER_2_COLUMN,
  ID_QVC_ORDER_2_DESC,
  ID_QVC_ORDER_3_ENABLE,
  ID_QVC_ORDER_3_TABLE,
  ID_QVC_ORDER_3_COLUMN,
  ID_QVC_ORDER_3_DESC,
  ID_QVC_ORDER_4_ENABLE,
  ID_QVC_ORDER_4_TABLE,
  ID_QVC_ORDER_4_COLUMN,
  ID_QVC_ORDER_4_DESC,
  ID_QVC_VIEW_TYPE,
  ID_QVC_VIEW_NAME,
  ID_QVC_VIEW_GEOTABLE,
  ID_QVC_VIEW_GEOMETRY,
  ID_QVC_WRITABLE_1,
  ID_QVC_WRITABLE_2,
  ID_AUTH_RDONLY,
  ID_AUTH_HIDDEN,
  ID_MALFORMED_GRID,
  ID_MALFORMED_CLOSE,
  ID_MALFORMED_REPAIR,
  ID_MALFORMED_CLEAR,
  ID_MALFORMED_ALL,
  ID_MALFORMED_ROW,
  ID_MALFORMED_COLUMN,
  ID_MALFORMED_COPY,
  ID_MALFORMED_BLOB,
  ID_DB_STATUS_GRID,
  ID_DB_STATUS_CLOSE,
  ID_DB_STATUS_RESET,
  ID_DB_STATUS_CLEAR,
  ID_DB_STATUS_ALL,
  ID_DB_STATUS_ROW,
  ID_DB_STATUS_COLUMN,
  ID_DB_STATUS_COPY,
  ID_STAT_CHART,
  ID_CHART_TYPE,
  ID_CHART_SIZE,
  ID_CHART_MODE,
  ID_CHART_CLASS,
  ID_CHART_COPY,
  ID_CHART_PNG,
  ID_CHART_SVG,
  ID_CHART_PDF,
  ID_MAP_SYMBOL,
  ID_MAP_SIZE,
  ID_MAP_SYM_SIZE,
  ID_MAP_THICKNESS,
  ID_MAP_FILL,
  ID_MAP_FILL_COL,
  ID_MAP_LINE_COL,
  ID_MAP_COPY,
  ID_MAP_PNG,
  ID_MAP_SVG,
  ID_MAP_PDF,
  ID_KML_NAME,
  ID_KML_NAME_K,
  ID_KML_DESC,
  ID_KML_DESC_K,
  ID_SHEET_DECIMAL_POINT,
  ID_SHEET_DATE_TIME,
  ID_ELEMGEOM_TABLE,
  ID_ELEMGEOM_PKEY,
  ID_ELEMGEOM_MULTI_ID,
  ID_ELEMGEOM_TYPE,
  ID_ELEMGEOM_SRID,
  ID_ELEMGEOM_COORDS,
  ID_ELEMGEOM_RTREE,
  ID_POSTGIS_SCHEMA,
  ID_POSTGIS_TABLE,
  ID_POSTGIS_LOWER,
  ID_POSTGIS_CREATE,
  ID_POSTGIS_SPINDEX,
  ID_SANEGEOM_PREFIX,
  ID_WFS_URL,
  ID_WFS_CATALOG,
  ID_WFS_RESET,
  ID_WFS_NAME,
  ID_WFS_SRID,
  ID_WFS_VERSION,
  ID_WFS_LABEL,
  ID_WFS_PAGE,
  ID_WFS_MAX,
  ID_WFS_PAGING,
  ID_WFS_TABLE,
  ID_WFS_PK,
  ID_WFS_RTREE,
  ID_WFS_SWAP,
  ID_WFS_LOAD,
  ID_WFS_EXTRA,
  ID_WFS_KEYWORD,
  ID_WFS_KEYFILTER,
  ID_WFS_KEYRESET,
  ID_WFS_STATUS,
  ID_WFS_THREAD_FINISHED
};

enum
{
// tree item data types
  MY_TABLE = 0,
  MY_VTABLE,
  MY_VIEW,
  MY_COLUMN,
  MY_VIEW_COLUMN,
  MY_VIRTUAL_COLUMN,
  MY_GEOMETRY,
  MY_GEOMETRY_INDEX,
  MY_GEOMETRY_CACHED,
  MY_VIEW_GEOMETRY,
  MY_VIEW_GEOMETRY_INDEX,
  MY_VIEW_GEOMETRY_CACHED,
  MY_VIRTUAL_GEOMETRY,
  MY_INDEX,
  MY_TRIGGER,
  MY_ATTACHED,
  MY_INT_VARIANT,
  MY_DBL_VARIANT,
  MY_TXT_VARIANT,
  MY_BLOB_VARIANT,
  MY_NULL_VARIANT
};

enum
{
// control IDs for timers
  ID_AUTO_SAVE_TIMER = 20000,
  ID_DB_STATUS_TIMER,
  ID_WFS_TIMER,
};

enum
{
// DOT-COMMANDS [SQL scripts]
  CMD_NONE = 0,
  CMD_LOADSHP,
  CMD_LOADDBF,
  CMD_LOADXL,
  CMD_DUMPSHP,
  CMD_DUMPDBF,
  CMD_SQLLOG
};

enum
{
// METADATA TYPEs
  METADATA_UNKNOWN = 0,
  METADATA_LEGACY,
  METADATA_CURRENT
};

class TopologyItem
{
//
// a class wrapping a Topology related Table or View
//
private:
  wxString Name;
  bool Table;
  bool View;
  TopologyItem *Next;
public:
    TopologyItem(wxString & name);
   ~TopologyItem()
  {;
  }
  wxString & GetName()
  {
    return Name;
  }
  void SetTable()
  {
    Table = true;
  }
  void SetView()
  {
    View = true;
  }
  bool IsTable()
  {
    return Table;
  }
  bool IsView()
  {
    return View;
  }
  void SetNext(TopologyItem * next)
  {
    Next = next;
  }
  TopologyItem *GetNext()
  {
    return Next;
  }
};

class TopologySet
{
//
// a class representing a full Topology Set
private:
  wxString Prefix;
  wxString CoordDims;
  int Srid;
  TopologyItem *First;
  TopologyItem *Last;
public:
    TopologySet();
   ~TopologySet();
  bool CheckPrefix();
  void SetPrefix(wxString & prefix)
  {
    Prefix = prefix;
  }
  wxString & GetPrefix()
  {
    return Prefix;
  }
  void SetSrid(int srid)
  {
    Srid = srid;
  }
  int GetSrid()
  {
    return Srid;
  }
  void SetCoordDims(const char *coords);
  void SetCoordDims(wxString & coords)
  {
    CoordDims = coords;
  }
  wxString & GetCoordDims()
  {
    return CoordDims;
  }
  void Add(const char *name, bool table, bool view);
  void AddTable(wxString & name);
  void AddView(wxString & name);
  void SetTable(wxString & table_name);
  void SetView(wxString & view_name);
  TopologyItem *GetFirst()
  {
    return First;
  }
};

class MyObject:public wxTreeItemData
{
//
// a class to store TreeItemData
//
private:
  int Type;                     // the object type
  wxString DbAlias;             // the DB alias [Attached DB] 
  wxString Name;                // the object name
  wxString Column;              // the column name [optional]
  bool Temporary;               // the TMP switch
public:
    MyObject(int type, wxString & name);
    MyObject(int type, wxString & name, bool tmp);
    MyObject(int type, wxString & dbAlias, wxString & name, bool tmp);
    MyObject(int type, wxString & name, wxString & column);
    MyObject(int type, bool attached, wxString & dbAlias, wxString & name);
    virtual ~ MyObject()
  {;
  }
  int GetType()
  {
    return Type;
  }
  wxString & GetDbAlias()
  {
    return DbAlias;
  }
  wxString & GetName()
  {
    return Name;
  }
  wxString & GetColumn()
  {
    return Column;
  }
  bool IsTemporary()
  {
    return Temporary;
  }
  bool IsAttached()
  {
    if (DbAlias.Len() > 0)
      return true;
    else
      return false;
  }
};

class MyColumnInfo
{
//
// a class to store a DB column
//
private:
  wxString Name;                // the column name
  bool PrimaryKey;              // Primary Key column
  bool Geometry;                // Geometry column
  bool GeometryIndex;           // Geometry column + SpatialIndex
  bool MbrCache;                // Geometry column + MbrCache
  MyColumnInfo *Next;           // pointer to next element into the linked list
public:
    MyColumnInfo(wxString & name, bool pkey);
   ~MyColumnInfo()
  {;
  }
  wxString & GetName()
  {
    return Name;
  }
  bool IsPrimaryKey()
  {
    return PrimaryKey;
  }
  void SetGeometry()
  {
    Geometry = true;
  }
  bool IsGeometry()
  {
    return Geometry;
  }
  void SetGeometryIndex()
  {
    GeometryIndex = true;
  }
  bool IsGeometryIndex()
  {
    return GeometryIndex;
  }
  void SetMbrCache()
  {
    MbrCache = true;
  }
  bool IsMbrCache()
  {
    return MbrCache;
  }
  void SetNext(MyColumnInfo * next)
  {
    Next = next;
  }
  MyColumnInfo *GetNext()
  {
    return Next;
  }
};

class MyIndexInfo
{
//
// a class to store a DB index
//
private:
  wxString Name;                // the index name
  MyIndexInfo *Next;            // pointer to next element into the linked list
public:
    MyIndexInfo(wxString & name);
    MyIndexInfo()
  {;
  }
  wxString & GetName()
  {
    return Name;
  }
  void SetNext(MyIndexInfo * next)
  {
    Next = next;
  }
  MyIndexInfo *GetNext()
  {
    return Next;
  }
};

class MyTriggerInfo
{
//
// a class to store a DB trigger
//
private:
  wxString Name;                // the trigger name
  MyTriggerInfo *Next;          // pointer to next element into the linked list
public:
    MyTriggerInfo(wxString & name);
   ~MyTriggerInfo()
  {;
  }
  wxString & GetName()
  {
    return Name;
  }
  void SetNext(MyTriggerInfo * next)
  {
    Next = next;
  }
  MyTriggerInfo *GetNext()
  {
    return Next;
  }
};

class MyTableInfo
{
//
// a class to store DB table columns
//
private:
  MyColumnInfo * FirstColumn;   // first element into the columns linked list
  MyColumnInfo *LastColumn;     // last element into the columns linked list
  MyIndexInfo *FirstIndex;      // first element into the indices linked list
  MyIndexInfo *LastIndex;       // last element into the indices linked list
  MyTriggerInfo *FirstTrigger;  // first element into the triggers linked list
  MyTriggerInfo *LastTrigger;   // last element into the triggers linked list
public:
    MyTableInfo()
  {
    FirstColumn = NULL;
    LastColumn = NULL;
    FirstIndex = NULL;
    LastIndex = NULL;
    FirstTrigger = NULL;
    LastTrigger = NULL;
  }
   ~MyTableInfo();
  void AddColumn(wxString & name, bool pkey);
  void SetGeometry(wxString & name, bool index, bool cached);
  void AddIndex(wxString & name);
  void AddTrigger(wxString & name);
  MyColumnInfo *GetFirstColumn()
  {
    return FirstColumn;
  }
  MyIndexInfo *GetFirstIndex()
  {
    return FirstIndex;
  }
  MyTriggerInfo *GetFirstTrigger()
  {
    return FirstTrigger;
  }
};

class MyViewInfo
{
//
// a class to store DB view columns
//
private:
  MyColumnInfo * First;         // first element into the columns linked list
  MyColumnInfo *Last;           // last element into the columns linked list
  MyTriggerInfo *FirstTrigger;  // first element into the triggers linked list
  MyTriggerInfo *LastTrigger;   // last element into the triggers linked list
public:
    MyViewInfo()
  {
    First = NULL;
    Last = NULL;
    FirstTrigger = NULL;
    LastTrigger = NULL;
  }
   ~MyViewInfo();
  void AddColumn(wxString & name);
  void AddTrigger(wxString & name);
  void SetGeometry(wxString & name, bool index, bool cached);
  MyColumnInfo *GetFirst()
  {
    return First;
  }
  MyTriggerInfo *GetFirstTrigger()
  {
    return FirstTrigger;
  }
};

class MyVariant
{
//
// a class to store Variant-Type values
//
private:
  int Type;                     // the Variant-Type
  sqlite3_int64 IntValue;       // the Integer value
  double DblValue;              // the Double value
  wxString TxtValue;            // the Text value
  unsigned char *Blob;          // the BLOB value
  int BlobSize;                 // the BLOB size
public:
    MyVariant()
  {
    Type = MY_NULL_VARIANT;
    Blob = NULL;
  }
   ~MyVariant()
  {
    if (Blob)
      delete[]Blob;
  }
  void Clear()
  {
    if (Blob)
      delete[]Blob;
    Blob = NULL;
    Type = MY_NULL_VARIANT;
  }
  void Set(sqlite3_int64 value)
  {
    Type = MY_INT_VARIANT;
    IntValue = value;
  }
  void Set(double value)
  {
    Type = MY_DBL_VARIANT;
    DblValue = value;
  }
  void Set(const unsigned char *text);
  void Set(wxString & string)
  {
    Type = MY_TXT_VARIANT;
    TxtValue = string;
  }
  void Set(const void *blob, int size);
  void Copy(MyVariant * other);
  int GetType()
  {
    return Type;
  }
  sqlite3_int64 GetIntValue()
  {
    return IntValue;
  }
  double GetDblValue()
  {
    return DblValue;
  }
  wxString & GetTxtValue()
  {
    return TxtValue;
  }
  int GetBlobSize()
  {
    return BlobSize;
  }
  unsigned char *GetBlob()
  {
    return Blob;
  }
};

class MyRowVariant
{
//
// a class to store a row composed of Variant-Type values
//
private:
  int NumCols;                  // number of columns
  MyVariant *ColumnArray;       // the column as an array
  bool Deleted;                 // switch to mark row deletion
  MyRowVariant *Next;           // pointer to next element into the linked list
public:
    MyRowVariant()
  {
    NumCols = 0;
    ColumnArray = NULL;
    Deleted = false;
    Next = NULL;
  }
  MyRowVariant(int cols)
  {
    NumCols = cols;
    ColumnArray = new MyVariant[cols];
    Next = NULL;
  }
  ~MyRowVariant()
  {
    if (ColumnArray)
      delete[]ColumnArray;
  }
  void Create(int cols);
  int GetNumCols()
  {
    return NumCols;
  }
  void Set(int col, sqlite3_int64 value);
  void Set(int col, double value);
  void Set(int col, const unsigned char *text);
  void Set(int col, const void *blob, int size);
  MyVariant *GetColumn(int col);
  void SetDeleted()
  {
    Deleted = true;
  }
  bool IsDeleted()
  {
    return Deleted;
  }
  void SetNext(MyRowVariant * next)
  {
    Next = next;
  }
  MyRowVariant *GetNext()
  {
    return Next;
  }
};

class MyVariantList
{
//
// a class to store a whole result set
//
private:
  int NumCols;                  // number of columns
  wxString *ColumnName;         // the column names
  MyRowVariant *First;          // first element into the linked list
  MyRowVariant *Last;           // last element into the linked list
public:
    MyVariantList();
   ~MyVariantList();
  void Reset(void);
  MyRowVariant *Add(int columns);
  void SetColumnName(int col, const char *colName);
  MyRowVariant *GetFirst()
  {
    return First;
  }
  int GetRows();
  int GetColumns()
  {
    return NumCols;
  }
  wxString & GetColumnName(int col);
};

class MyBlobs
{
//
// a class to store BLOBs
//
private:
  int NumRows;                  // the number of rows
  int NumCols;                  // the number of columns
  MyRowVariant *Rows;           // pointer to an array of rows
public:
    MyBlobs(int rows, int cols);
   ~MyBlobs();
  void SetBlob(int row, int col, MyVariant * blobVar);
  MyVariant *GetBlob(int row, int col);
};

class MyValues
{
//
// a class to store column values for editing
//
private:
  int NumRows;                  // the number of rows
  int NumCols;                  // the number of columns
  MyRowVariant *Rows;           // pointer to an array of rows
public:
    MyValues(int rows, int cols);
   ~MyValues();
  void SetValue(int row, int col, sqlite3_int64 value);
  void SetValue(int row, int col, double value);
  void SetValue(int row, int col, wxString & string);
  MyRowVariant *GetRow(int row);
  MyVariant *GetValue(int row, int col);
};

class MySqlQuery
{
//
// a class to store an SQL query - history
//
private:
  wxString Sql;
  MySqlQuery *Prev;
  MySqlQuery *Next;
public:
    MySqlQuery(wxString & sql)
  {
    Sql = sql;
    Prev = NULL;
    Next = NULL;
  }
   ~MySqlQuery()
  {;
  }
  wxString & GetSql()
  {
    return Sql;
  }
  void SetPrev(MySqlQuery * prev)
  {
    Prev = prev;
  }
  MySqlQuery *GetPrev()
  {
    return Prev;
  }
  void SetNext(MySqlQuery * next)
  {
    Next = next;
  }
  MySqlQuery *GetNext()
  {
    return Next;
  }
};

class MySqlHistory
{
//
// a class supporting SQL queries history
//
private:
  MySqlQuery * First;
  MySqlQuery *Last;
  MySqlQuery *Current;
public:
    MySqlHistory()
  {
    First = NULL;
    Last = NULL;
    Current = NULL;
  }
   ~MySqlHistory();
  void Prepend(wxString & sql);
  void Add(wxString & sql);
  MySqlQuery *GetCurrent()
  {
    return Current;
  }
  MySqlQuery *GetNext();
  MySqlQuery *GetPrev();
  bool TestNext();
  bool TestPrev();
};

class MyApp:public wxApp
{
//
// the main APP
//
  virtual bool OnInit();
};

class DuplColumn
{
//
// a column value in a duplicated row
//
private:
  int Pos;
  wxString Name;
  int Type;
  sqlite3_int64 IntValue;
  double DblValue;
  const char *TxtValue;
  const void *Blob;
  int Size;
  int QueryPos;
  DuplColumn *Next;
public:
    DuplColumn(int pos, wxString & name)
  {
    Pos = pos;
    Name = name;
    Type = SQLITE_NULL;
    Next = NULL;
  }
   ~DuplColumn()
  {;
  }
  int GetPos()
  {
    return Pos;
  }
  wxString & GetName()
  {
    return Name;
  }
  void SetValue(sqlite3_int64 value)
  {
    Type = SQLITE_INTEGER;
    IntValue = value;
  }
  void SetValue(double value)
  {
    Type = SQLITE_FLOAT;
    DblValue = value;
  }
  void SetValue(const char *value)
  {
    Type = SQLITE_TEXT;
    TxtValue = value;
  }
  void SetValue(const void *blob, int size)
  {
    Type = SQLITE_BLOB;
    Blob = blob;
    Size = size;
  }
  void SetValue(void)
  {
    Type = SQLITE_NULL;
  }
  int GetType()
  {
    return Type;
  }
  sqlite3_int64 GetIntValue()
  {
    return IntValue;
  }
  double GetDblValue()
  {
    return DblValue;
  }
  const char *GetTxtValue()
  {
    return TxtValue;
  }
  bool CheckBlob(const void *blob, int size);
  void SetQueryPos(int pos)
  {
    QueryPos = pos;
  }
  int GetQueryPos()
  {
    return QueryPos;
  }
  void SetNext(DuplColumn * next)
  {
    Next = next;
  }
  DuplColumn *GetNext()
  {
    return Next;
  }
};

class DuplRow
{
//
// a duplicated row with column values
//
private:
  int Count;
  DuplColumn *First;
  DuplColumn *Last;
  wxString Table;
public:
    DuplRow()
  {
    Count = 0;
    First = NULL;
    Last = NULL;
  }
   ~DuplRow();
  void SetTable(wxString & table)
  {
    Table = table;
  }
  wxString & GetTable()
  {
    return Table;
  }
  void Add(wxString & name);
  void SetValue(int pos, sqlite3_int64 value);
  void SetValue(int pos, double value);
  void SetValue(int pos, const char *value);
  void SetValue(int pos, const void *blob, int size);
  void SetValue(int pos);
  void ResetQueryPos();
  bool CheckBlob(int pos, const void *blob, int size);
  DuplColumn *GetFirst()
  {
    return First;
  }
};

class Topology
{
//
// Topology container
//
private:
  wxTreeItemId TopologyNode;
  TopologySet TopologyItems;
  Topology *Next;
public:
    Topology(class MyTableTree * tree, wxTreeItemId & root,
             TopologySet * topology);
   ~Topology()
  {;
  }
  wxTreeItemId *Check(wxString & table);
  Topology *GetNext()
  {
    return Next;
  }
  void SetNext(Topology * next)
  {
    Next = next;
  }
};

class TopologyList
{
//
// Topology container
//
private:
  Topology * First;
  Topology *Last;
  int Count;
public:
    TopologyList()
  {
    First = NULL;
    Last = NULL;
    Count = 0;
  }
   ~TopologyList()
  {
    Flush();
  }
  void Flush();
  void Add(class MyTableTree * tree, wxTreeItemId & root,
           TopologySet * topology);
  wxTreeItemId *FindNode(wxString & table);
  int GetCount()
  {
    return Count;
  }
};

class RootNodes
{
//
// a class wrapping root nodes for an Attached DB
//
private:
  wxString dbAlias;
  wxTreeItemId rootUserData;
  wxTreeItemId rootTopologies;
  wxTreeItemId rootStyling;
  wxTreeItemId rootIsoMetadata;
  wxTreeItemId rootMetadata;
  wxTreeItemId rootInternal;
  wxTreeItemId rootSpatialIndex;
public:
    RootNodes(wxString & alias, wxTreeItemId userData, wxTreeItemId topologies,
              wxTreeItemId styling, wxTreeItemId isoMetadata,
              wxTreeItemId metadata, wxTreeItemId internal,
              wxTreeItemId spatialIndex)
  {
    dbAlias = alias;
    rootUserData = userData;
    rootTopologies = topologies;
    rootStyling = styling;
    rootIsoMetadata = isoMetadata;
    rootMetadata = metadata;
    rootInternal = internal;
    rootSpatialIndex = spatialIndex;
  }
   ~RootNodes()
  {;
  }
  wxString & GetDbAlias()
  {
    return dbAlias;
  }
  wxTreeItemId & GetRootUserData()
  {
    return rootUserData;
  }
  wxTreeItemId & GetRootTopologies()
  {
    return rootTopologies;
  }
  wxTreeItemId & GetRootStyling()
  {
    return rootStyling;
  }
  wxTreeItemId & GetRootIsoMetadata()
  {
    return rootIsoMetadata;
  }
  wxTreeItemId & GetRootMetadata()
  {
    return rootMetadata;
  }
  wxTreeItemId & GetRootInternal()
  {
    return rootInternal;
  }
  wxTreeItemId & GetRootSpatialIndex()
  {
    return rootSpatialIndex;
  }
};

class MyTableTree:public wxTreeCtrl
{
//
// a tree-control used for SQLite DB tables
//
private:
  class MyFrame * MainFrame;
  wxTreeItemId Root;            // the root node
  wxTreeItemId RootUserData;
  wxTreeItemId RootTopologies;
  wxTreeItemId RootStyling;
  wxTreeItemId RootIsoMetadata;
  TopologyList Topologies;
  wxTreeItemId RootMetadata;
  wxTreeItemId RootInternal;
  wxTreeItemId RootSpatialIndex;
  wxImageList *Images;          // the images list
  wxTreeItemId CurrentItem;     // the tree item holding the current context menu
  bool doDeleteDuplicates(wxString & sql1, wxString & sql2, DuplRow * values,
                          int *count);
  bool doDeleteDuplicates2(sqlite3_stmt * stmt1, DuplRow * values, int *count);
public:
    MyTableTree()
  {;
  }
  MyTableTree(class MyFrame * parent, wxWindowID id = wxID_ANY);
  virtual ~ MyTableTree()
  {;
  }
  void SetPath(wxString & path)
  {
    SetItemText(Root, path);
  }
  void FlushAll();
  wxTreeItemId & GetRootNode(wxString & tableName);
  wxTreeItemId & GetRootNode(wxString & tableName, RootNodes * nodes);
  void AddTable(wxString & tableName, bool virtualTable, bool tmp);
  void AddView(wxString & viewName, bool tmp);
  wxTreeItemId & AddAttached(wxString & dbAlias, wxString & path);
  void AddTable(wxString & dbAlias, wxString & tableName,
                bool virtualTable, RootNodes * list);
  void AddView(wxString & dbAlias, wxString & viewName, RootNodes * list);
  void ExpandRoot()
  {
    Expand(Root);
    Expand(RootUserData);
    CollapseAllChildren(RootTopologies);
    CollapseAllChildren(RootStyling);
    CollapseAllChildren(RootIsoMetadata);
    Collapse(RootMetadata);
    Collapse(RootInternal);
    Collapse(RootSpatialIndex);
  }
  void AddTopology(TopologySet * topology)
  {
    Topologies.Add(this, RootTopologies, topology);
  }
  void AddTopology(wxTreeItemId & rootTopologies, TopologySet * topology)
  {
    Topologies.Add(this, rootTopologies, topology);
  }
  void DeleteTopologies(wxTreeItemId & root_topologies);
  void OnSelChanged(wxTreeEvent & event);
  void OnRightClick(wxTreeEvent & event);
  void OnCmdQueryViewComposer(wxCommandEvent & event);
  void OnCmdNewTable(wxCommandEvent & event);
  void OnCmdNewView(wxCommandEvent & event);
  void OnCmdNewIndex(wxCommandEvent & event);
  void OnCmdNewTrigger(wxCommandEvent & event);
  void OnCmdNewColumn(wxCommandEvent & event);
  void OnCmdShow(wxCommandEvent & event);
  void OnCmdDrop(wxCommandEvent & event);
  void OnCmdRename(wxCommandEvent & event);
  void OnCmdSelect(wxCommandEvent & event);
  void OnCmdRefresh(wxCommandEvent & event);
  void OnCmdRecover(wxCommandEvent & event);
  void OnCmdShowSql(wxCommandEvent & event);
  void OnCmdSpatialIndex(wxCommandEvent & event);
  void OnCmdCheckSpatialIndex(wxCommandEvent & event);
  void OnCmdRecoverSpatialIndex(wxCommandEvent & event);
  void OnCmdMbrCache(wxCommandEvent & event);
  void OnCmdRebuildTriggers(wxCommandEvent & event);
  void OnCmdCheckGeometry(wxCommandEvent & event);
  void OnCmdExtent(wxCommandEvent & event);
  void OnCmdUpdateLayerStatistics(wxCommandEvent & event);
  void OnCmdUpdateLayerStatisticsAll(wxCommandEvent & event);
  void OnCmdElementaryGeometries(wxCommandEvent & event);
  void OnCmdMalformedGeometries(wxCommandEvent & event);
  void OnCmdRepairPolygons(wxCommandEvent & event);
  void OnCmdSetSrid(wxCommandEvent & event);
  void OnCmdDumpShp(wxCommandEvent & event);
  void OnCmdDumpKml(wxCommandEvent & event);
  void OnCmdDumpTxtTab(wxCommandEvent & event);
  void OnCmdDumpCsv(wxCommandEvent & event);
  void OnCmdDumpHtml(wxCommandEvent & event);
  void OnCmdDumpDif(wxCommandEvent & event);
  void OnCmdDumpSylk(wxCommandEvent & event);
  void OnCmdDumpDbf(wxCommandEvent & event);
  void OnCmdDumpPostGIS(wxCommandEvent & event);
  void OnCmdEdit(wxCommandEvent & event);
  bool DropRenameAux1(MyObject * obj, class GeomColsList * geometries,
                      bool * autoincrement);
  void DropRenameAux2(MyObject * obj, GeomColsList * geometries,
                      wxString & aliasTable, wxString & renameSql,
                      wxString & dropSql, wxString & disableSpatialIdxSql,
                      wxString & dropSpatialIdxSql,
                      wxString & createSpatialIdxSql,
                      wxString & discardGeometrySql);
  void DropRenameAux3(MyObject * obj, GeomColsList * geometries,
                      class TblIndexList * index, wxString & addGeometrySql);
  void OnCmdDropColumn(wxCommandEvent & event);
  void OnCmdRenameColumn(wxCommandEvent & event);
  void OnCmdGisLayerAuth(wxCommandEvent & event);
  void OnCmdColumnStats(wxCommandEvent & event);
  void OnCmdMapPreview(wxCommandEvent & event);
  void OnCmdCheckDuplicates(wxCommandEvent & event);
  void OnCmdRemoveDuplicates(wxCommandEvent & event);
  void OnCmdDetachDB(wxCommandEvent & event);
  void OnCmdCheckGeometries(wxCommandEvent & event);
  void OnCmdSanitizeGeometries(wxCommandEvent & event);
};

class SqlThreadParams
{
//
// an auxiliary class used for SQL threaded queries
//
private:
  class MyResultSetView * Mother;
  wxString Sql;
  sqlite3_stmt *Stmt;
  int FromRow;
  int EndRow;
  int MaxRow;
  MyVariantList List;
  sqlite3 *Sqlite;
  clock_t Start;
  clock_t LastGuiUpdate;
  bool Error;
  int FetchedRows;
  int StatFullscanStep;
  int StatSort;
  int StatAutoindex;
  double ElapsedTime;
  bool AbortRequested;
  bool Valid;
public:
    SqlThreadParams()
  {
    Reset();
  }
   ~SqlThreadParams()
  {;
  }
  void Initialize(MyResultSetView * mother, wxString & sql, sqlite3_stmt * stmt,
                  int from, sqlite3 * sqlite, clock_t start);
  void Reset(void);
  bool IsValid()
  {
    return Valid;
  }
  MyResultSetView *GetMother()
  {
    return Mother;
  }
  wxString & GetSql()
  {
    return Sql;
  }
  sqlite3_stmt *GetStmt()
  {
    return Stmt;
  }
  int GetFromRow()
  {
    return FromRow;
  }
  void SetEndRow(int end)
  {
    EndRow = end;
  }
  int GetEndRow()
  {
    return EndRow;
  }
  void SetMaxRow(int max)
  {
    MaxRow = max;
  }
  int GetMaxRow()
  {
    return MaxRow;
  }
  MyVariantList *GetList()
  {
    return &List;
  }
  sqlite3 *GetSqlite()
  {
    return Sqlite;
  }
  clock_t GetStart()
  {
    return Start;
  }
  void FetchedRow()
  {
    FetchedRows++;
  }
  void UpdateStats(int fullscan, int sort, int autoindex, clock_t now);
  int GetFetchedRows()
  {
    return FetchedRows;
  }
  int GetStatFullscanStep()
  {
    return StatFullscanStep;
  }
  int GetStatSort()
  {
    return StatSort;
  }
  int GetStatAutoindex()
  {
    return StatAutoindex;
  }
  double GetElapsedTime()
  {
    return ElapsedTime;
  }
  bool GuiHasToBeUpdated(clock_t now, int millisecs);
  void SetLastGuiUpdate(clock_t now)
  {
    LastGuiUpdate = now;
  }
  void SetError()
  {
    Error = true;
  }
  bool IsError()
  {
    return Error;
  }
  void Abort()
  {
    AbortRequested = true;
  }
  bool IsAbortRequested()
  {
    return AbortRequested;
  }
};

class MyResultSetView:public wxPanel
{
//
// a panel to be used for SQL Queries
//
private:
  class MyFrame * MainFrame;
  wxBitmapButton *BtnRsFirst;
  wxBitmapButton *BtnRsLast;
  wxBitmapButton *BtnRsNext;
  wxBitmapButton *BtnRsPrevious;
  wxBitmapButton *BtnRefresh;
  wxStaticText *RsCurrentBlock;
  int RsBlock;
  int RsBeginRow;
  int RsEndRow;
  int RsMaxRow;
  int CurrentEvtRow;
  int CurrentEvtColumn;
  MyVariant *CurrentBlob;
  wxGrid *TableView;
  MyBlobs *TableBlobs;
  MyValues *TableValues;
  bool ReadOnly;
  sqlite3_int64 *RowIds;
  int PrimaryKeys[1024];
  int BlobColumns[1024];
  wxString TableName;
  bool InsertPending;
  MyRowVariant *InsertRow;
  wxString SqlErrorMsg;
  SqlThreadParams ThreadParams;
  void XmlBlobOut(bool indented);
public:
    MyResultSetView()
  {;
  }
  MyResultSetView(MyFrame * parent, wxWindowID id = wxID_ANY);
  virtual ~ MyResultSetView();
  void HideControls();
  void ShowControls();
  wxString & GetSqlErrorMsg()
  {
    return SqlErrorMsg;
  }
  bool IsPrimaryKey(int column);
  bool IsBlobColumn(int column);
  void EditTable(wxString & sql, int *primaryKeys, int *blobCols,
                 wxString & tableName);
  void CreateGrid(int rows, int cols);
  void CreateStatsGrid();
  bool ExecuteSqlPre(wxString & sql, int from, bool read_only);
  bool ExecuteSqlPost(void);
  void AbortRequested(void);
  wxStaticText *GetCurrentBlock()
  {
    return RsCurrentBlock;
  }
  void FormatElapsedTime(double seconds, char *elapsed, bool simple = false);
  int GetRsBlock()
  {
    return RsBlock;
  }
  void SetSqlErrorMsg(wxString & msg)
  {
    SqlErrorMsg = msg;
  }
  void ResizeView(void);
  void DoInsert(bool confirmed);
  void HexBlobValue(unsigned char *blob, int size, wxString & hex);
  void OnSize(wxSizeEvent & event);
  void OnRsFirst(wxCommandEvent & event);
  void OnRsLast(wxCommandEvent & event);
  void OnRsNext(wxCommandEvent & event);
  void OnRsPrevious(wxCommandEvent & event);
  void OnRefresh(wxCommandEvent & event);
  void OnThreadFinished(wxCommandEvent & event);
  void OnStatsUpdate(wxCommandEvent & event);
  void OnCellSelected(wxGridEvent & event);
  void OnRightClick(wxGridEvent & event);
  void OnCellChanged(wxGridEvent & event);
  void OnCmdDelete(wxCommandEvent & event);
  void OnCmdInsert(wxCommandEvent & event);
  void OnCmdAbort(wxCommandEvent & event);
  void OnCmdClearSelection(wxCommandEvent & event);
  void OnCmdSelectAll(wxCommandEvent & event);
  void OnCmdSelectRow(wxCommandEvent & event);
  void OnCmdSelectColumn(wxCommandEvent & event);
  void OnCmdCopy(wxCommandEvent & event);
  void OnCmdBlob(wxCommandEvent & event);
  void OnCmdBlobIn(wxCommandEvent & event);
  void OnCmdBlobOut(wxCommandEvent & event);
  void OnCmdBlobNull(wxCommandEvent & event);
  void OnCmdXmlBlobIn(wxCommandEvent & event);
  void OnCmdXmlBlobOut(wxCommandEvent & event);
  void OnCmdXmlBlobOutIndented(wxCommandEvent & event);
  void OnCmdExpTxtTab(wxCommandEvent & event);
  void OnCmdExpCsv(wxCommandEvent & event);
  void OnCmdExpHtml(wxCommandEvent & event);
  void OnCmdExpShp(wxCommandEvent & event);
  void OnCmdExpDif(wxCommandEvent & event);
  void OnCmdExpSylk(wxCommandEvent & event);
  void OnCmdExpDbf(wxCommandEvent & event);
};

class MySqlControl:public wxTextCtrl
{
//
// the SQL text control
//
private:
  class MyQueryView * Parent;
public:
  MySqlControl(MyQueryView * parent, wxWindowID id, const wxString & value,
               const wxPoint & pos, const wxSize & size, long style);
    virtual ~ MySqlControl()
  {;
  }
  void OnSqlMousePosition(wxMouseEvent & event);
  void OnSqlArrowPosition(wxKeyEvent & event);
};

class SqlTokenizer
{
//
// a class used for tokenizing SQL statements
//
private:
  wxString ** TokenList;
  int Block;
  int Max;
  int Index;
  void Expand();
  void Insert(wxString * token);
  wxString CurrentToken;
public:
    SqlTokenizer(wxString & sql);
   ~SqlTokenizer();
  bool HasMoreTokens();
    wxString & GetNextToken();
};

class MyQueryView:public wxPanel
{
//
// a panel to be used for SQL Queries
//
private:
  class MyFrame * MainFrame;
  MySqlHistory History;
  MySqlControl *SqlCtrl;
  wxBitmapButton *BtnSqlGo;
  wxBitmapButton *BtnSqlAbort;
  wxBitmapButton *BtnHistoryBack;
  wxBitmapButton *BtnHistoryForward;
  int BracketStart;
  int BracketEnd;
  bool IgnoreEvent;
public:
    MyQueryView()
  {;
  }
  MyQueryView(MyFrame * parent, wxWindowID id = wxID_ANY);
  virtual ~ MyQueryView()
  {;
  }
  void HideControls();
  void ShowControls();
  static bool IsSqliteExtra(wxString & str);
  static bool IsSqlString(wxString & str);
  static bool IsSqlNumber(wxString & str);
  static bool IsSqlFunction(wxString & str, char next_c);
  static bool IsSqlGeoFunction(wxString & str, char next_c);
  bool IsIgnoreEvent()
  {
    return IgnoreEvent;
  }
  void EventBrackets();
  bool CheckBrackets(int pos, bool reverse_direction, int *on, int *off);
  void EvidBrackets(int on, int off);
  void DoSqlSyntaxColor();
  void EnableAbortButton()
  {
    BtnSqlAbort->Enable(true);
  }
  void DisableAbortButton()
  {
    BtnSqlAbort->Enable(false);
  }
  wxTextCtrl *GetSqlCtrl()
  {
    return SqlCtrl;
  }
  MySqlHistory *GetHistory()
  {
    return &History;
  }
  void SetSql(wxString & sql, bool execute);
  void SetHistoryStates();
  void OnSize(wxSizeEvent & event);
  void OnSqlGo(wxCommandEvent & event);
  void OnSqlAbort(wxCommandEvent & event);
  void OnHistoryBack(wxCommandEvent & event);
  void OnHistoryForward(wxCommandEvent & event);
  void OnSqlSyntaxColor(wxCommandEvent & event);
  void AddToHistory(wxString & sql);
};

class MalformedGeom
{
//
// a malformed geometry item
//
private:
  sqlite3_int64 RowId;
  int Severity;
  wxString Error;
  wxString GeosMsg;
  bool CanFix;
  MalformedGeom *Next;
public:
    MalformedGeom(sqlite3_int64 rowid, int severity, bool canFix,
                  wxString & error);
    MalformedGeom(sqlite3_int64 rowid, int severity, bool canFix,
                  wxString & error, wxString & geosMsg);
   ~MalformedGeom()
  {;
  }
  sqlite3_int64 GetRowId()
  {
    return RowId;
  }
  int GetSeverity()
  {
    return Severity;
  }
  bool CanBeFixed()
  {
    return CanFix;
  }
  wxString & GetError()
  {
    return Error;
  }
  wxString & GetGeosMsg()
  {
    return GeosMsg;
  }
  void SetNext(MalformedGeom * next)
  {
    Next = next;
  }
  MalformedGeom *GetNext()
  {
    return Next;
  }
};

class MalformedGeomsList
{
//
// a list of malformed geometries
//
private:
  MalformedGeom * First;
  MalformedGeom *Last;
public:
    MalformedGeomsList()
  {
    First = NULL;
    Last = NULL;
  }
   ~MalformedGeomsList();
  void AddEntity(sqlite3_int64 rowid, int severity, bool canFix,
                 wxString & error);
  void AddEntity(sqlite3_int64 rowid, int severity, bool CanFix,
                 wxString & error, wxString & geosMsg);
  MalformedGeom *GetFirst()
  {
    return First;
  }
};

class SanitizeAllGeometriesDialog:public wxDialog
{
//
// a dialog supporting Sanitize All Geometries
//
private:
  MyFrame * MainFrame;
  wxString TmpPrefix;
public:
    SanitizeAllGeometriesDialog()
  {;
  }
  virtual ~ SanitizeAllGeometriesDialog()
  {;
  }
  bool Create(MyFrame * parent);
  void CreateControls();
  wxString & GetTmpPrefix()
  {
    return TmpPrefix;
  }
  void OnYes(wxCommandEvent & event);
  void OnNo(wxCommandEvent & event);
};

class WfsParams
{
// parameters for WFS download
private:
  class WfsDialog * Mother;
  wxGauge *ProgressCtrl;
  sqlite3 *sqlite;
  wxString url;
  wxString alt_describe;
  wxString layer_name;
  int swap_axes;
  wxString table;
  wxString primary_key;
  int spatial_index;
  int page_size;
  wxString extra;
  void (*callback) (int, void *);
  int ret;
  char *err_msg;
  int rows;
  int ProgressCount;
  int LastProgressCount;
public:
    WfsParams()
  {;
  }
   ~WfsParams()
  {;
  }
  void Initialize(WfsDialog * mother, sqlite3 * sqlite, wxString & url,
                  wxString & alt_describe, wxString & layer_name, int swap_axes,
                  wxString & table, wxString & primary_key, int spatial_index,
                  int page_size, wxString & extra, void (*callback) (int,
                                                                     void *))
  {
    Mother = mother;
    this->sqlite = sqlite;
    this->url = url;
    this->alt_describe = alt_describe;
    this->layer_name = layer_name;
    this->swap_axes = swap_axes;
    this->table = table;
    this->primary_key = primary_key;
    this->spatial_index = spatial_index;
    this->page_size = page_size;
    this->extra = extra;
    this->callback = callback;
    ProgressCount = 0;
    LastProgressCount = 0;
  }
  WfsDialog *GetMother()
  {
    return Mother;
  }
  sqlite3 *GetSqlite()
  {
    return sqlite;
  }
  wxString & GetUrl()
  {
    return url;
  }
  wxString & GetAltDescribeUri()
  {
    return alt_describe;
  }
  wxString & GetLayerName()
  {
    return layer_name;
  }
  int GetSwapAxes()
  {
    return swap_axes;
  }
  wxString & GetTable()
  {
    return table;
  }
  wxString & GetPrimaryKey()
  {
    return primary_key;
  }
  int GetSpatialIndex()
  {
    return spatial_index;
  }
  int GetPageSize()
  {
    return page_size;
  }
  wxString & GetExtra()
  {
    return extra;
  }
  void (*GetCallback()) (int, void *)
  {
    return callback;
  }
  void SetRet(int ret)
  {
    this->ret = ret;
  }
  int GetRet()
  {
    return ret;
  }
  void SetErrMsg(char *err_msg)
  {
    this->err_msg = err_msg;
  }
  char *GetErrMsg()
  {
    return err_msg;
  }
  void SetRows(int rows)
  {
    this->rows = rows;
  }
  int GetRows()
  {
    return rows;
  }
  int *GetProgressCountPtr()
  {
    return &ProgressCount;
  }
  void SetLastProgressCount(int last)
  {
    LastProgressCount = 0;
  }
  int GetProgressCount()
  {
    return ProgressCount;
  }
  int GetLastProgressCount()
  {
    return LastProgressCount;
  }
};

class WfsKey
{
// a WFS Keyword
private:
  wxString Keyword;
  WfsKey *Next;
public:
    WfsKey(wxString key)
  {
    Keyword = key;
    Next = NULL;
  }
   ~WfsKey()
  {;
  }
  wxString & GetKeyword()
  {
    return Keyword;
  }
  void SetNext(WfsKey * next)
  {
    Next = next;
  }
  WfsKey *GetNext()
  {
    return Next;
  }
};

class WfsKeywords
{
// an ancillary class storing the WFS Keywords dictionary
private:
  WfsKey * First;
  WfsKey *Last;
  WfsKey **SortedArray;
  int MaxSorted;
public:
    WfsKeywords()
  {
    First = NULL;
    Last = NULL;
    SortedArray = NULL;
    MaxSorted = 0;
  }
   ~WfsKeywords();
  void Add(const char *key);
  void Sort();
  int GetMaxSorted()
  {
    return MaxSorted;
  }
  WfsKey *GetKey(int index);
};

class WfsDialog:public wxDialog
{
//
// a dialog supporting data import from a WFS datasource
//
private:
  wxTimer * ProgressTimer;
  MyFrame *MainFrame;
  WfsParams Params;
  gaiaWFScatalogPtr Catalog;
  WfsKeywords *Keywords;
  wxGrid *WfsView;
  wxGauge *Progress;
  int CurrentEvtRow;
  int CurrentEvtColumn;
  gaiaWFSitemPtr FindLayerByName(wxString & name);
  void SelectLayer();
public:
    WfsDialog()
  {
    Catalog = NULL;
    WfsView = NULL;
    Keywords = NULL;
    ProgressTimer = NULL;
  }
  virtual ~ WfsDialog()
  {
    if (ProgressTimer)
      {
        ProgressTimer->Stop();
        delete ProgressTimer;
      }
    if (Catalog != NULL)
      destroy_wfs_catalog(Catalog);
    if (Keywords != NULL)
      delete Keywords;
  }
  void ResetProgress();
  void ProgressWait();
  void ProgressUpdate(int rows);
  bool Create(MyFrame * parent);
  void CreateControls();
  void OnPagingChanged(wxCommandEvent & event);
  void OnLeftClick(wxGridEvent & event);
  void OnRightClick(wxGridEvent & event);
  void OnKeyFilter(wxCommandEvent & event);
  void OnKeyReset(wxCommandEvent & event);
  void OnCatalog(wxCommandEvent & event);
  void OnReset(wxCommandEvent & event);
  void OnLoadFromWfs(wxCommandEvent & event);
  void OnQuit(wxCommandEvent & event);
  void OnCmdCopy(wxCommandEvent & event);
  void OnCmdSelectLayer(wxCommandEvent & event);
  void OnThreadFinished(wxCommandEvent & event);
  void OnRefreshTimer(wxTimerEvent & event);
};

class CheckGeometryDialog:public wxDialog
{
//
// a dialog supporting Check Geometry Column
//
private:
  MyFrame * MainFrame;
  wxString Table;
  wxString Geometry;
public:
    CheckGeometryDialog()
  {;
  }
  virtual ~ CheckGeometryDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & table, wxString & geom);
  void CreateControls();
  void OnYes(wxCommandEvent & event);
  void OnNo(wxCommandEvent & event);
};

class SanitizeGeometryDialog:public wxDialog
{
//
// a dialog supporting Sanitize Geometry Column
//
private:
  MyFrame * MainFrame;
  wxString TmpPrefix;
  wxString Table;
  wxString Geometry;
public:
    SanitizeGeometryDialog()
  {;
  }
  virtual ~ SanitizeGeometryDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & table, wxString & geom);
  void CreateControls();
  wxString & GetTmpPrefix()
  {
    return TmpPrefix;
  }
  void OnYes(wxCommandEvent & event);
  void OnNo(wxCommandEvent & event);
};

class MalformedGeomsDialog:public wxDialog
{
//
// a dialog displaying a Malformed Geometries list
//
private:
  MyFrame * MainFrame;
  wxString Table;
  wxString Geometry;
  wxGrid *GridCtrl;
  MalformedGeomsList *List;
  int CurrentEvtRow;
  int CurrentEvtColumn;
public:
    MalformedGeomsDialog()
  {
    List = NULL;
  }
  MalformedGeomsDialog(MyFrame * parent, wxString & table, wxString & column,
                       MalformedGeomsList * list);
  bool Create(MyFrame * parent, wxString & table, wxString & column,
              MalformedGeomsList * list);
  virtual ~ MalformedGeomsDialog()
  {
    if (List)
      delete List;
  }
  void CreateControls();
  void OnClose(wxCommandEvent & event);
  void OnRepair(wxCommandEvent & event);
  void OnRightClick(wxGridEvent & event);
  void OnCmdClearSelection(wxCommandEvent & event);
  void OnCmdSelectAll(wxCommandEvent & event);
  void OnCmdSelectRow(wxCommandEvent & event);
  void OnCmdSelectColumn(wxCommandEvent & event);
  void OnCmdCopy(wxCommandEvent & event);
  void OnCmdBlob(wxCommandEvent & event);
};

class DbStatusDialog:public wxDialog
{
//
// a dialog displaying DB Status infos
//
private:
  MyFrame * MainFrame;
  wxGrid *GridCtrl;
  wxStaticBitmap *Graph;
  int CurrentEvtRow;
  int CurrentEvtColumn;
  int *DynamicIds;
  int *DynamicModes;
  wxTimer *RefreshTimer;
  enum StatusModes
  {
    ModeNone,
    ModeStatusBoth,
    ModeStatusFirst,
    ModeStatusSecond,
    ModeStatusBothBytes,
    ModeStatusFirstBytes,
    ModeStatusSecondBytes,
    ModeDbStatusBoth,
    ModeDbStatusFirst,
    ModeDbStatusSecond,
    ModeDbStatusBothBytes,
    ModeDbStatusFirstBytes,
    ModeDbStatusSecondBytes
  };
public:
    DbStatusDialog();
  bool Create(MyFrame * parent);
    virtual ~ DbStatusDialog();
  void CreateControls();
  void OnClose(wxCommandEvent & event);
  void OnReset(wxCommandEvent & event);
  void OnRightClick(wxGridEvent & event);
  void OnCmdClearSelection(wxCommandEvent & event);
  void OnCmdSelectAll(wxCommandEvent & event);
  void OnCmdSelectRow(wxCommandEvent & event);
  void OnCmdSelectColumn(wxCommandEvent & event);
  void OnCmdCopy(wxCommandEvent & event);
  void OnRefreshTimer(wxTimerEvent & event);
};

class MyFrame:public wxFrame
{
//
// the main GUI frame
//
private:
  wxString AutoFDOmsg;
  bool SpatiaLiteMetadata;
  wxAuiManager Manager;         // the GUI manager
  wxString ConfigLayout;        // PERSISTENCY - the layout configuration
  int ConfigPaneX;              // PERSISTENCY - the main pane screen origin X
  int ConfigPaneY;              // PERSISTENCY - the main pane screen origin Y
  int ConfigPaneWidth;          // PERSISTENCY - the main pane screen width
  int ConfigPaneHeight;         // PERSISTENCY - the main pane screen height
  wxString ConfigDbPath;        // PERSISTENCY - the last opend DB path
  wxString ConfigDir;           // PERSISTENCY -  the last used directory
  MyTableTree *TableTree;       // the tables tree list
  MyQueryView *QueryView;       // the QueryResult panel
  MyResultSetView *RsView;      // the QueryResult panel
  bool HelpPane;                // is the HELP pane already opened ?
  sqlite3 *SqliteHandle;        // handle for SQLite DB
  wxString SqlitePath;          // path of SQLite DB
  void *InternalCache;          // pointer to the InternalCache supporting the DB connection
  wxString ExternalSqlitePath;  // path of external SQLite DB [LOAD/SAVE MEMORY database]
  bool MemoryDatabase;          // TRUE if we are currently working on the MEMORY database
  wxString LastDirectory;       // path of directory used  
  int CharsetsLen;              // # charsets defined
  wxString *Charsets;           // table of charsets [code only]
  wxString *CharsetsNames;      // table of charsets [with description]
  wxString LocaleCharset;       // locale charset
  wxString DefaultCharset;      // default charset
  bool AskCharset;              // switch to set default charset for every output
  int TablesLen;                // # tables defined
  wxString *TableNames;         // array of tables
  wxBitmap *BtnCreateNew;       // button icon for DB CREATE&CONNECT
  wxBitmap *BtnConnect;         // button icon for DB CONNECT
  wxBitmap *BtnDisconnect;      // button icon for DB DISCONNECT
  wxBitmap *BtnMemDbLoad;       // button icon for MEMORY DB LOAD
  wxBitmap *BtnMemDbNew;        // button icon for MEMORY DB NEW
  wxBitmap *BtnMemDbClock;      // button icon for MEMORY DB CLOCK
  wxBitmap *BtnMemDbSave;       // button icon for MEMORY DB SAVE
  wxBitmap *BtnVacuum;          // button icon for DB VACUUM
  wxBitmap *BtnSqlScript;       // button icon for Execute SQL SCRIPT
  wxBitmap *BtnQueryComposer;   // button icon for Query/View Composer
  wxBitmap *BtnCharset;         // button icon for Default CHARSET
  wxBitmap *BtnLoadShp;         // button icon for LOAD SHP
  wxBitmap *BtnLoadTxt;         // button icon for LOAD TXT/CSV
  wxBitmap *BtnLoadDbf;         // button icon for LOAD DBF
  wxBitmap *BtnLoadXL;          // button icon for LOAD_XL
  wxBitmap *BtnVirtualShp;      // button icon for VIRTUAL SHP
  wxBitmap *BtnVirtualTxt;      // button icon for VIRTUAL TXT/CSV
  wxBitmap *BtnVirtualDbf;      // button icon for VIRTUAL DBF
  wxBitmap *BtnVirtualXL;       // button icon for VIRTUAL XL
  wxBitmap *BtnNetwork;         // button icon for BUILD NETWORK
  wxBitmap *BtnExif;            // button icon for EXIF LOAD
  wxBitmap *BtnLoadXml;         // button icon for XML LOAD
  wxBitmap *BtnSrids;           // button icon for SEARCH SRIDs
  wxBitmap *BtnHelp;            // button icon for HELP
  wxBitmap *BtnAbout;           // button icon for ABOUT
  wxBitmap *BtnExit;            // button icon for EXIT
  wxBitmap *BtnAttach;          // button icon for ATTACH
  wxBitmap *BtnSqlLog;          // button icon for SQL LOG
  wxBitmap *BtnDbStatus;        // button icon for DB STATUS
  wxBitmap *BtnCheckGeom;       // button icon for CheckGeom
  wxBitmap *BtnSaneGeom;        // button icon for SaneGeom
  wxBitmap *BtnWFS;             // button icon for WFS
  wxBitmap *BtnDXF;             // button icon for DXF
// AutoSave timer
  int AutoSaveInterval;
  int LastTotalChanges;
  wxTimer *TimerAutoSave;
  gaiaGeomCollPtr GeomFromPoint(gaiaPointPtr pt, int srid);
  gaiaGeomCollPtr GeomFromLinestring(gaiaLinestringPtr ln, int srid);
  gaiaGeomCollPtr GeomFromPolygon(gaiaPolygonPtr pg, int srid);
  sqlite3_int64 LastSqlLogID;
  bool SqlLogEnabled;
  bool GetLwGeomVersion(char *buf);
  bool GetLibXml2Version(char *buf);
public:
    MyFrame(const wxString & title, const wxPoint & pos, const wxSize & size);
    virtual ~ MyFrame();

  void UpdateStatusBar();
  bool IsConnected()
  {
    if (SqliteHandle)
      return true;
    else
      return false;
  }

  static void DoubleQuotedSql(char *str);
  static void CleanSqlString(char *sql);

  void InsertIntoLog(wxString & sql);
  void UpdateLog(void);
  void UpdateLog(wxString & error_msg);
  void UpdateAbortedLog(void);
  void EnableSqlLog();

  bool DoAttachDatabase(wxString & path);
  void GetNextAttachedSymbol(wxString & symbol);

  void CleanTxtTab(char *str);
  void CleanCsv(char *str);
  void CleanHtml(char *str);
  void DifQuote(char *str);
  void SylkQuote(char *str);
  void DecimalNumber(double num, char *str, char decimal_point);
  bool TestDateValue(char *date);
  bool TestDateTimeValue(char *datetime);
  bool TestTimeValue(char *time);
  int ComputeSpreadsheetDate(int yy, int mm, int dd);
  double ComputeSpreadsheetTime(int hh, int mm, int ss);
  int GetDateValue(char *date);
  double GetDateTimeValue(char *datetime);
  double GetTimeValue(char *time);
  wxString & GetSqlitePath()
  {
    return SqlitePath;
  }
  wxString & GetExternalSqlitePath()
  {
    return ExternalSqlitePath;
  }
  void SetExternalSqlitePath(wxString & path)
  {
    ExternalSqlitePath = path;
  }
  sqlite3 *GetSqlite()
  {
    return SqliteHandle;
  }
  void *GetInternalCache()
  {
    return InternalCache;
  }
  void CloseHelpPane()
  {
    HelpPane = false;
  }
  void OpenHelpPane()
  {
    HelpPane = true;
  }

  bool OpenDB();
  bool CreateDB();
  void CloseDB();
  void InitializeSpatialMetadata();
  void AutoFDOStart();
  void AutoFDOStop();
  void InitTableTree();
  void LoadHistory();
  bool HasHistory();
  void ListAttached();
  void InitTableTree(wxString & dbAlias, wxString & path);
  void ClearTableTree();
  int GetMetaDataType();
  bool HasViewsMetadata();
  bool HasVirtsMetadata();
  bool HasViewsMetadata(wxString & dbAlias);
  bool HasVirtsMetadata(wxString & dbAlias);
  void GetTableColumns(wxString & tableName, MyTableInfo * list);
  void GetTableIndices(wxString & tableName, MyTableInfo * list);
  void GetTableTriggers(wxString & tableName, MyTableInfo * list);
  void GetViewColumns(wxString & viewName, MyViewInfo * list);
  void GetViewTriggers(wxString & viewName, MyViewInfo * list);
  void GetIndexFields(wxString & indexName, wxTreeItemId & node);
  void GetPrimaryKeyFields(wxString & indexName, wxTreeItemId & node);
  void GetForeignKeys(wxString & tableName, wxTreeItemId & node);
  void GetTableColumns(wxString & dbAlias, wxString & tableName,
                       MyTableInfo * list);
  void GetTableIndices(wxString & dbAlias, wxString & tableName,
                       MyTableInfo * list);
  void GetTableTriggers(wxString & dbAlias, wxString & tableName,
                        MyTableInfo * list);
  void GetViewColumns(wxString & dbAlias, wxString & viewName,
                      MyViewInfo * list);
  void GetViewTriggers(wxString & dbAlias, wxString & viewName,
                       MyViewInfo * list);
  void GetIndexFields(wxString & dbAlias, wxString & indexName,
                      wxTreeItemId & node);
  void GetPrimaryKeyFields(wxString & dbAlias, wxString & indexName,
                           wxTreeItemId & node);
  void GetForeignKeys(wxString & dbAlias, wxString & tableName,
                      wxTreeItemId & node);
  bool ExistsTopologyMaster();
  bool ExistsTopologyMaster(wxString & dbAlias);
  void GetTopologyColumns(wxString * list);
  void GetTopologyColumns(wxString & dbAlias, wxString * list);
  void CheckIfExists(const char *name, bool * table, bool * view);
  void CheckIfExists(wxString & dbAlias, const char *name, bool * table,
                     bool * view);
  void ElementaryGeoms(wxString & inTable, wxString & geometry,
                       wxString & outTable, wxString & pKey, wxString & multiID,
                       wxString & type, int *srid, wxString & coordDims,
                       bool * spIdx);
  bool DoElementaryGeometries(wxString & inTable, wxString & geometry,
                              wxString & outTable, wxString & pKey,
                              wxString & multiID, wxString & type, int srid,
                              wxString & coordDims, bool spIdx);
  void EditTable(wxString & sql, int *primaryKeys, int *blobCols,
                 wxString & table)
  {
    RsView->EditTable(sql, primaryKeys, blobCols, table);
  }
  bool IsSpatialIndex(wxString & tableName);
  bool IsSpatialIndex(wxString & dbAlias, wxString & tableName);
  void SetSql(wxString & sql, bool execute)
  {
    QueryView->SetSql(sql, execute);
  }
  bool ExecuteSql(const char *sql, int rowNo);
  void Rollback();
  bool TableAlreadyExists(wxString & name);
  bool SridNotExists(int srid);
  bool CheckMetadata();
  bool CheckMetadata(wxString & dbAlias);
  void SaveConfig();
  void LoadConfig(wxString & externalPath);
  wxString *GetCharsets()
  {
    return Charsets;
  }
  wxString *GetCharsetsNames()
  {
    return CharsetsNames;
  }
  int GetCharsetsLen()
  {
    return CharsetsLen;
  }
  gaiaDbfFieldPtr GetDbfField(gaiaDbfListPtr list, char *name);
  void OutputPrjFile(wxString & path, int srid);
  void LoadText(wxString & path, wxString & table, wxString & charset,
                bool first_titles, const char decimal_separator,
                const char separator, const char text_separator);
  void DumpTxtTab(wxString & path, wxString & table, wxString & charset);
  void DumpCsv(wxString & path, wxString & table, wxString & charset);
  void DumpHtml(wxString & path, wxString & table, wxString & dbPath,
                wxString & charset);
  void DumpDif(wxString & path, wxString & table, wxString & charset,
               char decimal_point, bool date_time);
  void DumpSylk(wxString & path, wxString & table, wxString & charset,
                bool date_time);
  void DumpKml(wxString & path, wxString & table, wxString & column,
               int precision, wxString & name, bool isNameConst,
               wxString & desc, bool isDescConst);
  void ExportResultSetAsTxtTab(wxString & path, wxString & sql,
                               wxString & charset);
  void ExportResultSetAsCsv(wxString & path, wxString & sql,
                            wxString & charset);
  void ExportResultSetAsHtml(wxString & path, wxString & sql, wxString & dbPath,
                             wxString & charset);
  bool ExportHtmlColorSqlSyntax(FILE * out, wxString & sql, char *out_cs);
  void ExportResultSetAsShp(wxString & path, wxString & sql,
                            wxString & charset);
  void ExportResultSetAsDif(wxString & path, wxString & sql,
                            wxString & charset, char decimal_point,
                            bool date_time);
  void ExportResultSetAsSylk(wxString & path, wxString & sql,
                             wxString & charset, bool date_time);
  void ExportResultSetAsDbf(wxString & path, wxString & sql,
                            wxString & charset);
  void GetHelp(wxString & html);
  wxString *GetColumnNames(wxString & table, int *columns);
  void SetLastDirectory(wxString & path)
  {
    LastDirectory = path;
  }
  wxString & GetLastDirectory()
  {
    return LastDirectory;
  }
  wxString & GetLocaleCharset()
  {
    return LocaleCharset;
  }
  wxString & GetDefaultCharset()
  {
    return DefaultCharset;
  }
  wxString & GetCharsetName(wxString & charset);
  int GetCharsetIndex(wxString & charset);
  bool IsSetAskCharset()
  {
    return AskCharset;
  }
  char *ReadSqlLine(FILE * fl, int *len, int *eof);
  MyQueryView *GetQueryView()
  {
    return QueryView;
  }
  MyResultSetView *GetRsView()
  {
    return RsView;
  }
  wxString *GetTables(int *cnt);
  void BuildNetwork(wxString & table, wxString & from, wxString & to,
                    wxString & geometry, wxString & name, bool cost_length,
                    wxString & cost, bool bidirectional, bool one_way,
                    wxString & one_way_from_to, wxString & one_way_to_from,
                    bool aStarSupported);
  bool CreateNetwork(class Network * net, wxString & table, wxString & from,
                     wxString & to, wxString & geometry, wxString & name,
                     bool aStarSupported, double aStarCoeff);
  void ImportExifPhotos(wxString & path, bool folder, bool metadata,
                        bool gps_only);
  void ImportXmlDocuments(wxString & path, bool folder, wxString & suffix,
                          wxString & table, wxString & pkName,
                          wxString & xmlColumn, wxString & inPathColumn,
                          wxString & schemaColumn, wxString & parseErrColumn,
                          wxString & validateErrColumn, int compressed,
                          const char *schemaURI, bool isInternaleSchemaUri);
  void ImportDXFfiles(wxString & path, bool folder, wxString & prefix,
                      wxString & layer, int srid, bool force2d, bool force3d,
                      bool mixed, bool linked, bool unlinked, bool append);
  void OutputNetNode(unsigned char *auxbuf, int *size, int ind, bool node_code,
                     int max_node_length, class NetNode * pN, int endian_arch,
                     bool aStarSupported);
  bool CheckExifTables();
  int ExifLoadDir(wxString & path, bool gps_only, bool metadata);
  int ExifLoadFile(wxString & path, bool gps_only, bool metadata);
  bool UpdateExifTables(unsigned char *blob, int sz,
                        gaiaExifTagListPtr tag_list, bool metadata,
                        wxString & path);
  bool IsExifGps(gaiaExifTagListPtr tag_list);
  int DxfLoadDir(wxString & path, wxString & prefix, wxString & layer, int srid,
                 bool force2d, bool force3d, bool mixed, bool linked,
                 bool unlinked, bool append, int *failed);
  int DxfLoadFile(wxString & path, wxString & prefix, wxString & layer,
                  int srid, bool force2d, bool force3d, bool mixed, bool linked,
                  bool unlinked, bool append, int *failed);
  bool CheckOrCreateXmlTable(wxString & table, wxString & pkName,
                             wxString & xmlColumn, wxString & inPathColumn,
                             wxString & schemaUriColumn,
                             wxString & parseErrColumn,
                             wxString & validateErrColumn);
  bool IsValidSuffix(const char *fileName, wxString & suffix);
  int XmlDocumentLoadDir(wxString & path, wxString & suffix, int compressed,
                         const char *schemaURI, bool isInternalSchemaUri,
                         wxString & inPathColumn, wxString & schemaUriColumn,
                         wxString & parseErrColumn,
                         wxString & validateErrColumn, sqlite3_stmt * stmt,
                         int *failed);
  int XmlDocumentLoadFile(wxString & path, int compressed,
                          const char *schemaURI, bool isInternalSchemaUri,
                          wxString & inPathColumn, wxString & parseErrColumn,
                          wxString & validateErrColumn,
                          wxString & schemaUriColumn, sqlite3_stmt * stmt,
                          int *failed);
  bool InsertIntoXmlTable(sqlite3_stmt * stmt, char *blob, int sz,
                          wxString & inPathColumn, wxString & path,
                          wxString & schemaUriColumn, const char *schemaUri,
                          wxString & parseErrColumn, const char *parseError,
                          wxString & validateErrColumn,
                          const char *validateError);
  void GetMake(gaiaExifTagListPtr tag_list, wxString & str, bool * ok);
  void GetModel(gaiaExifTagListPtr tag_list, wxString & str, bool * ok);
  void GetGpsTimestamp(gaiaExifTagListPtr tag_list, wxString & str, bool * ok);
  void GetDate(gaiaExifTagListPtr tag_list, wxString & str, bool * ok);
  double GetGpsDirection(gaiaExifTagListPtr tag_list, bool * ok);
  void GetGpsSatellites(gaiaExifTagListPtr tag_list, wxString & str, bool * ok);
  void GetGpsCoords(gaiaExifTagListPtr tag_list, double *longitude,
                    double *latitude, bool * ok);
  sqlite3_int64 GetPixelX(gaiaExifTagListPtr tag_list, bool * ok);
  sqlite3_int64 GetPixelY(gaiaExifTagListPtr tag_list, bool * ok);

  bool MemoryDbSave();
  void LastDitchMemoryDbSave();
  void QueryViewComposer();
  MalformedGeomsList *FindMalformedGeoms(wxString & table, wxString & geom,
                                         bool allowRepair);
  void PreRepairPolygons(wxString & table, wxString & geom, int *count);
  void RepairPolygons(wxString & table, wxString & geom, int *count);

  bool IsPrimaryKey(wxString & table, wxString & column);
  void DbPagesCount(int *total, int *frees);


  int TestDotCommand(const char *stmt);
  bool IsDotCommandLoadShp(const char *stmt, char *path, char *table,
                           char *charset, char *column, int *srid,
                           bool * coerce2D, bool * compressed);
  bool IsDotCommandLoadDbf(const char *stmt, char *path, char *table,
                           char *charset);
  bool IsDotCommandLoadXL(const char *stmt, char *path, char *table,
                          int *worksheetIndex, int *firstTitle);
  bool IsDotCommandDumpShp(const char *stmt, char *table,
                           char *column, char *path, char *charset, char *type);
  bool IsViewGeometry(wxString & table, wxString & column);

  void OnQuit(wxCommandEvent & event);
  void OnAbout(wxCommandEvent & event);
  void OnConnect(wxCommandEvent & event);
  void OnCreateNew(wxCommandEvent & event);
  void OnDisconnect(wxCommandEvent & event);
  void OnMemoryDbLoad(wxCommandEvent & event);
  void OnMemoryDbNew(wxCommandEvent & event);
  void OnMemoryDbClock(wxCommandEvent & event);
  void OnMemoryDbSave(wxCommandEvent & event);
  void OnVacuum(wxCommandEvent & event);
  void OnSqlScript(wxCommandEvent & event);
  void OnQueryViewComposer(wxCommandEvent & event);
  void OnCharset(wxCommandEvent & event);
  void OnLoadShp(wxCommandEvent & event);
  void OnLoadTxt(wxCommandEvent & event);
  void OnLoadDbf(wxCommandEvent & event);
  void OnLoadXL(wxCommandEvent & event);
  void OnVirtualShp(wxCommandEvent & event);
  void OnVirtualTxt(wxCommandEvent & event);
  void OnVirtualDbf(wxCommandEvent & event);
  void OnVirtualXL(wxCommandEvent & event);
  void OnNetwork(wxCommandEvent & event);
  void OnImportExifPhotos(wxCommandEvent & event);
  void OnImportXmlDocuments(wxCommandEvent & event);
  void OnImportWFS(wxCommandEvent & event);
  void OnImportDXF(wxCommandEvent & event);
  void OnSrids(wxCommandEvent & event);
  void OnAttachDatabase(wxCommandEvent & event);
  void OnSqlLog(wxCommandEvent & event);
  void OnDbStatus(wxCommandEvent & event);
  void OnCheckGeometries(wxCommandEvent & event);
  void OnSanitizeGeometries(wxCommandEvent & event);
  void OnHelp(wxCommandEvent & event);
  void OnMouseMove(wxMouseEvent & event);
  void OnTimerAutoSave(wxTimerEvent & event);
};

class HelpDialog:public wxDialog
{
//
// the help dialog
//
private:
  MyFrame * MainFrame;
public:
  HelpDialog()
  {
    MainFrame = NULL;
  }
  HelpDialog(MyFrame * parent)
  {
    Create(parent);
  }
  bool Create(MyFrame * parent);
  virtual ~ HelpDialog()
  {;
  }
  void CreateControls();
  void OnClose(wxCloseEvent & event);
  void OnSize(wxSizeEvent & event);
};

class SearchSridDialog:public wxDialog
{
//
// a dialog preparing a Search SRID by name
//
private:
  MyFrame * MainFrame;
  wxString String;              // search string [name]
  int Srid;                     // search id [srid]
  bool SearchBySrid;
public:
    SearchSridDialog()
  {;
  }
  SearchSridDialog(MyFrame * parent);
  bool Create(MyFrame * parent);
  virtual ~ SearchSridDialog()
  {;
  }
  void CreateControls();
  wxString & GetString()
  {
    return String;
  }
  int GetSrid()
  {
    return Srid;
  }
  bool IsSearchBySrid()
  {
    return SearchBySrid;
  }
  void OnOk(wxCommandEvent & event);
  void OnSearchBySrid(wxCommandEvent & event);
};

class SetSridDialog:public wxDialog
{
//
// a dialog preparing a SET SRID
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Column;              // the column's name to be recovered
  int OldSrid;                  // SRID to substitute
  int Srid;                     // required SRID
public:
    SetSridDialog()
  {;
  }
  SetSridDialog(MyFrame * parent, wxString & table, wxString & column);
  bool Create(MyFrame * parent, wxString & table, wxString & column);
  virtual ~ SetSridDialog()
  {;
  }
  void CreateControls();
  int GetOldSrid()
  {
    return OldSrid;
  }
  int GetSrid()
  {
    return Srid;
  }
  void OnOk(wxCommandEvent & event);
};

class RecoverDialog:public wxDialog
{
//
// a dialog preparing a RECOVER GEOMETRY
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Column;              // the column's name to be recovered
  int Srid;                     // required SRID
  wxString Type;                // required Geometry Type
  wxString Dimension;           // required CoordDimension
public:
    RecoverDialog()
  {;
  }
  RecoverDialog(MyFrame * parent, wxString & table, wxString & column);
  bool Create(MyFrame * parent, wxString & table, wxString & column);
  virtual ~ RecoverDialog()
  {;
  }
  void CreateControls();
  wxString & GetType()
  {
    return Type;
  }
  wxString & GetDimension()
  {
    return Dimension;
  }
  int GetSrid()
  {
    return Srid;
  }
  void OnOk(wxCommandEvent & event);
};

class ElementaryGeomsDialog:public wxDialog
{
//
// a dialog asking ElementaryGeoms args
//
private:
  MyFrame * MainFrame;
  wxString InTable;
  wxString Geometry;
  wxString OutTable;
  wxString PrimaryKey;
  wxString MultiID;
  wxString Type;
  int Srid;
  wxString CoordDims;
  bool SpatialIndex;
public:
    ElementaryGeomsDialog()
  {;
  }
  ElementaryGeomsDialog(MyFrame * parent, wxString & table, wxString & column);
  bool Create(MyFrame * parent, wxString & table, wxString & column);
  virtual ~ ElementaryGeomsDialog()
  {;
  }
  wxString & GetOutTable()
  {
    return OutTable;
  }
  wxString & GetPrimaryKey()
  {
    return PrimaryKey;
  }
  wxString & GetMultiID()
  {
    return MultiID;
  }
  wxString & GetType()
  {
    return Type;
  }
  int GetSrid()
  {
    return Srid;
  }
  wxString & GetCoordDims()
  {
    return CoordDims;
  }
  bool IsSpatialIndex()
  {
    return SpatialIndex;
  }
  void CreateControls();
  void OnOk(wxCommandEvent & event);
};

class ColumnStatsDialog:public wxDialog
{
//
// a dialog showing Column Stats
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Column;              // the column's name 
  int NullValues;
  int TextValues;
  int IntegerValues;
  int RealValues;
  int BlobValues;
  double Min;
  double Max;
  double Avg;
  double StdDevPop;
  double StdDevSamp;
  double VarPop;
  double VarSamp;
  int DistinctValues;
  void CleanDecimals(char *number);
public:
    ColumnStatsDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & table, wxString & column,
              int null_count, int text_count, int integer_count, int real_count,
              int blob_count, double min, double max, double avg,
              double stddev_pop, double stddev_samp, double var_pop,
              double var_samp, int distinct_values);
  virtual ~ ColumnStatsDialog()
  {;
  }
  void CreateControls();
  void OnShowChart(wxCommandEvent & event);
  void OnExit(wxCommandEvent & event);
};

class MyChartIntervalClass
{
// a Chart interval class
private:
  double Min;
  double Max;
  int Count;
public:
    MyChartIntervalClass()
  {;
  }
   ~MyChartIntervalClass()
  {;
  }
  void Create(double min, double max)
  {
    Min = min;
    Max = max;
    Count = 0;
  }
  double GetMin()
  {
    return Min;
  }
  double GetMax()
  {
    return Max;
  }
  void Add()
  {
    Count++;
  }
  int GetCount()
  {
    return Count;
  }
};

class MyChartUniqueClass
{
// a Chart unique-value class
private:
  wxString Value;
  int Count;
  MyChartUniqueClass *Next;
public:
    MyChartUniqueClass(wxString & value, int count)
  {
    Value = value;
    Count = count;
    Next = NULL;
  }
   ~MyChartUniqueClass()
  {;
  }
  wxString & GetValue()
  {
    return Value;
  }
  int GetCount()
  {
    return Count;
  }
  void SetNext(MyChartUniqueClass * p)
  {
    Next = p;
  }
  MyChartUniqueClass *GetNext()
  {
    return Next;
  }
};

class MyChartData
{
// a container storing Chart data classes
private:
  bool Initialized;
  MyChartIntervalClass *Array;
  double Min;
  double Max;
  int MaxFreq;
  int TotFreq;
  MyChartUniqueClass *First;
  MyChartUniqueClass *Last;
  int MaxClasses;
  int NumClasses;
  int OtherUniquesFreq;
  int OtherUniquesCount;
  bool Valid;
  bool ByIntervals;
public:
    MyChartData();
   ~MyChartData();
  bool Create(int max_classes);
  bool Create(double min, double max, int classes);
  void Add(wxString & value, int count);
  void Add(double value);
  MyChartUniqueClass *GetFirst()
  {
    return First;
  }
  int GetMaxFreq()
  {
    return MaxFreq;
  }
  int GetTotFreq()
  {
    return TotFreq;
  }
  int GetNumClasses()
  {
    return NumClasses;
  }
  int GetOtherUniquesFreq()
  {
    return OtherUniquesFreq;
  }
  int GetOtherUniquesCount()
  {
    return OtherUniquesCount;
  }
  MyChartIntervalClass *GetClass(int idx);
  void SetValid()
  {
    Valid = true;
  }
  bool IsValid()
  {
    return Valid;
  }
  void CleanData();
  bool Check(bool by_intervals, int classes);
};

class MyChartScaleLabel
{
// a Chart Scale label
private:
  wxString Label;
  double Position;
  MyChartScaleLabel *Next;
public:
    MyChartScaleLabel(wxString & label, double pos)
  {
    Label = label;
    Position = pos;
    Next = NULL;
  }
   ~MyChartScaleLabel()
  {;
  }
  wxString & GetLabel()
  {
    return Label;
  }
  double GetPosition()
  {
    return Position;
  }
  void SetNext(MyChartScaleLabel * p)
  {
    Next = p;
  }
  MyChartScaleLabel *GetNext()
  {
    return Next;
  }
};

class MyPieChartLabel
{
// a PieChart label
private:
  wxString Label;
  double X;
  double Y;
  MyPieChartLabel *Next;
public:
    MyPieChartLabel(wxString & label, double x, double y)
  {
    Label = label;
    X = x;
    Y = y;
    Next = NULL;
  }
   ~MyPieChartLabel()
  {;
  }
  wxString & GetLabel()
  {
    return Label;
  }
  double GetX()
  {
    return X;
  }
  double GetY()
  {
    return Y;
  }
  void SetNext(MyPieChartLabel * p)
  {
    Next = p;
  }
  MyPieChartLabel *GetNext()
  {
    return Next;
  }
};

class MyChartScaleLabels
{
// a container storing Chart Scale labels
private:
  MyChartScaleLabel * First;
  MyChartScaleLabel *Last;
public:
    MyChartScaleLabels()
  {
    First = NULL;
    Last = NULL;
  }
   ~MyChartScaleLabels();
  void Initialize(double span, int max_freq);
  void Add(const char *label, double pos);
  MyChartScaleLabel *GetFirst()
  {
    return First;
  }
};

class MyPieChartLabels
{
// a container storing PieChart labels
private:
  MyPieChartLabel * First;
  MyPieChartLabel *Last;
  MyPieChartLabel **LeftLabels;
  int NumLeftLabels;
  MyPieChartLabel **RightLabels;
  int NumRightLabels;
public:
    MyPieChartLabels();
   ~MyPieChartLabels();
  void Add(const char *label, double x, double y);
  void Sort(double cx);
  int GetNumLeftLabels()
  {
    return NumLeftLabels;
  }
  MyPieChartLabel *GetLeftLabel(int idx);
  int GetNumRightLabels()
  {
    return NumRightLabels;
  }
  MyPieChartLabel *GetRightLabel(int idx);
};

class StatsChartDialog:public wxDialog
{
//
// a dialog generating a Stat Chart
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Column;              // the column's name 
  bool NumericData;
  double Min;
  double Max;
  wxRadioBox *TypeCtrl;
  wxRadioBox *SizeCtrl;
  wxRadioBox *ModeCtrl;
  wxSpinCtrl *ClassCtrl;
  wxStaticBitmap *ChartShow;
  bool Histogram;
  bool LineChart;
  bool PieChart;
  bool ByInterval;
  int Classes;
  MyChartData ChartData;
  wxString ExportPath;
  enum Targets
  {
    CHART_TARGET_IS_PREVIEW,
    CHART_TARGET_IS_COPY,
    CHART_TARGET_IS_PNG,
    CHART_TARGET_IS_SVG,
    CHART_TARGET_IS_PDF
  };
public:
    StatsChartDialog()
  {;
  }
  bool Create(ColumnStatsDialog * parent, MyFrame * granny, wxString & table,
              wxString & column, bool numeric, double min, double max);
  virtual ~ StatsChartDialog()
  {;
  }
  void CreateControls();
  void CleanDecimals(char *number);
  void ReloadData();
  void UpdatePreview();
  void PrepareDataByInterval(int classes);
  void PrepareDataByUniqueValue(int classes);
  void DoIntervalHistogram(int hsize, int vsize, int target, int font_size);
  void DoIntervalLineChart(int hsize, int vsize, int target, int font_size);
  void DoIntervalPieChart(int hsize, int vsize, int target, int font_size);
  void DoUniqueHistogram(int hsize, int vsize, int target, int font_size);
  void DoUniqueLineChart(int hsize, int vsize, int target, int font_size);
  void DoUniquePieChart(int hsize, int vsize, int target, int font_size);
  void OnChartTypeChanged(wxCommandEvent & event);
  void OnChartModeChanged(wxCommandEvent & event);
  void OnChartClassesChanged(wxCommandEvent & event);
  void OnChartCopy(wxCommandEvent & event);
  void OnChartPng(wxCommandEvent & event);
  void OnChartSvg(wxCommandEvent & event);
  void OnChartPdf(wxCommandEvent & event);
  void OnExit(wxCommandEvent & event);
};

class MapPreviewDialog:public wxDialog
{
//
// a dialog generating a Map Preview
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Column;              // the column's name 
  double MinX;
  double MinY;
  double MaxX;
  double MaxY;
  wxColour LineColor;
  wxColour FillColor;
  wxRadioBox *SizeCtrl;
  wxRadioBox *SymbolCtrl;
  wxRadioBox *FillCtrl;
  wxSpinCtrl *SymSizeCtrl;
  wxSpinCtrl *ThicknessCtrl;
  wxBitmapButton *LineColorCtrl;
  wxBitmapButton *FillColorCtrl;
  wxStaticBitmap *MapShow;
  wxString ExportPath;
  enum Targets
  {
    MAP_TARGET_IS_PREVIEW,
    MAP_TARGET_IS_COPY,
    MAP_TARGET_IS_PNG,
    MAP_TARGET_IS_SVG,
    MAP_TARGET_IS_PDF
  };
public:
    MapPreviewDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & table, wxString & column,
              double minx, double mixy, double maxx, double maxy);
  virtual ~ MapPreviewDialog()
  {;
  }
  void CreateControls();
  void UpdatePreview();
  void DoMap(int hsize, int vsize, int target);
  void GetButtonBitmap(wxColour & color, wxBitmap & bmp);
  void OnSizeChanged(wxCommandEvent & event);
  void OnSymbolTypeChanged(wxCommandEvent & event);
  void OnFillModeChanged(wxCommandEvent & event);
  void OnSymbolSizeChanged(wxCommandEvent & event);
  void OnLineThicknessChanged(wxCommandEvent & event);
  void OnLineColor(wxCommandEvent & event);
  void OnFillColor(wxCommandEvent & event);
  void OnMapCopy(wxCommandEvent & event);
  void OnMapPng(wxCommandEvent & event);
  void OnMapSvg(wxCommandEvent & event);
  void OnMapPdf(wxCommandEvent & event);
  void OnExit(wxCommandEvent & event);
};

class GisLayerAuthDialog:public wxDialog
{
//
// a dialog to set GIS Layer auths
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name 
  wxString Geometry;            // the geometry column's name 
  bool ReadOnly;
  bool Hidden;
public:
    GisLayerAuthDialog()
  {;
  }
  GisLayerAuthDialog(MyFrame * parent, wxString & table, wxString & geom,
                     bool rdOnly, bool hidden);
  bool Create(MyFrame * parent, wxString & table, wxString & geom, bool rdOnly,
              bool hidden);
  virtual ~ GisLayerAuthDialog()
  {;
  }
  void CreateControls();
  bool IsReadOnly()
  {
    return ReadOnly;
  }
  bool IsHidden()
  {
    return Hidden;
  }
  void OnReadOnlyChanged(wxCommandEvent & event);
  void OnHiddenChanged(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class VirtualShpDialog:public wxDialog
{
//
// a dialog preparing a CREATE VIRTUAL SHAPE
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the SHP base path
  wxString Table;               // the table name
  wxString Default;             // the default charset
  wxString Charset;             // the SHP charset
  int Srid;                     // the SRID
public:
    VirtualShpDialog()
  {;
  }
  VirtualShpDialog(MyFrame * parent, wxString & path, wxString & table,
                   wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & defCs);
  virtual ~ VirtualShpDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  int GetSrid()
  {
    return Srid;
  }
  void OnOk(wxCommandEvent & event);
};

class VirtualTxtDialog:public wxDialog
{
//
// a dialog preparing a CREATE VIRTUAL TEXT
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the CSV/TXT base path
  wxString Table;               // the table name
  wxString Default;             // the default charset
  wxString Charset;             // the CSV/TXT charset
  bool FirstLineTitles;         // TRUE if first line stores column titles
  char Separator;               // the character to be used as field separator
  char TextSeparator;           // the character to be used as text separator
  bool DecimalPointIsComma;     // TRUE if decimal separator is COMMA
public:
    VirtualTxtDialog()
  {;
  }
  VirtualTxtDialog(MyFrame * parent, wxString & path, wxString & table,
                   wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & defCs);
  virtual ~ VirtualTxtDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  bool IsFirstLineTitles()
  {
    return FirstLineTitles;
  }
  char GetSeparator()
  {
    return Separator;
  }
  char GetTextSeparator()
  {
    return TextSeparator;
  }
  bool IsDecimalPointComma()
  {
    return DecimalPointIsComma;
  }
  void OnSeparator(wxCommandEvent & event);
  void OnDecimalSeparator(wxCommandEvent & event);
  void OnQuote(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class VirtualDbfDialog:public wxDialog
{
//
// a dialog preparing a CREATE VIRTUAL DBF
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the DBF path
  wxString Table;               // the table name
  wxString Default;             // the default charset
  wxString Charset;             // the DBF charset
public:
    VirtualDbfDialog()
  {;
  }
  VirtualDbfDialog(MyFrame * parent, wxString & path, wxString & table,
                   wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & defCs);
  virtual ~ VirtualDbfDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  void OnOk(wxCommandEvent & event);
};

class LoadShpDialog:public wxDialog
{
//
// a dialog preparing a LOAD SHAPE
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the SHP base path
  wxString Table;               // the table's name to be created
  wxString Column;              // the column's name for Geometry
  wxString Default;             // the default charset
  wxString Charset;             // the SHP charset
  int Srid;                     // the SRID
  bool Coerce2D;                // coercing to 2D [x,y]
  bool Compressed;              // compressed geometries
  bool SpatialIndex;            // building the Spatial Index (or not)
  bool UserDefinedGType;        // mode: automatic / user defined Geometry Type
  wxString GeometryType;        // User Defined Geometry Type
  bool UserDefinedPKey;         // mode: automatic / user defined Primary Key
  wxString PKColumn;            // User Defined Primary Key 
  int PKCount;                  // # Primary Key Columns
  wxString *PKFields;           // array of Primary Key Columns
  wxString *PKFieldsEx;         // array of Primary Key Columns (full detail)
public:
    LoadShpDialog()
  {;
  }
  LoadShpDialog(MyFrame * parent, wxString & path, wxString & table, int srid,
                wxString & column, wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table, int srid,
              wxString & column, wxString & defCs);
  virtual ~ LoadShpDialog();
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetColumn()
  {
    return Column;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  int GetSrid()
  {
    return Srid;
  }
  bool ApplyCoertion2D()
  {
    return Coerce2D;
  }
  bool ApplyCompression()
  {
    return Compressed;
  }
  bool CreateSpatialIndex()
  {
    return SpatialIndex;
  }
  bool IsUserDefinedGType()
  {
    return UserDefinedGType;
  }
  wxString & GetGeometryType()
  {
    return GeometryType;
  }
  bool IsUserDefinedPKey()
  {
    return UserDefinedPKey;
  }
  wxString & GetPKColumn()
  {
    return PKColumn;
  }
  void LoadPKFields();
  void OnOk(wxCommandEvent & event);
  void OnUserGType(wxCommandEvent & event);
  void OnUserPKey(wxCommandEvent & event);
};

class DumpPostGISDialog:public wxDialog
{
//
// a dialog preparing a SQL DUMP for PostGIS
//
private:
  MyFrame * MainFrame;
  wxString SchemaName;          // the PostGIS target schema
  wxString TableName;           // the PostGIS table name
  bool Lowercase;               // column-names to lowercase
  bool CreateTable;             // creating (or not) the PostGIS table
  bool SpatialIndex;            // creating (or not) the PostGIS Spatial Index
public:
    DumpPostGISDialog()
  {;
  }
  DumpPostGISDialog(MyFrame * parent, wxString & table);
  bool Create(MyFrame * parent, wxString & table);
  virtual ~ DumpPostGISDialog()
  {;
  }
  void CreateControls();
  wxString & GetSchemaName()
  {
    return SchemaName;
  }
  wxString & GetTableName()
  {
    return TableName;
  }
  bool IsLowercase()
  {
    return Lowercase;
  }
  bool IsCreateTable()
  {
    return CreateTable;
  }
  bool IsSpatialIndex()
  {
    return SpatialIndex;
  }
  void OnLowercase(wxCommandEvent & event);
  void OnCreateTable(wxCommandEvent & event);
  void OnSpatialIndex(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class DumpShpDialog:public wxDialog
{
//
// a dialog preparing a DUMP SHAPE
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the SHP base path
  wxString Table;               // the table's name to be created
  wxString Column;              // the column's name for Geometry
  wxString Default;             // the default charset
  wxString Charset;             // the SHP charset
public:
    DumpShpDialog()
  {;
  }
  DumpShpDialog(MyFrame * parent, wxString & path, wxString & table,
                wxString & column, wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & column, wxString & defCs);
  virtual ~ DumpShpDialog()
  {;
  }
  void CreateControls();
  wxString & GetCharset()
  {
    return Charset;
  }
  void OnOk(wxCommandEvent & event);
};

class LoadXLDialog:public wxDialog
{
//
// a dialog preparing a LOAD XL
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the XLS path
  wxString Table;               // the table name
  wxString *Worksheets;         // Worksheet array
  int WorksheetCount;           // array items
  int WorksheetIndex;           // selected Worksheet Index
  bool FirstLineTitles;         // first line contains column names
  bool Invalid;
  void GetWorksheets();
public:
    LoadXLDialog()
  {;
  }
  LoadXLDialog(MyFrame * parent, wxString & path, wxString & table);
  bool Create(MyFrame * parent, wxString & path, wxString & table);
  virtual ~ LoadXLDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  int GetWorksheetIndex()
  {
    return WorksheetIndex;
  }
  bool IsFirstLineTitles()
  {
    return FirstLineTitles;
  }
  void OnOk(wxCommandEvent & event);
};

class VirtualXLDialog:public wxDialog
{
//
// a dialog preparing a CREATE VIRTUAL XL
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the XLS path
  wxString Table;               // the table name
  wxString *Worksheets;         // Worksheet array
  int WorksheetCount;           // array items
  int WorksheetIndex;           // selected Worksheet Index
  bool FirstLineTitles;         // first line contains column names
  bool Invalid;
  void GetWorksheets();
public:
    VirtualXLDialog()
  {;
  }
  VirtualXLDialog(MyFrame * parent, wxString & path, wxString & table);
  bool Create(MyFrame * parent, wxString & path, wxString & table);
  virtual ~ VirtualXLDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  int GetWorksheetIndex()
  {
    return WorksheetIndex;
  }
  bool IsFirstLineTitles()
  {
    return FirstLineTitles;
  }
  void OnOk(wxCommandEvent & event);
};

class LoadTxtDialog:public wxDialog
{
//
// a dialog preparing a LOAD TXT/CSV
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the CSV/TXT base path
  wxString Table;               // the table name
  wxString Default;             // the default charset
  wxString Charset;             // the CSV/TXT charset
  bool FirstLineTitles;         // TRUE if first line stores column titles
  char Separator;               // the character to be used as field separator
  char TextSeparator;           // the character to be used as text separator
  bool DecimalPointIsComma;     // TRUE if decimal separator is COMMA
public:
    LoadTxtDialog()
  {;
  }
  LoadTxtDialog(MyFrame * parent, wxString & path, wxString & table,
                wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & defCs);
  virtual ~ LoadTxtDialog()
  {;
  }
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  bool IsFirstLineTitles()
  {
    return FirstLineTitles;
  }
  char GetSeparator()
  {
    return Separator;
  }
  char GetTextSeparator()
  {
    return TextSeparator;
  }
  bool IsDecimalPointComma()
  {
    return DecimalPointIsComma;
  }
  void OnSeparator(wxCommandEvent & event);
  void OnDecimalSeparator(wxCommandEvent & event);
  void OnQuote(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class DumpTxtDialog:public wxDialog
{
//
// a dialog preparing a DUMP generic text
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the SHP base path
  wxString Default;             // the default charset
  wxString Charset;             // the target charset
public:
    DumpTxtDialog()
  {;
  }
  DumpTxtDialog(MyFrame * parent, wxString & path, wxString & target,
                wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & target,
              wxString & defCs);
  virtual ~ DumpTxtDialog()
  {;
  }
  void CreateControls();
  wxString & GetCharset()
  {
    return Charset;
  }
  void OnOk(wxCommandEvent & event);
};

class LoadDbfDialog:public wxDialog
{
//
// a dialog preparing a LOAD DBF
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the DBF base path
  wxString Table;               // the table name
  wxString Default;             // the default charset
  wxString Charset;             // the DBF charset
  bool UserDefinedPKey;         // mode: automatic / user defined Primary Key
  wxString PKColumn;            // User Defined Primary Key 
  int PKCount;                  // # Primary Key Columns
  wxString *PKFields;           // array of Primary Key Columns
  wxString *PKFieldsEx;         // array of Primary Key Columns (full detail)
public:
    LoadDbfDialog()
  {;
  }
  LoadDbfDialog(MyFrame * parent, wxString & path, wxString & table,
                wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & table,
              wxString & defCs);
  virtual ~ LoadDbfDialog();
  void CreateControls();
  wxString & GetTable()
  {
    return Table;
  }
  wxString & GetCharset()
  {
    return Charset;
  }
  bool IsUserDefinedPKey()
  {
    return UserDefinedPKey;
  }
  wxString & GetPKColumn()
  {
    return PKColumn;
  }
  void LoadPKFields();
  void OnOk(wxCommandEvent & event);
  void OnUserPKey(wxCommandEvent & event);
};

class NetworkDialog:public wxDialog
{
//
// a dialog preparing a BUILD NETWORK
//
private:
  MyFrame * MainFrame;
  wxString TableName;           // the table name
  wxString FromColumn;          // the NodeFrom column name
  wxString ToColumn;            // the NodeTo column name
  wxString GeomColumn;          // the Geometry column name
  bool GeomLength;              // Cost is Geometry Length
  wxString CostColumn;          // the Cost column name
  bool Bidirectional;           // Bidirectional arcs
  bool OneWays;                 // OneWays columns supported
  wxString OneWayToFrom;        // the OneWay To-From column
  wxString OneWayFromTo;        // the OneWay From-To column
  bool NameEnabled;             // Name column supported
  wxString NameColumn;          // the Name column name
  bool AStarSupported;          // A* algorithm supported
public:
    NetworkDialog()
  {;
  }
  NetworkDialog(MyFrame * parent);
  bool Create(MyFrame * parent);
  virtual ~ NetworkDialog()
  {;
  }
  void CreateControls();
  wxString & GetTableName()
  {
    return TableName;
  }
  wxString & GetFromColumn()
  {
    return FromColumn;
  }
  wxString & GetToColumn()
  {
    return ToColumn;
  }
  wxString & GetGeomColumn()
  {
    return GeomColumn;
  }
  wxString & GetNameColumn()
  {
    return NameColumn;
  }
  bool IsGeomLength()
  {
    return GeomLength;
  }
  wxString & GetCostColumn()
  {
    return CostColumn;
  }
  bool IsBidirectional()
  {
    return Bidirectional;
  }
  bool IsOneWays()
  {
    return OneWays;
  }
  wxString & GetOneWayFromTo()
  {
    return OneWayFromTo;
  }
  wxString & GetOneWayToFrom()
  {
    return OneWayToFrom;
  }
  bool IsNameEnabled()
  {
    return NameEnabled;
  }
  bool IsAStarSupported()
  {
    return AStarSupported;
  }
  void OnTable(wxCommandEvent & event);
  void OnDirection(wxCommandEvent & event);
  void OnCost(wxCommandEvent & event);
  void OnOneWay(wxCommandEvent & event);
  void OnNameEnabled(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class ExifDialog:public wxDialog
{
//
// a dialog preparing an IMPORT EXIF PHOTOS
//
private:
  MyFrame * MainFrame;
  wxString ImgPath;             // the file name
  wxString DirPath;             // the folder path
  bool Folder;                  // import a whole folder
  bool Metadata;                // feed Metadata tables
  bool GpsOnly;                 // import only if GpsExif present
public:
    ExifDialog()
  {;
  }
  ExifDialog(MyFrame * parent, wxString & dir_path, wxString & img_path);
  bool Create(MyFrame * parent, wxString & dir_path, wxString & img_path);
  virtual ~ ExifDialog()
  {;
  }
  void CreateControls();
  wxString & GetImgPath()
  {
    return ImgPath;
  }
  wxString & GetDirPath()
  {
    return DirPath;
  }
  bool IsFolder()
  {
    return Folder;
  }
  bool IsMetadata()
  {
    return Metadata;
  }
  bool IsGpsOnly()
  {
    return GpsOnly;
  }
  void OnFolder(wxCommandEvent & event);
  void OnMetadata(wxCommandEvent & event);
  void OnGpsOnly(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class XmlDocumentsDialog:public wxDialog
{
//
// a dialog preparing an IMPORT XML DOCUMENTS
//
private:
  MyFrame * MainFrame;
  wxString XmlPath;             // the file name
  wxString DirPath;             // the folder path
  wxString Suffix;              // the optional file suffix
  wxString TargetTable;         // the target Table
  wxString PkName;              // the Primary Key name
  wxString XmlColumn;           // the XML Payload Column
  wxString SchemaUriColumn;     // the SchemaURI Column
  wxString InPathColumn;        // the InPath Column
  wxString ParseErrorColumn;    // the XmlParseError Column
  wxString ValidateErrorColumn; // the XmlValidateError Column
  bool Folder;                  // import a whole folder
  bool Compressed;              // compressed XmlBLOB
  bool Validated;               // apply Schema Validation
  bool InternalSchema;          // apply the Internally declared Schema URI
  bool OkSuffix;                // apply suffix restriction
  bool OkSchemaColumn;          // create a "schemaURI" column
  bool OkInPathColumn;          // create an "inPath" column
  bool OkParseErrorColumn;      // create the "XmlParseError" column
  bool OkValidateErrorColumn;   // create the "XmlSchemaValidateError" column
  wxString SchemaURI;           // the Schema URI for validation
public:
    XmlDocumentsDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & dir_path, wxString & xml_path);
  virtual ~ XmlDocumentsDialog()
  {;
  }
  void CreateControls();
  wxString & GetXmlPath()
  {
    return XmlPath;
  }
  wxString & GetDirPath()
  {
    return DirPath;
  }
  wxString & GetSuffix()
  {
    return Suffix;
  }
  wxString & GetTargetTable()
  {
    return TargetTable;
  }
  wxString & GetPkName()
  {
    return PkName;
  }
  wxString & GetXmlColumn()
  {
    return XmlColumn;
  }
  bool IsFolder()
  {
    return Folder;
  }
  bool IsCompressed()
  {
    return Compressed;
  }
  bool IsInternalSchemaURI()
  {
    return InternalSchema;
  }
  wxString & GetSchemaURI()
  {
    return SchemaURI;
  }
  wxString & GetSchemaUriColumn()
  {
    return SchemaUriColumn;
  }
  wxString & GetInPathColumn()
  {
    return InPathColumn;
  }
  wxString & GetParseErrorColumn()
  {
    return ParseErrorColumn;
  }
  wxString & GetValidateErrorColumn()
  {
    return ValidateErrorColumn;
  }
  void OnFolder(wxCommandEvent & event);
  void OnSuffixChanged(wxCommandEvent & event);
  void OnCompressionChanged(wxCommandEvent & event);
  void OnValidationChanged(wxCommandEvent & event);
  void OnInternalSchemaChanged(wxCommandEvent & event);
  void OnSchemaColumnChanged(wxCommandEvent & event);
  void OnInPathColumnChanged(wxCommandEvent & event);
  void OnParseErrorColumnChanged(wxCommandEvent & event);
  void OnValidateErrorColumnChanged(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class DxfDialog:public wxDialog
{
//
// a dialog preparing an IMPORT DXF FILE(s)
//
private:
  MyFrame * MainFrame;
  wxString DxfPath;             // the file name
  wxString DirPath;             // the folder path
  wxString Prefix;              // the optional table-name prefix
  wxString SingleLayer;         // filtering a single DXF layer by its name
  int Srid;                     // the SRID to be applied
  bool Folder;                  // import a whole folder
  bool OkPrefix;                // apply table-name prefix
  bool OkSingle;                // apply single layer filter
  bool Force2D;                 // always forcing 2D
  bool Force3D;                 // always forcing 3D
  bool LinkedRings;             // special - linked rings
  bool UnlinkedRings;           // special - unlinked rings
  bool ImportMixed;             // mixed layers mode
  bool AppendMode;              // append mode
public:
    DxfDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & dir_path, wxString & dxf_path);
  virtual ~ DxfDialog()
  {;
  }
  void CreateControls();
  wxString & GetDxfPath()
  {
    return DxfPath;
  }
  wxString & GetDirPath()
  {
    return DirPath;
  }
  wxString & GetPrefix()
  {
    return Prefix;
  }
  wxString & GetSingleLayer()
  {
    return SingleLayer;
  }
  int GetSrid()
  {
    return Srid;
  }
  bool IsFolder()
  {
    return Folder;
  }
  bool IsForce2D()
  {
    return Force2D;
  }
  bool IsForce3D()
  {
    return Force3D;
  }
  bool IsLinkedRings()
  {
    return LinkedRings;
  }
  bool IsUnlinkedRings()
  {
    return UnlinkedRings;
  }
  bool IsImportMixed()
  {
    return ImportMixed;
  }
  bool IsAppendMode()
  {
    return AppendMode;
  }
  void OnFolder(wxCommandEvent & event);
  void OnPrefixChanged(wxCommandEvent & event);
  void OnSingleLayerChanged(wxCommandEvent & event);
  void OnDimensionChanged(wxCommandEvent & event);
  void OnModeChanged(wxCommandEvent & event);
  void OnRingsChanged(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class SqlScriptDialog:public wxDialog
{
//
// a dialog preparing an SQL SCRIPT execute
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the SHP base path
  wxString Default;             // the default charset
  wxString Charset;             // the target charset
public:
    SqlScriptDialog()
  {;
  }
  SqlScriptDialog(MyFrame * parent, wxString & path, wxString & defCs);
  bool Create(MyFrame * parent, wxString & path, wxString & defCs);
  virtual ~ SqlScriptDialog()
  {;
  }
  void CreateControls();
  wxString & GetCharset()
  {
    return Charset;
  }
  void OnOk(wxCommandEvent & event);
};

class DefaultCharsetDialog:public wxDialog
{
//
// a dialog for selecting DEFAULT CHARSET
//
private:
  MyFrame * MainFrame;
  wxString Charset;             // the default charset
  bool AskCharset;              // true / false
public:
    DefaultCharsetDialog()
  {;
  }
  DefaultCharsetDialog(MyFrame * parent, wxString & charset, bool ask);
  bool Create(MyFrame * parent, wxString & charset, bool ask);
  virtual ~ DefaultCharsetDialog()
  {;
  }
  void CreateControls();
  wxString & GetCharset()
  {
    return Charset;
  }
  bool IsSetAskCharset()
  {
    return AskCharset;
  }
  void OnOk(wxCommandEvent & event);
};

class BlobExplorerDialog:public wxPropertySheetDialog
{
//
// a dialog to explore a BLOB value
//
private:
  MyFrame * MainFrame;
  int BlobSize;                 // the BLOB size
  unsigned char *Blob;          // the BLOB value
  int BlobType;                 // the BLOB type
  bool IsSVG;
  double SvgWidth;
  double SvgHeight;
  int SvgSize;
  gaiaGeomCollPtr Geometry;     // the geometry [optional]
  wxString XMLDocument;         // the XMLDocument [optional]
  wxString XMLIndented;         // the XMLDocument (indented) [optional]
  wxImage *Image;               // the image [optional]
  wxBitmap GeomPreview;         // the geometry preview
  wxString WKTstring;           // the WKT Geometry notation
  wxString EWKTstring;          // the EWKT Geometry notation
  wxString SVGstring;           // the SVG Geometry notation
  wxString KMLstring;           // the KML Geometry notation
  wxString GMLstring;           // the GML Geometry notation
  wxString GeoJSONstring;       // the GeoJSON Geometry notation
  bool SVGrelative;             // SVG relative / absolute mode
  int SVGprecision;             // SVG precision
  int KMLprecision;             // KML precision
  bool GMLv2v3;                 // GML version (v2 / v3)
  int GMLprecision;             // GML precision
  int GeoJSONoptions;           // GeoJSON options
  int GeoJSONprecision;         // GeoJSON precision
  void FormatWKT(wxTextCtrl * txtCtrl, wxString & in, wxString & out);
  void FormatSVG(wxTextCtrl * txtCtrl, wxString & in, wxString & out);
public:
    BlobExplorerDialog()
  {;
  }
  BlobExplorerDialog(MyFrame * parent, int blob_size, unsigned char *blob);
  bool Create(MyFrame * parent, int blob_size, unsigned char *blob);
  virtual ~ BlobExplorerDialog()
  {
    if (Geometry)
      gaiaFreeGeomColl(Geometry);
    if (Image)
      delete Image;
  }
  void DrawGeometry(int horz, int vert);
  wxPanel *CreateHexadecimalPage(wxWindow * book);
  wxPanel *CreateGeometryPage(wxWindow * book);
  wxPanel *CreateWKTPage(wxWindow * book);
  wxPanel *CreateEWKTPage(wxWindow * book);
  wxPanel *CreateSVGPage(wxWindow * book);
  wxPanel *CreateKMLPage(wxWindow * book);
  wxPanel *CreateGMLPage(wxWindow * book);
  wxPanel *CreateGeoJSONPage(wxWindow * book);
  wxPanel *CreateImagePage(wxWindow * book);
  wxPanel *CreateXmlDocumentPage(wxWindow * book);
  wxPanel *CreateXmlIndentedPage(wxWindow * book);
  void UpdateHexadecimalPage();
  void UpdateGeometryPage();
  void UpdateImagePage();
  void UpdateXmlDocumentPage();
  void UpdateXmlIndentedPage();
  void UpdateWKTPage();
  void UpdateEWKTPage();
  void UpdateSVGPage();
  void UpdateKMLPage();
  void UpdateGMLPage();
  void UpdateGeoJSONPage();
  gaiaGeomCollPtr GetGeometry()
  {
    return Geometry;
  }
  wxImage *GetImage()
  {
    return Image;
  }
  int GetBlobType()
  {
    return BlobType;
  }
  void OnOk(wxCommandEvent & event);
  void OnPageChanged(wxNotebookEvent & event);
  void OnCopyWKT(wxCommandEvent & event);
  void OnCopyEWKT(wxCommandEvent & event);
  void OnSVGRelative(wxCommandEvent & event);
  void OnSVGPrecision(wxCommandEvent & event);
  void OnCopySVG(wxCommandEvent & event);
  void OnKMLPrecision(wxCommandEvent & event);
  void OnCopyKML(wxCommandEvent & event);
  void OnGMLv2v3(wxCommandEvent & event);
  void OnGMLPrecision(wxCommandEvent & event);
  void OnCopyGML(wxCommandEvent & event);
  void OnGeoJSONOptions(wxCommandEvent & event);
  void OnGeoJSONPrecision(wxCommandEvent & event);
  void OnCopyGeoJSON(wxCommandEvent & event);
  void OnCopyXmlDocument(wxCommandEvent & event);
  void OnCopyXmlIndented(wxCommandEvent & event);
};

class GraphicsGeometry:public wxStaticBitmap
{
//
// a window to show some Geometry in a graphical fashion
//
private:
  BlobExplorerDialog * Parent;
public:
  GraphicsGeometry(BlobExplorerDialog * parent, wxWindow * panel, wxWindowID id,
                   const wxBitmap & bmp, wxSize const &size);
    virtual ~ GraphicsGeometry()
  {;
  }
};

class ImageShow:public wxStaticBitmap
{
//
// a window to show some Image [Jpeg-Png-Gif]
//
private:
  BlobExplorerDialog * Parent;
public:
  ImageShow(BlobExplorerDialog * parent, wxWindow * panel, wxWindowID id,
            const wxBitmap & bmp, const wxSize & size);
    virtual ~ ImageShow()
  {;
  }
  void OnRightClick(wxMouseEvent & event);
  void OnCmdCopy(wxCommandEvent & event);
};

class MyHexList:public wxListCtrl
{
//
// a class for Hexdecimal dumps
//
private:
  BlobExplorerDialog * Parent;
  int BlobSize;                 // the BLOB size
  unsigned char *Blob;          // the BLOB value
public:
    MyHexList(BlobExplorerDialog * parent, unsigned char *blob,
              int blob_size, wxWindow * panel, wxWindowID id,
              const wxPoint & pos = wxDefaultPosition, const wxSize & size =
              wxDefaultSize, long style = 0);
    virtual ~ MyHexList();
  virtual wxString OnGetItemText(long item, long column) const;
};

class NetNodePre
{
//
// a class to store a temporary node for Network
//
private:
  sqlite3_int64 Id;
  wxString Code;
  NetNodePre *Next;
public:
    NetNodePre(sqlite3_int64 id);
    NetNodePre(const char *code);
   ~NetNodePre()
  {;
  }
  sqlite3_int64 GetId()
  {
    return Id;
  }
  wxString & GetCode()
  {
    return Code;
  }
  void SetNext(NetNodePre * next)
  {
    Next = next;
  }
  NetNodePre *GetNext()
  {
    return Next;
  }
};

class NetNode
{
//
// a class to store a final node for Network
//
private:
  int InternalIndex;
  sqlite3_int64 Id;
  wxString Code;
  double X;
  double Y;
  class NetArcRef *First;
  class NetArcRef *Last;
  NetNode *Next;
public:
    NetNode(sqlite3_int64 id);
    NetNode(wxString & code);
   ~NetNode();
  int GetInternalIndex()
  {
    return InternalIndex;
  }
  void SetInternalIndex(int idx)
  {
    InternalIndex = idx;
  }
  sqlite3_int64 GetId()
  {
    return Id;
  }
  wxString & GetCode()
  {
    return Code;
  }
  double GetX()
  {
    return X;
  }
  void SetX(double x)
  {
    X = x;
  }
  double GetY()
  {
    return Y;
  }
  void SetY(double y)
  {
    Y = y;
  }
  void AddOutcoming(class NetArc * pA);
  NetArcRef *GetFirst()
  {
    return First;
  }
  NetArc **PrepareOutcomings(int *count);
  void SetNext(NetNode * next)
  {
    Next = next;
  }
  NetNode *GetNext()
  {
    return Next;
  }
};

class NetArc
{
//
// a class to store an arc for Network
//
private:
  sqlite3_int64 RowId;
  NetNode *From;
  NetNode *To;
  double Cost;
  NetArc *Next;
public:
    NetArc(sqlite3_int64 rowid, NetNode * from, NetNode * to, double cost);
   ~NetArc()
  {;
  }
  sqlite3_int64 GetRowId()
  {
    return RowId;
  }
  NetNode *GetFrom()
  {
    return From;
  }
  NetNode *GetTo()
  {
    return To;
  }
  double GetCost()
  {
    return Cost;
  }
  void SetNext(NetArc * next)
  {
    Next = next;
  }
  NetArc *GetNext()
  {
    return Next;
  }
};

class NetArcRef
{
//
// a class to store a reference to an arc for Network
//
private:
  NetArc * Reference;
  NetArcRef *Next;
public:
    NetArcRef(NetArc * arc)
  {
    Reference = arc;
    Next = NULL;
  }
   ~NetArcRef()
  {;
  }
  NetArc *GetReference()
  {
    return Reference;
  }
  void SetNext(NetArcRef * next)
  {
    Next = next;
  }
  NetArcRef *GetNext()
  {
    return Next;
  }
};

class Network
{
//
// a class representing a Network
//
private:
  NetNodePre * FirstPre;
  NetNodePre *LastPre;
  int NumPreNodes;
  NetNodePre **SortedPreNodes;
  NetArc *FirstArc;
  NetArc *LastArc;
  NetNode *FirstNode;
  NetNode *LastNode;
  int NumNodes;
  NetNode **SortedNodes;
  bool Error;
  bool NodeCode;
  int MaxCodeLength;
public:
    Network();
   ~Network();
  void CleanPreNodes();
  void InsertNode(sqlite3_int64 id);
  void InsertNode(const char *code);
  void AddNode(sqlite3_int64 id);
  void AddNode(wxString & code);
  NetNode *ProcessNode(sqlite3_int64 id, double x, double y, NetNode ** pOther);
  NetNode *ProcessNode(wxString & code, double x, double y, NetNode ** pOther);
  void Sort();
  NetNode *Find(sqlite3_int64 id);
  NetNode *Find(wxString & code);
  NetNode *GetSortedNode(sqlite3_int64 x);
  void AddArc(sqlite3_int64 rowid, sqlite3_int64 id_from, sqlite3_int64 id_to,
              double node_from_x, double node_from_y, double node_to_x,
              double node_to_y, double cost);
  void AddArc(sqlite3_int64 rowid, const char *code_from, const char *code_to,
              double node_from_x, double node_from_y, double node_to_x,
              double node_to_y, double cost);
  void InitNodes();
  void SetError()
  {
    Error = true;
  }
  bool IsError()
  {
    return Error;
  }
  void SetNodeCode(bool mode)
  {
    NodeCode = mode;
  }
  bool IsNodeCode()
  {
    return NodeCode;
  }
  int GetNumNodes()
  {
    return NumNodes;
  }
  int GetMaxCodeLength()
  {
    return MaxCodeLength;
  }
};

class AutoFDOTable
{
private:
  char *Name;
  AutoFDOTable *Next;
public:
    AutoFDOTable(const char *name, const int len)
  {
    Name = new char[len+1];
      strcpy(Name, name);
      Next = NULL;
  }
   ~AutoFDOTable()
  {
    if (Name)
      delete[]Name;
  }
  char *GetName()
  {
    return Name;
  }
  void SetNext(AutoFDOTable * next)
  {
    Next = next;
  }
  AutoFDOTable *GetNext()
  {
    return Next;
  }
};

class AutoFDOTables
{
private:
  AutoFDOTable * First;
  AutoFDOTable *Last;
public:
    AutoFDOTables()
  {
    First = NULL;
    Last = NULL;
  }
   ~AutoFDOTables();
  void Add(const char *name, const int len);
  AutoFDOTable *GetFirst()
  {
    return First;
  }
};

class AutoSaveDialog:public wxDialog
{
//
// a dialog to manage AutoSave
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the path to save
  int Seconds;                  // interval 
  wxRadioBox *IntervalCtrl;
  wxTextCtrl *PathCtrl;
public:
    AutoSaveDialog()
  {;
  }
  AutoSaveDialog(MyFrame * parent, wxString & path, int secs);
  bool Create(MyFrame * parent, wxString & path, int secs);
  virtual ~ AutoSaveDialog()
  {;
  }
  void CreateControls();
  int GetSeconds()
  {
    return Seconds;
  }
  void OnOk(wxCommandEvent & event);
  wxString & GetPath()
  {
    return Path;
  }
  void OnIntervalChanged(wxCommandEvent & event);
  void OnChangePath(wxCommandEvent & event);
};

class LoadXmlDialog:public wxDialog
{
//
// a dialog to load XML documents
//
private:
  MyFrame * MainFrame;
  wxString Path;                // the XML path
  bool Compressed;              // compressed XML 
  bool Validate;                // Schema validation
  wxString SchemaURI;           // the Schema URI (if validation is required)
public:
    LoadXmlDialog()
  {;
  }
  bool Create(MyFrame * parent, wxString & path);
  virtual ~ LoadXmlDialog()
  {;
  }
  void CreateControls();
  bool IsCompressed()
  {
    return Compressed;
  }
  wxString & GetSchemaURI()
  {
    return SchemaURI;
  }
  void OnOk(wxCommandEvent & event);
  void OnCompressionChanged(wxCommandEvent & event);
  void OnValidationChanged(wxCommandEvent & event);
};

class AuxTable
{
// a class used by ComposerDialog [table item]
private:
  wxString TableName;
  wxString Geometries[128];
  int MaxGeometryIndex;
  AuxTable *Next;
public:
    AuxTable(wxString & table);
   ~AuxTable()
  {;
  }
  void AddGeometryColumn(wxString & geom);
  wxString & GetTableName()
  {
    return TableName;
  }
  wxString & GetGeometryColumn(int ind);
  int GetGeometriesCount()
  {
    return MaxGeometryIndex;
  }
  void SetNext(AuxTable * next)
  {
    Next = next;
  }
  AuxTable *GetNext()
  {
    return Next;
  }
};

class AuxTableList
{
// a class used by ComposerDialog [tables list]
private:
  AuxTable * First;
  AuxTable *Last;
  int Count;
public:
    AuxTableList();
   ~AuxTableList();
  void Flush();
  void Populate(sqlite3 * handle);
  int GetCount()
  {
    return Count;
  }
  AuxTable *GetFirst()
  {
    return First;
  }
};

class AuxColumn
{
// a class used by ComposerDialog [table item]
private:
  wxString Name;
  wxString AliasName;
  bool Selected;
  AuxColumn *Next;
public:
    AuxColumn(wxString & name);
   ~AuxColumn()
  {;
  }
  wxString & GetName()
  {
    return Name;
  }
  void SetState(bool mode)
  {
    Selected = mode;
  }
  bool IsSelected()
  {
    return Selected;
  }
  void SetAliasName(wxString & alias)
  {
    AliasName = alias;
  }
  wxString & GetAliasName()
  {
    return AliasName;
  }
  void SetNext(AuxColumn * next)
  {
    Next = next;
  }
  AuxColumn *GetNext()
  {
    return Next;
  }
};

class AuxColumnList
{
// a class used by ComposerDialog [tables list]
private:
  AuxColumn * First;
  AuxColumn *Last;
  int Count;
public:
    AuxColumnList();
   ~AuxColumnList();
  void Flush();
  void Populate(sqlite3 * handle, wxString & table);
  int GetCount()
  {
    return Count;
  }
  AuxColumn *GetFirst()
  {
    return First;
  }
  void SetState(int ind, bool mode);
  void SetState(wxString & column);
  void SetAlias(wxString & column, wxString & alias);
  bool HasSelectedColumns();
};

class DumpKmlDialog:public wxDialog
{
//
// a dialog preparing a DUMP KML
//
private:
  MyFrame * MainFrame;
  wxString Table;               // the table's name
  wxString Column;              // the column's name for Geometry
  bool isNameConst;
  bool isDescConst;
  wxString Name;
  wxString Desc;
  int Precision;
  wxComboBox *NameCtrl;
  wxTextCtrl *NameConstCtrl;
  wxComboBox *DescCtrl;
  wxTextCtrl *DescConstCtrl;
  wxSpinCtrl *PrecisionCtrl;
  AuxColumnList ColumnList;
  void InitializeComboColumns(wxComboBox * ctrl);
public:
    DumpKmlDialog()
  {;
  }
  DumpKmlDialog(MyFrame * parent, wxString & table, wxString & column);
  bool Create(MyFrame * parent, wxString & table, wxString & column);
  virtual ~ DumpKmlDialog()
  {;
  }
  void CreateControls();
  bool IsNameConst()
  {
    return isNameConst;
  }
  wxString & GetName()
  {
    return Name;
  }
  bool IsDescConst()
  {
    return isDescConst;
  }
  wxString & GetDesc()
  {
    return Desc;
  }
  int GetPrecision()
  {
    return Precision;
  }
  void OnNameSelected(wxCommandEvent & event);
  void OnDescSelected(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class DumpSpreadsheetDialog:public wxDialog
{
//
// a dialog preparing a DUMP Speadsheet
//
private:
  MyFrame * MainFrame;
  char DecimalPoint;
  bool DateTimes;
public:
    DumpSpreadsheetDialog()
  {;
  }
  DumpSpreadsheetDialog(MyFrame * parent);
  bool Create(MyFrame * parent);
  virtual ~ DumpSpreadsheetDialog()
  {;
  }
  void CreateControls();
  char GetDecimalPoint()
  {
    return DecimalPoint;
  }
  bool IsDateTimes()
  {
    return DateTimes;
  }
  void OnDecimalPointSelected(wxCommandEvent & event);
  void OnDateTimesSelected(wxCommandEvent & event);
  void OnOk(wxCommandEvent & event);
};

class ComposerMainPage:public wxPanel
{
//
// first page used by Query/View COMPOSER
//
private:
  class ComposerDialog * Parent;
  wxCheckBox *Table2Ctrl;
  wxComboBox *Table1NameCtrl;
  wxComboBox *Table2NameCtrl;
  wxTextCtrl *Table1AliasCtrl;
  wxTextCtrl *Table2AliasCtrl;
  wxListBox *Table1ColumnsCtrl;
  wxListBox *Table2ColumnsCtrl;
  wxRadioBox *JoinModeCtrl;
  wxCheckBox *Match2Ctrl;
  wxCheckBox *Match3Ctrl;
  wxComboBox *Match1Table1Ctrl;
  wxComboBox *Match1Table2Ctrl;
  wxComboBox *Match2Table1Ctrl;
  wxComboBox *Match2Table2Ctrl;
  wxComboBox *Match3Table1Ctrl;
  wxComboBox *Match3Table2Ctrl;
public:
    ComposerMainPage()
  {;
  }
  bool Create(ComposerDialog * parent);
  virtual ~ ComposerMainPage()
  {;
  }
  void CreateControls();
  void SelectGeometryColumn(wxString & column, bool table2);
  void InitializeComboColumns(wxComboBox * ctrl, bool table2);
  void InitializeListColumns(wxListBox * ctrl, bool table2);
  void OnTable2Enabled(wxCommandEvent & event);
  void OnTable1Selected(wxCommandEvent & event);
  void OnTable2Selected(wxCommandEvent & event);
  void OnColumns1Selected(wxCommandEvent & event);
  void OnColumns2Selected(wxCommandEvent & event);
  void OnTable1AliasChanged(wxCommandEvent & event);
  void OnTable2AliasChanged(wxCommandEvent & event);
  void OnJoinModeChanged(wxCommandEvent & event);
  void OnMatch2Enabled(wxCommandEvent & event);
  void OnMatch3Enabled(wxCommandEvent & event);
  void OnMatch1Table1Selected(wxCommandEvent & event);
  void OnMatch1Table2Selected(wxCommandEvent & event);
  void OnMatch2Table1Selected(wxCommandEvent & event);
  void OnMatch2Table2Selected(wxCommandEvent & event);
  void OnMatch3Table1Selected(wxCommandEvent & event);
  void OnMatch3Table2Selected(wxCommandEvent & event);
};

class ComposerFilterPage:public wxPanel
{
//
// second page used by Query/View COMPOSER
//
private:
  class ComposerDialog * Parent;
  wxCheckBox *Where1EnabledCtrl;
  wxRadioBox *Where1TableCtrl;
  wxComboBox *Where1ColumnCtrl;
  wxComboBox *Where1OperatorCtrl;
  wxTextCtrl *Where1ValueCtrl;
  wxCheckBox *Where2EnabledCtrl;
  wxRadioBox *Where2TableCtrl;
  wxComboBox *Where2ColumnCtrl;
  wxComboBox *Where2OperatorCtrl;
  wxTextCtrl *Where2ValueCtrl;
  wxCheckBox *Where3EnabledCtrl;
  wxRadioBox *Where3TableCtrl;
  wxComboBox *Where3ColumnCtrl;
  wxComboBox *Where3OperatorCtrl;
  wxTextCtrl *Where3ValueCtrl;
  wxRadioBox *Connector12Ctrl;
  wxRadioBox *Connector23Ctrl;
public:
    ComposerFilterPage()
  {;
  }
  bool Create(ComposerDialog * parent);
  virtual ~ ComposerFilterPage()
  {;
  }
  void CreateControls();
  void Table1Status(bool ok);
  void Table2Status(bool ok);
  void InitializeColumns(wxComboBox * ctrl, bool table2);
  void InitializeOperators(wxComboBox * ctrl);
  void OnWhere1Enabled(wxCommandEvent & event);
  void OnWhere1TableChanged(wxCommandEvent & event);
  void OnWhere1ColumnSelected(wxCommandEvent & event);
  void OnWhere1OperatorSelected(wxCommandEvent & event);
  void OnWhere1ValueChanged(wxCommandEvent & event);
  void OnWhere2Enabled(wxCommandEvent & event);
  void OnWhere2TableChanged(wxCommandEvent & event);
  void OnWhere2ColumnSelected(wxCommandEvent & event);
  void OnWhere2OperatorSelected(wxCommandEvent & event);
  void OnWhere2ValueChanged(wxCommandEvent & event);
  void OnWhere3Enabled(wxCommandEvent & event);
  void OnWhere3TableChanged(wxCommandEvent & event);
  void OnWhere3ColumnSelected(wxCommandEvent & event);
  void OnWhere3OperatorSelected(wxCommandEvent & event);
  void OnWhere3ValueChanged(wxCommandEvent & event);
  void OnConnector12Changed(wxCommandEvent & event);
  void OnConnector23Changed(wxCommandEvent & event);
};

class ComposerOrderPage:public wxPanel
{
//
// third page used by Query/View COMPOSER
//
private:
  class ComposerDialog * Parent;
  wxCheckBox *Order1EnabledCtrl;
  wxRadioBox *Order1TableCtrl;
  wxComboBox *Order1ColumnCtrl;
  wxRadioBox *Order1DescCtrl;
  wxCheckBox *Order2EnabledCtrl;
  wxRadioBox *Order2TableCtrl;
  wxComboBox *Order2ColumnCtrl;
  wxRadioBox *Order2DescCtrl;
  wxCheckBox *Order3EnabledCtrl;
  wxRadioBox *Order3TableCtrl;
  wxComboBox *Order3ColumnCtrl;
  wxRadioBox *Order3DescCtrl;
  wxCheckBox *Order4EnabledCtrl;
  wxRadioBox *Order4TableCtrl;
  wxComboBox *Order4ColumnCtrl;
  wxRadioBox *Order4DescCtrl;
public:
    ComposerOrderPage()
  {;
  }
  bool Create(ComposerDialog * parent);
  virtual ~ ComposerOrderPage()
  {;
  }
  void CreateControls();
  void Table1Status(bool ok);
  void Table2Status(bool ok);
  void InitializeColumns(wxComboBox * ctrl, bool table2);
  void OnOrder1Enabled(wxCommandEvent & event);
  void OnOrder1TableChanged(wxCommandEvent & event);
  void OnOrder1ColumnSelected(wxCommandEvent & event);
  void OnOrder1DescChanged(wxCommandEvent & event);
  void OnOrder2Enabled(wxCommandEvent & event);
  void OnOrder2TableChanged(wxCommandEvent & event);
  void OnOrder2ColumnSelected(wxCommandEvent & event);
  void OnOrder2DescChanged(wxCommandEvent & event);
  void OnOrder3Enabled(wxCommandEvent & event);
  void OnOrder3TableChanged(wxCommandEvent & event);
  void OnOrder3ColumnSelected(wxCommandEvent & event);
  void OnOrder3DescChanged(wxCommandEvent & event);
  void OnOrder4Enabled(wxCommandEvent & event);
  void OnOrder4TableChanged(wxCommandEvent & event);
  void OnOrder4ColumnSelected(wxCommandEvent & event);
  void OnOrder4DescChanged(wxCommandEvent & event);
};

class ComposerViewPage:public wxPanel
{
//
// fourth page used by Query/View COMPOSER
//
private:
  class ComposerDialog * Parent;
  wxRadioBox *ViewTypeCtrl;
  wxTextCtrl *ViewNameCtrl;
  wxRadioBox *GeomTableCtrl;
  wxComboBox *GeometryColumnCtrl;
  wxCheckBox *Writable1Ctrl;
  wxCheckBox *Writable2Ctrl;
public:
    ComposerViewPage()
  {;
  }
  bool Create(ComposerDialog * parent);
  virtual ~ ComposerViewPage()
  {;
  }
  void CreateControls();
  void Table1Status(bool ok);
  void Table2Status(bool ok);
  void OnGeomTableChanged(wxCommandEvent & event);
  void InitializeGeometries(bool table2);
  void OnGeometryColumnSelected(wxCommandEvent & event);
  void OnViewTypeChanged(wxCommandEvent & event);
  void OnViewNameChanged(wxCommandEvent & event);
  void OnWritable1Changed(wxCommandEvent & event);
  void OnWritable2Changed(wxCommandEvent & event);
};

class ComposerDialog:public wxDialog
{
//
// a dialog used by Query/View COMPOSER
//
private:
  MyFrame * MainFrame;
  AuxTableList TableList;
  AuxColumnList Column1List;
  AuxColumnList Column2List;
  bool IncompleteSql;
  bool Table2Enabled;
  wxString TableName1;
  wxString TableName2;
  wxString TableAlias1;
  wxString TableAlias2;
  bool LeftJoin;
  bool Match2Enabled;
  bool Match3Enabled;
  wxString Match1Table1;
  wxString Match1Table2;
  wxString Match2Table1;
  wxString Match2Table2;
  wxString Match3Table1;
  wxString Match3Table2;
  bool Where1Enabled;
  bool Where2Enabled;
  bool Where3Enabled;
  bool Where1Table2;
  bool Where2Table2;
  bool Where3Table2;
  bool AndOr12;
  bool AndOr23;
  wxString Where1Column;
  wxString Where2Column;
  wxString Where3Column;
  wxString Where1Operator;
  wxString Where2Operator;
  wxString Where3Operator;
  wxString Where1Value;
  wxString Where2Value;
  wxString Where3Value;
  bool Order1Enabled;
  bool Order2Enabled;
  bool Order3Enabled;
  bool Order4Enabled;
  bool Order1Table2;
  bool Order2Table2;
  bool Order3Table2;
  bool Order4Table2;
  wxString Order1Column;
  wxString Order2Column;
  wxString Order3Column;
  wxString Order4Column;
  bool Order1Desc;
  bool Order2Desc;
  bool Order3Desc;
  bool Order4Desc;
  bool PlainView;
  bool SpatialView;
  wxString ViewName;
  bool ViewGeomTable2;
  wxString GeometryColumn;
  bool Writable1;
  bool Writable2;
  wxString GeometryColumnAlias;
  wxString GeometryRowidAlias;
  wxString SqlSample;
  wxString SqlTriggerInsert;
  wxString SqlTriggerUpdate;
  wxString SqlTriggerDelete;
  wxTextCtrl *SqlCtrl;
  wxNotebook *TabCtrl;
  ComposerMainPage *Page1;
  ComposerFilterPage *Page2;
  ComposerOrderPage *Page3;
  ComposerViewPage *Page4;
public:
    ComposerDialog()
  {;
  }
  bool Create(MyFrame * parent);
  virtual ~ ComposerDialog()
  {;
  }
  void CreateControls();
  AuxTableList *GetTableList()
  {
    return &TableList;
  }
  void PopulateColumnList1()
  {
    Column1List.Populate(MainFrame->GetSqlite(), TableName1);
  }
  void PopulateColumnList2()
  {
    Column2List.Populate(MainFrame->GetSqlite(), TableName2);
  }
  AuxColumnList *GetColumn1List()
  {
    return &Column1List;
  }
  AuxColumnList *GetColumn2List()
  {
    return &Column2List;
  }
  void SetTable2Enabled(bool mode)
  {
    Table2Enabled = mode;
  }
  bool IsTable2Enabled()
  {
    return Table2Enabled;
  }
  void SetTableName1(wxString name)
  {
    TableName1 = name;
  }
  wxString & GetTableName1()
  {
    return TableName1;
  }
  void SetTableName2(wxString name)
  {
    TableName2 = name;
  }
  wxString & GetTableAlias2()
  {
    return TableAlias2;
  }
  void SetTableAlias1(wxString alias)
  {
    TableAlias1 = alias;
  }
  wxString & GetTableAlias1()
  {
    return TableAlias1;
  }
  void SetTableAlias2(wxString alias)
  {
    TableAlias2 = alias;
  }
  wxString & GetTableName2()
  {
    return TableName2;
  }
  void SetLeftJoin(bool mode)
  {
    LeftJoin = mode;
  }
  bool IsLeftJoin()
  {
    return LeftJoin;
  }
  void SetMatch2Enabled(bool mode)
  {
    Match2Enabled = mode;
  }
  bool IsMatch2Enabled()
  {
    return Match2Enabled;
  }
  void SetMatch3Enabled(bool mode)
  {
    Match3Enabled = mode;
  }
  bool IsMatch3Enabled()
  {
    return Match3Enabled;
  }
  void SetMatch1Table1(wxString name)
  {
    Match1Table1 = name;
  }
  wxString & GetMatch1Table1()
  {
    return Match1Table1;
  }
  void SetMatch1Table2(wxString name)
  {
    Match1Table2 = name;
  }
  wxString & GetMatch1Table2()
  {
    return Match1Table2;
  }
  void SetMatch2Table1(wxString name)
  {
    Match2Table1 = name;
  }
  wxString & GetMatch2Table1()
  {
    return Match2Table1;
  }
  void SetMatch2Table2(wxString name)
  {
    Match2Table2 = name;
  }
  wxString & GetMatch2Table2()
  {
    return Match2Table2;
  }
  void SetMatch3Table1(wxString name)
  {
    Match3Table1 = name;
  }
  wxString & GetMatch3Table1()
  {
    return Match3Table1;
  }
  void SetMatch3Table2(wxString name)
  {
    Match3Table2 = name;
  }
  wxString & GetMatch3Table2()
  {
    return Match3Table2;
  }
  void SetWhere1Enabled(bool mode)
  {
    Where1Enabled = mode;
  }
  bool IsWhere1Enabled()
  {
    return Where1Enabled;
  }
  void SetWhere2Enabled(bool mode)
  {
    Where2Enabled = mode;
  }
  bool IsWhere2Enabled()
  {
    return Where2Enabled;
  }
  void SetWhere3Enabled(bool mode)
  {
    Where3Enabled = mode;
  }
  bool IsWhere3Enabled()
  {
    return Where3Enabled;
  }
  void SetWhere1Table2(bool mode)
  {
    Where1Table2 = mode;
  }
  bool IsWhere1Table2()
  {
    return Where1Table2;
  }
  void SetWhere2Table2(bool mode)
  {
    Where2Table2 = mode;
  }
  bool IsWhere2Table2()
  {
    return Where2Table2;
  }
  void SetWhere3Table2(bool mode)
  {
    Where3Table2 = mode;
  }
  bool IsWhere3Table2()
  {
    return Where3Table2;
  }
  void SetAndOr12(bool mode)
  {
    AndOr12 = mode;
  }
  bool IsAndOr12()
  {
    return AndOr12;
  }
  void SetAndOr23(bool mode)
  {
    AndOr23 = mode;
  }
  bool IsAndOr23()
  {
    return AndOr23;
  }
  void SetWhere1Column(wxString name)
  {
    Where1Column = name;
  }
  wxString & GetWhere1Column()
  {
    return Where1Column;
  }
  void SetWhere2Column(wxString name)
  {
    Where2Column = name;
  }
  wxString & GetWhere2Column()
  {
    return Where2Column;
  }
  void SetWhere3Column(wxString name)
  {
    Where3Column = name;
  }
  wxString & GetWhere3Column()
  {
    return Where3Column;
  }
  void SetWhere1Operator(wxString name)
  {
    Where1Operator = name;
  }
  wxString & GetWhere1Operator()
  {
    return Where1Operator;
  }
  void SetWhere2Operator(wxString name)
  {
    Where2Operator = name;
  }
  wxString & GetWhere2Operator()
  {
    return Where2Operator;
  }
  void SetWhere3Operator(wxString name)
  {
    Where3Operator = name;
  }
  wxString & GetWhere3Operator()
  {
    return Where3Operator;
  }
  void SetWhere1Value(wxString name)
  {
    Where1Value = name;
  }
  wxString & GetWhere1Value()
  {
    return Where1Value;
  }
  void SetWhere2Value(wxString name)
  {
    Where2Value = name;
  }
  wxString & GetWhere2Value()
  {
    return Where2Value;
  }
  void SetWhere3Value(wxString name)
  {
    Where3Value = name;
  }
  wxString & GetWhere3Value()
  {
    return Where3Value;
  }
  void SetOrder1Enabled(bool mode)
  {
    Order1Enabled = mode;
  }
  bool IsOrder1Enabled()
  {
    return Order1Enabled;
  }
  void SetOrder2Enabled(bool mode)
  {
    Order2Enabled = mode;
  }
  bool IsOrder2Enabled()
  {
    return Order2Enabled;
  }
  void SetOrder3Enabled(bool mode)
  {
    Order3Enabled = mode;
  }
  bool IsOrder3Enabled()
  {
    return Order3Enabled;
  }
  void SetOrder4Enabled(bool mode)
  {
    Order4Enabled = mode;
  }
  bool IsOrder4Enabled()
  {
    return Order4Enabled;
  }
  void SetOrder1Table2(bool mode)
  {
    Order1Table2 = mode;
  }
  bool IsOrder1Table2()
  {
    return Order1Table2;
  }
  void SetOrder2Table2(bool mode)
  {
    Order2Table2 = mode;
  }
  bool IsOrder2Table2()
  {
    return Order2Table2;
  }
  void SetOrder3Table2(bool mode)
  {
    Order3Table2 = mode;
  }
  bool IsOrder3Table2()
  {
    return Order3Table2;
  }
  void SetOrder4Table2(bool mode)
  {
    Order4Table2 = mode;
  }
  bool IsOrder4Table2()
  {
    return Order4Table2;
  }
  void SetOrder1Column(wxString name)
  {
    Order1Column = name;
  }
  wxString & GetOrder1Column()
  {
    return Order1Column;
  }
  void SetOrder2Column(wxString name)
  {
    Order2Column = name;
  }
  wxString & GetOrder2Column()
  {
    return Order2Column;
  }
  void SetOrder3Column(wxString name)
  {
    Order3Column = name;
  }
  wxString & GetOrder3Column()
  {
    return Order3Column;
  }
  void SetOrder4Column(wxString name)
  {
    Order4Column = name;
  }
  wxString & GetOrder4Column()
  {
    return Order4Column;
  }
  void SetOrder1Desc(bool mode)
  {
    Order1Desc = mode;
  }
  bool IsOrder1Desc()
  {
    return Order1Desc;
  }
  void SetOrder2Desc(bool mode)
  {
    Order2Desc = mode;
  }
  bool IsOrder2Desc()
  {
    return Order2Desc;
  }
  void SetOrder3Desc(bool mode)
  {
    Order3Desc = mode;
  }
  bool IsOrder3Desc()
  {
    return Order3Desc;
  }
  void SetOrder4Desc(bool mode)
  {
    Order4Desc = mode;
  }
  bool IsOrder4Desc()
  {
    return Order4Desc;
  }
  void SetPlainView(bool mode)
  {
    PlainView = mode;
  }
  bool IsPlainView()
  {
    return PlainView;
  }
  void SetSpatialView(bool mode)
  {
    SpatialView = mode;
  }
  bool IsSpatialView()
  {
    return SpatialView;
  }
  void SetViewName(wxString name)
  {
    ViewName = name;
  }
  wxString & GetViewName()
  {
    return ViewName;
  }
  void SetViewGeomTable2(bool mode)
  {
    ViewGeomTable2 = mode;
  }
  bool IsViewGeomTable2()
  {
    return ViewGeomTable2;
  }
  void SetGeometryColumn(wxString name)
  {
    GeometryColumn = name;
  }
  wxString & GetGeometryColumn()
  {
    return GeometryColumn;
  }
  void SetWritable1(bool value)
  {
    Writable1 = value;
  }
  void SetWritable2(bool value)
  {
    Writable2 = value;
  }
  bool IsWritable1()
  {
    return Writable1;
  }
  bool IsWritable2()
  {
    return Writable2;
  }
  bool IsDuplicateAlias(wxString & alias);
  void SetAliases();
  wxString & GetGeometryColumnAlias()
  {
    return GeometryColumnAlias;
  }
  wxString & GetGeometryRowidAlias()
  {
    return GeometryRowidAlias;
  }
  wxString & GetSqlSample()
  {
    return SqlSample;
  }
  wxString & GetSqlTriggerInsert()
  {
    return SqlTriggerInsert;
  }
  wxString & GetSqlTriggerUpdate()
  {
    return SqlTriggerUpdate;
  }
  wxString & GetSqlTriggerDelete()
  {
    return SqlTriggerDelete;
  }
  wxNotebook *GetTabCtrl()
  {
    return TabCtrl;
  }
  void Table1Status(bool ok);
  void Table2Status(bool ok);
  bool SqlCleanString(wxString & dirty, wxString & clean);
  void SqlCleanList(wxString & list, wxString & clean, int *style, int *start,
                    int *stop, int *next, int base);
  void SelectGeometryColumn();
  void UpdateSqlSample();
  void PrepareSqlTriggers();
  void OnOk(wxCommandEvent & event);
};

class GeomColumn
{
//
// a class representing a Geometry Column
//
private:
  wxString GeometryName;
  wxString GeometryType;
  wxString CoordDims;
  int Srid;
  bool RTree;
  bool MbrCache;
  bool NotNull;
  GeomColumn *Next;
public:
    GeomColumn(wxString & name, wxString & type, wxString & dims, int srid,
               int idx);
   ~GeomColumn()
  {;
  }
  wxString & GetGeometryName()
  {
    return GeometryName;
  }
  wxString & GetGeometryType()
  {
    return GeometryType;
  }
  wxString & GetCoordDims()
  {
    return CoordDims;
  }
  int GetSrid()
  {
    return Srid;
  }
  bool IsRTree()
  {
    return RTree;
  }
  bool IsMbrCache()
  {
    return MbrCache;
  }
  void SetNotNull()
  {
    NotNull = true;
  }
  bool IsNotNull()
  {
    return NotNull;
  }
  void SetNext(GeomColumn * next)
  {
    Next = next;
  }
  GeomColumn *GetNext()
  {
    return Next;
  }
};

class GeomColsList
{
//
// a class representing a Geometry Columns list
//
private:
  GeomColumn * First;
  GeomColumn *Last;
public:
    GeomColsList();
   ~GeomColsList();
  void Add(wxString & name, wxString & type, wxString & dims, int srid,
           int idx);
  GeomColumn *GetFirst()
  {
    return First;
  }
  void SetNotNull(wxString & geom);
};

class IndexColumn
{
//
// a class representing an Index Column
//
private:
  wxString ColumnName;
  bool Valid;
  IndexColumn *Next;
public:
    IndexColumn(wxString & name);
   ~IndexColumn()
  {;
  }
  wxString & GetColumnName()
  {
    return ColumnName;
  }
  bool IsValid()
  {
    return Valid;
  }
  void Invalidate()
  {
    Valid = false;
  }
  void SetNext(IndexColumn * next)
  {
    Next = next;
  }
  IndexColumn *GetNext()
  {
    return Next;
  }
};

class TblIndex
{
//
// a class representing a Table Index
//
private:
  wxString IndexName;
  bool Unique;
  bool Valid;
  IndexColumn *First;
  IndexColumn *Last;
  TblIndex *Next;
public:
    TblIndex(wxString & name, bool unique);
   ~TblIndex();
  void Add(wxString & column);
    wxString & GetIndexName()
  {
    return IndexName;
  }
  bool IsUnique()
  {
    return Unique;
  }
  void Invalidate(wxString & colName);
  bool IsValid()
  {
    return Valid;
  }
  void SetNext(TblIndex * next)
  {
    Next = next;
  }
  TblIndex *GetNext()
  {
    return Next;
  }
  IndexColumn *GetFirst()
  {
    return First;
  }
};

class TblIndexList
{
//
// a class representing a Table Index list
//
private:
  TblIndex * First;
  TblIndex *Last;
public:
    TblIndexList();
   ~TblIndexList();
  void Add(wxString & name, bool unique);
  TblIndex *GetFirst()
  {
    return First;
  }
  void Invalidate(wxString & colName);
};

class ResultSetShapefileGeometry
{
//
// a class wrapping a (possible) Shapefile Geometry
//
private:
  int Type;
  int Dims;
  int Srid;
  int Count;
  ResultSetShapefileGeometry *Next;
public:
    ResultSetShapefileGeometry(int type, int dims, int srid)
  {
    Type = type;
    Dims = dims;
    Srid = srid;
    Count = 1;
    Next = NULL;
  }
   ~ResultSetShapefileGeometry()
  {;
  }
  int GetType()
  {
    return Type;
  }
  int GetDims()
  {
    return Dims;
  }
  int GetSrid()
  {
    return Srid;
  }
  int GetCount()
  {
    return Count;
  }
  void Update()
  {
    Count++;
  }
  void SetNext(ResultSetShapefileGeometry * next)
  {
    Next = next;
  }
  ResultSetShapefileGeometry *GetNext()
  {
    return Next;
  }
};

class ResultSetShapefileColumn
{
//
// a class wrapping a (possible) Shapefile column
//
private:
  char *Name;
  int NullCount;
  int TextCount;
  int MaxTextLen;
  int IntCount;
  int DoubleCount;
  int BlobCount;
  int DbfType;
  ResultSetShapefileGeometry *First;
  ResultSetShapefileGeometry *Last;
public:
    ResultSetShapefileColumn();
   ~ResultSetShapefileColumn();
  void SetName(const char *name);
  char *GetName()
  {
    return Name;
  }
  void UpdateNull()
  {
    NullCount++;
  }
  void UpdateText(int len)
  {
    TextCount++;
    if (len > MaxTextLen)
      MaxTextLen = len;
  }
  void UpdateInteger()
  {
    IntCount++;
  }
  void UpdateDouble()
  {
    DoubleCount++;
  }
  void UpdateBlob()
  {
    BlobCount++;
  }
  void UpdateGeometry(gaiaGeomCollPtr geom);
  bool Validate();
  int GetDbfType()
  {
    return DbfType;
  }
  int GetMaxTextLen()
  {
    return MaxTextLen;
  }
  ResultSetShapefileGeometry *GetFirst()
  {
    return First;
  }

};

class ResultSetShapefileAnalyzer
{
//
// a class representing a (possible) Shapefile 
// corresponding to some generic ResultSet
//
private:
  int ColumnCount;
  int GeometryColumn;
  ResultSetShapefileColumn *Columns;
public:
    ResultSetShapefileAnalyzer()
  {
    ColumnCount = 0;
    GeometryColumn = -1;
    Columns = NULL;
  }
   ~ResultSetShapefileAnalyzer();
  bool Validate();
  void SetColumnName(int column, const char *name);
  void Init(int count);
  int GetColumnCount()
  {
    return ColumnCount;
  }
  int GetGeometryColumn()
  {
    return GeometryColumn;
  }
  void UpdateNull(int column);
  void UpdateText(int column, int len);
  void UpdateInteger(int column);
  void UpdateDouble(int column);
  void UpdateGeometry(int column, gaiaGeomCollPtr geom);
  void UpdateBlob(int column);
  ResultSetShapefileColumn *GetColumn(int column);
  ResultSetShapefileColumn *GetGeometry();
};

class PostGISColumn
{
//
// a class wrapping a PostGIS column
//
private:
  wxString ColumnName;          // the column name
  bool PrimaryKey;              // Primary Key column
  bool Autoincrement;           // Autoincrement Primary Key
  bool Nullable;                // IS NULL
  int Null;
  int Boolean;
  int Int8;
  int UInt8;
  int Int16;
  int UInt16;
  int Int32;
  int UInt32;
  int Int64;
  int Double;
  int Text;
  int MaxTextLen;
  int Date;
  int DateTime;
  int Blob;
  int Point;
  int MultiPoint;
  int LineString;
  int MultiLineString;
  int Polygon;
  int MultiPolygon;
  int GeometryCollection;
  int Srid1;
  int Srid2;
  int CoordDims1;
  int CoordDims2;
  int DataType;
public:
    PostGISColumn();
   ~PostGISColumn()
  {;
  }
  void SetName(wxString & name)
  {
    ColumnName = name;
  }
  wxString & GetName()
  {
    return ColumnName;
  }
  void SetNotNull()
  {
    Nullable = false;
  }
  bool IsNotNull()
  {
    if (Nullable == true)
      return false;
    else
      return true;
  }
  void SetPrimaryKey()
  {
    PrimaryKey = true;
  }
  bool IsPrimaryKey()
  {
    return PrimaryKey;
  }
  void IncrNull()
  {
    Null++;
  }
  void IncrBoolean()
  {
    Boolean++;
  }
  void IncrInt8()
  {
    Int8++;
  }
  void IncrUInt8()
  {
    UInt8++;
  }
  void IncrInt16()
  {
    Int16++;
  }
  void IncrUInt16()
  {
    UInt16++;
  }
  void IncrInt32()
  {
    Int32++;
  }
  void IncrUInt32()
  {
    UInt32++;
  }
  void IncrInt64()
  {
    Int64++;
  }
  void IncrDouble()
  {
    Double++;
  }
  void IncrText(int len)
  {
    Text++;
    if (len > MaxTextLen)
      MaxTextLen = len;
  }
  void IncrDate()
  {
    Date++;
  }
  void IncrDateTime()
  {
    DateTime++;
  }
  void IncrBlob()
  {
    Blob++;
  }
  void IncrPoint(int srid, int coord_dims);
  void IncrMultiPoint(int srid, int coord_dims);
  void IncrLineString(int srid, int coord_dims);
  void IncrMultiLineString(int srid, int coord_dims);
  void IncrPolygon(int srid, int coord_dims);
  void IncrMultiPolygon(int srid, int coord_dims);
  void IncrGeometryCollection(int srid, int coord_dims);
  bool IsDate(const char *txt);
  bool IsDateTime(const char *txt);
  int GetMaxTextLen()
  {
    return MaxTextLen;
  }
  bool IsGeometry();
  int GetDataType()
  {
    return DataType;
  }
  void Prepare();
  int GetSrid()
  {
    return Srid1;
  }
  int GetCoordDims()
  {
    return CoordDims1;
  }
};

class PostGISIndexField
{
//
// a class wrapping a PostGIS Index field
private:
  int SeqNo;
  PostGISColumn *ColumnRef;
  PostGISIndexField *Next;
public:
    PostGISIndexField(int seq, PostGISColumn * col)
  {
    SeqNo = seq;
    ColumnRef = col;
    Next = NULL;
  }
   ~PostGISIndexField()
  {;
  }
  int GetSeqNo()
  {
    return SeqNo;
  }
  PostGISColumn *GetColumnRef()
  {
    return ColumnRef;
  }
  void SetNext(PostGISIndexField * next)
  {
    Next = next;
  }
  PostGISIndexField *GetNext()
  {
    return Next;
  }
};

class PostGISIndex
{
//
// a class wrapping a PostGIS Index
private:
  bool PrimaryKey;
  bool Unique;
  wxString Name;
  PostGISIndexField *First;
  PostGISIndexField *Last;
  PostGISIndex *Next;
public:
    PostGISIndex(wxString & name)
  {
    Name = name;
    PrimaryKey = true;
    Unique = true;
    First = NULL;
    Last = NULL;
    Next = NULL;
  }
  PostGISIndex(wxString & name, bool unique)
  {
    Name = name;
    PrimaryKey = false;
    Unique = unique;
    First = NULL;
    Last = NULL;
    Next = NULL;
  }
  ~PostGISIndex();
  wxString & GetName()
  {
    return Name;
  }
  bool IsPrimaryKey()
  {
    return PrimaryKey;
  }
  bool IsUnique()
  {
    return Unique;
  }
  void AddField(int seq, PostGISColumn * column);
  PostGISIndexField *GetFirst()
  {
    return First;
  }
  void SetNext(PostGISIndex * next)
  {
    Next = next;
  }
  PostGISIndex *GetNext()
  {
    return Next;
  }
};

class PostGISHelper
{
//
// a class wrapping a PostGIS table
//
private:
  wxString DumbName;
  int Count;                    // how many columns
  PostGISColumn *Columns;       // array of columns
  PostGISIndex *FirstIdx;
  PostGISIndex *LastIdx;
  bool Autoincrement;
public:
    PostGISHelper();
   ~PostGISHelper();
  void Alloc(int count);
  int GetCount()
  {
    return Count;
  }
  PostGISIndex *AddIndex(wxString & name, bool unique);
  PostGISIndex *AddIndex(wxString & name);
  PostGISColumn *Find(wxString & name);
  PostGISIndex *GetFirstIndex()
  {
    return FirstIdx;
  }
  void ExpandIndexFields(MyFrame * mother, PostGISIndex * index,
                         wxString & name);
  bool IsSingleFieldPrimaryKey();
  bool IsAutoincrement()
  {
    return Autoincrement;
  }
  void SetName(int pos, const char *name);
  void Eval(int pos, sqlite3_int64 val);
  void Eval(int pos, double val);
  void Eval(int pos, const char *val);
  void Eval(int pos, gaiaGeomCollPtr geom);
  void EvalBlob(int pos);
  void Eval(int pos);
  wxString & GetName(int pos, bool to_lower);
  bool IsGeometry(int pos);
  int GetDataType(int pos);
  void GetDataType(int pos, char *definition);
  int GetSrid(int pos);
  int GetCoordDims(int pos);
  void SetColumn(wxString & name, bool isNull, bool pKey);
  void GetKeys(MyFrame * mother, wxString & table);
  void Prepare();
  void OutputBooleanValue(FILE * out, sqlite3_int64 value);
  void OutputValue(FILE * out, sqlite3_int64 value);
  void OutputValue(FILE * out, double value);
  void OutputValue(FILE * out, const char *value);
  void OutputValue(FILE * out, gaiaGeomCollPtr value);
  void OutputValue(FILE * out, const unsigned char *value, int len);
  enum
  {
    // data types constants
    DATA_TYPE_UNDEFINED = 0,
    DATA_TYPE_BOOLEAN,
    DATA_TYPE_INT8,
    DATA_TYPE_UINT8,
    DATA_TYPE_INT16,
    DATA_TYPE_UINT16,
    DATA_TYPE_INT32,
    DATA_TYPE_UINT32,
    DATA_TYPE_INT64,
    DATA_TYPE_UINT64,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_TEXT,
    DATA_TYPE_DATE,
    DATA_TYPE_DATETIME,
    DATA_TYPE_BLOB,
    DATA_TYPE_POINT,
    DATA_TYPE_LINESTRING,
    DATA_TYPE_POLYGON,
    DATA_TYPE_MULTIPOINT,
    DATA_TYPE_MULTILINESTRING,
    DATA_TYPE_MULTIPOLYGON,
    DATA_TYPE_GEOMETRYCOLLECTION,
    DATA_TYPE_GEOMETRY
  };
};
