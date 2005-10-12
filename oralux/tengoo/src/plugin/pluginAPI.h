#ifndef PLUGINAPI_H
#define PLUGINAPI_H

#include <glib.h>
#include "termapi.h"

/* function types for creating or deleting myPlugin */
/* < create_plugin */
/* 
   Input: theTermAPI, thexxxLength, and theDocument: pointer adress.
   Output: theDocument : pointer to the new document which must be deleted by the caller.
   Return value: a pointer to the plugin. 
*/
typedef void* (CREATE_PLUGIN)( termAPI* theTermAPI, int theInputOutputMaxLength, void** theDocument);
/* > */
typedef void (DELETE_PLUGIN)( void* thePlugin);

typedef struct pluginAPI pluginAPI;

/* managing input/output */
typedef GByteArray* (TRANSCODE_PLUGINAPI)( pluginAPI* thePluginAPI, char* theBuffer, int theLength);

struct pluginAPI 
{
  void* myPlugin;
  termAPI* myTermAPI;
  GByteArray* myBuffer;
  void* myDocument;
  TRANSCODE_PLUGINAPI* transcodeInput;
  TRANSCODE_PLUGINAPI* transcodeOutput;
  CREATE_PLUGIN* create;
  DELETE_PLUGIN* delete;
};

/* 
Local variables:
folded-file: t
folding-internal-margins: nil
*/

#endif
