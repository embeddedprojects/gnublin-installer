#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/frame.h>

#ifndef VERSION
#warning VERSION is not defined!
#define VERSION "0.0-development"
#endif

class SettingsFrame : public wxFrame {
  private:

  protected:
    void SaveSettings(wxCommandEvent& event);
    void OnClose(wxCommandEvent& event);
    wxCheckBox* set_hidedev;
    wxSpinCtrl* set_hidesize;
    wxStaticText* set_unit;
    wxStaticText* set_bs_label;
    wxSpinCtrl* set_bs_size;
    wxCheckBox* set_checkroot;
    wxStdDialogButtonSizer* set_btn;
    wxButton* set_btnSave;
    wxButton* set_btnCancel;

  public:

    SettingsFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 242), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~SettingsFrame();
    void SetHideDev(bool h);
    void SetHideDevSize(unsigned long s);
    void SetCheckRoot(bool c);
    void SetBSSize(unsigned long s);

};


#endif

