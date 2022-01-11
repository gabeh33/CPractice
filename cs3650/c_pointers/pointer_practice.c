//Question 1 Part A
#include <stdio.h>
int main()
{
    char x1[100] = "The quick brown fox jumped over the lazy dog.\n";
    int i = 0;
    int lines, chars = 0;
    int words = 1;
    while (x1[i] != '\0') {
        if (x1[i] == ' ') {
            words++;
        } else if (x1[i, i+1] == '\n') {
            lines++;
        }else {
            chars++;
        }
        i++;
    }
    printf("Words:%d, Lines:%d, Characters:%d",words, lines, chars);
    
    return 0;
}

//Question 1 Part B
#include <stdio.h>
int main()
{
    char x1[100] = "The quick brown fox jumped over the lazy dog.\n";
    int i = 0;
    int lines, chars = 0; 
    int words = 1;
    char *x1ptr = x1;
    while (*(x1ptr + i) != '\0') {
        if (*(x1ptr + i) == ' ') {
            words++;
        } else if (*(x1ptr + i) == '\n') {
            lines++;
        }else {
            chars++;
        }
        i++;
    }
    printf("Words:%d, Lines:%d, Characters:%d",words, lines, chars);
    
    return 0;
}

//Question 1 part C
#include <stdio.h>
int main()
{
    char x1[100] = "The quick brown fox jumped over the lazy dog.\n";
    int i = 0;
    int lines, chars = 0; 
    int words = 1;
    char *x1ptr = x1;
    while (*x1ptr != '\0') {
        if (*x1ptr == ' ') {
            words++;
        } else if (*x1ptr == '\n') {
            lines++;
        }else {
            chars++;
        }
        *x1ptr++;
    }
    printf("Words:%d, Lines:%d, Characters:%d",words, lines, chars);
    
    return 0;
}



//Question 2 Part A
#include <stdio.h>

/* count lines, words, and characters in input*/

int main()
{
    int c, i, nwhite, nother;
    int ndigit[10];
    nwhite = nother = 0;
    for (i = 0; i < 10; ++i)
        ndigit[i] = 0;
     char x1[100]
    = "The 25 quick brown foxes jumped over the 27 lazy dogs 17 times.";
    i = 0;
    while (x1[i] != '\0') {
        if (x1[i] >= '0' && x1[i] <= '9')
            ++ndigit[x1[i]-'0'];
        else if (x1[i] == ' ' || x1[i] == '\n' || x1[i] == '\t')
            ++nwhite;
        else
            ++nother;
        i++;
    }
    printf("digits =");
    for (i = 0; i < 10; ++i)
        printf(" %d", ndigit[i]);
    printf(", white space = %d, other = %d\n",
        nwhite, nother);
    return 0;
}

//Question 2 Part B
#include <stdio.h>

/* count lines, words, and characters in input*/

int main()
{
    int c, i, nwhite, nother;
    int ndigit[10];
    nwhite = nother = 0;
    for (i = 0; i < 10; ++i)
        ndigit[i] = 0;
     char x1[100]
    = "The 25 quick brown foxes jumped over the 27 lazy dogs 17 times.";
    char *x1ptr = x1;
    char *ndigitptr = ndigit;
    i = 0;
    while (*(x1ptr + i) != '\0') {
        if (*(x1ptr + i) >= '0' && *(x1ptr + i) <= '9')
            ndigit[*(x1ptr + i)-'0']++;
        else if (*(x1ptr + i) == ' ' || *(x1ptr + i) == '\n' || *(x1ptr + i) == '\t')
            ++nwhite;
        else
            ++nother;
        i++;
    }
    printf("digits =");
    for (i = 0; i < 10; ++i)
        printf(" %d", ndigit[i]);
    printf(", white space = %d, other = %d\n",
        nwhite, nother);
    return 0;
}

//Question 2 Part C
#include <stdio.h>

/* count lines, words, and characters in input*/

int main()
{
    int c, i, nwhite, nother;
    int ndigit[10];
    nwhite = nother = 0;
    for (i = 0; i < 10; ++i)
        ndigit[i] = 0;
     char x1[100]
    = "The 25 quick brown foxes jumped over the 27 lazy dogs 17 times.";
    char *x1ptr = x1;
    char *ndigitptr = ndigit;
    
    while (*x1ptr != '\0') {
        if (*x1ptr >= '0' && *x1ptr <= '9') {
            ndigit[*x1ptr-'0']++;
        }
        else if (*x1ptr == ' ' || *x1ptr == '\n' || *x1ptr == '\t')
            ++nwhite;
        else
            ++nother;
        *x1ptr++;
    }
    printf("digits =");
    for (i = 0; i < 10; ++i)
        printf(" %d", ndigit[i]);
    printf(", white space = %d, other = %d\n",
        nwhite, nother);
    return 0;
}