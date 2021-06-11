#include "Sim1.h"

simomg::simomg( void (*f)(char), char (*g)(void), bool (*h)(void), bool (*i)(void))
{
       consoleout=f;
       consolein=g;
       tecken_sands_till_P = false;
       tecken_tas_emot_fran_P = false;
       time_for_interrupt = h;
       gui_wants_to_quit = i;
       ds = new memory(0xFF);
       rs = new memory(0xFF);
       heap = new memory(0xFFFF);
       novix = new nc4k(ds,rs,heap);
       novix->write_x_port((las_reg(reg_Xdata) & 15)+16);//en logisk ah”g niv† p† X4.
       ld_bin();
}

void simomg::en_klockcykel(void)
{
  //if (!time_for_interrupt)
   one_cycle();
   //if (las_reg(reg_P) == 0x16b9) novix->write_register(reg_T,0); //Fulfusk #1!!
   //if (las_reg(reg_P) == 0x169a) novix->write_register(reg_T,0x01a2); //Fulfusk #2!!
}

void simomg::run()
{  //unit pc_value = read_reg(pc);

      oldX0 = false;
      bool bp_reached = false;

      while (!bp_reached && !gui_wants_to_quit() && !heap->do_break()) { //S† l„ngs som GUI inte vill avbryta ,och  brytpunktselement
        //unit pc_value = read_reg(pc);                //inte ==1;
	    //if (time_for_interrupt)//{processor.interrupt();}  // Kolla om GUI vill g”ra interrupt,i s† fall g”r det
	    //unit pc_value = read_reg(pc);                  //  Kolla „ven om nuvarande PC-v„rde „r brytpunktsmarkerad
        /*if (byte_BP_array[pc_value] > 1)
          {                                            // I s† fall r„kna ner brytpunkts elementets v„rde
	      byte_BP_array[pc_value]-=1;
	      } else
	      {
	        if (byte_BP_array[pc_value] == 1) {bp_reached = true;}
	      }
          */

        one_cycle();                                     // Stega fram n„sta cykel.

        //if (las_reg(reg_P) == 0x16b9) novix->write_register(reg_T,0); //Fulfusk #1!!
        //if (las_reg(reg_P) == 0x169a) novix->write_register(reg_T,0x01a2); //Fulfusk #2!!

        if (BP_array[las_reg(reg_P)] > 0){
          if (BP_array[las_reg(reg_P)] > 1) {
            BP_array[las_reg(reg_P)] -= 1;
          } else {
            bp_reached = true;
          }
        }
        
      }
}

bool simomg::give_bit(char c,int bitnr)
{
  byte bitvalues[8] = {1,2,4,8,16,32,64,128};
  byte compareval = bitvalues[bitnr];
  return ((c && compareval));
}

void simomg::reset()
{
  //processor.reset();
}

void simomg::clear()
{
  //processor.clear();
}

void simomg::one_cycle()
{
//  int bitval[8] = {128,64,32,16,8,4,2,1};
  int bitval[8] = {1,2,4,8,16,32,64,128};
  int cycles = novix->step_instruction();      //stega processorn en cykel ..

  if (tecken_sands_till_P){              //..och kolla om simulatorogivningen „r under s„ndning till P.
    tecken_ut_count_till_P += 1;   // I s† fall s† ”ka ”verf”ringsr„knaren_GUI_till_P
    bool dags_att_skicka_tecken_P = ((tecken_ut_count_till_P % 417) == 0);   // Om du „r klar med en bit
    if (dags_att_skicka_tecken_P) {   // ..och det „r dags att ”verf”ra en ny enskild bit.
      if (tecken_ut_count_till_P < 4170+417*2) {   //..och om hela byten „nnu inte ”verf”rts till processorn dvs.(8+2 bitar)
        int vilken_bit_skall_skickas_P = (tecken_ut_count_till_P / 417) - 1;    //S† l„gg ut n„sta bit p† X1-porten.
        if (vilken_bit_skall_skickas_P < 8){
          bool etta = (byte_till_P & bitval[vilken_bit_skall_skickas_P]) != 0;
          if (!etta)
            novix->write_x_port((las_reg(reg_Xdata) & 15));
          else
            novix->write_x_port((las_reg(reg_Xdata) & 15)+16);
          //h„r l„gger vi med hj„lp av  proceduren givebit ut r„tt bit till processorn
        }
          else
        {
          //h|r l|gger vi ut en etta d.v.s stoppbit(bit 10) till processorn
          novix->write_x_port((las_reg(reg_Xdata) & 15)+16);
        }
      }	//Det vill s„ga l†s X1-pinnen till ett nytt v„rde inf”r n„sta 417 klockcykler.
        else                               //tecken_ut_count_till_P har blivir 4170,och det har skett en fullst„ndig
      {
         tecken_sands_till_P = false;  //”verf”ring fr†n GUI till Processorn,s„tt d„rf”r s„ndflaggan till falsk.
         tecken_ut_count_till_P = 0;  //..och nollst„ll ”verf”ringsr„knaren.
         novix->write_x_port((las_reg(reg_Xdata) & 15)+16);//L„gg till slut ut en logisk ah”g niv† p† X1.
      }
    }
  }
    else                                        //Du „r allts† inte under s„ndning till processorn
  {
    char ascii_char_from_GUI = consolein(); //Vill GUI ”verf”ra n†t tecken till processorn?
    if (ascii_char_from_GUI != 0 ) {
      byte_till_P = ascii_char_from_GUI;//L„gg ascii-tecknet ifr†n GUI i v†r privata variabel byte_till_p, som skall ”verf”ras till processorn  ett boolskt bitf„lt,med start och stopp-bitar
      tecken_ut_count_till_P = 0;//on”digt
      novix->write_x_port((las_reg(reg_Xdata) & 15));
      //**h|r l|gger vi ut en logisk nolla till porrcessorn ( startbit allts])
      tecken_sands_till_P = true; //Nu „r du i ”verf”ringsmode fr†n GUI till processor.Men „nnu „r inget ”verf”rt.
    }
  }
   //Proceduren one_step har ovan tagit hand om eventuell ”verf”ring ifr†n GUI till processorn.
   //Nu kommer den ta hand om eventuell ”verf”ring ifr†n processorn till GUI.
  if (tecken_tas_emot_fran_P) {            //Om processorn „r under ”verf”ring
     tecken_in_count_fran_P += 1;          //processorn en klockcykel i b”rjan av proceduren.
    if ((tecken_in_count_fran_P) < cyclelength*10 ) {  //Om ”verf”ringen av 8 + 2 bitar a` 417 cykler „nnu inte „r fullbordad ..
       bool dags_att_taemot_tecken_P = ((tecken_in_count_fran_P % (cyclelength)) == 0);   // Om du „r mitt inne i ett bitintervall
	   if (dags_att_taemot_tecken_P) {     // Och en ny bit just har b”rjat f”ras ”ver S† lagra den nya biten p† r„tt plats ..
	      int vilken_bit_skall_tasemot_fr_P = (tecken_in_count_fran_P / (cyclelength+3));
               if ((vilken_bit_skall_tasemot_fr_P > 0) && (vilken_bit_skall_tasemot_fr_P < 9))
               if (novix->read_x_port() & 1) {
                 byte_fran_P += bitval[vilken_bit_skall_tasemot_fr_P-1];
                 //consoleout('1');
               } else ;//consoleout('0');

       }                                            // Annars „r det „nnu inte dags att ta emot ett tecken.
	 }
	   else                              // Om tecken_in_count_P -r„knaren har stegats upp till 4170,s†
	 {
           consoleout(byte_fran_P);
           tecken_tas_emot_fran_P  = false;// s† skall flaggan som idikerar ”verf”ring ifr†n processor till GUI s„ttas
	   tecken_in_count_fran_P       = 0;    // till false och r„knaren ska nollst„llas,dvs.f”rbered eventuella framtida ”verf”ringar ifr†n processor till GUI. console out skall anropas
           time_for_interrupt();
	   oldX0 = true;
           //console_out(det f„rdiga tecknet);
	 }
  }
    else // Hit kommer man om flaggan f”r ”verf”ring fr†n processorn till GUI inte var satt i b”rjan av proceduren.
         // Om flaggan f”r ”verf”ring inte „r satt AND pinne X4 „r l†g s† har den g†tt l†g f”r f”rsta g†ngen sen senaste
	 // fullbordade ”verf”ring ifr†n processorn till GUI.*/
  {
     bool newX0 = novix->read_x_port() & 1;
     if ((!newX0) && oldX0 && (!tecken_tas_emot_fran_P))
     {
       tecken_tas_emot_fran_P = true;
       tecken_in_count_fran_P       = cyclelength / 2 + 10;
//       tecken_in_count_fran_P       = 0;
       byte_fran_P = 0;
     }
     oldX0 = newX0;
  }//Pinne X4 „r fortfarande h”g.Dvs.,processorn har ingenting att s„nda ”ver till GUI.
}



unit simomg::read_ds(unit stackpos)
{
  return ds->read(stackpos);
}

unit simomg::read_rs(unit stackpos)
{
  return rs->read(stackpos);
}
unit simomg::read_b_port(void)
{
  return novix->read_b_port();
}
unit simomg::read_x_port(void)
{
  return novix->read_x_port();
}
void simomg::write_b_port(unit data)
{
  novix->write_b_port(data);
}
void simomg::write_x_port(unit data)
{
  novix->write_x_port(data);
}

unit simomg::read_heap(unit heappos)
{
  return heap->read(heappos);
}

void simomg::write_ds(byte stackpos,unit data)
{
  ds->write(stackpos,data);
}

void simomg::write_heap(unit heappos,unit data)
{
  heap->write(heappos,data);
}

void simomg::write_rs(byte stackpos,unit data)
{
  rs->write(stackpos,data);
}

byte simomg::read_BP(unit BP_pos)
{
  return BP_array[BP_pos];
}

void simomg::write_BP(unit BP_pos, unit BP_count)
{
  BP_array[BP_pos] = BP_count;
}

unit simomg::las_reg(whatreg r){
  return novix->read_register(r);
}
void simomg::write_reg(whatreg r,unit data)
{
  novix->write_register(r,data);
}
void simomg::ld_bin(void) {
	int fdatalo, fdatahi;
	unit adr=0x2000, data;
	FILE *hifile, *lofile;
        int i;


	if ((hifile=fopen("hi.bin","rb"))==NULL) printf("could not find hi.bin\n");
	if ((lofile=fopen("lo.bin","rb"))==NULL) printf("could not find lo.bin\n");
	fdatalo=fgetc(lofile);
	fdatahi=fgetc(hifile);
	//printf("loading first half of binaries to base 0x2000...");
	do {data=(fdatahi<<8) + fdatalo;
		heap->write(adr++,data);
		fdatalo=fgetc(lofile);
		fdatahi=fgetc(hifile);
	} while (adr<0x3000);
	//printf("Done.\nLoading second half of binaries to base 0x1000...");
	adr=0x1000;
	do {
		data=(fdatahi<<8) + fdatalo;
		heap->write(adr++,data);
		fdatalo=fgetc(lofile);
		fdatahi=fgetc(hifile);
	} while (feof(lofile)==0 && feof(hifile)==0);
	printf("Done.\nFinished loading at 0x%.4hX\n",adr);
	fclose(lofile);fclose(hifile);

        for (i = 0; i < 0x10000; i++) {
          write_BP ((unit) i, 0);
        }


//        heap->write (0x16b6, 0xdf4a);  // Bug in cmForth? Count is one too high
//        heap->write (0x16bA, 0x8000);  // Bin„rpatcha bort flushbuffers ned en NOOP 
}
