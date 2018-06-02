#include <iostream>
#include "elzip.hpp"

/* what a nice main file amirite */
using namespace std;

int main(int argc, char *argv[])
{
	cout << "PurpleHLE, an experimental open source HLE iPhone emulator." << endl;
	cout << "Built on: " << __DATE__ " at: " << __TIME__ << endl;
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " <path_to_ipsw> <path_to_extract>" << endl;
		exit(0);
	}
	cout << "Extracting " << argv[1] << " to " << argv[2] << endl;
	elz::extractZip(argv[1], argv[2]);
}
