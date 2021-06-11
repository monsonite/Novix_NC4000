//---------------------------------------------------------------------------
#include <vcl\vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("gui.cpp", Form1);
USERES("GUIProj.res");
USEUNIT("Sim1.cpp");
USEUNIT("nc4k.cpp");
USEUNIT("general.cpp");
USEFORM("gui_change_reg_dialog.cpp", change_val_dialog);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
                 Application->CreateForm(__classid(Tchange_val_dialog), &change_val_dialog);
                 Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
