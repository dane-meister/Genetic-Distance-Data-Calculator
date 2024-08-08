#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "debug.h"

int main(int argc, char **argv)
{
    if(validargs(argc, argv))
        USAGE(*argv, EXIT_FAILURE);
    if(global_options == HELP_OPTION)
        USAGE(*argv, EXIT_SUCCESS);
    // TO BE IMPLEMENTED
    //*read distance data
    int result;
    result = read_distance_data(stdin);
    if (result == -1)
    {
        return EXIT_FAILURE;
    }
    if (global_options == MATRIX_OPTION)
    {
        //*matrix option
        result = build_taxonomy(stdout);
        if (result == -1)
        {
            return EXIT_FAILURE;
        }
        emit_distance_matrix(stdout);
        if (result == -1)
        {
            return EXIT_FAILURE;
        }
    }
    else if (global_options == NEWICK_OPTION)
    {
        //*newick option
        result = emit_newick_format(stdout);
        if (result == -1)
        {
            return EXIT_FAILURE;
        }
    }
    else 
    {
        //*base option
        result = build_taxonomy(stdout);
        if (result == -1)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS; 
}

/**
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
