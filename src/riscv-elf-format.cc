//
//  riscv-elf-format.cc
//

#include <cstdint>
#include <vector>
#include <string>

#include "riscv-elf.h"
#include "riscv-elf-file.h"
#include "riscv-elf-format.h"
#include "riscv-util.h"

const char* elf_ei_class_name(int v)
{
	switch (v) {
		case ELFCLASS32: return "ELF32";
		case ELFCLASS64: return "ELF64";
		default: return "unknown";
	};
}

const char* elf_ei_data_name(int v)
{
	switch (v) {
		case ELFDATA2LSB: return "little-endian";
		case ELFDATA2MSB: return "big-endian";
		default: return "unknown";
	};
}

const char* elf_e_type_name(int v)
{
	switch (v) {
		case ET_NONE: return "none";
		case ET_REL: return "relocatable object";
		case ET_EXEC: return "executable";
		case ET_DYN: return "shared object";
		case ET_CORE: return "core";
		default: return "unknown";
	};
};

const char* elf_e_machine_name(int v)
{
	switch (v) {
		case EM_NONE: return "none";
		case EM_X86_64: return "x86-64";
		case EM_RISCV: return "RISC-V";
		default: return "unknown";
	}
};

const char* elf_p_type_name(int v)
{
	switch (v) {
		case PT_NULL: return "NULL";
		case PT_LOAD: return "LOAD";
		case PT_DYNAMIC: return "DYNAMIC";
		case PT_INTERP: return "INTERP";
		case PT_NOTE: return "NOTE";
		case PT_SHLIB: return "SHLIB";
		case PT_PHDR: return "PHDR";
		default: return "UNKNOWN";
	}
}

const std::string elf_p_flags_name(int v)
{
	std::string s;
	if (v & PF_X) s+= "+X";
	if (v & PF_W) s+= "+W";
	if (v & PF_R) s+= "+R";
	return s;
}

const char* elf_sh_type_name(int v)
{
	switch (v) {
		case SHT_NULL: return "NULL";
		case SHT_PROGBITS: return "PROGBITS";
		case SHT_SYMTAB: return "SYMTAB";
		case SHT_STRTAB: return "STRTAB";
		case SHT_RELA: return "RELA";
		case SHT_HASH: return "HASH" ;
		case SHT_DYNAMIC: return "DYNAMIC";
		case SHT_NOTE: return "NOTE";
		case SHT_NOBITS: return "NOBITS";
		case SHT_REL: return "REL";
		case SHT_SHLIB: return "SHLIB";
		case SHT_DYNSYM: return "DYNSYM";
		default: return "UNKNOWN";
	}
};

const std::string elf_sh_flags_name(int v)
{
	std::string s;
	if (v & SHF_WRITE) s+= "+WRITE";
	if (v & SHF_ALLOC) s+= "+ALLOC";
	if (v & SHF_EXECINSTR) s+= "+EXEC";
	return s;
};

const char* elf_st_info_name(int v)
{
	switch (v) {
		case STT_NOTYPE: return "NOTYPE";
		case STT_OBJECT: return "OBJECT";
		case STT_FUNC: return "FUNC";
		case STT_SECTION: return "SECTION";
		case STT_FILE: return "FILE";
		default: return "UNKNOWN";
	}
};

const std::string elf_file_info(elf_file &elf)
{
	return format_string("%s: %s %s %s %s",
		elf.filename.c_str(),
		elf_ei_class_name(elf.ei_class),
		elf_ei_data_name(elf.ei_data),
		elf_e_machine_name(elf.ehdr.e_machine),
		elf_e_type_name(elf.ehdr.e_type));
}

const std::string elf_phdr_info(Elf64_Phdr &phdr)
{
	return format_string("%10s %20s 0x%016x 0x%016x 0x%016x 0x%016x %lu",
		elf_p_type_name(phdr.p_type),
		elf_p_flags_name(phdr.p_flags).c_str(),
		phdr.p_vaddr,
		phdr.p_paddr,
		phdr.p_filesz,
		phdr.p_memsz,
		phdr.p_align);
}

const std::string elf_shdr_info(Elf64_Shdr &shdr)
{
	return format_string("%10s %20s 0x%016x 0x%016x 0x%016x",
		elf_sh_type_name(shdr.sh_type),
		elf_sh_flags_name(shdr.sh_flags).c_str(),
		shdr.sh_addr,
		shdr.sh_offset,
		shdr.sh_size);
}

void elf_print_info(elf_file &elf)
{
	printf("%s\n\n", elf_file_info(elf).c_str());
	printf("                   %10s %20s %18s %18s %18s\n",
		"Type", "Flags", "Addr", "Offset", "Size");
	for (size_t i = 0; i < elf.shdrs.size(); i++) {
		printf("section header[%02lu] %s\n", i, elf_shdr_info(elf.shdrs[i]).c_str());
	}
	printf("\n");
	printf("                   %10s %20s %18s %18s %18s %18s %s\n",
		"Type", "Flags", "VirtAddr", "PhysAddr", "FileSize", "MemSize", "Align");
	for (size_t i = 0; i < elf.phdrs.size(); i++) {
		printf("program header[%02lu] %s\n", i, elf_phdr_info(elf.phdrs[i]).c_str());
	}
	printf("\n");
	printf("EntryAddr: 0x%016llx\n\n", elf.ehdr.e_entry);
}
