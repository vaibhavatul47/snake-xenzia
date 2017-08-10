#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace std;

int main()
{
	map<int, string> m;
	string s, name;
	int x, score;
	ifstream ptr("HighScores.txt");
	ofstream tmp("TemporaryFile");
	
	cout << "Enter Name: ";
	cin >> name;
	while (ptr.good()) {
		ptr >> x;
		ptr >> s;
		map.insert(pair<int, string>(x, s));
	}

	return 0;
}
