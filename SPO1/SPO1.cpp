#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include "Fileapi.h"
#include "TextTable.h"
using namespace std;
using namespace std::chrono;

#define ORIGINAL_FILE "FirstLab.txt"
#define BIG_FILE "BigFile.txt"
#define TARGET_FILE "TargetFile.txt"


int copy_c(char* src_file)
{
	char buffer[BUFSIZ];

	FILE* oldFile = fopen(src_file, "rb");
	if (oldFile == NULL)
	{
		perror(src_file); char c;
		cin >> c;
		return EXIT_FAILURE;
	}

	FILE* newFile = fopen(TARGET_FILE, "wb");
	if (newFile == NULL)
	{
		perror(TARGET_FILE);
		return EXIT_FAILURE;
	}
	size_t inBytes, outBytes;
	while ((inBytes = fread(buffer, 1, BUFSIZ, oldFile)) > 0)
	{
		outBytes = fwrite(buffer, 1, inBytes, newFile);

		if (outBytes != inBytes)
		{
			perror("Failed to copy file via C functions.");
			return EXIT_FAILURE;
		}
	}

	fclose(oldFile);
	fclose(newFile);
	return EXIT_SUCCESS;
}
int copy_windows(char* src_file)
{
	CHAR buffer[BUFSIZ];

	HANDLE hIn = CreateFile(src_file, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		cout << "Failed to open input file: error code " << GetLastError();
		return EXIT_FAILURE;
	}

	HANDLE hOut = CreateFile(TARGET_FILE, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		cout << "Failed to open output file: error code " << GetLastError();
		return EXIT_FAILURE;
	}

	DWORD nIn, nOut;
	while (ReadFile(hIn, buffer, BUFSIZ, &nIn, NULL) && nIn > 0)
	{
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut)
		{
			cout << "Failed to copy file: error code " << GetLastError();
			return EXIT_FAILURE;
		}
	}

	CloseHandle(hIn);
	CloseHandle(hOut);

	return EXIT_SUCCESS;
}
int copy_copyfile(char* src_file)
{
	int success = CopyFile(src_file, TARGET_FILE, FALSE);
	if (!success) cout << "Failed to copy file: error code " << GetLastError();
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
//
//double calculateAveragePoints()
//{
//	ifstream input(ORIGINAL_FILE, ios_base::in);
//
//	int count = 0;
//	float sum = 0;
//
//	int semester, points;
//	string discipline, teacher;
//	while (input >> semester >> discipline >> points >> teacher)
//	{
//		sum += points;
//		count++;
//	}
//
//	input.close();
//
//	float avg = sum / count;
//	if (!isnan(avg)) {
//		//cout << "Average points: " << avg << endl;
//		return avg;
//	}
//	else {
//		cout << "Incorrect file format; unable to parse points." << endl;
//		return 0;
//	}
//}
time_t execTime(int(*function)(char*), char* srcfile)
{
	auto startTime = steady_clock::now();
	int result = function(srcfile);
	if (result == EXIT_FAILURE) return -1;

	auto duration = duration_cast<microseconds>(steady_clock::now() - startTime);
	return duration.count();
}

string time_stringed(time_t a) {
	std::stringstream ss;
	ss << a;
	return ss.str();
}
string double_stringed(double a) {

	std::ostringstream strs;
	strs << a;
	return strs.str();
}
int main()
{
	time_t time_c1, time_windows1, time_copyfile1,
		time_c2, time_windows2, time_copyfile2;
	time_c1 = execTime(&copy_c, _strdup(ORIGINAL_FILE));
	time_windows1 = execTime(&copy_windows, _strdup(ORIGINAL_FILE));
	time_copyfile1 = execTime(&copy_copyfile, _strdup(ORIGINAL_FILE));

	time_c2 = execTime(&copy_c, _strdup(BIG_FILE));
	time_windows2 = execTime(&copy_windows, _strdup(BIG_FILE));
	time_copyfile2 = execTime(&copy_copyfile, _strdup(BIG_FILE));

	//average();
	std::ifstream b(ORIGINAL_FILE, std::ifstream::ate | std::ifstream::binary);
	double original_file_size = (double)b.tellg() / 1024;
	std::ifstream a(BIG_FILE, std::ifstream::ate | std::ifstream::binary);
	double bigger_file_size = (double)a.tellg() / 1024;

	TextTable t('-', '|', '+');
	t.add(""); t.add("SIZE (kb)"); t.add("C lib"); t.add("Windows"); t.add("CopyFile");
	t.endOfRow();

	t.add(ORIGINAL_FILE); t.add(double_stringed(original_file_size)); t.add(time_stringed(time_c1));
	t.add(time_stringed(time_windows1)); t.add(time_stringed(time_copyfile1));
	t.endOfRow();

	t.add(BIG_FILE); t.add(double_stringed(bigger_file_size)); t.add(time_stringed(time_c2));
	t.add(time_stringed(time_windows2)); t.add(time_stringed(time_copyfile2));
	t.endOfRow();
	cout << "\n" << t;
	char dsa;
	cin >> dsa;
	return 0;
}
