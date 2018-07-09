//---------------------------------------------------------------------------
#ifndef LabelledImageH
#define LabelledImageH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "ImageDocuments.h"
#include "GraphicsMode.h"
//---------------------------------------------------------------------------
class TfrmLabelledImage : public TFrame
{
__published:	// IDE-managed Components
    TPanel *panImage;
    TImage *imgImage;
    TLabel *lblCaption;
    void __fastcall imgImageClick(TObject *Sender);
private:	// User declarations
    TNotifyEvent            FOnClick;
    ImageDocument*          m_Document;
const Agdx::GraphicsMode&   m_GraphicsMode;     // the graphics mode used by the project

    void        __fastcall  SetSelected(bool state);
    void        __fastcall  SetShowCaption(bool state);
    void        __fastcall  SetImage(ImageDocument* document);
    void        __fastcall  Update();

public:		// User declarations
                __fastcall TfrmLabelledImage(TComponent* Owner);

    bool        __property  Selected = { write = SetSelected };
__property  ImageDocument*  Image = { read = m_Document, write = SetImage };
    bool        __property  ShowCaption = { write = SetShowCaption };

            // click events
   __property  TNotifyEvent OnSelectedClick = { read = FOnClick, write = FOnClick };
};
//---------------------------------------------------------------------------
#endif
