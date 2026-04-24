#include <stdio.h>

typedef struct ArgTemplate {
    char *programName;
    int numShortOptions;
    char **shortOptions;
    int numLongOptions;
    char **longOptions;
    int numParameterOptions;
    char **parameterOptions;
    char **parameters;
    char **otherArgs;
}

typedef enum ArgError {
    WRONG_PROGRAM_NAME,
    UNKNOWN_SHORT_OPTION,
    UNKNOWN_LONG_OPTION,
    UNKNOWN_PARAMETER_OPTION
} ArgError;

void
copy_array_of_strings(char **dst, char **src, int numStrings)
{
    dst = malloc(sizeof(char *) * numStrings);
    for (int i = 0; i<numStrings; i++) {
        int stringLength = strlen(src[i]);
        dst[i] = malloc(sizeof(char *) * strlen(src[i]));
        for (int j = 0; j<stringLength; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

void
set_arg_template(
    ArgTemplate *at,
    char *programName,
    int numShortOptions,
    char **shortOptions,
    int numLongOptions,
    char **numLongOptions,
    int numParameterOptions,
    char **parameterOptions
)
{
    at->programName = programName;
    copy_array_of_strings(at->shortOptions, shortOptions, numShortOptions);
    copy_array_of_strings(at->longOptions, longOptions, numLongOptions);
    copy_array_of_strings(at->parameterOptions, parameterOptions,
        numParameterOptions);
    at->numShortOptions = numShortOptions;
    at->numLongOptions = numLongOptions;
    at->numParameterOptions = numParameterOptions;
}

// if this returns 0, that means the option was not a parameterOption
// Otherwise, returns the length of the parameter in the arg string
// The parameter can be extracted as
// parameterString = (option+(optionLength-retval));
int parameterLengthExtractor(char *arg, int argLength)
{
    int parameterLength = 0;
    for (int j = 0; j<argLength; j++) {
        if (arg[j] == '=') {
            parameterLength = argLength - j - 1;
            break;
        }
    }
    return parameterLength;
}

ArgError
parse_args(ArgTemplate *template, ArgTemplate *as, int argc, char *argv[])
{
    if (strcmp(argv[0], template->programName) != 0) {
        return WRONG_PROGRAM_NAME;
    }

    for (int i = 1; i<argc; i++) {
        if (argv[i][0] == '-')
            int stringLength = strlen(argv[i]);
            int parameterLength = parameterLengthExtractor(argv[i],
                                    stringLength);

            if (parameterLength != 0) {
                // parameterOption
            } else if (argv[i][1] == '-') {
                // longOption
            } else {
                // shortOption
            }
        } else {
            // otherArgs
        }
    }
}

