# argc
Quick Argument parsing for C

# Rules
- Three types of arguments: options, parameter options, other arguments
    - Options, parameter options: prefixed with a single '-' on the command line.
    - Other arguments: no '-' prefix on the command line.
- '=' is a special token used to separate a parameter option name, and its parameter when providing it as an argument on the command line. Doing so makes the argument either:
    - Evaluate to an (invalid) option if nothing follows '='.
    - Evaluate to an parameter option if something follows '='.
- For the api, simply specify an array of options/parameterOptions as such:
    - char *options[2] = {"lol", "lmao"};
    - char *parameterOptions[3] = {"p1", "parameter2", pm3"};
- A 'string' is simply a character array/pointer

# Quick start
test.c:
```c
#include "agp.h"

int
main(int argc, char *argv[])
{
    agp_ArgTemplate at;
    char *options[2] = {"lol", "lmao"};
    char *parameterOptions[3] = {"p1", "parameter2", "pm3"};

    agp_initArgTemplate(
        &at,
        argv[0],

        2,
        options,
        NULL,   // specify an array of default optionPresence values (0 or 1), or pass NULL

        3,
        parameterOptions,
        NULL    // specify an array of default parameterOption values (array of strings), or pass NULL
    );

    printf("numOptions : %d\n", at.numOptions);
    printf("numParameterOptions : %d\n", at.numParameterOptions);
    printf("numOtherArgs : %d\n", at.numOtherArgs);

    int error = agp_parseArgs(&at, argc, argv);

    printf("---Argument parsing---\n");

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

    agp_freeArgTemplate(&at);

    return 0;
}

```

run:
```bash
gcc test.c agp.c -o test.out
./test.out -lol -p1=me you -parameter2=happy
```
