//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#include <string.h>
#include <stdlib.h>
#pragma hdrstop
#include "gui.h"
//---------------------------------------------------------------------------
#pragma link "Grids"
#pragma resource "*.dfm"
void chrout(char c);
char charin(void);
bool timefori(void);
bool guiwq(void);
TForm1 *Form1;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner):TForm(Owner)
{
/*        simomg( void (*f)(char), char (*g)(), bool (*h)(), bool (*i)());*/
 // simomg sim = simomg(chout,chin,tfi,gwq);
  opgrid_infront = true;
  CharHead = NULL;
  console_row = 0;
  init_gui();
  sim = new simomg(&chrout,&charin,&timefori,&guiwq);

  //sim->write_x_port((sim->read_x_port() & 15));
//  sim->write_BP(0x13db,1); //
//  sim->write_BP(0x1281,1); // riktig dot
//  sim->write_BP(0x1269,1); //dividehelvete
//  sim->write_BP(0x1040,1); //dividehelvete
//  sim->write_BP(0x1095,1); //"/"
//  sim->write_BP(0x108f,1); //"*"
//  sim->write_BP(0x1816,1); //":"
//   sim->write_BP(0x1425,1); // Interpret
//   sim->write_BP(0x1410,1); // "-'"
//sim->write_BP(0x1119,1); // EXECUTE
//   sim->write_BP(0x16b7,1); // Interpret
  // sim->write_BP(5820,1);
  // sim->write_BP(5175,1); //Mystisk retur from expect;
  // sim->write_BP(5136,1); //den kommer inte tillbaka h„rifr†n;
}
//---------------------------------------------------------------------------
void chrout(char c)
{
  Form1->charout(c);
}
//---------------------------------------------------------------------------
void TForm1::charout(char c)
{


  switch (c)
  {
    case 13:
      console_row = Memo1->Lines->Add(c);
      break;

    case 10:
      break;
    case 8:  {
                AnsiString TempStr = Memo1->Lines->Strings[console_row];
                TempStr.Delete(TempStr.Length(),1);
                Memo1->Lines->Delete(console_row);
                Memo1->Lines->Add(TempStr);
             }
             break;
    case 32: {
                AnsiString TempStr = Memo1->Lines->Strings[console_row];
                Memo1->Lines->Delete(console_row);
                Memo1->Lines->Add(TempStr+=' ');
             }
             break;

    default:
      if ((c < 0x20) || (c > 0x7f)) {
        break;
//        c = '*';
      }
      AnsiString TempStr = Memo1->Lines->Strings[console_row];
      Memo1->Lines->Delete(console_row);
      Memo1->Lines->Add(TempStr+=c);
   }
}
//---------------------------------------------------------------------------
bool timefori(void)
{
  return Form1->tfi();
}
//---------------------------------------------------------------------------
bool TForm1::tfi(void)
{
  return true;
}
//---------------------------------------------------------------------------
bool guiwq(void)
{
  return Form1->gwq();
}
//---------------------------------------------------------------------------
bool TForm1::gwq(void)
{
  return stop_running;
}
//---------------------------------------------------------------------------
char charin(void)
{
  return Form1->chin();
}
//---------------------------------------------------------------------------
AnsiString TForm1::itohex(int i)
{
  char hexsiffror[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  AnsiString hexstr = "";


  hexstr += hexsiffror[(i / 4096) % 16];
  hexstr += hexsiffror[(i % 4096) / 256];
  hexstr += hexsiffror[(i % 256) / 16];
  hexstr += hexsiffror[i % 16];

  return hexstr;
}
//---------------------------------------------------------------------------
AnsiString TForm1::itohex2(int i)
{
  char hexsiffror[16] =
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  AnsiString hexstr = "";
  hexstr += hexsiffror[(i % 256) / 16];
  hexstr += hexsiffror[i % 16];
  return hexstr;
}
//---------------------------------------------------------------------------
void TForm1::read_novix_datastack(void)
{
  unit stackdjup = sim->las_reg(reg_K);


  if (stackdjup > 0) {
    ds_grid->RowCount = stackdjup+2;

    ds_grid->Cells[1][0] = itohex(sim->las_reg(reg_T));
    ds_grid->Cells[1][1] = itohex(sim->las_reg(reg_N));

    for (unit i = 0; i < stackdjup; i++)
        ds_grid->Cells[1][stackdjup - i - 1 + 2] = itohex(sim->read_ds(i+1));
  } else {
    ds_grid->RowCount = 2;
    ds_grid->Cells[1][0] = itohex(sim->las_reg(reg_T));
    ds_grid->Cells[1][1] = itohex(sim->las_reg(reg_N));
  }
}
//---------------------------------------------------------------------------
void TForm1::read_novix_returnstack(void)
{
  unit stackdjup = sim->las_reg(reg_J);


  if (stackdjup > 0) {
    rs_grid->RowCount = stackdjup+1;

    rs_grid->Cells[1][0] = itohex(sim->las_reg(reg_I));

    for (unit i = 0; i < stackdjup; i++)
      rs_grid->Cells[1][stackdjup - i -1 + 1] = itohex(sim->read_rs(i+1));
  } else {
    rs_grid->RowCount = 1;
    rs_grid->Cells[1][0] = itohex(sim->las_reg(reg_I));
  }
}
//---------------------------------------------------------------------------
void TForm1::read_novix_registers(void)
{
  reg_grid->Cells[1][0] =itohex(sim->las_reg(reg_T));
  reg_grid->Cells[1][1] =itohex(sim->las_reg(reg_N));
  reg_grid->Cells[1][2] =itohex(sim->las_reg(reg_K));
  reg_grid->Cells[1][3] =itohex(sim->las_reg(reg_I));
  reg_grid->Cells[1][4] =itohex(sim->las_reg(reg_J));
  reg_grid->Cells[1][5] =itohex(sim->las_reg(reg_M));
  reg_grid->Cells[1][6] =itohex(sim->las_reg(reg_L));
  reg_grid->Cells[1][7] =itohex(sim->las_reg(reg_P));
  reg_grid->Cells[1][8] =itohex(sim->las_reg(reg_MD));
  reg_grid->Cells[1][9] =itohex(sim->las_reg(reg_SR));
  reg_grid->Cells[1][10] =itohex(sim->las_reg(reg_carry));
  reg_grid->Cells[1][11] =itohex(sim->las_reg(reg_Xdata));
  reg_grid->Cells[1][12] =itohex(sim->las_reg(reg_Xmask));
  reg_grid->Cells[1][13] =itohex(sim->las_reg(reg_Xdir));
  reg_grid->Cells[1][14] =itohex(sim->las_reg(reg_Xtri));
  reg_grid->Cells[1][15] =itohex(sim->las_reg(reg_Bdata));
  reg_grid->Cells[1][16] =itohex(sim->las_reg(reg_Bmask));
  reg_grid->Cells[1][17] =itohex(sim->las_reg(reg_Bdir));
  reg_grid->Cells[1][18] =itohex(sim->las_reg(reg_Btri));

  Edit3->Text = itohex(sim->read_x_port());
  Edit4->Text = itohex(sim->read_b_port());
}
//---------------------------------------------------------------------------
int TForm1::chrtoi(char c)
{
  char chval[32] =  {'0','0','1','1','2','2','3','3','4','4'
  					,'5','5','6','6','7','7','8','8','9','9'
                    ,'a','A','b','B','c','C','d','D','e','E','f','F'};
  int val = 0;
  for (int i = 0;i<32;i++) {if (c == chval[i]) val = i / 2;}
  return val;
}
//---------------------------------------------------------------------------
void TForm1::init_gui(void)
{
  internal_change = false;

  tb_mempos->PageSize = opgrid->RowCount - 2;
  
  basadress = 0x0ffA;
  ds_grid->ColWidths[0] = 50;
  ds_grid->ColWidths[1] = 50;
  rs_grid->ColWidths[0] = 50;
  rs_grid->ColWidths[1] = 50;
  reg_grid->ColWidths[0] = 60;
  reg_grid->ColWidths[1] = 50;

  opgrid->ColWidths[0] = 14;
  opgrid->ColWidths[1] = 40;
  opgrid->ColWidths[2] = 253;

  hexgrid->ColWidths[0] = 42;
  hexgrid->ColWidths[1] = 42;
  hexgrid->ColWidths[2] = 42;
  hexgrid->ColWidths[3] = 42;
  hexgrid->ColWidths[4] = 42;
  hexgrid->ColWidths[5] = 92;

  reg_grid->Cells[0][0] = "T";
  reg_grid->Cells[0][1] = "N";
  reg_grid->Cells[0][2] = "K";
  reg_grid->Cells[0][3] = "I";
  reg_grid->Cells[0][4] = "J";
  reg_grid->Cells[0][5] = "M";
  reg_grid->Cells[0][6] = "L";
  reg_grid->Cells[0][7] = "P";
  reg_grid->Cells[0][8] = "MD";
  reg_grid->Cells[0][9] = "SR";
  reg_grid->Cells[0][10] = "carry";
  reg_grid->Cells[0][11] = "Xdata";
  reg_grid->Cells[0][12] = "Xmask";
  reg_grid->Cells[0][13] = "Xdir";
  reg_grid->Cells[0][14] = "Xtri";
  reg_grid->Cells[0][15] = "Bdata";
  reg_grid->Cells[0][16] = "Bmask";
  reg_grid->Cells[0][17] = "Bdir";
  reg_grid->Cells[0][18] = "Btri";

  ds_grid->Cells[0][0] = "  T";
  ds_grid->Cells[0][1] = "  N";

  rs_grid->Cells[0][0] = "  I";
  for (int i = 0; i<256;i++) {
    char string[25];

    sprintf (string, "%3d", i);
    ds_grid->Cells[0][i+2] = string;

    sprintf (string, "%3d", i);
    rs_grid->Cells[0][i+1] = string;
  }

}
//---------------------------------------------------------------------------
void __fastcall TForm1::ds_gridDblClick(TObject *Sender)
{
  chval_dialog = new Tchange_val_dialog(Application);
  AnsiString temstr = "DS Pos:"+IntToStr(ds_grid->Row);
  chval_dialog->Caption = temstr;
  chval_dialog->Editbox->Text = ds_grid->Cells[1][ds_grid->Row];
  if (chval_dialog->ShowModal() == mrOk) {
    AnsiString stri = chval_dialog->Editbox->Text.Trim();
    int pos = 1;
    int retval = 0;
    while ((stri.Length() >= pos)) {
      retval *= 16;
      retval += chrtoi(stri[pos]);
      pos += 1;
    }
    ds_grid->Cells[1][ds_grid->Row] = itohex(retval);
    sim->write_ds(ds_grid->Row,retval);
    chval_dialog->Tchange_val_dialog::~Tchange_val_dialog();
  };
}
//---------------------------------------------------------------------------
void __fastcall TForm1::rs_gridDblClick(TObject *Sender)
{
  chval_dialog = new Tchange_val_dialog(Application);
  AnsiString temstr = "RS Pos:"+IntToStr(rs_grid->Row);
  chval_dialog->Caption = temstr;
  chval_dialog->Editbox->Text = rs_grid->Cells[1][rs_grid->Row];
  if (chval_dialog->ShowModal() == mrOk) {
    AnsiString stri = chval_dialog->Editbox->Text.Trim();
    int pos = 1;
    int retval = 0;
    while ((stri.Length() >= pos)) {
      retval *= 16;
      retval += chrtoi(stri[pos]);
      pos += 1;
    }
    rs_grid->Cells[1][rs_grid->Row] = itohex(retval);
    sim->write_rs(rs_grid->Row,retval);
    chval_dialog->Tchange_val_dialog::~Tchange_val_dialog();
  };
}
//---------------------------------------------------------------------------
void __fastcall TForm1::reg_gridDblClick(TObject *Sender)
{
  whatreg regarr[19] = {reg_T,reg_N,reg_K,reg_I,reg_J,reg_M,reg_L,reg_P,
                        reg_MD,reg_SR,reg_carry,reg_Xdata,reg_Xmask,reg_Xdir
                        ,reg_Xtri,reg_Bdata,reg_Bmask,reg_Bdir,reg_Btri};
  chval_dialog = new Tchange_val_dialog(Application);
  AnsiString temstr = "Register: "+reg_grid->Cells[0][reg_grid->Row];
  chval_dialog->Caption = temstr;
  chval_dialog->Editbox->Text = reg_grid->Cells[1][reg_grid->Row];
  if (chval_dialog->ShowModal() == mrOk) {
    AnsiString stri = chval_dialog->Editbox->Text.Trim();
    int pos = 1;
    int retval = 0;
    while ((stri.Length() >= pos)) {
      retval *= 16;
      retval += chrtoi(stri[pos]);
      pos += 1;
    }
    reg_grid->Cells[1][reg_grid->Row] = itohex(retval);
    sim->write_reg(regarr[rs_grid->Row],retval);
    chval_dialog->Tchange_val_dialog::~Tchange_val_dialog();
  };

}
//---------------------------------------------------------------------------
AnsiString int_reg(int reg_nr)
{
  AnsiString reg_str;
  switch (reg_nr)
  {
    case  00: reg_str = "j/k";
    break;
    case  01: reg_str = "index";
    break;
    case  02: reg_str = "pc";
    break;
    case  03: reg_str = "true";
    break;
    case  04: reg_str = "md";
    break;
    case  06: reg_str = "sr";
    break;
    case  010: reg_str = "B";
    break;
    case  011: reg_str = "B mask";
    break;
    case  012: reg_str = "B i/o";
    break;
    case  013: reg_str = "B tristate";
    break;
    case  014: reg_str = "X";
    break;
    case  015: reg_str = "X mask";
    break;
    case  016: reg_str = "X i/o";
    break;
    case  017: reg_str = "X tristate";
    break;
    case  021: reg_str = "#times";
    break;
  }
  return reg_str;
}


//---------------------------------------------------------------------------
AnsiString TForm1::opcode_to_text(unit opc,bool *flf)
{
  AnsiString opstr;
  *flf = false;
  switch (opc)
  {
    //Arithmetic instructions
    case 0100000 : {opstr = "NOOP" ;}
    break;
    case 0100020 : {opstr = "NIP" ;}
    break;
    case 0107020 : {opstr = "DROP" ;}
    break;
    case 0107000 : {opstr = "DROP DUP" ;}
    break;
    case 0100120 : {opstr = "DUP" ;}
    break;
    case 0107120 : {opstr = "OVER" ;}
    break;
    case 0107100 : {opstr = "SWAP" ;}
    break;
    case 0104020 : {opstr = "+" ;}
    break;
    case 0104220 : {opstr = "+c" ;}
    break;
    case 0106020 : {opstr = "-" ;}
    break;
    case 0106220 : {opstr = "-c" ;}
    break;
    case 0102020 : {opstr = "SWAP -" ;}
    break;
    case 0102220 : {opstr = "SWAP -c " ;}
    break;
    case 0103020 : {opstr = "OR" ;}
    break;
    case 0105020 : {opstr = "XOR" ;}
    break;
    case 0101020 : {opstr = "AND" ;}
    break;
    case 0100001 : {opstr = "2/" ;}
    break;
    case 0100002 : {opstr = "2*" ;}
    break;
    case 0100003 : {opstr = "0<" ;}
    break;
    case 0100011 : {opstr = "D2/" ;}
    break;
    case 0100012 : {opstr = "D2*" ;}
    break;
    case 0104411 : {opstr = "*'" ;}
    break;
    case 0102411 : {opstr = "*-" ;}
    break;
    case 0102412 : {opstr = "*F" ;}
    break;
    case 0102416 : {opstr = "/'" ;}
    break;
    case 0102414 : {opstr = '/'+'"' ;}
    break;
    case 0102616 : {opstr = "S'" ;}
    break;
    //Return stack operators
    case 0147321 : {opstr = "R>" ;}
    break;
    case 0147301 : {opstr = "R@ (#I)" ;}
    break;
    case 0140721 : {opstr = "R> DROP" ;}
    break;
    case 0157201 : {opstr = ">R" ;}
    break;
    case 0157701 : {opstr = "R> SWAP >R" ;}
    break;
    //Return stack operators
    case 0167100 : {opstr = "@" ;}
    break;
    case 0164000 : {opstr = "@ +" ;}
    break;
    case 0164200 : {opstr = "@ +c" ;}
    break;
    case 0162000 : {opstr = "@ -" ;}
    break;
    case 0162200 : {opstr = "@ -c" ;}
    break;
    case 0166000 : {opstr = "@ SWAP -" ;}
    break;
    case 0166200 : {opstr = "@ SWAP -c" ;}
    break;
    case 0163000 : {opstr = "@ OR" ;}
    break;
    case 0165000 : {opstr = "@ XOR" ;}
    break;
    case 0161000 : {opstr = "@ AND" ;}
    break;
    case 0177000 : {opstr = "!" ;}
    break;
    case 0177100 : {opstr = "DUP !" ;}
    break;
    //Full literal fetch
    case 0147500 : {opstr = " (FLF)" ;}
    *flf = true;
    break;
    case 0144400 : {opstr = " + (FLF)" ;}
    *flf = true;
    break;
    case 0144600 : {opstr = " +c (FLF)" ;}
    *flf = true;
    break;
    case 0142400 : {opstr = " - (FLF)" ;}
    *flf = true;
    break;
    case 0142600 : {opstr = " -c (FLF)" ;}
    *flf = true;
    break;
    case 0146400 : {opstr = " SWAP - (FLF)" ;}
    *flf = true;
    break;
    case 0146600 : {opstr = "n SWAP -c (FLF)" ;}
    *flf = true;
    break;
    case 0143400 : {opstr = "n OR (FLF)" ;}
    *flf = true;
    break;
    case 0145400 : {opstr = "n XOR (FLF)" ;}
    *flf = true;
    break;
    case 0141400 : {opstr = "n AND (FLF)" ;}
    *flf = true;
    break;
    //Short Literal fetch
    default: if ((opc & 32768) == 0) {
               opstr = "CALL " + itohex(opc);
             } else {
               switch ( opc / 64)
               {
                 // Data fetch and store
                 case 01647 : {opstr = "DUP @ SWAP "+itohex2(opc % 64)+" +" ;}
			     break;
                 case 01627 : {opstr = "DUP @ SWAP "+itohex2(opc % 64)+" -" ;}
			     break;
                 case 01747 : {opstr = "SWAP OVER ! "+itohex2(opc % 64)+" +" ;}
			     break;
                 case 01727 : {opstr = "SWAP OVER ! "+itohex2(opc % 64)+" -" ;}
			     break;
                 // Local data fetch and store
                 case 01471 : {opstr = itohex2(opc % 64)+" @" ;}
			     break;
                 case 01440 : {opstr = itohex2(opc % 64)+" @ +" ;}
			     break;
                 case 01442 : {opstr = itohex2(opc % 64)+" @ +c" ;}
			     break;
                 case 01420 : {opstr = itohex2(opc % 64)+" @ -" ;}
			     break;
                 case 01422 : {opstr = itohex2(opc % 64)+" @ -c" ;}
			     break;
                 case 01460 : {opstr = itohex2(opc % 64)+" @ SWAP -" ;}
			     break;
                 case 01462 : {opstr = itohex2(opc % 64)+" @ SWAP -c" ;}
			     break;
                 case 01430 : {opstr = itohex2(opc % 64)+" @ OR" ;}
			     break;
                 case 01450 : {opstr = itohex2(opc % 64)+" @ XOR" ;}
			     break;
                 case 01410 : {opstr = itohex2(opc % 64)+" @ AND" ;}
			     break;

                 case 01570 : {opstr = itohex2(opc % 64)+" !" ;}
			     break;
                 case 01511 : {opstr = "DUP "+itohex2(opc % 64)+" !" ;}
			     break;
                 case 01540 : {opstr = "DUP "+itohex2(opc % 64)+" ! +" ;}
			     break;
                 case 01560 : {opstr = "DUP "+itohex2(opc % 64)+" ! -" ;}
			     break;
                 case 01520 : {opstr = "DUP "+itohex2(opc % 64)+" ! SWAP -" ;}
			     break;
                 case 01530 : {opstr = "DUP "+itohex2(opc % 64)+" ! OR" ;}
			     break;
                 case 01550 : {opstr = "DUP "+itohex2(opc % 64)+" ! XOR" ;}
			     break;
                 case 01510 : {opstr = "DUP "+itohex2(opc % 64)+" ! AND" ;}
			     break;
                 // Internal data fetch and store
                 case 01473 : {opstr = itohex2(opc % 64)+" I@ ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01447 : {opstr = itohex2(opc % 64)+" I@ + ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01427 : {opstr = itohex2(opc % 64)+" I@ - ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01467 : {opstr = itohex2(opc % 64)+" I@ SWAP - ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01437 : {opstr = itohex2(opc % 64)+" I@ OR ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01457 : {opstr = itohex2(opc % 64)+" I@ XOR ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01417 : {opstr = itohex2(opc % 64)+" I@ AND ("+int_reg(opc % 64)+")" ;}
			     break;

                 case 01572 : {opstr = itohex2(opc % 64)+" I! ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01503 : {opstr = "DUP "+itohex2(opc % 64)+" I! ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01542 : {opstr = "DUP "+itohex2(opc % 64)+" I! + ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01562 : {opstr = "DUP "+itohex2(opc % 64)+" I! - ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01522 : {opstr = "DUP "+itohex2(opc % 64)+" I! SWAP - ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01532 : {opstr = "DUP "+itohex2(opc % 64)+" I! OR ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01552 : {opstr = "DUP "+itohex2(opc % 64)+" I! XOR ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01512 : {opstr = "DUP "+itohex2(opc % 64)+" I! AND ("+int_reg(opc % 64)+")" ;}
			     break;
                 case 01577 : {opstr = itohex2(opc % 64)+" I@! ("+int_reg(opc % 64)+")" ;}
			     break;

                 case 01575 : {opstr = itohex2(opc % 64)+" (SLF)" ;}
                             break;
                 case 01544 : {opstr = itohex2(opc % 64)+" + (SLF)" ;}
                             break;
                 case 01546 : {opstr = itohex2(opc % 64)+" +c (SLF)" ;}
			     break;
                 case 01524 : {opstr = itohex2(opc % 64)+" - (SLF)" ;}
			     break;
                 case 01526 : {opstr = itohex2(opc % 64)+" -c (SLF)" ;}
			     break;
                 case 01564 : {opstr = itohex2(opc % 64)+" SWAP - (SLF)" ;}
			     break;
                 case 01566 : {opstr = itohex2(opc % 64)+" SWAP -c (SLF)" ;}
			     break;
                 case 01534 : {opstr = itohex2(opc % 64)+" OR (SLF)" ;}
			     break;
                 case 01554 : {opstr = itohex2(opc % 64)+" XOR (SLF)" ;}
			     break;
                 case 01514 : {opstr = itohex2(opc % 64)+" AND (SLF)" ;}
			     break;
                 default:
                   switch (opc / 4096) {
                     case 9 : {opstr = "IF "+itohex(opc % 4096);}// %1001
                     break;
                     case 10 : {opstr = "LOOP "+itohex(opc % 4096);}//%1010
                     break;
                     case 11 : {opstr = "ELSE "+itohex(opc % 4096);}//%1011
                     break;
                   }
               }

             }
  }
  if (opc & 32) opstr += " ;";
  if (opstr == "") opstr = "RAW: " + itohex(opc);
  return opstr;
}

//---------------------------------------------------------------------------
AnsiString TForm1::funcname(unit op_addr)
{
  AnsiString opstr;
  char tmp[256];
  int len = 0, i;
  unit addr, val;

  
  addr = sim->read_heap(op_addr);

  if ((addr < 0x1000) || (addr > 0x2000)) {
     return ("");
  }

  // Find start of string with length in first msb byte
  while ((len = (((val = sim->read_heap(--addr)) & 0xff00) >> 8)) > 10) ;

  if (len < 1) {
    return ("");
  }

  i = 0;
  tmp[i++] = sim->read_heap(addr++) & 0xff;
  len--;

  while (len > 0) {
    tmp[i++] = (sim->read_heap(addr) & 0xff00) >> 8;
    len--;
    if (len--) {
      val = sim->read_heap(addr++) & 0xff;
      if (len == 0) {
        if ((val >= ' ') && (val < 0x80)) {
          tmp[i++] = val;
        }
      } else {
        tmp[i++] = val;
      }
    }
  }

  tmp[i] = 0;
  opstr = tmp;
  
  return (opstr);

}


//----------------------------------------------------------------------------
void TForm1::display_memory_opcode(unit baseadress)
{
  bool flf = false;
  AnsiString opkodstr, bas_adr_str, fname;
  unit pc = sim->las_reg(reg_P);
  unit opkod_adr = baseadress, opcode;


  for (int i = 0; i < (opgrid->RowCount); i++){
    flf = false;
    opkodstr = opcode_to_text(sim->read_heap(opkod_adr), &flf);
    if (flf) {
      opgrid->Cells[2][i] =
        "[" + itohex(opkod_adr) + "] " + itohex(sim->read_heap(opkod_adr+1)) +
        " " + opkodstr;
    } else {
      opcode = sim->read_heap(opkod_adr);

      if (!(opcode & 0x8000)) {
        fname = funcname (opkod_adr);
      } else {
        fname = "";
      }

      opgrid->Cells[2][i] = "[" + itohex(opkod_adr) + "] " +
                          itohex(opcode) + "  " + opkodstr + " " + fname;
    }

    if (opkod_adr == pc) opgrid->Row = i;

    if (sim->read_BP(opkod_adr) > 0) {
      char string[25];

      itoa(sim->read_BP(opkod_adr), string, 10);
      opgrid->Cells[0][i] = string;
      opgrid->Cells[1][i] = string;
    } else {
      opgrid->Cells[0][i] = "";
      opgrid->Cells[1][i] = "";
    }

    opkodadresser[i] = opkod_adr;
    opkod_adr += 1;
    if (flf) opkod_adr += 1;
  }
}
//---------------------------------------------------------------------------
void TForm1::display_memory_hexdump(unit baseadress)
{
  int pc = sim->las_reg(reg_P);
  for (int i = 0; i < hexgrid->RowCount;i++){
    hexgrid->Cells[0][i] = itohex(baseadress+i*4);
    AnsiString mem_ascii = "[";
    for (int j = 0;j<4;j++) {
      hexgrid->Cells[j+1][i] = itohex(sim->read_heap(baseadress+i*4+j));
      if (baseadress+i*4+j == pc) {
         hexgrid->Col = j+1;
         hexgrid->Row = i;
       }
      mem_ascii += give_display_char(sim->read_heap(baseadress+i*4+j) / 256);
      mem_ascii += give_display_char(sim->read_heap(baseadress+i*4+j) % 256);
    }
  	hexgrid->Cells[5][i] = mem_ascii + "]";
  }
}
//---------------------------------------------------------------------------
char TForm1::give_display_char(int charval)
{
  char retchar = '.';
  if (charval > 31 && charval < 128) retchar = charval;
  return retchar;
}
//---------------------------------------------------------------------------
void TForm1::display_memdump(void)
{
  if (rb_hexdump->Checked){
    hexgrid->BringToFront();
    basadress = (basadress /4) *4;
    hexgrid->BringToFront();
    display_memory_hexdump(basadress);
  } else {
    opgrid->BringToFront();
    display_memory_opcode(basadress);
    //if (lb_opkod->ItemIndex == -1) display_memory_opcode(basadress+1);
  }
  if (!internal_change) {
    internal_change = true;
    int pc = sim ->las_reg(reg_P);
    tb_mempos->Position = pc;
    internal_change = false;
    Edit2->Text = itohex(pc);
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::hexgridDblClick(TObject *Sender)
{
  if (opgrid->Enabled)
  if (hexgrid->Col < 5 && hexgrid->Col > 0) {
    chval_dialog = new Tchange_val_dialog(Application);
    AnsiString temstr = "Minnesposition: "+hexgrid->Cells[hexgrid->Col][hexgrid->Row];
    chval_dialog->Caption = temstr;
    chval_dialog->Editbox->Text = hexgrid->Cells[hexgrid->Col][hexgrid->Row];
    if (chval_dialog->ShowModal() == mrOk) {
      AnsiString stri = chval_dialog->Editbox->Text.Trim();
      int pos = 1;
      int retval = 0;
      while ((stri.Length() >= pos)) {
        retval *= 16;
        retval += chrtoi(stri[pos]);
        pos += 1;
      }
      hexgrid->Cells[hexgrid->Col][hexgrid->Row] = itohex(retval);
      sim->write_heap(basadress+(hexgrid->Col-1)+hexgrid->Row*4,retval);
      display_memory_hexdump(basadress);
      chval_dialog->Tchange_val_dialog::~Tchange_val_dialog();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::tb_memposChange(TObject *Sender)
{
  bool oldinternal = internal_change;
  if (!internal_change) {
    internal_change = true;
    basadress = tb_mempos->Position;
    display_memdump();
    internal_change = oldinternal;
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton2Click(TObject *Sender)
{
  internal_change = true;

  if (rb_hexdump->Checked) basadress += 4; else basadress += 1;
  tb_mempos->Position = basadress;
  display_memdump();

  internal_change = false;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::rb_opkodClick(TObject *Sender)
{
  display_memdump();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::SpeedButton1Click(TObject *Sender)
{

  internal_change = true;

  if (rb_hexdump->Checked) basadress -= 4; else basadress -= 1;

  tb_mempos->Position = basadress;
  display_memdump();

  internal_change = false;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Edit2Exit(TObject *Sender)
{
  AnsiString stri = Edit2->Text.Trim();
  int pos = 1;
  int retval = 0;


  internal_change = true;

  while ((stri.Length() >= pos)) {
    retval *= 16;
    retval += chrtoi(stri[pos]);
    pos += 1;
  }
  basadress = retval;
  display_memdump();

  tb_mempos->Position = basadress;

  internal_change = false;

}
//---------------------------------------------------------------------------
void TForm1::update_gui(void)
{
  read_novix_registers();
  read_novix_datastack();
  read_novix_returnstack();
  display_memdump();
}

void __fastcall TForm1::FormShow(TObject *Sender)
{
  update_gui();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::opgridDrawCell(TObject *Sender, int ACol, int ARow,
      TRect &Rect, TGridDrawState State)
{
  //long index = Row * DrawGrid1->ColCount + Col;
  switch (ACol) {
    case 0:
      opgrid->Canvas->Brush->Color = clBtnFace;
      opgrid->Canvas->FillRect(Rect);
      if (opgrid->Cells[0][ARow] != "") {
        ImageList1->Draw(opgrid->Canvas,Rect.Left+2,Rect.Top+5,0,true);
      }
      if (State.Contains(gdFocused))
      opgrid->Canvas->DrawFocusRect(Rect);
      break;
    case 1:
      if (!opgrid->Enabled) opgrid->Canvas->Font->Color = clGrayText;
      opgrid->Canvas->Brush->Color = clBtnFace;
      opgrid->Canvas->FillRect(Rect);
      if (opgrid->Cells[0][ARow] != "") {
        opgrid->Canvas->TextOutA(Rect.Left+2,Rect.Top+2,opgrid->Cells[0][ARow]);
      }
      if (State.Contains(gdFocused))
      opgrid->Canvas->DrawFocusRect(Rect);
      break;
    case 2:
      if (opkodadresser[ARow] == sim->las_reg(reg_P)) {
        opgrid->Canvas->Brush->Color = clHighlight;
        opgrid->Canvas->Font->Color = clWindow;
      } else {
        opgrid->Canvas->Brush->Color = clWindow;
        opgrid->Canvas->Font->Color = clBlack;
      }
      if (!opgrid->Enabled) {
        opgrid->Canvas->Brush->Color = clBtnFace;
        opgrid->Canvas->Font->Color = clGrayText;
      }
      opgrid->Canvas->FillRect(Rect);
      opgrid->Canvas->TextOutA(Rect.Left+2,Rect.Top+2,opgrid->Cells[2][ARow]);
      if (State.Contains(gdFocused))
      opgrid->Canvas->DrawFocusRect(Rect);
      break;
  }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::opgridMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if (opgrid->Enabled) {
    int Row = Y / (opgrid->DefaultRowHeight);

    if (Row > (opgrid->RowCount-1))
       return;

    if (X < 15) {
     if (sim->read_BP(opkodadresser[Row]) > 0 ) {
       sim->write_BP(opkodadresser[Row],0);
       opgrid->Cells[0][Row] = "";
       opgrid->Cells[1][Row] = "";
     } else {
 //      ShowMessage(itohex(opkodadresser[Row]));
       sim->write_BP(opkodadresser[Row],1);
       opgrid->Cells[0][Row] = "1";
       opgrid->Cells[1][Row] = "";
     }
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::opgridDblClick(TObject *Sender)
{
  if (opgrid->Enabled)
  if (opgrid->Col == 1) {
    chval_dialog = new Tchange_val_dialog(Application);
    AnsiString temstr = "BP #, Adr:"+itohex(opkodadresser[opgrid->Row]);
    chval_dialog->Caption = temstr;
    chval_dialog->Editbox->Text = AnsiString(sim->read_BP(opkodadresser[opgrid->Row]));
    if (chval_dialog->ShowModal() == mrOk) {
      AnsiString stri = chval_dialog->Editbox->Text.Trim();
      int pos = 1;
      int retval = 0;
      while ((stri.Length() >= pos)) {
        retval *= 10;
        retval += chrtoi(stri[pos]);
        pos += 1;
      }
      sim->write_BP(opkodadresser[opgrid->Row],retval);
      if (retval == 0) {
        opgrid->Cells[0][opgrid->Row] = "";
        opgrid->Cells[1][opgrid->Row] = "";
      } else {
        opgrid->Cells[0][opgrid->Row] = AnsiString(retval);
        opgrid->Cells[1][opgrid->Row] = AnsiString(retval);
      }
      chval_dialog->Tchange_val_dialog::~Tchange_val_dialog();
    }
  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::AfterRun(TObject *Sender)
{
  run_mode(false);
  basadress = sim->las_reg(reg_P)-6;
  update_gui();
}
//---------------------------------------------------------------------------
void TForm1::run_mode(bool Running)
{
  TColor gridcol;
  if (Running) gridcol = clBtnFace; else gridcol = clWindow;
  ds_grid->Color = gridcol;
  ds_grid->Enabled = !Running;
  rs_grid->Color = gridcol;
  rs_grid->Enabled = !Running;
  reg_grid->Color = gridcol;
  reg_grid->Enabled = !Running;
  SpeedButton1->Enabled = !Running;
  SpeedButton2->Enabled = !Running;
  tb_mempos->Enabled = !Running;
  opgrid->Color = gridcol;
  opgrid->Enabled = !Running;
  rb_hexdump->Enabled = !Running;
  rb_opkod->Enabled = !Running;
  Edit2->Enabled = !Running;
  bu_singlestep->Enabled = !Running;
  bu_run->Enabled = !Running;
  bu_stop->Enabled = Running;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::bu_singlestepClick(TObject *Sender)
{
 int pc;


 sim->en_klockcykel();

 internal_change = true;
 
 pc = sim->las_reg(reg_P);

 if ((pc < (basadress + 5)) || (pc > ((basadress + opgrid->RowCount) - 5))) {
  basadress = sim->las_reg(reg_P)-5;
 }

 update_gui();

 tb_mempos->Position = pc;

 internal_change = false;

}

//---------------------------------------------------------------------------
void __fastcall TForm1::bu_runClick(TObject *Sender)
{
  stop_running = false;
  run_mode(true);
  RunThread *runtrad;
  runtrad = new RunThread(false);
  runtrad->simul = sim;
  runtrad->Resume();
  runtrad->OnTerminate = AfterRun;
}
//---------------------------------------------------------------------------

__fastcall RunThread::RunThread(bool CreateSuspended)
        : TThread(CreateSuspended)
{
}
//---------------------------------------------------------------------------
void __fastcall RunThread::Execute()
{
  simul->run();
}



void __fastcall TForm1::bu_stopClick(TObject *Sender)
{
  stop_running = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Memo1KeyPress(TObject *Sender, char &Key)
{
  if (Key != '\r') putcharinqueue(Key); else putcharinqueue(13);
  Key = 0;
}
//---------------------------------------------------------------------------
char TForm1::chin()
{
  char retval = 0;
  if (CharHead != NULL) {
    retval = (*CharHead).character;
    LinkedChar* NextChar;
    NextChar = (*CharHead).next;
    delete(CharHead);
    CharHead = NextChar;
  }
  return retval;
}
//---------------------------------------------------------------------------
void TForm1::putcharinqueue(char c){
  LinkedChar* NewChar;
  NewChar = new(LinkedChar);
  (*NewChar).next = NULL;
  (*NewChar).character = c;
  if (CharHead != NULL) {
    LinkedChar LastChar = *CharHead;
    while (LastChar.next != NULL) LastChar = *LastChar.next;
    LastChar.next = NewChar;
  } else CharHead = NewChar;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::bu_xportClick(TObject *Sender)
{
  AnsiString stri = Edit3->Text.Trim();
  int pos = 1;
  int retval = 0;
  while ((stri.Length() >= pos)) {
    retval *= 16;
    retval += chrtoi(stri[pos]);
    pos += 1;
  }
  sim->write_x_port(retval);
  update_gui();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::bubportClick(TObject *Sender)
{
  AnsiString stri = Edit4->Text.Trim();
  int pos = 1;
  int retval = 0;
  while ((stri.Length() >= pos)) {
    retval *= 16;
    retval += chrtoi(stri[pos]);
    pos += 1;
  }
  sim->write_b_port(retval);
  update_gui();
}
//---------------------------------------------------------------------------



void __fastcall TForm1::Edit2Change(TObject *Sender)
{
  Edit2Exit (Sender);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::FormKeyPress(TObject *Sender, char &Key)
{
  if (Key != '\r') putcharinqueue(Key); else putcharinqueue(13);
  Key = 0;
}
//---------------------------------------------------------------------------

