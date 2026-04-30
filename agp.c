#include "agp.h"

/* Types of options (i.e. arguments prefixed by '-' */
typedef enum agp_OptionType {
    OPTION,
    PARAMETER_OPTION
} agp_OptionType;

/* --- PRIVATE --- */
static void
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

static void
freeNullSafe(void *pointer)
{
    if (pointer != NULL) {
        free(pointer);
    }
}

static void
freeArrayOfStringsNullSafe(char **array, int numElements)
{
    if (array == NULL) {
        return;
    }

    for (int i = 0; i<numElements; i++) {
        freeNullSafe(array[i]);
    }
}

static void
printfreed(char *freed)
{
    printf("[DEBUG] %s freed!\n", freed);
}

static void
getOptionStringFromOption(
    char **dst, char *src, int srcLen, agp_OptionType type, int paramLen)
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

static int
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

/* if this returns 0, that means the option was not a parameterOption
 * Otherwise, returns the length of the parameter in the arg string.
 * So if a bad parameter was passed in, like '-pm=', it is detected
 * as an 'option' instead of a parameter option
 * The parameter can be extracted at
 * parameterString = (option+(optionLength-retval));
 */
static int
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

/* --- PUBLIC --- */

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

/* Never free anything inside an agp_ArgTemplate, use this function
 * to free everything inside an agp_ArgTemplate. agp_ArgTemplate is designed
 * to be a read-only struct
 */
void
agp_freeArgTemplate(agp_ArgTemplate *at)
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

agp_ArgError
agp_parseArgs(agp_ArgTemplate *at, int argc, char *argv[])
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
                freeNullSafe(parameter);
                freeNullSafe(parameterOption);
                return UNKNOWN_PARAMETER_OPTION;
            }

            // do not freeNullSafe(parameter) here. The parameter is a valid
            // pointer, and is being used inside `at`.
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
agp_checkOptionPresence(agp_ArgTemplate *at, char *option)
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
agp_getParameter(agp_ArgTemplate *at, char *parameterOption)
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
