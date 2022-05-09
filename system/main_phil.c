/*  main.c  - main */


#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5	//number of philosophers and forks

//TODO - locks must be declared and initialized here
lid32	lock;
lid32	fork[N]; //= {FALSE, FALSE, FALSE, FALSE, FALSE};


/*
 * Delay for a random amount of time
 * @param alpha delay factor
 */
void	holdup(int32 alpha)
{
	long delay = rand() * alpha;
	while (delay-- > 0)
		;	//no op
}

/**
 * Eat for a random amount of time
 */
void	eat()
{
	holdup(10000);
}

/**
 * Think for a random amount of time
 */
void	think()
{
	holdup(1000);
}

/**
 * Philosopher's code
 * @param phil_id philosopher's id
 */
void	philosopher(uint32 phil_id)
{
	uint32 right = phil_id;				//right fork
	uint32 left = N - ((N-phil_id) % N) - 1;	//left fork

	while (TRUE)
	{
		//think 70% of the time
		if ((rand()*1.0)/RAND_MAX < 0.7)
		{
			acquire(lock);
			kprintf("Philosopher %d thinking: zzzzzZZZz\n", phil_id);
			release(lock);

			think();
		}
		else	//eat 30% of the time
		{
			acquire(fork[right]);	//grab the right fork (or wait)
			if (locktab[fork[left]].lock = FALSE)
			{
				acquire(fork[left]);	//grab the left fork

				acquire(lock);
				kprintf("Philosopher %d eating: nom nom nom\n", phil_id);
				release(lock);

				eat();

				release(fork[left]);
				release(fork[right]);
			}
			else
			{
				release(fork[right]);	//relinquish right fork
			}
		}
	}
}

int	main(uint32 argc, uint32 *argv)
{
    //initialize locks
    lock = lock_create();
    int32 i;
    for (i = 0; i < N; i++)
    {
        fork[i] = lock_create();
    }

	ready(create((void*) philosopher, INITSTK, 15, "Ph1", 1, 0), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph2", 1, 1), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph3", 1, 2), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph4", 1, 3), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph5", 1, 4), FALSE);

	return 0;
}