#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <Windows.h>

#define MAX_THREADS 1

using namespace std;

unsigned long _w64 __stdcall calculator(void *path)
{
	ifstream objifstream(static_cast<const char *>(path));

	if(!objifstream.is_open()){
		cout << endl << " ERROR : Can not open file ." << endl;
		return EXIT_FAILURE;
	}
	else
		cout << " File " << path << " Opened successfuly ." << endl;



	return EXIT_SUCCESS;
}

int main()
{
	string path;
	cout << " Please enter file address : " ;
	cin >> path;

	cout << endl << endl << " Lets checking CPU information : " << endl << endl;

	// get system information
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	//get number of Processors (cores) from system information structure
	short numberOfProcessors = sysinfo.dwNumberOfProcessors;

	//may return 0 when not able to detect
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();

	cout << " CPU Information : ";

	switch (sysinfo.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:	cout << "x64 (AMD or Intel)"	<< endl; break;
		case PROCESSOR_ARCHITECTURE_ARM:	cout << "ARM"					<< endl; break;
		case PROCESSOR_ARCHITECTURE_IA64:	cout << "Intel Itanium-based"	<< endl; break;
		case PROCESSOR_ARCHITECTURE_INTEL:	cout << "x86"					<< endl; break;
		case PROCESSOR_ARCHITECTURE_UNKNOWN:cout << "Unknown architecture." << endl; break;
	}

	cout << "\tNumber of processors (cores) : " << numberOfProcessors << endl;
	cout << "\tNumber of concurrent threads supported : " << concurentThreadsSupported << endl;

	short numberOfCoresToUse = 0;
	do{
		cout << endl << " how many processors would you like to use ? ( )\b\b" ;
		cin >> numberOfCoresToUse;
	}while(numberOfCoresToUse <= 0 || numberOfCoresToUse > numberOfProcessors);


	// create threadMask for thread affinity
	unsigned long _w64 threadMask = 1;
	for (int i = 0; i < numberOfCoresToUse; i++)
		threadMask |= 1 << i;

	// create thread
	HANDLE hThread;
	unsigned long _w64 threadId = 0; 

	//create the thread and store its handle for later, and pass the processor count to the thread
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)calculator, (void *)path.c_str(), NULL, &threadId);

	if(hThread){
		SetThreadAffinityMask(hThread, threadMask); //assigns the thread to a processor/core

		WaitForMultipleObjects(MAX_THREADS, (const HANDLE*)&hThread, TRUE, 0xFFFFFFFF);
	
		CloseHandle(hThread);
	}
	else
		cout << " ERROR : app failed on thread creation ." << endl;


	cout << " Press enter to exit .. " << endl;
	fflush(stdin);
	getchar();

	return EXIT_SUCCESS;
}