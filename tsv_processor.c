
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#define CHAR_CR '\r'         /* CR character, in PC-format text files */
#define CHAR_NL '\n'         /* newline character, ends each input line*/
#define CHAR_TB '\t'         /* tab character, ends each TSV field*/

#define STATUS_EOL 1        /* return value for end of line field*/
#define STATUS_EOF 2        /* return value for end of file field*/
#define STATUS_NML 3        /* return value if neither of previous two*/

#define MAXFIELDLEN 50      /* maximum field length for each field*/
#define MAXCOLS 5           /* we expect exactly 5 columns*/
#define MAXROWS 1000        /* maximum number of rows in the TSV file*/

/* one tsv field, stored within a fixed-length character string */
typedef char field_t[MAXFIELDLEN + 1]; 

/**************************************************************/

int mygetchar(void);
int getfield(field_t dest);
void print_stage1(field_t data[][MAXCOLS], int nrows, field_t headers[]);
void print_stage2_output(field_t data[][MAXCOLS], int nrows, field_t headers[], int *sort_columns, int num_columns);
void insertion_sort(field_t data[][MAXCOLS], int nrows, int *sort_columns, int num_columns);
int compare_rows(field_t row1[], field_t row2[], int *sort_columns, int num_columns);
void print_stage3_header(int *sort_columns, int num_columns, field_t headers[]);
void tabulate_data(field_t data[][MAXCOLS], int nrows, int *sort_columns, int num_columns);

/**************************************************************/
int main(int argc, char *argv[]) {
    field_t headers[MAXCOLS];        /*array to store the header row*/
    field_t data[MAXROWS][MAXCOLS];  /* 2D array to store the TSV data*/
    int sort_columns[MAXCOLS];       /* array to store sort column indices*/
    int num_columns = argc - 1;      /* number of sort columns provided*/
    int nrows = 0;                   /* row counter*/
    int status, nfields;

    /* If no sort columns are provided, exit after Stage 1 */
    if (argc == 1) {
        printf("No column numbers provided. Exiting after Stage 1.\n");
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        sort_columns[i - 1] = atoi(argv[i]) - 1;  
        if (sort_columns[i - 1] < 0 || sort_columns[i - 1] >= MAXCOLS) {
            printf("Invalid column. Please specify a column between 1 and %d.\n", MAXCOLS);
            return 1;
        }
    }

    /* read the header row*/
    for (int i = 0; i < MAXCOLS; i++) {
        getfield(headers[i]);
    }

    /*read the data rows*/
    while (nrows < MAXROWS) {
        nfields = 0;
        while (nfields < MAXCOLS) {
            status = getfield(data[nrows][nfields]);
            nfields++;
            if (status == STATUS_EOL || status == STATUS_EOF) {
                break;
            }
        }
        if (nfields == MAXCOLS) {
            nrows++;
        }
        if (status == STATUS_EOF) {
            break;
        }
    }

    /* print Stage 1 output*/
    print_stage1(data, nrows, headers);

    /* sort the data using insertion sort (Stage 2)*/
    insertion_sort(data, nrows, sort_columns, num_columns);

    /* print Stage 2 output: first, middle, and last rows after sorting*/
    print_stage2_output(data, nrows, headers, sort_columns, num_columns);

    /* print Stage 3 header*/
    print_stage3_header(sort_columns, num_columns, headers);

    /* proceed to Stage 3: Tabulation*/
    tabulate_data(data, nrows, sort_columns, num_columns);

    return 0;
}

/**************************************************************/

/* read characters and build a string, stopping when a tab or newline
   as encountered, with the return value indicating what that
   terminating character was
*/
int getfield(field_t dest) {
    int ch, nchs = 0;
    int status = STATUS_NML; 

    dest[0] = '\0'; 
    while ((ch = mygetchar()) && (ch != CHAR_TB) && (ch != CHAR_NL) && (ch != EOF)) {
        if (nchs < MAXFIELDLEN) {
		   /* ok to save this character */
            dest[nchs++] = ch;
            dest[nchs] = '\0';  
        }
    }
    if (ch == EOF) {
        status = STATUS_EOF;
    } else if (ch == CHAR_NL) {
        status = STATUS_EOL;
    } else if (ch == CHAR_TB) {
        status = STATUS_NML;
    }

    return status;
}

/**************************************************************/

/* read a single character, bypassing any CR characters encountered,
   so as to work correctly with either PC-type or Unix-type input
 */
int mygetchar(void) {
    int c;
    while ((c = getchar()) == CHAR_CR) {
       /* empty loop body */
    }
    return c;  
}

/* function to print Stage 1 output*/
void print_stage1(field_t data[][MAXCOLS], int nrows, field_t headers[]) {
    printf("Stage 1\n");
    printf("input tsv data has %d rows and %d columns\n", nrows, MAXCOLS);
    printf("row %d is:\n", nrows);
    for (int i = 0; i < MAXCOLS; i++) {
        printf("   %d: %-10s %s\n", i + 1, headers[i], data[nrows - 1][i]);
    }
}

/*function to print the first, middle, and last rows after sorting */
void print_stage2_output(field_t data[][MAXCOLS], int nrows, field_t headers[], int *sort_columns, int num_columns) {
    printf("\nStage 2\n");
    printf("sorting by \"%s\"", headers[sort_columns[0]]);
    for (int i = 1; i < num_columns; i++) {
        printf(",\n   then by \"%s\"", headers[sort_columns[i]]);
    }
    printf("\n");

    int middle = (nrows + 1) / 2 - 1;  /*middle row index (⌈r/2⌉, 0-based index)*/

    printf("row 1 is:\n");
    for (int j = 0; j < MAXCOLS; j++) {
        printf("   %d: %-10s %s\n", j + 1, headers[j], data[0][j]);  
    }

    printf("row %d is:\n", middle + 1);  
    for (int j = 0; j < MAXCOLS; j++) {
        printf("   %d: %-10s %s\n", j + 1, headers[j], data[middle][j]);  
    }

    printf("row %d is:\n", nrows);
    for (int j = 0; j < MAXCOLS; j++) {
        printf("   %d: %-10s %s\n", j + 1, headers[j], data[nrows - 1][j]); 
    }
}

/* function to compare two rows based on the sort columns*/
int compare_rows(field_t row1[], field_t row2[], int *sort_columns, int num_columns) {
    for (int i = 0; i < num_columns; i++) {
        int col = sort_columns[i];
        int cmp = strcmp(row1[col], row2[col]);
        if (cmp != 0) {
            return cmp;
        }
    }
    return 0; 
}

/* function to sort the data using insertion sort based on the sort columns*/
void insertion_sort(field_t data[][MAXCOLS], int nrows, int *sort_columns, int num_columns) {
    for (int i = 1; i < nrows; i++) {
        field_t temp[MAXCOLS];
        memcpy(temp, data[i], sizeof(temp));  
        int j = i - 1;
        while (j >= 0 && compare_rows(data[j], temp, sort_columns, num_columns) > 0) {
            memcpy(data[j + 1], data[j], sizeof(temp));
            j--;
        }
        memcpy(data[j + 1], temp, sizeof(temp));  
    }
}

/* function to print the Stage 3 header based on the selected sort columns*/
void print_stage3_header(int *sort_columns, int num_columns, field_t headers[]) {
    printf("\nStage 3\n");
    printf("------------------\n");
    for (int i = 0; i < num_columns; i++) {
        printf("%s\n", headers[sort_columns[i]]); 
        if (i < num_columns - 1) {
            printf("    ");  
        }
    }
    printf("Count\n");  
    printf("------------------\n");
}

/*function to tabulate and print data, with proper alignment for the Count column*/
void tabulate_data(field_t data[][MAXCOLS], int nrows, int *sort_columns, int num_columns) {
    int longest_entry = 0;

    /*find the longest entry for column width*/
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < num_columns; j++) {
            int len = strlen(data[i][sort_columns[j]]);
            if (len > longest_entry) {
                longest_entry = len;
            }
        }
    }

    /* print the data with aligned Count column*/
    int count = 1;
    int first_country = 1;  
    for (int i = 1; i < nrows; i++) {
        if (strcmp(data[i][sort_columns[0]], data[i - 1][sort_columns[0]]) == 0 &&
            strcmp(data[i][sort_columns[1]], data[i - 1][sort_columns[1]]) == 0) {
            count++;  
        } else {
            if (!first_country) {
                printf("\n");  
            }

            if (strcmp(data[i - 1][sort_columns[0]], data[i][sort_columns[0]]) != 0 || first_country) {
                printf("%-*s\n", longest_entry + 4, data[i - 1][sort_columns[0]]); 
            }

            printf("    %-*s %5d\n", longest_entry, data[i - 1][sort_columns[1]], count); 
            count = 1;  
            first_country = 0; 
        }
    }

    printf("%-*s\n", longest_entry + 4, data[nrows - 1][sort_columns[0]]);  
    printf("    %-*s %5d\n", longest_entry, data[nrows - 1][sort_columns[1]], count); 

    printf("------------------\n");
    printf("ta daa!");
}
