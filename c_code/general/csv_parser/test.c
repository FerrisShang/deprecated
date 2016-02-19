#include <stdio.h>
#include "csv.h"

int main(void)
{
  const char *test = ",,41,42,43,44,45,46,,48,48,47,,a,x,\",\",\"\"\",,\"";
  char **parsed = parse_csv( test );
  char **ptr;
  int i = 1;

  for ( ptr = parsed; *ptr; ptr++ )
    printf( "Field %d: %s\n", i++, *ptr );

  free_csv_line( parsed );

  return 1;
}
