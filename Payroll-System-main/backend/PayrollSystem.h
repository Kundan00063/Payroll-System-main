#ifndef PAYROLLSYSTEM_H
#define PAYROLLSYSTEM_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Employee.h"
#include "FileManager.h"
#include "Analytics.h"

/**
 * @class PayrollSystem
 * @brief Central system managing employees, payroll, and persistence.
 *
 * Demonstrates: Composition (FileManager), Aggregation (Analytics),
 * Exception Handling, Smart Pointers, Dynamic Binding.
 */
class PayrollSystem {
private:
    std::vector<std::unique_ptr<Employee>> employees;
    FileManager fileManager;
    std::vector<std::string> salaryHistory;

    void autoSave();
    std::string employeeToJson(const Employee& emp) const;

public:
    PayrollSystem();
    ~PayrollSystem();

    void loadData();
    void addEmployee(std::unique_ptr<Employee> emp);
    bool removeEmployee(const std::string& id);
    bool updateEmployee(const std::string& id, std::unique_ptr<Employee> updatedEmp);
    Employee* searchById(const std::string& id);
    std::vector<Employee*> searchByName(const std::string& name);
    std::vector<Employee*> filterByDepartment(const std::string& dept);
    std::vector<Employee*> filterByType(const std::string& type);
    std::vector<Employee*> filterBySalaryRange(double minSal, double maxSal);

    std::string generatePayslip(const std::string& id);
    std::string calculateSalary(const std::string& id);
    std::string getAllEmployeesJson() const;
    std::string getEmployeeJson(const std::string& id) const;
    std::string getDashboardJson() const;
    std::string getAnalyticsJson() const;
    std::string getPayrollHistoryJson() const;

    void recordSalaryIncrement(const std::string& id, double oldSalary, double newSalary);
    std::vector<std::string> getSalaryHistory() const;

    bool idExists(const std::string& id) const;
    int getEmployeeCount() const;
};

#endif // PAYROLLSYSTEM_H
