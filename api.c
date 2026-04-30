#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Main struct for parsing arguments
 * NOTE: do not free/write any datato any of the pointers within
 * this struct. This struct must only be interacted with using the
 * api functions, or as a read-only struct
 */
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
copyArrayOfStrings(char ***dst, char ***src, int numStrings)
{
    *dst = malloc(sizeof(char *) * numStrings);
    for (int i = 0; i<numStrings; i++) {
        if ((*src)[i] != NULL) {
            int argLen = strlen((*src)[i]);
            (*dst)[i] = malloc(sizeof(char) * (argLen+1));
            strcpy((*dst)[i], (*src)[i]);
        } else {
            (*dst)[i] = NULL;
        }
    }
}

void
initArgTemplate(
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
    int programNameLen = strlen(programName);
    at->programName = malloc(sizeof(char) * (programNameLen+1));
    strcpy(at->programName, programName);

    at->numOptions = numOptions;
    if (numOptions != 0) {
        copyArrayOfStrings(&at->options, &options, numOptions);

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
        copyArrayOfStrings(&at->parameterOptions, &parameterOptions,
            numParameterOptions);

        if (parameters != NULL) {
            copyArrayOfStrings(&at->parameters, &parameters,
                numParameterOptions);
        } else {
            at->parameters = malloc(sizeof(char *) * numParameterOptions);
            for (int i = 0; i<numParameterOptions; i++) {
                at->parameters[i] = NULL;
            }
        }
    } else {
        at->parameterOptions = NULL;
        at->parameters = NULL;
    }

    at->numOtherArgs = 0;
    at->otherArgs = NULL;
}

void
freeNullSafe(void *pointer)
{
    if (pointer != NULL) {
        free(pointer);
    }
}

void
freeArrayOfStringsNullSafe(char **array, int numElements)
{
    if (array == NULL) {
        return;
    }

    for (int i = 0; i<numElements; i++) {
        freeNullSafe(array[i]);
    }
}

void
printfreed(char *freed)
{
    printf("[DEBUG] %s freed!\n", freed);
}

/* Never free anything inside an ArgTemplate, use this function
 * to free everything inside an ArgTemplate. ArgTemplate is designed
 * to be a read-only struct
 */
void
freeArgTemplate(ArgTemplate *at)
{
    freeNullSafe(at->programName);

    freeArrayOfStringsNullSafe(at->options, at->numOptions);
    freeNullSafe(at->options);
    freeNullSafe(at->optionPresence);

    freeArrayOfStringsNullSafe(at->parameterOptions, at->numParameterOptions);
    freeArrayOfStringsNullSafe(at->parameters, at->numParameterOptions);
    freeNullSafe(at->parameterOptions);
    freeNullSafe(at->parameters);

    freeArrayOfStringsNullSafe(at->otherArgs, at->numOtherArgs);
    freeNullSafe(at->otherArgs);
}

// if this returns 0, that means the option was not a parameterOption
// Otherwise, returns the length of the parameter in the arg string
// The parameter can be extracted at
// parameterString = (option+(optionLength-retval));
int
getParameterLength(char *arg, int argLen)
{
    int parameterLength = 0;
    for (int j = 0; j<argLen; j++) {
        if (arg[j] == '=') {
            parameterLength = argLen - j - 1;
            break;
        }
    }
    return parameterLength;
}

void
getOptionStringFromOption(
    char **dst, char *src, int srcLen, OptionType type, int paramLen)
{
    int offset; // due to '=' in the argument
    if (type == OPTION) {
        // option
        offset = 0;
        paramLen = 0;
    } else {
        // parameterOption
        offset = 1;
    }

    *dst = malloc(sizeof(char) * (srcLen-paramLen-offset));
    (*dst)[srcLen-paramLen-1-offset] = '\0';
    strncpy((*dst), src+1, srcLen-paramLen-1-offset);
}

int
findStringIdx(char **array, int arrayLen, char *string) {
    if (array == NULL || string == NULL) {
        return -1;
    }

    for (int i = 0; i<arrayLen; i++) {
        if (array[i] != NULL && strcmp(array[i], string) == 0) {
            return i;
        }
    }

    return -1;
}

ArgError
parseArgs(ArgTemplate *at, int argc, char *argv[])
{
    if (strcmp(argv[0], at->programName) != 0) {
        return WRONG_PROGRAM_NAME;
    }

    int numOtherArgs = 0;
    for (int i = 1; i<argc; i++) {
        if (argv[i][0] != '-') {
            numOtherArgs++;
            continue;
        }

        int argLen = strlen(argv[i]);
        int parameterLength = getParameterLength(
                                argv[i],
                                argLen
                            );

        if (parameterLength != 0) {
            // parameterOption
            char *parameter = malloc(sizeof(char) * (parameterLength+1));
            strcpy(parameter, argv[i] + argLen-parameterLength);

            char *parameterOption = NULL;
            getOptionStringFromOption(
                &parameterOption, argv[i],
                argLen, PARAMETER_OPTION, parameterLength
            );

            int valid = 0;
            int parameterOptionIdx = findStringIdx(
                                        at->parameterOptions,
                                        at->numParameterOptions,
                                        parameterOption
                                    );
            if (parameterOptionIdx >= 0) {
                valid = 1;
                at->parameters[parameterOptionIdx] = parameter;
            }

            if (!valid) {
                freeNullSafe(parameterOption);
                return UNKNOWN_PARAMETER_OPTION;
            }

            freeNullSafe(parameterOption);
        } else {
            // option
            char *option = NULL;
            getOptionStringFromOption(
                &option, argv[i],
                argLen, OPTION, parameterLength
            );

            int valid = 0;
            int optionIdx = findStringIdx(
                                        at->options,
                                        at->numOptions,
                                        option
                                    );
            if (optionIdx >= 0) {
                valid = 1;
                at->optionPresence[optionIdx] = 1;
            }

            if (!valid) {
                freeNullSafe(option);
                return UNKNOWN_OPTION;
            }

            freeNullSafe(option);
        }
    }

    at->numOtherArgs = numOtherArgs;
    at->otherArgs = malloc(sizeof(char *) * numOtherArgs);
    int otherArgIdx = 0;
    for (int i = 1; i<argc; i++) {
        if (argv[i][0] != '-') {
            int argLen = strlen(argv[i]);
            at->otherArgs[otherArgIdx] = malloc(sizeof(char) * (argLen+1));
            strcpy(at->otherArgs[otherArgIdx], argv[i]);
            otherArgIdx++;
        }
    }
    return NONE;
}

int
checkOptionPresence(ArgTemplate *at, char *option)
{
    if (at->options == NULL || option == NULL) {
        return 0;
    }

    for (int i = 0; i<at->numOptions; i++) {
        if (at->options[i] != NULL
            && strcmp(at->options[i], option) == 0) {
            if (at->optionPresence[i] != 0) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    return 0;
}

char *
getParameter(ArgTemplate *at, char *parameterOption)
{
    if (at->parameterOptions == NULL || parameterOption == NULL) {
        return NULL;
    }

    for (int i = 0; i<at->numParameterOptions; i++) {
        if (at->parameterOptions[i] != NULL
            && strcmp(at->parameterOptions[i], parameterOption) == 0) {

            return at->parameters[i];
        }
    }

    return NULL;
}

int
main(int argc, char *argv[])
{
    ArgTemplate at;
    char *options[2] = {"lol", "lmao"};
    char *parameterOptions[3] = {"po1", "poo2", "p3"};

    initArgTemplate(
        &at,
        argv[0],

        2,
        options,
        NULL,

        3,
        parameterOptions,
        NULL
    );

    printf("---initArgTemplate test---\n");
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
        for (int i = 0; i<at.numParameterOptions; i++) {
            printf("parameterOption %d : %s\n", i+1, at.parameterOptions[i]);
        }
    }

    int error = parseArgs(&at, argc, argv);

    printf("---parseArgs test---\n");

    printf("numOptions : %d\n", at.numOptions);
    printf("numParameterOptions : %d\n", at.numParameterOptions);
    printf("numOtherArgs : %d\n", at.numOtherArgs);

    if (error != NONE) {
        printf("Error: %d\n", error);
        return 0;
    }

    for (int i = 0; i<at.numOptions; i++) {
        printf("Option: %s, presence: %d\n", at.options[i], at.optionPresence[i]);
    }
    for (int i = 0; i<at.numParameterOptions; i++) {
        printf("parameterOption: %s, value: %s\n", at.parameterOptions[i], at.parameters[i] != NULL ? at.parameters[i] : "-");
    }
    for (int i = 0; i<at.numOtherArgs; i++) {
        printf("otherArgs: %s\n", at.otherArgs[i]);
    }

    printf("---freeArgTemplate test---\n");
    freeArgTemplate(&at);
    printf("freeArgTemplate worked!\n");

    return 0;
}
