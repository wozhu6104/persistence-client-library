/******************************************************************************
 * Project         Persistency
 * (c) copyright   2012
 * Company         XS Embedded GmbH
 *****************************************************************************/
/******************************************************************************
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a  copy of the MPL was not distributed
 * with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
******************************************************************************/
 /**
 * @file           persistence_client_library.c
 * @ingroup        Persistence client library
 * @author         Ingo Huerner
 * @brief          Implementation of the persistence client library.
 *                 Library provides an API to access persistent data
 * @see            
 */


#include "persistence_client_library.h"
#include "persistence_client_library_lc_interface.h"
#include "persistence_client_library_pas_interface.h"
#include "persistence_client_library_dbus_service.h"
#include "persistence_client_library_handle.h"
#include "persistence_client_library_data_access.h"
#include "persistence_client_library_custom_loader.h"
#include "persistence_client_library_access_helper.h"

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dbus/dbus.h>

#include <dlt/dlt.h>
#include <dlt/dlt_common.h>

extern char* __progname;

/// debug log and trace (DLT) setup
DLT_DECLARE_CONTEXT(persClientLibCtx);

const char* gResTableCfg = "/resource-table-cfg.itz";


/// shared cached default database
const char* gSharedCachedDefault = "/cached-default.itz";
/// shared cached database
const char* gSharedCached        = "/cached.itz";
/// shared write through default database
const char* gSharedWtDefault     = "/wt-default.itz";
/// shared write through database
const char* gSharedWt            = "/wt.itz";


/// local cached default database
const char* gLocalCachedDefault  = "cached-default.itz";
/// local cached default database
const char* gLocalCached         = "/cached.itz";
/// local write through default database
const char* gLocalWtDefault      = "wt-default.itz";
/// local write through default database
const char* gLocalWt             = "/wt.itz";



/// directory structure node name defintion
const char* gNode = "/node";
/// directory structure user name defintion
const char* gUser = "/user/";
/// directory structure seat name defintion
const char* gSeat = "/seat/";


/// path prefic for local cached database: /Data/mnt_c/<appId>/<database_name>
const char* gLocalCachePath        = "/Data/mnt-c/%s%s";
/// path prefic for local write through database /Data/mnt_wt/<appId>/<database_name>
const char* gLocalWtPath           = "/Data/mnt-wt/%s%s";
/// path prefic for shared cached database: /Data/mnt_c/Shared/Group/<group_no>/<database_name>
const char* gSharedCachePath       = "/Data/mnt-c/shared/group/%x%s";
/// path prefic for shared write through database: /Data/mnt_wt/Shared/Group/<group_no>/<database_name>
const char* gSharedWtPath          = "/Data/mnt-wt/shared/group/%x%s";
/// path prefic for shared public cached database: /Data/mnt_c/Shared/Public//<database_name>
const char* gSharedPublicCachePath = "/Data/mnt-c/shared/public%s";
/// path prefic for shared public write through database: /Data/mnt_wt/Shared/Public/<database_name>
const char* gSharedPublicWtPath    = "/Data/mnt-wt/shared/public%s";


/// application id
char gAppId[maxAppNameLen];

/// max key value data size [default 16kB]
int gMaxKeyValDataSize = defaultMaxKeyValDataSize;


/// library constructor
void pers_library_init(void) __attribute__((constructor));

/// library deconstructor
void pers_library_destroy(void) __attribute__((destructor));



void pers_library_init(void)
{
   int status = 0;
   int i = 0;

   DLT_REGISTER_APP("Persistence Client Library","persClientLib");
   DLT_REGISTER_CONTEXT(persClientLibCtx,"persClientLib","Context for Logging");

   DLT_LOG(persClientLibCtx, DLT_LOG_ERROR, DLT_STRING("Initialize Persistence Client Library!!!!"));

   /// environment variable for on demand loading of custom libraries
   const char *pOnDemandLoad = getenv("PERS_CUSTOM_LIB_LOAD_ON_DEMAND");

   /// environment variable for max key value data
   const char *pDataSize = getenv("PERS_MAX_KEY_VAL_DATA_SIZE");

   if(pDataSize != NULL)
   {
      gMaxKeyValDataSize = atoi(pDataSize);
   }

   setup_dbus_mainloop();


   // register for lifecycle and persistence admin service dbus messages
   register_lifecycle();
   register_pers_admin_service();

   // clear the open file descriptor array
   memset(gOpenFdArray, maxPersHandle, sizeof(int));

   /// get custom library names to load
   status = get_custom_libraries();
   if(status < 0)
   {
      printf("Failed to load custom library config table => error number %d\n", status );
   }

   // initialize custom library structure
   for(i = 0; i<PersCustomLib_LastEntry; i++)
   {
      gPersCustomFuncs[i].handle  = NULL;
      gPersCustomFuncs[i].custom_plugin_init = NULL;
      gPersCustomFuncs[i].custom_plugin_deinit = NULL;
      gPersCustomFuncs[i].custom_plugin_handle_open = NULL;
      gPersCustomFuncs[i].custom_plugin_handle_close = NULL;
      gPersCustomFuncs[i].custom_plugin_handle_get_data = NULL;
      gPersCustomFuncs[i].custom_plugin_handle_set_data  = NULL;
      gPersCustomFuncs[i].custom_plugin_get_data = NULL;
      gPersCustomFuncs[i].custom_plugin_set_data = NULL;
      gPersCustomFuncs[i].custom_plugin_delete_data = NULL;
      gPersCustomFuncs[i].custom_plugin_get_status_notification_clbk = NULL;
      gPersCustomFuncs[i].custom_plugin_handle_get_size = NULL;
      gPersCustomFuncs[i].custom_plugin_get_size = NULL;
      gPersCustomFuncs[i].custom_plugin_create_backup = NULL;
      gPersCustomFuncs[i].custom_plugin_get_backup = NULL;
      gPersCustomFuncs[i].custom_plugin_restore_backup = NULL;
   }

   if(pOnDemandLoad == NULL)  // load all available libraries now
   {
      for(i=0; i < get_num_custom_client_libs(); i++ )
      {
         if(load_custom_library(get_custom_client_position_in_array(i), &gPersCustomFuncs[i] ) == -1)
         {
            printf("E r r o r could not load plugin: %s \n", get_custom_client_lib_name(get_custom_client_position_in_array(i)));
            break;
         }
         gPersCustomFuncs[i].custom_plugin_init();
      }

      /// just testing
      //gPersCustomFuncs[PersCustomLib_early].custom_plugin_open("Hallo", 88, 99);
      //gPersCustomFuncs[PersCustomLib_early].custom_plugin_close(17);
   }

   //printf("A p p l i c a t i o n   n a m e => %s \n", __progname /*program_invocation_short_name*/);   // TODO: only temp solution for application name
   strncpy(gAppId, __progname, maxAppNameLen);
   // destory mutex
   pthread_mutex_destroy(&gDbusInitializedMtx);
   pthread_cond_destroy(&gDbusInitializedCond);
}



void pers_library_destroy(void)
{


   // unregister for lifecycle and persistence admin service dbus messages
   unregister_lifecycle();
   unregister_pers_admin_service();


   DLT_UNREGISTER_CONTEXT(persClientLibCtx);
   DLT_UNREGISTER_APP();
   dlt_free();
}





