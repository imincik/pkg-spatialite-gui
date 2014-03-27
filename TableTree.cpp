/*
/ TableTree.cpp
/ tree control to show tables, columns, indices and triggers
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

#include <float.h>

#include "Classdef.h"

#include "wx/filename.h"
#include "wx/imaglist.h"

#if defined(_WIN32) && !defined(__MINGW32__)
#define strcasecmp	_stricmp
#endif

//
// ICONs in XPM format [universally portable]
//
#include "icons/db.xpm"
#include "icons/table.xpm"
#include "icons/geotable.xpm"
#include "icons/vtable.xpm"
#include "icons/geovtable.xpm"
#include "icons/composer.xpm"
#include "icons/view.xpm"
#include "icons/geoview.xpm"
#include "icons/pkey.xpm"
#include "icons/column.xpm"
#include "icons/index.xpm"
#include "icons/trigger.xpm"
#include "icons/geometry.xpm"
#include "icons/spatialidx.xpm"
#include "icons/mbrcache.xpm"
#include "icons/kill_spindex.xpm"
#include "icons/dumpshp.xpm"
#include "icons/tmp_table.xpm"
#include "icons/tmp_view.xpm"
#include "icons/malformed_geoms.xpm"
#include "icons/statistics.xpm"
#include "icons/map_preview.xpm"
#include "icons/tables.xpm"
#include "icons/foreign_key.xpm"
#include "icons/primary_key.xpm"
#include "icons/topology.xpm"
#include "icons/attach.xpm"
#include "icons/checkgeom.xpm"
#include "icons/sanegeom.xpm"

MyTableTree::MyTableTree(MyFrame * parent, wxWindowID id):wxTreeCtrl(parent, id)
{
//
// constructor: TREE control to show DB objects
//
  MainFrame = parent;
  Root = AddRoot(wxT("no current DB"));
  RootUserData = AppendItem(Root, wxT("User Data"));
  RootIsoMetadata = AppendItem(Root, wxT("ISO / INSPIRE Metadata"));
  RootStyling = AppendItem(Root, wxT("Styling (SLD/SE)"));
  RootTopologies = AppendItem(Root, wxT("Topologies"));
  RootMetadata = AppendItem(Root, wxT("Metadata"));
  RootInternal = AppendItem(Root, wxT("Internal Data"));
  RootSpatialIndex = AppendItem(Root, wxT("Spatial Index"));
// setting up icons 
  Images = new wxImageList(16, 16, true);
  wxIcon icons[22];
  icons[0] = wxIcon(db_xpm);
  icons[1] = wxIcon(table_xpm);
  icons[2] = wxIcon(pkey_xpm);
  icons[3] = wxIcon(column_xpm);
  icons[4] = wxIcon(index_xpm);
  icons[5] = wxIcon(trigger_xpm);
  icons[6] = wxIcon(geometry_xpm);
  icons[7] = wxIcon(spatialidx_xpm);
  icons[8] = wxIcon(vtable_xpm);
  icons[9] = wxIcon(view_xpm);
  icons[10] = wxIcon(geotable_xpm);
  icons[11] = wxIcon(mbrcache_xpm);
  icons[12] = wxIcon(geoview_xpm);
  icons[13] = wxIcon(geovtable_xpm);
  icons[14] = wxIcon(tmp_table_xpm);
  icons[15] = wxIcon(tmp_view_xpm);
  icons[16] = wxIcon(malformed_geoms_xpm);
  icons[17] = wxIcon(tables_xpm);
  icons[18] = wxIcon(foreign_key_xpm);
  icons[19] = wxIcon(primary_key_xpm);
  icons[20] = wxIcon(topology_xpm);
  icons[21] = wxIcon(attach_xpm);
  Images->Add(icons[0]);
  Images->Add(icons[1]);
  Images->Add(icons[2]);
  Images->Add(icons[3]);
  Images->Add(icons[4]);
  Images->Add(icons[5]);
  Images->Add(icons[6]);
  Images->Add(icons[7]);
  Images->Add(icons[8]);
  Images->Add(icons[9]);
  Images->Add(icons[10]);
  Images->Add(icons[11]);
  Images->Add(icons[12]);
  Images->Add(icons[13]);
  Images->Add(icons[14]);
  Images->Add(icons[15]);
  Images->Add(icons[16]);
  Images->Add(icons[17]);
  Images->Add(icons[18]);
  Images->Add(icons[19]);
  Images->Add(icons[20]);
  Images->Add(icons[21]);
  SetImageList(Images);
  SetItemImage(Root, 0);
  SetItemImage(RootUserData, 17);
  SetItemImage(RootTopologies, 17);
  SetItemImage(RootStyling, 17);
  SetItemImage(RootIsoMetadata, 17);
  SetItemImage(RootMetadata, 17);
  SetItemImage(RootInternal, 17);
  SetItemImage(RootSpatialIndex, 17);
// setting up event handlers 
  Connect(wxID_ANY, wxEVT_COMMAND_TREE_SEL_CHANGED,
          (wxObjectEventFunction) & MyTableTree::OnSelChanged);
  Connect(wxID_ANY, wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,
          (wxObjectEventFunction) & MyTableTree::OnRightClick);
  Connect(Tree_QueryViewComposer, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdQueryViewComposer);
  Connect(Tree_NewTable, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdNewTable);
  Connect(Tree_NewView, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdNewView);
  Connect(Tree_NewIndex, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdNewIndex);
  Connect(Tree_NewTrigger, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdNewTrigger);
  Connect(Tree_NewColumn, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdNewColumn);
  Connect(Tree_Show, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdShow);
  Connect(Tree_Drop, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDrop);
  Connect(Tree_Detach, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDetachDB);
  Connect(Tree_Rename, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRename);
  Connect(Tree_Select, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdSelect);
  Connect(Tree_Refresh, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRefresh);
  Connect(Tree_Recover, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRecover);
  Connect(Tree_ShowSql, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdShowSql);
  Connect(Tree_SpatialIndex, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdSpatialIndex);
  Connect(Tree_CheckSpatialIndex, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdCheckSpatialIndex);
  Connect(Tree_RecoverSpatialIndex, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRecoverSpatialIndex);
  Connect(Tree_MbrCache, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdMbrCache);
  Connect(Tree_RebuildTriggers, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRebuildTriggers);
  Connect(Tree_GisLayerAuth, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdGisLayerAuth);
  Connect(Tree_CheckGeometry, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdCheckGeometry);
  Connect(Tree_Extent, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdExtent);
  Connect(Tree_UpdateLayerStatistics, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdUpdateLayerStatistics);
  Connect(Tree_UpdateLayerStatisticsAll, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdUpdateLayerStatisticsAll);
  Connect(Tree_ElementaryGeoms, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdElementaryGeometries);
  Connect(Tree_MalformedGeometries, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdMalformedGeometries);
  Connect(Tree_RepairPolygons, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRepairPolygons);
  Connect(Tree_SetSrid, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdSetSrid);
  Connect(Tree_DumpShp, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpShp);
  Connect(Tree_DumpKml, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpKml);
  Connect(Tree_DumpTxtTab, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpTxtTab);
  Connect(Tree_DumpCsv, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpCsv);
  Connect(Tree_DumpHtml, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpHtml);
  Connect(Tree_DumpDif, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpDif);
  Connect(Tree_DumpSylk, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpSylk);
  Connect(Tree_DumpDbf, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpDbf);
  Connect(Tree_DumpPostGIS, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDumpPostGIS);
  Connect(Tree_Edit, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdEdit);
  Connect(Tree_DropColumn, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdDropColumn);
  Connect(Tree_RenameColumn, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRenameColumn);
  Connect(Tree_ColumnStats, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdColumnStats);
  Connect(Tree_MapPreview, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdMapPreview);
  Connect(Tree_CheckDuplicates, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdCheckDuplicates);
  Connect(Tree_RemoveDuplicates, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdRemoveDuplicates);
  Connect(Tree_CheckGeom, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdCheckGeometries);
  Connect(Tree_SaneGeom, wxEVT_COMMAND_MENU_SELECTED,
          (wxObjectEventFunction) & MyTableTree::OnCmdSanitizeGeometries);
}

void MyTableTree::FlushAll()
{
// resetting to the initial empty state
  wxTreeItemId child;
  wxTreeItemIdValue cookie;
  bool ok = true;
  DeleteChildren(RootUserData);
  DeleteTopologies(RootTopologies);
  DeleteChildren(RootStyling);
  DeleteChildren(RootIsoMetadata);
  DeleteChildren(RootMetadata);
  DeleteChildren(RootInternal);
  DeleteChildren(RootSpatialIndex);
  while (ok)
    {
      ok = false;
      child = GetFirstChild(Root, cookie);
      while (true)
        {
          bool kill = true;
          if (child.IsOk() == false)
            break;
          if (child == RootUserData)
            kill = false;
          if (child == RootTopologies)
            kill = false;
          if (child == RootStyling)
            kill = false;
          if (child == RootIsoMetadata)
            kill = false;
          if (child == RootMetadata)
            kill = false;
          if (child == RootInternal)
            kill = false;
          if (child == RootSpatialIndex)
            kill = false;
          if (kill == true)
            {
              Delete(child);
              ok = true;
              break;
            }
          child = GetNextChild(Root, cookie);
        }
    }
}

void MyTableTree::DeleteTopologies(wxTreeItemId & RootTopologies)
{
// deleting Topology Nodes
  Topologies.Flush();
  DeleteChildren(RootTopologies);
}

wxTreeItemId & MyTableTree::GetRootNode(wxString & tableName)
{
//
// determines the Table Root Node 
//
  if (tableName == wxT("geometry_columns"))
    return RootMetadata;
  if (tableName == wxT("views_geometry_columns"))
    return RootMetadata;
  if (tableName == wxT("virts_geometry_columns"))
    return RootMetadata;
  if (tableName == wxT("spatial_ref_sys"))
    return RootMetadata;
  if (tableName == wxT("geom_cols_ref_sys"))
    return RootMetadata;
  if (tableName == wxT("geometry_columns_time"))
    return RootMetadata;
  if (tableName == wxT("spatialite_history"))
    return RootMetadata;
  if (tableName == wxT("raster_coverages"))
    return RootMetadata;
  if (tableName == wxT("raster_coverages_ref_sys"))
    return RootMetadata;
  if (tableName == wxT("vector_layers"))
    return RootMetadata;
  if (tableName == wxT("topology_master"))
    return RootMetadata;

  if (tableName == wxT("sqlite_stat1"))
    return RootInternal;
  if (tableName == wxT("sqlite_stat3"))
    return RootInternal;
  if (tableName == wxT("sqlite_sequence"))
    return RootInternal;
  if (tableName == wxT("layer_params"))
    return RootInternal;
  if (tableName == wxT("layer_statistics"))
    return RootInternal;
  if (tableName == wxT("geometry_columns_statistics"))
    return RootInternal;
  if (tableName == wxT("views_layer_statistics"))
    return RootInternal;
  if (tableName == wxT("views_geometry_columns_statistics"))
    return RootInternal;
  if (tableName == wxT("virts_layer_statistics"))
    return RootInternal;
  if (tableName == wxT("virts_geometry_columns_statistics"))
    return RootInternal;
  if (tableName == wxT("geometry_columns_field_infos"))
    return RootInternal;
  if (tableName == wxT("views_geometry_columns_field_infos"))
    return RootInternal;
  if (tableName == wxT("virts_geometry_columns_field_infos"))
    return RootInternal;
  if (tableName == wxT("geometry_columns_auth"))
    return RootInternal;
  if (tableName == wxT("views_geometry_columns_auth"))
    return RootInternal;
  if (tableName == wxT("virts_geometry_columns_auth"))
    return RootInternal;
  if (tableName == wxT("vector_layers_auth"))
    return RootInternal;
  if (tableName == wxT("vector_layers_statistics"))
    return RootInternal;
  if (tableName == wxT("vector_layers_field_infos"))
    return RootInternal;
  if (tableName == wxT("layer_sub_classes"))
    return RootInternal;
  if (tableName == wxT("layer_table_layout"))
    return RootInternal;
  if (tableName == wxT("pattern_bitmaps"))
    return RootInternal;
  if (tableName == wxT("symbol_bitmaps"))
    return RootInternal;
  if (tableName == wxT("project_defs"))
    return RootInternal;
  if (tableName == wxT("raster_pyramids"))
    return RootInternal;
  if (tableName == wxT("rasterlite2_styles"))
    return RootInternal;
  if (tableName == wxT("rasterlite2_metadata"))
    return RootInternal;
  if (tableName == wxT("sql_statements_log"))
    return RootInternal;

  if (tableName == wxT("SE_external_graphics"))
    return RootStyling;
  if (tableName == wxT("SE_external_graphics_view"))
    return RootStyling;
  if (tableName == wxT("SE_raster_styled_layers"))
    return RootStyling;
  if (tableName == wxT("SE_raster_styled_layers_view"))
    return RootStyling;
  if (tableName == wxT("SE_vector_styled_layers"))
    return RootStyling;
  if (tableName == wxT("SE_vector_styled_layers_view"))
    return RootStyling;
  if (tableName == wxT("SE_styled_groups"))
    return RootStyling;
  if (tableName == wxT("SE_styled_group_refs"))
    return RootStyling;
  if (tableName == wxT("SE_styled_groups_view"))
    return RootStyling;

  if (tableName == wxT("ISO_metadata"))
    return RootIsoMetadata;
  if (tableName == wxT("ISO_metadata_reference"))
    return RootIsoMetadata;
  if (tableName == wxT("ISO_metadata_view"))
    return RootIsoMetadata;

  if (tableName == wxT("SpatialIndex"))
    return RootSpatialIndex;
  if (MainFrame->IsSpatialIndex(tableName) == true)
    return RootSpatialIndex;
  wxTreeItemId *topologyNode = Topologies.FindNode(tableName);
  if (topologyNode != NULL)
    return *topologyNode;
  return RootUserData;
}

wxTreeItemId & MyTableTree::GetRootNode(wxString & tableName, RootNodes * nodes)
{
//
// determines the Table Root Node [Attached DB]
//
  if (tableName == wxT("geometry_columns"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("views_geometry_columns"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("virts_geometry_columns"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("spatial_ref_sys"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("geom_cols_ref_sys"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("geometry_columns_time"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("spatialite_history"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("vector_layers"))
    return nodes->GetRootMetadata();
  if (tableName == wxT("topology_master"))
    return nodes->GetRootMetadata();

  if (tableName == wxT("sqlite_stat1"))
    return nodes->GetRootInternal();
  if (tableName == wxT("sqlite_sequence"))
    return nodes->GetRootInternal();
  if (tableName == wxT("layer_params"))
    return nodes->GetRootInternal();
  if (tableName == wxT("layer_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("geometry_columns_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("views_layer_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("views_geometry_columns_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("virts_layer_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("virts_geometry_columns_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("geometry_columns_field_infos"))
    return nodes->GetRootInternal();
  if (tableName == wxT("views_geometry_columns_field_infos"))
    return nodes->GetRootInternal();
  if (tableName == wxT("virts_geometry_columns_field_infos"))
    return nodes->GetRootInternal();
  if (tableName == wxT("geometry_columns_auth"))
    return nodes->GetRootInternal();
  if (tableName == wxT("views_geometry_columns_auth"))
    return nodes->GetRootInternal();
  if (tableName == wxT("virts_geometry_columns_auth"))
    return nodes->GetRootInternal();
  if (tableName == wxT("vector_layers_auth"))
    return nodes->GetRootInternal();
  if (tableName == wxT("vector_layers_statistics"))
    return nodes->GetRootInternal();
  if (tableName == wxT("vector_layers_field_infos"))
    return nodes->GetRootInternal();
  if (tableName == wxT("layer_sub_classes"))
    return nodes->GetRootInternal();
  if (tableName == wxT("layer_table_layout"))
    return nodes->GetRootInternal();
  if (tableName == wxT("pattern_bitmaps"))
    return nodes->GetRootInternal();
  if (tableName == wxT("symbol_bitmaps"))
    return nodes->GetRootInternal();
  if (tableName == wxT("project_defs"))
    return nodes->GetRootInternal();
  if (tableName == wxT("raster_pyramids"))
    return nodes->GetRootInternal();
  if (tableName == wxT("rasterlite2_styles"))
    return nodes->GetRootInternal();
  if (tableName == wxT("rasterlite2_metadata"))
    return nodes->GetRootInternal();
  if (tableName == wxT("sql_statements_log"))
    return nodes->GetRootInternal();

  if (tableName == wxT("SpatialIndex"))
    return nodes->GetRootSpatialIndex();
  if (MainFrame->IsSpatialIndex(nodes->GetDbAlias(), tableName) == true)
    return nodes->GetRootSpatialIndex();
  wxTreeItemId *topologyNode = Topologies.FindNode(tableName);
  if (topologyNode != NULL)
    return *topologyNode;
  return nodes->GetRootUserData();
}

void MyTableTree::AddTable(wxString & tableName, bool virtualTable, bool tmp)
{
//
// appends a table to the TREE list
//
  MyTableInfo list;
  MyColumnInfo *col;
  MyIndexInfo *idx;
  MyTriggerInfo *trgr;
  wxTreeItemId item;
  wxTreeItemId item2;
  wxString columnInfo;
  wxString indexInfo;
  wxString triggerInfo;
  int icon = 1;
  if (virtualTable == true)
    icon = 8;
  if (tmp == true)
    icon = 14;
  wxTreeItemId rootNode = GetRootNode(tableName);
  item = AppendItem(rootNode, tableName, icon);
  if (virtualTable == true)
    SetItemData(item,
                (wxTreeItemData *) (new MyObject(MY_VTABLE, tableName, tmp)));
  else
    SetItemData(item,
                (wxTreeItemData *) (new MyObject(MY_TABLE, tableName, tmp)));
  MainFrame->GetTableColumns(tableName, &list);
  MainFrame->GetTableIndices(tableName, &list);
  MainFrame->GetTableTriggers(tableName, &list);
  col = list.GetFirstColumn();
  while (col)
    {
      int icon;
      if (col->IsPrimaryKey() == true)
        icon = 2;
      else
        {
          if (col->IsGeometry() == true)
            {
              if (virtualTable == false)
                SetItemImage(item, 10);
              else
                SetItemImage(item, 13);
              if (col->IsGeometryIndex() == true)
                icon = 7;
              else if (col->IsMbrCache() == true)
                icon = 11;
              else
                icon = 6;
          } else
            icon = 3;
        }
      columnInfo = col->GetName();
      item2 = AppendItem(item, columnInfo, icon);
      if (virtualTable == true)
        {
          if (col->IsGeometry() == true)
            {
              SetItemData(item2,
                          (wxTreeItemData *) (new
                                              MyObject(MY_VIRTUAL_GEOMETRY,
                                                       tableName,
                                                       col->GetName())));
          } else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIRTUAL_COLUMN,
                                                     tableName,
                                                     col->GetName())));
      } else
        {
          if (col->IsGeometry() == true)
            {
              if (col->IsGeometryIndex() == true)
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY_INDEX,
                                                         tableName,
                                                         col->GetName())));
              else if (col->IsMbrCache() == true)
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY_CACHED,
                                                         tableName,
                                                         col->GetName())));
              else
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY, tableName,
                                                         col->GetName())));
          } else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_COLUMN, tableName,
                                                     col->GetName())));
        }
      col = col->GetNext();
    }
// setting up Primary Key
  bool ok_pk = false;
  idx = list.GetFirstIndex();
  while (idx)
    {
      indexInfo = idx->GetName();
      if (indexInfo.StartsWith(wxT("sqlite_autoindex_")) == true)
        {
          item2 = AppendItem(item, wxT("PrimaryKey"), 19);
          MainFrame->GetPrimaryKeyFields(indexInfo, item2);
          ok_pk = true;
          break;
        }
      idx = idx->GetNext();
    }
  if (ok_pk == false)
    {
      col = list.GetFirstColumn();
      while (col)
        {
          if (col->IsPrimaryKey() == true)
            {
              ok_pk = true;
              break;
            }
          col = col->GetNext();
        }
      if (ok_pk == true)
        {
          item2 = AppendItem(item, wxT("PrimaryKey"), 19);
          col = list.GetFirstColumn();
          while (col)
            {
              if (col->IsPrimaryKey() == true)
                {
                  AppendItem(item2, col->GetName(), 2);
                  break;
                }
              col = col->GetNext();
            }
        }
    }
// setting up Foreign Keys
  MainFrame->GetForeignKeys(tableName, item);
  idx = list.GetFirstIndex();
  while (idx)
    {
      // setting up Indices
      indexInfo = idx->GetName();
      if (indexInfo.StartsWith(wxT("sqlite_autoindex_")) == true)
        {
          idx = idx->GetNext();
          continue;
        }
      item2 = AppendItem(item, indexInfo, 4);
      SetItemData(item2,
                  (wxTreeItemData *) (new MyObject(MY_INDEX, idx->GetName())));
      MainFrame->GetIndexFields(indexInfo, item2);
      idx = idx->GetNext();
    }
  trgr = list.GetFirstTrigger();
  while (trgr)
    {
      // setting up Triggers
      triggerInfo = trgr->GetName();
      item2 = AppendItem(item, triggerInfo, 5);
      SetItemData(item2,
                  (wxTreeItemData *) (new
                                      MyObject(MY_TRIGGER, trgr->GetName())));
      trgr = trgr->GetNext();
    }
}

void MyTableTree::AddView(wxString & viewName, bool tmp)
{
//
// appends a view to the TREE list
  MyViewInfo list;
  MyColumnInfo *col;
  MyTriggerInfo *trgr;
  wxTreeItemId item;
  wxTreeItemId item2;
  wxString columnInfo;
  wxString triggerInfo;
  int icon = 9;
  if (tmp == true)
    icon = 15;
  wxTreeItemId rootNode = GetRootNode(viewName);
  item = AppendItem(rootNode, viewName, icon);
  SetItemData(item, (wxTreeItemData *) (new MyObject(MY_VIEW, viewName, tmp)));
  MainFrame->GetViewColumns(viewName, &list);
  MainFrame->GetViewTriggers(viewName, &list);
  col = list.GetFirst();
  while (col)
    {
      columnInfo = col->GetName();
      if (col->IsGeometry() == true)
        {
          SetItemImage(item, 12);
          if (col->IsGeometryIndex() == true)
            icon = 7;
          else if (col->IsMbrCache() == true)
            icon = 11;
          else
            icon = 6;
      } else
        icon = 3;
      item2 = AppendItem(item, columnInfo, icon);
      if (col->IsGeometry() == true)
        {
          if (col->IsGeometryIndex() == true)
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY_INDEX,
                                                     viewName,
                                                     col->GetName())));
          else if (col->IsMbrCache() == true)
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY_CACHED,
                                                     viewName,
                                                     col->GetName())));
          else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY, viewName,
                                                     col->GetName())));
      } else
        SetItemData(item2,
                    (wxTreeItemData *) (new
                                        MyObject(MY_VIEW_COLUMN, viewName,
                                                 col->GetName())));
      col = col->GetNext();
    }
  trgr = list.GetFirstTrigger();
  while (trgr)
    {
      // setting up Triggers
      triggerInfo = trgr->GetName();
      item2 = AppendItem(item, triggerInfo, 5);
      SetItemData(item2,
                  (wxTreeItemData *) (new
                                      MyObject(MY_TRIGGER, trgr->GetName())));
      trgr = trgr->GetNext();
    }
}

void MyTableTree::AddTable(wxString & dbAlias,
                           wxString & tableName, bool virtualTable,
                           RootNodes * nodes)
{
//
// appends a table to the TREE list [ATTACHED DB]
//
  MyTableInfo list;
  MyColumnInfo *col;
  MyIndexInfo *idx;
  MyTriggerInfo *trgr;
  wxTreeItemId item;
  wxTreeItemId item2;
  wxString columnInfo;
  wxString indexInfo;
  wxString triggerInfo;
  int icon = 1;
  if (virtualTable == true)
    icon = 8;
  wxTreeItemId rootNode = GetRootNode(tableName, nodes);
  item = AppendItem(rootNode, tableName, icon);
  if (virtualTable == true)
    SetItemData(item,
                (wxTreeItemData *) (new
                                    MyObject(MY_VTABLE, true, dbAlias,
                                             tableName)));
  else
    SetItemData(item,
                (wxTreeItemData *) (new
                                    MyObject(MY_TABLE, true, dbAlias,
                                             tableName)));
  MainFrame->GetTableColumns(dbAlias, tableName, &list);
  MainFrame->GetTableIndices(dbAlias, tableName, &list);
  MainFrame->GetTableTriggers(dbAlias, tableName, &list);
  col = list.GetFirstColumn();
  while (col)
    {
      int icon;
      if (col->IsPrimaryKey() == true)
        icon = 2;
      else
        {
          if (col->IsGeometry() == true)
            {
              if (virtualTable == false)
                SetItemImage(item, 10);
              else
                SetItemImage(item, 13);
              if (col->IsGeometryIndex() == true)
                icon = 7;
              else if (col->IsMbrCache() == true)
                icon = 11;
              else
                icon = 6;
          } else
            icon = 3;
        }
      columnInfo = col->GetName();
      item2 = AppendItem(item, columnInfo, icon);
      if (virtualTable == true)
        {
          if (col->IsGeometry() == true)
            {
              SetItemData(item2,
                          (wxTreeItemData *) (new
                                              MyObject(MY_VIRTUAL_GEOMETRY,
                                                       tableName,
                                                       col->GetName())));
          } else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIRTUAL_COLUMN,
                                                     tableName,
                                                     col->GetName())));
      } else
        {
          if (col->IsGeometry() == true)
            {
              if (col->IsGeometryIndex() == true)
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY_INDEX,
                                                         tableName,
                                                         col->GetName())));
              else if (col->IsMbrCache() == true)
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY_CACHED,
                                                         tableName,
                                                         col->GetName())));
              else
                SetItemData(item2,
                            (wxTreeItemData *) (new
                                                MyObject(MY_GEOMETRY, tableName,
                                                         col->GetName())));
          } else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_COLUMN, tableName,
                                                     col->GetName())));
        }
      col = col->GetNext();
    }
// setting up Primary Key
  bool ok_pk = false;
  idx = list.GetFirstIndex();
  while (idx)
    {
      indexInfo = idx->GetName();
      if (indexInfo.StartsWith(wxT("sqlite_autoindex_")) == true)
        {
          item2 = AppendItem(item, wxT("PrimaryKey"), 19);
          MainFrame->GetPrimaryKeyFields(dbAlias, indexInfo, item2);
          ok_pk = true;
          break;
        }
      idx = idx->GetNext();
    }
  if (ok_pk == false)
    {
      col = list.GetFirstColumn();
      while (col)
        {
          if (col->IsPrimaryKey() == true)
            {
              ok_pk = true;
              break;
            }
          col = col->GetNext();
        }
      if (ok_pk == true)
        {
          item2 = AppendItem(item, wxT("PrimaryKey"), 19);
          col = list.GetFirstColumn();
          while (col)
            {
              if (col->IsPrimaryKey() == true)
                {
                  AppendItem(item2, col->GetName(), 2);
                  break;
                }
              col = col->GetNext();
            }
        }
    }
// setting up Foreign Keys
  MainFrame->GetForeignKeys(dbAlias, tableName, item);
  idx = list.GetFirstIndex();
  while (idx)
    {
      // setting up Indices
      indexInfo = idx->GetName();
      if (indexInfo.StartsWith(wxT("sqlite_autoindex_")) == true)
        {
          idx = idx->GetNext();
          continue;
        }
      item2 = AppendItem(item, indexInfo, 4);
      SetItemData(item2,
                  (wxTreeItemData *) (new MyObject(MY_INDEX, idx->GetName())));
      MainFrame->GetIndexFields(dbAlias, indexInfo, item2);
      idx = idx->GetNext();
    }
  trgr = list.GetFirstTrigger();
  while (trgr)
    {
      // setting up Triggers
      triggerInfo = trgr->GetName();
      item2 = AppendItem(item, triggerInfo, 5);
      SetItemData(item2,
                  (wxTreeItemData *) (new
                                      MyObject(MY_TRIGGER, trgr->GetName())));
      trgr = trgr->GetNext();
    }
}

void MyTableTree::AddView(wxString & dbAlias,
                          wxString & viewName, RootNodes * nodes)
{
//
// appends a view to the TREE list [ATTACHED DB]
  MyViewInfo list;
  MyColumnInfo *col;
  MyTriggerInfo *trgr;
  wxTreeItemId item;
  wxTreeItemId item2;
  wxString columnInfo;
  wxString triggerInfo;
  int icon = 9;
  wxTreeItemId rootNode = GetRootNode(viewName, nodes);
  item = AppendItem(rootNode, viewName, icon);
  SetItemData(item,
              (wxTreeItemData *) (new
                                  MyObject(MY_VIEW, true, dbAlias, viewName)));
  MainFrame->GetViewColumns(dbAlias, viewName, &list);
  MainFrame->GetViewTriggers(dbAlias, viewName, &list);
  col = list.GetFirst();
  while (col)
    {
      columnInfo = col->GetName();
      if (col->IsGeometry() == true)
        {
          SetItemImage(item, 12);
          if (col->IsGeometryIndex() == true)
            icon = 7;
          else if (col->IsMbrCache() == true)
            icon = 11;
          else
            icon = 6;
      } else
        icon = 3;
      item2 = AppendItem(item, columnInfo, icon);
      if (col->IsGeometry() == true)
        {
          if (col->IsGeometryIndex() == true)
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY_INDEX,
                                                     viewName,
                                                     col->GetName())));
          else if (col->IsMbrCache() == true)
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY_CACHED,
                                                     viewName,
                                                     col->GetName())));
          else
            SetItemData(item2,
                        (wxTreeItemData *) (new
                                            MyObject(MY_VIEW_GEOMETRY, viewName,
                                                     col->GetName())));
      } else
        SetItemData(item2,
                    (wxTreeItemData *) (new
                                        MyObject(MY_VIEW_COLUMN, viewName,
                                                 col->GetName())));
      col = col->GetNext();
    }
  trgr = list.GetFirstTrigger();
  while (trgr)
    {
      // setting up Triggers
      triggerInfo = trgr->GetName();
      item2 = AppendItem(item, triggerInfo, 5);
      SetItemData(item2,
                  (wxTreeItemData *) (new
                                      MyObject(MY_TRIGGER, trgr->GetName())));
      trgr = trgr->GetNext();
    }
}

void MyTableTree::OnSelChanged(wxTreeEvent & event)
{
//
// selecting some node [mouse action]
//
  wxTreeItemId item = event.GetItem();
  if (item == Root || item == RootUserData || item == RootTopologies
      || item == RootStyling || item == RootIsoMetadata || item == RootMetadata
      || item == RootInternal || item == RootSpatialIndex)
    return;
  MyObject *obj = (MyObject *) GetItemData(item);
  if (obj == NULL)
    return;
  CurrentItem = item;
}

void MyTableTree::OnRightClick(wxTreeEvent & event)
{
//
// right click on some node [mouse action]
//
  wxMenu *menu = new wxMenu();
  wxMenuItem *menuItem;
  wxString title;
  bool table = false;
  bool canEdit = false;
  bool view = false;
  bool column = false;
  bool geometry = false;
  bool geometry_index = false;
  bool geometry_cached = false;
  bool view_column = false;
  bool view_geometry = false;
  bool virtual_column = false;
  bool virtual_geometry = false;
  bool index = false;
  bool trigger = false;
  bool attached_db = false;
  bool metadata = MainFrame->CheckMetadata();
  if (MainFrame->IsConnected() == false)
    return;
  wxTreeItemId item = event.GetItem();
  SelectItem(item);
  wxPoint pt = event.GetPoint();
  if (item == Root || item == RootUserData || item == RootTopologies
      || item == RootStyling || item == RootIsoMetadata || item == RootMetadata
      || item == RootInternal || item == RootSpatialIndex)
    {
      CurrentItem = wxTreeItemId();
      menuItem = new wxMenuItem(menu, Tree_Refresh, wxT("&Refresh"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_QueryViewComposer,
                       wxT("Query/View &Composer"));
      menuItem->SetBitmap(wxBitmap(composer_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_NewTable, wxT("Create New &Table"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_NewView, wxT("Create New &View"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatisticsAll,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      PopupMenu(menu, pt);
      return;
    }
  MyObject *obj = (MyObject *) GetItemData(item);
  if (obj == NULL)
    {
      CurrentItem = wxTreeItemId();
      menuItem = new wxMenuItem(menu, Tree_Refresh, wxT("&Refresh"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_QueryViewComposer,
                       wxT("Query/View &Composer"));
      menuItem->SetBitmap(wxBitmap(composer_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_NewTable, wxT("Create New &Table"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_NewView, wxT("Create New &View"));
      menu->Append(menuItem);
      PopupMenu(menu, pt);
      return;
    }
  switch (obj->GetType())
    {
      case MY_VTABLE:
      case MY_TABLE:
        table = true;
        break;
      case MY_VIEW:
        view = true;
        break;
      case MY_COLUMN:
        column = true;
        break;
      case MY_GEOMETRY:
        geometry = true;
        break;
      case MY_GEOMETRY_INDEX:
        geometry_index = true;
        break;
      case MY_GEOMETRY_CACHED:
        geometry_cached = true;
        break;
      case MY_VIEW_COLUMN:
        view_column = true;
        break;
      case MY_VIEW_GEOMETRY:
      case MY_VIEW_GEOMETRY_INDEX:
      case MY_VIEW_GEOMETRY_CACHED:
        view_geometry = true;
        break;
      case MY_VIRTUAL_COLUMN:
        virtual_column = true;
        break;
      case MY_VIRTUAL_GEOMETRY:
        virtual_geometry = true;
        break;
      case MY_INDEX:
        index = true;
        break;
      case MY_TRIGGER:
        trigger = true;
      case MY_ATTACHED:
        attached_db = true;
        break;
    };
  if (obj->GetType() == MY_TABLE)
    canEdit = true;
  CurrentItem = item;
  menuItem = new wxMenuItem(menu, Tree_Refresh, wxT("&Refresh"));
  menu->Append(menuItem);
  if (table == true)
    {
      if (obj->IsAttached() == true)
        {
          wxString title =
            wxT("Table: ") + obj->GetDbAlias() + wxT(".") + obj->GetName();
          menu->SetTitle(title);
          menu->AppendSeparator();
          menuItem = new wxMenuItem(menu, Tree_Select, wxT("&Query table"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show columns"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_ShowSql, wxT("&Show CREATE statement"));
          menu->Append(menuItem);
      } else
        {
          wxString title = wxT("Table: ") + obj->GetName();
          menu->SetTitle(title);
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_QueryViewComposer,
                           wxT("Query/View &Composer"));
          menuItem->SetBitmap(wxBitmap(composer_xpm));
          menu->Append(menuItem);
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_NewTable, wxT("Create New &Table"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_NewView, wxT("Create New &View"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                           wxT("Update Layer &Statistics"));
          menu->Append(menuItem);
          menu->AppendSeparator();
          if (canEdit == true)
            {
              menuItem =
                new wxMenuItem(menu, Tree_Edit, wxT("&Edit table rows"));
              menu->Append(menuItem);
          } else
            {
              menuItem = new wxMenuItem(menu, Tree_Select, wxT("&Query table"));
              menu->Append(menuItem);
            }
          menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show columns"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_ShowSql, wxT("&Show CREATE statement"));
          menu->Append(menuItem);
          menu->AppendSeparator();
          wxMenu *maintenanceMenu = new wxMenu();
          menuItem =
            new wxMenuItem(maintenanceMenu, Tree_NewColumn,
                           wxT("Add New &Column"));
          maintenanceMenu->Append(menuItem);
          menuItem =
            new wxMenuItem(maintenanceMenu, Tree_Rename, wxT("&Rename table"));
          maintenanceMenu->Append(menuItem);
          menuItem =
            new wxMenuItem(maintenanceMenu, Tree_Drop, wxT("&Drop table"));
          maintenanceMenu->Append(menuItem);
          maintenanceMenu->AppendSeparator();
          menuItem =
            new wxMenuItem(maintenanceMenu, Tree_NewIndex,
                           wxT("Create New &Index"));
          maintenanceMenu->Append(menuItem);
          menuItem =
            new wxMenuItem(maintenanceMenu, Tree_NewTrigger,
                           wxT("Create New &Trigger"));
          maintenanceMenu->Append(menuItem);
          if (obj->GetType() == MY_TABLE)
            {
              maintenanceMenu->AppendSeparator();
              menuItem =
                new wxMenuItem(maintenanceMenu, Tree_CheckDuplicates,
                               wxT("Check &Duplicate rows"));
              maintenanceMenu->Append(menuItem);
              menuItem =
                new wxMenuItem(maintenanceMenu, Tree_RemoveDuplicates,
                               wxT("Remove Duplicate rows"));
            }
          maintenanceMenu->Append(menuItem);
          menu->AppendSubMenu(maintenanceMenu, wxT("&Maintenance"));
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_DumpTxtTab, wxT("Export as &Txt/Tab"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpCsv, wxT("Export as &CSV"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpHtml, wxT("Export as &HTML"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpDif, wxT("Export as &DIF"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpSylk, wxT("Export as &SYLK"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpDbf, wxT("Export as &DBF"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpPostGIS,
                           wxT("SQL Dump for &PostGIS"));
          menu->Append(menuItem);
        }
    }
  if (view == true)
    {
      if (obj->IsAttached() == true)
        {
          wxString title =
            wxT("View: ") + obj->GetDbAlias() + wxT(".") + obj->GetName();
          menu->SetTitle(title);
          menu->AppendSeparator();
          menuItem = new wxMenuItem(menu, Tree_Select, wxT("&Query view"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show columns"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_ShowSql, wxT("&Show CREATE statement"));
          menu->Append(menuItem);
      } else
        {
          wxString title = wxT("View: ") + obj->GetName();
          menu->SetTitle(title);
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_QueryViewComposer,
                           wxT("Query/View &Composer"));
          menuItem->SetBitmap(wxBitmap(composer_xpm));
          menu->Append(menuItem);
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_NewTable, wxT("Create New &Table"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_NewView, wxT("Create New &View"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                           wxT("Update Layer &Statistics"));
          menu->Append(menuItem);
          menu->AppendSeparator();
          menuItem = new wxMenuItem(menu, Tree_Select, wxT("&Query view"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show columns"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_ShowSql, wxT("&Show CREATE statement"));
          menu->Append(menuItem);
          menu->AppendSeparator();
          menuItem = new wxMenuItem(menu, Tree_Drop, wxT("&Drop view"));
          menu->Append(menuItem);
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_DumpTxtTab, wxT("Export as &Txt/Tab"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpCsv, wxT("Export as &CSV"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpHtml, wxT("Export as &HTML"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpDif, wxT("Export as &DIF"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpSylk, wxT("Export as &SYLK"));
          menu->Append(menuItem);
          menuItem = new wxMenuItem(menu, Tree_DumpDbf, wxT("Export as &DBF"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_DumpPostGIS,
                           wxT("SQL Dump for &PostGIS"));
          menu->Append(menuItem);
        }
    }
  if (column == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menu->AppendSeparator();
      if (MainFrame->IsPrimaryKey(obj->GetName(), obj->GetColumn()) == false)
        {
          menuItem = new wxMenuItem(menu, Tree_DropColumn, wxT("&Drop Column"));
          menu->Append(menuItem);
          menuItem =
            new wxMenuItem(menu, Tree_RenameColumn, wxT("&Rename Column"));
          menu->Append(menuItem);
          menu->AppendSeparator();
        }
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeometry, wxT("&Check geometries"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_SetSrid, wxT("&Set SRID"));
      menu->Append(menuItem);
      if (metadata == true)
        {
          menu->AppendSeparator();
          menuItem =
            new wxMenuItem(menu, Tree_Recover, wxT("&Recover geometry column"));
          menu->Append(menuItem);
        }
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ColumnStats, wxT("&Statistic snapshot"));
      menuItem->SetBitmap(wxBitmap(statistics_xpm));
      menu->Append(menuItem);
    }
  if (view_column == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ColumnStats, wxT("&Statistic snapshot"));
      menuItem->SetBitmap(wxBitmap(statistics_xpm));
      menu->Append(menuItem);
    }
  if (virtual_column == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ColumnStats, wxT("&Statistic snapshot"));
      menuItem->SetBitmap(wxBitmap(statistics_xpm));
      menu->Append(menuItem);
    }
  if (geometry == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show Spatial Metadata"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeom, wxT("&LWGEOM Check geometries"));
      menuItem->SetBitmap(wxBitmap(checkgeom_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_SaneGeom, wxT("&LWGEOM Sanitize geometries"));
      menuItem->SetBitmap(wxBitmap(sanegeom_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_GisLayerAuth,
                       wxT("&GIS layer authorizations"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeometry, wxT("&Check geometries"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_SpatialIndex, wxT("&Build Spatial Index"));
      menuItem->SetBitmap(wxBitmap(spatialidx_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_MbrCache, wxT("Build &MBR cache"));
      menuItem->SetBitmap(wxBitmap(mbrcache_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_RebuildTriggers,
                       wxT("Rebuild Geometry &Triggers"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_DumpShp, wxT("Export as &Shapefile"));
      menuItem->SetBitmap(wxBitmap(dumpshp_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_DumpKml, wxT("Export as &KML"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ElementaryGeoms,
                       wxT("&separating elementary Geometries"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_MalformedGeometries,
                       wxT("&Malformed geometries"));
      menuItem->SetBitmap(wxBitmap(malformed_geoms_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_RepairPolygons, wxT("&Repair Polygons"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
    }
  if (geometry_index == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show Spatial Metadata"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_GisLayerAuth,
                       wxT("&GIS layer authorizations"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeom, wxT("&LWGEOM Check geometries"));
      menuItem->SetBitmap(wxBitmap(checkgeom_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_SaneGeom, wxT("&LWGEOM Sanitize geometries"));
      menuItem->SetBitmap(wxBitmap(sanegeom_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeometry, wxT("&Check geometries"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_SpatialIndex, wxT("&Remove Spatial Index"));
      menuItem->SetBitmap(wxBitmap(kill_spindex_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_CheckSpatialIndex,
                       wxT("&Check Spatial Index"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_RecoverSpatialIndex,
                       wxT("&Recover Spatial Index"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_RebuildTriggers,
                       wxT("Rebuild Geometry &Triggers"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_DumpShp, wxT("Export as &Shapefile"));
      menuItem->SetBitmap(wxBitmap(dumpshp_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_DumpKml, wxT("Export as &KML"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ElementaryGeoms,
                       wxT("&separating elementary Geometries"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_MalformedGeometries,
                       wxT("&Malformed geometries"));
      menuItem->SetBitmap(wxBitmap(malformed_geoms_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_RepairPolygons, wxT("&Repair Polygons"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
    }
  if (geometry_cached == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show Spatial Metadata"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_GisLayerAuth,
                       wxT("&GIS layer authorizations"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeom, wxT("&LWGEOM Check geometries"));
      menuItem->SetBitmap(wxBitmap(checkgeom_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_SaneGeom, wxT("&LWGEOM Sanitize geometries"));
      menuItem->SetBitmap(wxBitmap(sanegeom_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_CheckGeometry, wxT("&Check geometries"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MbrCache, wxT("&Remove MBR cache"));
      menuItem->SetBitmap(wxBitmap(kill_spindex_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_RebuildTriggers,
                       wxT("Rebuild Geometry &Triggers"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_DumpShp, wxT("Export as &Shapefile"));
      menuItem->SetBitmap(wxBitmap(dumpshp_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_DumpKml, wxT("Export as &KML"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ElementaryGeoms,
                       wxT("&separating elementary Geometries"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_MalformedGeometries,
                       wxT("&Malformed geometries"));
      menuItem->SetBitmap(wxBitmap(malformed_geoms_xpm));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_RepairPolygons, wxT("&Repair Polygons"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
    }
  if (view_geometry == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show Spatial Metadata"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_GisLayerAuth,
                       wxT("&GIS layer authorizations"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_DumpShp, wxT("Export as &Shapefile"));
      menuItem->SetBitmap(wxBitmap(dumpshp_xpm));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_DumpKml, wxT("Export as &KML"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ElementaryGeoms,
                       wxT("&separating elementary Geometries"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_MalformedGeometries,
                       wxT("&Malformed geometries"));
      menuItem->SetBitmap(wxBitmap(malformed_geoms_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
    }
  if (virtual_geometry == true)
    {
      wxString title =
        wxT("Column: ") + obj->GetName() + wxT(".") + obj->GetColumn();
      menu->SetTitle(title);
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show Spatial Metadata"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_GisLayerAuth,
                       wxT("&GIS layer authorizations"));
      menu->Append(menuItem);
      menuItem = new wxMenuItem(menu, Tree_Extent, wxT("&Extent"));
      menu->Append(menuItem);
      menuItem =
        new wxMenuItem(menu, Tree_UpdateLayerStatistics,
                       wxT("Update Layer &Statistics"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_DumpKml, wxT("Export as &KML"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_ElementaryGeoms,
                       wxT("&separating elementary Geometries"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem =
        new wxMenuItem(menu, Tree_MalformedGeometries,
                       wxT("&Malformed geometries"));
      menuItem->SetBitmap(wxBitmap(malformed_geoms_xpm));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_MapPreview, wxT("&Map Preview"));
      menuItem->SetBitmap(wxBitmap(map_preview_xpm));
      menu->Append(menuItem);
    }
  if (index == true)
    {
      wxString title = wxT("Index: ") + obj->GetName();
      menu->SetTitle(title);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show index"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_Drop, wxT("&Drop index"));
      menu->Append(menuItem);
    }
  if (trigger == true)
    {
      wxString title = wxT("Trigger: ") + obj->GetName();
      menu->SetTitle(title);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_Show, wxT("&Show trigger"));
      menu->Append(menuItem);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_Drop, wxT("&Drop trigger"));
      menu->Append(menuItem);
    }
  if (attached_db == true)
    {
      wxString title = wxT("Attached DB: ") + obj->GetName();
      menu->SetTitle(title);
      menu->AppendSeparator();
      menuItem = new wxMenuItem(menu, Tree_Detach, wxT("&Detach Database"));
      menu->Append(menuItem);
    }
  PopupMenu(menu, pt);
}

void MyTableTree::OnCmdQueryViewComposer(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - query/view composer invoked
//
  MainFrame->QueryViewComposer();
}

void MyTableTree::OnCmdNewTable(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - new table creation required
//
  wxString sql;
  sql = wxT("CREATE TABLE ...table-name... (\n");
  sql += wxT("...column1,\n...column2,\n...columnN)");
  MainFrame->SetSql(sql, false);
}

void MyTableTree::OnCmdNewView(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - new view creation required
//
  wxString sql;
  sql = wxT("CREATE VIEW ...view-name... AS\n");
  sql += wxT("SELECT ...sql-select-statement...");
  MainFrame->SetSql(sql, false);
}

void MyTableTree::OnCmdNewIndex(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - new index creation required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      sql = wxT("CREATE [ UNIQUE ] INDEX ...index-name...\nON ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\n(\n...column1, column2, columnN...\n)");
      MainFrame->SetSql(sql, false);
    }
}

void MyTableTree::OnCmdNewTrigger(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - new trigger creation required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      sql = wxT("CREATE TRIGGER ...trigger-name...\n[ BEFORE | AFTER ]\n");
      sql += wxT("[ INSERT | UPDATE | DELETE ]\nON ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\n...sql-statement...");
      MainFrame->SetSql(sql, false);
    }
}

void MyTableTree::OnCmdNewColumn(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - new column creation required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      sql = wxT("ALTER TABLE ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nADD COLUMN ...column-name column-type...");
      MainFrame->SetSql(sql, false);
    }
}

void MyTableTree::OnCmdSelect(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - examining table rows required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  sql = wxT("SELECT * FROM ");
  if (obj->IsAttached() == true)
    sql += obj->GetDbAlias() + wxT(".");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  MainFrame->SetSql(sql, true);
}

void MyTableTree::OnCmdShow(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - examining full infos required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE || obj->GetType() == MY_VTABLE
      || obj->GetType() == MY_VIEW)
    {
      if (obj->IsAttached() == true)
        sql = wxT("PRAGMA ") + obj->GetDbAlias() + wxT(".table_info(");
      else
        sql = wxT("PRAGMA table_info(");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")");
    }
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      sql = wxT("SELECT *\nFROM geom_cols_ref_sys\n");
      sql += wxT("WHERE Lower(f_table_name) = Lower('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')\nAND Lower(f_geometry_column) = Lower('");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
    }
  if (obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED)
    {
      sql = wxT("SELECT *\nFROM views_geometry_columns AS a\n");
      sql += wxT("JOIN geometry_columns AS b ON (");
      sql += wxT("Lower(a.f_table_name) = Lower(b.f_table_name) AND ");
      sql += wxT("Lower(a.f_geometry_column) = Lower(b.f_geometry_column))\n");
      sql += wxT("JOIN spatial_ref_sys USING (srid)\n");
      sql += wxT("WHERE Lower(view_name) = Lower('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')\nAND Lower(view_geometry) = Lower('");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
    }
  if (obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      sql = wxT("SELECT *\nFROM virts_geometry_columns\n");
      sql += wxT("JOIN spatial_ref_sys USING (srid)\n");
      sql += wxT("WHERE Lower(virt_name) = Lower('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')\nAND Lower(virt_geometry) = Lower('");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
    }
  if (obj->GetType() == MY_INDEX)
    {
      sql = wxT("PRAGMA index_info(");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")");
    }
  if (obj->GetType() == MY_TRIGGER)
    {
      sql =
        wxT
        ("SELECT sql FROM sqlite_master\nWHERE type = 'trigger' AND name = '");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("'");
    }
  if (sql.Len() < 1)
    return;
  MainFrame->SetSql(sql, true);
}

void MyTableTree::OnCmdDrop(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dropping some object required
//
  wxString sql;
  wxString sql2;
  wxString msg;
  char *errMsg = NULL;
  int ret;
  wxString name;
  char xname[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      sql = wxT("DROP TABLE IF EXISTS ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      msg = wxT("Do you really intend to drop the Table named: ");
      msg += obj->GetName();
      msg += wxT("\n[and any other related object] ?");
    }
  if (obj->GetType() == MY_VTABLE)
    {
      msg = wxT("Do you really intend to drop the VirtualTable named: ");
      msg += obj->GetName();
      msg += wxT(" ?");
      sql = wxT("DROP TABLE IF EXISTS ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
    }
  if (obj->GetType() == MY_VIEW)
    {
      msg = wxT("Do you really intend to drop the View named: ");
      msg += obj->GetName();
      msg += wxT(" ?");
      sql = wxT("DROP VIEW IF EXISTS ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
    }
  if (obj->GetType() == MY_INDEX)
    {
      msg = wxT("Do you really intend to drop the Index named: ");
      msg += obj->GetName();
      msg += wxT(" ?");
      sql = wxT("DROP INDEX IF EXISTS ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
    }
  if (obj->GetType() == MY_TRIGGER)
    {
      msg = wxT("Do you really intend to drop the Trigger named: ");
      msg += obj->GetName();
      msg += wxT(" ?");
      sql = wxT("DROP TRIGGER IF EXISTS ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
    }
  wxMessageDialog confirm(this, msg, wxT("Confirming DROP"),
                          wxOK | wxCANCEL | wxICON_QUESTION);
  ret = confirm.ShowModal();
  if (ret != wxID_OK)
    return;
  ::wxBeginBusyCursor();
  if (obj->GetType() == MY_TABLE)
    {
      strcpy(xname, obj->GetName().ToUTF8());
      gaiaDropTable(sqlite, xname);
    }
  if (obj->GetType() == MY_VTABLE)
    {
      strcpy(xname, obj->GetName().ToUTF8());
      gaiaDropTable(sqlite, xname);
    }
  if (obj->GetType() == MY_VIEW)
    {
      strcpy(xname, obj->GetName().ToUTF8());
      gaiaDropTable(sqlite, xname);
    }
  ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      goto rollback;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT("Selected object '") + obj->GetName() +
               wxT("' was successfully removed"), wxT("spatialite_gui"),
               wxOK | wxICON_INFORMATION, this);
  MainFrame->InitTableTree();
  return;
rollback:
  ret = sqlite3_exec(sqlite, "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("An error occurred\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdRename(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - table renaming required
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE || obj->GetType() == MY_VTABLE)
    {
      sql = wxT("ALTER TABLE ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nRENAME TO ...new-table-name...");
      MainFrame->SetSql(sql, false);
    }
}

void MyTableTree::OnCmdDetachDB(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - DETACH DATABASE
//
  wxString sql;
  int ret;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_ATTACHED)
    {
      wxString msg = wxT("Do you really intend to detach this database ?\n\n");
      msg += obj->GetName();
      msg += wxT(" AS ");
      msg += obj->GetDbAlias();
      wxMessageDialog confirm(this, msg, wxT("Confirming DETACH DATABASE"),
                              wxOK | wxCANCEL | wxICON_QUESTION);
      ret = confirm.ShowModal();
      if (ret != wxID_OK)
        return;

      char *errMsg = NULL;
      sqlite3 *sqlite = MainFrame->GetSqlite();
      wxString sql = wxT("DETACH DATABASE ") + obj->GetDbAlias();
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") +
                       wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                       wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      MainFrame->InitTableTree();
    }
}

void MyTableTree::OnCmdRefresh(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - refreshing the Tree
//
  MainFrame->InitTableTree();
}

void MyTableTree::OnCmdRecover(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Recover Geometry
//
  char *errMsg = NULL;
  int ret;
  wxString sql;
  int i;
  char **results;
  int rows;
  int columns;
  int retval = 0;
  int srid = -1;
  char dummy[128];
  wxString type;
  wxString dims;
  RecoverDialog dlg;
  char xname[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN)
    {
      // trying to recover a geometry column
      if (MainFrame->CheckMetadata() == false)
        {
          wxMessageBox(wxT
                       ("Missing Spatial Metadata tables\n\ntry to run the 'init_spatialite.sql' script ..."),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          return;
        }
      dlg.Create(MainFrame, obj->GetName(), obj->GetColumn());
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          srid = dlg.GetSrid();
          type = dlg.GetType();
          dims = dlg.GetDimension();
          ::wxBeginBusyCursor();
          ret = sqlite3_exec(sqlite, "BEGIN", NULL, NULL, &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              ::wxEndBusyCursor();
              return;
            }
          sql = wxT("SELECT RecoverGeometryColumn('");
          strcpy(xname, obj->GetName().ToUTF8());
          MainFrame->CleanSqlString(xname);
          sql += wxString::FromUTF8(xname);
          sql += wxT("', '");
          strcpy(xname, obj->GetColumn().ToUTF8());
          MainFrame->CleanSqlString(xname);
          sql += wxString::FromUTF8(xname);
          sprintf(dummy, "', %d, '", srid);
          sql += wxString::FromUTF8(dummy);
          sql += type;
          sql += wxT("', '");
          sql += dims;
          sql += wxT("')");
          ret =
            sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                              &errMsg);
          if (ret != SQLITE_OK)
            {
              wxMessageBox(wxT("SQLite SQL error: ") +
                           wxString::FromUTF8(errMsg), wxT("spatialite_gui"),
                           wxOK | wxICON_ERROR, this);
              sqlite3_free(errMsg);
              goto rollback;
            }
          if (rows < 1)
            ;
          else
            {
              for (i = 1; i <= rows; i++)
                {
                  if (results[(i * columns) + 0])
                    retval = atoi(results[(i * columns) + 0]);
                }
            }
          sqlite3_free_table(results);
          if (!retval)
            goto rollback;
          ret = sqlite3_exec(sqlite, "COMMIT", NULL, NULL, &errMsg);
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
          wxMessageBox(wxT("Geometry column ") + obj->GetName() +
                       wxT(".") + obj->GetColumn() +
                       wxT(" was successfully recovered"),
                       wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
        }
    }
  return;
rollback:
  ret = sqlite3_exec(sqlite, "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("Geometry column doesn't satisfies required constraints\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdShowSql(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Showing CREATE TABLE statement
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE || obj->GetType() == MY_VTABLE)
    {
      sql = wxT("SELECT sql FROM ");
      if (obj->IsAttached() == true)
        sql += obj->GetDbAlias() + wxT(".");
      if (obj->IsTemporary() == true)
        sql += wxT("sqlite_temp_master");
      else
        sql += wxT("sqlite_master");
      sql += wxT("\nWHERE type = 'table' AND name = '");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("'");
      MainFrame->SetSql(sql, true);
    }
  if (obj->GetType() == MY_VIEW)
    {
      sql = wxT("SELECT sql FROM ");
      if (obj->IsAttached() == true)
        sql += obj->GetDbAlias() + wxT(".");
      if (obj->IsTemporary() == true)
        sql += wxT("sqlite_temp_master");
      else
        sql += wxT("sqlite_master");
      sql += wxT("\nWHERE type = 'view' AND name = '");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("'");
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::OnCmdCheckSpatialIndex(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Check Spatial Index for validity 
//
  char *errMsg = NULL;
  int ret;
  wxString sql;
  wxString msg;
  int i;
  char **results;
  int rows;
  int columns;
  int retval = 0;
  char xtable[1024];
  char xcolumn[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj->GetType() == MY_GEOMETRY_INDEX)
    {
      // checking the Spatial Index
      ::wxBeginBusyCursor();
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT CheckSpatialIndex('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      ::wxEndBusyCursor();
      if (retval)
        wxMessageBox(wxT("Spatial Index idx_") + obj->GetName() +
                     wxT("_") + obj->GetColumn() +
                     wxT(" is valid and consistent"), wxT("spatialite_gui"),
                     wxOK | wxICON_INFORMATION, this);
      else
        wxMessageBox(wxT("Spatial Index idx_") + obj->GetName() +
                     wxT("_") + obj->GetColumn() +
                     wxT(" is invalid and inconsistent\n") +
                     wxT
                     ("Please, recover this Spatial Index as soon as possible"),
                     wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
    }
}

void MyTableTree::OnCmdRecoverSpatialIndex(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Recover Spatial Index (rebuilding from scratch) 
//
  char *errMsg = NULL;
  int ret;
  wxString sql;
  wxString msg;
  int i;
  char **results;
  int rows;
  int columns;
  int retval = 0;
  char xtable[1024];
  char xcolumn[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj->GetType() == MY_GEOMETRY_INDEX)
    {
      // recovering the Spatial Index
      ::wxBeginBusyCursor();
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT RecoverSpatialIndex('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      ::wxEndBusyCursor();
      if (retval)
        wxMessageBox(wxT("Spatial Index idx_") + obj->GetName() +
                     wxT("_") + obj->GetColumn() +
                     wxT(" was succesfully recovered"), wxT("spatialite_gui"),
                     wxOK | wxICON_INFORMATION, this);
      else
        wxMessageBox(wxT("ERROR: unable to recover Spatial Index idx_") +
                     obj->GetName() + wxT("_") + obj->GetColumn(),
                     wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
    }
}

void MyTableTree::OnCmdSpatialIndex(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Spatial Index creation-destruction
//
  char *errMsg = NULL;
  int ret;
  wxString sql;
  wxString msg;
  int i;
  char **results;
  int rows;
  int columns;
  int retval = 0;
  wxString name;
  char xname[1024];
  char xtable[1024];
  char xcolumn[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY)
    {
      // creating the Spatial Index
      ::wxBeginBusyCursor();
      ret = sqlite3_exec(sqlite, "BEGIN", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT CreateSpatialIndex('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto rollback;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      if (!retval)
        goto rollback;
      ret = sqlite3_exec(sqlite, "COMMIT", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ::wxEndBusyCursor();
      wxMessageBox(wxT("Spatial Index idx_") + obj->GetName() +
                   wxT("_") + obj->GetColumn() +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      MainFrame->InitTableTree();
  } else if (obj->GetType() == MY_GEOMETRY_INDEX)
    {
      // dropping the Spatial Index
      ::wxBeginBusyCursor();
      ret = sqlite3_exec(sqlite, "BEGIN", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT DisableSpatialIndex('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      msg = wxT("Do you really intend to delete the SpatialIndex\n");
      msg += wxT("on column ");
      msg += obj->GetName();
      msg += wxT(".");
      msg += obj->GetColumn();
      msg += wxT(" ?");
      wxMessageDialog confirm(this, msg, wxT("Confirming Delete Spatial Index"),
                              wxOK | wxCANCEL | wxICON_QUESTION);
      ret = confirm.ShowModal();
      if (ret != wxID_OK)
        return;
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto rollback;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      if (!retval)
        goto rollback;
      sql = wxT("DROP TABLE IF EXISTS ");
      name = wxT("idx_");
      name += obj->GetName();
      name += wxT("_");
      name += obj->GetColumn();
      strcpy(xname, name.ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ret = sqlite3_exec(sqlite, "COMMIT", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ::wxEndBusyCursor();
      wxMessageBox(wxT("Spatial Index idx_") + obj->GetName() +
                   wxT("_") + obj->GetColumn() +
                   wxT(" was successfully removed"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      MainFrame->InitTableTree();
    }
  return;
rollback:
  ret = sqlite3_exec(sqlite, "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("An error occurred\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdMbrCache(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - MBR cache creation-destruction
//
  char *errMsg = NULL;
  int ret;
  wxString sql;
  wxString msg;
  int i;
  char **results;
  int rows;
  int columns;
  int retval = 0;
  wxString name;
  char xname[1024];
  char xtable[1024];
  char xcolumn[1024];
  sqlite3 *sqlite = MainFrame->GetSqlite();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY)
    {
      // creating the MBR cache
      ::wxBeginBusyCursor();
      ret = sqlite3_exec(sqlite, "BEGIN", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT CreateMbrCache('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto rollback;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      if (!retval)
        goto rollback;
      ret = sqlite3_exec(sqlite, "COMMIT", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ::wxEndBusyCursor();
      wxMessageBox(wxT("MBR cache cache_") + obj->GetName() +
                   wxT("_") + obj->GetColumn() +
                   wxT(" was successfully created"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      MainFrame->InitTableTree();
  } else if (obj->GetType() == MY_GEOMETRY_CACHED)
    {
      // dropping the MBR cache
      ::wxBeginBusyCursor();
      ret = sqlite3_exec(sqlite, "BEGIN", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT DisableSpatialIndex('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      msg = wxT("Do you really intend to delete the MBR cache\n");
      msg += wxT("on column ");
      msg += obj->GetName();
      msg += wxT(".");
      msg += obj->GetColumn();
      msg += wxT(" ?");
      wxMessageDialog confirm(this, msg, wxT("Confirming Delete MBR cache"),
                              wxOK | wxCANCEL | wxICON_QUESTION);
      ret = confirm.ShowModal();
      if (ret != wxID_OK)
        return;
      ret =
        sqlite3_get_table(sqlite, sql.ToUTF8(), &results, &rows, &columns,
                          &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          goto rollback;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              if (results[(i * columns) + 0])
                retval = atoi(results[(i * columns) + 0]);
            }
        }
      sqlite3_free_table(results);
      if (!retval)
        goto rollback;
      sql = wxT("DROP TABLE IF EXISTS ");
      name = wxT("cache_");
      name += obj->GetName();
      name += wxT("_");
      name += obj->GetColumn();
      strcpy(xname, name.ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      ret = sqlite3_exec(sqlite, sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ret = sqlite3_exec(sqlite, "COMMIT", NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      ::wxEndBusyCursor();
      wxMessageBox(wxT("MBR cache cache_") + obj->GetName() +
                   wxT("_") + obj->GetColumn() +
                   wxT(" was successfully removed"), wxT("spatialite_gui"),
                   wxOK | wxICON_INFORMATION, this);
      MainFrame->InitTableTree();
    }
  return;
rollback:
  ret = sqlite3_exec(sqlite, "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("An error occurred\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdRebuildTriggers(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - rebuilding Geometry Triggers
//
  wxString sql;
  char xtable[1024];
  char xcolumn[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN || obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      strcpy(xtable, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xtable);
      strcpy(xcolumn, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xcolumn);
      sql = wxT("SELECT RebuildGeometryTriggers('");
      sql += wxString::FromUTF8(xtable);
      sql += wxT("', '");
      sql += wxString::FromUTF8(xcolumn);
      sql += wxT("')");
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::OnCmdGisLayerAuth(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - setting the GIS Layer auth
//
  wxString sql;
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  char *value;
  bool readOnly = false;
  bool hidden = false;
  GisLayerAuthDialog dlg;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED
      || obj->GetType() == MY_VIEW_GEOMETRY)
    {
      sql = wxT("SELECT read_only, hidden FROM geometry_columns AS a ");
      sql += wxT("LEFT JOIN geometry_columns_auth AS b ON ");
      sql += wxT("(Lower(a.f_table_name) = Lower(b.f_table_name) AND ");
      sql += wxT("Lower(a.f_geometry_column) = Lower(b.f_geometry_column)) ");
      sql += wxT("WHERE Lower(f_table_name) = Lower('");
      sql += obj->GetName();
      sql += wxT("') AND Lower(f_geometry_column) = Lower('");
      sql += obj->GetColumn();
      sql += wxT("')");
    }
  if (obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
      || obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      sql = wxT("SELECT read_only, hidden FROM views_geometry_columns ");
      sql += wxT("LEFT JOIN geometry_columns_auth AS x ON ");
      sql += wxT("(Lower(view_name) = Lower(x.f_table_name) AND ");
      sql += wxT("Lower(view_geometry) = Lower(x.f_geometry_column)) ");
      sql += wxT("WHERE Lower(view_name) = Lower('");
      sql += obj->GetName();
      sql += wxT("') AND Lower(view_geometry) = Lower('");
      sql += obj->GetColumn();
      sql += wxT("')");
    }
  if (obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      sql = wxT("SELECT read_only, hidden FROM virts_geometry_columns ");
      sql += wxT("LEFT JOIN geometry_columns_auth ON ");
      sql += wxT("(Lower(virt_name) = Lower(f_table_name) AND ");
      sql += wxT("Lower(virt_geometry) = Lower(f_geometry_column)) ");
      sql += wxT("WHERE Lower(virt_name) = Lower('");
      sql += obj->GetName();
      sql += wxT("') AND Lower(virt_geometry) = Lower('");
      sql += obj->GetColumn();
      sql += wxT("')");
    }
  if (sql.Len() == 0)
    return;
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 0];
          if (value)
            {
              if (atoi(value) == 0)
                readOnly = false;
              else
                readOnly = true;
            }
          value = results[(i * columns) + 1];
          if (value)
            {
              if (atoi(value) == 0)
                hidden = false;
              else
                hidden = true;
            }
        }
    }
  sqlite3_free_table(results);
  dlg.Create(MainFrame, obj->GetName(), obj->GetColumn(), readOnly, hidden);
  ret = dlg.ShowModal();
  if (ret == wxID_OK)
    {
      // updating the GEOMETRY_COLUMNS_AUTH table
      sql = wxT("INSERT OR REPLACE INTO geometry_columns_auth ");
      sql += wxT("(f_table_name, f_geometry_column, read_only, hidden) ");
      sql += wxT("VALUES ('");
      sql += obj->GetName();
      sql += wxT("', '");
      sql += obj->GetColumn();
      if (dlg.IsHidden() == true)
        sql += wxT("', 0, 1)");
      else if (dlg.IsReadOnly() == true)
        sql += wxT("', 1, 0)");
      else
        sql += wxT("', 0, 0)");
      ret =
        sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
        }
    }
}

void MyTableTree::OnCmdCheckGeometry(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - checking geometries
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN || obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      sql = wxT("SELECT Count(*), GeometryType(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("), Srid(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("), CoordDimension(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")\nFROM ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nGROUP BY 2, 3, 4");
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::OnCmdExtent(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - computing Extent
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN || obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED
      || obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
      || obj->GetType() == MY_VIRTUAL_COLUMN
      || obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      sql = wxT("SELECT Min(MbrMinX(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")), Min(MbrMinY(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")), Max(MbrMaxX(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")), Max(MbrMaxY(");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("))\nFROM ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::OnCmdUpdateLayerStatistics(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Updating Layer Statistics
//
  wxString sql;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN || obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED
      || obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
      || obj->GetType() == MY_VIRTUAL_COLUMN
      || obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      sql = wxT("SELECT UpdateLayerStatistics('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("', '");
      strcpy(xname, obj->GetColumn().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      MainFrame->SetSql(sql, true);
    }
  if (obj->GetType() == MY_VTABLE || obj->GetType() == MY_TABLE
      || obj->GetType() == MY_VIEW)
    {
      sql = wxT("SELECT UpdateLayerStatistics('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::
OnCmdUpdateLayerStatisticsAll(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Updating Layer Statistics [ALL]
//
  wxString sql = wxT("SELECT UpdateLayerStatistics()");
  MainFrame->SetSql(sql, true);
}

void MyTableTree::OnCmdElementaryGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - creating a derived table (elementary geometries)
//
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED
      || obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
      || obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      ElementaryGeomsDialog dlg;
      dlg.Create(MainFrame, obj->GetName(), obj->GetColumn());
      if (dlg.ShowModal() == wxID_OK)
        {
          wxString outTable = dlg.GetOutTable();
          wxString pKey = dlg.GetPrimaryKey();
          wxString multiID = dlg.GetMultiID();
          wxString type = dlg.GetType();
          int srid = dlg.GetSrid();
          wxString coordDims = dlg.GetCoordDims();
          bool spIdx = dlg.IsSpatialIndex();
          ::wxBeginBusyCursor();
          bool ret =
            MainFrame->DoElementaryGeometries(obj->GetName(), obj->GetColumn(),
                                              outTable, pKey, multiID, type,
                                              srid, coordDims, spIdx);
          ::wxEndBusyCursor();
          if (ret)
            MainFrame->InitTableTree();
        }
    }
}

void MyTableTree::OnCmdMalformedGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - identifying malformed geometries
//
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED
      || obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
      || obj->GetType() == MY_VIRTUAL_GEOMETRY)
    {
      bool repair = true;
      if (obj->GetType() == MY_VIEW_GEOMETRY
          || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
          || obj->GetType() == MY_VIEW_GEOMETRY_CACHED
          || obj->GetType() == MY_VIRTUAL_GEOMETRY)
        repair = false;
      MalformedGeomsList *list =
        MainFrame->FindMalformedGeoms(obj->GetName(), obj->GetColumn(), repair);
      if (list->GetFirst() == NULL)
        {
          delete list;
          wxString msg = wxT("Any geometry found in ");
          msg += obj->GetName();
          msg += wxT(".");
          msg += obj->GetColumn();
          msg += wxT(" is valid");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                       this);
          return;
      } else
        {
          MalformedGeomsDialog dlg;
          dlg.Create(MainFrame, obj->GetName(), obj->GetColumn(), list);
          dlg.ShowModal();
        }
    }
}

void MyTableTree::OnCmdRepairPolygons(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - attempting to repair malformed polygons
//
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      int count;
      ::wxBeginBusyCursor();
      MainFrame->PreRepairPolygons(obj->GetName(), obj->GetColumn(), &count);
      if (count == 0)
        {
          wxString msg = wxT("No Polygon to be repaired found in ");
          msg += obj->GetName();
          msg += wxT(".");
          msg += obj->GetColumn();
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_INFORMATION,
                       this);
          ::wxEndBusyCursor();
          return;
        }
      MainFrame->RepairPolygons(obj->GetName(), obj->GetColumn(), &count);
      if (count > 0)
        {
          char str[256];
          sprintf(str, "%d Geometries were succesfully updated", count);
          wxMessageBox(wxString::FromUTF8(str), wxT("spatialite_gui"),
                       wxOK | wxICON_INFORMATION, this);
          ::wxEndBusyCursor();
          return;
        }
      ::wxEndBusyCursor();
    }
}

void MyTableTree::OnCmdSetSrid(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - setting SRID for geometries
//
  SetSridDialog dlg;
  wxString sql;
  int srid;
  int oldSrid;
  int ret;
  char dummy[128];
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_COLUMN)
    {
      dlg.Create(MainFrame, obj->GetName(), obj->GetColumn());
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          srid = dlg.GetSrid();
          oldSrid = dlg.GetOldSrid();
          sql = wxT("UPDATE ");
          strcpy(xname, obj->GetName().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          sql += wxString::FromUTF8(xname);
          sql += wxT(" SET ");
          strcpy(xname, obj->GetColumn().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          sql += wxString::FromUTF8(xname);
          sql += wxT(" = SetSrid(");
          strcpy(xname, obj->GetColumn().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          sql += wxString::FromUTF8(xname);
          sprintf(dummy, ", %d)", srid);
          sql += wxString::FromUTF8(dummy);
          sql += wxT("\nWHERE Srid(");
          strcpy(xname, obj->GetColumn().ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          sql += wxString::FromUTF8(xname);
          sprintf(dummy, ") = %d", oldSrid);
          sql += wxString::FromUTF8(dummy);
          MainFrame->SetSql(sql, true);
        }
    }
}

void MyTableTree::OnCmdDumpShp(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as Shapefile
//
  int ret;
  wxString path;
  wxString lastDir;
  bool isView = false;
  int metadata_type = MainFrame->GetMetaDataType();
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED)
    isView = true;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED || isView == true)
    {
      wxFileDialog fileDialog(this, wxT("Dump Shapefile"),
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
          char x_path[1024];
          char x_table[1024];
          char x_column[1024];
          char x_type[1024];
          char x_charset[1024];
          char err_msg[1024];
          int rt;
          char **results;
          int rows;
          int columns;
          int i;
          char *errMsg = NULL;
          *x_type = '\0';
          wxFileName file(fileDialog.GetPath());
          path = file.GetPath();
          path += file.GetPathSeparator();
          path += file.GetName();
          lastDir = file.GetPath();
          strcpy(x_path, path.ToUTF8());
          strcpy(x_table, obj->GetName().ToUTF8());
          strcpy(x_column, obj->GetColumn().ToUTF8());
          if (isView == true)
            {
              wxString sql;
              if (metadata_type == METADATA_CURRENT)
                sql =
                  wxT("SELECT geometry_type FROM views_geometry_columns AS a ");
              else if (metadata_type == METADATA_LEGACY)
                sql = wxT("SELECT type FROM views_geometry_columns AS a ");
              sql += wxT("JOIN geometry_columns AS b ON (");
              sql += wxT("Lower(a.f_table_name) = Lower(b.f_table_name) AND ");
              sql +=
                wxT
                ("Lower(a.f_geometry_column) = Lower(b.f_geometry_column)) ");
              sql += wxT("WHERE Lower(view_name) = Lower('");
              sql += obj->GetName();
              sql += wxT("') AND Lower(view_geometry) = Lower('");
              sql += obj->GetColumn();
              sql += wxT("')");
              ret =
                sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(),
                                  &results, &rows, &columns, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("dump shapefile error:") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  return;
                }
              if (rows < 1)
                ;
              else
                {
                  for (i = 1; i <= rows; i++)
                    {
                      if (metadata_type == METADATA_LEGACY)
                        strcpy(x_type, results[(i * columns) + 0]);
                      else if (metadata_type == METADATA_CURRENT)
                        {
                          switch (atoi(results[(i * columns) + 0]))
                            {
                              case 0:
                              case 1000:
                              case 2000:
                              case 3000:
                                strcpy(x_type, "GEOMETRY");
                                break;
                              case 1:
                              case 1001:
                              case 2001:
                              case 3001:
                                strcpy(x_type, "POINT");
                                break;
                              case 2:
                              case 1002:
                              case 2002:
                              case 3002:
                                strcpy(x_type, "LINESTRING");
                                break;
                              case 3:
                              case 1003:
                              case 2003:
                              case 3003:
                                strcpy(x_type, "POLYGON");
                                break;
                              case 4:
                              case 1004:
                              case 2004:
                              case 3004:
                                strcpy(x_type, "MULTIPOINT");
                                break;
                              case 5:
                              case 1005:
                              case 2005:
                              case 3005:
                                strcpy(x_type, "MULTILINESTRING");
                                break;
                              case 6:
                              case 1006:
                              case 2006:
                              case 3006:
                                strcpy(x_type, "MULTIPOLYGON");
                                break;
                              case 7:
                              case 1007:
                              case 2007:
                              case 3007:
                                strcpy(x_type, "GEOMETRYCOLLECTION");
                                break;
                            };
                        }
                    }
                }
              sqlite3_free_table(results);
          } else
            {
              wxString sql;
              if (metadata_type == METADATA_CURRENT)
                sql =
                  wxT
                  ("SELECT geometry_type FROM geometry_columns WHERE Lower(f_table_name) = Lower('");
              else if (metadata_type == METADATA_LEGACY)
                sql =
                  wxT
                  ("SELECT type FROM geometry_columns WHERE Lower(f_table_name) = Lower('");
              sql += obj->GetName();
              sql += wxT("') AND Lower(f_geometry_column) = Lower('");
              sql += obj->GetColumn();
              sql += wxT("')");
              ret =
                sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(),
                                  &results, &rows, &columns, &errMsg);
              if (ret != SQLITE_OK)
                {
                  wxMessageBox(wxT("dump shapefile error:") +
                               wxString::FromUTF8(errMsg),
                               wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                               this);
                  sqlite3_free(errMsg);
                  return;
                }
              if (rows < 1)
                ;
              else
                {
                  for (i = 1; i <= rows; i++)
                    {
                      if (metadata_type == METADATA_LEGACY)
                        strcpy(x_type, results[(i * columns) + 0]);
                      else if (metadata_type == METADATA_CURRENT)
                        {
                          switch (atoi(results[(i * columns) + 0]))
                            {
                              case 0:
                              case 1000:
                              case 2000:
                              case 3000:
                                strcpy(x_type, "GEOMETRY");
                                break;
                              case 1:
                              case 1001:
                              case 2001:
                              case 3001:
                                strcpy(x_type, "POINT");
                                break;
                              case 2:
                              case 1002:
                              case 2002:
                              case 3002:
                                strcpy(x_type, "LINESTRING");
                                break;
                              case 3:
                              case 1003:
                              case 2003:
                              case 3003:
                                strcpy(x_type, "POLYGON");
                                break;
                              case 4:
                              case 1004:
                              case 2004:
                              case 3004:
                                strcpy(x_type, "MULTIPOINT");
                                break;
                              case 5:
                              case 1005:
                              case 2005:
                              case 3005:
                                strcpy(x_type, "MULTILINESTRING");
                                break;
                              case 6:
                              case 1006:
                              case 2006:
                              case 3006:
                                strcpy(x_type, "MULTIPOLYGON");
                                break;
                              case 7:
                              case 1007:
                              case 2007:
                              case 3007:
                                strcpy(x_type, "GEOMETRYCOLLECTION");
                                break;
                            };
                        }
                    }
                }
              sqlite3_free_table(results);
            }
          if (MainFrame->IsSetAskCharset() == false)
            {
              // using the default output charset
              MainFrame->SetLastDirectory(lastDir);
              ::wxBeginBusyCursor();
              strcpy(x_charset, MainFrame->GetDefaultCharset().ToUTF8());
              rt =
                dump_shapefile(MainFrame->GetSqlite(), x_table, x_column,
                               x_path, x_charset, x_type, 0, &rows, err_msg);
              ::wxEndBusyCursor();
              if (rt)
                wxMessageBox(wxT("dump shp:") +
                             wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                             wxOK | wxICON_INFORMATION, this);
              else
                wxMessageBox(wxT("dump shp error:") +
                             wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                             wxOK | wxICON_ERROR, this);
          } else
            {
              // asking the charset to be used
              DumpShpDialog dlg;
              dlg.Create(MainFrame, path, obj->GetName(), obj->GetColumn(),
                         MainFrame->GetDefaultCharset());
              ret = dlg.ShowModal();
              if (ret == wxID_OK)
                {
                  MainFrame->SetLastDirectory(lastDir);
                  ::wxBeginBusyCursor();
                  strcpy(x_charset, dlg.GetCharset().ToUTF8());
                  rt =
                    dump_shapefile(MainFrame->GetSqlite(), x_table, x_column,
                                   x_path, x_charset, x_type, 0, &rows,
                                   err_msg);
                  ::wxEndBusyCursor();
                  if (rt)
                    wxMessageBox(wxT("dump shp:") +
                                 wxString::FromUTF8(err_msg),
                                 wxT("spatialite_gui"),
                                 wxOK | wxICON_INFORMATION, this);
                  else
                    wxMessageBox(wxT("dump shp error:") +
                                 wxString::FromUTF8(err_msg),
                                 wxT("spatialite_gui"), wxOK | wxICON_ERROR,
                                 this);
                }
            }
        }
    }
}

void MyTableTree::OnCmdDumpKml(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as KML
//
  int ret;
  wxString path;
  wxString lastDir;
  bool isView = false;
  bool isVirtual = false;
  bool isNameConst = false;
  bool isDescConst = false;
  wxString Name;
  wxString Desc;
  int precision = -1;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_VIEW_GEOMETRY
      || obj->GetType() == MY_VIEW_GEOMETRY_INDEX
      || obj->GetType() == MY_VIEW_GEOMETRY_CACHED)
    isView = true;
  if (obj->GetType() == MY_VIRTUAL_GEOMETRY)
    isVirtual = true;
  if (obj->GetType() == MY_GEOMETRY || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED || isView == true
      || isVirtual == true)
    {
      DumpKmlDialog dlg;
      dlg.Create(MainFrame, obj->GetName(), obj->GetColumn());
      ret = dlg.ShowModal();
      if (ret == wxID_OK)
        {
          isNameConst = dlg.IsNameConst();
          Name = dlg.GetName();
          isDescConst = dlg.IsDescConst();
          Desc = dlg.GetDesc();
          precision = dlg.GetPrecision();
      } else
        return;
      wxFileDialog fileDialog(this, wxT("Dump KML"),
                              wxT(""), wxT("export_file.kml"),
                              wxT
                              ("KML file (*.kml)|*.kml|All files (*.*)|*.*"),
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
          path += wxT(".kml");
          lastDir = file.GetPath();
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          MainFrame->DumpKml(path, obj->GetName(), obj->GetColumn(), precision,
                             Name, isNameConst, Desc, isDescConst);
          ::wxEndBusyCursor();
        }
    }
}

void MyTableTree::OnCmdDumpTxtTab(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as TxtTab
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  wxFileDialog fileDialog(this, wxT("Dump Txt/Tab file"),
                          wxT(""), wxT("table.txt"),
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
          MainFrame->DumpTxtTab(path, obj->GetName(),
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
              MainFrame->DumpTxtTab(path, obj->GetName(), dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyTableTree::OnCmdDumpCsv(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as CSV
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  wxFileDialog fileDialog(this, wxT("Dump CSV file"),
                          wxT(""), wxT("table.csv"),
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
          MainFrame->DumpCsv(path, obj->GetName(),
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
              MainFrame->DumpCsv(path, obj->GetName(), dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyTableTree::OnCmdDumpHtml(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as Html
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  wxFileDialog fileDialog(this, wxT("Dump HTML file"),
                          wxT(""), wxT("table.html"),
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
          MainFrame->DumpHtml(path, obj->GetName(), MainFrame->GetSqlitePath(),
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
              MainFrame->DumpHtml(path, obj->GetName(),
                                  MainFrame->GetSqlitePath(), dlg.GetCharset());
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyTableTree::OnCmdDumpDif(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as DIF
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
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
  wxFileDialog fileDialog(this, wxT("Dump DIF file"),
                          wxT(""), wxT("table.dif"),
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
          MainFrame->DumpDif(path, obj->GetName(),
                             MainFrame->GetDefaultCharset(), decimal_point,
                             date_times);
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
              MainFrame->DumpDif(path, obj->GetName(), dlg.GetCharset(),
                                 decimal_point, date_times);
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyTableTree::OnCmdDumpSylk(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as SYLK
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
// asking Decimal Point / Date-Times params
  DumpSpreadsheetDialog sheet_dlg;
  bool date_times;
  sheet_dlg.Create(MainFrame);
  ret = sheet_dlg.ShowModal();
  if (ret == wxID_OK)
    date_times = sheet_dlg.IsDateTimes();
  else
    return;
  wxFileDialog fileDialog(this, wxT("Dump SYLK file"),
                          wxT(""), wxT("table.slk"),
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
          MainFrame->DumpSylk(path, obj->GetName(),
                              MainFrame->GetDefaultCharset(), date_times);
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
              MainFrame->DumpSylk(path, obj->GetName(), dlg.GetCharset(),
                                  date_times);
              ::wxEndBusyCursor();
            }
        }
    }
}

void MyTableTree::OnCmdDumpDbf(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - dumping as DBF
//
  int ret;
  wxString path;
  wxString lastDir;
  wxString target;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  wxFileDialog fileDialog(this, wxT("Dump DBF file"),
                          wxT(""), wxT("table.dbf"),
                          wxT
                          ("DBF archive (*.dbf)|*.dbf|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      char x_path[1024];
      char x_table[1024];
      char x_charset[1024];
      char err_msg[1024];
      int rt;
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".dbf");
      lastDir = file.GetPath();
      strcpy(x_path, path.ToUTF8());
      strcpy(x_table, obj->GetName().ToUTF8());
      if (MainFrame->IsSetAskCharset() == false)
        {
          // using the default output charset
          MainFrame->SetLastDirectory(lastDir);
          ::wxBeginBusyCursor();
          strcpy(x_charset, MainFrame->GetDefaultCharset().ToUTF8());
          rt =
            dump_dbf(MainFrame->GetSqlite(), x_table, x_path, x_charset,
                     err_msg);
          ::wxEndBusyCursor();
          if (rt)
            wxMessageBox(wxT("dump dbf:") +
                         wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                         wxOK | wxICON_INFORMATION, this);
          else
            wxMessageBox(wxT("dump dbf error:") +
                         wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                         wxOK | wxICON_ERROR, this);
      } else
        {
          // asking the charset to be used
          DumpTxtDialog dlg;
          target = wxT("DBF archive");
          dlg.Create(MainFrame, path, obj->GetName(),
                     MainFrame->GetDefaultCharset());
          ret = dlg.ShowModal();
          if (ret == wxID_OK)
            {
              MainFrame->SetLastDirectory(lastDir);
              strcpy(x_charset, dlg.GetCharset().ToUTF8());
              ::wxBeginBusyCursor();
              rt =
                dump_dbf(MainFrame->GetSqlite(), x_table, x_path, x_charset,
                         err_msg);
              ::wxEndBusyCursor();
              if (rt)
                wxMessageBox(wxT("dump dbf:") +
                             wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                             wxOK | wxICON_INFORMATION, this);
              else
                wxMessageBox(wxT("dump dbf error:") +
                             wxString::FromUTF8(err_msg), wxT("spatialite_gui"),
                             wxOK | wxICON_ERROR, this);
            }
        }
    }
}

void MyTableTree::OnCmdDumpPostGIS(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - SQL dump for PostGIS
//
  wxString sql;
  sqlite3_stmt *stmt;
  char xname[1024];
  wxString lastDir;
  int ret;
  DumpPostGISDialog postgis_dlg;
  bool lowercase;
  bool create_table;
  bool spatial_index;
  wxString schema_name;
  wxString table_name;
  int rows = 0;
  int n_cols;
  int i;
  gaiaGeomCollPtr geom;
  PostGISHelper postgis;
  wxFileDialog fileDialog(this, wxT("SQL Dump for PostGIS"),
                          wxT(""), wxT("postgis.sql"),
                          wxT
                          ("SQL dump (*.sql)|*.sql|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;

// asking PostGIS options
  postgis_dlg.Create(MainFrame, obj->GetName());
  ret = postgis_dlg.ShowModal();
  if (ret == wxID_OK)
    {
      lowercase = postgis_dlg.IsLowercase();
      create_table = postgis_dlg.IsCreateTable();
      spatial_index = postgis_dlg.IsSpatialIndex();
      schema_name = postgis_dlg.GetSchemaName();
      table_name = postgis_dlg.GetTableName();
  } else
    return;

//
// preparing SQL statement 
//
  sql = wxT("SELECT * FROM ");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
//
// compiling SQL prepared statement 
//
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), sql.ToUTF8(), sql.Len(), &stmt,
                       NULL);
  if (ret != SQLITE_OK)
    goto sql_error;

  ::wxBeginBusyCursor();
  while (1)
    {
      ret = sqlite3_step(stmt);
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          n_cols = sqlite3_column_count(stmt);
          if (rows == 0)
            {
              // setting the column names
              postgis.Alloc(n_cols);
              for (i = 0; i < n_cols; i++)
                postgis.SetName(i, (const char *) sqlite3_column_name(stmt, i));
            }
          rows++;
          for (i = 0; i < n_cols; i++)
            {
              if (sqlite3_column_type(stmt, i) == SQLITE_INTEGER)
                postgis.Eval(i, sqlite3_column_int64(stmt, i));
              else if (sqlite3_column_type(stmt, i) == SQLITE_FLOAT)
                postgis.Eval(i, sqlite3_column_double(stmt, i));
              else if (sqlite3_column_type(stmt, i) == SQLITE_TEXT)
                postgis.Eval(i, (const char *) sqlite3_column_text(stmt, i));
              else if (sqlite3_column_type(stmt, i) == SQLITE_BLOB)
                {
                  const void *blob_value = sqlite3_column_blob(stmt, i);
                  int len = sqlite3_column_bytes(stmt, i);
                  geom =
                    gaiaFromSpatiaLiteBlobWkb((unsigned char *) blob_value,
                                              len);
                  if (geom)
                    {
                      postgis.Eval(i, geom);
                      gaiaFreeGeomColl(geom);
                  } else
                    postgis.EvalBlob(i);
              } else
                postgis.Eval(i);
            }
      } else
        {
          sqlite3_finalize(stmt);
          goto sql_error;
        }
    }
  postgis.GetKeys(MainFrame, obj->GetName());
  postgis.Prepare();
  ::wxEndBusyCursor();

// rewinding the result-set
  ret = sqlite3_reset(stmt);
  if (ret != SQLITE_OK)
    {
      sqlite3_finalize(stmt);
      goto sql_error;
    }

  lastDir = MainFrame->GetLastDirectory();
  if (lastDir.Len() >= 1)
    fileDialog.SetDirectory(lastDir);
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      wxString path;
      wxFileName file(fileDialog.GetPath());
      path = file.GetPath();
      path += file.GetPathSeparator();
      path += file.GetName();
      path += wxT(".sql");
      lastDir = file.GetPath();
      char x_path[1024];
      strcpy(x_path, path.ToUTF8());
      FILE *out = fopen(x_path, "wb");
      if (out == NULL)
        {
          wxMessageBox(wxT
                       ("PostGIS SQL dump error: unable to create output file"),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto stop;
        }
      ::wxBeginBusyCursor();
      fprintf(out,
              "--\n-- SQL dump automatically generated by \"spatialite_gui\" [GPLv3]\n");
      wxDateTime now = wxDateTime::Now();
      fprintf(out, "-- created on: %04d-%02d-%02d %02d:%02d:%02d\n--\n",
              now.GetYear(), now.GetMonth() + 1, now.GetDay(), now.GetHour(),
              now.GetMinute(), now.GetSecond());
      strcpy(x_path, MainFrame->GetSqlitePath().ToUTF8());
      fprintf(out, "-- DB-file origin: %s\n", x_path);
      fprintf(out, "-- Table origin: %s\n--\n", xname);
      fprintf(out, "-- intended target is: PostgreSQL + PostGIS\n--\n\n");
      if (create_table)
        {
          fprintf(out, "CREATE TABLE ");
          if (schema_name.Len() > 0)
            {
              strcpy(xname, schema_name.ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              fprintf(out, "%s.", xname);
            }
          strcpy(xname, table_name.ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          fprintf(out, "%s (\n", xname);
          for (i = 0; i < postgis.GetCount(); i++)
            {
              if (postgis.IsGeometry(i) == true)
                {
                  // skipping any Geometry column
                  continue;
                }
              if (postgis.GetDataType(i) == postgis.DATA_TYPE_UNDEFINED)
                {
                  // skipping any invalid column
                  continue;
                }
              strcpy(xname, postgis.GetName(i, lowercase).ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              char data_type[128];
              postgis.GetDataType(i, data_type);
              if (i == 0)
                fprintf(out, "\t%s %s", xname, data_type);
              else
                fprintf(out, ",\n\t%s %s", xname, data_type);
            }
// definining any Primary Key Constraint (if any)
          PostGISIndex *idx = postgis.GetFirstIndex();
          while (idx)
            {
              if (idx->IsPrimaryKey() != true)
                {
                  idx = idx->GetNext();
                  continue;
                }
              if (postgis.IsSingleFieldPrimaryKey() == true)
                {
                  idx = idx->GetNext();
                  continue;
                }
              fprintf(out, "m\nCONSTRAINT ");
              strcpy(xname, idx->GetName().ToUTF8());
              MyFrame::DoubleQuotedSql(xname);
              fprintf(out, "%s PRIMARY KEY (", xname);
              PostGISIndexField *idx_fld = idx->GetFirst();
              while (idx_fld)
                {
                  if (idx_fld == idx->GetFirst())
                    ;
                  else
                    fprintf(out, ", ");
                  strcpy(xname, idx_fld->GetColumnRef()->GetName().ToUTF8());
                  MyFrame::DoubleQuotedSql(xname);
                  fprintf(out, "%s", xname);
                  idx_fld = idx_fld->GetNext();
                }
              fprintf(out, ")");
              idx = idx->GetNext();
            }
          fprintf(out, ");\n\n");
          for (i = 0; i < postgis.GetCount(); i++)
            {
              if (postgis.IsGeometry(i) != true)
                {
                  // skipping any not-Geometry column
                  continue;
                }
              fprintf(out, "SELECT AddGeometryColumn(");
              if (schema_name.Len() > 0)
                {
                  strcpy(xname, schema_name.ToUTF8());
                  MyFrame::CleanSqlString(xname);
                  fprintf(out, "'%s', ", xname);
                }
              strcpy(xname, table_name.ToUTF8());
              MyFrame::CleanSqlString(xname);
              fprintf(out, "'%s', ", xname);
              strcpy(xname, postgis.GetName(i, lowercase).ToUTF8());
              MyFrame::CleanSqlString(xname);
              fprintf(out, "'%s', %d, ", xname, postgis.GetSrid(i));
              switch (postgis.GetDataType(i))
                {
                  case PostGISHelper::DATA_TYPE_POINT:
                    fprintf(out, "'POINT");
                    break;
                  case PostGISHelper::DATA_TYPE_MULTIPOINT:
                    fprintf(out, "'MULTIPOINT");
                    break;
                  case PostGISHelper::DATA_TYPE_LINESTRING:
                    fprintf(out, "'LINESTRING");
                    break;
                  case PostGISHelper::DATA_TYPE_MULTILINESTRING:
                    fprintf(out, "'MULTILINESTRING");
                    break;
                  case PostGISHelper::DATA_TYPE_POLYGON:
                    fprintf(out, "'POLYGON");
                    break;
                  case PostGISHelper::DATA_TYPE_MULTIPOLYGON:
                    fprintf(out, "'MULTIPOLYGON");
                    break;
                  case PostGISHelper::DATA_TYPE_GEOMETRYCOLLECTION:
                    fprintf(out, "'GEOMETRYCOLLECTION");
                    break;
                  default:
                    fprintf(out, "'GEOMETRY");
                    break;
                };
              switch (postgis.GetCoordDims(i))
                {
                  case GAIA_XY_Z_M:
                    fprintf(out, "', 4");
                    break;
                  case GAIA_XY_Z:
                    fprintf(out, "', 3");
                    break;
                  case GAIA_XY_M:
                    fprintf(out, "M', 3");
                    break;
                  default:
                    fprintf(out, "', 2");
                    break;
                }
              fprintf(out, ");\n");
              if (spatial_index)
                {
                  fprintf(out, "CREATE INDEX ");
                  wxString idx_name = wxT("idx_");
                  idx_name += table_name;
                  idx_name += wxT("_");
                  idx_name += postgis.GetName(i, lowercase);
                  strcpy(xname, idx_name.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  fprintf(out, "%s ON ", xname);
                  if (schema_name.Len() > 0)
                    {
                      strcpy(xname, schema_name.ToUTF8());
                      MyFrame::DoubleQuotedSql(xname);
                      fprintf(out, "%s.", xname);
                    }
                  strcpy(xname, table_name.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  fprintf(out, "%s USING GIST (", xname);
                  strcpy(xname, postgis.GetName(i, lowercase).ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  fprintf(out, "%s);\n", xname);
                }
            }

          idx = postgis.GetFirstIndex();
          while (idx)
            {
              if (idx->IsPrimaryKey() == true)
                {
                  idx = idx->GetNext();
                  continue;
                }
              if (idx->IsUnique() == true)
                fprintf(out, "CREATE UNIQUE INDEX ");
              else
                fprintf(out, "CREATE INDEX ");
              strcpy(xname, idx->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              fprintf(out, "%s ON ", xname);
              if (schema_name.Len() > 0)
                {
                  strcpy(xname, schema_name.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  fprintf(out, "%s.", xname);
                }
              strcpy(xname, table_name.ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              fprintf(out, "%s (", xname);
              PostGISIndexField *idx_fld = idx->GetFirst();
              while (idx_fld)
                {
                  if (idx_fld != idx->GetFirst())
                    fprintf(out, ", ");
                  strcpy(xname, idx_fld->GetColumnRef()->GetName().ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  fprintf(out, "%s", xname);
                  idx_fld = idx_fld->GetNext();
                }
              fprintf(out, ");\n");
              idx = idx->GetNext();
            }
          fprintf(out, "\n");

          rows = 0;
          fprintf(out, "BEGIN;\n");
          while (1)
            {
              ret = sqlite3_step(stmt);
              if (ret == SQLITE_DONE)
                break;          // end of result set
              if (ret == SQLITE_ROW)
                {
                  if (rows > 0)
                    {
                      if ((rows % 1000) == 0)
                        {
                          // COMMIT and then restarts a new Transaction
                          fprintf(out, "COMMIT;\n\n");
                          fprintf(out, "-- %d rows\n\nBEGIN;\n", rows);
                        }
                    }
                  rows++;
                  fprintf(out, "INSERT INTO ");
                  if (schema_name.Len() > 0)
                    {
                      strcpy(xname, schema_name.ToUTF8());
                      MyFrame::DoubleQuotedSql(xname);
                      fprintf(out, "%s.", xname);
                    }
                  strcpy(xname, table_name.ToUTF8());
                  MyFrame::DoubleQuotedSql(xname);
                  fprintf(out, "%s (", xname);
                  for (i = 0; i < postgis.GetCount(); i++)
                    {
                      if (postgis.GetDataType(i) == postgis.DATA_TYPE_UNDEFINED)
                        {
                          // skipping any invalid column
                          continue;
                        }
                      strcpy(xname, postgis.GetName(i, lowercase).ToUTF8());
                      MainFrame->DoubleQuotedSql(xname);
                      if (i == 0)
                        fprintf(out, "%s", xname);
                      else
                        fprintf(out, ", %s", xname);
                    }
                  fprintf(out, ") VALUES (");
                  for (i = 0; i < n_cols; i++)
                    {
                      if (i > 0)
                        fprintf(out, ", ");
                      int type = sqlite3_column_type(stmt, i);
                      int data_type = postgis.GetDataType(i);
                      switch (type)
                        {
                          case SQLITE_NULL:
                            fprintf(out, "NULL");
                            break;
                          case SQLITE_INTEGER:
                            if (data_type == PostGISHelper::DATA_TYPE_BOOLEAN)
                              postgis.OutputBooleanValue(out,
                                                         sqlite3_column_int64
                                                         (stmt, i));
                            else
                              postgis.OutputValue(out,
                                                  sqlite3_column_int64(stmt,
                                                                       i));
                            break;
                          case SQLITE_FLOAT:
                            postgis.OutputValue(out,
                                                sqlite3_column_double(stmt, i));
                            break;
                          case SQLITE_TEXT:
                            postgis.OutputValue(out,
                                                (const char *)
                                                sqlite3_column_text(stmt, i));
                            break;
                          case SQLITE_BLOB:
                            if (postgis.IsGeometry(i))
                              {
                                const void *blob_value;
                                blob_value = sqlite3_column_blob(stmt, i);
                                int len = sqlite3_column_bytes(stmt, i);
                                geom =
                                  gaiaFromSpatiaLiteBlobWkb((unsigned char *)
                                                            blob_value, len);
                                if (geom == NULL)
                                  fprintf(out, "NULL");
                                else
                                  {
                                    postgis.OutputValue(out, geom);
                                    gaiaFreeGeomColl(geom);
                                  }
                            } else
                              {
                                int len = sqlite3_column_bytes(stmt, i);
                                postgis.OutputValue(out,
                                                    (unsigned char *)
                                                    sqlite3_column_blob(stmt,
                                                                        i),
                                                    len);
                              }
                            break;
                        };
                    }
                  fprintf(out, ");\n");
              } else
                {
                  sqlite3_finalize(stmt);
                  goto sql_error;
                }
            }
          fprintf(out, "COMMIT;\n");
        }


      fprintf(out, "\n--\n-- end SQL dump\n");
      fprintf(out, "--\n");
      ::wxEndBusyCursor();
      fclose(out);
    }

stop:
  sqlite3_finalize(stmt);
  return;

sql_error:
//
// some SQL error occurred
//
  sqlite3_finalize(stmt);
  ::wxEndBusyCursor();
  wxMessageBox(wxT("PostGIS SQL dump error:") +
               wxString::FromUTF8(sqlite3_errmsg(MainFrame->GetSqlite())),
               wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
  return;
}

void MyTableTree::OnCmdMapPreview(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Map Preview
//
  char table_name[1024];
  char column_name[1024];
  char sql[4192];
  char sql2[2048];
  int ret;
  char err_msg[2048];
  sqlite3_stmt *stmt;
  double minx = DBL_MAX;
  double miny = DBL_MAX;
  double maxx = DBL_MAX;
  double maxy = DBL_MAX;

  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;

  ::wxBeginBusyCursor();
  strcpy(table_name, obj->GetName().ToUTF8());
  strcpy(column_name, obj->GetColumn().ToUTF8());
  MainFrame->DoubleQuotedSql(table_name);
  MainFrame->DoubleQuotedSql(column_name);
  sprintf(sql, "SELECT Min(MbrMinX(%s)), Min(MbrMinY(%s)), ", column_name,
          column_name);
  sprintf(sql2, "Max(MbrMaxX(%s)), Max(MbrMaxY(%s)) ", column_name,
          column_name);
  strcat(sql, sql2);
  sprintf(sql2, "FROM %s", table_name);
  strcat(sql, sql2);

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
          if (sqlite3_column_type(stmt, 0) == SQLITE_FLOAT)
            minx = sqlite3_column_double(stmt, 0);
          if (sqlite3_column_type(stmt, 1) == SQLITE_FLOAT)
            miny = sqlite3_column_double(stmt, 1);
          if (sqlite3_column_type(stmt, 2) == SQLITE_FLOAT)
            maxx = sqlite3_column_double(stmt, 2);
          if (sqlite3_column_type(stmt, 3) == SQLITE_FLOAT)
            maxy = sqlite3_column_double(stmt, 3);
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

  if (minx == DBL_MAX || miny == DBL_MAX || maxx == DBL_MAX || maxy == DBL_MAX)
    {
      wxMessageBox(wxT("This Column doesn't contains any Geometry: sorry ..."),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return;
    }

  MapPreviewDialog dlg;
  dlg.Create(MainFrame, obj->GetName(), obj->GetColumn(), minx, miny, maxx,
             maxy);
  dlg.ShowModal();
}

void MyTableTree::OnCmdColumnStats(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - column stats
//
  char table_name[1024];
  char column_name[1024];
  char sql[4192];
  char sql2[4192];
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  const char *value;
  char err_msg[2048];
  sqlite3_stmt *stmt;
  int count;
  int null_count = 0;
  int text_count = 0;
  int integer_count = 0;
  int real_count = 0;
  int blob_count = 0;
  double min;
  double max;
  double avg;
  double stddev_pop;
  double stddev_samp;
  double var_pop;
  double var_samp;
  int distinct_values = 0;

  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;

  ::wxBeginBusyCursor();
  strcpy(table_name, obj->GetName().ToUTF8());
  strcpy(column_name, obj->GetColumn().ToUTF8());
  MainFrame->DoubleQuotedSql(table_name);
  MainFrame->DoubleQuotedSql(column_name);
  sprintf(sql, "SELECT Typeof(%s), Count(*) FROM %s GROUP BY Typeof(%s)",
          column_name, table_name, column_name);

  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql, &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 0];
          count = atoi(results[(i * columns) + 1]);
          if (strcasecmp(value, "null") == 0)
            null_count += count;
          if (strcasecmp(value, "text") == 0)
            text_count += count;
          if (strcasecmp(value, "integer") == 0)
            integer_count += count;
          if (strcasecmp(value, "real") == 0)
            real_count += count;
          if (strcasecmp(value, "blob") == 0)
            blob_count += count;
        }
    }
  sqlite3_free_table(results);

  if ((real_count + integer_count) > 0)
    {
      // computing statistic analysis
      sprintf(sql, "SELECT Min(%s), Max(%s), Avg(%s), ", column_name,
              column_name, column_name);
      sprintf(sql2, "StdDev_pop(%s), StdDev_samp(%s), ", column_name,
              column_name);
      strcat(sql, sql2);
      sprintf(sql2, "Var_pop(%s), Var_samp(%s) FROM %s", column_name,
              column_name, table_name);
      strcat(sql, sql2);

      ret = sqlite3_get_table(MainFrame->GetSqlite(), sql, &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          ::wxEndBusyCursor();
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              min = atof(results[(i * columns) + 0]);
              max = atof(results[(i * columns) + 1]);
              avg = atof(results[(i * columns) + 2]);
              stddev_pop = atof(results[(i * columns) + 3]);
              stddev_samp = atof(results[(i * columns) + 4]);
              var_pop = atof(results[(i * columns) + 5]);
              var_samp = atof(results[(i * columns) + 6]);
            }
        }
      sqlite3_free_table(results);
    }
// computing DISTINCT values
  sprintf(sql, "SELECT DISTINCT %s FROM %s", column_name, table_name);
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
          distinct_values++;
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

  ColumnStatsDialog dlg;
  dlg.Create(MainFrame, obj->GetName(), obj->GetColumn(), null_count,
             text_count, integer_count, real_count, blob_count, min, max, avg,
             stddev_pop, stddev_samp, var_pop, var_samp, distinct_values);
  dlg.ShowModal();
}

bool MyTableTree::DropRenameAux1(MyObject * obj, GeomColsList * Geometries,
                                 bool * autoincrement)
{
//
// common tasks: drop/rename column auxiliaries
//
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  bool check_autoincrement = false;
  wxString geomColumn;
  wxString geomType;
  wxString coordDims;
  int geomSrid;
  int indexType;
  wxString indexName;
  char xname[1024];
  char *value;
  int metadata_type;

// checking if the SQLITE_SEQUENCE table exists
  sql = wxT("PRAGMA table_info(sqlite_sequence)");
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          check_autoincrement = true;
        }
    }
  sqlite3_free_table(results);
  if (check_autoincrement == true)
    {
// checking if there is an AUTOINCREMENT Primary Key
      sql = wxT("SELECT name FROM sqlite_sequence WHERE Lower(name) = Lower('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("')");
      ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return false;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              *autoincrement = true;
            }
        }
      sqlite3_free_table(results);
    }
// checking if there is some Spatial Index already defined
  metadata_type = MainFrame->GetMetaDataType();
  if (metadata_type == METADATA_LEGACY)
    sql =
      wxT
      ("SELECT f_geometry_column, type, coord_dimension, srid, spatial_index_enabled ");
  else if (metadata_type == METADATA_CURRENT)
    sql =
      wxT
      ("SELECT f_geometry_column, geometry_type, srid, spatial_index_enabled ");
  else
    return false;
  sql += wxT("FROM geometry_columns WHERE Lower(f_table_name) = Lower('");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->CleanSqlString(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("')");
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return false;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          if (metadata_type == METADATA_LEGACY)
            {
              /* legacy Spatial MetaData layout */
              value = results[(i * columns) + 0];
              geomColumn = wxString::FromUTF8(value);
              value = results[(i * columns) + 1];
              geomType = wxString::FromUTF8(value);
              value = results[(i * columns) + 2];
              coordDims = wxString::FromUTF8(value);
              value = results[(i * columns) + 3];
              geomSrid = atoi(value);
              value = results[(i * columns) + 4];
              indexType = atoi(value);
          } else
            {
              /* current Spatial MetaData layout */
              value = results[(i * columns) + 0];
              geomColumn = wxString::FromUTF8(value);
              switch (atoi(results[(i * columns) + 1]))
                {
                  case 0:
                    geomType = wxT("GEOMETRY");
                    coordDims = wxT("XY");
                    break;
                  case 1000:
                    geomType = wxT("GEOMETRY");
                    coordDims = wxT("XYZ");
                    break;
                  case 2000:
                    geomType = wxT("GEOMETRY");
                    coordDims = wxT("XYM");
                    break;
                  case 3000:
                    geomType = wxT("GEOMETRY");
                    coordDims = wxT("XYZM");
                    break;
                  case 1:
                    geomType = wxT("POINT");
                    coordDims = wxT("XY");
                    break;
                  case 1001:
                    geomType = wxT("POINT");
                    coordDims = wxT("XYZ");
                    break;
                  case 2001:
                    geomType = wxT("POINT");
                    coordDims = wxT("XYM");
                    break;
                  case 3001:
                    geomType = wxT("POINT");
                    coordDims = wxT("XYZM");
                    break;
                  case 2:
                    geomType = wxT("LINESTRING");
                    coordDims = wxT("XY");
                    break;
                  case 1002:
                    geomType = wxT("LINESTRING");
                    coordDims = wxT("XYZ");
                    break;
                  case 2002:
                    geomType = wxT("LINESTRING");
                    coordDims = wxT("XYM");
                    break;
                  case 3002:
                    geomType = wxT("LINESTRING");
                    coordDims = wxT("XYZM");
                    break;
                  case 3:
                    geomType = wxT("POLYGON");
                    coordDims = wxT("XY");
                    break;
                  case 1003:
                    geomType = wxT("POLYGON");
                    coordDims = wxT("XYZ");
                    break;
                  case 2003:
                    geomType = wxT("POLYGON");
                    coordDims = wxT("XYM");
                    break;
                  case 3003:
                    geomType = wxT("POLYGON");
                    coordDims = wxT("XYZM");
                    break;
                  case 4:
                    geomType = wxT("MULTIPOINT");
                    coordDims = wxT("XY");
                    break;
                  case 1004:
                    geomType = wxT("MULTIPOINT");
                    coordDims = wxT("XYZ");
                    break;
                  case 2004:
                    geomType = wxT("MULTIPOINT");
                    coordDims = wxT("XYM");
                    break;
                  case 3004:
                    geomType = wxT("MULTIPOINT");
                    coordDims = wxT("XYZM");
                    break;
                  case 5:
                    geomType = wxT("MULTILINESTRING");
                    coordDims = wxT("XY");
                    break;
                  case 1005:
                    geomType = wxT("MULTILINESTRING");
                    coordDims = wxT("XYZ");
                    break;
                  case 2005:
                    geomType = wxT("MULTILINESTRING");
                    coordDims = wxT("XYM");
                    break;
                  case 3005:
                    geomType = wxT("MULTILINESTRING");
                    coordDims = wxT("XYZM");
                    break;
                  case 6:
                    geomType = wxT("MULTIPOLYGON");
                    coordDims = wxT("XY");
                    break;
                  case 1006:
                    geomType = wxT("MULTIPOLYGON");
                    coordDims = wxT("XYZ");
                    break;
                  case 2006:
                    geomType = wxT("MULTIPOLYGON");
                    coordDims = wxT("XYM");
                    break;
                  case 3006:
                    geomType = wxT("MULTIPOLYGON");
                    coordDims = wxT("XYZM");
                    break;
                  case 7:
                    geomType = wxT("GEOMETRYCOLLECTION");
                    coordDims = wxT("XY");
                    break;
                  case 1007:
                    geomType = wxT("GEOMETRYCOLLECTION");
                    coordDims = wxT("XYZ");
                    break;
                  case 2007:
                    geomType = wxT("GEOMETRYCOLLECTION");
                    coordDims = wxT("XYM");
                    break;
                  case 3007:
                    geomType = wxT("GEOMETRYCOLLECTION");
                    coordDims = wxT("XYZM");
                    break;
                };
              value = results[(i * columns) + 2];
              geomSrid = atoi(value);
              value = results[(i * columns) + 3];
              indexType = atoi(value);
            }
          Geometries->Add(geomColumn, geomType, coordDims, geomSrid, indexType);
        }
    }
  sqlite3_free_table(results);
  return true;
}

void MyTableTree::DropRenameAux2(MyObject * obj, GeomColsList * Geometries,
                                 wxString & aliasTable, wxString & renameSql,
                                 wxString & dropSql,
                                 wxString & disableSpatialIdxSql,
                                 wxString & dropSpatialIdxSql,
                                 wxString & createSpatialIdxSql,
                                 wxString & discardGeometrySql)
{
//
// common tasks: drop/rename column auxiliaries
//
  GeomColumn *pG;
  wxString name;
  char xname[1024];
  char column[1024];

// creating the SQL fragments
  aliasTable = wxT("tmp_alias ");
  aliasTable += obj->GetName();
  aliasTable += wxT(" tmp_alias");
  strcpy(xname, aliasTable.ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  aliasTable = wxString::FromUTF8(xname);
  renameSql = wxT("ALTER TABLE ");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  renameSql += wxString::FromUTF8(xname);
  renameSql += wxT(" RENAME TO ");
  renameSql += aliasTable;
  renameSql += wxT(";\n");

  dropSql = wxT("DROP TABLE ");
  dropSql += aliasTable;
  dropSql += wxT(";\n");
  strcpy(column, obj->GetColumn().ToUTF8());
  pG = Geometries->GetFirst();
  while (pG)
    {
      if (pG->IsRTree() == true || pG->IsMbrCache() == true)
        {
          // disabling a Spatial Index
          disableSpatialIdxSql += wxT("SELECT DisableSpatialIndex('");
          strcpy(xname, obj->GetName().ToUTF8());
          MainFrame->CleanSqlString(xname);
          disableSpatialIdxSql += wxString::FromUTF8(xname);
          disableSpatialIdxSql += wxT("', '");
          strcpy(xname, pG->GetGeometryName().ToUTF8());
          MainFrame->CleanSqlString(xname);
          disableSpatialIdxSql += wxString::FromUTF8(xname);
          disableSpatialIdxSql += wxT("');\n");
          dropSpatialIdxSql += wxT("DROP TABLE IF EXISTS ");
          name = wxT("idx_");
          name += obj->GetName();
          name += wxT("_");
          name += pG->GetGeometryName();
          strcpy(xname, name.ToUTF8());
          MainFrame->DoubleQuotedSql(xname);
          dropSpatialIdxSql += wxString::FromUTF8(xname);
          dropSpatialIdxSql += wxT(";\n");
          if (pG->IsRTree() == true)
            {
              // creating an RTree Spatial Index
              createSpatialIdxSql += wxT("SELECT CreateSpatialIndex('");
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->CleanSqlString(xname);
              createSpatialIdxSql += wxString::FromUTF8(xname);
              createSpatialIdxSql += wxT("', '");
              strcpy(xname, pG->GetGeometryName().ToUTF8());
              MainFrame->CleanSqlString(xname);
              createSpatialIdxSql += wxString::FromUTF8(xname);
              createSpatialIdxSql += wxT("');\n");
          } else
            {
              // creating an MbrCache Spatial Index
              createSpatialIdxSql += wxT("SELECT CreateMbrCache('");
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->CleanSqlString(xname);
              createSpatialIdxSql += wxString::FromUTF8(xname);
              createSpatialIdxSql += wxT("', '");
              strcpy(xname, pG->GetGeometryName().ToUTF8());
              MainFrame->CleanSqlString(xname);
              createSpatialIdxSql += wxString::FromUTF8(xname);
              createSpatialIdxSql += wxT("');\n");
            }
        }
      // discarding a Geometry Column
      discardGeometrySql += wxT("SELECT DiscardGeometryColumn('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      discardGeometrySql += wxString::FromUTF8(xname);
      discardGeometrySql += wxT("', '");
      strcpy(xname, pG->GetGeometryName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      discardGeometrySql += wxString::FromUTF8(xname);
      discardGeometrySql += wxT("');\n");
      pG = pG->GetNext();
    }
}

void MyTableTree::DropRenameAux3(MyObject * obj, GeomColsList * Geometries,
                                 TblIndexList * Index,
                                 wxString & addGeometrySql)
{
//
// common tasks: drop/rename column auxiliaries
//
  GeomColumn *pG;
  TblIndex *pI;
  char xname[1024];
  char dummy[64];
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  char *value;
  wxString indexName;
  bool uniqueIndex;

  pG = Geometries->GetFirst();
  while (pG)
    {
      // adding a Geometry Column
      addGeometrySql += wxT("SELECT AddGeometryColumn('");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      addGeometrySql += wxString::FromUTF8(xname);
      addGeometrySql += wxT("', '");
      strcpy(xname, pG->GetGeometryName().ToUTF8());
      MainFrame->CleanSqlString(xname);
      addGeometrySql += wxString::FromUTF8(xname);
      sprintf(dummy, "', %d", pG->GetSrid());
      addGeometrySql += wxString::FromUTF8(dummy);
      addGeometrySql += wxT(", '");
      addGeometrySql += pG->GetGeometryType();
      if (pG->GetCoordDims() == wxT('2') || pG->GetCoordDims() == wxT('3'))
        {
          addGeometrySql += wxT("', ");
          addGeometrySql += pG->GetCoordDims();
      } else
        {
          addGeometrySql += wxT("', '");
          addGeometrySql += pG->GetCoordDims();
          addGeometrySql += wxT("'");
        }
      if (pG->IsNotNull() == false)
        addGeometrySql += wxT(");\n");
      else
        addGeometrySql += wxT(", 1);\n");
      pG = pG->GetNext();
    }

// retrieving any related Index
  sql = wxT("PRAGMA index_list(");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 1];
          if (strncmp(value, "sqlite_autoindex_", 17) == 0)
            {
              // sandro 2011-01-03: discarding any Primary Key Index
              continue;
            }
          indexName = wxString::FromUTF8(value);
          value = results[(i * columns) + 2];
          if (atoi(value) == 0)
            uniqueIndex = false;
          else
            uniqueIndex = true;
          Index->Add(indexName, uniqueIndex);
        }
    }
  sqlite3_free_table(results);
  pI = Index->GetFirst();
  while (pI)
    {
      // retrieving any Index Column
      sql = wxT("PRAGMA index_info(");
      strcpy(xname, pI->GetIndexName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")");
      ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              value = results[(i * columns) + 2];
              indexName = wxString::FromUTF8(value);
              pI->Add(indexName);
            }
        }
      sqlite3_free_table(results);
      pI = pI->GetNext();
    }

}

void MyTableTree::OnCmdDropColumn(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - drop column
//
  char column[1024];
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  wxString createSql;
  wxString insertSql;
  wxString insertFromSql;
  wxString renameSql;
  wxString dropSql;
  wxString dropIndexSql;
  wxString createIndexSql;
  wxString disableSpatialIdxSql;
  wxString dropSpatialIdxSql;
  wxString createSpatialIdxSql;
  wxString discardGeometrySql;
  wxString addGeometrySql;
  wxString geomColumn;
  bool comma = false;
  char *value;
  bool autoincrement = false;
  wxString aliasTable;
  bool isGeom;
  wxString msg;
  GeomColsList Geometries;
  GeomColumn *pG;
  TblIndexList Index;
  TblIndex *pI;
  IndexColumn *pC;
  wxString name;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;

  strcpy(column, obj->GetColumn().ToUTF8());
  if (DropRenameAux1(obj, &Geometries, &autoincrement) == false)
    return;
  DropRenameAux2(obj, &Geometries, aliasTable, renameSql, dropSql,
                 disableSpatialIdxSql, dropSpatialIdxSql, createSpatialIdxSql,
                 discardGeometrySql);

// retrieving the Column names
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      createSql = wxT("CREATE TABLE ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      createSql += wxString::FromUTF8(xname);
      createSql += wxT(" (\n");
      insertSql = wxT("INSERT INTO ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      insertSql += wxString::FromUTF8(xname);
      insertSql += wxT(" (");
      insertFromSql = wxT("SELECT ");
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 1];
          if (strcasecmp(value, column) == 0)
            continue;
          isGeom = false;
          pG = Geometries.GetFirst();
          while (pG)
            {
              char geom[1024];
              strcpy(geom, pG->GetGeometryName().ToUTF8());
              if (strcasecmp(geom, value) == 0)
                {
                  isGeom = true;
                  geomColumn = pG->GetGeometryName();
                  break;
                }
              pG = pG->GetNext();
            }
          if (comma == true)
            {
              if (isGeom == false)
                createSql += wxT(",\n");
              insertSql += wxT(", ");
              insertFromSql += wxT(", ");
            }
          if (isGeom == false)
            {
              strcpy(xname, value);
              MainFrame->DoubleQuotedSql(xname);
              createSql += wxString::FromUTF8(xname);
              createSql += wxT(" ");
            }
          strcpy(xname, value);
          MainFrame->DoubleQuotedSql(xname);
          insertSql += wxString::FromUTF8(xname);
          strcpy(xname, value);
          MainFrame->DoubleQuotedSql(xname);
          insertFromSql += wxString::FromUTF8(xname);
          value = results[(i * columns) + 2];
          if (isGeom == false)
            createSql += wxString::FromUTF8(value);
          value = results[(i * columns) + 5];
          if (value)
            {
              if (atoi(value) != 0)
                {
                  if (isGeom == false)
                    {
                      createSql += wxT(" PRIMARY KEY");
                      if (autoincrement == true)
                        createSql += wxT(" AUTOINCREMENT");
                    }
                }
            }
          value = results[(i * columns) + 3];
          if (value)
            {
              if (atoi(value) != 0)
                {
                  if (isGeom == true)
                    Geometries.SetNotNull(geomColumn);
                  else
                    createSql += wxT(" NOT NULL");
                }
            }
          value = results[(i * columns) + 4];
          if (value && isGeom == false)
            {
              createSql += wxT(" DEFAULT ");
              createSql += wxString::FromUTF8(value);
            }
          comma = true;
        }
      createSql += wxT(");\n");
      insertSql += wxT(")\n");
      insertFromSql += wxT("\nFROM ");
      insertFromSql += aliasTable;
      insertFromSql += wxT(";\n");
      insertSql += insertFromSql;
    }
  sqlite3_free_table(results);

  DropRenameAux3(obj, &Geometries, &Index, addGeometrySql);

// setting up the Index SQL fragments
  Index.Invalidate(obj->GetColumn());
  pI = Index.GetFirst();
  while (pI)
    {
      dropIndexSql += wxT("DROP INDEX ");
      strcpy(xname, pI->GetIndexName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      dropIndexSql += wxString::FromUTF8(xname);
      dropIndexSql += wxT(";\n");
      if (pI->IsValid() == true)
        {
          if (pI->GetFirst())
            {
              if (pI->IsUnique() == true)
                createIndexSql += wxT("CREATE UNIQUE INDEX ");
              else
                createIndexSql += wxT("CREATE INDEX ");
              strcpy(xname, pI->GetIndexName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              createIndexSql += wxString::FromUTF8(xname);
              createIndexSql += wxT(" ON ");
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              createIndexSql += wxString::FromUTF8(xname);
              createIndexSql += wxT(" (");
            }
          comma = false;
          pC = pI->GetFirst();
          while (pC)
            {
              if (comma == true)
                createIndexSql += wxT(", ");
              strcpy(xname, pC->GetColumnName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              createIndexSql += wxString::FromUTF8(xname);
              comma = true;
              pC = pC->GetNext();
            }
          if (pI->GetFirst())
            createIndexSql += wxT(");\n");
        }
      pI = pI->GetNext();
    }

// setting up the SQL complex statement
  sql = wxT("BEGIN;\n");
  sql += disableSpatialIdxSql;
  sql += discardGeometrySql;
  sql += dropSpatialIdxSql;
  sql += dropIndexSql;
  sql += renameSql;
  sql += createSql;
  sql += addGeometrySql;
  sql += createSpatialIdxSql;
  sql += createIndexSql;
  sql += insertSql;
  sql += dropSql;
  sql += wxT("COMMIT;");
  if (sql.Len() < 1)
    return;
  msg = wxT("Do you really intend to drop the Column ");
  msg += obj->GetColumn();
  msg += wxT("\nfrom the Table ");
  msg += obj->GetName();
  msg += wxT(" ?");
  wxMessageDialog confirm(this, msg, wxT("Confirming DROP COLUMN"),
                          wxOK | wxCANCEL | wxICON_QUESTION);
  ret = confirm.ShowModal();
  if (ret != wxID_OK)
    return;

// executing
  ::wxBeginBusyCursor();
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      goto rollback;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT("The column ") + obj->GetColumn() +
               wxT("\nwas successfully removed\nfrom the Table ") +
               obj->GetName(), wxT("spatialite_gui"),
               wxOK | wxICON_INFORMATION, this);
  MainFrame->InitTableTree();
  return;
rollback:
  ret = sqlite3_exec(MainFrame->GetSqlite(), "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("An error occurred\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdRenameColumn(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - rename column
//
  char column[1024];
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  wxString createSql;
  wxString insertSql;
  wxString insertFromSql;
  wxString renameSql;
  wxString dropSql;
  wxString dropIndexSql;
  wxString createIndexSql;
  wxString disableSpatialIdxSql;
  wxString dropSpatialIdxSql;
  wxString createSpatialIdxSql;
  wxString discardGeometrySql;
  wxString addGeometrySql;
  wxString geomColumn;
  bool comma = false;
  char *value;
  bool autoincrement = false;
  wxString aliasTable;
  bool isGeom;
  wxString msg;
  GeomColsList Geometries;
  GeomColumn *pG;
  TblIndexList Index;
  TblIndex *pI;
  IndexColumn *pC;
  wxString name;
  char xname[1024];
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;

// asking the new column name
  wxString newColumn =
    wxGetTextFromUser(wxT("Please, insert the new Column Name"),
                      wxT("Rename Column"), obj->GetColumn(),
                      MainFrame, wxDefaultCoord, wxDefaultCoord, false);

  strcpy(column, obj->GetColumn().ToUTF8());
  if (DropRenameAux1(obj, &Geometries, &autoincrement) == false)
    return;
  DropRenameAux2(obj, &Geometries, aliasTable, renameSql, dropSql,
                 disableSpatialIdxSql, dropSpatialIdxSql, createSpatialIdxSql,
                 discardGeometrySql);

// retrieving the Column names
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                          &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      createSql = wxT("CREATE TABLE ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      createSql += wxString::FromUTF8(xname);
      createSql += wxT(" (\n");
      insertSql = wxT("INSERT INTO ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      insertSql += wxString::FromUTF8(xname);
      insertSql += wxT(" (");
      insertFromSql = wxT("SELECT ");
      for (i = 1; i <= rows; i++)
        {
          value = results[(i * columns) + 1];
          isGeom = false;
          pG = Geometries.GetFirst();
          while (pG)
            {
              char geom[1024];
              strcpy(geom, pG->GetGeometryName().ToUTF8());
              if (strcasecmp(geom, value) == 0)
                {
                  isGeom = true;
                  geomColumn = pG->GetGeometryName();
                  break;
                }
              pG = pG->GetNext();
            }
          if (comma == true)
            {
              if (isGeom == false)
                createSql += wxT(",\n");
              insertSql += wxT(", ");
              insertFromSql += wxT(", ");
            }
          if (isGeom == false)
            {
              if (strcasecmp(value, column) == 0)
                {
                  strcpy(xname, newColumn.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  createSql += wxString::FromUTF8(xname);
              } else
                {
                  strcpy(xname, value);
                  MainFrame->DoubleQuotedSql(xname);
                  createSql += wxString::FromUTF8(xname);
                }
              createSql += wxT(" ");
            }
          if (strcasecmp(value, column) == 0)
            {
              strcpy(xname, newColumn.ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              insertSql += wxString::FromUTF8(xname);
          } else
            {
              strcpy(xname, value);
              MainFrame->DoubleQuotedSql(xname);
              insertSql += wxString::FromUTF8(xname);
            }
          strcpy(xname, value);
          MainFrame->DoubleQuotedSql(xname);
          insertFromSql += wxString::FromUTF8(xname);
          value = results[(i * columns) + 2];
          if (isGeom == false)
            createSql += wxString::FromUTF8(value);
          value = results[(i * columns) + 5];
          if (value)
            {
              if (atoi(value) != 0)
                {
                  if (isGeom == false)
                    {
                      createSql += wxT(" PRIMARY KEY");
                      if (autoincrement == true)
                        createSql += wxT(" AUTOINCREMENT");
                    }
                }
            }
          value = results[(i * columns) + 3];
          if (value)
            {
              if (atoi(value) != 0)
                {
                  if (isGeom == true)
                    Geometries.SetNotNull(geomColumn);
                  else
                    createSql += wxT(" NOT NULL");
                }
            }
          value = results[(i * columns) + 4];
          if (value && isGeom == false)
            {
              createSql += wxT(" DEFAULT ");
              createSql += wxString::FromUTF8(value);
            }
          comma = true;
        }
      createSql += wxT(");\n");
      insertSql += wxT(")\n");
      insertFromSql += wxT("\nFROM ");
      insertFromSql += aliasTable;
      insertFromSql += wxT(";\n");
      insertSql += insertFromSql;
    }
  sqlite3_free_table(results);

  DropRenameAux3(obj, &Geometries, &Index, addGeometrySql);

// setting up the Index SQL fragments
  pI = Index.GetFirst();
  while (pI)
    {
      dropIndexSql += wxT("DROP INDEX ");
      strcpy(xname, pI->GetIndexName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      dropIndexSql += wxString::FromUTF8(xname);
      dropIndexSql += wxT(";\n");
      if (pI->IsValid() == true)
        {
          if (pI->GetFirst())
            {
              if (pI->IsUnique() == true)
                createIndexSql += wxT("CREATE UNIQUE INDEX ");
              else
                createIndexSql += wxT("CREATE INDEX ");
              strcpy(xname, pI->GetIndexName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              createIndexSql += wxString::FromUTF8(xname);
              createIndexSql += wxT(" ON ");
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              createIndexSql += wxString::FromUTF8(xname);
              createIndexSql += wxT(" (");
            }
          comma = false;
          pC = pI->GetFirst();
          while (pC)
            {
              char xvalue[1024];
              strcpy(xvalue, pC->GetColumnName().ToUTF8());
              if (comma == true)
                createIndexSql += wxT(", ");
              if (strcasecmp(xvalue, column) == 0)
                {
                  strcpy(xname, newColumn.ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  createIndexSql += wxString::FromUTF8(xname);
              } else
                {
                  strcpy(xname, pC->GetColumnName().ToUTF8());
                  MainFrame->DoubleQuotedSql(xname);
                  createIndexSql += wxString::FromUTF8(xname);
                }
              comma = true;
              pC = pC->GetNext();
            }
          if (pI->GetFirst())
            createIndexSql += wxT(");\n");
        }
      pI = pI->GetNext();
    }

// setting up the SQL complex statement
  sql = wxT("BEGIN;\n");
  sql += disableSpatialIdxSql;
  sql += discardGeometrySql;
  sql += dropSpatialIdxSql;
  sql += dropIndexSql;
  sql += renameSql;
  sql += createSql;
  sql += addGeometrySql;
  sql += createSpatialIdxSql;
  sql += createIndexSql;
  sql += insertSql;
  sql += dropSql;
  sql += wxT("COMMIT;");
  if (sql.Len() < 1)
    return;
  char cazzo[8192];
  strcpy(cazzo, sql.ToUTF8());
  msg = wxT("Do you really intend to rename the Column ");
  msg += obj->GetColumn();
  msg += wxT(" as ");
  msg += newColumn;
  msg += wxT("\ninto the Table ");
  msg += obj->GetName();
  msg += wxT(" ?");
  wxMessageDialog confirm(this, msg, wxT("Confirming RENAME COLUMN"),
                          wxOK | wxCANCEL | wxICON_QUESTION);
  ret = confirm.ShowModal();
  if (ret != wxID_OK)
    return;

// executing
  ::wxBeginBusyCursor();
  ret = sqlite3_exec(MainFrame->GetSqlite(), sql.ToUTF8(), NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      goto rollback;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT("The column ") + obj->GetColumn() +
               wxT("\nwas successfully renamed as ") + newColumn +
               wxT("\ninto the Table ") + obj->GetName(),
               wxT("spatialite_gui"), wxOK | wxICON_INFORMATION, this);
  MainFrame->InitTableTree();
  return;
rollback:
  ret = sqlite3_exec(MainFrame->GetSqlite(), "ROLLBACK", NULL, NULL, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      ::wxEndBusyCursor();
      return;
    }
  ::wxEndBusyCursor();
  wxMessageBox(wxT
               ("An error occurred\n\na ROLLBACK was automatically performed"),
               wxT("spatialite_gui"), wxOK | wxICON_WARNING, this);
}

void MyTableTree::OnCmdEdit(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - editing row values
//
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  wxString sql;
  char *column;
  char *type;
  int pk = 0;
  int pb = 0;
  int primaryKeys[1024];
  int blobCols[1024];
  char xname[1024];
  for (i = 0; i < 1024; i++)
    {
      primaryKeys[i] = -1;
      blobCols[i] = -1;
    }
  primaryKeys[pk++] = 0;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  sql = wxT("PRAGMA table_info(");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT(")");
  int ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
  if (ret != SQLITE_OK)
    {
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(errMsg);
      return;
    }
  if (rows < 1)
    ;
  else
    {
      sql = wxT("SELECT ROWID");
      for (i = 1; i <= rows; i++)
        {
          column = results[(i * columns) + 1];
          sql += wxT(", ");
          strcpy(xname, column);
          MainFrame->DoubleQuotedSql(xname);
          sql += wxString::FromUTF8(xname);
          type = results[(i * columns) + 2];
          if (strcasecmp(type, "BLOB") == 0)
            blobCols[pb++] = i;
          if (atoi(results[(i * columns) + 5]) == 0)
            ;
          else
            primaryKeys[pk++] = i;
        }
    }
  sqlite3_free_table(results);
  if (sql.Len() < 1)
    return;
  sql += wxT("\nFROM ");
  strcpy(xname, obj->GetName().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += wxT("\nORDER BY ROWID");
  MainFrame->EditTable(sql, primaryKeys, blobCols, obj->GetName());
}

void MyTableTree::OnCmdCheckDuplicates(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Checking for Duplicate rows
//
  wxString sql;
  wxString col_list;
  bool first = true;
  char xname[1024];
  int pk;
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      // extracting the column names (excluding any Primary Key)
      sql = wxT("PRAGMA table_info(");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")");
      ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              strcpy(xname, results[(i * columns) + 1]);
              pk = atoi(results[(i * columns) + 5]);
              if (!pk)
                {
                  if (first)
                    first = false;
                  else
                    col_list += wxT(", ");
                  MainFrame->DoubleQuotedSql(xname);
                  col_list += wxString::FromUTF8(xname);
                }
            }
        }
      sqlite3_free_table(results);
      // preparing the SQL statement
      sql = wxT("SELECT Count(*) AS \"[dupl-count]\", ");
      sql += col_list;
      sql += wxT("\nFROM ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nGROUP BY ");
      sql += col_list;
      sql += wxT("\nHAVING \"[dupl-count]\" > 1");
      sql += wxT("\nORDER BY \"[dupl-count]\" DESC");
      MainFrame->SetSql(sql, true);
    }
}

void MyTableTree::OnCmdRemoveDuplicates(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Removing Duplicate rows
//
  DuplRow value_list;
  wxString sql;
  wxString sql2;
  wxString col_list;
  wxString params_list;
  bool first = true;
  char xname[1024];
  int count;
  int pk;
  int ret;
  char **results;
  int rows;
  int columns;
  int i;
  char *errMsg = NULL;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_TABLE)
    {
      // extracting the column names (excluding any Primary Key)
      value_list.SetTable(obj->GetName());
      sql = wxT("PRAGMA table_info(");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT(")");
      ret = sqlite3_get_table(MainFrame->GetSqlite(), sql.ToUTF8(), &results,
                              &rows, &columns, &errMsg);
      if (ret != SQLITE_OK)
        {
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(errMsg),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          sqlite3_free(errMsg);
          return;
        }
      if (rows < 1)
        ;
      else
        {
          for (i = 1; i <= rows; i++)
            {
              strcpy(xname, results[(i * columns) + 1]);
              pk = atoi(results[(i * columns) + 5]);
              if (!pk)
                {
                  if (first)
                    first = false;
                  else
                    col_list += wxT(", ");
                  MainFrame->DoubleQuotedSql(xname);
                  wxString col_name = wxString::FromUTF8(xname);
                  col_list += col_name;
                  value_list.Add(col_name);
                }
            }
        }
      sqlite3_free_table(results);
      // preparing the SQL statement (identifying duplicated rows)
      sql = wxT("SELECT Count(*) AS \"[dupl-count]\", ");
      sql += col_list;
      sql += wxT("\nFROM ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql += wxString::FromUTF8(xname);
      sql += wxT("\nGROUP BY ");
      sql += col_list;
      sql += wxT("\nHAVING \"[dupl-count]\" > 1");
      // preparing the SQL statement [delete]
      sql2 = wxT("DELETE FROM ");
      strcpy(xname, obj->GetName().ToUTF8());
      MainFrame->DoubleQuotedSql(xname);
      sql2 += wxString::FromUTF8(xname);
      sql2 += wxT(" WHERE ROWID = ?");

      if (doDeleteDuplicates(sql, sql2, &value_list, &count) == true)
        {
          if (!count)
            {
              strcpy(xname, "No duplicated rows have been identified on ");
              sql = wxString::FromUTF8(xname);
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              sql += wxString::FromUTF8(xname);
              wxMessageBox(sql, wxT("spatialite_gui"),
                           wxOK | wxICON_INFORMATION, this);
          } else
            {
              sprintf(xname, "%d duplicated rows deleted from ", count);
              sql = wxString::FromUTF8(xname);
              strcpy(xname, obj->GetName().ToUTF8());
              MainFrame->DoubleQuotedSql(xname);
              sql += wxString::FromUTF8(xname);
              wxMessageBox(sql, wxT("spatialite_gui"),
                           wxOK | wxICON_INFORMATION, this);
            }
        }
    }
}

void MyTableTree::OnCmdCheckGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Checking a Geometry Column
//
  int ret;
  wxString table;
  wxString geom;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      table = obj->GetName();
      geom = obj->GetColumn();
  } else
    return;
  CheckGeometryDialog dlg;
  dlg.Create(MainFrame, table, geom);
  ret = dlg.ShowModal();
  if (ret != wxYES)
    return;

  char xtable[1024];
  char xgeometry[1024];
  char *err_msg = NULL;
  char report_path[1024];
  wxString msg;
  strcpy(xtable, table.ToUTF8());
  strcpy(xgeometry, geom.ToUTF8());

  wxFileDialog fileDialog(this, wxT("Diagnostic Report"),
                          wxT(""), wxT("report.html"),
                          wxT
                          ("HTML document (*.html)|*.html|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(report_path, fileDialog.GetPath().ToUTF8());
      ::wxBeginBusyCursor();
      int n_invalids;
      ret =
        check_geometry_column(MainFrame->GetSqlite(), xtable, xgeometry,
                              report_path, NULL, &n_invalids, &err_msg);
      ::wxEndBusyCursor();
      if (ret == 0)
        {
          // reporting some error condition
          msg = wxT("Some unexpected error occurred:\n\n");
          if (err_msg != NULL)
            {
              msg += wxString::FromUTF8(err_msg);
              free(err_msg);
          } else
            msg += wxT("Sorry, no further details are available");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      } else
        {
          int mode;
          if (n_invalids > 0)
            {
              msg =
                wxT
                ("ATTENTION: some invalid Geometries have been detected !!!\n\n");
              mode = wxICON_WARNING;
          } else
            {
              msg =
                wxT
                ("No invalid Geometries have been detected; this layer is full valid\n\n");
              mode = wxICON_INFORMATION;
            }
          msg += wxT("A full diagnostic report has been created.\n");
          msg +=
            wxT
            ("Please point your WEB Browser at the following HTML document containing the report:\n\n");
          msg += fileDialog.GetPath();
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | mode, this);
        }
    }
}

void MyTableTree::OnCmdSanitizeGeometries(wxCommandEvent & WXUNUSED(event))
{
//
// menu event - Attempting to sanitize a Geometry Column
//
  int ret;
  wxString table;
  wxString geom;
  MyObject *obj = (MyObject *) GetItemData(CurrentItem);
  if (obj == NULL)
    return;
  if (obj->GetType() == MY_GEOMETRY
      || obj->GetType() == MY_GEOMETRY_INDEX
      || obj->GetType() == MY_GEOMETRY_CACHED)
    {
      table = obj->GetName();
      geom = obj->GetColumn();
  } else
    return;
  SanitizeGeometryDialog dlg;
  dlg.Create(MainFrame, table, geom);
  ret = dlg.ShowModal();
  if (ret != wxYES)
    return;

  char tmp_prefix[1024];
  char xtable[1024];
  char xgeometry[1024];
  char *err_msg = NULL;
  char report_path[1024];
  wxString msg;
  strcpy(tmp_prefix, dlg.GetTmpPrefix().ToUTF8());
  strcpy(xtable, table.ToUTF8());
  strcpy(xgeometry, geom.ToUTF8());

  wxFileDialog fileDialog(this, wxT("Diagnostic Report"),
                          wxT(""), wxT("report.html"),
                          wxT
                          ("HTML document (*.html)|*.html|All files (*.*)|*.*"),
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition,
                          wxDefaultSize, wxT("filedlg"));
  ret = fileDialog.ShowModal();
  if (ret == wxID_OK)
    {
      strcpy(report_path, fileDialog.GetPath().ToUTF8());
      ::wxBeginBusyCursor();
      int n_failures;
      ret =
        sanitize_geometry_column(MainFrame->GetSqlite(), xtable, xgeometry,
                                 tmp_prefix, report_path, NULL, NULL, NULL,
                                 &n_failures, &err_msg);
      ::wxEndBusyCursor();
      if (ret == 0)
        {
          // reporting some error condition
          msg = wxT("Some unexpected error occurred:\n\n");
          if (err_msg != NULL)
            {
              msg += wxString::FromUTF8(err_msg);
              free(err_msg);
          } else
            msg += wxT("Sorry, no further details are available");
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      } else
        {
          int mode;
          if (n_failures > 0)
            {
              msg =
                wxT
                ("ATTENTION: some invalid Geometries still remain invalid !!!\n\n");
              mode = wxICON_WARNING;
          } else
            {
              msg =
                wxT
                ("All invalid Geometries have been saned; this layer is now full valid\n\n");
              mode = wxICON_INFORMATION;
            }
          msg += wxT("A full diagnostic report has been created.\n");
          msg +=
            wxT
            ("Please point your WEB Browser at the following HTML document containing the report:\n\n");
          msg += fileDialog.GetPath();
          wxMessageBox(msg, wxT("spatialite_gui"), wxOK | mode, this);
        }
    }
}

bool MyTableTree::doDeleteDuplicates(wxString & sql1, wxString & sql2,
                                     DuplRow * value_list, int *count)
{
// deleting duplicate rows
  char xsql[8192];
  sqlite3_stmt *stmt1 = NULL;
  sqlite3_stmt *stmt2 = NULL;
  int ret;
  int xcnt;
  int cnt = 0;
  int n_cols;
  int col_no;
  char *sql_err = NULL;

  *count = 0;
  ::wxBeginBusyCursor();

// the complete operation is handled as an unique SQL Transaction 
  ret = sqlite3_exec(MainFrame->GetSqlite(), "BEGIN", NULL, NULL, &sql_err);
  if (ret != SQLITE_OK)
    {
      sqlite3_free(sql_err);
      return false;
    }
// preparing the main SELECT statement
  strcpy(xsql, sql1.ToUTF8());
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), xsql, strlen(xsql), &stmt1,
                       NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(xsql, "SQL error: %s", sqlite3_errmsg(MainFrame->GetSqlite()));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(xsql),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      return false;
    }
// preparing the DELETE statement
  strcpy(xsql, sql2.ToUTF8());
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), xsql, strlen(xsql), &stmt2,
                       NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(xsql, "SQL error: %s", sqlite3_errmsg(MainFrame->GetSqlite()));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(xsql),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      goto error;
    }

  while (1)
    {
      //
      // fetching the result set rows 
      //
      ret = sqlite3_step(stmt1);
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          //
          // fetching a row
          //
          sqlite3_reset(stmt2);
          sqlite3_clear_bindings(stmt2);
          n_cols = sqlite3_column_count(stmt1);
          for (col_no = 1; col_no < n_cols; col_no++)
            {
              // saving column values
              if (sqlite3_column_type(stmt1, col_no) == SQLITE_INTEGER)
                value_list->SetValue(col_no - 1,
                                     sqlite3_column_int64(stmt1, col_no));
              if (sqlite3_column_type(stmt1, col_no) == SQLITE_FLOAT)
                value_list->SetValue(col_no - 1,
                                     sqlite3_column_double(stmt1, col_no));
              if (sqlite3_column_type(stmt1, col_no) == SQLITE_TEXT)
                {
                  const char *xtext =
                    (const char *) sqlite3_column_text(stmt1, col_no);
                  value_list->SetValue(col_no - 1, xtext);
                }
              if (sqlite3_column_type(stmt1, col_no) == SQLITE_BLOB)
                {
                  const void *blob = sqlite3_column_blob(stmt1, col_no);
                  int blob_size = sqlite3_column_bytes(stmt1, col_no);
                  value_list->SetValue(col_no - 1, blob, blob_size);
                }
              if (sqlite3_column_type(stmt1, col_no) == SQLITE_NULL)
                value_list->SetValue(col_no - 1);
            }
          if (doDeleteDuplicates2(stmt2, value_list, &xcnt) == true)
            cnt += xcnt;
          else
            goto error;
      } else
        {
          sprintf(xsql, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(xsql),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto error;
        }
    }

  sqlite3_finalize(stmt1);
  sqlite3_finalize(stmt2);

// confirm the still pending Transaction 
  ret = sqlite3_exec(MainFrame->GetSqlite(), "COMMIT", NULL, NULL, &sql_err);
  if (ret != SQLITE_OK)
    {
      sprintf(xsql, "COMMIT TRANSACTION error: %s\n", sql_err);
      wxMessageBox(wxString::FromUTF8(xsql),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(sql_err);
      return false;
    }

  ::wxEndBusyCursor();
  *count = cnt;
  return true;

error:
  *count = 0;
  if (stmt1)
    sqlite3_finalize(stmt1);
  if (stmt2)
    sqlite3_finalize(stmt2);

// performing a ROLLBACK anyway 
  ret = sqlite3_exec(MainFrame->GetSqlite(), "ROLLBACK", NULL, NULL, &sql_err);
  if (ret != SQLITE_OK)
    {
      sprintf(xsql, "ROLLBACK TRANSACTION error: %s\n", sql_err);
      wxMessageBox(wxString::FromUTF8(xsql),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      sqlite3_free(sql_err);
      return false;
    }

  ::wxEndBusyCursor();
  return false;
}

bool MyTableTree::doDeleteDuplicates2(sqlite3_stmt * stmt1,
                                      DuplRow * value_list, int *count)
{
// deleting duplicate rows [actual delete]
  int cnt = 0;
  int row_no = 0;
  char xsql[8192];
  char xname[1024];
  int ret;
  sqlite3_stmt *stmt2 = NULL;
  DuplColumn *col;
  wxString sql;
  wxString where;
  wxString condition;
  bool first = true;
  int qcnt = 0;
  int param = 1;
  bool match;
  int n_cols;
  int col_no;

  *count = 0;
  value_list->ResetQueryPos();

// preparing the query statement
  sql = wxT("SELECT ROWID");
  where = wxT("\nWHERE ");
  col = value_list->GetFirst();
  while (col)
    {
      if (col->GetType() == SQLITE_BLOB)
        {
          sql += wxT(", ");
          sql += col->GetName();
          col->SetQueryPos(qcnt++);
      } else if (col->GetType() == SQLITE_NULL)
        {
          if (first == true)
            {
              first = false;
              condition = col->GetName();
          } else
            {
              condition = wxT(" AND ");
              condition += col->GetName();
            }
          condition += wxT(" IS NULL");
          where += condition;
      } else
        {
          if (first == true)
            {
              first = false;
              condition = col->GetName();
          } else
            {
              condition = wxT(" AND ");
              condition += col->GetName();
            }
          condition += wxT(" = ?");
          where += condition;
          col->SetQueryPos(param++);
        }
      col = col->GetNext();
    }
  sql += wxT("\nFROM ");
  strcpy(xname, value_list->GetTable().ToUTF8());
  MainFrame->DoubleQuotedSql(xname);
  sql += wxString::FromUTF8(xname);
  sql += where;

  strcpy(xsql, sql.ToUTF8());
  ret =
    sqlite3_prepare_v2(MainFrame->GetSqlite(), xsql, strlen(xsql), &stmt2,
                       NULL);
  if (ret != SQLITE_OK)
    {
      sprintf(xsql, "SQL error: %s", sqlite3_errmsg(MainFrame->GetSqlite()));
      wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(xsql),
                   wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
      goto error;
    }

  sqlite3_reset(stmt2);
  sqlite3_clear_bindings(stmt2);
  col = value_list->GetFirst();
  while (col)
    {
      // binding query params
      if (col->GetType() == SQLITE_INTEGER)
        sqlite3_bind_int64(stmt2, col->GetQueryPos(), col->GetIntValue());
      if (col->GetType() == SQLITE_FLOAT)
        sqlite3_bind_double(stmt2, col->GetQueryPos(), col->GetDblValue());
      if (col->GetType() == SQLITE_TEXT)
        sqlite3_bind_text(stmt2, col->GetQueryPos(), col->GetTxtValue(),
                          strlen(col->GetTxtValue()), SQLITE_STATIC);
      col = col->GetNext();
    }

  while (1)
    {
      //
      // fetching the result set rows 
      //
      ret = sqlite3_step(stmt2);
      if (ret == SQLITE_DONE)
        break;                  // end of result set
      if (ret == SQLITE_ROW)
        {
          //
          // fetching a row
          //
          match = true;
          n_cols = sqlite3_column_count(stmt2);
          for (col_no = 1; col_no < n_cols; col_no++)
            {
              // checking blob columns
              if (sqlite3_column_type(stmt2, col_no) == SQLITE_BLOB)
                {
                  const void *blob = sqlite3_column_blob(stmt2, col_no);
                  int blob_size = sqlite3_column_bytes(stmt2, col_no);
                  if (value_list->CheckBlob(col_no - 1, blob, blob_size) ==
                      false)
                    match = false;
              } else
                match = false;
              if (match == false)
                break;
            }
          if (match == false)
            continue;
          row_no++;
          if (row_no > 1)
            {
              // deleting any duplicated row except the first one
              sqlite3_reset(stmt1);
              sqlite3_clear_bindings(stmt1);
              sqlite3_bind_int64(stmt1, 1, sqlite3_column_int64(stmt2, 0));
              ret = sqlite3_step(stmt1);
              if (ret == SQLITE_DONE || ret == SQLITE_ROW)
                cnt++;
              else
                {
                  sprintf(xsql, "SQL error: %s",
                          sqlite3_errmsg(MainFrame->GetSqlite()));
                  wxMessageBox(wxT("SQLite SQL error: ") +
                               wxString::FromUTF8(xsql), wxT("spatialite_gui"),
                               wxOK | wxICON_ERROR, this);
                  goto error;
                }
            }
      } else
        {
          sprintf(xsql, "SQL error: %s",
                  sqlite3_errmsg(MainFrame->GetSqlite()));
          wxMessageBox(wxT("SQLite SQL error: ") + wxString::FromUTF8(xsql),
                       wxT("spatialite_gui"), wxOK | wxICON_ERROR, this);
          goto error;
        }
    }
  if (stmt2)
    sqlite3_finalize(stmt2);
  *count = cnt;
  return true;

error:
  if (stmt2)
    sqlite3_finalize(stmt2);
  *count = 0;

  return false;
}

DuplRow::~DuplRow()
{
// destructor
  DuplColumn *p;
  DuplColumn *pn;
  p = First;
  while (p)
    {
      pn = p->GetNext();
      delete p;
      p = pn;
    }
}

void DuplRow::Add(wxString & name)
{
// adding a column
  DuplColumn *p = new DuplColumn(Count, name);
  Count++;
  if (First == NULL)
    First = p;
  if (Last)
    Last->SetNext(p);
  Last = p;
}

void DuplRow::SetValue(int pos, sqlite3_int64 value)
{
// setting up an integer value
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetPos() == pos)
        {
          p->SetValue(value);
          return;
        }
      p = p->GetNext();
    }
}

void DuplRow::SetValue(int pos, double value)
{
// setting up a double value
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetPos() == pos)
        {
          p->SetValue(value);
          return;
        }
      p = p->GetNext();
    }
}

void DuplRow::SetValue(int pos, const char *value)
{
// setting up a text value
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetPos() == pos)
        {
          p->SetValue(value);
          return;
        }
      p = p->GetNext();
    }
}

void DuplRow::SetValue(int pos, const void *blob, int size)
{
// setting up a blob value
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetPos() == pos)
        {
          p->SetValue(blob, size);
          return;
        }
      p = p->GetNext();
    }
}

void DuplRow::SetValue(int pos)
{
// setting up a NULL value
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetPos() == pos)
        {
          p->SetValue();
          return;
        }
      p = p->GetNext();
    }
}

void DuplRow::ResetQueryPos()
{
// resetting QueryPos for BLOBs
  DuplColumn *p = First;
  while (p)
    {
      p->SetQueryPos(-1);
      p = p->GetNext();
    }
}

bool DuplRow::CheckBlob(int pos, const void *blob, int size)
{
// checking a BLOB value 
  DuplColumn *p = First;
  while (p)
    {
      if (p->GetQueryPos() == pos)
        {
          return p->CheckBlob(blob, size);
        }
      p = p->GetNext();
    }
  return false;
}

bool DuplColumn::CheckBlob(const void *blob, int size)
{
// checking a BLOB value 
  if (Type != SQLITE_BLOB)
    return false;
  if (Size != size)
    return false;
  if (memcmp(Blob, blob, size) != 0)
    return false;
  return true;
}
