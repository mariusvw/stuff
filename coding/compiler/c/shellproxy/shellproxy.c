/*
# Shell Proxy
#
# For example in PHP this gives you the ability to fire a command using system()
# and not let PHP wait for the closure of stdin/stdout/stderr.
#
# You will mostly want to use it when starting things in de background.
#
# Compile:
# gcc -o shellproxy shellproxy.c
# clang -o shellproxy shellproxy.c
#
# Any questions?
# Marius van Witzenburg <info@mariusvw.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char command[1024];
    int t;

    if (argc == 1) {
        printf("\nShell Proxy\n");
        printf("-----------\n");
        printf("Usage:\n");
        printf("- CLI:\n");
        printf("shellproxy <cmd> <arg>\n");
        printf("shellproxy <cmd> <arg> > redirect.txt\n");
        printf("- PHP:\n");
        printf("system('shellproxy <cmd> <arg> &');\n");
        printf("system('shellproxy <cmd> <arg> > redirect.txt');\n\n");
    }

    command[0] = 0;

    for (t = 1; t < argc; t++) {
        strcat(command, argv[t]);
        strcat(command, " ");
    }

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);

    system(command);
}
