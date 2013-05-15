#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/filepicker.h>
#include <wx/frame.h>
#include <wx/aboutdlg.h>
#include <wx/xml/xml.h>
#include <wx/spinctrl.h>
#include "backup.h"

#define C_STR(str) const_cast<char*>((const char*)(str).mb_str())

class Window: public wxFrame {
  public:
    Window(wxFrame* frame, const wxString& title);
    Window();
    void EnableGUI(bool enabled);
    void EnableTotalGUI(bool enabled);
    void ReadSettings();
    void WriteSettings();
    void ReadDevices();

    void SetHideDev(bool h);
    void SetHideDevSize(unsigned long s);
    void SetCheckRoot(bool c);
    void SetBSSize(unsigned long s);

    void StopBackup();


  private:
    void OnQuit(wxCommandEvent& event);
    void DoInstall(wxCommandEvent& event);
    void DeviceSelected(wxListEvent& event);
    void DeviceDeselected(wxListEvent& event);
    void ShowAbout(wxCommandEvent& event);
    void ShowSettings(wxCommandEvent& event);
    void CreateBackup(wxCommandEvent& event);
    void RestoreBackup(wxCommandEvent& event);
    void ReadURLs();
    void CopyDevice(FILE* in, FILE* out, long long kbytes, BackupProgress* b);

    unsigned long bootsector_size;
    unsigned long hidesize;
    bool hidedevice;
    bool checkroot;
    char url_kernel[512];
    char url_bootloader[512];
    char url_rootfs[512];
    wxString display_url;
    volatile int stop_backup;

    wxStaticText* label_select_device;
    wxListCtrl* listctrl_device;
    wxCheckBox* check_repartition;
    wxStaticLine* line_1;
    wxRadioButton* boot_no_change;
    wxRadioButton* boot_net;
    wxRadioButton* boot_file;
    wxFilePickerCtrl* file_bootloader;
    wxRadioButton* root_no_change;
    wxRadioButton* kernel_no_change;
    wxRadioButton* root_net;
    wxRadioButton* root_file;
    wxFilePickerCtrl* file_rootfs;
    wxRadioButton* krnl_net;
    wxRadioButton* krnl_file;
    wxFilePickerCtrl* file_zimage;
    wxStdDialogButtonSizer* btn_apply;
    wxButton* btn_applyApply;
    wxMenuBar* main_menu;
    wxMenu* menu_installer;
    wxMenu* menu_backup;
    wxMenu* menu_about;
    wxMenuItem* mitem_create_backup;
    wxMenuItem* mitem_restore_backup;


    //DECLARE_EVENT_TABLE()


};

class installer : public wxApp {
  public:
    virtual bool OnInit();
};




