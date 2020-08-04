#include <stdio.h>
#include <gmp.h>

char *prog;

typedef struct elliptic_curve_t
{
	mpz_t A,B,p;
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

void generate_random_seed(mpz_t seed){
	__uint8_t input[20];
	FILE *fp;
	if((fp = fopen("/dev/urandom","r")) == NULL){
		fprintf(stderr, "%s: can't read /dev/urandom \n", prog);
	}else{
		for(int i = 0; i < 19; i++){
			input[i] = getc(fp);
		}
		mpz_import(seed, sizeof(input), 1, sizeof(input[0]), 0, 0, input);
	}
}

void generate_random_number(mpz_t x, int num_bits){
	gmp_randstate_t state;
	gmp_randinit_default(state);

	mpz_t seed;
	mpz_init(seed);

	generate_random_seed(seed);
	gmp_randseed(state, seed);

	mpz_urandomb(x, state , num_bits);
	gmp_randclear(state);
}


int elliptic_curve_random_generate(elliptic_curve* e, int n)
{
	// Randomly choose a prime p
	mpz_t p;
	mpz_init(p);

	int val = 0;

	// while(val != 2){
	// 	generate_random_number(p, n);
	// 	val = mpz_probab_prime_p(p, 40);
	// }
	
	generate_random_number(p, n);

	int cnt = 0;
	while(1){
		val = mpz_probab_prime_p(p, 40);
		if(val == 2){
			break;
		}
		if(mpz_even_p(p)){
			mpz_add_ui(p, p, 1);
			//printf("1\n");
		}else{
			mpz_add_ui(p, p, 2);
			//printf("2\n");
		}
	}

	mpz_out_str(stdout,10,p);
	printf("\n");

	mpz_t A, B;
	mpz_init(A);
	mpz_init(B);
	do{
		// Randomly choose A
		generate_random_number(A, n);
		// Randomly choose B
		generate_random_number(B, n);
		mpz_set(e->A, A);
		mpz_set(e->B, B);
		mpz_set(e->p, p);
	}while(is_Singular(*e));

	return 0;
}

point elliptic_curve_operation(point p1, point p2)
{
	point result;
	point_init(&result);
	return result;
}

//gcc crypto.c -lgmp
int main(int argc, char *argv[])
{
	prog = argv[0];
	elliptic_curve e1,e2;
	int num_bits;
	printf("Enter number of bits: ");
	scanf("%d", &num_bits);

	elliptic_curve_init(&e1);
	elliptic_curve_init(&e2);

	elliptic_curve_random_generate(&e1, num_bits);
	return 0;
}
