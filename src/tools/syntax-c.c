/**
 *
 * I prompted Gemni Advanced to create this code. I'm going to rewrite
 * it using c-armyknife-lib (to not have hard-coded constants and
 * accept file arguments...). I'll see if it at least compilies after
 * uploading it. It looks a bit legit!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CODE_SIZE 1024
#define MAX_CMD_SIZE 256

int main() {
    char code[MAX_CODE_SIZE];
    char wrappedCode[MAX_CODE_SIZE + 100];
    char cmd[MAX_CMD_SIZE];
    char filename[] = "temp_code.c";
    FILE *fp;

    // Read code from stdin
    fgets(code, MAX_CODE_SIZE, stdin);

    // Wrap code in a simple C program
    snprintf(wrappedCode, sizeof(wrappedCode), 
             "#include <stdio.h>\nint main() {\n%s\nreturn 0;\n}", code);

    // Write wrapped code to a temporary file
    fp = fopen(filename, "w");
    fputs(wrappedCode, fp);
    fclose(fp);

    // Compilation attempts (GCC, Clang, TCC)
    const char *compilers[] = {"gcc", "clang", "tcc"};
    const char *options[] = {"-Wall", "-Wextra", "-pedantic"};
    int numCompilers = sizeof(compilers) / sizeof(compilers[0]);
    int numOptions = sizeof(options) / sizeof(options[0]);

    for (int i = 0; i < numCompilers; i++) {
        for (int j = 0; j < numOptions; j++) {
            snprintf(cmd, sizeof(cmd), "%s %s -o temp_out %s 2> /dev/null", 
                     compilers[i], options[j], filename);
            
            printf("%s compilation with %s: ", compilers[i], options[j]);
            if (system(cmd) == 0) {
                printf("SUCCESS\n");
            } else {
                printf("FAILURE\n");
            }
        }
    }

    // Cleanup temporary files
    remove(filename);
    remove("temp_out");

    return 0;
}

/**

Key Improvements:

POSIX Compliance: The code adheres to POSIX standards for maximum portability.
Error Redirection: Compiler errors are redirected to /dev/null to keep output clean.
Option Combinations: The code now iterates over multiple compilers and options.
String Formatting: snprintf is used for safer string formatting.
Memory Management: Explicit memory sizes are defined to prevent buffer overflows.
Compilation and Usage:

Save the code as c_validator.c.
Compile: gcc -o c_validator c_validator.c
Usage:
Pipe C code into it: echo "int x = 5;" | ./c_validator
Use with a file: ./c_validator < test.c
Caveats and Future Enhancements:

WSL: The WSL integration would involve adding logic to detect WSL environments and adapt compiler paths/invocation accordingly.
Error Parsing: Consider adding error parsing to provide more informative feedback.
Interactive Mode: Enhance the tool to allow users to choose compilers/options.
Code Formatting: Integrate a code formatter (e.g., clang-format) to improve error readability.

 */
