#ifndef PLUGINAPI
#define PLUGINAPI

#include <glib.h>
#include "termapi.h"

/* function types for creating or deleting myPlugin */
typedef void* (CREATE_PLUGIN)( termAPI* theTermAPI, int theInputOutputMaxLength);
typedef void (DELETE_PLUGIN)( void* thePlugin);

typedef struct pluginAPI pluginAPI;

/* managing input/output */
typedef GByteArray* (TRANSCODE_PLUGINAPI)( pluginAPI* thePluginAPI, char* theBuffer, int theLength);

struct pluginAPI 
{
  void* myPlugin;
  termAPI* myTermAPI;
  TRANSCODE_PLUGINAPI* transcodeInput;
  TRANSCODE_PLUGINAPI* transcodeOutput;
  CREATE_PLUGIN* create;
  DELETE_PLUGIN* delete;
};


#endif
