### **TSV Data Processor: Sorting and Tabulation Tool**
_**Project Overview**_
This project implements a TSV (Tab-Separated Values) data processor that performs sorting, tabulation, and data analysis on structured input files. It reads TSV files, processes rows and columns, sorts based on user-specified criteria, and outputs formatted results. The project is divided into three main stages:

**Stage 1:** Reads TSV input and prints a summary of the data.

**Stage 2:** Sorts the data based on specified columns using Insertion Sort and outputs the first, middle, and last rows.

**Stage 3:** Tabulates and counts entries for grouped column values.

_**Features**_
Flexible Sorting: Allows sorting by multiple columns provided as command-line arguments.
Insertion Sort Implementation: Efficiently sorts the rows based on lexicographical order of fields.
Readable Output:
Summarizes TSV data with clear labels.
Displays key rows (first, middle, last) after sorting.
Groups and tabulates data with counts.
Robust Input Handling: Ensures proper handling of tabs, newlines, and EOF for TSV files.
 
_**Code Structure**_
The project is structured as follows:

**main():** Controls program flow across all stages.
**getfield():** Reads a single TSV field and terminates at \t, \n, or EOF.
**mygetchar():** Custom getchar implementation that bypasses carriage return (\r).
**print_stage1():** Outputs the summary of input TSV data (headers and rows).
**print_stage2_output():** Displays first, middle, and last rows after sorting.
**compare_rows():** Compares two rows based on specified sort columns.
**insertion_sort():** Sorts rows using Insertion Sort based on user-specified columns.
**print_stage3_header():** Prints a formatted header for tabulated output.
**tabulate_data():** Groups rows by column values and counts occurrences.

**How to Run**
**1. Compile the Code:**
   example:
   gcc -o tsv_processor tsv_processor.c

**2. Run the Program:**
    Provide the column numbers to sort by as command-line arguments.
    Example:
    ./tsv_processor 1 2

**3. Input:**
   Provide TSV data as input via standard input (stdin). Example input:
   Country	Region	Population	Density	Capital
   USA	North America	331002651	36.0	Washington DC
   India	Asia	1380004385	464.0	New Delhi
  Australia	Oceania	25499884	3.2	Canberra

**4. Output:**
   The program will display results for all three stages:

        **Stage 1:** Input summary
        **Stage 2:** Sorted rows
        **Stage 3:** Tabulated counts

### Sample Output

**Stage 1**
  Stage 1
  input tsv data has 3 rows and 5 columns
  row 3 is:
     1: Country     Australia
     2: Region      Oceania
     3: Population  25499884
     4: Density     3.2
     5: Capital     Canberra

**Stage 2**
  Stage 2
  sorting by "Country"

  row 1 is:
     1: Country     Australia
     2: Region      Oceania
    
  row 2 is:
     1: Country     India
     2: Region      Asia
   
  row 3 is:
     1: Country     USA
     2: Region      North America
    
**Stage 3**
  Stage 3
  - - - - - - - - - - - 
  Country
      Region     Count
  - - - - - - - - - - -
  Australia
      Oceania        1
  India
      Asia           1
  USA
     North America  1
  - - - - - - - - - - -  
**Future Improvements**
    Support dynamic column numbers for larger TSV files.
    Add support for CSV (Comma-Separated Values).
    Optimize sorting with more efficient algorithms (e.g., QuickSort).
    Allow file input instead of only standard input.
