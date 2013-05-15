/*  backup.cpp
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * Backup Progress Bar
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

#include "backup.h"
#include "installer.h"

extern Window* frame;

BackupProgress::BackupProgress(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer4;
  bSizer4 = new wxBoxSizer(wxVERTICAL);

  text_backup_info = new wxStaticText(this, wxID_ANY, _("Creating backup, this might take some time..."), wxDefaultPosition, wxDefaultSize, 0);
  text_backup_info->Wrap(-1);
  text_backup_info->SetFont(wxFont(9, 74, 90, 92, false, wxT("Sans")));

  bSizer4->Add(text_backup_info, 0, wxALL, 5);

  wxFlexGridSizer* fgSizer6;
  fgSizer6 = new wxFlexGridSizer(1, 2, 0, 0);
  fgSizer6->AddGrowableCol(1);
  fgSizer6->SetFlexibleDirection(wxBOTH);
  fgSizer6->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  m_staticText8 = new wxStaticText(this, wxID_ANY, _("Remaining time:"), wxDefaultPosition, wxDefaultSize, 0);
  m_staticText8->Wrap(-1);
  fgSizer6->Add(m_staticText8, 0, wxALL, 5);

  txt_remain = new wxStaticText(this, wxID_ANY, _("unknown"), wxDefaultPosition, wxDefaultSize, 0);
  txt_remain->Wrap(-1);
  fgSizer6->Add(txt_remain, 0, wxALL, 5);

  bSizer4->Add(fgSizer6, 0, wxEXPAND | wxALL, 5);

  progress_backup = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
  bSizer4->Add(progress_backup, 0, wxALL | wxEXPAND, 5);

  m_sdbSizer4 = new wxStdDialogButtonSizer();
  m_sdbSizer4Cancel = new wxButton(this, wxID_CANCEL);
  m_sdbSizer4->AddButton(m_sdbSizer4Cancel);
  m_sdbSizer4->Realize();
  bSizer4->Add(m_sdbSizer4, 1, wxEXPAND | wxALL, 5);

  this->SetSizer(bSizer4);
  this->Layout();

  this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(BackupProgress::OnQuit));
  m_sdbSizer4Cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(BackupProgress::OnClose), NULL, this);

}

void BackupProgress::OnQuit(wxCloseEvent& event) {
  frame->StopBackup();
  frame->EnableTotalGUI(true);
  //event.Skip();
}

void BackupProgress::OnClose(wxCommandEvent& event) {
  frame->StopBackup();
  frame->EnableTotalGUI(true);
  event.Skip();
}

void BackupProgress::SetProgress(int progress) {
  progress_backup->SetValue(progress);
  wxYield();
  Update();
}

void BackupProgress::SetTextCreate() {
  text_backup_info->SetLabel(_("Creating backup, this might take some time..."));
  SetRemainingTime(_("N/A"));
}

void BackupProgress::SetTextRestore() {
  text_backup_info->SetLabel(_("Restoring backup, this might take some time..."));
  SetRemainingTime(_("N/A"));
}

void BackupProgress::SetRemainingTime(wxString time) {
  txt_remain->SetLabel(time);
  Update();
}

BackupProgress::~BackupProgress() {
}

