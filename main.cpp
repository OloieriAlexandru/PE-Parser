#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include "header files/DataGroup.h"
#include "header files/DataPair.h"
#include "header files/ExportEntryWithName.h"
#include "header files/ExportEntryWithoutName.h"
#include "header files/ImportEntryDll.h"
#include "header files/ImportEntryDllFunction.h"
#include "header files/ResourceDataBuffer.h"
#include "header files/ResourceEntry.h"

#include "header files/ConstantMatch.h"
#include "header files/Vector.h"
#include "header files/Pair.h"
#include "header files/BitSet.h"

int EXPORT_INDEX = 0;
int IMPORT_INDEX = 1;
int RESOURCES_INDEX = 2;
int INDEXES_OFFSET = 0;

Vector<Pair<unsigned,Pair<unsigned, unsigned>>> sections;

void RenameFilePath(char* filePath);
void CheckIfValidArchitectureParameter(char* architectureParameter);
bool Is64BitFile(char* architectureParameter);

FILE* OpenFile(char* filePath);

void ReportError(const char* message);

void ExtractSectionsAddresses(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, _IMAGE_DOS_HEADER* header);
int GetFileOffsetOfVirtualAddress(DWORD virtualAddress);

void ParseFile(FILE* file, DataGroup* mzpeInfo);
void ParseImageFileHeader(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo);

int GetAddressOfFunctionByOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, short functionOrdinal);
int GetAddressOfFunctionWithNameByOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, short functionOrdinal);
short ParseExportedFunctionOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, int functionIndex);
void ParseExportedFunctionWithName(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, int functionIndex, Composite* mzpeInfo, BitSet* bitSet);
void ParseExports(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo);

void ParseDllName(FILE* file, int dllNameAddress, char** buffer);
void ParseDllImportedFunctions(FILE* file, _IMAGE_IMPORT_DESCRIPTOR* imageImport, Composite* mzpeInfo);
void ParseFunctionName(FILE* file, int functionNameAddress, Composite* mzpeInfo, char** buffer);
void ParseDllInfo(FILE* file, _IMAGE_IMPORT_DESCRIPTOR* imageImport, int dllNameAddress, Composite* mzpeInfo);
void ParseImports(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, _IMAGE_DOS_HEADER* header, Composite* mzpeInfo);

void ResourceNamedEntriesPutDataInBuffer(FILE* file, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY* directoryEntry, int resourceSectionStartAddress, int level);
void ParseNamedDirectoryEntries(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY* resourceDirectory, int resourceSectionStartAddress, int level);
void ParseResourceLeafEntry(FILE* file, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY* directoryEntry, int resourceSectionStartAddress);
void ResourceIdEntriesPutDataInBuffer(ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY* directoryEntry, int level);
void ParseIdDirectoryEntries(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY* resourceDirectory, int resourceSectionStartAddress, int level);
void ParseResourceDirectory(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer* resourceDataBuffer, int resourceSectionStartAddress, int level);
void ParseResources(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo);

void PrintMzpeInfo(Composite* mzpeInfo, char *fileName);

int main(int argc, char* argv[])
{
	if (argc < 2)
		ReportError("You have to provide the path to a file"); //and for what architecture it was compiled(x86 / x64)");

	RenameFilePath(argv[1]);
	/*CheckIfValidArchitectureParameter(argv[2]);
	if (Is64BitFile(argv[2]))
		INDEXES_OFFSET = 2;*/

	DataGroup mzpeInfo("MZPE Information:\n");

	FILE* file = OpenFile(argv[1]);

	ParseFile(file, &mzpeInfo);

	PrintMzpeInfo(&mzpeInfo, argv[1]);
	return 0;
}

void ReportError(const char* message)
{
	printf("%s\n", message);
	exit(0);
}

void TryToSeekSet(FILE *file, int offset)
{
	int res = fseek(file, offset, SEEK_SET);
	if (res)
		ReportError("Seek error!\n");
}

void RenameFilePath(char* filePath)
{
	for (int i = 0; *(filePath + i); ++i)
		if (*(filePath + i) == '_')
			* (filePath + i) = ' ';
}

void CheckIfValidArchitectureParameter(char* architectureParameter)
{
	int ln = strlen(architectureParameter);
	if (ln == 1 || ln > 3) ReportError("Invalid Architecture Parameter - Accepted: 86/x86/64/x64");
	for (int i = 0; i < ln; ++i)
		if (architectureParameter[i] >= 'A' && architectureParameter[i] <= 'Z')
			architectureParameter -= 'A', architectureParameter += 'a';
	if (ln == 2)
	{
		if (strcmp(architectureParameter, "86") && strcmp(architectureParameter, "64")) ReportError("Invalid Architecture Parameter - Accepted: 86/x86/64/x64");
		else return;
	}
	
	if (strcmp(architectureParameter, "x86") && strcmp(architectureParameter, "x64")) ReportError("Invalid Architecture Parameter - Accepted: 86/x86/64/x64");
}

bool Is64BitFile(char* architectureParameter)
{
	int ln = strlen(architectureParameter);
	if (ln == 2) return (!strcmp(architectureParameter, "64"));
	return (!strcmp(architectureParameter, "x64"));
}

FILE* OpenFile(char* filePath)
{
	FILE* fin = fopen(filePath, "rb");
	if (fin == NULL)
	{
		printf("Invalid path!\n");
		exit(0);
	}
	return fin;
}

void ExtractSectionsAddresses(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, _IMAGE_DOS_HEADER* header)
{
	_IMAGE_SECTION_HEADER currentSection;
	int sectionsStartAddress = header->e_lfanew + 24 + imageFileHeader->FileHeader.SizeOfOptionalHeader;

	TryToSeekSet(file, sectionsStartAddress);

	for (int i = 0; i < imageFileHeader->FileHeader.NumberOfSections; ++i)
	{
		fread(&currentSection, sizeof(_IMAGE_SECTION_HEADER), 1, file);
		sections.Add(Pair<unsigned, Pair<unsigned, unsigned>>(currentSection.PointerToRawData, Pair<unsigned, unsigned>(currentSection.VirtualAddress, currentSection.SizeOfRawData)));
	}
}

int GetFileOffsetOfVirtualAddress(DWORD virtualAddress)
{
	auto it = sections.begin();

	while (it != sections.end())
	{
		if ((*it).second.first <= virtualAddress && virtualAddress <= (*it).second.first + (*it).second.second)
			return (virtualAddress - (*it).second.first) + (*it).first;
		++it;
	}
	
	return -1;
}

void ParseFile(FILE* file, DataGroup* mzpeInfo)
{
	_IMAGE_DOS_HEADER header;
	fread(&header, sizeof(_IMAGE_DOS_HEADER), 1, file);

	_IMAGE_NT_HEADERS imageFileHeader;

	TryToSeekSet(file, header.e_lfanew);
	fread(&imageFileHeader, sizeof(_IMAGE_NT_HEADERS), 1, file);

	ExtractSectionsAddresses(file, &imageFileHeader, &header);

	Composite* fileInformation = new DataGroup("Information: ");
	mzpeInfo->Add(fileInformation);
	ParseImageFileHeader(file, &imageFileHeader, fileInformation);
	
	Composite* exports = new DataGroup("Exports: ");
	mzpeInfo->Add(exports);
	ParseExports(file, &imageFileHeader, exports);

	Composite* imports = new DataGroup("Imports: ");
	mzpeInfo->Add(imports);
	ParseImports(file, &imageFileHeader, &header, imports);

	Composite* resources = new DataGroup("Resources: ");
	mzpeInfo->Add(resources);
	ParseResources(file, &imageFileHeader, resources);
}

void ParseImageFileHeader(FILE *file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo)
{	
	mzpeInfo->Add(new DataPair("Machine", GetMachineType(imageFileHeader)));
	mzpeInfo->Add(new DataPair("Type", GetFileType(imageFileHeader)));
	AddFileCreationDate(imageFileHeader, mzpeInfo);

	if (!(imageFileHeader->FileHeader.Characteristics & IMAGE_FILE_32BIT_MACHINE))
	{
		INDEXES_OFFSET = 2;
		mzpeInfo->Add(new DataPair("Architecture", "64 bit\n"));
	}
	else
		mzpeInfo->Add(new DataPair("Architecture", "32 bit\n"));
}

int GetAddressOfFunctionByOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, short functionOrdinal)
{
	int pointerAddressOfFunctions = GetFileOffsetOfVirtualAddress(imageExport->AddressOfFunctions + 4 * functionOrdinal);
	if (pointerAddressOfFunctions == -1)
		return -1;

	TryToSeekSet(file, pointerAddressOfFunctions);

	int addressOfFunction;
	fread(&addressOfFunction, sizeof(addressOfFunction), 1, file);

	return addressOfFunction;
}

void ParseFunctionName(FILE* file, int functionNameAddress, Composite* mzpeInfo, char** buffer)
{
	Vector<char>functionName;
	char currChar;
	TryToSeekSet(file, functionNameAddress);
	while (1)
	{
		fread(&currChar, 1, 1, file);
		if (!currChar) break;
		functionName.Add(currChar);
	}
	functionName.GetVectorAsCharArray(buffer);
}

int GetAddressOfFunctionWithNameByOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, short functionOrdinal)
{
	return GetAddressOfFunctionByOrdinal(file, imageExport, functionOrdinal - (short)imageExport->Base);
}

short ParseExportedFunctionOrdinal(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, int functionIndex)
{
	int pointerAddressToOrdinal = GetFileOffsetOfVirtualAddress(imageExport->AddressOfNameOrdinals + 2 * functionIndex);
	if (pointerAddressToOrdinal == -1)
		return -1;

	TryToSeekSet(file, pointerAddressToOrdinal);

	short ordinal;
	fread(&ordinal, sizeof(ordinal), 1, file);

	return ordinal + (short)imageExport->Base;
}

void ParseExportedFunctionWithName(FILE* file, _IMAGE_EXPORT_DIRECTORY* imageExport, int functionIndex, Composite* mzpeInfo, BitSet *bitSet)
{
	int pointerAddressToName = GetFileOffsetOfVirtualAddress(imageExport->AddressOfNames + 4 * functionIndex);
	if (pointerAddressToName == -1) return;

	TryToSeekSet(file, pointerAddressToName);
	
	int addressOfName;
	fread(&addressOfName, sizeof(addressOfName), 1, file);
	addressOfName = GetFileOffsetOfVirtualAddress(addressOfName);

	char* functionName;
	ParseFunctionName(file, addressOfName, mzpeInfo, &functionName);
	short functionOrdinal = ParseExportedFunctionOrdinal(file, imageExport, functionIndex);
	int functionAddress = GetAddressOfFunctionWithNameByOrdinal(file, imageExport, functionOrdinal);

	mzpeInfo->Add(new ExportEntryWithName(functionName, functionAddress, (int)functionOrdinal));

	delete[] functionName;
	bitSet->Set(functionOrdinal - (short)imageExport->Base);
}

void ParseExports(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo)
{
	_IMAGE_EXPORT_DIRECTORY imageExport;

	int realAddress = GetFileOffsetOfVirtualAddress(imageFileHeader->OptionalHeader.DataDirectory[EXPORT_INDEX + INDEXES_OFFSET].VirtualAddress);
	if (realAddress == -1) return;

	TryToSeekSet(file, realAddress);

	fread(&imageExport, sizeof(_IMAGE_EXPORT_DIRECTORY), 1, file);

	BitSet ordinalsOfFunctionsWithName(imageExport.NumberOfFunctions);

	for (int i = 0; i < (int)imageExport.NumberOfNames; ++i)
		ParseExportedFunctionWithName(file, &imageExport, i, mzpeInfo, &ordinalsOfFunctionsWithName);
	
	for (int i = 0; i < (int)imageExport.NumberOfFunctions; ++i)
		if (!ordinalsOfFunctionsWithName.Test(i))
		{
			int functionAddress = GetAddressOfFunctionByOrdinal(file, &imageExport, (short)i);
			if (!functionAddress || functionAddress == -1) continue;
			mzpeInfo->Add(new ExportEntryWithoutName(functionAddress, i));
		}
}

void ParseDllName(FILE* file, int dllNameAddress, char **buffer)
{
	Vector<char>dllName;
	char currCh;
	TryToSeekSet(file, dllNameAddress);
	while (1)
	{
		fread(&currCh, 1, 1, file);
		if (!currCh) break;
		dllName.Add(currCh);
	}
	dllName.GetVectorAsCharArray(buffer);
}

void ParseDllImportedFunctions(FILE* file, _IMAGE_IMPORT_DESCRIPTOR* imageImport, Composite* mzpeInfo)
{
	int functionAddress = GetFileOffsetOfVirtualAddress(imageImport->Characteristics);
	if (functionAddress == -1) return;
	int functionRVA = imageImport->FirstThunk;
	int functionNameVirtualAddress, functionNameAddress;
	bool oneMoreFunctionAddressExists = true;
	while (oneMoreFunctionAddressExists)
	{
		TryToSeekSet(file, functionAddress);
		fread(&functionNameVirtualAddress, 4, 1, file);
		functionNameAddress = GetFileOffsetOfVirtualAddress(functionNameVirtualAddress);

		if (functionNameAddress == -1)
		{
			oneMoreFunctionAddressExists = false;
			continue;
		}

		char* functionName;
		ParseFunctionName(file, functionNameAddress + 2, mzpeInfo, &functionName);
		mzpeInfo->Add(new ImportEntryDllFunction(functionName, functionRVA));
		delete[] functionName;

		functionAddress += 4;
		functionRVA += 4;
	}
}

void ParseDllInfo(FILE* file, _IMAGE_IMPORT_DESCRIPTOR* imageImport, int dllNameAddress, Composite* mzpeInfo)
{
	char* dllName = NULL;
	ParseDllName(file, dllNameAddress, &dllName);
	ImportEntryDll* importedDll = new ImportEntryDll(dllName);
	mzpeInfo->Add(importedDll);
	delete[] dllName;
	ParseDllImportedFunctions(file, imageImport, importedDll);
}

void ParseImports(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, _IMAGE_DOS_HEADER* header, Composite* mzpeInfo)
{
	_IMAGE_IMPORT_DESCRIPTOR imageImport;
	bool oneMoreImageImportDescriptorExists = true;

	int realAddress = GetFileOffsetOfVirtualAddress(imageFileHeader->OptionalHeader.DataDirectory[IMPORT_INDEX + INDEXES_OFFSET].VirtualAddress);
	if (realAddress == -1) return;
		
	while (oneMoreImageImportDescriptorExists)
	{
		TryToSeekSet(file, realAddress);

		fread(&imageImport, sizeof(_IMAGE_IMPORT_DESCRIPTOR), 1, file);

		int dllNameAddress = GetFileOffsetOfVirtualAddress(imageImport.Name);
		if (dllNameAddress == -1)
		{
			oneMoreImageImportDescriptorExists = false;
			continue;
		}

		ParseDllInfo(file, &imageImport, dllNameAddress, mzpeInfo);

		realAddress += sizeof(_IMAGE_IMPORT_DESCRIPTOR);
	}
}

void ResourceNamedEntriesPutDataInBuffer(FILE *file, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY* directoryEntry, int resourceSectionStartAddress, int level)
{
	
}

void ParseNamedDirectoryEntries(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY* resourceDirectory, int resourceSectionStartAddress, int level)
{
	IMAGE_RESOURCE_DIRECTORY_ENTRY directoryEntry;

	for (int i = 0; i < resourceDirectory->NumberOfNamedEntries; ++i)
	{
		TryToSeekSet(file, directoryAddress + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
		fread(&directoryEntry, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY), 1, file);

		ResourceIdEntriesPutDataInBuffer(resourceDataBuffer, &directoryEntry, level);

		if (directoryEntry.OffsetToData & 0x80000000)
			ParseResourceDirectory(file, resourceSectionStartAddress + directoryEntry.OffsetToDirectory, composite, resourceDataBuffer, resourceSectionStartAddress, level + 1);
		else
			ParseResourceLeafEntry(file, composite, resourceDataBuffer, &directoryEntry, resourceSectionStartAddress);
	}
}

void ParseResourceLeafEntry(FILE* file, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY *directoryEntry, int resourceSectionStartAddress)
{
	IMAGE_RESOURCE_DATA_ENTRY data;
	TryToSeekSet(file, resourceSectionStartAddress + directoryEntry->OffsetToData);
	fread(&data, sizeof(IMAGE_RESOURCE_DATA_ENTRY), 1, file);
	int realAddress = GetFileOffsetOfVirtualAddress(data.OffsetToData);

	TryToSeekSet(file, realAddress);
	Vector<char>resourceInfo;
	char* resourceInfoCharArray, ch;
	for (int i = 0; i < (int)data.Size; ++i)
	{
		fread(&ch, 1, 1, file);
		if (ch) resourceInfo.Add(ch);
	}
	resourceInfo.GetVectorAsCharArray(&resourceInfoCharArray);

	char* typeAddress = NULL;
	char* nameAddress = NULL;
	if (resourceDataBuffer->type) typeAddress = *(resourceDataBuffer->type);
	if (resourceDataBuffer->name) nameAddress = *(resourceDataBuffer->name);
	composite->Add(new ResourceEntry
	(typeAddress, resourceDataBuffer->idType,
		nameAddress, resourceDataBuffer->idName,
		data.Size, realAddress, directoryEntry->Id));

	delete[] resourceInfoCharArray;
}

void ResourceIdEntriesPutDataInBuffer(ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY_ENTRY* directoryEntry, int level)
{
	if (level == 1)
		resourceDataBuffer->idType = directoryEntry->Id, resourceDataBuffer->type = NULL;
	else
	if (level == 2)
		resourceDataBuffer->idName = directoryEntry->Id, resourceDataBuffer->name = NULL;
}

void ParseIdDirectoryEntries(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer* resourceDataBuffer, IMAGE_RESOURCE_DIRECTORY* resourceDirectory, int resourceSectionStartAddress, int level)
{
	IMAGE_RESOURCE_DIRECTORY_ENTRY directoryEntry;
	
	for (int i = resourceDirectory->NumberOfNamedEntries; i < resourceDirectory->NumberOfNamedEntries + resourceDirectory->NumberOfIdEntries; ++i)
	{
		TryToSeekSet(file, directoryAddress + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY));
		fread(&directoryEntry, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY), 1, file);

		ResourceIdEntriesPutDataInBuffer(resourceDataBuffer, &directoryEntry, level);

		if (directoryEntry.OffsetToData & 0x80000000)
			ParseResourceDirectory(file, resourceSectionStartAddress + directoryEntry.OffsetToDirectory, composite, resourceDataBuffer, resourceSectionStartAddress, level + 1);
		else
			ParseResourceLeafEntry(file, composite, resourceDataBuffer, &directoryEntry, resourceSectionStartAddress);
	}
}

void ParseResourceDirectory(FILE* file, int directoryAddress, Composite* composite, ResourceDataBuffer *resourceDataBuffer, int resourceSectionStartAddress, int level)
{
	IMAGE_RESOURCE_DIRECTORY resourceDirectory;

	TryToSeekSet(file, directoryAddress);
	fread(&resourceDirectory, sizeof(IMAGE_RESOURCE_DIRECTORY), 1, file);

	ParseNamedDirectoryEntries(file, directoryAddress, composite, resourceDataBuffer, &resourceDirectory, resourceSectionStartAddress, level);
	ParseIdDirectoryEntries(file, directoryAddress, composite, resourceDataBuffer, &resourceDirectory, resourceSectionStartAddress, level);
}

void ParseResources(FILE* file, _IMAGE_NT_HEADERS* imageFileHeader, Composite* mzpeInfo)
{
	int resourceDirectoryRootAddress = GetFileOffsetOfVirtualAddress(imageFileHeader->OptionalHeader.DataDirectory[RESOURCES_INDEX + INDEXES_OFFSET].VirtualAddress);
	if (resourceDirectoryRootAddress == -1) return;
	ResourceDataBuffer resourceDataBuffer;
	ParseResourceDirectory(file, resourceDirectoryRootAddress, mzpeInfo, &resourceDataBuffer,resourceDirectoryRootAddress, 1);
}

void PrintMzpeInfo(Composite* mzpeInfo, char* filePath)
{
	int ln = strlen(filePath), startIndex, endIndex;
	for (endIndex = ln - 1; endIndex >= 0; --endIndex)
		if (filePath[endIndex] == '.') break;
	if (endIndex == -1)
		startIndex = 0, endIndex = ln - 1;
	else
	{
		for (startIndex = endIndex - 1; startIndex >= 0; --startIndex)
			if (filePath[startIndex] == '\\')
				break;
		++startIndex, --endIndex;
	}
	ln = endIndex - startIndex + 1;
	
	char* newFileName = new char[ln + 18];
	memset(newFileName, 0, sizeof(newFileName));
	strncpy(newFileName, filePath + startIndex, ln);
	newFileName[ln] = 0;
	strcat(newFileName, "-mzpe-parsed.txt");

	std::ofstream fileOut(newFileName);
	mzpeInfo->PrintInfoToStream(fileOut);
	fileOut.close();

	delete[] newFileName;
}