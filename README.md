# proj1-turtlez by Jasper Cheung Jeffery Luo

## Features:
-	Forks and executes commands!
-	Parses multiple commands on one line using ';'!
-	Stdin & stdout redirection allowed through < and >


## Attempted:
-	The following did not end up working, but have been left in the code, commented out
-	pipes 
-   not implemented, >>.
-	Looked at replacing ~/ with home directory, got seg faults

## Bugs:
-	Exiting terminal by typing "exit" may not work consistently

## Files & Function Headers:

### parse.c
Handles all line parsing fucntions

```

/*======== char **parse_input() =========
Inputs:  char *line
          char* delimiter

Returns: Array of char pointers, with each char pointer being a token
of line separated by string delimiter. Separates string line by the 
string delimiter.
====================*/

/*======== char *trim_string() =========
Inputs: char *s
int size
Returns: String representing first size characters of string s, trimming out
garbage values.

Returns the first size characters of string s. Terminating NULL is automatically
added to end of the returned string.
====================*/

/*======== char *trim_trailing() ==========
Inputs: char *s 
char c 
Returns: String without char c's in the front and end. 

Trims characters c from beginning and end of string s. Stops in one direction when
a different character is encountered. Returned string does contain a terminating NULL.
====================*/

/*======== int count_occur() ==========
Inputs: char *s
char *occ

Returns: An integer representing the number of times occ appear in string s

Counts occurrences of substring occ in string s. The number of tokens can be
found by adding 1 onto the return value.
====================*/
```

### main.c
Handles the forking and executing of commands...
