#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

/* P and Q are similar to RSA Keys*/
#define P 62467ul
#define Q 62459ul

/* M is P*Q, which is the local RAND_MAX */
#define M ((P)*(Q))

/* S is the initial seed. Needs to be (coprime to P,Q)^2 < M */
#define S 3770328409ul

static uint32_t seed = S;

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
	 *per iteration. Or someone could pick better P and Q values
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
