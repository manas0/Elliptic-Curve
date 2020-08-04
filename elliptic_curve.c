#include <stdio.h>
#include <gmp.h>

typedef struct elliptic_curve_t
{
	mpz_t A,B, p;
} elliptic_curve;

typedef struct point_t
{
	mpz_t x,y;
} point;


void elliptic_curve_init(elliptic_curve* e)
{
	mpz_init(e->A);
	mpz_init(e->B);
	mpz_init(e->p);
}

void point_init(point* p)
{
	mpz_init(p->x);
	mpz_init(p->y);
}

int is_Singular(elliptic_curve e)
{
	// Check condition
	mpz_t a3, b2; 
	mpz_init(a3);
	mpz_init(b2);

	mpz_powm_ui(a3 , e.A , 3, e.p);
	mpz_powm_ui(b2 , e.B , 2 , e.p);

	mpz_mul_ui(a3 , a3 , 4);
	mpz_addmul_ui(a3 , b2 , 27);

	mpz_mod (a3 , a3 , e.p);
	
	if(mpz_sgn(a3))
		return 0;

	return 1;
}

int elliptic_curve_random_generate(elliptic_curve* e, int n)
{
	// Randomly choose a prime p

	do{
		// Randomly choose A

		// Randomly choose B

	}while(is_Singular(*e));

	return 0;
}

point elliptic_curve_operation(point p1, point p2)
{
	point result;
	point_init(&result);

	return result;
}



int main()
{
	elliptic_curve e1,e2;
	int num_bits;
	printf("Enter number of bits: ");
	scanf("%d", &num_bits);

	elliptic_curve_init(&e1);
	elliptic_curve_init(&e2);
	//elliptic_curve_random_generate(&e1, num_bits);

	mpz_t x,y,z, seed;
	mpz_init(x);
	mpz_init(y);
	mpz_init(z);


	gmp_randstate_t state;
	gmp_randinit_default(state);
	gmp_randseed(state, seed);

	mpz_urandomb(x, state , num_bits);
	mpz_urandomb(y, state , num_bits);
	mpz_urandomb(z, state , num_bits);
	
	mpz_out_str(stdout,10,x);
	
	printf("\n");
	mpz_out_str(stdout,10,y);
	printf("\n");
	mpz_out_str(stdout,10,z);
	printf("\n");

	mpz_set(e2.A , x);
	mpz_set(e2.B , z);
	mpz_set_str(e2.p , "5" , 10);
	gmp_randclear(state);

	//printf("\n%d\n",is_Singular(e2));

	return 0;
}
