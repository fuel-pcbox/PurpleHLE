#include <iostream>
#include <boost/locale/encoding_utf.hpp>
#include "elzip.hpp"
#include "Plist.hpp"

/* what a nice main file amirite */
using namespace std;

int main(int argc, char *argv[])
{
	cout << "PurpleHLE, an experimental open source HLE iPhone emulator." << endl;
	cout << "Built on: " << __DATE__ " at: " << __TIME__ << endl;
	if(argc == 2)
	{
		cout << "hello world" << endl; //fixme
	}
	else if(argc == 3)
	{
		cout << "Extracting " << argv[1] << " to " << argv[2] << endl;
		elz::extractZip(argv[1], argv[2]);
		/* this is ugly :X */
		char plistLocation[1024];
  		strcpy(plistLocation, argv[2]);
		strcpy(plistLocation, "/Restore.plist");
		map<string, boost::any> dict;
		Plist::readPlist(plistLocation, dict);
	}
	else
	{
		cout << "Usage:" << endl;
		cout << argv[0] << " <path_to_directory> (bootstrap extracted iOS build)" << endl; //"kickstart" an OS
		cout << argv[0] << " <path_to_ipsw> <path_to_extract> (prep an iOS build for booting)" << endl; //"prep" an OS for kickstart
		exit(0);
	}
}
