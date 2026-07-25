#include "Intern.h"

Intern::Intern() : Employee(), university(""), internshipMonths(0) {}

Intern::Intern(const std::string& id, const std::string& name,
               const std::string& dept, const std::string& desig,
               double salary, double bonusAmt, double allowAmt,
               const std::string& uni, int months)
    : Employee(id, name, dept, desig, salary, bonusAmt, allowAmt),
      university(uni), internshipMonths(months) {}

double Intern::calculateTax() const {
    return calculateGrossSalary() * 0.05;
}

std::string Intern::getEmployeeType() const {
    return "Intern";
}

std::string Intern::serialize() const {
    return "Intern|" + employeeId + "|" + employeeName + "|" + department + "|" +
           designation + "|" + std::to_string(baseSalary) + "|" +
           std::to_string(bonus) + "|" + std::to_string(allowance) + "|" +
           university + "|" + std::to_string(internshipMonths) + "|0|" + photoPath;
}

void Intern::display() const {
    std::cout << "[Intern] ";
    Employee::display();
    std::cout << "  University: " << university
              << " | Duration: " << internshipMonths << " months" << std::endl;
}

std::string Intern::getUniversity() const { return university; }
int Intern::getInternshipMonths() const { return internshipMonths; }
void Intern::setUniversity(const std::string& uni) { university = uni; }
void Intern::setInternshipMonths(int months) { internshipMonths = months; }
