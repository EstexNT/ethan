#include <stdio.h>
#include <stdlib.h>
#include "pe.hpp"
#include "memory.hpp"


namespace PE {

FILE *gPEFile = nullptr;

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
}

// resolvers go here

extern ResolvedImport ResolveMSVCR80(char *name);



namespace PE {

static struct {
    char *name;
    ResolvedImport (*fn)(char *name);
} supportedDlls[] = {
    { "MSVCR80.dll", ResolveMSVCR80 },
};

static void ResolveByName(char *entryName, char *importName, Ia64Addr outAddr) {
    for (int i = 0; i < NELEM(supportedDlls); i++) {
        if (strcmp(supportedDlls[i].name, entryName) == 0) {
            Ia64Addr fnAddr = supportedDlls[i].fn(importName).addr;
            Memory::WriteAt<Ia64Addr>(&fnAddr, outAddr);
        }
    }
}

void ParseImports(void) {
    uint32_t importDescAddr = PE::GetNt()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    IMAGE_IMPORT_DESCRIPTOR importDescriptor;
    int j = 0;

    while (true) {
        Memory::ReadAt<IMAGE_IMPORT_DESCRIPTOR>(&importDescriptor, importDescAddr + GetNt()->OptionalHeader.ImageBase + sizeof(IMAGE_IMPORT_DESCRIPTOR) * j);
        if (importDescriptor.Name == 0) {
            break;
        }
    
        char *entryName = Memory::ReadString(importDescriptor.Name + GetNt()->OptionalHeader.ImageBase);

        printf("%s:\n", entryName);

        IMAGE_THUNK_DATA64 lookupTable;
        int i = 0;
        while (true) {
            Memory::ReadAt<IMAGE_THUNK_DATA64>(&lookupTable, importDescriptor.OriginalFirstThunk + GetNt()->OptionalHeader.ImageBase + i * sizeof(IMAGE_THUNK_DATA64));
            if (lookupTable.u1.AddressOfData == 0) {
                break;
            }
            if (IMAGE_SNAP_BY_ORDINAL64(lookupTable.u1.Ordinal)) {
                fprintf(stderr, "unsupported import type (ordinal); dying\n");
                exit(1);
            } else {
                IMAGE_IMPORT_BY_NAME byName;
                Memory::ReadAt<IMAGE_IMPORT_BY_NAME>(&byName, lookupTable.u1.AddressOfData + GetNt()->OptionalHeader.ImageBase);
                char *importName = Memory::ReadString(
                    (Ia64Addr)((IMAGE_IMPORT_BY_NAME *)(lookupTable.u1.AddressOfData + GetNt()->OptionalHeader.ImageBase))->Name);
                printf("\t%s = 0x%04x\n", importName, byName.Hint);
                ResolveByName(entryName, importName, 
                            Ia64Addr(importDescriptor.FirstThunk + GetNt()->OptionalHeader.ImageBase + i * sizeof(Ia64Addr)));
                free(importName);
            }


            i++;
        }

        printf("\n");
        j++;
        free(entryName);
    }

}


} // PE

static struct ImportedStruct {
    char *dllName;
    char **funcNames;
    void **funcPtrs;
    Ia64Addr *funcAddrs;
} *imported = nullptr;
static ImportedStruct *FindImported(char *dllName) {
    for (int i = 0; (imported + i) != nullptr; i++) {
        if (strcmp(imported[i].dllName, dllName) == 0) {
            return &imported[i];
        }
    }
    return nullptr;
}

Ia64Addr MakeUpImportAddr(char *dllName, char *funcName) {
    static Ia64Addr importAddr = 0x70000000;
    static int count = 0;
    ImportedStruct *imp = nullptr;

    if (imported == nullptr) {
        imported = (ImportedStruct *)malloc(sizeof(ImportedStruct));
        imported[0].funcNames = (char **)malloc(sizeof(char *));
        imported[0].funcPtrs = (void **)malloc(sizeof(void *));
        imported[0].funcAddrs = (Ia64Addr *)malloc(sizeof(Ia64Addr));
        imp = &imported[0];
    } else {
        imp = FindImported(dllName);
        if (imp == nullptr) {
            count++;
            imported = (ImportedStruct *)realloc(imported, sizeof(ImportedStruct) * (1 + count));
            imported[count].dllName = strdup(dllName);
            imported[count].funcNames = (char **)malloc(sizeof(char *));
            imported[count].funcPtrs = (void **)malloc(sizeof(void *));
            imported[count].funcAddrs = (Ia64Addr *)malloc(sizeof(Ia64Addr));
        }
    }
    Ia64Addr out = importAddr;
    importAddr += 0x10;
    return out;
}

void CallImportedFunc(Ia64Addr addr) {
    return;
}
