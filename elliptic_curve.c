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

	mpz_mod(a3 , a3 , e.p);
	
	if(mpz_sgn(a3))
		return 0;

	return 1;
}

void generate_random_seed(mpz_t seed)
{
	__uint8_t input[20];
	FILE *fp;
	if((fp = fopen("/dev/urandom","r")) == NULL)
	{
		fprintf(stderr, "%s: can't read /dev/urandom \n", prog);
	}
	else
	{
		for(int i = 0; i < 19; i++)
		{
			input[i] = getc(fp);
		}
		mpz_import(seed, sizeof(input), 1, sizeof(input[0]), 0, 0, input);
	}

}

void generate_random_number(mpz_t x, int num_bits)
{
	gmp_randstate_t state;
	gmp_randinit_default(state);

	mpz_t seed;
	mpz_init(seed);

	generate_random_seed(seed);
	gmp_randseed(state, seed);

	mpz_urandomb(x, state , num_bits);
	gmp_randclear(state);

}

void elliptic_curve_random_generate(elliptic_curve* e, int n) // n = number of bits
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
		if(val == 1){
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
	mpz_set(e->p, p);
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
		
	}while(is_Singular(*e));

	return ;
}

void slope(elliptic_curve e, point p1, point p2 , mpz_t lambda)
{
	mpz_t temp;
	mpz_init(temp);

	if(mpz_cmp(p1.x , p2.x) == 0 && mpz_cmp(p1.y , p2.y) == 0)
	{
		mpz_powm_ui(lambda , p1.x , 2 , e.p);
		mpz_mul_ui(lambda , lambda , 3);
		mpz_add(lambda , lambda , e.A);
		mpz_mod(lambda , lambda , e.p);
		mpz_mul_ui(temp , p1.y , 2);
		mpz_mod(temp , temp , e.p);
		mpz_invert(temp , temp , e.p);
		mpz_mul(lambda , lambda , temp);
		mpz_mod(lambda , lambda , e.p);

		return ;	
	}


	mpz_sub(temp , p2.x , p1.x);
	mpz_mod(temp , temp , e.p);
	mpz_invert(lambda, temp , e.p);
		
	mpz_sub(temp , p2.y , p1.y);
	mpz_mod(temp , temp , e.p);

	mpz_mul(lambda , lambda , temp);
	mpz_mod(lambda , lambda , e.p);

	return ;
	
}

point elliptic_curve_operation(elliptic_curve e, point p1, point p2) // operation = addition 
{
	point result;
	point_init(&result);
	
	mpz_mod(p1.x , p1.x , e.p);
	mpz_mod(p1.y , p1.y , e.p);
	mpz_mod(p2.x , p2.x , e.p);
	mpz_mod(p2.y , p2.y , e.p);

	mpz_t temp;
	mpz_init(temp);
	mpz_add(temp , p1.y , p2.y);
	mpz_mod(temp ,temp , e.p);

	if(mpz_cmp(p1.x , p2.x) == 0 && mpz_sgn(temp) == 0)
	{
		mpz_set_str(result.x , "0", 10);
		mpz_set_str(result.y , "0", 10);
		return result;
	}

	mpz_t lambda, lambda_squared;

	mpz_init(lambda);
	mpz_init(lambda_squared);

	slope(e, p1, p2 , lambda);
	mpz_powm_ui(lambda_squared , lambda, 2 , e.p);

	mpz_sub(result.x , lambda_squared , p1.x);
	mpz_sub(result.x , result.x , p2.x);
	mpz_mod(result.x , result.x , e.p);

	mpz_sub(result.y , p1.x, result.x);
	mpz_mul(result.y , result.y , lambda);
	mpz_sub(result.y , result.y , p1.y);
	mpz_mod(result.y , result.y , e.p);

	return result;
}



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

	point p1, p2, p3;
	point_init(&p1);
	point_init(&p2);
	point_init(&p3);

	mpz_set_str(e2.A , "1" , 10);
	mpz_set_str(e2.B , "6" , 10);
	mpz_set_str(e2.p , "11" , 10);

	mpz_set_str(p1.x , "2" , 10);
	mpz_set_str(p1.y , "7" , 10);

	mpz_set_str(p2.x , "2" , 10);
	mpz_set_str(p2.y , "7" , 10);
	
	printf("\n");

	for(int i = 0 ; i < 12 ; i++)
	{
		printf("Point %d :\n" , i+1);
		p3 = elliptic_curve_operation(e2 , p1 , p2);
		mpz_out_str(stdout,10,p3.x);
		printf("\n");
		mpz_out_str(stdout,10,p3.y);
		printf("\n\n");

		p2 = p3;
	}

	return 0;
}