#ifndef argc_H
#define argc_H

#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Main struct for parsing arguments
 * NOTE: do not free/write any datato any of the pointers within
 * this struct. This struct must only be interacted with using the
 * api functions, or as a read-only struct.
 * Note that none of options, parameterOptions, or otherArgs can have
 * '=' in thier name, when passed as arguments, or when init'ing argc_ArgTemplate
 * '=' is used as a separator between parameterOption and the corresponding
 * parameter on the command line
 */
typedef struct argc_ArgTemplate {
    char *programName;
    int numOptions;
    char **options;
    int *optionPresence;        // boolean array
    int numParameterOptions;
    char **parameterOptions;
    char **parameters;
    int numOtherArgs;
    char **otherArgs;
} argc_ArgTemplate;

/* Error values encountered while parsing arguments */
typedef enum argc_ArgError {
    NONE,
    WRONG_PROGRAM_NAME,
    UNKNOWN_OPTION,
    UNKNOWN_PARAMETER_OPTION
} argc_ArgError;

/* Init: pass in your arguments of your argc_ArgTemplate to init */
void
argc_initArgTemplate(
    argc_ArgTemplate *at,
    char *programName,

    int numOptions,
    char **options,
    int *optionPresence,

    int numParameterOptions,
    char **parameterOptions,
    char **parameters
);

/* Free: free the memory occupied by argc_ArgTemplate */
void
argc_freeArgTemplate(argc_ArgTemplate *at);

argc_ArgError
argc_parseArgs(argc_ArgTemplate *at, int argc, char *argv[]);

/* check if 'option' is present in at.
 * If the 'option' is an invalid option that was never a part of at->options,
 * returns 0
 * If the 'option' was present in the arguments,
 * returns 1
 * else returns 0
 */
int
argc_checkOptionPresence(argc_ArgTemplate *at, char *option);

/* find out the parameter corresponding to 'parameterOption' in arguments
 * If the 'parameterOption' is an invalid parameterOption that was never
 * a part of at->options,
 * returns NULL
 * If the 'parameterOption' was present in the arguments,
 * returns the corresponding parameter
 * else returns NULL
 */
char *
argc_getParameter(argc_ArgTemplate *at, char *parameterOption);

#endif
