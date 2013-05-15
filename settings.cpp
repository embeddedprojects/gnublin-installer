/*  settings.cpp
 *
 *  Author:    Michael Schwarz
 *  Copyright (C) 2011 Michael Schwarz
 *
 * Settings Dialog
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

#include "settings.h"
#include "installer.h"

extern Window* frame;

SettingsFrame::SettingsFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) {
  this->SetSizeHints(wxDefaultSize, wxDefaultSize);

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer(wxVERTICAL);

  wxStaticBoxSizer* sbSizer5;
  sbSizer5 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Drive")), wxVERTICAL);

  wxFlexGridSizer* fgSizer4;
  fgSizer4 = new wxFlexGridSizer(1, 3, 0, 0);
  fgSizer4->SetFlexibleDirection(wxBOTH);
  fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  set_hidedev = new wxCheckBox(this, wxID_ANY, _("Only show devices that are smaller than"), wxDefaultPosition, wxDefaultSize, 0);
  set_hidedev->SetValue(true);
  fgSizer4->Add(set_hidedev, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  set_hidesize = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024, 16);
  fgSizer4->Add(set_hidesize, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  set_unit = new wxStaticText(this, wxID_ANY, _("GB"), wxDefaultPosition, wxDefaultSize, 0);
  set_unit->Wrap(-1);
  fgSizer4->Add(set_unit, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  sbSizer5->Add(fgSizer4, 1, wxEXPAND, 5);

  wxFlexGridSizer* fgSizer5;
  fgSizer5 = new wxFlexGridSizer(1, 2, 0, 0);
  fgSizer5->SetFlexibleDirection(wxBOTH);
  fgSizer5->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

  set_bs_label = new wxStaticText(this, wxID_ANY, _("Bootsector size (in MB)"), wxDefaultPosition, wxDefaultSize, 0);
  set_bs_label->Wrap(-1);
  fgSizer5->Add(set_bs_label, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

  set_bs_size = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 4, 128, 8);
  fgSizer5->Add(set_bs_size, 0, wxALL, 5);

  sbSizer5->Add(fgSizer5, 1, wxEXPAND, 5);

  bSizer3->Add(sbSizer5, 0, wxEXPAND | wxALL, 5);

  wxStaticBoxSizer* sbSizer6;
  sbSizer6 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("Permission")), wxVERTICAL);

  set_checkroot = new wxCheckBox(this, wxID_ANY, _("Check if user is root"), wxDefaultPosition, wxDefaultSize, 0);
  set_checkroot->SetValue(true);
  sbSizer6->Add(set_checkroot, 0, wxALL, 5);

  bSizer3->Add(sbSizer6, 0, wxEXPAND | wxALL, 5);

  set_btn = new wxStdDialogButtonSizer();
  set_btnSave = new wxButton(this, wxID_SAVE);
  set_btn->AddButton(set_btnSave);
  set_btnCancel = new wxButton(this, wxID_CANCEL);
  set_btn->AddButton(set_btnCancel);
  set_btn->Realize();
  bSizer3->Add(set_btn, 1, wxEXPAND | wxALL, 5);

  this->SetSizer(bSizer3);
  this->Layout();

  set_btnCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsFrame::OnClose), NULL, this);
  set_btnSave->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SettingsFrame::SaveSettings), NULL, this);
}

SettingsFrame::~SettingsFrame() {
}

void SettingsFrame::OnClose(wxCommandEvent& event) {
  this->Close();
}

void SettingsFrame::SaveSettings(wxCommandEvent& event) {
  this->Close();
  // return settings to main window
  frame->SetHideDev(set_hidedev->GetValue());
  frame->SetHideDevSize(set_hidesize->GetValue());
  frame->SetCheckRoot(set_checkroot->GetValue());
  frame->SetBSSize(set_bs_size->GetValue());
  frame->WriteSettings();
  frame->ReadDevices();
}

void SettingsFrame::SetHideDev(bool h) {
  set_hidedev->SetValue(h);
}

void SettingsFrame::SetHideDevSize(unsigned long s) {
  set_hidesize->SetValue(s);
}

void SettingsFrame::SetCheckRoot(bool c) {
  set_checkroot->SetValue(c);
}

void SettingsFrame::SetBSSize(unsigned long s) {
  set_bs_size->SetValue(s);
}


