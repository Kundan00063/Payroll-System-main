#include "Analytics.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <cmath>
Analytics::Analytics(const std::vector<std::unique_ptr<Employee>>& emps)
    : employees(emps) {}
template<typename CompareFunc>
double Analytics::findExtremeSalary(CompareFunc compare) const {
    if (employees.empty()) return 0.0;
    double result = employees[0]->calculateGrossSalary();
    for (const auto& emp : employees) {
        double salary = emp->calculateGrossSalary();
        if (compare(salary, result)) {
            result = salary;
        }
    }
    return result;
}
double Analytics::getHighestSalary() const {
    return findExtremeSalary([](double a, double b) { return a > b; });
}
double Analytics::getLowestSalary() const {
    return findExtremeSalary([](double a, double b) { return a < b; });
}
double Analytics::getAverageSalary() const {
    if (employees.empty()) return 0.0;
    double total = 0.0;
    for (const auto& emp : employees) {
        total += emp->calculateGrossSalary();
    }
    return total / employees.size();
}
double Analytics::getTotalPayroll() const {
    double total = 0.0;
    for (const auto& emp : employees) {
        total += emp->calculateNetSalary();
    }
    return total;
}
double Analytics::getTotalTax() const {
    double total = 0.0;
    for (const auto& emp : employees) {
        total += emp->calculateTax();
    }
    return total;
}
int Analytics::getEmployeeCount() const {
    return static_cast<int>(employees.size());
}
std::map<std::string, double> Analytics::getDepartmentWiseSalary() const {
    std::map<std::string, double> deptSalary;
    for (const auto& emp : employees) {
        deptSalary[emp->getDepartment()] += emp->calculateGrossSalary();
    }
    return deptSalary;
}
std::map<std::string, int> Analytics::getEmployeeTypeDistribution() const {
    std::map<std::string, int> typeCount;
    for (const auto& emp : employees) {
        typeCount[emp->getEmployeeType()]++;
    }
    return typeCount;
}
std::map<std::string, double> Analytics::getDepartmentWiseAverage() const {
    std::map<std::string, double> deptTotal;
    std::map<std::string, int> deptCount;
    for (const auto& emp : employees) {
        deptTotal[emp->getDepartment()] += emp->calculateGrossSalary();
        deptCount[emp->getDepartment()]++;
    }
    std::map<std::string, double> deptAvg;
    for (const auto& [dept, total] : deptTotal) {
        deptAvg[dept] = total / deptCount[dept];
    }
    return deptAvg;
}
std::vector<std::pair<std::string, double>> Analytics::getTopPaidEmployees(int count) const {
    std::vector<std::pair<std::string, double>> ranked;
    for (const auto& emp : employees) {
        ranked.emplace_back(emp->getEmployeeName(), emp->calculateGrossSalary());
    }
    std::sort(ranked.begin(), ranked.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    if (static_cast<int>(ranked.size()) > count) {
        ranked.resize(count);
    }
    return ranked;
}
std::string Analytics::toJson() const {
    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{";
    json << "\"employeeCount\":" << getEmployeeCount() << ",";
    json << "\"highestSalary\":" << getHighestSalary() << ",";
    json << "\"lowestSalary\":" << getLowestSalary() << ",";
    json << "\"averageSalary\":" << getAverageSalary() << ",";
    json << "\"totalPayroll\":" << getTotalPayroll() << ",";
    json << "\"totalTax\":" << getTotalTax() << ",";
    json << "\"departmentSalary\":{";
    auto deptSal = getDepartmentWiseSalary();
    bool first = true;
    for (const auto& [dept, sal] : deptSal) {
        if (!first) json << ",";
        json << "\"" << dept << "\":" << sal;
        first = false;
    }
    json << "},";
    json << "\"typeDistribution\":{";
    auto typeDist = getEmployeeTypeDistribution();
    first = true;
    for (const auto& [type, cnt] : typeDist) {
        if (!first) json << ",";
        json << "\"" << type << "\":" << cnt;
        first = false;
    }
    json << "},";
    json << "\"topPaid\":[";
    auto topPaid = getTopPaidEmployees(5);
    first = true;
    for (const auto& [name, sal] : topPaid) {
        if (!first) json << ",";
        json << "{\"name\":\"" << name << "\",\"salary\":" << sal << "}";
        first = false;
    }
    json << "]}";
    return json.str();
}