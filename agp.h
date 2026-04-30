#ifndef AGP_H
#define AGP_H

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Main struct for parsing arguments
 * NOTE: do not free/write any datato any of the pointers within
 * this struct. This struct must only be interacted with using the
 * api functions, or as a read-only struct.
 * Note that none of options, parameterOptions, or otherArgs can have
 * '=' in thier name, when passed as arguments, or when init'ing agp_ArgTemplate
 * '=' is used as a separator between parameterOption and the corresponding
 * parameter on the command line
 */
typedef struct agp_ArgTemplate {
    char *programName;
    int numOptions;
    char **options;
    int *optionPresence;        // boolean array
    int numParameterOptions;
    char **parameterOptions;
    char **parameters;
    int numOtherArgs;
    char **otherArgs;
} agp_ArgTemplate;

/* Error values encountered while parsing arguments */
typedef enum agp_ArgError {
    NONE,
    WRONG_PROGRAM_NAME,
    UNKNOWN_OPTION,
    UNKNOWN_PARAMETER_OPTION
} agp_ArgError;

/* Init: pass in your arguments of your agp_ArgTemplate to init */
void
agp_initArgTemplate(
    agp_ArgTemplate *at,
    char *programName,

    int numOptions,
    char **options,
    int *optionPresence,

    int numParameterOptions,
    char **parameterOptions,
    char **parameters
);

/* Free: free the memory occupied by agp_ArgTemplate */
void
agp_freeArgTemplate(agp_ArgTemplate *at);

agp_ArgError
agp_parseArgs(agp_ArgTemplate *at, int argc, char *argv[]);

/* check if 'option' is present in at.
 * If the 'option' is an invalid option that was never a part of at->options,
 * returns 0
 * If the 'option' was present in the arguments,
 * returns 1
 * else returns 0
 */
int
agp_checkOptionPresence(agp_ArgTemplate *at, char *option);

/* find out the parameter corresponding to 'parameterOption' in arguments
 * If the 'parameterOption' is an invalid parameterOption that was never
 * a part of at->options,
 * returns NULL
 * If the 'parameterOption' was present in the arguments,
 * returns the corresponding parameter
 * else returns NULL
 */
char *
agp_getParameter(agp_ArgTemplate *at, char *parameterOption);

#endif
