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

typedef enum OptionType {
    OPTION,
    PARAMETER_OPTION
} OptionType;

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

    at->numOptions = numOptions;
    if (numOptions != 0) {
        copy_array_of_strings(&at->options, &options, numOptions);

        at->optionPresence = malloc(sizeof(int) * numOptions);
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
        at->options = NULL;
        at->optionPresence = NULL;
    }

    at->numParameterOptions = numParameterOptions;
    if (numParameterOptions != 0) {
        copy_array_of_strings(&at->parameterOptions, &parameterOptions,
            numParameterOptions);

        if (parameters != NULL) {
            copy_array_of_strings(&at->parameters, &parameters,
                numParameterOptions);
        } else {
            for (int i = 0; i<numParameterOptions; i++) {
                at->parameterOptions[i] = NULL;
            }
        }
    } else {
        at->parameterOptions = NULL;
        at->parameters = NULL;

    }

    at->numOtherArgs = 0;
    at->otherArgs = NULL;
}

// if this returns 0, that means the option was not a parameterOption
// Otherwise, returns the length of the parameter in the arg string
// The parameter can be extracted as
// parameterString = (option+(optionLength-retval));
int
parameterLengthExtractor(char *arg, int argLength)
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

void
get_string_from_option(
    char **dst, char *src, int srcLen, OptionType type, int paramLen)
{
    int offset;
    if (type == 0) {
        // option
        offset = 0;
        paramLen = 0;
    } else {
        // parameterOption
        offset = 1;
    }

    *dst = malloc(sizeof(char) * srcLen-paramLen-offset);
    (*dst)[srcLen-paramLen-1-offset] = '\0';
    strncpy((*dst), src+1, srcLen-paramLen-1-offset);
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

                char *parameterOption = NULL;
                get_string_from_option(
                    &parameterOption, argv[i],
                    stringLength, PARAMETER_OPTION, parameterLength
                );

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
                char *option = NULL;
                get_string_from_option(
                    &option, argv[i],
                    stringLength, OPTION, parameterLength
                );

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
            strcpy(as->otherArgs[otherArgIdx], argv[i]);
            otherArgIdx++;
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
        3,
        parameterOptions,
        NULL
    );

    printf("---init_arg_template_test---\n");
    printf("numOptions : %d\n", at.numOptions);
    if (at.options == NULL) {
        printf("OPTIONS IS NULL\n");
    } else {
        for (int i = 0; i<at.numOptions; i++) {
            printf("Option %d : %s\n", i+1, at.options[i]);
        }
    }

    printf("numParameterOptions : %d\n", at.numParameterOptions);
    if (at.parameterOptions == NULL) {
        printf("PARAMETER_OPTIONS IS NULL\n");
    } else {
        for (int i = 0; i<at.numOptions; i++) {
            printf("parameterOption %d : %s\n", i+1, at.parameterOptions[i]);
        }
    }

    int error = parse_args(&at, &as, argc, argv);

    printf("---parse_args_test---\n");
    if (error != NONE) {
        printf("Error: %d\n", error);
        return 0;
    }

    for (int i = 0; i<as.numOptions; i++) {
        printf("Option: %s, presence: %d\n", as.options[i], as.optionPresence[i]);
    }
    for (int i = 0; i<as.numParameterOptions; i++) {
        printf("parameterOption: %s, value: %s\n", as.parameterOptions[i], as.parameters[i] != NULL ? as.parameters[i] : "-");
    }
    for (int i = 0; i<as.numOtherArgs; i++) {
        printf("otherArgs: %s\n", as.otherArgs[i]);
    }

    return 0;
}
