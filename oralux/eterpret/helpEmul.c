#include "helpEmul.h"

// timeDiff comes from ParleMax

/* Wait for events and handle them. This function never returns, termination is done via exit, not return in main.
*/
struct timeval myOriginTime;

double timeDiff(struct timeval *t)
{
  //return 0;
    struct timeval ct;
    struct timezone zone;

    gettimeofday(&ct, &zone);
    
    return (ct.tv_sec - t->tv_sec + (ct.tv_usec - t->tv_usec) * 1e-6);
}


