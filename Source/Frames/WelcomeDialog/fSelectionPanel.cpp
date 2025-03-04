//---------------------------------------------------------------------------
#include "AGD Studio.pch.h"
//---------------------------------------------------------------------------
#include <Vcl.Imaging.pngimage.hpp>
#include "fSelectionPanel.h"
#include "Project/Documents/MachineConfig.h"
#include "Settings/ThemeManager.h"
#include "Services/File.h"
#include "Services/Folders.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TSelectionPanelFrame::TSelectionPanelFrame(TComponent* Owner)
: TFrame(Owner)
, m_Selected(false)
, m_Highlighted(false)
, FOnClick(nullptr)
, FOnRemoveClick(nullptr)
{
    panProjectInfo->Color = ThemeManager::Background;       //StyleServices()->GetStyleColor(scGenericGradientEnd);
    panRemove->Color = ThemeManager::Background;            //StyleServices()->GetStyleColor(scGenericGradientEnd);
    lblProjectName->Font->Color = ThemeManager::Foreground; //StyleServices()->GetStyleFontColor(sfSmCaptionTextNormal);
    lblProjectPath->Font->Color = ThemeManager::Foreground; //StyleServices()->GetStyleFontColor(sfSmCaptionTextNormal);
    imgRemove->Visible = false;
    imgOpenProjectFolder->Visible = false;
}
//---------------------------------------------------------------------------
__fastcall TSelectionPanelFrame::~TSelectionPanelFrame()
{
    Parent = nullptr;
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::panProjectInfoMouseMove(TObject* /*Sender*/, TShiftState /*Shift*/, int /*X*/, int /*Y*/)
{
    Highlighted = true;
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetName(String name)
{
    m_Name = name;
    lblProjectName->Caption = name;
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetPath(String path)
{
    m_Path = path;
    lblProjectPath->Caption = Services::File::PathOf(path);

    // check for a custom project image in the project folder, else we'll use the machines default image
    auto file = Services::File::Combine(Services::File::Combine(Services::Folders::Projects, m_Name), m_Name + ".png");
    if (Services::File::Exists(file)) {
        auto image = std::make_unique<TPngImage>();
        image->LoadFromFile(file);
        imgLogo->Picture->Assign(image.get());
        imgLogo->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetMachine(String machine)
{
    m_Machine = machine;
    lblMachine->Caption = machine;

    if (!imgLogo->Visible) {
        // a png file with an image of the machine 120x56
        auto mc = std::make_unique<Project::MachineConfig>(machine);
        mc->LoadFile(machine);
        auto imgFile = Services::File::Combine(Services::Folders::Application + Services::Folders::Separator + "Images" + Services::Folders::Separator + m_Theme, mc->Name + ".png");
        if (!Services::File::Exists(imgFile)) {
            imgFile = Services::File::Combine(Services::Folders::Application + Services::Folders::Separator + "Images" + Services::Folders::Separator + "Logos", mc->Name + ".png");
        }
        if (Services::File::Exists(imgFile)) {
            if (Services::File::Exists(imgFile)) {
                auto image = std::make_unique<TPngImage>();
                image->LoadFromFile(imgFile);
                imgLogo->Picture->Assign(image.get());
                imgLogo->Visible = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetTheme(String folder)
{
    // a png file with an image of the machine 120x56
    m_Theme = "Logos";
    auto path = Services::Folders::Application + Services::Folders::Separator + "Images" + Services::Folders::Separator + m_Theme;
    if (Services::Folders::Exists(path)) {
        m_Theme = folder;
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetSelected(bool state)
{
    m_Selected = state;
    UpdateControl();
    if (state) {
        // change all other TSelectionPanelFrames
        for (int i = 0; i < Parent->ControlCount; i++) {
            TSelectionPanelFrame* frame = dynamic_cast<TSelectionPanelFrame*>(Parent->Controls[i]);
            if (frame != nullptr && frame != this) {
                frame->Selected = false;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetHighlighted(bool state)
{
    m_Highlighted = state;
    UpdateControl();
    if (state) {
        // change all other TSelectionPanelFrames
        for (int i = 0; i < Parent->ControlCount; i++) {
            TSelectionPanelFrame* frame = dynamic_cast<TSelectionPanelFrame*>(Parent->Controls[i]);
            if (frame != nullptr && frame != this) {
                frame->Highlighted = false;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::SetLoading(bool state)
{
    prgLoading->Visible = state;
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::UpdateControl()
{
    TColor color = m_Selected ? ThemeManager::Highlight : (m_Highlighted ? ThemeManager::Highlight : ThemeManager::Background);
    panProjectInfo->Color = color;
    panRemove->Color = color;
    imgRemove->Visible = m_Highlighted;
    imgOpenProjectFolder->Visible = m_Highlighted;
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::Tick()
{
    prgLoading->StepIt();
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::lblProjectNameClick(TObject* /*Sender*/)
{
    Selected = true;
    if (FOnClick != nullptr) FOnClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::imgRemoveClick(TObject* /*Sender*/)
{
    if (FOnRemoveClick != nullptr) FOnRemoveClick(this);
}
//---------------------------------------------------------------------------
void __fastcall TSelectionPanelFrame::imgOpenProjectFolderClick(TObject* /*Sender*/)
{
    auto folder = Services::File::Combine(Services::Folders::Projects, m_Name);
    ShellExecute(nullptr, L"open", nullptr, nullptr, folder.c_str(), SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------

