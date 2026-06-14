#pragma once

#using <System.dll>
#using <System.Windows.Forms.dll>
#using <System.Drawing.dll>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Drawing;

#include "MainForm.h"

public ref class LoginForm : public Form {
private:
    ComboBox^ cmbRole;
    Button^ btnLogin;

public:
    LoginForm() {
        this->Text = L"HR System Login";
        this->Size = Drawing::Size(300, 130);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
        this->MaximizeBox = false;

        Label^ lblRole = gcnew Label();
        lblRole->Text = L"Select role:";
        lblRole->Location = Point(30, 30);
        lblRole->Size = Drawing::Size(100, 25);

        cmbRole = gcnew ComboBox();
        cmbRole->Location = Point(140, 28);
        cmbRole->Size = Drawing::Size(120, 25);
        cmbRole->Items->Add(L"HR");
        cmbRole->Items->Add(L"Manager");
        cmbRole->SelectedIndex = 0;

        btnLogin = gcnew Button();
        btnLogin->Text = L"Login";
        btnLogin->Location = Point(100, 65);
        btnLogin->Size = Drawing::Size(100, 30);
        btnLogin->Click += gcnew EventHandler(this, &LoginForm::OnLogin);

        this->Controls->AddRange(gcnew array<Control^>{ lblRole, cmbRole, btnLogin });
    }

private:
    void OnLogin(Object^ sender, EventArgs^ e) {
        String^ role = cmbRole->SelectedItem->ToString();

        this->Hide();
        MainForm^ mainForm = gcnew MainForm(role);
        mainForm->ShowDialog();
        this->Close();
    }
};