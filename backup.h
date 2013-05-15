#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/frame.h>

class BackupProgress : public wxFrame {
  private:
    void OnQuit(wxCloseEvent& event);
    void OnClose(wxCommandEvent& event);

  protected:
    wxStaticText* text_backup_info;
    wxStaticText* m_staticText8;
    wxStaticText* txt_remain;
    wxGauge* progress_backup;
    wxStdDialogButtonSizer* m_sdbSizer4;
    wxButton* m_sdbSizer4Cancel;

  public:

    BackupProgress(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(397, 174), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
    ~BackupProgress();
    void SetProgress(int progress);
    void SetRemainingTime(wxString time);
    void SetTextCreate();
    void SetTextRestore();

};

#endif
