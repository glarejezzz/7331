#pragma once

using namespace System;
using namespace System::Collections::Generic;

public ref struct Department {
    int department_id;
    String^ department_code;
    String^ department_name;
    String^ manager_name;
    String^ phone;
    String^ location;
    double budget;
};

public ref struct Employee {
    int employee_id;
    String^ last_name;
    String^ first_name;
    String^ middle_name;
    String^ position;
    double salary;
    int department_id;
    String^ department_name;
    String^ hire_date;
    String^ email;
    String^ phone;
    String^ birth_date;

    property String^ FullName {
        String^ get() {
            return String::Format(L"{0} {1} {2}", last_name, first_name, middle_name);
        }
    }
};