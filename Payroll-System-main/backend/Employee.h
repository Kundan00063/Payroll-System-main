#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <string>
#include <stdexcept>

/**
 * @class Employee
 * @brief Abstract base class representing an employee in the payroll system.
 *
 * Demonstrates: Abstraction, Encapsulation, Pure Virtual Functions,
 * Virtual Functions, Dynamic Binding, Static Members, Friend Function.
 */
class Employee {
protected:
    std::string employeeId;
    std::string employeeName;
    std::string department;
    std::string designation;
    double baseSalary;
    double bonus;
    double allowance;
    std::string photoPath;

    static int totalEmployees;

public:
    Employee();
    Employee(const std::string& id, const std::string& name,
             const std::string& dept, const std::string& desig,
             double salary, double bonusAmt, double allowAmt);
    Employee(const Employee& other);
    virtual ~Employee();

    virtual double calculateTax() const = 0;
    virtual double calculateGrossSalary() const;
    virtual double calculateNetSalary() const;
    virtual double calculatePF() const;
    virtual std::string getEmployeeType() const = 0;
    virtual std::string serialize() const = 0;
    virtual void display() const;

    std::string getEmployeeId() const;
    std::string getEmployeeName() const;
    std::string getDepartment() const;
    std::string getDesignation() const;
    double getBaseSalary() const;
    double getBonus() const;
    double getAllowance() const;
    std::string getPhotoPath() const;

    void setEmployeeId(const std::string& id);
    void setEmployeeName(const std::string& name);
    void setDepartment(const std::string& dept);
    void setDesignation(const std::string& desig);
    void setBaseSalary(double salary);
    void setBonus(double bonusAmt);
    void setAllowance(double allowAmt);
    void setPhotoPath(const std::string& path);

    static int getTotalEmployees();
    static void resetEmployeeCount();

    friend std::ostream& operator<<(std::ostream& os, const Employee& emp);
};

#endif // EMPLOYEE_H
