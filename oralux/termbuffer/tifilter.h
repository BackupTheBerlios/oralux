#ifndef TIFILTER_H
#define TIFILTER_H


/* terminfofilter
The input terminfo is filtered, the result is supplied in the output terminfo. 
The output is NULL in case of no change.
Otherwise, the caller must free the output.
*/

void terminfofilter(termapi* theTermAPI, chartype* theInputTerminfo, chartype* theOutputTerminfo); 

#endif
