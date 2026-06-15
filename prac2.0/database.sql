
 CREATE DATABASE hr_db;
USE hr_db;

CREATE TABLE departments (
    department_id INT AUTO_INCREMENT PRIMARY KEY,
    department_code VARCHAR(10) NOT NULL UNIQUE,
    department_name VARCHAR(100) NOT NULL UNIQUE,
    manager_name VARCHAR(100),
    phone VARCHAR(20),
    location VARCHAR(100),
    budget DECIMAL(12, 2)
);

CREATE TABLE employees (
    employee_id INT AUTO_INCREMENT PRIMARY KEY,
    last_name VARCHAR(50) NOT NULL,
    first_name VARCHAR(50) NOT NULL,
    middle_name VARCHAR(50),
    position VARCHAR(100) NOT NULL,
    salary DECIMAL(10, 2) NOT NULL,
    department_id INT,
    hire_date DATE NOT NULL,
    email VARCHAR(100),
    phone VARCHAR(20),
    birth_date DATE,
    FOREIGN KEY (department_id) REFERENCES departments(department_id)
);



INSERT INTO departments (department_code, department_name, manager_name, phone, location, budget) VALUES 
('IT-01', 'IT', 'Tehnikov Anton Pavlovich', '+7(495)123-4501', '5th floor', 15000000.00),
('HR-02', 'HR', 'Kadrova Elena Vladimirovna', '+7(495)123-4502', '3rd floor', 8000000.00),
('SALE-03', 'Sales', 'Marketov Dmitry Sergeevich', '+7(495)123-4503', '2nd floor', 20000000.00);

INSERT INTO employees (last_name, first_name, middle_name, position, salary, department_id, hire_date, email, phone, birth_date) VALUES 
('Ivanov', 'Ivan', 'Ivanovich', 'Senior Programmer', 120000, 1, '2021-01-15', 'ivanov@company.ru', '+7(916)123-4501', '1985-05-10'),
('Petrova', 'Maria', 'Sergeevna', 'HR Director', 150000, 2, '2021-02-10', 'petrova@company.ru', '+7(916)123-4502', '1982-08-20'),
('Sidorov', 'Alexey', 'Petrovich', 'Sales Manager', 140000, 3, '2021-03-20', 'sidorov@company.ru', '+7(916)123-4503', '1988-03-15');



