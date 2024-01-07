#include <stdio.h>
#include <stdlib.h>
#include "pe.hpp"
#include "cpu.hpp"


namespace PE {

FILE *gPEFile = nullptr;
static Ia64Cpu *gCPU = nullptr;

static IMAGE_DOS_HEADER dosHeader = {0};
static IMAGE_NT_HEADERS64 ntHeaders64 = {0};
static IMAGE_SECTION_HEADER *sectionHeaders = {0};

bool Open(char *fname) {
    gPEFile = fopen(fname, "rb");
    return gPEFile != nullptr;
}

void Close(void) {
    free(sectionHeaders);
    fclose(gPEFile);
}

IMAGE_DOS_HEADER *ParseDos(void) {
    fseek(gPEFile, 0, SEEK_SET);
    fread(&dosHeader, sizeof(IMAGE_DOS_HEADER), 1, gPEFile);
    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE) {
        fprintf(stderr, "The input file is not a valid PE file!\n");
        return nullptr;
    }
    return &dosHeader;
}
IMAGE_DOS_HEADER *GetDos(void) {
    return &dosHeader;
}

IMAGE_NT_HEADERS64 *ParseNt(void) {
    fseek(gPEFile, dosHeader.e_lfanew, SEEK_SET);
    fread(&ntHeaders64, sizeof(IMAGE_NT_HEADERS64), 1, gPEFile);
    return &ntHeaders64;
}
IMAGE_NT_HEADERS64 *GetNt(void) {
    return &ntHeaders64;
}

static void PrintSection(IMAGE_SECTION_HEADER *sh) {
    for (int i = 0; i < 8; i++) {
        printf("%c", sh->Name[i]);
    }
    printf("\t 0x%08x", sh->VirtualAddress);
    printf("\n");
}

IMAGE_SECTION_HEADER *ParseSections(void) {
    uint64_t off = GetDos()->e_lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + GetNt()->FileHeader.SizeOfOptionalHeader;
    fseek(gPEFile, off, SEEK_SET);
    sectionHeaders = (IMAGE_SECTION_HEADER *)malloc(IMAGE_SIZEOF_SECTION_HEADER * ntHeaders64.FileHeader.NumberOfSections);
    printf("Sections: \n");
    for (int i = 0; i < ntHeaders64.FileHeader.NumberOfSections; i++) {
        printf("\t");
        fread(sectionHeaders + i, IMAGE_SIZEOF_SECTION_HEADER, 1, gPEFile);
        PrintSection(&sectionHeaders[i]);
    }
    return sectionHeaders;
}
IMAGE_SECTION_HEADER *GetSections(void) {
    return sectionHeaders;
}
IMAGE_SECTION_HEADER *FindSection(uint64_t addr) {
    for (int i = 0; i < ntHeaders64.FileHeader.NumberOfSections; i++) {
        IMAGE_SECTION_HEADER *section = &sectionHeaders[i];
        if (((section->VirtualAddress + ntHeaders64.OptionalHeader.ImageBase) <= addr)
             && ((section->VirtualAddress + section->SizeOfRawData + ntHeaders64.OptionalHeader.ImageBase) > addr)) {
            return section;
        }
    }
    return nullptr;
}

uint64_t GetSize(void) {
#if 0
    // TODO: don't assume that the PE will have this info laid out correctly 

    uint64_t size = GetNt()->OptionalHeader.SizeOfHeaders;

    for (int i = 0; i < GetNt()->FileHeader.NumberOfSections; i++) {
        size += GetSections()[i].SizeOfRawData;
    }

    size += GetNt()->OptionalHeader.SectionAlignment * GetNt()->FileHeader.NumberOfSections;

    return size;
#else
    return GetNt()->OptionalHeader.SizeOfImage;
#endif
}

void AttachCpu(Ia64Cpu *cpu) {
    gCPU = cpu;
}


} // PE