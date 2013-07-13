#include <stdlib.h>
#include <stdio.h>

int test_known_values( void )
{
size_t i;
int retn;

retn = 0;

#define NUM_TESTS 5
static const int known_values[NUM_TESTS]=
	{
	1335961580,
	2004700132,
	1977926730,
	1908820283,
	1607533783,
	};

srand(0);

for( i = 0; i < NUM_TESTS; ++i )
	{
	int compare;
	compare = rand();
	if( known_values[i] != compare )
		{
		printf(
			"Failure: at state %i. rand() returned %i but expected %i\n",
			i,
			compare,
			known_values[i]
			);
		retn++;
		}
	}
#undef NUM_TESTS
return retn;
}

int main()
{
int failures;
int total_failures = 0;

failures = test_known_values();
total_failures += failures;
printf("Known values test failures:%i\n", failures);

return total_failures;
}
