#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BIT(i) (1 << (i))

#define HLT BIT(7) // halt
#define MI BIT(6) // memory address register in
#define RI BIT(5) // ram in
#define RO BIT(4) // ram out
#define IO BIT(3) // instruction register out
#define II BIT(2) // instruction register in
#define AI BIT(1) // register A in
#define AO BIT(0) // register A out
#define EO BIT(15) // ALU out
#define SU BIT(14) // ALU subtract
#define BI BIT(13) // register B in
#define OI BIT(12) // output register in
#define CE BIT(11) // counter enable
#define CO BIT(10) // counter out
#define J BIT(9) // jump
#define FI BIT(8) // flags register in

#define INVERT (MI | RO | IO | II | AI | AO | EO | BI | CO | J | FI)
#define INSTRUCTION { EO | RI, RO | AI, EO | RI, RO | BI, EO | RI, RO | AI, EO | RI, RO | BI }

static const uint16_t microcode[64][8] = {
	/* NOP */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },

	/* HCF */
	{ MI | CO, RO | II | CE, HLT, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, HLT, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, HLT, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, HLT, 0, 0, 0, 0, 0 },

	/* OUT */
	{ MI | CO, RO | II | CE, AO | OI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | OI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | OI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | OI, 0, 0, 0, 0, 0 },

	/* unused */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },

	/* TIA */
	{ MI | CO, RO | II | CE, IO | AI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | AI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | AI, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | AI, 0, 0, 0, 0, 0 },
	
	/* unused */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },

	/* TRA */
	{ MI | CO, RO | II | CE, IO | MI, RO | AI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | AI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | AI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | AI, 0, 0, 0, 0 },

	/* TAR */
	{ MI | CO, RO | II | CE, IO | MI, AO | RI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, AO | RI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, AO | RI, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, AO | RI, 0, 0, 0, 0 },

	/* ADD */
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI, 0, 0, 0 },

	/* SUB */
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI | SU, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI | SU, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI | SU, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | MI, RO | BI, EO | AI | FI | SU, 0, 0, 0 },
	
	/* unused */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	
	/* unused */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	
	/* BRA */
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	
	/* BRR */
	{ MI | CO, RO | II | CE, AO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, AO | J, 0, 0, 0, 0, 0 },
	
	/* BCS */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	
	/* BZS */
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
	{ MI | CO, RO | II | CE, IO | J, 0, 0, 0, 0, 0 },
};

static const uint16_t microcode_nop[8] = { MI | CO, RO | II | CE, 0, 0, 0, 0, 0, 0 };

uint16_t get_microcode(unsigned int index)
{
	index = (index & 0xFFFFFF7E) | (index >> 7 & 1);
	if ((index & 0x400) == 0)
		return microcode[index / 256 + index / 8 % 16 * 4][index % 8] ^ INVERT;
	return microcode_nop[index % 8] ^ INVERT;
}

int main(void)
{
	FILE *ee0 = fopen("ee0.bin", "wb");
	FILE *ee1 = fopen("ee1.bin", "wb");

	unsigned int index;
	for (index = 0; index < 2048; ++index) {
		uint16_t code = get_microcode(index);
		fputc(code >> 0 & 0xFF, ee0);
		fputc(code >> 8 & 0xFF, ee1);
	}

	fclose(ee0);
	fclose(ee1);

	return EXIT_SUCCESS;
}
