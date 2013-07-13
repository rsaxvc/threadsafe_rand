#include <assert.h>

#include <stdint.h>
#include <stdlib.h>

//#define DEBUG 1

#ifndef DEBUG
	/* P and Q are similar to RSA Keys*/
	#define P 62467ul
	#define Q 62459ul

	/* M is P*Q, which is the local RAND_MAX */
	#define M ((P)*(Q))

	/* S is the initial seed. Needs to be (coprime to P,Q)^2 < M */
	#define S 3770328409ul

	/* C is the Carmichael value of M */
	#define C 1950750714ul
#else
	/* Example Wikipedia Values*/
	#define P 11
	#define Q 19
	#define M ((P)*(Q))
	#define S 81
	#define C 90
#endif
static uint32_t seed = S;

/*
 *Given the current state, compute the next state,
 *but skip any states outside of RAND_MAX.
 */
static inline uint32_t bbs_rand( uint32_t crnt )
{
do
	{
	/*
	 *Compute one iteration of BBS
	 */
	crnt = ( (uint64_t)crnt * crnt ) % M;

	/*
	 *Discard any states that are past RAND_MAX.
	 *With RAND_MAX=2^31-1, there's about a 40% chance of this
	 *per iteration. I should pick better P and Q values to
	 *decrease the chance of repeating the calculation.
	 */
	} while( crnt >= RAND_MAX );
return crnt;
}

/*
 *Threadsafe implementation of Blum Blum Shub rand()
 *
 *Threadsafety:
 *	Generates a different random number per caller
 *	Doesn't crash if reentered ( unlike a certain embedded vendor clib)
 *	srand() only works when single-threaded - otherwise another thread may intrude
 */
int rand( void )
{
uint32_t oldval;
uint32_t newval;
uint32_t syncval;

syncval = seed;
do
	{
	oldval = syncval;
	newval = bbs_rand( oldval );
	syncval = __sync_val_compare_and_swap( &seed, oldval, newval );
	}while( oldval != syncval );

return newval;
}

/*
stackoverflow.com/questions/101439/the-most-efficient-way-to-implement-an-integer-based-power-function-powint-int
static inline unsigned long long upow( unsigned long long base, unsigned long long exp )
{
unsigned long long result = 1;
while (exp)
	{
	if (exp & 1)
		result *= base;
	exp >>= 1;
	base *= base;
	}
return result;
}
*/

/*
 *Set PRNG seed. Currently only supports resetting seed.
 */
void srand( unsigned int seed_in )
{
assert( seed_in == 0 );
seed = S;

/*
 *By using the Carmichael function, we can directly
 *compute the Nth value in the sequence, and still
 *keep our long cycle length obtained by picking
 *good a good initial value. C is precalulated above.
 *
 *seed = upow( S, upow( 2, seed_in ) % C ) % M;
 *
 *If this result is larger than RAND_MAX, we don't
 *need to fix it, as the next call to rand() will.
 *
 *However, there is overflow in the above statement
 *that requires a significantly larger arithmetic
 *type than is available in 'C' when raising S to
 *high powers. This could be fixed with an arbitrary
 *length arithmetic library and without breaking
 *the stdc API.
 */
}
