#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::Globalization;

#include "PostgreSQL_Helper.h"

public ref class AddEditForm : public Form {
private:
    Object^ currentDB;
    String^ currentDBType;
    Employee^ editingEmployee;

    TextBox^ txtLastName;
    TextBox^ txtFirstName;
    TextBox^ txtMiddleName;
    TextBox^ txtPosition;
    TextBox^ txtSalary;
    ComboBox^ cmbDepartment;
    DateTimePicker^ dtpHireDate;
    DateTimePicker^ dtpBirthDate;
    TextBox^ txtEmail;
    TextBox^ txtPhone;
    Button^ btnSave;
    Button^ btnCancel;

public:
    AddEditForm(Object^ db, String^ dbType, Employee^ employee) {
        currentDB = db;
        currentDBType = dbType;
        editingEmployee = employee;
        InitializeComponent();
        LoadDepartments();

        if (editingEmployee != nullptr) {
            this->Text = L"Edit Employee";
            txtLastName->Text = editingEmployee->last_name;
            txtFirstName->Text = editingEmployee->first_name;
            txtMiddleName->Text = editingEmployee->middle_name;
            txtPosition->Text = editingEmployee->position;
            txtSalary->Text = editingEmployee->salary.ToString();
            txtEmail->Text = editingEmployee->email;
            txtPhone->Text = editingEmployee->phone;
            dtpHireDate->Value = DateTime::Parse(editingEmployee->hire_date);

            if (editingEmployee->birth_date != L"") {
                dtpBirthDate->Value = DateTime::Parse(editingEmployee->birth_date);
                dtpBirthDate->Checked = true;
            }

            for (int i = 0; i < cmbDepartment->Items->Count; i++) {
                Department^ d = safe_cast<Department^>(cmbDepartment->Items[i]);
                if (d->department_id == editingEmployee->department_id) {
                    cmbDepartment->SelectedIndex = i;
                    break;
                }
            }
        }
        else {
            this->Text = L"Add Employee";
            dtpHireDate->Value = DateTime::Now;
            dtpBirthDate->Checked = false;
        }
    }

private:
    void InitializeComponent() {
        this->Size = Drawing::Size(450, 520);
        this->StartPosition = FormStartPosition::CenterParent;
        this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
        this->MaximizeBox = false;
        this->MinimizeBox = false;

        int y = 15;
        int labelX = 20;
        int fieldX = 130;
        int fieldWidth = 270;

        Label^ lblLastName = gcnew Label();
        lblLastName->Text = L"Last Name:*";
        lblLastName->Location = Point(labelX, y);
        lblLastName->Size = Drawing::Size(100, 25);

        txtLastName = gcnew TextBox();
        txtLastName->Location = Point(fieldX, y);
        txtLastName->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        Label^ lblFirstName = gcnew Label();
        lblFirstName->Text = L"First Name:*";
        lblFirstName->Location = Point(labelX, y);
        lblFirstName->Size = Drawing::Size(100, 25);

        txtFirstName = gcnew TextBox();
        txtFirstName->Location = Point(fieldX, y);
        txtFirstName->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        Label^ lblMiddleName = gcnew Label();
        lblMiddleName->Text = L"Middle Name:";
        lblMiddleName->Location = Point(labelX, y);
        lblMiddleName->Size = Drawing::Size(100, 25);

        txtMiddleName = gcnew TextBox();
        txtMiddleName->Location = Point(fieldX, y);
        txtMiddleName->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        // Position
        Label^ lblPosition = gcnew Label();
        lblPosition->Text = L"Position:*";
        lblPosition->Location = Point(labelX, y);
        lblPosition->Size = Drawing::Size(100, 25);

        txtPosition = gcnew TextBox();
        txtPosition->Location = Point(fieldX, y);
        txtPosition->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        // Salary
        Label^ lblSalary = gcnew Label();
        lblSalary->Text = L"Salary:*";
        lblSalary->Location = Point(labelX, y);
        lblSalary->Size = Drawing::Size(100, 25);

        txtSalary = gcnew TextBox();
        txtSalary->Location = Point(fieldX, y);
        txtSalary->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        Label^ lblDept = gcnew Label();
        lblDept->Text = L"Department:*";
        lblDept->Location = Point(labelX, y);
        lblDept->Size = Drawing::Size(100, 25);

        cmbDepartment = gcnew ComboBox();
        cmbDepartment->Location = Point(fieldX, y);
        cmbDepartment->Size = Drawing::Size(fieldWidth, 25);
        cmbDepartment->DisplayMember = L"department_name";
        cmbDepartment->ValueMember = L"department_id";
        cmbDepartment->DropDownStyle = ComboBoxStyle::DropDownList;

        y += 35;

        Label^ lblEmail = gcnew Label();
        lblEmail->Text = L"Email:";
        lblEmail->Location = Point(labelX, y);
        lblEmail->Size = Drawing::Size(100, 25);

        txtEmail = gcnew TextBox();
        txtEmail->Location = Point(fieldX, y);
        txtEmail->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        Label^ lblPhone = gcnew Label();
        lblPhone->Text = L"Phone:";
        lblPhone->Location = Point(labelX, y);
        lblPhone->Size = Drawing::Size(100, 25);

        txtPhone = gcnew TextBox();
        txtPhone->Location = Point(fieldX, y);
        txtPhone->Size = Drawing::Size(fieldWidth, 25);

        y += 35;

        Label^ lblHireDate = gcnew Label();
        lblHireDate->Text = L"Hire Date:*";
        lblHireDate->Location = Point(labelX, y);
        lblHireDate->Size = Drawing::Size(100, 25);

        dtpHireDate = gcnew DateTimePicker();
        dtpHireDate->Location = Point(fieldX, y);
        dtpHireDate->Size = Drawing::Size(fieldWidth, 25);
        dtpHireDate->Format = DateTimePickerFormat::Custom;
        dtpHireDate->CustomFormat = L"yyyy-MM-dd";

        y += 35;

        Label^ lblBirthDate = gcnew Label();
        lblBirthDate->Text = L"Birth Date:";
        lblBirthDate->Location = Point(labelX, y);
        lblBirthDate->Size = Drawing::Size(100, 25);

        dtpBirthDate = gcnew DateTimePicker();
        dtpBirthDate->Location = Point(fieldX, y);
        dtpBirthDate->Size = Drawing::Size(fieldWidth, 25);
        dtpBirthDate->Format = DateTimePickerFormat::Custom;
        dtpBirthDate->CustomFormat = L"yyyy-MM-dd";
        dtpBirthDate->ShowCheckBox = true;

        y += 45;

        btnSave = gcnew Button();
        btnSave->Text = L"Save";
        btnSave->Location = Point(fieldX, y);
        btnSave->Size = Drawing::Size(100, 30);
        btnSave->Click += gcnew EventHandler(this, &AddEditForm::OnSave);

        btnCancel = gcnew Button();
        btnCancel->Text = L"Cancel";
        btnCancel->Location = Point(fieldX + 120, y);
        btnCancel->Size = Drawing::Size(100, 30);
        btnCancel->Click += gcnew EventHandler(this, &AddEditForm::OnCancel);

        this->Controls->AddRange(gcnew array<Control^>{
            lblLastName, txtLastName,
                lblFirstName, txtFirstName,
                lblMiddleName, txtMiddleName,
                lblPosition, txtPosition,
                lblSalary, txtSalary,
                lblDept, cmbDepartment,
                lblEmail, txtEmail,
                lblPhone, txtPhone,
                lblHireDate, dtpHireDate,
                lblBirthDate, dtpBirthDate,
                btnSave, btnCancel
        });
    }

    void LoadDepartments() {
        try {
            PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
            List<Department^>^ depts = pg->GetDepartments();
            for each (Department ^ d in depts) {
                cmbDepartment->Items->Add(d);
            }

            if (cmbDepartment->Items->Count > 0) {
                cmbDepartment->SelectedIndex = 0;
            }
        }
        catch (Exception^) {
            Department^ d1 = gcnew Department();
            d1->department_id = 1;
            d1->department_name = L"IT";

            Department^ d2 = gcnew Department();
            d2->department_id = 2;
            d2->department_name = L"HR";

            Department^ d3 = gcnew Department();
            d3->department_id = 3;
            d3->department_name = L"Sales";

            cmbDepartment->Items->Add(d1);
            cmbDepartment->Items->Add(d2);
            cmbDepartment->Items->Add(d3);
            cmbDepartment->SelectedIndex = 0;
        }
    }

    void OnSave(Object^ sender, EventArgs^ e) {
        if (txtLastName->Text == L"") {
            MessageBox::Show(L"Please enter last name!", L"Error");
            txtLastName->Focus();
            return;
        }

        if (txtFirstName->Text == L"") {
            MessageBox::Show(L"Please enter first name!", L"Error");
            txtFirstName->Focus();
            return;
        }

        if (txtPosition->Text == L"") {
            MessageBox::Show(L"Please enter position!", L"Error");
            txtPosition->Focus();
            return;
        }

        if (txtSalary->Text == L"") {
            MessageBox::Show(L"Please enter salary!", L"Error");
            txtSalary->Focus();
            return;
        }

        double salary = 0;
        try {
            salary = Double::Parse(txtSalary->Text, CultureInfo::InvariantCulture);
        }
        catch (FormatException^) {
            MessageBox::Show(L"Salary must be a number (example: 50000 or 50000.00)!", L"Error");
            txtSalary->Focus();
            return;
        }

        if (cmbDepartment->SelectedItem == nullptr) {
            MessageBox::Show(L"Please select department!", L"Error");
            return;
        }

        Employee^ emp = gcnew Employee();
        emp->last_name = txtLastName->Text;
        emp->first_name = txtFirstName->Text;
        emp->middle_name = txtMiddleName->Text;
        emp->position = txtPosition->Text;
        emp->salary = salary;

        Department^ selectedDept = safe_cast<Department^>(cmbDepartment->SelectedItem);
        emp->department_id = selectedDept->department_id;

        emp->hire_date = dtpHireDate->Value.ToString(L"yyyy-MM-dd");
        emp->email = txtEmail->Text;
        emp->phone = txtPhone->Text;

        if (dtpBirthDate->Checked) {
            emp->birth_date = dtpBirthDate->Value.ToString(L"yyyy-MM-dd");
        }
        else {
            emp->birth_date = L"";
        }

        bool success = false;

        if (editingEmployee != nullptr) {
            emp->employee_id = editingEmployee->employee_id;
            PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
            success = pg->UpdateEmployee(emp);
        }
        else {
            PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
            success = pg->AddEmployee(emp);
        }

        if (success) {
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            this->Close();
        }
        else {
            MessageBox::Show(L"Error saving employee!", L"Error");
        }
    }

    void OnCancel(Object^ sender, EventArgs^ e) {
        this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
        this->Close();
    }
};