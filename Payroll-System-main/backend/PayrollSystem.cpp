#include "PayrollSystem.h"
#include "FullTimeEmployee.h"
#include "Contractor.h"
#include "Intern.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

PayrollSystem::PayrollSystem() : fileManager("data/employees.txt", "data/payroll.txt", "data/logs.txt") {
    loadData();
}

PayrollSystem::~PayrollSystem() {
    try {
        autoSave();
    } catch (...) {}
}

void PayrollSystem::loadData() {
    Employee::resetEmployeeCount();
    employees = fileManager.loadEmployees();
}

void PayrollSystem::autoSave() {
    fileManager.saveEmployees(employees);
}

void PayrollSystem::addEmployee(std::unique_ptr<Employee> emp) {
    if (!emp) throw std::invalid_argument("Employee cannot be null");
    if (idExists(emp->getEmployeeId())) {
        throw std::runtime_error("Employee ID already exists: " + emp->getEmployeeId());
    }
    employees.push_back(std::move(emp));
    autoSave();
    fileManager.logAction("Added employee: " + employees.back()->getEmployeeId());
}

bool PayrollSystem::removeEmployee(const std::string& id) {
    auto it = std::find_if(employees.begin(), employees.end(),
        [&id](const std::unique_ptr<Employee>& e) {
            return e->getEmployeeId() == id;
        });

    if (it == employees.end()) return false;

    fileManager.logAction("Removed employee: " + id);
    employees.erase(it);
    autoSave();
    return true;
}

bool PayrollSystem::updateEmployee(const std::string& id, std::unique_ptr<Employee> updatedEmp) {
    if (!updatedEmp) throw std::invalid_argument("Updated employee cannot be null");

    auto it = std::find_if(employees.begin(), employees.end(),
        [&id](const std::unique_ptr<Employee>& e) {
            return e->getEmployeeId() == id;
        });

    if (it == employees.end()) return false;

    double oldSalary = (*it)->getBaseSalary();
    double newSalary = updatedEmp->getBaseSalary();
    if (oldSalary != newSalary) {
        recordSalaryIncrement(id, oldSalary, newSalary);
    }

    *it = std::move(updatedEmp);
    autoSave();
    fileManager.logAction("Updated employee: " + id);
    return true;
}

Employee* PayrollSystem::searchById(const std::string& id) {
    auto it = std::find_if(employees.begin(), employees.end(),
        [&id](const std::unique_ptr<Employee>& e) {
            return e->getEmployeeId() == id;
        });
    return (it != employees.end()) ? it->get() : nullptr;
}

std::vector<Employee*> PayrollSystem::searchByName(const std::string& name) {
    std::vector<Employee*> results;
    for (const auto& emp : employees) {
        if (emp->getEmployeeName().find(name) != std::string::npos) {
            results.push_back(emp.get());
        }
    }
    return results;
}

std::vector<Employee*> PayrollSystem::filterByDepartment(const std::string& dept) {
    std::vector<Employee*> results;
    for (const auto& emp : employees) {
        if (emp->getDepartment() == dept) results.push_back(emp.get());
    }
    return results;
}

std::vector<Employee*> PayrollSystem::filterByType(const std::string& type) {
    std::vector<Employee*> results;
    for (const auto& emp : employees) {
        if (emp->getEmployeeType() == type) results.push_back(emp.get());
    }
    return results;
}

std::vector<Employee*> PayrollSystem::filterBySalaryRange(double minSal, double maxSal) {
    std::vector<Employee*> results;
    for (const auto& emp : employees) {
        double gross = emp->calculateGrossSalary();
        if (gross >= minSal && gross <= maxSal) results.push_back(emp.get());
    }
    return results;
}

std::string PayrollSystem::employeeToJson(const Employee& emp) const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{";
    json << "\"id\":\"" << emp.getEmployeeId() << "\",";
    json << "\"name\":\"" << emp.getEmployeeName() << "\",";
    json << "\"department\":\"" << emp.getDepartment() << "\",";
    json << "\"designation\":\"" << emp.getDesignation() << "\",";
    json << "\"type\":\"" << emp.getEmployeeType() << "\",";
    json << "\"baseSalary\":" << emp.getBaseSalary() << ",";
    json << "\"bonus\":" << emp.getBonus() << ",";
    json << "\"allowance\":" << emp.getAllowance() << ",";
    json << "\"grossSalary\":" << emp.calculateGrossSalary() << ",";
    json << "\"tax\":" << emp.calculateTax() << ",";
    json << "\"pf\":" << emp.calculatePF() << ",";
    json << "\"netSalary\":" << emp.calculateNetSalary() << ",";
    json << "\"photoPath\":\"" << emp.getPhotoPath() << "\"";

    if (emp.getEmployeeType() == "FullTime") {
        const auto& ft = dynamic_cast<const FullTimeEmployee&>(emp);
        json << ",\"hra\":" << ft.getHRA();
        json << ",\"da\":" << ft.getDA();
        json << ",\"pfAmount\":" << ft.getPF();
    } else if (emp.getEmployeeType() == "Contractor") {
        const auto& c = dynamic_cast<const Contractor&>(emp);
        json << ",\"contractDuration\":" << c.getContractDuration();
    } else if (emp.getEmployeeType() == "Intern") {
        const auto& i = dynamic_cast<const Intern&>(emp);
        json << ",\"university\":\"" << i.getUniversity() << "\"";
        json << ",\"internshipMonths\":" << i.getInternshipMonths();
    }

    json << "}";
    return json.str();
}

std::string PayrollSystem::generatePayslip(const std::string& id) {
    Employee* emp = searchById(id);
    if (!emp) throw std::runtime_error("Employee not found: " + id);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif

    std::ostringstream payslip;
    payslip << std::fixed << std::setprecision(2);
    payslip << "{";
    payslip << "\"employee\":" << employeeToJson(*emp) << ",";
    payslip << "\"generatedDate\":\"" << std::put_time(&tm_buf, "%Y-%m-%d") << "\",";
    payslip << "\"month\":\"" << std::put_time(&tm_buf, "%B %Y") << "\",";
    payslip << "\"companyName\":\"PayrollPro Enterprise\",";
    payslip << "\"breakdown\":{";
    payslip << "\"baseSalary\":" << emp->getBaseSalary() << ",";
    payslip << "\"bonus\":" << emp->getBonus() << ",";
    payslip << "\"allowance\":" << emp->getAllowance() << ",";
    payslip << "\"grossSalary\":" << emp->calculateGrossSalary() << ",";
    payslip << "\"tax\":" << emp->calculateTax() << ",";
    payslip << "\"pf\":" << emp->calculatePF() << ",";
    payslip << "\"netSalary\":" << emp->calculateNetSalary();
    payslip << "}}";

    std::ostringstream record;
    record << std::put_time(&tm_buf, "%Y-%m-%d") << "|" << id << "|"
           << emp->calculateGrossSalary() << "|" << emp->calculateTax() << "|"
           << emp->calculateNetSalary();
    fileManager.savePayrollRecord(record.str());

    return payslip.str();
}

std::string PayrollSystem::calculateSalary(const std::string& id) {
    Employee* emp = searchById(id);
    if (!emp) throw std::runtime_error("Employee not found: " + id);
    return employeeToJson(*emp);
}

std::string PayrollSystem::getAllEmployeesJson() const {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < employees.size(); ++i) {
        if (i > 0) json << ",";
        json << employeeToJson(*employees[i]);
    }
    json << "]";
    return json.str();
}

std::string PayrollSystem::getEmployeeJson(const std::string& id) const {
    for (const auto& emp : employees) {
        if (emp->getEmployeeId() == id) {
            return employeeToJson(*emp);
        }
    }
    throw std::runtime_error("Employee not found: " + id);
}

std::string PayrollSystem::getDashboardJson() const {
    Analytics analytics(employees);
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{";
    json << "\"totalEmployees\":" << analytics.getEmployeeCount() << ",";
    json << "\"totalPayroll\":" << analytics.getTotalPayroll() << ",";
    json << "\"averageSalary\":" << analytics.getAverageSalary() << ",";
    json << "\"highestSalary\":" << analytics.getHighestSalary() << ",";
    json << "\"totalTax\":" << analytics.getTotalTax();
    json << "}";
    return json.str();
}

std::string PayrollSystem::getAnalyticsJson() const {
    Analytics analytics(employees);
    return analytics.toJson();
}

std::string PayrollSystem::getPayrollHistoryJson() const {
    auto records = fileManager.readPayrollHistory();
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < records.size(); ++i) {
        if (i > 0) json << ",";
        auto parts = FileManager::split(records[i], '|');
        if (parts.size() >= 5) {
            json << "{\"date\":\"" << parts[0] << "\",";
            json << "\"employeeId\":\"" << parts[1] << "\",";
            json << "\"gross\":" << parts[2] << ",";
            json << "\"tax\":" << parts[3] << ",";
            json << "\"net\":" << parts[4] << "}";
        }
    }
    json << "]";
    return json.str();
}

void PayrollSystem::recordSalaryIncrement(const std::string& id, double oldSalary, double newSalary) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif

    std::ostringstream record;
    record << std::put_time(&tm_buf, "%Y-%m-%d") << "|" << id << "|"
           << oldSalary << "|" << newSalary;
    salaryHistory.push_back(record.str());
    fileManager.logAction("Salary increment for " + id + ": " +
                          std::to_string(oldSalary) + " -> " + std::to_string(newSalary));
}

std::vector<std::string> PayrollSystem::getSalaryHistory() const {
    return salaryHistory;
}

bool PayrollSystem::idExists(const std::string& id) const {
    return std::any_of(employees.begin(), employees.end(),
        [&id](const std::unique_ptr<Employee>& e) {
            return e->getEmployeeId() == id;
        });
}

int PayrollSystem::getEmployeeCount() const {
    return static_cast<int>(employees.size());
}
