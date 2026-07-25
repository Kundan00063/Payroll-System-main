#ifndef ANALYTICS_H
#define ANALYTICS_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Employee.h"
/**
 * @class Analytics
 * @brief Computes payroll statistics and department-wise analytics.
 *
 * Demonstrates: Aggregation (references employee collection),
 * Templates for generic min/max operations.
 */
class Analytics {
private:
    const std::vector<std::unique_ptr<Employee>>& employees;
    template<typename CompareFunc>
    double findExtremeSalary(CompareFunc compare) const;
public:
    explicit Analytics(const std::vector<std::unique_ptr<Employee>>& emps);
    double getHighestSalary() const;
    double getLowestSalary() const;
    double getAverageSalary() const;
    double getTotalPayroll() const;
    double getTotalTax() const;
    int getEmployeeCount() const;
    std::map<std::string, double> getDepartmentWiseSalary() const;
    std::map<std::string, int> getEmployeeTypeDistribution() const;
    std::map<std::string, double> getDepartmentWiseAverage() const;
    std::vector<std::pair<std::string, double>> getTopPaidEmployees(int count = 5) const;
    std::string toJson() const;
};
#endif // ANALYTICS_H
