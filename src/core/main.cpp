#include <iostream>

/* what a nice main file amirite */
using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " <path_to_ipsw> <path_to_extract>" << endl;
		exit(0);
	}
}
