// The almighty stack-overflow gods blessed me with: https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
#pragma once

// @desc	Replaces the first instance of `rep` with `with` in `orig`. Does not modify any of its inputs! Must be freed.
// @arg(orig)	Text to find-and-replace in (haystack)
// @arg(rep)	Text to find in `orig` (needle)
// @arg(with)	Text to replace `rep` with
// @returns	A new malloced string with the first instance of `rep` replaced with `with`
char *strrep(char *orig, char *rep, char *with);
// @desc	Replaces all instances of `rep` with `with`. Does not modify any of its inputs, must be freed.
// @arg(orig)	Text to find-and-replace in (haystack)
// @arg(rep)	Text to find in `orig` (needle)
// @arg(with)	Text to replace `rep` with
// @returns	A new malloced string with all instances of `rep` replaced with `with`
char *strrepall(char *orig, char *rep, char *with);
