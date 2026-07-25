#include "FullTimeEmployee.h"

FullTimeEmployee::FullTimeEmployee() : Employee(), hra(0.0), da(0.0), pf(0.0) {}

FullTimeEmployee::FullTimeEmployee(const std::string& id, const std::string& name,
                                   const std::string& dept, const std::string& desig,
                                   double salary, double bonusAmt, double allowAmt,
                                   double hraAmt, double daAmt, double pfAmt)
    : Employee(id, name, dept, desig, salary, bonusAmt, allowAmt),
      hra(hraAmt), da(daAmt), pf(pfAmt) {}

double FullTimeEmployee::calculateGrossSalary() const {
    return baseSalary + bonus + allowance + hra + da;
}

double FullTimeEmployee::calculateTax() const {
    double gross = calculateGrossSalary();
    double tax = 0.0;

    if (gross <= 300000) {
        tax = gross * 0.05;
    } else if (gross <= 600000) {
        tax = 300000 * 0.05 + (gross - 300000) * 0.10;
    } else if (gross <= 1000000) {
        tax = 300000 * 0.05 + 300000 * 0.10 + (gross - 600000) * 0.20;
    } else {
        tax = 300000 * 0.05 + 300000 * 0.10 + 400000 * 0.20 + (gross - 1000000) * 0.30;
    }
    return tax;
}

double FullTimeEmployee::calculatePF() const {
    return pf > 0 ? pf : baseSalary * 0.12;
}

std::string FullTimeEmployee::getEmployeeType() const {
    return "FullTime";
}

std::string FullTimeEmployee::serialize() const {
    return "FullTime|" + employeeId + "|" + employeeName + "|" + department + "|" +
           designation + "|" + std::to_string(baseSalary) + "|" +
           std::to_string(bonus) + "|" + std::to_string(allowance) + "|" +
           std::to_string(hra) + "|" + std::to_string(da) + "|" +
           std::to_string(pf) + "|" + photoPath;
}

void FullTimeEmployee::display() const {
    std::cout << "[Full-Time] ";
    Employee::display();
    std::cout << "  HRA: " << hra << " | DA: " << da << " | PF: " << calculatePF() << std::endl;
}

double FullTimeEmployee::getHRA() const { return hra; }
double FullTimeEmployee::getDA() const { return da; }
double FullTimeEmployee::getPF() const { return pf; }
void FullTimeEmployee::setHRA(double val) { hra = val; }
void FullTimeEmployee::setDA(double val) { da = val; }
void FullTimeEmployee::setPF(double val) { pf = val; }
