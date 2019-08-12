# An extended wc version
**NAME**  
    wordCounter - count and print the number of word/characters in a sentence or number of characters in the longest word.
**SYNOPSIS**  
    wordCounter[OPTION]…  
**DESCRIPTION**  
    wordCounter receives text characters from standard input and prints the number of words/characters or number of characters in the longest word to the standard output, according to the options specified by the user.  
**OPTIONS**  
-w  
    number of words. (If there is no option this is the default option)-c  
    number of characters.  
-l  
    number of characters in the longest word  
**EXAMPLES**  
> #> wordCounter -w  
Hi, My name is Noam.  
^D  
5  
#>wordCounter -c  
Hi, My name  
^D  
9  
#>wordCounter -l  
Hi, My name  
^D  
4  
#>wordCounter -c -l  
Hi, My name  
^D  
9  
4  
