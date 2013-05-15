/*  progress.cpp
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * Installer Window to show current progress
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

#include "installer.h"
#include "progress.h"

extern Window* frame;

void InstallerFrame::AddLog(wxString item) {
  Update();
  wxArrayString it;
  it.Add(item);
  inst_log->InsertItems(it, log_entries++);
  inst_log->SetSelection(log_entries - 1);
  wxYield();
  Update();
}

void InstallerFrame::SetProgress(int progress) {
  inst_progr->SetValue(progress);
  Update();
}

void InstallerFrame::OnQuit(wxCloseEvent& event) {
  frame->EnableTotalGUI(true);
  event.Skip();
}


InstallerFrame::InstallerFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* installer_sizer;
  installer_sizer = new wxBoxSizer(wxVERTICAL);

  inst_text = new wxStaticText(this, wxID_ANY, _("Installing GNUBLIN..."), wxDefaultPosition, wxDefaultSize, 0);
  inst_text->Wrap(-1);
  installer_sizer->Add(inst_text, 0, wxALL, 5);

  inst_progr = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
  inst_progr->SetValue(0);
  installer_sizer->Add(inst_progr, 0, wxALL | wxEXPAND, 5);

  inst_log = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0);
  installer_sizer->Add(inst_log, 1, wxALL | wxEXPAND, 5);

  /*
  inst_ok = new wxStdDialogButtonSizer();
  inst_okOK = new wxButton( this, wxID_OK );
  inst_ok->AddButton( inst_okOK );
  inst_ok->Realize();
  installer_sizer->Add( inst_ok, 0, wxEXPAND|wxALL, 5 );
  */

  this->SetSizer(installer_sizer);
  this->Layout();

  this->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(InstallerFrame::OnQuit));

  log_entries = 0;
}

InstallerFrame::~InstallerFrame() {
}


