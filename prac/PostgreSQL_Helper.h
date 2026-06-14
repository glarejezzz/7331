#pragma once

#include "Database.h"
#include <libpq-fe.h>
#include <msclr/marshal_cppstd.h>

using namespace msclr::interop;

public ref class PostgreSQL_Helper {
private:
    PGconn* conn;
    marshal_context^ context;

    std::string ToStdString(String^ str) {
        return context->marshal_as<std::string>(str);
    }

    String^ ToSystemString(const char* str) {
        return gcnew String(str);
    }

public:
    PostgreSQL_Helper() {
        conn = nullptr;
        context = gcnew marshal_context();
    }

    ~PostgreSQL_Helper() {
        if (conn != nullptr) {
            PQfinish(conn);
        }
        delete context;
    }

    bool Connect(String^ host, String^ port, String^ dbname, String^ user, String^ password) {
        char conninfo[256];
        sprintf_s(conninfo, "host=%s port=%s dbname=%s user=%s password=%s",
            ToStdString(host).c_str(),
            ToStdString(port).c_str(),
            ToStdString(dbname).c_str(),
            ToStdString(user).c_str(),
            ToStdString(password).c_str());

        conn = PQconnectdb(conninfo);

        if (PQstatus(conn) != CONNECTION_OK) {
            return false;
        }

        PQexec(conn, "SET CLIENT_ENCODING TO 'UTF8'");
        return true;
    }

    List<Employee^>^ GetEmployees(int offset, int limit) {
        List<Employee^>^ result = gcnew List<Employee^>();

        char sql[1024];
        sprintf_s(sql,
            "SELECT e.employee_id, e.last_name, e.first_name, e.middle_name, "
            "e.position, e.salary, e.department_id, d.department_name, "
            "e.hire_date, e.email, e.phone, e.birth_date "
            "FROM employees e "
            "LEFT JOIN departments d ON e.department_id = d.department_id "
            "ORDER BY e.employee_id LIMIT %d OFFSET %d", limit, offset);

        PGresult* res = PQexec(conn, sql);

        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            Employee^ emp = gcnew Employee();
            emp->employee_id = atoi(PQgetvalue(res, i, 0));
            emp->last_name = ToSystemString(PQgetvalue(res, i, 1));
            emp->first_name = ToSystemString(PQgetvalue(res, i, 2));
            emp->middle_name = ToSystemString(PQgetvalue(res, i, 3));
            emp->position = ToSystemString(PQgetvalue(res, i, 4));
            emp->salary = atof(PQgetvalue(res, i, 5));
            emp->department_id = atoi(PQgetvalue(res, i, 6));
            emp->department_name = ToSystemString(PQgetvalue(res, i, 7));
            emp->hire_date = ToSystemString(PQgetvalue(res, i, 8));
            emp->email = ToSystemString(PQgetvalue(res, i, 9));
            emp->phone = ToSystemString(PQgetvalue(res, i, 10));
            emp->birth_date = ToSystemString(PQgetvalue(res, i, 11));
            result->Add(emp);
        }

        PQclear(res);
        return result;
    }

    int GetTotalCount() {
        PGresult* res = PQexec(conn, "SELECT COUNT(*) FROM employees");
        int count = atoi(PQgetvalue(res, 0, 0));
        PQclear(res);
        return count;
    }

    List<Employee^>^ SearchEmployees(String^ keyword, int offset, int limit) {
        List<Employee^>^ result = gcnew List<Employee^>();

        char sql[2048];
        sprintf_s(sql,
            "SELECT e.employee_id, e.last_name, e.first_name, e.middle_name, "
            "e.position, e.salary, e.department_id, d.department_name, "
            "e.hire_date, e.email, e.phone, e.birth_date "
            "FROM employees e "
            "LEFT JOIN departments d ON e.department_id = d.department_id "
            "WHERE e.last_name ILIKE '%%%s%%' OR e.first_name ILIKE '%%%s%%' "
            "OR e.position ILIKE '%%%s%%' OR d.department_name ILIKE '%%%s%%' "
            "OR e.email ILIKE '%%%s%%' "
            "ORDER BY e.employee_id LIMIT %d OFFSET %d",
            ToStdString(keyword).c_str(),
            ToStdString(keyword).c_str(),
            ToStdString(keyword).c_str(),
            ToStdString(keyword).c_str(),
            ToStdString(keyword).c_str(),
            limit, offset);

        PGresult* res = PQexec(conn, sql);

        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            Employee^ emp = gcnew Employee();
            emp->employee_id = atoi(PQgetvalue(res, i, 0));
            emp->last_name = ToSystemString(PQgetvalue(res, i, 1));
            emp->first_name = ToSystemString(PQgetvalue(res, i, 2));
            emp->middle_name = ToSystemString(PQgetvalue(res, i, 3));
            emp->position = ToSystemString(PQgetvalue(res, i, 4));
            emp->salary = atof(PQgetvalue(res, i, 5));
            emp->department_id = atoi(PQgetvalue(res, i, 6));
            emp->department_name = ToSystemString(PQgetvalue(res, i, 7));
            emp->hire_date = ToSystemString(PQgetvalue(res, i, 8));
            emp->email = ToSystemString(PQgetvalue(res, i, 9));
            emp->phone = ToSystemString(PQgetvalue(res, i, 10));
            emp->birth_date = ToSystemString(PQgetvalue(res, i, 11));
            result->Add(emp);
        }

        PQclear(res);
        return result;
    }

    Employee^ GetEmployeeById(int id) {
        char sql[512];
        sprintf_s(sql,
            "SELECT employee_id, last_name, first_name, middle_name, position, "
            "salary, department_id, hire_date, email, phone, birth_date "
            "FROM employees WHERE employee_id = %d", id);

        PGresult* res = PQexec(conn, sql);

        if (PQntuples(res) == 0) {
            PQclear(res);
            return nullptr;
        }

        Employee^ emp = gcnew Employee();
        emp->employee_id = atoi(PQgetvalue(res, 0, 0));
        emp->last_name = ToSystemString(PQgetvalue(res, 0, 1));
        emp->first_name = ToSystemString(PQgetvalue(res, 0, 2));
        emp->middle_name = ToSystemString(PQgetvalue(res, 0, 3));
        emp->position = ToSystemString(PQgetvalue(res, 0, 4));
        emp->salary = atof(PQgetvalue(res, 0, 5));
        emp->department_id = atoi(PQgetvalue(res, 0, 6));
        emp->hire_date = ToSystemString(PQgetvalue(res, 0, 7));
        emp->email = ToSystemString(PQgetvalue(res, 0, 8));
        emp->phone = ToSystemString(PQgetvalue(res, 0, 9));
        emp->birth_date = ToSystemString(PQgetvalue(res, 0, 10));

        PQclear(res);
        return emp;
    }

    bool AddEmployee(Employee^ emp) {
        char sql[2048];
        sprintf_s(sql,
            "INSERT INTO employees (last_name, first_name, middle_name, position, "
            "salary, department_id, hire_date, email, phone, birth_date) "
            "VALUES ('%s', '%s', '%s', '%s', %.2f, %d, '%s', '%s', '%s', '%s')",
            ToStdString(emp->last_name).c_str(),
            ToStdString(emp->first_name).c_str(),
            ToStdString(emp->middle_name).c_str(),
            ToStdString(emp->position).c_str(),
            emp->salary,
            emp->department_id,
            ToStdString(emp->hire_date).c_str(),
            ToStdString(emp->email).c_str(),
            ToStdString(emp->phone).c_str(),
            ToStdString(emp->birth_date).c_str());

        PGresult* res = PQexec(conn, sql);
        bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
        PQclear(res);
        return success;
    }

    bool UpdateEmployee(Employee^ emp) {
        char sql[2048];
        sprintf_s(sql,
            "UPDATE employees SET last_name = '%s', first_name = '%s', "
            "middle_name = '%s', position = '%s', salary = %.2f, "
            "department_id = %d, hire_date = '%s', email = '%s', "
            "phone = '%s', birth_date = '%s' WHERE employee_id = %d",
            ToStdString(emp->last_name).c_str(),
            ToStdString(emp->first_name).c_str(),
            ToStdString(emp->middle_name).c_str(),
            ToStdString(emp->position).c_str(),
            emp->salary,
            emp->department_id,
            ToStdString(emp->hire_date).c_str(),
            ToStdString(emp->email).c_str(),
            ToStdString(emp->phone).c_str(),
            ToStdString(emp->birth_date).c_str(),
            emp->employee_id);

        PGresult* res = PQexec(conn, sql);
        bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
        PQclear(res);
        return success;
    }

    bool DeleteEmployee(int id) {
        char sql[128];
        sprintf_s(sql, "DELETE FROM employees WHERE employee_id = %d", id);

        PGresult* res = PQexec(conn, sql);
        bool success = (PQresultStatus(res) == PGRES_COMMAND_OK);
        PQclear(res);
        return success;
    }

    List<Department^>^ GetDepartments() {
        List<Department^>^ result = gcnew List<Department^>();

        PGresult* res = PQexec(conn, "SELECT department_id, department_code, department_name, "
            "manager_name, phone, location, budget FROM departments ORDER BY department_id");

        int rows = PQntuples(res);
        for (int i = 0; i < rows; i++) {
            Department^ dept = gcnew Department();
            dept->department_id = atoi(PQgetvalue(res, i, 0));
            dept->department_code = ToSystemString(PQgetvalue(res, i, 1));
            dept->department_name = ToSystemString(PQgetvalue(res, i, 2));
            dept->manager_name = ToSystemString(PQgetvalue(res, i, 3));
            dept->phone = ToSystemString(PQgetvalue(res, i, 4));
            dept->location = ToSystemString(PQgetvalue(res, i, 5));
            dept->budget = atof(PQgetvalue(res, i, 6));
            result->Add(dept);
        }

        PQclear(res);
        return result;
    }
};