/*******************************************************************************
*                                                                              *
*   NC4000 simulator core -- class implementation                              *
*                                                                              *
*******************************************************************************/

#include "nc4k.h"

//*********************************
// Construction/Destruction methods
//*********************************


nc4k::nc4k(memory* d, memory* r, memory* m) {
	ds=d;
	rs=r;
	mem=m;
	reset();
}


nc4k::~nc4k() {}


//**************************
// Private/Protected methods
//**************************


void nc4k::PushDS(void) {
	K++;		// Perform 8-bit wraparound
	K&=0x0FF;	
	ds->write(K,N); //Preincrement
}


void nc4k::PopDS(void) {
	N=ds->read(K--);	//Postdecrement
	K&=0x0FF;	//Perform 8-bit wraparound
}


void nc4k::PushRS(void) {
	J++;		// Perform 8-bit wraparound
	J&=0x0FF;
	rs->write(J,I);	//Preincrement
}


void nc4k::PopRS(void) {
	I=rs->read(J--);	//Postdecrement
	J&=0x0FF;	// Perform 8-bit wraparound
}


void nc4k::ALU_Op() {
	unit op2, result, carry_in;

	//
	// 1. Get operand 2
	//

	switch ((L & M_Opy)>>7) {		// get operand 2
	case 0:
	case 1:
		op2=N;
		break;
	case 2:
		op2=MD;
		break;
	case 3:
		op2=SR;
		break;
	default:
		break;
	}

	// manage Tn & Sa bits
	if (L & M_TtoN) {	// Tn bit set..
		if (L & M_SA)	{// ..and Sa bit set
			PushDS();
		}
		N=T;
	} else {			// Tn bit not set..
		if (L & M_SA)	{// ..but Sa bit set
			PopDS();
		}
	}

	//
	// 2. Do arithmetic operation
	//

	// check if N with carry. If so move carry to carry_in
	if (((L & M_Opy)>>7)==1) carry_in=carry; else carry_in=0;

	// Perform ALU function
	result=DoALU(op2,carry_in,SM_ALU);

	//
	// 3. Write results to T (if allowed by step instructions)
	//
	if (!( ((L & M_Div)&&!carry) || ((L & M_Opy)==2 && (N & 0x0001)))) T=result; //also check extra div cond (/").

	//
	// 4. Do shift operation
	//
	switch (((L & M_32bit)>>1)|(L & M_Shift)) { //Mask out D,SR,SL and compact
	case 0:			// No shift
		break;
	case 1:			// Shift result right
		T>>=1;
		break;
	case 2:			// Shift result left
		T<<=1;
		break;
	case 3:			// Propagate msb (used as '0<')
		if (T & M_Sign) T=0xFFFF; else T=0;
		break;
	case 4:			// Special case: left shift N only
		N=(N<<1)|carry; // carry into N
	case 5:			// 32-bit shift right (T as most sign. word)
		N=(N>>1)|((T & 1)<<15);
		T>>=1;
		break;
	case 6:			// 32-bit shift left (T as most sign. word)
		T=(T<<1)|(N>>15 & 0x0001); // 0x0001 is just to make sure unsigned
		N=(N<<=1)|carry; // carry into N
		break;
	default:		// 7 not valid combination
		break;
	}

	//
	// 5. Return bit (;) handling
	//
	if (L & M_Ret) {
		P=I&0x7FFF;carry=I>>15&1; // restore P and carry
		PopRS();
	}
}

unit nc4k::DoALU(unit Op_Y,unit carry_in, unit ALU_func) {
	unit result;
	doubleunit c_check;

	result=carry_in;	// Get carry in (if any)
	carry=0;	//Reset carry now and correct it in ALU operation

	// Perform ALU function
	switch (ALU_func) {
	case 0:		//Pass T
		result=T; // D-uh!
		break;
	case 1:		//Bitwise AND
		result=T&Op_Y;
		break;
	case 2:		//T-Op_Y
		c_check=T+(~Op_Y)+1+result;
		result=c_check & 0xFFFF;	//mask out unit size
		if (c_check>>16) carry=1;	//overflow/carry?
		break;
	case 3:		//Bitwise OR
		result=T|Op_Y;
		break;
	case 4:		//T+Op_Y
		c_check=T+Op_Y+result;
		result=c_check & 0xFFFF;	//mask out unit size
		if (c_check>>16) carry=1;	//overflow/carry?
		break;
	case 5:		//Bitwise XOR
		result=T^Op_Y;
		break;
	case 6:		//Op_Y-T	(Real stack subtraction)
		c_check=Op_Y+(~T)+1+result;
		result=c_check & 0xFFFF;	//mask out unit size
		if (c_check>>16) carry=1;	//overflow/carry?
		break;
	case 7:		//Pass Opy
		result=Op_Y;
		break;
	default:
		break;
	}
	return result;
}


int nc4k::IO_LLI(int store) {
	int cycles =1;
	unit swap;

	if (store==0) {				// Fetch type
		if (L&M_TtoN) {PushDS(); N=T;}	// Tn bit active
		switch (SM_IOselect) {
			case 0:
			case 1:
			case 2:	// Local fetch
				M=mem->read(L&M_5bit);
				T=DoALU(M, carry&SM_carryin, SM_ALU);
				cycles=2;
				break;
			case 3:
			case 7:	// Internal fetch
				T=DoALU(int_read_reg(L&M_4bit), carry&SM_carryin, SM_ALU);
				if (L&M_SA) PopRS();
				break;
			default: // 16-bit literal fetch
				M=mem->read(P++);
				T=DoALU(M, carry&SM_carryin, SM_ALU);
				cycles=2;
				break;
		}
	} else {					// Store type
		switch (SM_IOselect) {
			case 0:
			case 1: // Local store
				M=L&M_5bit; mem->write(M, T);
				T=DoALU(N, carry&SM_carryin, SM_ALU);
				if (!(L&M_TtoN)) PopDS(); // Tn bit for stores
				cycles=2;
				break;
			case 2:
			case 3:	// Internal store
				if (L&M_SA) {PushRS(); timesmode=true;}
                else if ((L&M_4bit)==1) PushRS();
				int_write_reg(L&M_4bit, T);
				T=DoALU(N, carry&SM_carryin, SM_ALU);
				if (!(L&M_TtoN)) PopDS(); // Tn bit for stores
				break;
			case 7: // Internal swap
				//if (M_SA) {PushRS(); timesmode=true;}
				if (L==0xDE91) {PushRS(); timesmode=true;}
				swap=int_read_reg(L&M_4bit);
				int_write_reg(L&M_4bit, T);
				T=DoALU(swap, carry&SM_carryin, SM_ALU);
				break;
			default: // Short literal fetch
				if (L&M_TtoN) {PushDS(); N=T;}	// Tn bit active
				T=DoALU(L&M_5bit, carry&SM_carryin, SM_ALU);
				break;
		}

	}
	// Handle return bit (Illegal results in some cases)
	if (L & M_Ret) {
		P=I&0x7FFF;carry=I>>15&1; // restore P and carry
		PopRS();
	}
	return cycles;
}


void nc4k::IO_Mem(int store) {
    if (store==0) {
        if (SM_IOselect==7) {
            PushDS();
            N=mem->read(T);
            T=DoALU(L&M_5bit, carry&SM_carryin, SM_ALU);
        } else {
            // Extended memory read is ignored
	    T=mem->read(T); // Cheating...
            if (!(L&M_TtoN)) {T=DoALU(N, carry&SM_carryin, SM_ALU); PopDS();}	// Tn bit active
        }

        // DUP @ SWAP nn + (1647nn) ignored
    } else {
        mem->write(T,N); PopDS();
        if  (SM_IOselect==7) {
            T=DoALU(L&M_5bit, carry&SM_carryin, SM_ALU);
        } else {
            // Extended memory write is ignored
            if (!(L&M_TtoN)) {T=DoALU(N, carry&SM_carryin,SM_ALU); PopDS(); }
        }
    }
    return;
}


unit nc4k::int_read_reg(unit reg) { //Kind of self-explanatory really...
switch (reg) {
	case 0:	// JK register
		return J<<8|K;
	case 1:
		return I;
	case 2:
		return P;
	case 3:
		return 0xFFFF; // Logical true (-1)
	case 4:
	case 5:
		return MD;
	case 6:
	case 7:
		return SR;
	case 8:
		return Binput^Bdata&~Bdir; // (XOR'ed) Xdata
	case 9:
		return Bmask;
	case 10:
		return Bdir;
	case 11:
		return Btri;
	case 12:
		return (Xinput^Xdata&~Xdir)&0x1F; // (XOR'ed) Xdata
	case 13:
		return Xmask&0x1F;
	case 14:
		return Xdir&0x1F;
	case 15:
		return Xtri&0x1F;
	default:
		return 0;
	}
}


void nc4k::int_write_reg(unit reg, unit data) {
	switch (reg) {
	case 0: // JK register
		J=0xFF&data>>8; K=0xFF&data;
		break;
	case 1:
		I=data;
		break;
	case 2:
		P=data;
		break;
	case 4:
	case 5:
		MD=data;
		break;
	case 6:
	case 7:
		SR=data;
		break;
	case 8:
		Bdata=data&~Bmask;
		break;
	case 9:
		Bmask=data;
		break;
	case 10:
		Bdir=data;
		break;
	case 11:
		Btri=data;
		break;
	case 12:
		Xdata=data&~Xmask&0x1F;
		break;
	case 13:
		Xmask=data&0x1F;
		break;
	case 14:
		Xdir=data&0x1F;
		break;
	case 15:
		Xtri=data&0x1F;
		break;
	default:
		break;
	};
	return;
}


// *************************
// Public (exported) methods
// *************************


int nc4k::step_instruction(void) {

	if (intreq&&(Xtri&0x0100)) { //Check for pending interrupt
		interrupt();
	}

    if (timesmode) { // TIMES instruction handling
    	M=mem->read(P);
    	if (I>0) I--; else {PopRS(); P++; timesmode=false;}
    } else M=mem->read(P++);		// Instruction increment


	L=M;					// transfer to instruction decode latch
	switch ((L & 0xF000)>>12) { //Mask and shift instruction type field

		case 8: 		// *** ALU instruction: 1000b ***
			ALU_Op();
			break;

		case 9: 		// *** IF(conditional branch): 1001b ***
			if (T) P=(P & 0xF000)|(L & 0x0FFF); // P gets masked value from instruction
			T=N; PopDS(); //Pop value from data stack
			// Observe: One cannot branch across 4K page boundary
			break;

		case 10:		// *** LOOP: 1010b ***
			I--;				// Decrement loop
			if (I) P=(P & 0xF000)|(L & 0x0FFF); // P gets masked value from instruction
			else PopRS();
			// Observe: One cannot branch across 4K page boundary
			break;

		case 11:		// *** ELSE(unconditional branch): 1011b ***
			P=(P & 0xF000)|(L & 0x0FFF);
			// Observe: One cannot branch across 4K page boundary
			break;

		case 12:		// *** Literal/Local/Internal fetch: 1100b ***
			return IO_LLI(0);

		case 13:		// *** Literal/Local/Internal store: 1101b ***
			return IO_LLI(1);

		case 14:		// *** Memory fetch: 1110b ***
			IO_Mem(0);
			return 2; //# cycles for memory instruction execution

		case 15:		// *** Memory store: 1111b ***
			IO_Mem(1);
			return 2; //# cycles for memory instruction execution

		default:		// *** Call instruction: 0111b or lower ***
			PushRS();
			I=P|carry<<15;	// Save carry and P
			P=L & 0x7FFF;			//call address -> P
			break;
	}
	return 1; //# cycles for instruction execution
}


unit nc4k::read_register(whatreg reg) { //Kind of self-explanatory really...
	if (reg==reg_JK) return J<<8|K;
	return regs[reg];
}


void nc4k::write_register(whatreg reg, unit data) {
	switch (reg) {

	case reg_Xdata:
	case reg_Xdir:
	case reg_Xmask:
		regs[reg]=data&0x1F;	// Only 5 bits in X-port
		break;

	case reg_Xtri:
		regs[reg]=data&0x011F; // Only 5 bits + intena(bit 8)
		break;

	case reg_JK:
		J=0xFF&data>>8; K=0xFF&data;
	default:
		regs[reg]=data;
		break;
	}
}


unit nc4k::read_x_port() {
	return Xdata&Xdir;// masked output bits (Tristate not implemented)
}


unit nc4k::read_b_port() {
	return Bdata&Bdir;// (See nc4k::read_x_port)
}


void nc4k::write_x_port(unit data) {
	Xinput=data&~Xdir&0x1F; // Xdata only has its input bits modified.
}


void nc4k::write_b_port(unit data) {
	Binput=data&~Bdir; // (See nc4k::write_x_port)
}


void nc4k::reset(void) {
	P=0x1000;	// 0x1000 = Reset vector
	J=0; K=0;	// Clear stack indexes
}


void nc4k::interrupt(void) {
	if (Xtri&0x0100) {	// Bit 8 in Xtri set? (interrupt enable)
		PushRS();
		I=P;
		P=0x0020;				//0x0020 = Interrupt vector
		intreq=false;
	} else {
		intreq=true;
	}
}