#pragma once

#include "Database.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <msclr/marshal_cppstd.h>

using namespace sql;

public ref class MySQL_Helper {
private:
    sql::mysql::MySQL_Driver* driver;
    Connection* conn;
    msclr::interop::marshal_context^ context;

    std::string ToStdString(String^ str) {
        return msclr::interop::marshal_as<std::string>(str);
    }

    String^ ToSystemString(const std::string& str) {
        return gcnew String(str.c_str());
    }

public:
    MySQL_Helper() : driver(nullptr), conn(nullptr) {
        context = gcnew msclr::interop::marshal_context();
    }

    ~MySQL_Helper() {
        if (conn) {
            delete conn;
        }
        delete context;
    }

    bool Connect(String^ host, String^ port, String^ dbname, String^ user, String^ password) {
        try {
            driver = sql::mysql::get_mysql_driver_instance();

            // Явно задаем все параметры
            std::string server = "127.0.0.1";
            std::string username = "hr_user";
            std::string pass = "1234";
            std::string database = "hr_db";

            // Подключение через TCP/IP с явными параметрами
            conn = driver->connect(server, username, pass);
            conn->setSchema(database);

            return true;
        }
        catch (sql::SQLException& e) {
            String^ errorMsg = gcnew String(e.what());
            MessageBox::Show(L"MySQL Error: " + errorMsg, L"Connection Error");
            return false;
        }
        catch (std::exception& e) {
            String^ errorMsg = gcnew String(e.what());
            MessageBox::Show(L"Error: " + errorMsg, L"Connection Error");
            return false;
        }
    }

    List<Employee^>^ GetEmployees(int offset, int limit) {
        List<Employee^>^ result = gcnew List<Employee^>();

        try {
            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT e.employee_id, e.last_name, e.first_name, e.middle_name, "
                "e.position, e.salary, e.department_id, d.department_name, "
                "e.hire_date, e.email, e.phone, e.birth_date "
                "FROM employees e "
                "LEFT JOIN departments d ON e.department_id = d.department_id "
                "ORDER BY e.employee_id LIMIT ? OFFSET ?");
            pstmt->setInt(1, limit);
            pstmt->setInt(2, offset);
            ResultSet* res = pstmt->executeQuery();

            while (res->next()) {
                Employee^ emp = gcnew Employee();
                emp->employee_id = res->getInt("employee_id");
                emp->last_name = ToSystemString(res->getString("last_name"));
                emp->first_name = ToSystemString(res->getString("first_name"));
                emp->middle_name = ToSystemString(res->getString("middle_name"));
                emp->position = ToSystemString(res->getString("position"));
                emp->salary = res->getDouble("salary");
                emp->department_id = res->getInt("department_id");
                emp->department_name = ToSystemString(res->getString("department_name"));
                emp->hire_date = ToSystemString(res->getString("hire_date"));
                emp->email = ToSystemString(res->getString("email"));
                emp->phone = ToSystemString(res->getString("phone"));
                emp->birth_date = ToSystemString(res->getString("birth_date"));
                result->Add(emp);
            }

            delete res;
            delete pstmt;
        }
        catch (SQLException&) {}

        return result;
    }

    int GetTotalCount() {
        try {
            Statement* stmt = conn->createStatement();
            ResultSet* res = stmt->executeQuery("SELECT COUNT(*) FROM employees");
            int count = 0;
            if (res->next()) {
                count = res->getInt(1);
            }
            delete res;
            delete stmt;
            return count;
        }
        catch (SQLException&) {
            return 0;
        }
    }

    List<Employee^>^ SearchEmployees(String^ keyword, int offset, int limit) {
        List<Employee^>^ result = gcnew List<Employee^>();

        try {
            std::string pattern = "%" + ToStdString(keyword) + "%";

            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT e.employee_id, e.last_name, e.first_name, e.middle_name, "
                "e.position, e.salary, e.department_id, d.department_name, "
                "e.hire_date, e.email, e.phone, e.birth_date "
                "FROM employees e "
                "LEFT JOIN departments d ON e.department_id = d.department_id "
                "WHERE e.last_name LIKE ? OR e.first_name LIKE ? "
                "OR e.position LIKE ? OR d.department_name LIKE ? "
                "OR e.email LIKE ? "
                "ORDER BY e.employee_id LIMIT ? OFFSET ?");

            pstmt->setString(1, pattern);
            pstmt->setString(2, pattern);
            pstmt->setString(3, pattern);
            pstmt->setString(4, pattern);
            pstmt->setString(5, pattern);
            pstmt->setInt(6, limit);
            pstmt->setInt(7, offset);

            ResultSet* res = pstmt->executeQuery();

            while (res->next()) {
                Employee^ emp = gcnew Employee();
                emp->employee_id = res->getInt("employee_id");
                emp->last_name = ToSystemString(res->getString("last_name"));
                emp->first_name = ToSystemString(res->getString("first_name"));
                emp->middle_name = ToSystemString(res->getString("middle_name"));
                emp->position = ToSystemString(res->getString("position"));
                emp->salary = res->getDouble("salary");
                emp->department_id = res->getInt("department_id");
                emp->department_name = ToSystemString(res->getString("department_name"));
                emp->hire_date = ToSystemString(res->getString("hire_date"));
                emp->email = ToSystemString(res->getString("email"));
                emp->phone = ToSystemString(res->getString("phone"));
                emp->birth_date = ToSystemString(res->getString("birth_date"));
                result->Add(emp);
            }

            delete res;
            delete pstmt;
        }
        catch (SQLException&) {}

        return result;
    }

    Employee^ GetEmployeeById(int id) {
        try {
            PreparedStatement* pstmt = conn->prepareStatement(
                "SELECT employee_id, last_name, first_name, middle_name, position, "
                "salary, department_id, hire_date, email, phone, birth_date "
                "FROM employees WHERE employee_id = ?");
            pstmt->setInt(1, id);
            ResultSet* res = pstmt->executeQuery();

            if (res->next()) {
                Employee^ emp = gcnew Employee();
                emp->employee_id = res->getInt("employee_id");
                emp->last_name = ToSystemString(res->getString("last_name"));
                emp->first_name = ToSystemString(res->getString("first_name"));
                emp->middle_name = ToSystemString(res->getString("middle_name"));
                emp->position = ToSystemString(res->getString("position"));
                emp->salary = res->getDouble("salary");
                emp->department_id = res->getInt("department_id");
                emp->hire_date = ToSystemString(res->getString("hire_date"));
                emp->email = ToSystemString(res->getString("email"));
                emp->phone = ToSystemString(res->getString("phone"));
                emp->birth_date = ToSystemString(res->getString("birth_date"));
                delete res;
                delete pstmt;
                return emp;
            }
            delete res;
            delete pstmt;
        }
        catch (SQLException&) {}
        return nullptr;
    }

    bool AddEmployee(Employee^ emp) {
        try {
            PreparedStatement* pstmt = conn->prepareStatement(
                "INSERT INTO employees (last_name, first_name, middle_name, position, "
                "salary, department_id, hire_date, email, phone, birth_date) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
            pstmt->setString(1, ToStdString(emp->last_name));
            pstmt->setString(2, ToStdString(emp->first_name));
            pstmt->setString(3, ToStdString(emp->middle_name));
            pstmt->setString(4, ToStdString(emp->position));
            pstmt->setDouble(5, emp->salary);
            pstmt->setInt(6, emp->department_id);
            pstmt->setString(7, ToStdString(emp->hire_date));
            pstmt->setString(8, ToStdString(emp->email));
            pstmt->setString(9, ToStdString(emp->phone));
            pstmt->setString(10, ToStdString(emp->birth_date));

            bool success = pstmt->executeUpdate() > 0;
            delete pstmt;
            return success;
        }
        catch (SQLException&) {
            return false;
        }
    }

    bool UpdateEmployee(Employee^ emp) {
        try {
            PreparedStatement* pstmt = conn->prepareStatement(
                "UPDATE employees SET last_name = ?, first_name = ?, middle_name = ?, "
                "position = ?, salary = ?, department_id = ?, hire_date = ?, "
                "email = ?, phone = ?, birth_date = ? WHERE employee_id = ?");
            pstmt->setString(1, ToStdString(emp->last_name));
            pstmt->setString(2, ToStdString(emp->first_name));
            pstmt->setString(3, ToStdString(emp->middle_name));
            pstmt->setString(4, ToStdString(emp->position));
            pstmt->setDouble(5, emp->salary);
            pstmt->setInt(6, emp->department_id);
            pstmt->setString(7, ToStdString(emp->hire_date));
            pstmt->setString(8, ToStdString(emp->email));
            pstmt->setString(9, ToStdString(emp->phone));
            pstmt->setString(10, ToStdString(emp->birth_date));
            pstmt->setInt(11, emp->employee_id);

            bool success = pstmt->executeUpdate() > 0;
            delete pstmt;
            return success;
        }
        catch (SQLException&) {
            return false;
        }
    }

    bool DeleteEmployee(int id) {
        try {
            PreparedStatement* pstmt = conn->prepareStatement("DELETE FROM employees WHERE employee_id = ?");
            pstmt->setInt(1, id);
            bool success = pstmt->executeUpdate() > 0;
            delete pstmt;
            return success;
        }
        catch (SQLException&) {
            return false;
        }
    }

    List<Department^>^ GetDepartments() {
        List<Department^>^ result = gcnew List<Department^>();

        try {
            Statement* stmt = conn->createStatement();
            ResultSet* res = stmt->executeQuery(
                "SELECT department_id, department_code, department_name, "
                "manager_name, phone, location, budget FROM departments ORDER BY department_id");

            while (res->next()) {
                Department^ dept = gcnew Department();
                dept->department_id = res->getInt("department_id");
                dept->department_code = ToSystemString(res->getString("department_code"));
                dept->department_name = ToSystemString(res->getString("department_name"));
                dept->manager_name = ToSystemString(res->getString("manager_name"));
                dept->phone = ToSystemString(res->getString("phone"));
                dept->location = ToSystemString(res->getString("location"));
                dept->budget = res->getDouble("budget");
                result->Add(dept);
            }
            delete res;
            delete stmt;
        }
        catch (SQLException&) {}
        return result;
    }
};