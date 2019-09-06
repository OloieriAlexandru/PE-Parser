#pragma once
#include "Windows.h"

#include "Composite.h"

const char* GetMachineType(_IMAGE_NT_HEADERS* imageFileHeader)
{
	switch (imageFileHeader->FileHeader.Machine)
	{
	case IMAGE_FILE_MACHINE_I386: return "Intel I386";
	case IMAGE_FILE_MACHINE_AMD64: return "AMD64";
	case IMAGE_FILE_MACHINE_IA64: return "MACHINE_IA64";
	case IMAGE_FILE_MACHINE_ARM: return "MACHINE_ARM";
	}
	return "UNKNOWN";
}

const char* GetFileType(_IMAGE_NT_HEADERS* imageFileHeader)
{
	if (imageFileHeader->FileHeader.Characteristics & IMAGE_FILE_DLL)
		return "DLL";
	if (imageFileHeader->FileHeader.Characteristics & IMAGE_FILE_SYSTEM)
		return "System File";
	if (imageFileHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE)
		return "Executable File";
}

void AddFileCreationDate(_IMAGE_NT_HEADERS* imageFileHeader, Composite *mzpeInfo)
{
	time_t rawtime = imageFileHeader->FileHeader.TimeDateStamp;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = gmtime(&rawtime);

	auto date = asctime(timeinfo);
	int len = strlen(date);
	for (int i=len-1;i>=0;--i)
		if (date[i] == '\n')
		{
			date[i] = 0;
			break;
		}

	mzpeInfo->Add(new DataPair("Created", date));
}