/**
 * @file util.h
 * @author Ben Hermans
 * @brief Utility functions.
 * @details This file contains some utility functions, to copy the settings, 
 * to update the solver status, to print information and to time the algorithm.
 */
#ifndef UTIL_H
#define UTIL_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "types.h"
#include "constants.h"


/**********************
* Utility Functions  *
**********************/

/**
 * Copy settings creating a new settings structure.
 * @warning This function uses malloc.
 * @param  settings Settings to be copied
 * @return          New settings structure
 */
QPALMSettings* copy_settings(const QPALMSettings *settings);


/**
 * Custom string copy to avoid string.h library.
 * @param dest   Destination string
 * @param source Source string
 */
void c_strcpy(char       dest[],
              const char source[]);


/**
 * Update solver status (value and string).
 * @param info QPALMInfo
 * @param status_val New status value
 */
void update_status(QPALMInfo *info,
                   c_int     status_val);


/**********************
* Print Functions  *
**********************/

#ifdef QPALM_PRINTING

/** 
 * Print the header with QPALM version number and fields.
 */
void print_header(void);

/**
 * Print information about the current iteration. (Residuals, stepsize and objective value)
 * @param iter The current iteration number
 * @param work Workspace
 */
void print_iteration(c_int iter, QPALMWorkspace *work);

/**
 * Print final message as a box with info. (Final residuals, objective function, runtime)
 * @param work Workspace
 */
void print_final_message(QPALMWorkspace *work);

#endif // QPALM_PRINTING


/*********************************
* Timer Structs and Functions * *
*********************************/

# ifdef QPALM_TIMING

/*! \cond PRIVATE */

// Windows
#  ifdef _WIN32

  // Some R packages clash with elements
  // of the windows.h header, so use a
  // slimmer version for conflict avoidance
  # ifdef R_LANG
    #define NOGDI
  # endif

#   include <windows.h>

struct QPALM_TIMER {
  LARGE_INTEGER tic;
  LARGE_INTEGER toc;
  LARGE_INTEGER freq;
};

// Mac
#  elif defined __APPLE__

#   include <mach/mach_time.h>

/* Use MAC OSX  mach_time for timing */
struct QPALM_TIMER {
  uint64_t                  tic;
  uint64_t                  toc;
  mach_timebase_info_data_t tinfo;
};

// Mac
#  elif defined __MACH__

#   include <mach/mach_time.h>

/* Use MAC OSX  mach_time for timing */
struct QPALM_TIMER {
  uint64_t                  tic;
  uint64_t                  toc;
  mach_timebase_info_data_t tinfo;
};

// Linux
#  elif defined __linux__ // ifdef _WIN32

#   include <time.h>
#   include <sys/time.h>

struct QPALM_TIMER {
  struct timespec tic;
  struct timespec toc;
};

// FreeBSD
#  elif defined __FreeBSD__ // ifdef _WIN32

#   include <time.h>
#   include <sys/time.h>

struct QPALM_TIMER {
  struct timespec tic;
  struct timespec toc;
};

#  endif // ifdef _WIN32

/*! \endcond */


/**
 * Start timer.
 * @param t Timer object
 */
void qpalm_tic(QPALMTimer *t);

/**
 * Report time in seconds since last call to qpalm_tic.
 * @param  t Timer object
 * @return   Reported time in seconds
 */
c_float qpalm_toc(QPALMTimer *t);

# endif /* END #ifdef QPALM_TIMING */


# ifdef __cplusplus
}
# endif 

#endif /* ifndef UTIL_H */