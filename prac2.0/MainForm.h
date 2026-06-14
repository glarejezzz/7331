#pragma once

#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Collections::Generic;
using namespace System::IO;

#include <chrono>
#include <string>

#include "PostgreSQL_Helper.h"
#include "MySQL_Helper.h"
#include "AddEditForm.h"

public ref class MainForm : public Form {
private:
    ComboBox^ cmbDBType;
    Button^ btnConnect;
    DataGridView^ dataGrid;
    Button^ btnAdd, ^ btnEdit, ^ btnDelete, ^ btnRefresh;
    TextBox^ txtSearch;
    Button^ btnSearch;
    Button^ btnPrev, ^ btnNext;
    Label^ lblPageInfo;
    Button^ btnExport, ^ btnTest;

    String^ currentRole;
    int currentPage;
    int pageSize;
    int totalRecords;

    Object^ currentDB;
    String^ currentDBType;

public:
    MainForm(String^ role) {
        currentRole = role;
        currentPage = 0;
        pageSize = 20;
        currentDB = nullptr;
        currentDBType = L"";
        InitializeComponent();
    }

private:
    void InitializeComponent() {
        this->Text = L"HR Management - " + currentRole;
        this->Size = Drawing::Size(1300, 700);
        this->StartPosition = FormStartPosition::CenterScreen;

        int y = 10;

        Label^ lblDB = gcnew Label();
        lblDB->Text = L"DB:";
        lblDB->Location = Point(10, y + 3);
        lblDB->Size = Drawing::Size(30, 25);

        cmbDBType = gcnew ComboBox();
        cmbDBType->Location = Point(45, y);
        cmbDBType->Size = Drawing::Size(120, 25);
        cmbDBType->Items->Add(L"PostgreSQL");
        cmbDBType->Items->Add(L"MySQL");
        cmbDBType->SelectedIndex = 0;

        btnConnect = gcnew Button();
        btnConnect->Text = L"Connect";
        btnConnect->Location = Point(175, y);
        btnConnect->Size = Drawing::Size(100, 25);
        btnConnect->Click += gcnew EventHandler(this, &MainForm::OnConnect);

        btnRefresh = gcnew Button();
        btnRefresh->Text = L"Refresh";
        btnRefresh->Location = Point(285, y);
        btnRefresh->Size = Drawing::Size(90, 25);
        btnRefresh->Click += gcnew EventHandler(this, &MainForm::OnRefresh);

        btnAdd = gcnew Button();
        btnAdd->Text = L"Add";
        btnAdd->Location = Point(385, y);
        btnAdd->Size = Drawing::Size(90, 25);
        btnAdd->Click += gcnew EventHandler(this, &MainForm::OnAdd);

        btnEdit = gcnew Button();
        btnEdit->Text = L"Edit";
        btnEdit->Location = Point(485, y);
        btnEdit->Size = Drawing::Size(100, 25);
        btnEdit->Click += gcnew EventHandler(this, &MainForm::OnEdit);

        btnDelete = gcnew Button();
        btnDelete->Text = L"Delete";
        btnDelete->Location = Point(595, y);
        btnDelete->Size = Drawing::Size(90, 25);
        btnDelete->Click += gcnew EventHandler(this, &MainForm::OnDelete);

        if (currentRole == L"Manager") {
            btnDelete->Visible = false;
        }

        txtSearch = gcnew TextBox();
        txtSearch->Location = Point(710, y);
        txtSearch->Size = Drawing::Size(200, 25);
        txtSearch->Text = L"Search...";

        btnSearch = gcnew Button();
        btnSearch->Text = L"Search";
        btnSearch->Location = Point(920, y);
        btnSearch->Size = Drawing::Size(80, 25);
        btnSearch->Click += gcnew EventHandler(this, &MainForm::OnSearch);

        btnExport = gcnew Button();
        btnExport->Text = L"Export CSV";
        btnExport->Location = Point(1010, y);
        btnExport->Size = Drawing::Size(100, 25);
        btnExport->Click += gcnew EventHandler(this, &MainForm::OnExport);

        btnTest = gcnew Button();
        btnTest->Text = L"Test";
        btnTest->Location = Point(1120, y);
        btnTest->Size = Drawing::Size(80, 25);
        btnTest->Click += gcnew EventHandler(this, &MainForm::OnTest);

        dataGrid = gcnew DataGridView();
        dataGrid->Location = Point(10, 45);
        dataGrid->Size = Drawing::Size(1260, 550);
        dataGrid->AllowUserToAddRows = false;
        dataGrid->ReadOnly = true;
        dataGrid->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
        dataGrid->MultiSelect = false;

        btnPrev = gcnew Button();
        btnPrev->Text = L"◀ Previous";
        btnPrev->Location = Point(10, 610);
        btnPrev->Size = Drawing::Size(120, 30);
        btnPrev->Click += gcnew EventHandler(this, &MainForm::OnPrevPage);

        lblPageInfo = gcnew Label();
        lblPageInfo->Text = L"Page 1";
        lblPageInfo->Location = Point(140, 617);
        lblPageInfo->Size = Drawing::Size(300, 25);

        btnNext = gcnew Button();
        btnNext->Text = L"Next ▶";
        btnNext->Location = Point(450, 610);
        btnNext->Size = Drawing::Size(120, 30);
        btnNext->Click += gcnew EventHandler(this, &MainForm::OnNextPage);

        this->Controls->AddRange(gcnew array<Control^>{
            lblDB, cmbDBType, btnConnect, btnRefresh, btnAdd, btnEdit, btnDelete,
                txtSearch, btnSearch, btnExport, btnTest, dataGrid, btnPrev, lblPageInfo, btnNext
        });
    }

    void OnConnect(Object^ sender, EventArgs^ e) {
        currentDBType = cmbDBType->SelectedItem->ToString();

        if (currentDBType == L"PostgreSQL") {
            PostgreSQL_Helper^ pg = gcnew PostgreSQL_Helper();
            if (pg->Connect("localhost", "5432", "hr_db", "postgres", "123")) {
                currentDB = pg;
                MessageBox::Show(L"Connected to PostgreSQL!", L"Success");
                LoadData();
            }
            else {
                MessageBox::Show(L"Failed to connect to PostgreSQL!\nCheck password and that database 'hr_db' exists.", L"Error");
            }
        }
        else if (currentDBType == L"MySQL") {
            MySQL_Helper^ mysql = gcnew MySQL_Helper();
            if (mysql->Connect("127.0.0.1", "3306", "hr_db", "hr_user", "1234")) {
                currentDB = mysql;
                MessageBox::Show(L"Connected to MySQL!", L"Success");
                LoadData();
            }
            else {
                MessageBox::Show(L"Failed to connect to MySQL!", L"Error");
            }
        }
    }

    void LoadData() {
        if (currentDB == nullptr) {
            MessageBox::Show(L"Please connect to database first!", L"Warning");
            return;
        }

        try {
            if (currentDBType == L"PostgreSQL") {
                PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
                totalRecords = pg->GetTotalCount();
                List<Employee^>^ employees = pg->GetEmployees(currentPage * pageSize, pageSize);
                DisplayEmployees(employees);
            }
            else if (currentDBType == L"MySQL") {
                MySQL_Helper^ mysql = safe_cast<MySQL_Helper^>(currentDB);
                totalRecords = mysql->GetTotalCount();
                List<Employee^>^ employees = mysql->GetEmployees(currentPage * pageSize, pageSize);
                DisplayEmployees(employees);
            }

            int totalPages = (totalRecords + pageSize - 1) / pageSize;
            lblPageInfo->Text = String::Format(L"Page {0} of {1} (total {2} records)",
                currentPage + 1, totalPages, totalRecords);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Error loading data: " + ex->Message);
        }
    }

    void DisplayEmployees(List<Employee^>^ employees) {
        dataGrid->Rows->Clear();
        dataGrid->Columns->Clear();

        dataGrid->Columns->Add(L"employee_id", L"ID");
        dataGrid->Columns->Add(L"last_name", L"Last Name");
        dataGrid->Columns->Add(L"first_name", L"First Name");
        dataGrid->Columns->Add(L"middle_name", L"Middle Name");
        dataGrid->Columns->Add(L"position", L"Position");
        dataGrid->Columns->Add(L"salary", L"Salary");
        dataGrid->Columns->Add(L"department_name", L"Department");
        dataGrid->Columns->Add(L"hire_date", L"Hire Date");
        dataGrid->Columns->Add(L"phone", L"Phone");
        dataGrid->Columns->Add(L"email", L"Email");

        dataGrid->Columns[L"employee_id"]->Width = 50;
        dataGrid->Columns[L"last_name"]->Width = 120;
        dataGrid->Columns[L"first_name"]->Width = 100;
        dataGrid->Columns[L"middle_name"]->Width = 100;
        dataGrid->Columns[L"position"]->Width = 150;
        dataGrid->Columns[L"salary"]->Width = 80;
        dataGrid->Columns[L"department_name"]->Width = 150;
        dataGrid->Columns[L"hire_date"]->Width = 90;
        dataGrid->Columns[L"phone"]->Width = 110;
        dataGrid->Columns[L"email"]->Width = 150;

        for each (Employee ^ emp in employees) {
            dataGrid->Rows->Add(
                emp->employee_id,
                emp->last_name,
                emp->first_name,
                emp->middle_name,
                emp->position,
                emp->salary,
                emp->department_name,
                emp->hire_date,
                emp->phone,
                emp->email
            );
        }
    }

    void OnRefresh(Object^ sender, EventArgs^ e) {
        currentPage = 0;
        LoadData();
    }

    void OnSearch(Object^ sender, EventArgs^ e) {
        String^ keyword = txtSearch->Text;
        if (keyword == L"" || keyword == L"Search...") {
            LoadData();
            return;
        }

        if (currentDB == nullptr) return;

        try {
            List<Employee^>^ results;

            if (currentDBType == L"PostgreSQL") {
                PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
                results = pg->SearchEmployees(keyword, 0, 100);
            }
            else {
                MySQL_Helper^ mysql = safe_cast<MySQL_Helper^>(currentDB);
                results = mysql->SearchEmployees(keyword, 0, 100);
            }

            DisplayEmployees(results);
            lblPageInfo->Text = String::Format(L"Found: {0} records", results->Count);
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Search error: " + ex->Message);
        }
    }
    void OnAdd(Object^ sender, EventArgs^ e) {
        AddEditForm^ form = gcnew AddEditForm(currentDB, currentDBType, nullptr);
        if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            LoadData();
        }
    }

    void OnEdit(Object^ sender, EventArgs^ e) {
        if (dataGrid->SelectedRows->Count == 0) {
            MessageBox::Show(L"Please select a record to edit!");
            return;
        }

        int id = Convert::ToInt32(dataGrid->SelectedRows[0]->Cells[L"employee_id"]->Value);

        Employee^ emp = nullptr;
        if (currentDBType == L"PostgreSQL") {
            PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
            emp = pg->GetEmployeeById(id);
        }
        else {
            MySQL_Helper^ mysql = safe_cast<MySQL_Helper^>(currentDB);
            emp = mysql->GetEmployeeById(id);
        }

        if (emp != nullptr) {
            AddEditForm^ form = gcnew AddEditForm(currentDB, currentDBType, emp);
            if (form->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
                LoadData();
            }
        }
    }

    void OnDelete(Object^ sender, EventArgs^ e) {
        if (dataGrid->SelectedRows->Count == 0) return;

        System::Windows::Forms::DialogResult result = MessageBox::Show(L"Delete selected employee?",
            L"Confirm", MessageBoxButtons::YesNo);

        if (result == System::Windows::Forms::DialogResult::Yes) {
            int id = Convert::ToInt32(dataGrid->SelectedRows[0]->Cells[L"employee_id"]->Value);
            bool success = false;

            if (currentDBType == L"PostgreSQL") {
                PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
                success = pg->DeleteEmployee(id);
            }
            else {
                MySQL_Helper^ mysql = safe_cast<MySQL_Helper^>(currentDB);
                success = mysql->DeleteEmployee(id);
            }

            if (success) {
                LoadData();
            }
            else {
                MessageBox::Show(L"Delete failed!");
            }
        }
    }

    void OnExport(Object^ sender, EventArgs^ e) {
        SaveFileDialog^ saveDialog = gcnew SaveFileDialog();
        saveDialog->Filter = L"CSV files (*.csv)|*.csv";
        saveDialog->Title = L"Save report";
        saveDialog->FileName = L"employees_export.csv";

        if (saveDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                StreamWriter^ sw = gcnew StreamWriter(saveDialog->FileName, false, System::Text::Encoding::UTF8);
                sw->WriteLine(L"ID;Last Name;First Name;Middle Name;Position;Salary;Department;Hire Date;Phone;Email");

                for each (DataGridViewRow ^ row in dataGrid->Rows) {
                    if (!row->IsNewRow && row->Cells[0]->Value != nullptr) {
                        sw->WriteLine(String::Format(L"{0};{1};{2};{3};{4};{5};{6};{7};{8};{9}",
                            row->Cells[0]->Value,
                            row->Cells[1]->Value,
                            row->Cells[2]->Value,
                            row->Cells[3]->Value,
                            row->Cells[4]->Value,
                            row->Cells[5]->Value,
                            row->Cells[6]->Value,
                            row->Cells[7]->Value,
                            row->Cells[8]->Value,
                            row->Cells[9]->Value));
                    }
                }

                sw->Close();
                MessageBox::Show(L"Export completed!", L"Success");
            }
            catch (Exception^ ex) {
                MessageBox::Show(L"Export error: " + ex->Message);
            }
        }
    }

    void OnTest(Object^ sender, EventArgs^ e) {
        if (currentDB == nullptr) {
            MessageBox::Show(L"Please connect to database first!", L"Error");
            return;
        }

        System::Windows::Forms::DialogResult confirm = MessageBox::Show(L"Insert 1000 test records? This may take a few seconds.",
            L"Performance Test", MessageBoxButtons::YesNo);

        if (confirm != System::Windows::Forms::DialogResult::Yes) {
            return;
        }

        btnTest->Enabled = false;
        btnConnect->Enabled = false;
        btnAdd->Enabled = false;
        btnEdit->Enabled = false;
        btnDelete->Enabled = false;

        try {
            auto start = std::chrono::high_resolution_clock::now();

            if (currentDBType == L"PostgreSQL") {
                PostgreSQL_Helper^ pg = safe_cast<PostgreSQL_Helper^>(currentDB);
                for (int i = 0; i < 1000; i++) {
                    Employee^ emp = gcnew Employee();
                    emp->last_name = String::Format(L"TestUser{0}", i);
                    emp->first_name = L"Test";
                    emp->middle_name = L"Testovich";
                    emp->position = L"Tester";
                    emp->salary = 30000.0 + (double)(i % 50000);
                    emp->department_id = (i % 3) + 1;
                    emp->hire_date = L"2024-01-01";
                    emp->email = String::Format(L"test{0}@test.com", i);
                    emp->phone = L"+7(999)000-0000";
                    emp->birth_date = L"1990-01-01";
                    pg->AddEmployee(emp);
                }
            }
            else {
                MySQL_Helper^ mysql = safe_cast<MySQL_Helper^>(currentDB);
                for (int i = 0; i < 1000; i++) {
                    Employee^ emp = gcnew Employee();
                    emp->last_name = String::Format(L"TestUser{0}", i);
                    emp->first_name = L"Test";
                    emp->middle_name = L"Testovich";
                    emp->position = L"Tester";
                    emp->salary = 30000.0 + (double)(i % 50000);
                    emp->department_id = (i % 3) + 1;
                    emp->hire_date = L"2024-01-01";
                    emp->email = String::Format(L"test{0}@test.com", i);
                    emp->phone = L"+7(999)000-0000";
                    emp->birth_date = L"1990-01-01";
                    mysql->AddEmployee(emp);
                }
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            double recordsPerSec = (1000.0 / duration) * 1000.0;

            MessageBox::Show(String::Format(L"Test completed!\n\nInserted: 1000 records\nTime: {0} ms\nSpeed: {1:F2} records/sec",
                duration, recordsPerSec), L"Test Results");

            LoadData();
        }
        catch (Exception^ ex) {
            MessageBox::Show(L"Test error: " + ex->Message, L"Error");
        }

        btnTest->Enabled = true;
        btnConnect->Enabled = true;
        btnAdd->Enabled = true;
        btnEdit->Enabled = true;
        btnDelete->Enabled = true;
    }
    void OnPrevPage(Object^ sender, EventArgs^ e) {
        if (currentPage > 0) {
            currentPage--;
            LoadData();
        }
    }

    void OnNextPage(Object^ sender, EventArgs^ e) {
        int totalPages = (totalRecords + pageSize - 1) / pageSize;
        if (currentPage + 1 < totalPages) {
            currentPage++;
            LoadData();
        }
    }
};