/*  installer.cpp
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * GNUBLIN Installer
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net.h"
#include "disk.h"
#include "archive.h"
#include "settings.h"
#include "progress.h"
#include "backup.h"
#include "installer.h"

#define mount_point "/tmp/SDCard"

IMPLEMENT_APP(installer);

Window* frame;

bool installer::OnInit() {
  frame = new Window(0l, _("GNUBLIN Installer"));
  frame->Show();
  return true;
}

Window::Window(wxFrame* frame, const wxString& title) : wxFrame(frame, -1, title) {
  ReadURLs();

  this->SetSizeHints(640, 510);

  wxBoxSizer* main_sizer;
  main_sizer = new wxBoxSizer(wxVERTICAL);

  label_select_device = new wxStaticText(this, wxID_ANY, _("Select Device"), wxDefaultPosition, wxDefaultSize, 0);
  label_select_device->Wrap(-1);
  label_select_device->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString));

  main_sizer->Add(label_select_device, 0, wxALL, 5);

  listctrl_device = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 100), wxLC_REPORT);
  main_sizer->Add(listctrl_device, 0, wxALL | wxEXPAND, 5);

  check_repartition = new wxCheckBox(this, wxID_ANY, _("Re-Partition"), wxDefaultPosition, wxDefaultSize, 0);
  check_repartition->SetValue(true);
  main_sizer->Add(check_repartition, 0, wxALL, 5);

  line_1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
  main_sizer->Add(line_1, 0, wxEXPAND | wxALL, 5);

  wxGridSizer* bl_krnl_sizer;
  bl_krnl_sizer = new wxGridSizer(1, 2, 0, 0);

  wxStaticBoxSizer* layout_bootloader;
  layout_bootloader = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Bootloader")), wxVERTICAL);

  boot_no_change = new wxRadioButton(this, wxID_ANY, _("do not change"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  layout_bootloader->Add(boot_no_change, 0, wxALL, 5);

  boot_net = new wxRadioButton(this, wxID_ANY, _("fetch from http://gnublin.org/"), wxDefaultPosition, wxDefaultSize, 0);
  layout_bootloader->Add(boot_net, 0, wxALL, 5);

  wxFlexGridSizer* sizer_boot;
  sizer_boot = new wxFlexGridSizer(1, 2, 0, 0);
  sizer_boot->AddGrowableCol(1);
  sizer_boot->SetFlexibleDirection(wxBOTH);
  sizer_boot->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  boot_file = new wxRadioButton(this, wxID_ANY, _("use file"), wxDefaultPosition, wxDefaultSize, 0);
  sizer_boot->Add(boot_file, 0, wxALL, 5);

  file_bootloader = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxSize(-1, -1), wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST | wxFLP_OPEN);
  sizer_boot->Add(file_bootloader, 0, wxALL | wxEXPAND, 5);

  layout_bootloader->Add(sizer_boot, 0, wxEXPAND, 5);

  bl_krnl_sizer->Add(layout_bootloader, 0, wxALL | wxEXPAND, 5);

  wxStaticBoxSizer* layout_rootfs;
  layout_rootfs = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("RootFS")), wxVERTICAL);

  root_no_change = new wxRadioButton(this, wxID_ANY, _("do not change"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  layout_rootfs->Add(root_no_change, 0, wxALL, 5);

  root_net = new wxRadioButton(this, wxID_ANY, _("fetch from http://gnublin.org/"), wxDefaultPosition, wxDefaultSize, 0);
  layout_rootfs->Add(root_net, 0, wxALL, 5);

  wxFlexGridSizer* sizer_root;
  sizer_root = new wxFlexGridSizer(1, 2, 0, 0);
  sizer_root->AddGrowableCol(1);
  sizer_root->SetFlexibleDirection(wxBOTH);
  sizer_root->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  root_file = new wxRadioButton(this, wxID_ANY, _("use file"), wxDefaultPosition, wxDefaultSize, 0);
  sizer_root->Add(root_file, 0, wxALL, 5);

  file_rootfs = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxSize(-1, -1), wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST | wxFLP_OPEN);
  sizer_root->Add(file_rootfs, 0, wxALL | wxEXPAND, 5);

  layout_rootfs->Add(sizer_root, 0, wxEXPAND, 5);

  bl_krnl_sizer->Add(layout_rootfs, 0, wxEXPAND | wxALL, 5);

  main_sizer->Add(bl_krnl_sizer, 0, wxEXPAND, 5);

  wxStaticBoxSizer* layout_kernel;
  layout_kernel = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Kernel")), wxVERTICAL);

  kernel_no_change = new wxRadioButton(this, wxID_ANY, _("do not change"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
  layout_kernel->Add(kernel_no_change, 0, wxALL, 5);

  krnl_net = new wxRadioButton(this, wxID_ANY, _("fetch from http://gnublin.org/"), wxDefaultPosition, wxDefaultSize, 0);
  layout_kernel->Add(krnl_net, 0, wxALL, 5);

  wxFlexGridSizer* sizer_kernel;
  sizer_kernel = new wxFlexGridSizer(1, 2, 0, 0);
  sizer_kernel->AddGrowableCol(1);
  sizer_kernel->SetFlexibleDirection(wxBOTH);
  sizer_kernel->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

  krnl_file = new wxRadioButton(this, wxID_ANY, _("use file"), wxDefaultPosition, wxDefaultSize, 0);
  sizer_kernel->Add(krnl_file, 0, wxALL, 5);

  file_zimage = new wxFilePickerCtrl(this, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxSize(-1, -1), wxFLP_DEFAULT_STYLE | wxFLP_FILE_MUST_EXIST | wxFLP_OPEN);
  sizer_kernel->Add(file_zimage, 1, wxALL | wxEXPAND, 5);

  layout_kernel->Add(sizer_kernel, 0, wxEXPAND, 5);

  main_sizer->Add(layout_kernel, 0, wxALL | wxEXPAND, 5);

  btn_apply = new wxStdDialogButtonSizer();
  btn_applyApply = new wxButton(this, wxID_APPLY);
  btn_apply->AddButton(btn_applyApply);
  btn_apply->Realize();
  main_sizer->Add(btn_apply, 1, wxEXPAND, 5);

  this->SetSizer(main_sizer);
  this->Layout();

  main_menu = new wxMenuBar(0);
  menu_installer = new wxMenu();
  wxMenuItem* mitem_settings;
  mitem_settings = new wxMenuItem(menu_installer, wxID_ANY, wxString(_("Settings")) , wxEmptyString, wxITEM_NORMAL);
  menu_installer->Append(mitem_settings);

  menu_installer->AppendSeparator();

  wxMenuItem* mitem_quit;
  mitem_quit = new wxMenuItem(menu_installer, wxID_ANY, wxString(_("Quit")) + wxT('\t') + wxT("Q"), wxEmptyString, wxITEM_NORMAL);
  menu_installer->Append(mitem_quit);

  main_menu->Append(menu_installer, _("Installer"));

  menu_backup = new wxMenu();
  mitem_create_backup = new wxMenuItem(menu_backup, wxID_ANY, wxString(_("Create Backup")) , wxEmptyString, wxITEM_NORMAL);
  menu_backup->Append(mitem_create_backup);

  mitem_restore_backup = new wxMenuItem(menu_backup, wxID_ANY, wxString(_("Restore Backup")) , wxEmptyString, wxITEM_NORMAL);
  menu_backup->Append(mitem_restore_backup);

  main_menu->Append(menu_backup, _("Backup"));

  menu_about = new wxMenu();
  wxMenuItem* mitem_about;
  mitem_about = new wxMenuItem(menu_about, wxID_ANY, wxString(_("About")) , wxEmptyString, wxITEM_NORMAL);
  menu_about->Append(mitem_about);

  main_menu->Append(menu_about, _("?"));

  this->SetMenuBar(main_menu);


  // create control list columns
  wxListItem col_dev;
  col_dev.SetId(0);
  col_dev.SetText(wxT("Device"));
  col_dev.SetWidth(170);
  listctrl_device->InsertColumn(0, col_dev);

  wxListItem col_size;
  col_size.SetId(1);
  col_size.SetText(wxT("Size"));
  col_size.SetWidth(100);
  listctrl_device->InsertColumn(1, col_size);

  wxListItem col_model;
  col_model.SetId(2);
  col_model.SetText(wxT("Model"));
  col_model.SetWidth(220);
  listctrl_device->InsertColumn(2, col_model);


  // read settings
  ReadSettings();

  // check if root
  if(!is_root() && checkroot) {
    wxMessageBox(_("You have to run this program as root!"), _("Error"), wxOK | wxICON_ERROR);
    Close();
    exit(0);
  }

  // get list of devices
  ReadDevices();

  // connect menu items
  this->Connect(mitem_quit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Window::OnQuit));
  this->Connect(mitem_about->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Window::ShowAbout));
  this->Connect(mitem_settings->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Window::ShowSettings));
  this->Connect(mitem_create_backup->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Window::CreateBackup));
  this->Connect(mitem_restore_backup->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Window::RestoreBackup));

  btn_applyApply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Window::DoInstall), NULL, this);
  listctrl_device->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(Window::DeviceDeselected), NULL, this);
  listctrl_device->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(Window::DeviceSelected), NULL, this);

  // disable gui
  EnableGUI(false);

}

void Window::ReadDevices() {
  // get list of drives
  char buffer[128];
  Drive** drives = get_devices();
  int id = 0;

  listctrl_device->DeleteAllItems();

  while(drives && *drives) {
    format_size(buffer, (*drives)->length *(*drives)->sector_size);

    // skip drives with size > 16 GB (change in settings)
    if((*drives)->length *(*drives)->sector_size / 1024 / 1024 > hidesize && hidedevice) {
      drives++;
      continue;
    }

    wxListItem item;
    item.SetId(id);
    item.SetText(wxString::FromAscii((*drives)->name));

    listctrl_device->InsertItem(item);
    listctrl_device->SetItem(id, 1, wxString::FromAscii(buffer));
    listctrl_device->SetItem(id, 2, wxString::FromAscii((*drives)->model));

    id++;
    drives++;
  }

  EnableGUI(false);
}


void Window::ReadSettings() {
  // default
  bootsector_size = 1024 * 1024 * 16; // 16 MB
  hidedevice = true;
  hidesize = 16 * 1024;  // 16 GB
  checkroot = true;

  wxXmlDocument doc;
  if(!doc.Load(wxT("settings.xml"))) {
    return;
  }

  // start processing the XML file
  if(doc.GetRoot()->GetName() != wxT("installer")) {
    return;
  }

  wxXmlNode* child = doc.GetRoot()->GetChildren();
  while(child) {

    // get settings
    if(child->GetName() == wxT("settings")) {
      wxXmlNode* s_child = child->GetChildren();

      while(s_child) {

        // get bootsector size
        if(s_child->GetName() == wxT("bootsector")) {
          wxString bs = s_child->GetNodeContent();
          printf("bootsector size: %s\n", C_STR(bs));

          long bs_size;
          if(!bs.ToLong(&bs_size)) {
            printf("Error reading bootsector size\n");
          } else {
            bootsector_size = bs_size * 1024 * 1024;
          }
        }

        // limit above which devices are not shown
        if(s_child->GetName() == wxT("hidesize")) {
          wxString hs = s_child->GetNodeContent();
          printf("hide if bigger than: %s\n", C_STR(hs));

          long hs_size;
          if(!hs.ToLong(&hs_size)) {
            printf("Error reading hidesize value\n");
          } else {
            hidesize = hs_size * 1024;
          }
        }

        // hide devices
        if(s_child->GetName() == wxT("hidedevice")) {
          wxString h = s_child->GetNodeContent();
          printf("hide devices: %s\n", C_STR(h));

          if(h == wxT("true")) {
            hidedevice = true;
          } else {
            hidedevice = false;
          }
        }

        // check if root
        if(s_child->GetName() == wxT("checkroot")) {
          wxString rt = s_child->GetNodeContent();
          printf("check for root: %s\n", C_STR(rt));

          if(rt == wxT("true")) {
            checkroot = true;
          } else {
            checkroot = false;
          }
        }

        s_child = s_child->GetNext();
      }
    }

    child = child->GetNext();
  }
}

void Window::WriteSettings() {
  wxXmlDocument doc;
  if(!doc.Load(wxT("settings.xml"))) {
    return;
  }

  // start processing the XML file
  if(doc.GetRoot()->GetName() != wxT("installer")) {
    return;
  }

  wxXmlNode* child = doc.GetRoot()->GetChildren();
  while(child) {

    // get settings
    if(child->GetName() == wxT("settings")) {
      wxXmlNode* s_child = child->GetChildren();

      while(s_child) {

        // bootsector size
        if(s_child->GetName() == wxT("bootsector")) {
          wxString bs = wxString::Format(wxT("%i"), bootsector_size / 1024 / 1024);
          s_child->GetChildren()->SetContent(bs);
        }

        // limit above which devices are not shown
        if(s_child->GetName() == wxT("hidesize")) {
          wxString hs = wxString::Format(wxT("%i"), hidesize / 1024);
          s_child->GetChildren()->SetContent(hs);
        }

        // hide devices
        if(s_child->GetName() == wxT("hidedevice")) {
          wxString h;
          if(hidedevice) {
            h = wxT("true");
          } else {
            h = wxT("false");
          }
          s_child->GetChildren()->SetContent(h);
        }

        // check if root
        if(s_child->GetName() == wxT("checkroot")) {
          wxString rt;
          if(checkroot) {
            rt = wxT("true");
          } else {
            rt = wxT("false");
          }
          s_child->GetChildren()->SetContent(rt);
        }

        s_child = s_child->GetNext();
      }
    }

    child = child->GetNext();
  }

  doc.Save(wxT("settings.xml"));
}

void Window::ReadURLs() {
  wxXmlDocument doc;
  if(!doc.Load(wxT("settings.xml"))) {
    return;
  }

  if(doc.GetRoot()->GetName() != wxT("installer")) {
    return;
  }

  wxXmlNode* child = doc.GetRoot()->GetChildren();
  while(child) {

    // get settings
    if(child->GetName() == wxT("url")) {
      wxXmlNode* s_child = child->GetChildren();

      while(s_child) {

        // get kernel url
        if(s_child->GetName() == wxT("kernel")) {
          wxString k = s_child->GetNodeContent();
          printf("kernel url: %s\n", C_STR(k));

          strcpy(url_kernel, C_STR(k));
        }

        // get bootloader url
        if(s_child->GetName() == wxT("bootloader")) {
          wxString b = s_child->GetNodeContent();
          printf("bootloader url: %s\n", C_STR(b));

          strcpy(url_bootloader, C_STR(b));
        }

        // get rootfs url
        if(s_child->GetName() == wxT("rootfs")) {
          wxString r = s_child->GetNodeContent();
          printf("rootfs url: %s\n", C_STR(r));

          strcpy(url_rootfs, C_STR(r));
        }

        // display
        if(s_child->GetName() == wxT("display")) {
          display_url = s_child->GetNodeContent();
          printf("url display: %s\n", C_STR(display_url));
        }

        s_child = s_child->GetNext();
      }
    }

    child = child->GetNext();
  }
}

void Window::EnableGUI(bool enabled) {
  check_repartition->Enable(enabled);
  boot_no_change->Enable(enabled);
  boot_net->Enable(enabled);
  boot_file->Enable(enabled);
  file_bootloader->Enable(enabled);
  krnl_net->Enable(enabled);
  krnl_file->Enable(enabled);
  file_zimage->Enable(enabled);
  root_no_change->Enable(enabled);
  kernel_no_change->Enable(enabled);
  root_net->Enable(enabled);
  root_file->Enable(enabled);
  file_rootfs->Enable(enabled);
  btn_applyApply->Enable(enabled);
  mitem_create_backup->Enable(enabled);
  mitem_restore_backup->Enable(enabled);
}

void Window::EnableTotalGUI(bool enabled) {
  EnableGUI(enabled);
  listctrl_device->Enable(enabled);
  main_menu->Enable(enabled);
}

void Window::DeviceSelected(wxListEvent& event) {
  EnableGUI(true);
}

void Window::DeviceDeselected(wxListEvent& event) {
  EnableGUI(false);
}

void Window::ShowSettings(wxCommandEvent& event) {
  SettingsFrame* s = new SettingsFrame(frame);
  s->Show();

  s->SetHideDev(hidedevice);
  s->SetHideDevSize(hidesize / 1024);
  s->SetCheckRoot(checkroot);
  s->SetBSSize(bootsector_size / 1024 / 1024);
}

void Window::StopBackup() {
  stop_backup = 1;
  printf("stopped!\n");
}

void Window::CopyDevice(FILE* in, FILE* out, long long kbytes, BackupProgress* b) {
  int blocksize = 1024 * 1024;
  char* buffer = (char*)malloc(blocksize);
  unsigned long total = 0;
  long long dev_size = kbytes;

  long start_time = time(NULL);

  while(!feof(in)) {
    fread(buffer, blocksize, 1, in);
    fwrite(buffer, blocksize, 1, out);
    total++;

    // calculate remaining time
    long time_delta = time(NULL) - start_time;
    int percent = (int)(100.f * total * (blocksize / 1024.f)) / dev_size;

    if(percent != 0) {
      //long total_time = (100 * time_delta / percent);
      long remaining_time = (time_delta / percent) * (100 - percent);

      wxString time_format;

      wxYield();
      if(stop_backup) {
        return;
      }
      b->SetRemainingTime(time_format.Format(_("%dm%02ds"), remaining_time / 60, remaining_time % 60));
    }

    wxYield();
    if(stop_backup) {
      return;
    }
    b->SetProgress(percent);
  }

  free(buffer);
}

void Window::CreateBackup(wxCommandEvent& event) {
  // check if a device is selected
  if(listctrl_device->GetSelectedItemCount() != 1) {
    return;
  }

  wxString device;

  wxFileDialog* saveFileDialog = new wxFileDialog(this, _("Save backup as..."), _(""), _(""),  _("Image|*.img"), wxSAVE, wxDefaultPosition);

  // create backup
  if(saveFileDialog->ShowModal() == wxID_OK) {

    stop_backup = 0;

    BackupProgress* b = new BackupProgress(frame);
    b->SetTextCreate();
    b->Show();
    EnableTotalGUI(false);

    Update();

    // start creating backup
    int itemIndex = listctrl_device->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    device = listctrl_device->GetItemText(itemIndex);
    long dev_size = get_device_size(C_STR(device)) / 1024; // device size in kbytes
    wxString image = saveFileDialog->GetFilename();

    if(dev_size == 0) {
      return;
    }

    FILE* in = fopen(C_STR(device), "rb");
    if(in == NULL) {
      wxMessageBox(_("Error opening device!"), _("Error"), wxOK | wxICON_ERROR);
      return;
    }

    FILE* out = fopen(C_STR(image), "wb");
    if(out == NULL) {
      wxMessageBox(_("Error opening file!"), _("Error"), wxOK | wxICON_ERROR);
      return;
    }

    CopyDevice(in, out, dev_size, b);

    b->Hide();
    b->Close();
    if(!stop_backup) {
      wxMessageBox(_("Creating backup was successful!"), _("Done"), wxOK);
    }

    fclose(in);
    fclose(out);
  }
}


void Window::RestoreBackup(wxCommandEvent& event) {
  // check if a device is selected
  if(listctrl_device->GetSelectedItemCount() != 1) {
    return;
  }

  // do you really want to do this, user?
  if(wxMessageBox(_("Do you really want to do this?\nThis destroys all the data on this device!"), _("Warning"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT) == wxNO) {
    return;
  }

  // get device
  wxString device;
  int itemIndex = listctrl_device->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

  device = listctrl_device->GetItemText(itemIndex);
  long dev_size = get_device_size(C_STR(device)) / 1024; // device size in kbytes

  // check if mounted
  int c;
  for(c = 0; c < 2; c++) {
    wxString part = device + wxString::FromAscii(c + 1 + '0');
    if(is_mounted(C_STR(part))) {
      unmount_partition(get_mountpoint(C_STR(part)));
      if(is_mounted(C_STR(part))) {
        wxMessageBox(_("Can not unmount '") + part + _("'!"), _("Error"), wxOK | wxICON_ERROR);
        return;
      }
    }
  }

  wxFileDialog* saveFileDialog = new wxFileDialog(this, _("Restore from..."), _(""), _(""),  _("Image|*.img"), wxOPEN | wxFILE_MUST_EXIST, wxDefaultPosition);

  // restore backup
  if(saveFileDialog->ShowModal() == wxID_OK) {
    wxString image = saveFileDialog->GetFilename();

    stop_backup = 0;

    BackupProgress* b = new BackupProgress(frame);
    b->SetTextRestore();
    b->Show();
    EnableTotalGUI(false);

    Update();

    // start creating backup


    if(dev_size == 0) {
      return;
    }

    FILE* out = fopen(C_STR(device), "wb");
    if(out == NULL) {
      wxMessageBox(_("Error opening device!"), _("Error"), wxOK | wxICON_ERROR);
      return;
    }

    FILE* in = fopen(C_STR(image), "rb");
    if(in == NULL) {
      wxMessageBox(_("Error opening file!"), _("Error"), wxOK | wxICON_ERROR);
      return;
    }

    CopyDevice(in, out, dev_size, b);

    b->Hide();
    b->Close();
    if(!stop_backup) {
      wxMessageBox(_("Restoring backup was successful!"), _("Done"), wxOK);
    }

    fclose(in);
    fclose(out);
  }
}

void Window::DoInstall(wxCommandEvent& event) {
  // check prerequisits
  if(listctrl_device->GetSelectedItemCount() != 1) {
    return;
  }

  // do you really want to do this, user?
  if(wxMessageBox(_("Do you really want to do this?\nThis destroys all the data on this device!"), _("Warning"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT) == wxNO) {
    return;
  }


  InstallerFrame* i = new InstallerFrame(frame);
  i->Show();

  EnableTotalGUI(false);

  Update();
  // start the installation
  //i->AddLog(_("---- [ I N F O ] -----------"));

  // -- gather all user informations --
  wxString device;
  bool repartition;
  bool write_bootl;
  bool dl_bootl;
  bool file_bootl;
  wxString bootloader_file;
  bool dl_kernel;
  wxString kernel_file;
  bool dl_rootfs;
  bool write_rootfs;
  bool file_rfs;
  wxString rootfs_file;

  int parts = 0;

  // get device
  int itemIndex = listctrl_device->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
  device = listctrl_device->GetItemText(itemIndex);
  // repartition?
  repartition = check_repartition->IsChecked();
  // write bootloader?
  write_bootl = !boot_no_change->GetValue();
  // download bootloader?
  dl_bootl = boot_net->GetValue();
  // bootloader file?
  file_bootl = boot_file->GetValue();
  if(file_bootl) {
    bootloader_file = file_bootloader->GetPath();
  }
  // download kernel?
  dl_kernel = krnl_net->GetValue();
  kernel_file = file_zimage->GetPath();
  // write rootfs?
  write_rootfs = !root_no_change->GetValue();
  // download rootfs?
  dl_rootfs = root_net->GetValue();
  // rootfs file?
  file_rfs = root_file->GetValue();
  if(file_rfs) {
    rootfs_file = file_rootfs->GetPath();
  }

  // check mounted, mount, copy kernel, unmount, sync
  parts = 5;
  if(dl_bootl) {
    parts++;
  }
  if(dl_kernel) {
    parts++;
  }
  if(dl_rootfs) {
    parts++;
  }
  if(repartition) {
    parts++;
  }
  if(write_bootl) {
    parts++;
  }
  if(write_rootfs) {
    parts++;
  }

  int prog_part = 100 / parts;
  int total_progress = 0;

  i->SetProgress(total_progress);

  //i->AddLog(_("----------------------------"));
  //i->AddLog(_(""));
  i->AddLog(_("Starting installation... This might take a few minutes"));

  // download bootloader
  if(dl_bootl) {
    mkdir("files", 0777);

    i->AddLog(_("Downloading bootloader (") + wxString::FromAscii(url_bootloader) + _(")"));
    get_file(url_bootloader, "files/apex.bin");
    total_progress += prog_part;
    i->SetProgress(total_progress);

    bootloader_file = _("files/apex.bin");
  }

  // download kernel
  if(dl_kernel) {
    mkdir("files", 0777);

    i->AddLog(_("Downloading kernel (") + wxString::FromAscii(url_kernel) + _(")"));
    get_file(url_kernel, "files/zImage");
    total_progress += prog_part;
    i->SetProgress(total_progress);


    kernel_file = _("files/zImage");
  }

  // download rootfs
  if(dl_rootfs) {
    mkdir("files", 0777);

    i->AddLog(_("Downloading RootFS (") + wxString::FromAscii(url_rootfs) + _(")"));
    get_file(url_rootfs, "files/rootfs.tar.gz");
    total_progress += prog_part;
    i->SetProgress(total_progress);

    rootfs_file = _("files/rootfs.tar.gz");
  }

  // check if mounted
  total_progress += prog_part;
  i->SetProgress(total_progress);

  int c;
  for(c = 0; c < 2; c++) {
    wxString part = device + wxString::FromAscii(c + 1 + '0');
    if(is_mounted(C_STR(part))) {
      i->AddLog(_("Unmounting ") + part);

      unmount_partition(get_mountpoint(C_STR(part)));

      if(is_mounted(C_STR(part))) {
        i->AddLog(_("ERROR: can not unmount ") + part);

        return;
      }
    }
  }
  // create partitions
  if(repartition) {
    i->AddLog(_("Creating partitions"));
    total_progress += prog_part;
    i->SetProgress(total_progress);

    create_partitions(C_STR(device), bootsector_size);
  }

  // copy bootloader
  if(write_bootl) {
    i->AddLog(_("Writing bootloader"));
    total_progress += prog_part;
    i->SetProgress(total_progress);

    wxString boot_part = device + _("2");
    dd(C_STR(bootloader_file), C_STR(boot_part), 512);
  }

  // mount
  i->AddLog(_("Mounting Linux partition"));
  total_progress += prog_part;
  i->SetProgress(total_progress);

  mkdir(mount_point, 0777);
  wxString linux_part;
  if(device.Contains(_("mmc"))){
  	linux_part = device + _("p1");
  }
  else 
  	linux_part = device + _("1");
  mount_partition(C_STR(linux_part), mount_point);

  if(!is_mounted(C_STR(linux_part))) {
    i->AddLog(_("ERROR: can not mount filesystem"));
    return;
  }
  // extract rootfs
  if(write_rootfs) {
    i->AddLog(_("Extracting RootFS"));
    total_progress += prog_part;
    i->SetProgress(total_progress);

    extract_archive(C_STR(rootfs_file), mount_point);
  }

std::cout << "copy rootfs done! starting copy kernel" << std::endl;

  // copy kernel
  i->AddLog(_("Copying kernel"));
  total_progress += prog_part;
  i->SetProgress(total_progress);
  wxString mnt_point = _(mount_point);
  wxString kernel_output_file = mnt_point + _("zImage");
  copy_file(C_STR(kernel_file), C_STR(kernel_output_file));

std::cout << "copy kernel done! unmount ..." << std::endl;

  // unmount
  i->AddLog(_("Unmounting Linux partition"));
  total_progress += prog_part;
  i->SetProgress(total_progress);

  unmount_partition(C_STR(linux_part));

std::cout << "unmount done! sync ..." << std::endl;

  // sync
  i->AddLog(_("Syncing"));
  total_progress += prog_part;
  i->SetProgress(total_progress);

  sync_card();

std::cout << "sync done!" << std::endl;

  i->AddLog(_("Done!"));
  i->AddLog(_("You may close this window now."));

  i->SetProgress(100);
}

void Window::SetHideDev(bool h) {
  hidedevice = h;
}

void Window::SetHideDevSize(unsigned long s) {
  hidesize = s * 1024;
}

void Window::SetCheckRoot(bool c) {
  checkroot = c;
}

void Window::SetBSSize(unsigned long s) {
  bootsector_size = s * 1024 * 1024;
}


void Window::ShowAbout(wxCommandEvent& event) {
  wxAboutDialogInfo info;
  info.SetName(_("GNUBLIN Installer"));
  info.SetVersion(_(VERSION));
  info.SetDescription(_("Installer for GNUBLIN Linux Board"));
  info.SetCopyright(wxT("(C) 2011 Michael Schwarz <michael.schwarz91@gmail.com>"));

  wxAboutBox(info);
}


void Window::OnQuit(wxCommandEvent& event) {
  Close();
}




