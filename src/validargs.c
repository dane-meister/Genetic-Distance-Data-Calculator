#include <stdlib.h>

#include "global.h"
#include "debug.h"

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the various options that were specified will be
 * encoded in the global variable 'global_options', where it will be
 * accessible elsewhere in the program.  For details of the required
 * encoding, see the assignment handout.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * @modifies global variable "global_options" to contain an encoded representation
 * of the selected program options.
 */
int validargs(int argc, char **argv)
{
    // No arguments / no flags check (progname, NULL)
    if (argc <= 1) {
        return 0;
    }

    // Pointer to the first argument
    char **arg_1 = (argv + 1);

    // Checks if the first argument is "-h", otherwise it does nothing
    if (arg_1 != NULL && **arg_1 == '-' && *(*(arg_1) + 1) == 'h' && *(*(arg_1) + 2) == '\0') 
    {
        global_options = HELP_OPTION;
        return 0;
    } 
    else if (arg_1 != NULL && **arg_1 == '-' && *(*(arg_1) + 1) == 'm' && *(*(arg_1) + 2) == '\0') 
    {
        // Checks for argument -m
        global_options = MATRIX_OPTION;
        if (*(arg_1 + 1) != NULL) {
            return -1;
        }
        return 0;
    } 
    else if (arg_1 != NULL && **arg_1 == '-' && *(*(arg_1) + 1) == 'n' && *(*(arg_1) + 2) == '\0') 
    {
        // Checks for argument -n
        global_options = NEWICK_OPTION;
        if (*(arg_1 + 1) == NULL) {
            // no optional -o flag
            return 0;
        } 
        else if (*(*(arg_1 + 1)) == '-' && *(*(arg_1 + 1) + 1) == 'o' && *(*(arg_1 + 1) + 2) == '\0') 
        {
            // optional -o flag is present
            if (*(arg_1 + 2) != NULL && *(arg_1 + 3) == NULL) 
            {
                outlier_name = *(arg_1 + 2);
                return 0;
            }
        }
    }
    return -1;
    abort();
}
