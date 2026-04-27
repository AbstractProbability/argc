#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ArgTemplate {
    char *programName;
    int numOptions;
    char **options;
    int *optionPresence;        // boolean array
    int numParameterOptions;
    char **parameterOptions;
    char **parameters;
    int numOtherArgs;
    char **otherArgs;
} ArgTemplate;

typedef enum ArgError {
    NONE,
    WRONG_PROGRAM_NAME,
    UNKNOWN_OPTION,
    UNKNOWN_PARAMETER_OPTION
} ArgError;

void
copy_array_of_strings(char ***dst, char ***src, int numStrings)
{
    *dst = malloc(sizeof(char *) * numStrings);
    for (int i = 0; i<numStrings; i++) {
        int stringLength = strlen((*src)[i]);
        (*dst)[i] = malloc(sizeof(char *) * (stringLength+1));
        for (int j = 0; j<stringLength; j++) {
            (*dst)[i][j] = (*src)[i][j];
        }
    }
}

void
init_arg_template(
    ArgTemplate *at,
    char *programName,
    int numOptions,
    char **options,
    int *optionPresence,
    int numParameterOptions,
    char **parameterOptions,
    char **parameters
)
{
    at->programName = programName;
    if (numOptions != 0) {
        copy_array_of_strings(&at->options, &options, numOptions);
    } else {
        at->options = NULL;
    }

    if (numParameterOptions != 0) {
        copy_array_of_strings(&at->parameterOptions, &parameterOptions,
            numParameterOptions);
    } else {
        at->parameterOptions = NULL;
    }

    // set default presence values, or 0 (false) if default values are not
    // passed
    at->optionPresence = malloc(sizeof(int) * numOptions);
    if (numOptions != 0) {
        if (optionPresence != NULL) {
            for (int i = 0; i<numOptions; i++) {
                at->optionPresence[i] = optionPresence[i];
            }
        } else {
            for (int i = 0; i<numOptions; i++) {
                at->optionPresence[i] = 0;
            }
        }
    } else {
        optionPresence = NULL;
    }

    // set default values, or NULL if default values are not passed
    if (numParameterOptions != 0) {
        if (parameters != NULL) {
            copy_array_of_strings(&at->parameters, &parameters,
                numParameterOptions);
        } else {
            for (int i = 0; i<numParameterOptions; i++) {
                at->parameterOptions[i] = NULL;
            }
        }
    } else {
        parameters = NULL;
    }

    at->numOptions = numOptions;
  //  at->numParameterOptions = numParameterOptions;

    at->numOtherArgs = 0;
    at->otherArgs = NULL;
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
parse_args(ArgTemplate *at, ArgTemplate *as, int argc, char *argv[])
{
    if (strcmp(argv[0], at->programName) != 0) {
        return WRONG_PROGRAM_NAME;
    }

    int numOtherArgs = 0;
    init_arg_template(
        as,
        at->programName,
        at->numOptions,
        at->options,
        at->optionPresence,
        at->numParameterOptions,
        at->parameterOptions,
        at->parameters
    );

    for (int i = 1; i<argc; i++) {
        if (argv[i][0] == '-') {
            int stringLength = strlen(argv[i]);
            int parameterLength = parameterLengthExtractor(argv[i],
                                    stringLength);

            if (parameterLength != 0) {
                // parameterOption
                char *parameter = argv[i] + 1 + stringLength-parameterLength;

                char *parameterOption = malloc(sizeof(char) * stringLength-parameterLength-1);
                parameterOption[stringLength-parameterLength-2] = '\0';
                strncpy(parameterOption, argv[i]+1, stringLength-parameterLength-2);

                int valid = 0;
                for (int j = 0; j<at->numParameterOptions; j++) {
                    if (strcmp(at->parameterOptions[j], parameterOption) == 0) {
                        as->parameters[j] = parameter;
                        valid = 1;
                        break;
                    }
                }

                if (!valid) {
                    return UNKNOWN_PARAMETER_OPTION;
                }
            } else {
                // option
                char *option = malloc(sizeof(char) * stringLength-parameterLength);
                option[stringLength-parameterLength-1] = '\0';
                strncpy(option, argv[i]+1, stringLength-parameterLength-1);

                int valid = 0;
                for (int j = 0; j<at->numOptions; j++) {
                    if (strcmp(at->options[j], option) == 0) {
                        as->optionPresence[j] = 1;
                        valid = 1;
                        break;
                    }
                }

                if (!valid) {
                    return UNKNOWN_OPTION;
                }
            }
        } else {
            numOtherArgs++;
        }
    }

    as->numOtherArgs = numOtherArgs;
    as->otherArgs = malloc(sizeof(char *) * numOtherArgs);
    int otherArgIdx = 0;
    for (int i = 1; i<argc; i++) {
        if (argv[i][0] != '-') {
            int otherArgLen = strlen(argv[i]);
            as->otherArgs[otherArgIdx] = malloc(sizeof(char) * (otherArgLen));
            strcpy(as->otherArgs[otherArgIdx], argv[i]+1);
        }
    }
    return NONE;
}

int main(int argc, char *argv[]) {
    ArgTemplate at, as;
    char **options = malloc(sizeof(char *) * 2);
    options[0] = malloc(sizeof(char) * 4);
    options[1] = malloc(sizeof(char) * 5);
    strcpy(options[0], "lol");
    strcpy(options[1], "lmao");

    char **parameterOptions = malloc(sizeof(char *) * 3);
    parameterOptions[0] = malloc(sizeof(char) * 4);
    parameterOptions[1] = malloc(sizeof(char) * 5);
    parameterOptions[2] = malloc(sizeof(char) * 3);
    strcpy(parameterOptions[0], "po1");
    strcpy(parameterOptions[0], "poo2");
    strcpy(parameterOptions[0], "po3");

    init_arg_template(
        &at,
        argv[0],
        2,
        options,
        NULL,
        0,
        NULL,
        NULL
    );

    printf("numOptions : %d\n", at.numOptions);
    if (at.options == NULL) {
        printf("OPTIONS IS NULL\n");
        return 0;
    }
    for (int i = 0; i<at.numOptions; i++) {
        printf("Option %d : %s\n", i+1, at.options[i]);
    }
    printf("numParameterOptions : %d\n", at.numParameterOptions);
    if (at.parameterOptions == NULL) {
        printf("PARAMETER_OPTIONS IS NULL\n");
        return 0;
    }
    for (int i = 0; i<at.numOptions; i++) {
        printf("parameterOption %d : %s\n", i+1, at.parameterOptions[i]);
    }

    return 0;
}
