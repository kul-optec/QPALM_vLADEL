/**
 * @file validate.h
 * @author Ben Hermans
 * @brief Validation of the user provided settings and data.
 * @details The assumptions on the settings can be found in the 
 * details of the QPALMSettings tab in data structures.
 */
#ifndef VALIDATE_H
# define VALIDATE_H

# ifdef __cplusplus
extern "C" {
# endif 

# include "types.h"

/***********************************************************
* Validation of data and settings * *
***********************************************************/

/**
 * Validate problem data.
 * 
 * Checks whether the upper bounds are always greater than or equal
 * to the lower bounds. Dimension checking of the data matrices and 
 * vectors is assumed to be done by the wrappers and interfaces.
 * 
 * @param  data Data to be validated
 * @return      Exitflag to check
 */
c_int validate_data(const QPALMData *data);


/**
 * Validate problem settings.
 * 
 * Checks the assumptions on all settings. The assumptions can be 
 * found in the details of the QPALMSettings tab in data structures.
 * 
 * @param  settings Settings to be validated
 * @return      Exitflag to check
 */
c_int validate_settings(const QPALMSettings *settings);


# ifdef __cplusplus
}
# endif 

#endif // ifndef AUXIL_H