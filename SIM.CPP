//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("sim.res");
USEUNIT("Sim1.cpp");
USEFORM("gui.cpp", Form1);
USEFORM("gui_change_reg_dialog.cpp", change_val_dialog);
USEUNIT("GUIProj.cpp");
USEUNIT("nc4k.cpp");
USEUNIT("General.cpp");
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
