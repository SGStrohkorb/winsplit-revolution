#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/dir.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include "dialog_activewndtools.h"
#include "hook.h"

void ActiveWndToolsDialog::ShowDialog()
{
  ActiveWndToolsDialog dlg(NULL, GetForegroundWindow());
  dlg.ShowModal();
}

ActiveWndToolsDialog::ActiveWndToolsDialog(wxWindow* parent, HWND activeWindow)
    : wxDialog(parent, -1, _("Tools for the active Window"), wxDefaultPosition, wxDefaultSize,
               wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)
{
  m_hActiveWindow = activeWindow;
  m_sTmpFName = wxString(wxTheApp->argv[0]);
  m_sTmpFName.AppendDir(_T ("screenshots"));
  if (!m_sTmpFName.DirExists())
    m_sTmpFName.Mkdir(0777, wxPATH_MKDIR_FULL);
  // Check if the window already has the style ALWAYS_ON_TOP
  long lStyle = ::GetWindowLong(m_hActiveWindow, GWL_EXSTYLE);
  m_bIsAlwaysOnTop = ((lStyle & WS_EX_TOPMOST) == WS_EX_TOPMOST);
  // Recover transparency, if possible
  GetTransparencyValues(m_hActiveWindow, m_bIsTransparent, m_iTranparency);

  GetScreenShots();
  CreateControls();
  CreateConnexions();
}

ActiveWndToolsDialog::~ActiveWndToolsDialog()
{
  // dtor
}

void ActiveWndToolsDialog::GetScreenShots()
{
  RECT rc;
  GetWindowRect(m_hActiveWindow, &rc);
  int w = rc.right - rc.left;
  int h = rc.bottom - rc.top;
  // HDC wndDC = GetWindowDC (m_hActiveWindow);
  wxWindow dummy;
  dummy.SetHWND(m_hActiveWindow);
  wxWindowDC dc(&dummy);
  // dc.SetHDC (wndDC);
  m_bmpWindow = wxBitmap(w, h, -1);
  wxMemoryDC mdc(m_bmpWindow);
  mdc.Blit(0, 0, w, h, &dc, 0, 0);
  mdc.SelectObject(wxNullBitmap);
  // ReleaseDC (m_hActiveWindow, wndDC);
}

void ActiveWndToolsDialog::CreateControls()
{
  // Create temporary wxBitmap for preview, max 250x200
  wxBitmap tmpBmp;
  wxImage tmpImg(m_bmpWindow.ConvertToImage());
  int wdth = m_bmpWindow.GetWidth();
  int hght = m_bmpWindow.GetHeight();
  double rX = 250. / wdth;
  double rY = 200. / hght;
  // Get the smallest ratio between rX and rY
  if (rX < rY) {
    tmpBmp = wxBitmap(tmpImg.Scale(250, int(rX * hght)));
  }
  else {
    tmpBmp = wxBitmap(tmpImg.Scale(int(rY * wdth), 200));
  }

  // Used for creating all wxStaticText
  wxStaticText* label;

  // The main wxSizer of the window
  wxBoxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);

  // The wxSizer corresponding to the first "block" of controls (changing the style of the window)
  wxStaticBoxSizer* box1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Window style : "));

  // 1st line: enable / disable Always on Top
  wxBoxSizer* b1line1 = new wxBoxSizer(wxHORIZONTAL);
  label = new wxStaticText(this, -1, _("Enable or disable the 'Always on top' style :"));
  b1line1->Add(label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  wxString sBtnTxt = (m_bIsAlwaysOnTop ? _("Disable") : _("Enable"));
  p_btnAlwaysOnTop = new wxButton(this, -1, sBtnTxt);
  b1line1->Add(p_btnAlwaysOnTop, 0, wxALL, 5);
  box1->Add(b1line1, 0, wxALL | wxEXPAND, 0);

  // 2nd line: the checkbox to enable or disable transparency
  p_chkEnableTransp = new wxCheckBox(this, -1, _("Enable transparency for this window"));
  box1->Add(p_chkEnableTransp, 0, wxALL | wxEXPAND, 5);
  p_chkEnableTransp->SetValue(m_bIsTransparent);

  // 3rd line: set transparency if it is enabled
  wxBoxSizer* b1line3 = new wxBoxSizer(wxHORIZONTAL);
  p_lblEnableTransp = new wxStaticText(this, -1, _("Change opacity :"));
  p_lblEnableTransp->Enable(m_bIsTransparent);
  b1line3->Add(p_lblEnableTransp, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
  p_sldTranparency = new wxSlider(this, -1, m_iTranparency, 50, 255);
  p_sldTranparency->Enable(m_bIsTransparent);
  b1line3->Add(p_sldTranparency, 1, wxALL, 5);
  box1->Add(b1line3, 0, wxALL | wxEXPAND, 0);
  mainsizer->Add(box1, 0, wxALL | wxEXPAND, 5);
  wxStaticBoxSizer* box2 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Screenshot : "));
  p_stbPreview = new wxStaticBitmap(this, -1, tmpBmp);
  box2->Add(p_stbPreview, 0, wxALL, 5);
  wxBoxSizer* szrActions = new wxBoxSizer(wxVERTICAL);
  wxString sInfos;
  sInfos.Printf(_("Image size : %0dx%0d"), wdth, hght);
  label = new wxStaticText(this, -1, sInfos);
  szrActions->Add(label, 0, wxALL | wxEXPAND, 5);
  p_btnSaveAs = new wxButton(this, -1, _("Save as image file"));
  szrActions->Add(p_btnSaveAs, 0, wxALL | wxEXPAND, 5);
  p_btnClipboard = new wxButton(this, -1, _("Copy to clipboard"));
  szrActions->Add(p_btnClipboard, 0, wxALL | wxEXPAND, 5);
  box2->Add(szrActions, 0, wxALL | wxEXPAND, 0);
  mainsizer->Add(box2, 0, wxALL | wxEXPAND, 5);
  SetSizer(mainsizer);
  mainsizer->SetSizeHints(this);
}

void ActiveWndToolsDialog::CreateConnexions()
{
  p_btnSaveAs->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                       wxCommandEventHandler(ActiveWndToolsDialog::OnSaveAsImageFile),
                       NULL,
                       this);
  p_btnClipboard->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                          wxCommandEventHandler(ActiveWndToolsDialog::OnCopyToClipboard),
                          NULL,
                          this);
  p_btnAlwaysOnTop->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                            wxCommandEventHandler(ActiveWndToolsDialog::OnChangeTopMostStyle),
                            NULL,
                            this);
  p_chkEnableTransp->Connect(
      wxEVT_COMMAND_CHECKBOX_CLICKED,
      wxCommandEventHandler(ActiveWndToolsDialog::OnEnableDisableTransparency),
      NULL,
      this);
  p_sldTranparency->Connect(wxEVT_SCROLL_CHANGED,
                            wxCommandEventHandler(ActiveWndToolsDialog::OnTransparencyChanged),
                            NULL,
                            this);
  p_sldTranparency->Connect(wxEVT_SCROLL_THUMBTRACK,
                            wxCommandEventHandler(ActiveWndToolsDialog::OnTransparencyChanged),
                            NULL,
                            this);
  p_stbPreview->Connect(
      wxEVT_MOTION, wxMouseEventHandler(ActiveWndToolsDialog::OnMouseMove), NULL, this);
  p_stbPreview->Connect(
      wxEVT_LEFT_DOWN, wxMouseEventHandler(ActiveWndToolsDialog::OnLeftButtonDown), NULL, this);
}

void ActiveWndToolsDialog::OnSaveAsImageFile(wxCommandEvent& event)
{
  wxString sWldCrds =
      _T ("PNG files (*.png)|*.png|JPEG files (*.jpg;*.jpeg)|*.jpg;*.jpeg|BMP files (*.bmp)|*.bmp");
  wxString sMsg = _("Enter the name of the file to save.");
  long style = wxFD_SAVE | wxFD_OVERWRITE_PROMPT;

  wxFileDialog fdlg(this, sMsg, _T (""), _T ("ScreenShot"), sWldCrds, style);
  fdlg.SetFilterIndex(0);
  if (fdlg.ShowModal() == wxID_CANCEL)
    return;

  int index = fdlg.GetFilterIndex();

  wxBitmapType type =
      (index == 0 ? wxBITMAP_TYPE_PNG : (index == 1 ? wxBITMAP_TYPE_JPEG : wxBITMAP_TYPE_BMP));

  m_bmpWindow.SaveFile(fdlg.GetPath(), type);
}

void ActiveWndToolsDialog::OnCopyToClipboard(wxCommandEvent& event)
{
  if (wxTheClipboard->Open()) {
    wxTheClipboard->SetData(new wxBitmapDataObject(m_bmpWindow));
    wxTheClipboard->Close();
    wxMessageBox(
        _("The image has been copied into the clipboard."), _("Success"), wxICON_INFORMATION);
  }
  else {
    wxMessageBox(_("There was an error while opening the clipboard !"), _("Error"), wxICON_ERROR);
  }
}

void ActiveWndToolsDialog::OnChangeTopMostStyle(wxCommandEvent& event)
{
  HWND newStyle;
  wxString sBtnLabel;
  if (m_bIsAlwaysOnTop) {
    newStyle = HWND_NOTOPMOST;
    sBtnLabel = _("Enable");
    m_bIsAlwaysOnTop = false;
  }
  else {
    newStyle = HWND_TOPMOST;
    sBtnLabel = _T ("Disable");
    m_bIsAlwaysOnTop = true;
  }

  SetWindowPos(m_hActiveWindow, newStyle, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  p_btnAlwaysOnTop->SetLabel(sBtnLabel);
  /*
   * Without the command below, the window disappears under the active window when applying the
   * "Always on Top" style
   */
  Raise();
}

void ActiveWndToolsDialog::OnEnableDisableTransparency(wxCommandEvent& event)
{
  bool bNewVal = p_chkEnableTransp->GetValue();
  p_lblEnableTransp->Enable(bNewVal);
  p_sldTranparency->Enable(bNewVal);
  long lStyle = ::GetWindowLong(m_hActiveWindow, GWL_EXSTYLE);
  if (bNewVal)
    lStyle = lStyle | WS_EX_LAYERED;
  else
    lStyle = lStyle & !WS_EX_LAYERED;
  SetWindowLong(m_hActiveWindow, GWL_EXSTYLE, lStyle);
}

void ActiveWndToolsDialog::OnTransparencyChanged(wxCommandEvent& event)
{
  SetLayeredWindowAttributes(m_hActiveWindow, 0, p_sldTranparency->GetValue(), LWA_ALPHA);
}

void ActiveWndToolsDialog::OnCancel(wxCommandEvent& event)
{
  EndModal(wxOK);
}

void ActiveWndToolsDialog::DeleteTempFiles()
{
  wxFileName fname = wxString(wxTheApp->argv[0]);
  fname.AppendDir(_T ("screenshots"));
  if (!fname.DirExists())
    return;
  wxArrayString files;
  if (wxDir::GetAllFiles(fname.GetPath(wxPATH_GET_VOLUME), &files, _T (""), wxDIR_FILES)) {
    for (size_t i = 0; i < files.GetCount(); i++)
      wxRemoveFile(files[i]);
  }
}

void ActiveWndToolsDialog::OnLeftButtonDown(wxMouseEvent& event)
{
  m_bIsDragging = false;
}

void ActiveWndToolsDialog::OnMouseMove(wxMouseEvent& event)
{
  // If the left mouse button is not pressed bail out even if the drag has already begun
  if ((!event.LeftIsDown()) || (m_bIsDragging))
    return;

  // If this is the first move with the left button pressed the start drag and drop
  if (!m_bIsDragging) {
    // Create the temporary file
    m_sTmpFName.SetFullName(wxString::Format(_T ("scr%0ld.png"), time(NULL)));
    m_bmpWindow.SaveFile(m_sTmpFName.GetFullPath(), wxBITMAP_TYPE_PNG);

    // Start drag and drop
    wxFileDataObject data;
    data.AddFile(m_sTmpFName.GetFullPath());
    wxDropSource source(this);
    source.SetData(data);
    wxDragResult res = source.DoDragDrop(wxDrag_DefaultMove);

    // Delete the temp file depending on the result
    if ((res == wxDragMove) || (res == wxDragCancel) || (res == wxDragNone) || (res == wxDragError))
      wxRemoveFile(m_sTmpFName.GetFullPath());
  }
}
