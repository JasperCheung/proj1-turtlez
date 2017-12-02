# proj1-turtlez by Jasper Cheung Jeffery Luo

## Features:
-	Forks and executes commands!
-	Parses multiple commands on one line!
-	Redirects using >!


## Attempted:
-	The following did not end up working, but have been left in the code, commented out
-	pipes 
-   not implemented, >>.
-	Looked at replacing ~/ with home directory, got seg faults

## Bugs:
	
	
## Files & Function Headers:
### parse.c
Handles all line parsing fucntions

    /*======== char **parse_input() =========
 	  Inputs:  char *line
             char* delimiter 
	  Returns: Array of strings seperated by delimiter      
    ====================*/
    
    /*======== char *trim_string() =========
    Inputs: char *s
            int size
    Returns: String with size, size
    
    Leaves only size characters, not including terminating null
     ====================*/
     
    /*======== char *trim_trailing() ==========
    Inputs: char *s 
            char c 
    Returns: String without charc in the front or end
    
    Trims characters c from beginning and end of string 
    ====================*/
    
    /*======== int count_occur() ==========
    Inputs: char *s
            char *occ
            
    Returns: Int of the number of times occ appear in string s
    
    Counts occurrences of substring occ in s
    ====================*/

### main.c
Handles the forking an executing of commands...

