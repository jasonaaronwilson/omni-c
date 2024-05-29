/**
 * @file syntax-c.c
 *
 * A tool that reads stdin and compiles it using gcc, clang and tcc to
 * understand if it is valid as a "top-level" form.
 */

#define C_ARMYKNIFE_LIB_IMPL
#include <c-armyknife-lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// jawilson: this appears to be used to construct a command line
// string. buffer_t* should be pretty good at handling that.
#define MAX_COMMAND_LINE_SIZE 256

int main() {
    char filename[] = "temp_src_code.c";

    // Slurp stdin
    buffer_t* input_buffer = buffer_append_all(make_buffer(0), stdin);

    // Wrap what we slurped in with a wrapper program...
    buffer_t* wrapped_src_code = make_buffer(input_buffer->capacity + 1024*1);
    wrapped_src_code = buffer_printf(wrapped_src_code, "#include <stdio.h>\nint main() {\n%s\nreturn 0;\n}", 
				     buffer_to_c_string(input_buffer));

    // Write wrapped src_code to a temporary file
    buffer_write_file(wrapped_src_code, filename);

    // Compilation attempts (GCC, Clang, TCC)
    const char *compilers[] = {"gcc", "clang", "tcc"};
    const char *options[] = {"-Wall", "-Wextra", "-pedantic"};
    int num_compilers = sizeof(compilers) / sizeof(compilers[0]);
    int num_options = sizeof(options) / sizeof(options[0]);

    for (int i = 0; i < num_compilers; i++) {
        for (int j = 0; j < num_options; j++) {
	  char command_line[MAX_COMMAND_LINE_SIZE];
            snprintf(command_line, sizeof(command_line), "%s %s -o temp_out %s 2> /dev/null", 
                     compilers[i], options[j], filename);
            
            printf("%s compilation with %s: ", compilers[i], options[j]);
            if (system(command_line) == 0) {
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
