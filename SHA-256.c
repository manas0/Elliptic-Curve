/**************************
Authors: Rajat & Manas
Algorithm Details: https://csrc.nist.gov/csrc/media/publications/fips/180/2/archive/2002-08-01/documents/fips180-2withchangenotice.pdf
			     : https://www.youtube.com/watch?v=f9EbD6iY9zI
*************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

// SHA256 outputs a 32 unchar digest
#define BLOCK_SIZE 32 

// 8-bit char
typedef unsigned char unchar;   
typedef unsigned int  unint;   
typedef unsigned long long unll;

typedef struct {
	unchar data[64];
	unint datalen, hashValue[8];
	unll totalLen;
} SHA_256;

/************-Operations on unints-********/
#define SHFR(x, n) (x >> n)
#define ROTL(x, n) ((x << n) | (x >> (32 - n)))
#define ROTR(x, n) ((x >> n) | (x << (32 - n)))
/******************************************/

/************-Functions-*******************/
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define EP0(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define EP1(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SIG0(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))
/*******************************************/

/************-SHA-256 Constants-************/
/*SHA-256 uses a sequence of sixty-four constant 32-bit unints, K 0 {256} , K 1 {256} ,K, K {256}
. These 63 unints represent the first thirty-two bits of the fractional parts of the cube roots of the first sixty-
four prime numbers. In hex, these constant unints are (from left to right)*/
static const unint k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};
/*******************************************/

/************-Function Definations-*******************/
void Transform(SHA_256 *msgDigest, const unchar data[])
{
	unint a, b, c, d, e, f, g, h;
	unint idx1, idx2, temp1, temp2;
	unint message[64];

	//Message Schedule
	for (idx1 = 0, idx2 = 0; idx1 < 16; idx1++, idx2 += 4){
		message[idx1] = (data[idx2] << 24) | (data[idx2 + 1] << 16) | (data[idx2 + 2] << 8) | (data[idx2 + 3]);
	}
	while(idx1 < 64){
		message[idx1] = SIG1(message[idx1 - 2]) + message[idx1 - 7] + SIG0(message[idx1 - 15]) + message[idx1 - 16];
		idx1++;
	}

	//Compression
	a = msgDigest->hashValue[0];
	b = msgDigest->hashValue[1];
	c = msgDigest->hashValue[2];
	d = msgDigest->hashValue[3];
	e = msgDigest->hashValue[4];
	f = msgDigest->hashValue[5];
	g = msgDigest->hashValue[6];
	h = msgDigest->hashValue[7];

	for (idx1 = 0; idx1 < 64; idx1++) {
		temp1 = h + EP1(e) + CH(e,f,g) + k[idx1] + message[idx1];
		temp2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + temp1;
		d = c;
		c = b;
		b = a;
		a = temp1 + temp2;
	}

	msgDigest->hashValue[0] += a;
	msgDigest->hashValue[1] += b;
	msgDigest->hashValue[2] += c;
	msgDigest->hashValue[3] += d;
	msgDigest->hashValue[4] += e;
	msgDigest->hashValue[5] += f;
	msgDigest->hashValue[6] += g;
	msgDigest->hashValue[7] += h;
}

void Initialize(SHA_256 *msgDigest)
{
	//Set the initial hash value, H (0)
	msgDigest->datalen = 0;
	msgDigest->totalLen = 0;
	msgDigest->hashValue[0] = 0x6a09e667;
	msgDigest->hashValue[1] = 0xbb67ae85;
	msgDigest->hashValue[2] = 0x3c6ef372;
	msgDigest->hashValue[3] = 0xa54ff53a;
	msgDigest->hashValue[4] = 0x510e527f;
	msgDigest->hashValue[5] = 0x9b05688c;
	msgDigest->hashValue[6] = 0x1f83d9ab;
	msgDigest->hashValue[7] = 0x5be0cd19;
}

void Update(SHA_256 *msgDigest, unchar data[], size_t len)
{
	unint idx;
	for (idx = 0; idx < len; idx++) {
		msgDigest->data[msgDigest->datalen] = data[idx];
		msgDigest->datalen++;
		if (msgDigest->datalen == 64) {
			//For each Message Block
			Transform(msgDigest, msgDigest->data);
			msgDigest->totalLen += 512;
			msgDigest->datalen = 0;
		}
	}
}

void OutputHash(SHA_256 *msgDigest, unchar hash[])
{
	unint idx;
	idx = msgDigest->datalen;
	/*Suppose that the length of the message, M, is l bits. Append the bit “ 1” to the end of the
message, followed by k zero bits, where k is the smallest, non-negative solution to the equation
l + 1 + k ” 448 mod 512 . Then append the 64-bit block that is equal to the number l expressed
using a binary representation. For example, the (8-bit ASCII) message “abc ” has length
8· 3 = 24 , so the message is padded with a one bit, then 448 - (24 +1) = 423 zero bits, and then
the message length, to become the 512-bit padded message*/
	//Padding
	if (msgDigest->datalen < 56) {
		msgDigest->data[idx] = 0x80;
		idx++;
		while (idx < 56){
			msgDigest->data[idx] = 0x00;
			idx++;
		}
	}
	else {
		msgDigest->data[idx] = 0x80;
		idx++;
		while (idx < 64){
			msgDigest->data[idx] = 0x00;
			idx++;
		}
		//For last Message Block
		Transform(msgDigest, msgDigest->data);
		memset(msgDigest->data, 0, 56);
	}

	// Append length of message in last padded block and Transform
	msgDigest->totalLen += msgDigest->datalen * 8;
	msgDigest->data[63] = msgDigest->totalLen;
	msgDigest->data[62] = msgDigest->totalLen >> 8;
	msgDigest->data[61] = msgDigest->totalLen >> 16;
	msgDigest->data[60] = msgDigest->totalLen >> 24;
	msgDigest->data[59] = msgDigest->totalLen >> 32;
	msgDigest->data[58] = msgDigest->totalLen >> 40;
	msgDigest->data[57] = msgDigest->totalLen >> 48;
	msgDigest->data[56] = msgDigest->totalLen >> 56;
	Transform(msgDigest, msgDigest->data);

	//SHA uses big endian thus reverse the unchars when copying the final hashValue to the output hash.
	for (idx = 0; idx < 4; idx++) {
		hash[idx]      = (msgDigest->hashValue[0] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 4]  = (msgDigest->hashValue[1] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 8]  = (msgDigest->hashValue[2] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 12] = (msgDigest->hashValue[3] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 16] = (msgDigest->hashValue[4] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 20] = (msgDigest->hashValue[5] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 24] = (msgDigest->hashValue[6] >> (24 - idx * 8)) & 0x000000ff;
		hash[idx + 28] = (msgDigest->hashValue[7] >> (24 - idx * 8)) & 0x000000ff;
	}
}
/*******************************************/

int main()
{
	unchar text[] = {"abc"};
	unchar hash[BLOCK_SIZE] = {0xba,0x78,0x16,0xbf,0x8f,0x01,0xcf,0xea,0x41,0x41,0x40,0xde,0x5d,0xae,0x22,0x23,
	                                 0xb0,0x03,0x61,0xa3,0x96,0x17,0x7a,0x9c,0xb4,0x10,0xff,0x61,0xf2,0x00,0x15,0xad};
	
	unchar buffer[BLOCK_SIZE];
	SHA_256 msgDigest;
	int pass = 1;

	Initialize(&msgDigest);
	Update(&msgDigest, text, strlen(text));
	OutputHash(&msgDigest, buffer);

	//Check If both Hash Values are same
	pass = pass && !memcmp(hash, buffer, BLOCK_SIZE);
	for(int idx = 0; idx < BLOCK_SIZE; idx++){
		printf("%x", buffer[idx]);
	}
	printf("\n");
	for(int idx = 0; idx < BLOCK_SIZE; idx++){
		printf("%x", hash[idx]);
	}
	printf("\n");

	printf("SHA-256 test: %s\n", pass ? "SUCCEEDED" : "FAILED");
	return 0;
}
