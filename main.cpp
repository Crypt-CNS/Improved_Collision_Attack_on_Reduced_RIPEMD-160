/*
Notice: In our program, we count the index from 0. However, we count the index from 1 in the paper!
Please run this program in Visual Studio! You can choose 'Release' instead of 'Debug' so that it will run faster!
*/

#include "RIPEMD160.h"

int main(){
	RIPEMD160 ripe;

	cout << "1---> Generate a starting point." << endl;
	cout << "2---> Find message words to ensure the right branch (The starting point is fixed as in the paper)." << endl;
	cout << endl;

	int cmd;
	cout << "Please select the command:";
	cin >> cmd;
	while (cmd == 1 || cmd == 2){
		switch (cmd){
		case 1:
			ripe.generateStartingPoint();
			break;
		case 2:
			ripe.FindCollisionOnRightBranch();
			break;
		default:
			break;
		}

		cout << endl;
		cout << "Please select the command:";
		cin >> cmd;
	}
	system("pause");
	return 0;
}