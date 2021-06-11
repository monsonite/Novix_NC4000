/**************************************************************
* Bench - a simple testbench for the nc4k and related objects *
**************************************************************/

#include "bench.h"

void main(void) {
	char i;
	int n,q;

	cout<<"*** Bench 0.01 ***\n";
	prompt();

	do {
		cin>>i;
		switch (i) {
		case 'R' :
		case 'r' :	showreg(); // Show all registers
					prompt();
					break;
		case 'M' :
		case 'm' :	showmem(); // Show portion of memory
					prompt();
					break;
		case 'S' :
		case 's' :	cout<<"No. of instructions: ";
					cin>>n;
					for(q=0;q<n;q++) fp.step_instruction();
					prompt();
					break;
		case 'W' :	modifyreg();
					prompt();
					break;
		case 'w' :	modifymem();
					prompt();
					break;
		case '.' :	cout<<"Quitting...";
					break;
		default :	cout<<"? Unknown command: "<<i<<"\n";
					prompt();
					break;
		};
	} while (i!='.');
	return;
}

void prompt(void) {
	cout<<": ";
}

void showreg(void) {
	printf("  T |  N | Nc |  K |  I |  J |  M |  L |  P | MD | SR |\n");
	printf("%.4hX",fp.read_register(reg_T));
	printf("|");
	printf("%.4hX",fp.read_register(reg_N));
	printf("|");
	printf("%.4hX",fp.read_register(reg_carry));
	printf("|");
	printf("%.4hX",fp.read_register(reg_K));
	printf("|");
	printf("%.4hX",fp.read_register(reg_I));
	printf("|");
	printf("%.4hX",fp.read_register(reg_J));
	printf("|");
	printf("%.4hX",fp.read_register(reg_M));
	printf("|");
	printf("%.4hX",fp.read_register(reg_L));
	printf("|");
	printf("%.4hX",fp.read_register(reg_P));
	printf("|");
	printf("%.4hX",fp.read_register(reg_MD));
	printf("|");
	printf("%.4hX",fp.read_register(reg_SR));
	printf("|\n");
}

void showmem(void) {
	memory* sel;
	char whatmem;
	unit adr, length;
	cout<<"Choose: (d)ata stack, (r)eturn stack or (m)ain memory : ";
	cin>>whatmem;
	switch (whatmem) {
	case 'd' :
	case 'D' :	sel=&ds;
				break;
	case 'r' :
	case 'R' :	sel=&rs;
				break;
	case 'm' :
	case 'M' :	sel=&mem;
				break;
	default :	cout<<"Wrong input. Assuming data stack.\n";
				sel=&ds;
				break;
	}
	cout<<"Enter address and length (both in decimal) : ";
	cin>>adr>>length;
	while (length>=8) {
		printf("%.4hX",adr);
		printf(" ");
		for (int i=0;i<8;i++) {
			printf("%.4hX",sel->read(adr++));
			printf(" ");
			adr%=sel->get_size();
		}
		length-=8;
		printf("\n");
	}
	for (int i=0;i<length;i++) {
		printf("%.4hX",sel->read(adr++));
		printf(" ");
		adr%=sel->get_size();
	}		
	printf("\n");
}

void modifymem(void) {
	char whatmem;
	mem_base* sel;
	unit adr,data;
	cout<<"Choose: (d)ata stack, (r)eturn stack or (m)ain memory : ";
	cin>>whatmem;
	switch (whatmem) {
	case 'd' :
	case 'D' :	sel=&ds;
				break;
	case 'r' :
	case 'R' :	sel=&rs;
				break;
	case 'm' :
	case 'M' :	sel=&mem;
				break;
	default :	cout<<"Wrong input. Assuming data stack.\n";
				sel=&ds;
				break;
	}
	cout<<"Enter address and value (both in decimal(sorry)) :";
	cin>>adr>>data;
	sel->write(adr,data);
}

void modifyreg(void) {
	int regnum;
	unit val;
	cout<<"Enter register number : ";
	cin>>regnum;
	printf("Current value : %.4hX",fp.read_register(whatreg(regnum)));
	cout<<"Enter new value : ";
	cin>>val;
	fp.write_register(whatreg(regnum), val);
}