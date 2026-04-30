#include "../agp.h"

int
main(int argc, char *argv[])
{
    agp_ArgTemplate at;
    char *options[2] = {"lol", "lmao"};
    char *parameterOptions[3] = {"po1", "poo2", "p3"};

    agp_initArgTemplate(
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

    int error = agp_parseArgs(&at, argc, argv);

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
    agp_freeArgTemplate(&at);
    printf("freeArgTemplate worked!\n");

    return 0;

    // ./api.out -lol -po1=name game
    // ./api.out -lol game
    // ./api.out
}
