// The almighty stack-overflow gods blessed me with: https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
#include "string_replace.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// You must free the result if result is non-NULL.
char *strrep(char *orig, char *rep, char *with) {
    char *result; // the return string
    char *ins;    // the next insert point
    char *tmp;    // varies
    int len_rep;  // length of rep (the string to remove)
    int len_with; // length of with (the string to replace rep with)
    int len_front; // distance between rep and end of last rep
    int count;    // number of replacements

    // sanity checks and initialization
    if (!orig || !rep)
        return NULL;
    len_rep = strlen(rep);
    if (len_rep == 0)
        return NULL; // empty rep causes infinite loop during count
    if (!with)
        with = "";
    len_with = strlen(with);

    // count the number of replacements needed
    ins = orig;
    for (count = 0; (tmp = strstr(ins, rep)); ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    // first time through the loop, all the variable are set correctly
    // from here on,
    //    tmp points to the end of the result string
    //    ins points to the next occurrence of rep in orig
    //    orig points to the remainder of orig after "end of rep"
    while (count--) {
        ins = strstr(orig, rep);
        len_front = ins - orig;
        tmp = strncpy(tmp, orig, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        orig += len_front + len_rep; // move to next "end of rep"
    }
    strcpy(tmp, orig);
    return result;
}

// Replaces all instances of rep with with
char *strrepall(char *orig, char *rep, char *with) {
    char *new = "";
    char *result = orig;
    bool new_is_stack = true;
    while (strcmp(new, result) != 0) {
	if (!new_is_stack && new != orig)
		free(new); // keeps strrep from leaking memory
	new_is_stack = false;
        new = result;
        result = strrep(result, rep, with);
    }
    char *heapresult = strdup(result);
    if (!new_is_stack && new != orig)
	    free(new);
    if (result != orig)
	    free(result);
    return heapresult;
}

