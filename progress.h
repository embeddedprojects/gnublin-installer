#ifndef _PROGRESS_H_
#define _PROGRESS_H_

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gauge.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/wx.h>


class InstallerFrame : public wxFrame {
  private:
    void OnQuit(wxCloseEvent& event);


  protected:
    wxStaticText* inst_text;
    wxGauge* inst_progr;
    wxListBox* inst_log;
    wxStdDialogButtonSizer* inst_ok;
    wxButton* inst_okOK;
    int log_entries;

  public:

    InstallerFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL | wxFRAME_FLOAT_ON_PARENT);
    ~InstallerFrame();
    void AddLog(wxString log);
    void SetProgress(int progress);

};


#endif

