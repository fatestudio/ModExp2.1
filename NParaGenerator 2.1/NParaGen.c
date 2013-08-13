// Giving big number n, compute the parameters nprime0, r and t
// Qin ZHOU: Simple
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

#define DATA_WIDTH	32	// width for a word segment
#define ADDR_WIDTH 	7	// address width
#define TOTAL_ADDR	128 	// total number of words
#define TOTAL_BITS 	4096	// total number of bits in the big number
#define MAX_U32 0xFFFFFFFF

typedef struct bigNum{	// big number structure
	u_int32_t num[TOTAL_ADDR + 1];	// higher bits in higher words, n[4095] is in n.num[127]
	int bits;
} BigNum;

int rand_range(int min_n, int max_n)	// for big number generator
{
    return rand() % (max_n - min_n + 1) + min_n;
}

bool printBigNum(BigNum n){	// print out a big number
	int i;

	printf("%d\n", n.bits);
	for(i = TOTAL_ADDR - 1; i >= 0; i--){
		printf("%08x ", n.num[i]);
	}
	printf("\n");
	return true;
}

u_int32_t checkBit(BigNum a, int bit){	// check a bit. useful for debugging
	u_int32_t num = a.num[bit / DATA_WIDTH];
	num = (num >> (bit % DATA_WIDTH)) % 2;

	return num;
}

int compareBigNum(BigNum bn1, BigNum bn2){
	int posflag = 0;	// 0: unset or equal; k: bn1[k] > bn2[k]; -k: bn1[k] < bn2[k] highest different bit
	int i;

	if(bn1.bits > bn2.bits){
    		posflag = bn1.bits;
	}
	else if(bn1.bits < bn2.bits){
		posflag = -bn2.bits;
	}
	else{	// bn1.bits == bn2.bits
		for(i = bn1.bits - 1; i >= 0; i--){
			u_int32_t b1 = checkBit(bn1, i);
			u_int32_t b2 = checkBit(bn2, i);
			if(b1 > b2){
				posflag = i+1;
				break;
			}
			else if(b1 < b2){
				posflag = -i-1;
				break;
			}
		}
	}

	return posflag;
}

bool initBigNum(BigNum *ret){	// initialize an all zeros big number
	memset(ret, 0, sizeof(BigNum));
	ret->bits = 1;

	return true;
}

BigNum copyBigNum(BigNum n){
	BigNum ret;
	initBigNum(&ret);

	int i;
	for(i = 0; i < (n.bits - 1)/ DATA_WIDTH + 1; i++){
		ret.num[i] = n.num[i];
	}
	ret.bits = n.bits;

	return ret;
}

int countIntBits(int num){
	int count = 0;
	if(num == 0){
		return 1;
	}
	while(num > 0){
		num = num / 2;
		count++;
	}
	return count;
}

BigNum sub(BigNum bn1, BigNum bn2, BigNum modN){ // subtraction and modulus
	int i, j;
	int posflag = compareBigNum(bn1, bn2);
	BigNum ret;
	initBigNum(&ret);

	if(posflag >= 0){ // ret is positive
		for(i = bn1.bits / DATA_WIDTH; i >= 0; i--){
			if(bn1.num[i] >= bn2.num[i]){
				ret.num[i] = bn1.num[i] - bn2.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - bn2.num[i];
				ret.num[i] = ret.num[i] + 1 + bn1.num[i]; // +1 is awesome!
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}

		}
		// set the ret.bits
		for(i = bn1.bits - 1; i >= 0; i--){
			u_int32_t num = checkBit(ret, i);
			if(num > 0){
				ret.bits = i + 1;
				break;
			}
		}
	}
	else{	// ret is negative, add modN
		for(i = bn2.bits / DATA_WIDTH; i >= 0; i--){
			if(bn2.num[i] >= bn1.num[i]){
				ret.num[i] = bn2.num[i] - bn1.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - bn1.num[i] + 1 + bn2.num[i];
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		for(i = modN.bits / DATA_WIDTH; i >= 0; i--){
			if(modN.num[i] >= ret.num[i]){
				ret.num[i] = modN.num[i] - ret.num[i];
			}
			else{ // smaller!
				ret.num[i] = MAX_U32 - ret.num[i] + 1 + modN.num[i];
				j = i + 1;
				while(ret.num[j] == 0){
					ret.num[j] = MAX_U32;
					j++;
				}
				ret.num[j]--;
			}
		}
		// set the ret bits
		for(i = modN.bits - 1; i >= 0; i--){
			u_int32_t num = checkBit(ret, i);
			if(num > 0){
				ret.bits = i + 1;
				break;
			}
		}
	}

	return ret;
}

BigNum shiftR(BigNum m){	// divide 2
	int i;
	BigNum ret = copyBigNum(m);

	for(i = 0; i < (ret.bits - 1) / DATA_WIDTH + 1; i++){
		ret.num[i] = ret.num[i] >> 1;
		if((i + 1) < (ret.bits - 1)/ DATA_WIDTH + 1){
			ret.num[i] = ret.num[i] ^ ((ret.num[i + 1] % 2) << (DATA_WIDTH - 1));
		}
	}
	ret.bits--;

	return ret;
}

BigNum shiftL1(BigNum a, BigNum modN){	// shift left one bit with modN
	int i;
	BigNum ret = copyBigNum(a);
	BigNum _0;
	initBigNum(&_0);

	if(compareBigNum(a, _0) != 0){	// a != 0, if a == 0 don't need shifting
		int gap2 = 1;	// shift bits
		for(i = (ret.bits - 1) / DATA_WIDTH; i >= 0; i--){
			ret.num[i + 1] = ret.num[i + 1] ^ (ret.num[i] >> (DATA_WIDTH - gap2));
			ret.num[i] = ret.num[i] << gap2;
		}
		ret.bits = ret.bits + 1;

		// may be ret > modN
		int posflag = compareBigNum(ret, modN);
		if(posflag >= 0){
			ret = sub(ret, modN, modN);
		}

	}

	return ret;
}

BigNum getBigNum(FILE *fp){
	BigNum ret;
	initBigNum(&ret);
	int i;
	char c = fgetc(fp);
	int bits = 0;	// how many bits ...
	int j = 1;	// j = 10 ^ ..
	while(c != '\n'){	// get bits
		bits = bits * 10 + (c - '0');
		c = fgetc(fp);
	}
	ret.bits = bits;

	int seg = (bits - 1)/ DATA_WIDTH + 1;
	for(i = 0; i < seg; i++){
		for(j = 0; j < 8; j++){	// 8 bytes
			c = fgetc(fp);
			int k = 0;
			if(c >= '0' && c <= '9')
				k = c - '0';
			else if(c >= 'a' && c <= 'f')
				k = c - 'a' + 10;

			ret.num[TOTAL_ADDR - 1 - i] = ret.num[TOTAL_ADDR - 1 - i] * 16 + k;
		}
	}

	return ret;
}

u_int32_t computeNprime0(BigNum n){	// compute Nprime0, following the slide's instruction
	u_int32_t y1 = 1;
	u_int32_t y2 = 0;
	u_int32_t exp2_i_1 = 2;	// store 2 ^ (i - 1)
	u_int32_t exp2_i = 4;	// store 2 ^ i
	int i;
	for(i = 2; i < DATA_WIDTH; i++){	// i is from 2 to DATA_WIDTH !
		if(exp2_i_1 < ((n.num[0] * y1) % exp2_i)){
			y2 = y1 + exp2_i_1;
		}
		else{
			y2 = y1;
		}
		y1 = y2;
		y2 = 0;
		exp2_i_1 = exp2_i;
		exp2_i = exp2_i * 2;
	}
	if(exp2_i_1 < n.num[0] * y1){
		y2 = y1 + exp2_i_1;
	}
	else{
		y2 = y1;
	}
	y1 = y2;
	y2 = 0;	

	return -y1;
}

BigNum computerR(BigNum n){	// compute r, following the slide's instruction
	BigNum r;
	int i;
	initBigNum(&r);
	r.num[TOTAL_ADDR] = 1;
	r.bits = TOTAL_BITS + 1;

	r = sub(r, n, n);	// (all zeros - n) mod n

	return r;
}

BigNum computerT(BigNum n){ // compute t, following the slide's instruction
	BigNum t;
	int i;
	initBigNum(&t);
	t.num[TOTAL_ADDR] = 1;
	t.bits = TOTAL_BITS + 1;
	t = sub(t, n, n);

	for(i = 0; i < TOTAL_BITS; i++){
		t = shiftL1(t, n);
	}

	return t;
}

int main(int argc, char *argv[]){
	BigNum n;
	u_int32_t nprime0;
	BigNum r;
	BigNum t;
	FILE *fp;

	if(argc != 2){
		printf("Please run like this: ./NParaGen inputfile\n");
		exit(-1);
	}

	fp = fopen(argv[1], "r");
	n = getBigNum(fp);
	printf("n:\n");
	printBigNum(n);


	nprime0 = computeNprime0(n);
	printf("nprime0:\t%x\n", nprime0);

	printf("r:\n");
	r = computerR(n);
	printBigNum(r);

	printf("t:\n");
	t = computerT(n);
	printBigNum(t);

	return 0;
}
