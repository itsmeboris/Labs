# Supporting input from a file  
**NAME**  
    wordCounter - count and print the number of word/characters in a sentence or number of characters in the longest word or ​number of lines.  
**SYNOPSIS**  
    wordCounter[OPTION]…  
**DESCRIPTION**  
   wordCounter receives text characters from standard input or from a file and prints the number of words/characters or number of characters in the longest word or number of lines in the file to the standard output, by options.  
**OPTIONS**  
-w  
    number of words. (If there is no option this is the default option)  
-c  
    number of characters.  
-l  
    number of characters in the longest word  
-n  
    number of lines in file OR in stdin.  
-i FILE  
    Input file. Read list of characters to be encoded from a file, instead of from standard input.  
**EXAMPLES**  
> #>cat input.txt  
My name  
Is Noam  
I study CS  
#> wordCounter -w -i input.txt  
7  
#> wordCounter -n -i input.txt  
3  
#>  
