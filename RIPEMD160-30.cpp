#include "RIPEMD160.h"
#include <fstream>
#include <cstdlib>
#include <string>
using namespace std;

//ofstream outFile12("Q12.txt", ios::app);
/*rotate left*/
unsigned int LL(unsigned int number, int r){
	number = (number >> (32 - r)) | (number << r);
	return number;
}

/*rotate right*/
unsigned int RR(unsigned int number, int r){
	number = (number << (32 - r)) | (number >> r);
	return number;
}

/*Get the certain bit*/
inline unsigned int getBit(unsigned int number, int position){
	return (number >> position) & 0x1;
}

/*round function XOR*/
unsigned int XOR(unsigned int x, unsigned int y, unsigned int z){
	return x^y^z;
}

/*round function IFX*/
unsigned int IFX(unsigned int x, unsigned int y, unsigned int z){
	return (x&y) ^ ((~x)&z);
}

/*round function XOR*/
unsigned int ONZ(unsigned int x, unsigned int y, unsigned int z){
	return (x | (~y)) ^ z;
}

/*round function XOR*/
unsigned int IFZ(unsigned int x, unsigned int y, unsigned int z){
	return (x&z) ^ (y&(~z));
}

/*round function ONX*/
unsigned int ONX(unsigned int x, unsigned int y, unsigned int z){
	return x ^ (y | (~z));
}

//convert to binary
void toBinary(unsigned int num){
	unsigned int left = 0;
	int array[32];
	for (int i = 0; i < 32; i++){
		array[i] = 0;
	}
	int count = 0;
	while (num){
		left = num % 2;
		array[count++] = left;
		num = num / 2;
	}

	for (int i = 31; i >= 0; i--){
		cout << array[i];
		//outFile12 << array[i];
		if (i % 4 == 0){
			cout << " ";
			//outFile12 << " ";
		}
	}
	cout << endl;
	//outFile12 << endl;
	//outFile12.close();
}

RIPEMD160::RIPEMD160(){
	//Initialization of IV
	LIV[0] = RIV[0] = 0xc059d148;
	LIV[1] = RIV[1] = 0x7c30f4b8;
	LIV[2] = RIV[2] = 0x1d840c95;
	LIV[3] = RIV[3] = 0x98badcfe;
	LIV[4] = RIV[4] = 0xefcdab89;

	//Initialization of constants in the left branch.
	LC[0] = 0X0;
	LC[1] = 0X5A827999;
	LC[2] = 0X6ED9EBA1;
	LC[3] = 0X8F1BBCDC;
	LC[4] = 0XA953FD4E;

	//Initialization of constants in the right branch.
	RC[0] = 0X50A28BE6;
	RC[1] = 0X5C4DD124;
	RC[2] = 0X6D703EF3;
	RC[3] = 0X7A6D76E9;
	RC[4] = 0X0;

	unsigned char LST[5][16] = { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
		7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
		11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
		11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
		9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };

	unsigned RST[5][16] = { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
		9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
		9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
		15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
		8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };

	unsigned int LIT[5][16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
		3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
		1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
		4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };

	unsigned int RIT[5][16] = { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
		6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
		15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
		8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
		12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };

	for (int i = 0; i != 5; i++){
		for (int j = 0; j != 16; j++){
			LS[i][j] = LST[i][j];//Initialization of rotation values in the left branch.
			RS[i][j] = RST[i][j];//Initialization of rotation values in the right branch.
			LI[i][j] = LIT[i][j];//Initialization of word permutation in the left branch.
			RI[i][j] = RIT[i][j];//Initialization of word permutation in the right branch.
		}
	}

	for (int i = 0; i < 32; i++){
		runningtimes[i] = 0;
		round2run[i] = 0;
	}

	in[10] = 0x1000000;
	out[10] = 0x1;

	in[11] = 0x15;
	out[11] = 0xa800;

	in[12] = 0x6ffba800;
	out[12] = 0xea001bff;

	in[13] = 0x40400001;
	out[13] = 0x1010;

	in[14] = 0xafffff5f;
	out[14] = 0xfff5fb00;

	in[15] = 0x9d020;
	out[15] = 0x2740800;

	in[16] = 0x85f87f2;
	out[16] = 0xbf0fe410;

	in[17] = 0;
	out[17] = 0;

	in[18] = 0xffffd008;
	out[18] = 0xe8040000;

	in[19] = 0xd75fbffc;
	out[19] = 0xafdffdec;

	in[20] = 0x10200813;
	out[20] = 0x812102;

	in[21] = 0xff7edffe;
	out[21] = 0x7edffeff;

	in[22] = 0x81000001;
	out[22] = 0x102;

	in[23] = 0xffffff00;
	out[23] = 0xfff80000;

	in[24] = 0x80000;
	out[24] = 0x4000000;

	in[25] = 0x1000800;
	out[25] = 0x80040000;

	in[26] = 0x7ffc0000;
	out[26] = 0xbffff800;

	in[27] = 0;
	out[27] = 0;

	in[28] = 0xc0000000;
	out[28] = 0xfffffff0;

	in[29] = 0x10;
	out[29] = 0x80000;

	preRandomValue = 0;
}

//Compute the hash value according to the start step and the steps
void RIPEMD160::computeHash(int start, int steps){
	for (int i = start; i < steps; i++){
		if (i < 5){
			switch (i){
			case 0:
				X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
				Q[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0];
				Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
				break;
			case 1:
				X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
				Q[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0];
				Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
				break;
			case 2:
				X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
				Q[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0];
				Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
				break;
			case 3:
				X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
				Q[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
				Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
				break;
			case 4:
				X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);
				Q[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
				Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
				break;
			}
		}
		else if (i < 16){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
			Q[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
		}
		else if (i < 32){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
			Q[i] = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1], RS[1][i - 16]);
		}
		else if (i < 48){
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[2][i - 32]] + LC[2], LS[2][i - 32]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[2][i - 32]] + RC[2], RS[2][i - 32]);
		}
		else if (i < 64){
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[3][i - 48]] + LC[3], LS[2][i - 48]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[3][i - 48]] + RC[3], RS[2][i - 48]);
		}
		else{
			//X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[4][i - 64]] + LC[4], LS[2][i - 64]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + XOR(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[4][i - 64]] + RC[4], RS[2][i - 64]);
		}
	}
}

//Get random value
inline unsigned int RIPEMD160::getRand(){
	unsigned int num1 = 0, num2 = 0, result;
	num1 = rand() & 0xffff;
	num2 = rand() & 0xffff;
	result = (num1 << 16) | num2;
	result = result + preRandomValue;
	preRandomValue = result;
	return result;
}

inline bool RIPEMD160::checkQ(unsigned int q, int index, int shift){
	if (LL(q + in[index], shift) != out[index] + LL(q, shift)){
		return false;
	}
	return true;
}

void RIPEMD160::getModification(unsigned int &number, int *pos, int length, int condition){
	for (int i = 0; i < length; i++){
		unsigned int t = getBit(number, pos[i]);
		if (t != condition){
			number = number ^ EXP[pos[i]];
		}
	}
}

unsigned int RIPEMD160::getMultiModification(unsigned int number1, unsigned int number2, int *pos1, int *pos2, int length, int condition){
	for (int i = 0; i < length; i++){
		unsigned int t1 = getBit(number1, pos1[i]);
		unsigned int t2 = getBit(number2, pos2[i]);
		if (condition == 1){//t1 != t2
			if (t1 == t2){
				number2 = number2 ^ EXP[pos2[i]];
			}
		}
		else{//t1 = t2
			if (t1 != t2){
				number2 = number2 ^ EXP[pos2[i]];
			}
		}
	}
	return number2;
}

inline void RIPEMD160::modifyY9(){
	//19 bits
	//Y9: 0---000000-1--1---0000--1-001010
	//Y9,31=0
	int condition1[5] = { 20, 17, 7, 3, 1 };
	int condition0[15] = { 31, 27, 26, 25, 24, 23, 22, 13, 12, 11, 10, 5, 4, 2, 0 };
	getModification(Y[9], condition1, 5, 1);
	getModification(Y[9], condition0, 15, 0);
}

inline void RIPEMD160::modifyY10(){
	//32 bits
	//Y11: 10--0---0000110110010000000nuuuu
	//Y11,31=1
	int condition1[10] = { 31, 19, 18, 16, 15, 12, 3, 2, 1, 0 };
	int condition0[17] = { 30, 27, 23, 22, 21, 20, 17, 14, 13, 11, 10, 9, 8, 7, 6, 5, 4 };
	getModification(Y[10], condition1, 10, 1);
	getModification(Y[10], condition0, 17, 0);

	//multi modification
	int pos1[5] = { 19, 18, 16, 15, 14 };
	int pos2[5] = { 29, 28, 26, 25, 24 };
	if (getBit(Y[9], 19) == 1){
		Y[10] = Y[10] | EXP[29];
	}

	if (getBit(Y[9], 18) == 1){
		Y[10] = Y[10] | EXP[28];
	}

	if (getBit(Y[9], 16) == 1){
		Y[10] = Y[10] | EXP[26];
	}

	if (getBit(Y[9], 15) == 1){
		Y[10] = Y[10] | EXP[25];
	}

	if (getBit(Y[9], 14) == 1){
		Y[10] = Y[10] | EXP[24];
	}
}

inline void RIPEMD160::modifyY11(){
	//28 bits
	//Y12: nuuuuuuuuuuuuuuuu0n0n00----01100
	int condition1[18] = { 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 3, 2, };
	int condition0[10] = { 31, 14, 13, 12, 11, 10, 9, 4, 1, 0 };
	getModification(Y[11], condition1, 18, 1);
	getModification(Y[11], condition0, 10, 0);
}

inline void RIPEMD160::modifyY12(){
	//30 bits
	//Y12: 0unn1uu-111-1-1--nuunn11011011un
	//Y12,20=1; Y12,18=0; Y12,16=0
	int condition1[19] = { 30, 27, 26, 25, 23, 22, 21, 20, 19, 17, 13, 12, 9, 8, 6, 5, 3, 2, 1 };
	int condition0[11] = { 31, 29, 28, 18, 14, 11, 10, 7, 4, 0, 16 };
	getModification(Y[12], condition1, 19, 1);
	getModification(Y[12], condition0, 11, 0);
}

inline void RIPEMD160::modifyY13(){
	//28 bits
	//Y13: -100001111----1-10nu10101-nu1-11
	//Y13,[6,19,20,21]=1
	int condition1[19] = { 30, 25, 24, 23, 22, 17, 15, 12, 11, 9, 7, 4, 3, 1, 0, 6, 21, 20, 19 };
	int condition0[9] = { 29, 28, 27, 26, 14, 13, 10, 8, 5 };
	getModification(Y[13], condition1, 19, 1);
	getModification(Y[13], condition0, 9, 0);
}

inline void RIPEMD160::modifyY14(){
	//26 bits
	//Y14: 00---01111-0u-u-101000-u----0-01
	//Y14,9=0; Y14,5=0; Y14,29=0; Y14,28=0; Y14,21=0;
	//Y14,27=1
	int condition1[11] = { 27, 25, 24, 23, 22, 19, 17, 15, 13, 8, 0 };
	int condition0[15] = { 29, 28, 31, 30, 26, 20, 14, 12, 11, 10, 3, 1, 5, 21, 9 };
	getModification(Y[14], condition1, 11, 1);
	getModification(Y[14], condition0, 15, 0);
}

inline void RIPEMD160::modifyY15(){
	//26 bits
	//Y15: 111-n1uu000n1n--0001n----nuuuuuu
	int condition1[15] = { 31, 30, 29, 28, 26, 25, 24, 19, 12, 5, 4, 3, 2, 1, 0 };
	int condition0[11] = { 27, 23, 22, 21, 20, 18, 15, 14, 13, 11, 6 };
	getModification(Y[15], condition1, 15, 1);
	getModification(Y[15], condition0, 11, 0);
}

inline void RIPEMD160::modifyY16(){
	//27Bits
	//Y16: 1u1-1--un--0111-00u10unnn-nnn01-
	//Y16,28=0; Y16,26=1; 
	int condition1[15] = { 31, 30, 29, 27, 26, 24, 21, 19, 18, 17, 13, 12, 10, 1 };
	int condition0[13] = { 28, 23, 20, 15, 14, 11, 9, 8, 7, 5, 4, 3, 2 };
	getModification(Y[16], condition1, 15, 1);
	getModification(Y[16], condition0, 13, 0);
}

inline void RIPEMD160::modifyY17(){
	//20Bits
	//Y17: 01------0n-011--1n0000----0-00-1
	int condition1[5] = { 30, 19, 18, 15, 0 };
	int condition0[12] = { 31, 23, 22, 20, 14, 13, 12, 11, 10, 5, 3, 2 };
	getModification(Y[17], condition1, 5, 1);
	getModification(Y[17], condition0, 12, 0);

	//multi modification
	int pos1[3] = { 28, 21, 16 };
	int pos2[3] = { 28, 21, 16 };
	Y[17] = getMultiModification(Y[16], Y[17], pos1, pos2, 3, 0);
}

inline void RIPEMD160::modifyY18(){
	//14bits
	//Y18: 1u------1--100--010----------1-1
	int condition1[7] = { 31, 30, 23, 20, 14, 2, 0 };
	int condition0[5] = { 21,19, 18, 15, 13 };
	getModification(Y[18], condition1, 7, 1);
	getModification(Y[18], condition0, 5, 0);

	//multi modification
	int pos1[3] = { 17, 8, 1 };
	int pos2[3] = { 17, 8, 1 };
	Y[18] = getMultiModification(Y[17], Y[18], pos1, pos2, 3, 0);
}

inline bool RIPEMD160::checkY18(){
	int bitpos[14] = { 19, 18, 15, 13, 31, 30, 23, 20, 14, 2, 0, 17, 8, 1 };
	int condition[14] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2 };
	for (int i = 0; i < 14; i++){
		if (condition[i] != 2){
			if (getBit(Y[18], bitpos[i]) != condition[i]){
				return false;
			}
		}
		else{
			if (getBit(Y[18], bitpos[i]) != getBit(Y[17], bitpos[i])){
				return false;
			}
		}
	}
	return true;
}

inline void RIPEMD160::modifyY19(){
	//11bits
	//Y19; -0--------1---------0nu11---11-0
	int condition1[6] = { 21, 9, 8, 7, 3, 2 };
	int condition0[4] = { 30, 11, 10, 0 };
	getModification(Y[19], condition1, 6, 1);
	getModification(Y[19], condition0, 4, 0);

	//multi modification
	int pos1[1] = { 24 };
	int pos2[1] = { 24 };
	Y[19] = getMultiModification(Y[18], Y[19], pos1, pos2, 1, 0);

	//Other conditions to control the modular difference
	unsigned int tmpY15 = LL(Y[15], 10);
	unsigned int low6Bits = tmpY15 & 0x3f;
	unsigned int low31Bits = tmpY15 & 0x7fffffff;

	unsigned int low6BitsY19 = Y[19] & 0x3f;
	unsigned int low31BitsY19 = Y[19] & 0x7fffffff;

	if (low6BitsY19 < low6Bits){
		if (getBit(Y[19], 6) != getBit(Y[15], 28)){
			Y[19] = Y[19] ^ EXP[6];
		}
	}
	else{
		if (getBit(Y[19], 6) == getBit(Y[15], 28)){
			Y[19] = Y[19] ^ EXP[6];
		}
	}

	if (low31BitsY19 < low31Bits){
		if (getBit(Y[19], 31) != getBit(Y[15], 21)){
			Y[19] = Y[19] ^ EXP[31];
		}
	}
	else{
		if (getBit(Y[19], 31) == getBit(Y[15], 21)){
			Y[19] = Y[19] ^ EXP[31];
		}
	}
}

inline void RIPEMD160::modifyY20(){
	//15bits
	//Y20; -1-----1011-----11111-101-------
	//Y20,6 = 1;

	int condition1[12] = { 30, 24, 22, 21, 15, 14, 13, 12, 11, 9, 7, 6 };
	int condition0[2] = { 23, 8 };
	getModification(Y[20], condition1, 12, 1);
	getModification(Y[20], condition0, 2, 0);

	//multi modification
	int pos1[1] = { 0 };
	int pos2[1] = { 10 };
	Y[20] = getMultiModification(Y[16], Y[20], pos1, pos2, 1, 0);
}

inline void RIPEMD160::modifyY21(){
	//Y21: u-----001-u-----------1u------00
	//Y21,30=1
	int condition1[6] = { 31, 30, 23, 21, 9, 8 };
	int condition0[7] = { 25, 24, 16, 15, 14, 1, 0 };// Y21,14 = 0; Y21,15 = 0, Y21,16 = 0;

	getModification(Y[21], condition1, 6, 1);
	getModification(Y[21], condition0, 7, 0);

	//multi modification
	int pos1[2] = { 19, 20 };
	int pos2[2] = { 19, 20 };
	Y[21] = getMultiModification(Y[20], Y[21], pos1, pos2, 2, 0);

	//multi modification
	int pos3[6] = { 24, 25, 26, 27, 28, 29 };
	int pos4[6] = { 2, 3, 4, 5, 6, 7 };
	Y[21] = getMultiModification(Y[17], Y[21], pos3, pos4, 6, 0);
}

inline void RIPEMD160::modifyY22(){
	//6Bits
	//Y22: 1--------------0-----01-------n-

	int condition1[3] = { 31, 22, 9 };
	int condition0[6] = { 26, 25, 24, 16, 10, 1 };//Add Y22,24 = 0; Y22,25 = 0; Y22,26 = 0;

	getModification(Y[22], condition1, 3, 1);
	getModification(Y[22], condition0, 6, 0);

	if (checkY22_8() == false){
		Y[22] = Y[22] ^ EXP[8];
	}
}

inline bool RIPEMD160::checkY22_8(){
	unsigned int lowY22_8 = Y[22] & 0xff;
	unsigned int lowY18_8 = LL(Y[18], 10) & 0xff;
	unsigned int Y22_8 = getBit(Y[22], 8);
	unsigned int Y18_30 = getBit(Y[18], 30);
	if (lowY22_8 >= lowY18_8 && Y22_8 != Y18_30){
		return true;
	}
	else if (lowY22_8 < lowY18_8 && Y22_8 == Y18_30){
		return true;
	}
	return false;
}

inline bool RIPEMD160::checkY22(){
	int bitpos[5] = { 1, 9, 10, 16, 31 };
	int condition[6] = { 0, 1, 0, 0, 1 };

	for (int i = 0; i < 5; i++){
		if (getBit(Y[22], bitpos[i]) != condition[i]){
			cout << "Y22(out):" << bitpos[i] << endl;
			return false;
		}
	}

	return true;
}

bool RIPEMD160::testRightBranch(){
	for (int i = 23; i < 30; i++){
		Q[i] = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
		Y[i] = LL(Y[i - 4], 10) + LL(Q[i], RS[1][i - 16]);

		if (i == 23){
			if (checkY23() == false){
				return false;
			}
		}

		else if (i == 24){
			if (checkY24() == false){
				return false;
			}
		}

		else if (i == 25){
			if (checkY25() == false){
				return false;
			}
		}

		else if (i == 26){
			if (checkY26() == false){
				return false;
			}
		}

		else if (i == 27){
			if (checkY27() == false){
				return false;
			}
		}

		else if (i == 28){
			if (getBit(Y[28], 8) != getBit(Y[27], 8)){
				return false;
			}
		}

		if (checkQ(Q[i], i, RS[1][i - 16]) == false){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY23(){
	int bitpos[6] = { 11, 1, 0, 31, 16, 9, };
	int condition[6] = { 0, 0, 0, 1, 1, 1 };

	for (int i = 0; i < 6; i++){
		if (getBit(Y[23], bitpos[i]) != condition[i]){
			return false;
		}
	}

	if (getBit(Y[23], 18) != getBit(Y[22], 18)){
		return false;
	}
	return true;
}

inline bool RIPEMD160::checkY24(){
	int bitpos[6] = { 26, 20, 1, 31, 11, 0, };
	int condition[6] = { 0, 0, 0, 1, 1, 1 };

	//According to newly added bit conditions, Y24,0 = 1 and Y24,1= 0 will hold with probability 1.
	if (getBit(Y[24], 0) != 1 || getBit(Y[24], 1) != 0){
		cout << "Y24 is wrong!" << endl;
	}

	for (int i = 0; i < 6; i++){
		if (getBit(Y[24], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY25(){
	int bitpos[4] = { 20, 10, 9, 11, };
	int condition[4] = { 0, 0, 0, 1 };

	for (int i = 0; i < 4; i++){
		if (getBit(Y[25], bitpos[i]) != condition[i]){
			return false;
		}
	}
	return true;
}

inline bool RIPEMD160::checkY26(){
	if (getBit(Y[26], 30) != 1 || getBit(Y[26], 4) != getBit(Y[25], 4)){
		return false;
	}

	return true;
}

inline bool RIPEMD160::checkY27(){
	int bitpos[3] = { 19, 20, 21 };

	for (int i = 0; i < 3; i++){
		if (getBit(Y[27], bitpos[i]) != getBit(Y[26], bitpos[i])){
			return false;
		}
	}
	return true;
}

//Meet in the middle: Application on RIPEMD-160
bool RIPEMD160::findStartingPoint(){
	for (int i = 9; i < 19; i++){
		Y[i] = getRand();
	}
	int index = 0;

	modifyY9();
	modifyY10();
	modifyY11();
	modifyY12();
	modifyY13();

	index = 14;
	modifyY14();
	word[3] = RR(Y[index] - LL(Y[index - 4], 10), RS[0][index % 16]) - (ONX(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[0]);//Then, we can compute word[3]
	Q[index] = RR(Y[index] - LL(Y[index - 4], 10), RS[0][index % 16]);
	if (checkQ(Q[index], index, RS[0][index % 16]) == false){
		cout << "Y" << index << " is wrong!" << endl;
	}

	index = 15;
	modifyY15();
	word[12] = RR(Y[index] - LL(Y[index - 4], 10), RS[0][index % 16]) - (ONX(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[0]);//Then, we can compute word[12]
	Q[index] = RR(Y[index] - LL(Y[index - 4], 10), RS[0][index % 16]);
	if (checkQ(Q[index], index, RS[0][index % 16]) == false){
		cout << "Y" << index << " is wrong!" << endl;
	}

	index = 16;
	modifyY16();
	word[6] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]) - (IFZ(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[1]);//Then, we can compute word[6]
	Q[index] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]);
	if (checkQ(Q[index], index, RS[1][index % 16]) == false){
		cout << "Y" << index << " is wrong!" << endl;
	}

	index = 17;
	modifyY17();
	word[11] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]) - (IFZ(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[1]);//Then, we can compute word[11]
	Q[index] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]);
	if (checkQ(Q[index], index, RS[1][index % 16]) == false){
		cout << "Y" << index << " is wrong!" << endl;
	}

	index = 18;
	Y[index] = LL(Y[index - 4], 10) + LL(IFZ(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[1] + word[3], RS[1][index % 16]);
	Q[index] = IFZ(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[1] + word[3];
	if (checkY18() == false){
		//cout << "Y18 WRONG!" << endl;
		return false;
	}
	else{
		if (checkQ(Q[index], index, RS[1][index % 16]) == false){
			cout << "FALSE!" << endl;
			system("pause");
			return false;
		}
	}
	return true;
}

void RIPEMD160::generateStartingPoint(){
	srand(time(NULL));
	unsigned int times = 0;
	bool flag = findStartingPoint();
	while (!flag){
		times++;
		flag = findStartingPoint();
	}

	int index = 19;
	modifyY19();
	Q[19] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]);
	while (!checkQ(Q[19], 19, RS[1][3])){
		cout << "WRONG!"<<endl;
		Y[19] = getRand();
		modifyY19();
		Q[19] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]);
	}
	word[7] = RR(Y[index] - LL(Y[index - 4], 10), RS[1][index % 16]) - (IFZ(Y[index - 1], Y[index - 2], LL(Y[index - 3], 10)) + LL(Y[index - 5], 10) + RC[1]);//Then, we can compute word[7]

	cout << endl;
	cout << "Total running times: 0x" << hex << times << endl;
	cout << "A starting point:" << endl;
	for (int i = 9; i < 20; i++){
		//toBinary(Y[i]);
		cout << "Y[" << dec << i << "] = 0x";
		cout << hex << Y[i] << ";" << endl;
	}
	cout << endl;

	cout << "word[3]=0x" << hex << word[3] << ";" << endl;
	cout << "word[12]=0x" << hex << word[12] << ";" << endl;
	cout << "word[6]=0x" << hex << word[6] << ";" << endl;
	cout << "word[11]=0x" << hex << word[11] << ";" << endl;
	cout << "word[7]=0x" << hex << word[7] << ";" << endl;
}

void RIPEMD160::initializeDensePart(){
	Y[9] = 0x703f408a;
	Y[10] = 0xb70d900f;
	Y[11] = 0x7fff810c;
	Y[12] = 0x4efa336e;
	Y[13] = 0x43ff9adf;
	Y[14] = 0xbcfa1d5;
	Y[15] = 0xf70b17bf;
	Y[16] = 0xef6f3403;
	Y[17] = 0x482f8241;
	Y[18] = 0xc592521d;
	Y[19] = 0x16603ac;

	word[3] = 0xd5f43d9f;
	word[12] = 0x20d0d1cb;
	word[6] = 0x1221b7bb;
	word[11] = 0xf567ac2e;
	word[7] = 0x42156657;

	srand(time(NULL));
}

void RIPEMD160::forwardFromMiddle(){
	for (int i = 20; i < 23; i++){
		Y[i] = getRand();
		if (i == 20){
			modifyY20();
		}
		else if (i == 21){
			modifyY21();
		}
		else if (i == 22){
			modifyY22();
		}

		Q[i] = RR(Y[i] - LL(Y[i - 4], 10), RS[1][i % 16]);
		if (checkQ(Q[i], i, RS[1][i % 16]) == false){
			cout << "Y" << i << " is wrong!" << endl;
		}

		word[RI[i / 16][i % 16]] = RR(Y[i] - LL(Y[i - 4], 10), RS[1][i % 16]) - (IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + LL(Y[i - 5], 10) + RC[1]);//Then, we can compute word[0]
	}
}

bool RIPEMD160::forwardFromIV(){
	Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[5] + RC[0], RS[0][0]);//word[5] is known

	word[14] = getRand();//word[14] = −−−−−100 0−−−−−−− −−−−−−−− −−−−−−−−.
	word[14] = word[14] & 0xf87fffff;
	word[14] = word[14] | 0x04000000;
	Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[14] + RC[0], RS[0][1]);//word[14] is unknown

	Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[7] + RC[0], RS[0][2]);//word[7] is known

	Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[0] + RC[0], RS[0][3]);//word[0] is known

	word[9] = getRand();
	Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[9] + RC[0], RS[0][4]);// word[9] is unknown


	//Let Y6=0x0, then compute Y[5]
	Y[6] = 0;
	unsigned int tmp = RR(Y[6] - LL(Y[2], 10), RS[0][6]) - (word[11] + RC[0] + LL(Y[1], 10));//word[11] is known
	Y[5] = ONX(tmp, Y[4], LL(Y[3], 10));
	word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (ONX(Y[4], Y[3], LL(Y[2], 10)) + LL(Y[0], 10) + RC[0]);//compute word[2]

	//compute Y[8]
	//Y[9] = Y[5]<<<10 + (ONX(Y[8],Y[7],Y[6]<<<10) + Y[4]<<<10 + word[6] +RC[0])<<<RS[0][9]
	//Since Y[6] = 0 ----> ONX(Y[8],Y[7],Y[6]<<<10) = Y[8]^0xffffffff;
	Y[8] = RR(Y[9] - LL(Y[5], 10), RS[0][9]) - (LL(Y[4], 10) + word[6] + RC[0]);//word[6] is known
	Y[8] = Y[8] ^ 0xffffffff;
	//check the bit condition on Y[8]
	//int condition[3] = {1, 1, 1 };
	//int pos[3] = { 22, 24, 26 };
	if (getBit(Y[8], 22) != 1 || getBit(Y[8], 24) != 1 || getBit(Y[8], 26) != 1){
		return false;
	}

	tmp = RR(Y[8] - LL(Y[4], 10), RS[0][8]) - (LL(Y[3], 10) + word[13] + RC[0]);//word[13] is known
	Y[7] = ONX(tmp, Y[6], LL(Y[5], 10));
	word[4] = RR(Y[7] - LL(Y[3], 10), RS[0][7]) - (ONX(Y[6], Y[5], LL(Y[4], 10)) + LL(Y[2], 10) + RC[0]);//compute word[4]

	//compute word[10], word[1], word[8], word[15] to link two parts
	for (int i = 10; i < 14; i++){
		word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + LL(Y[i - 5], 10) + RC[0]);
		//cout << (int)RI[0][i] << endl;
	}

	//Check the modulat difference
	for (int i = 10; i < 14; i++){
		Q[i] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]);
		if (checkQ(Q[i], i, RS[0][i]) == false){
			//cout << "WRONG Q " << dec << i << endl;
			return false;
		}
	}
	return true;
}

void RIPEMD160::FindCollisionOnRightBranch(){
	///
	unsigned long long times = 0;
	initializeDensePart();
	forwardFromMiddle();

	bool flag = forwardFromIV();
	while (!flag){
		times++;
		flag = forwardFromIV();
	}

	bool verifyFlag = testRightBranch();
	while (!verifyFlag){
		forwardFromMiddle();
		flag = forwardFromIV();
		times++;
		while (!flag){
			flag = forwardFromIV();
			times++;
		}
		verifyFlag = testRightBranch();
	}
	cout << "Running Times:" << hex << times << endl;
	cout << "Press any button to output the result!" << endl;
	system("pause");
	///

	/*//
	The instance in the paper
	word[0] = 0x284ca581;
	word[1] = 0x55fd6120;
	word[2] = 0x694b052c;
	word[3] = 0xd5f43d9f;
	word[4] = 0xa064a7c8;
	word[5] = 0xb9f7b3cd;
	word[6] = 0x1221b7bb;
	word[7] = 0x42156657;
	word[8] = 0x121ecfee;
	word[9] = 0xce7a7105;
	word[10] = 0xf2d47e6f;
	word[11] = 0xf567ac2e;
	word[12] = 0x20d0d1cb;
	word[13] = 0x9d928b7d;
	word[14] = 0x5c6ff19b;
	word[15] = 0xc306e50f;
	///*/

	/////Test
	computeHash(0, 30);
	for (int i = 10; i < 30; i++){
		if (checkQ(Q[i], i, RS[i / 16][i % 16]) == false){
			cout << "WRONG Q " << dec << i << endl;
		}
	}

	unsigned int tmpY[80];
	for (int i = 0; i < 30; i++){
		tmpY[i] = Y[i];
	}

	word[15] = word[15] + EXP[24];
	computeHash(0, 30);

	/////
	cout << "Difference Propagation:" << endl;
	for (int i = 0; i < 30; i++){
		cout << "Y" << dec << i << ": ";
		toBinary(Y[i] ^ tmpY[i]);
	}
	cout << endl;
	/////

	/////
	cout << "Internal States:" << endl;
	formalizeOutputForWindow(tmpY);
	cout << endl;
	/////

	//formalizeOutput(tmpY);//For Latex

	//output message words
	for (int i = 0; i < 15; i++){
		cout << "word[" << dec << i << "] = 0x";
		cout << hex << word[i] << endl;
	}
	cout << "word[15] = 0x" << hex << word[15] - EXP[24] << " -> 0x" << word[15] << endl;
}

void  RIPEMD160::formalizeOutput(unsigned int tmpY[]){
	/*for (int i = 0; i < 5; i++){
	cout << i - 4 << " &";
	outputOneLine(RIV[i]);
	cout << "\\" << "\\" << endl;
	}*/

	for (int i = 0; i < 30; i++){
		cout << i + 1 << " &";
		outputOneLine(tmpY[i], Y[i], i);
		cout << "\\" << "\\" << endl;
	}
}

void  RIPEMD160::formalizeOutputForWindow(unsigned int tmpY[]){
	for (int i = 0; i < 30; i++){
		cout << "Y" << (dec) << i + 1 << " ";
		outputOneLineForWindow(tmpY[i], Y[i], i);
		cout << endl;
	}
}

void RIPEMD160::outputOneLine(unsigned int original, unsigned int newValue, int steps){
	string str;
	str = "";
	unsigned int result;
	for (int i = 31; i >= 0; i--){
		if (getBit(original, i) == getBit(newValue, i)){
			result = getBit(original, i);
			if (result == 1){
				str += "1";
			}
			else{
				str += "0";
			}
		}
		else{
			if (getBit(original, i) == 0 && getBit(newValue, i) == 1){
				str += "n";
			}
			else if (getBit(original, i) == 1 && getBit(newValue, i) == 0){
				str += "u";
			}
		}
	}

	if (steps<9 || steps>19){
		for (int i = 0; i < 32; i++){
			if (i != 0 && i % 8 == 0){
				cout << " & ";
			}
			cout << str[i] << "&";
		}
		cout << dec << (int)RI[steps / 16][(steps + 1) % 16] << endl;
	}

	else{
		for (int i = 0; i < 32; i++){
			if (i != 0 && i % 8 == 0){
				cout << " & ";
			}
			cout << "\\textcolor{red}{" << str[i] << "}&";
		}
		cout << dec << (int)RI[steps / 16][(steps + 1) % 16] << endl;
	}
}

void RIPEMD160::outputOneLineForWindow(unsigned int original, unsigned int newValue, int steps){
	string str;
	str = "";
	unsigned int result;
	for (int i = 31; i >= 0; i--){
		if (getBit(original, i) == getBit(newValue, i)){
			result = getBit(original, i);
			if (result == 1){
				str += "1";
			}
			else{
				str += "0";
			}
		}
		else{
			if (getBit(original, i) == 0 && getBit(newValue, i) == 1){
				str += "n";
			}
			else if (getBit(original, i) == 1 && getBit(newValue, i) == 0){
				str += "u";
			}
		}
	}

	for (int i = 0; i < 32; i++){
		if (i != 0 && i % 8 == 0){
			cout << " ";
		}
		cout << str[i];
	}
	cout << hex << " " << original << "  ->  " << newValue << endl;
}


void RIPEMD160::outputOneLine(unsigned int value){
	//-4 &-&-&-&-&-&-&-&-& & -&-&-&-&-&-&-&-& & -&-&-&-&-&-&-&-& & -&-&-&-&-&-&-&-&
	string str;
	str = "";
	unsigned int result;
	for (int i = 31; i >= 0; i--){
		result = value&EXP[i];
		if (result == EXP[i]){
			str += "1";
		}
		else{
			str += "0";
		}
	}

	for (int i = 0; i < 32; i++){
		if (i != 0 && i % 8 == 0){
			cout << " & ";
		}
		cout << str[i] << "&";
	}
	cout << endl;
}
