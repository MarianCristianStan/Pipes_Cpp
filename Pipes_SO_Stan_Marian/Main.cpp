#include <iostream>
#include <windows.h>
#include <string>

bool isPrime(int num) {
	if (num < 2) {
		return false;
	}
	for (int i = 2; i * i <= num; ++i) {
		if (num % i == 0) {
			return false;
		}
	}
	return true;
}

void findPrimes(int start, int end, HANDLE writePipe) {
	for (int num = start; num <= end; ++num) {
		if (isPrime(num)) {
			DWORD bytesWritten;
			WriteFile(writePipe, &num, sizeof(num), &bytesWritten, NULL);
		}
	}
	CloseHandle(writePipe);
}

int main() {

	int start = 1;
	int end = 10000;

	// Create pipes and child processes
	const int numProcesses = 10;
	const int rangeSize = (end - start + 1) / numProcesses;

	HANDLE pipes[numProcesses][2];
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

	for (int i = 0; i < numProcesses; ++i) {
		CreatePipe(&pipes[i][0], &pipes[i][1], &sa, 0);

		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;

		int start = i * rangeSize + 1;
		int end = (i + 1) * rangeSize;
		findPrimes(start, end, pipes[i][1]);


	}

	// Read and print prime numbers from pipes
	std::cout << "Prime numbers:\n";

	for (int i = 0; i < numProcesses; ++i) {
		int prime;
		DWORD bytesRead;
		while (ReadFile(pipes[i][0], &prime, sizeof(prime), &bytesRead, NULL) && bytesRead > 0) {
			std::cout << prime << " ";
		}
		CloseHandle(pipes[i][0]);
	}

	std::cout << "\n";
	return 0;
}