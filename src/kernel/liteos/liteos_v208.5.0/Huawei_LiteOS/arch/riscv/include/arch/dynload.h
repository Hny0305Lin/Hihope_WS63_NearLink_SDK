/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: RISCV Dynamic Load HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#ifndef _ARCH_DYNLOAD_H
#define _ARCH_DYNLOAD_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DYNLOAD_ALIGN_SIZE  64
#define UINTMAX             UINT32_MAX

/* Symbol table */
typedef struct {
    UINT32 stName;  /* Symbol table name (string tbl index) */
    UINT32 stValue; /* Symbol table value */
    UINT32 stSize;  /* Symbol table size */
    UINT8 stInfo;   /* Symbol table type and binding */
    UINT8 stOther;  /* Symbol table visibility */
    UINT16 stShndx; /* Section table index */
} LDElf32Sym;

#define LD_ELF_SYM             LDElf32Sym

/* Section header */
typedef struct {
    UINT32 shName;      /* Section name (string tbl index) */
    UINT32 shType;      /* Section type */
    UINT32 shFlags;     /* Section flags */
    UINT32 shAddr;      /* Section virtual addr at execution */
    UINT32 shOffset;    /* Section file offset */
    UINT32 shSize;      /* Section size in bytes */
    UINT32 shLink;      /* Link to another section */
    UINT32 shInfo;      /* Additional section information */
    UINT32 shAddrAlign; /* Section alignment */
    UINT32 shEntSize;   /* Entry size if section holds table */
} LDElf32Shdr;

#define LD_ELF_SHDR            LDElf32Shdr

/* Program Header */
typedef struct {
    UINT32 type;     /* Segment type */
    UINT32 offset;   /* Segment file offset */
    UINT32 vAddr;    /* Segment virtual address */
    UINT32 phyAddr;  /* Segment physical address */
    UINT32 fileSize; /* Segment size in file */
    UINT32 memSize;  /* Segment size in memory */
    UINT32 flags;    /* Segment flags */
    UINT32 align;    /* Segment alignment */
} LDElf32Phdr;

#define LD_ELF_PHDR            LDElf32Phdr

/* Elf header */
#define LD_EI_NIDENT           16
typedef struct {
    UINT8       elfIdent[LD_EI_NIDENT]; /* Magic number and other info */
    UINT16      elfType;                /* Object file type */
    UINT16      elfMachine;             /* Architecture */
    UINT32      elfVersion;             /* Object file version */
    UINT32      elfEntry;               /* Entry point virtual address */
    UINT32      elfPhoff;               /* Program header table file offset */
    UINT32      elfShoff;               /* Section header table file offset */
    UINT32      elfFlags;               /* Processor-specific flags */
    UINT16      elfHeadSize;            /* ELF header size in bytes */
    UINT16      elfPhSize;              /* Program header table entry size */
    UINT16      elfPhNum;               /* Program header table entry count */
    UINT16      elfShEntSize;           /* Section header table entry size */
    UINT16      elfShNum;               /* Section header table entry count */
    UINT16      elfShStrIndex;          /* Section header string table index */
} LDElf32Ehdr;

#define LD_ELF_EHDR            LDElf32Ehdr

/* EI_CLASS */
#define LD_ELF_CLASS32         1
#define LD_ELF_CLASS           LD_ELF_CLASS32

/* e_machine */
#define LD_EM_RISCV            243   /* RISC_V */
#define LD_EM_TYPE             LD_EM_RISCV

/* Dynamic */
typedef struct {
    UINT32 dynTag;  /* Dynamic entry type */
    union {
        UINT32 val; /* Integer value */
        UINT32 ptr; /* Address value */
    } dyn;
} LDElf32Dyn;

#define LD_ELF_DYN             LDElf32Dyn

/* Relocation */
typedef struct {
    UINT32 offset; /* Address */
    UINT32 info;   /* Relocation type and symbol index */
} LDElf32Rel;

typedef struct {
    UINT32 offset; /* Address */
    UINT32 info;   /* Relocation type and symbol index */
    INT32 addend;  /* Addend */
} LDElf32Rela;

#define LD_ELF_REL             LDElf32Rel
#define LD_ELF_RELA            LDElf32Rela

#define LD_ELF_R_SYM(info) ((info) >> 8)
#define LD_ELF_R_TYPE(info) ((info) & 0xFFU)
#define LD_ELF_R_INFO(sym, type) (((sym) << 8) + (UINT8)(type))

/* Dynamic linker uses the following 12 relocation types */
#define OS_R_RISCV_NONE            0
#define OS_R_RISCV_32              1
#define OS_R_RISCV_64              2
#define OS_R_RISCV_RELATIVE        3
#define OS_R_RISCV_COPY            4
#define OS_R_RISCV_JUMP_SLOT       5
#define OS_R_RISCV_TLS_DTPMOD32    6
#define OS_R_RISCV_TLS_DTPMOD64    7
#define OS_R_RISCV_TLS_DTPREL32    8
#define OS_R_RISCV_TLS_DTPREL64    9
#define OS_R_RISCV_TLS_TPREL32     10
#define OS_R_RISCV_TLS_TPREL64     11

/* Dynamic linker doesn't use the following relocation types */
#define OS_R_RISCV_BRANCH          16
#define OS_R_RISCV_JAL             17
#define OS_R_RISCV_CALL            18
#define OS_R_RISCV_CALL_PLT        19
#define OS_R_RISCV_GOT_HI20        20
#define OS_R_RISCV_TLS_GOT_HI20    21
#define OS_R_RISCV_TLS_GD_HI20     22
#define OS_R_RISCV_PCREL_HI20      23
#define OS_R_RISCV_PCREL_LO12_I    24
#define OS_R_RISCV_PCREL_LO12_S    25
#define OS_R_RISCV_HI20            26
#define OS_R_RISCV_LO12_I          27
#define OS_R_RISCV_LO12_S          28
#define OS_R_RISCV_TPREL_HI20      29
#define OS_R_RISCV_TPREL_LO12_I    30
#define OS_R_RISCV_TPREL_LO12_S    31
#define OS_R_RISCV_TPREL_ADD       32
#define OS_R_RISCV_ADD8            33
#define OS_R_RISCV_ADD16           34
#define OS_R_RISCV_ADD32           35
#define OS_R_RISCV_ADD64           36
#define OS_R_RISCV_SUB8            37
#define OS_R_RISCV_SUB16           38
#define OS_R_RISCV_SUB32           39
#define OS_R_RISCV_SUB64           40
#define OS_R_RISCV_GNU_VTINHERIT   41
#define OS_R_RISCV_GNU_VTENTRY     42
#define OS_R_RISCV_ALIGN           43
#define OS_R_RISCV_RVC_BRANCH      44
#define OS_R_RISCV_RVC_JUMP        45
#define OS_R_RISCV_RVC_LUI         46
#define OS_R_RISCV_RELAX           51
#define OS_R_RISCV_GP_HI120        59
#define OS_R_RISCV_GP_LO12_I       60
#define OS_R_RISCV_GP_LO12_S       61

typedef struct {
    UINT32 relocType;
    UINT32 shType;        /* REL section or RELA section. */
    UINTPTR position;     /* the address of the place being relocated. */
    UINTPTR symAdd;       /* the address of the symbol. */
    ssize_t addend;       /* the addend for the relocation. */
    UINTPTR baseSegment;  /* the addressing origin of the output segment defining the symbol S. */
    UINTPTR gotAddr;      /* the address of the Global Offset Table. */
    /*
     * T is 1 if the target symbol S has type STT_FUNC and
     * the symbol addresses a Thumb instruction; it is 0 otherwise.
     */
    UINT8 thumbFunc;
#define RESERVE_NUM 7
    UINT8 reserved[RESERVE_NUM];
} LDRelocParam;

typedef struct {
    UINTPTR relTable;     /* address of relocation table. */
    UINTPTR relTableSize; /* total size of relocation table. */
    UINTPTR relEntrySize; /* size of every relocation entry. */
} RelocationInfo;

extern UINT32 ELF_RiscvRel32Reloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvJumpSlotReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvRelativeReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvJalReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvBranchReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvCallReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvHigh20Reloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvLow12IReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvLow12SReloc(const LDRelocParam *relocParam);
extern UINT32 ELF_RiscvAddReloc(const LDRelocParam *relocParam, UINT32 type);
extern UINT32 ELF_RiscvSubReloc(const LDRelocParam *relocParam, UINT32 type);
extern UINT32 ArchSegRelocImp(const LD_ELF_RELA *rela, const LDRelocParam *relocParam);
extern UINT32 ArchObjRelocImp(const LD_ELF_RELA *relocCmd, const LDRelocParam *relocParam);
extern UINT32 ArchSegRelocTblCheck(const RelocationInfo *relocInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_DYNLOAD_H */
