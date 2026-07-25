#include "Contractor.h"
Contractor::Contractor() : Employee(), contractDuration(0.0) {}
Contractor::Contractor(const std::string& id, const std::string& name,
                       const std::string& dept, const std::string& desig,
                       double salary, double bonusAmt, double allowAmt,
                       double duration)
    : Employee(id, name, dept, desig, salary, bonusAmt, allowAmt),
      contractDuration(duration) {}
double Contractor::calculateTax() const {
    return calculateGrossSalary() * 0.10;
}
std::string Contractor::getEmployeeType() const {
    return "Contractor";
}
std::string Contractor::serialize() const {
    return "Contractor|" + employeeId + "|" + employeeName + "|" + department + "|" +
           designation + "|" + std::to_string(baseSalary) + "|" +
           std::to_string(bonus) + "|" + std::to_string(allowance) + "|" +
           std::to_string(contractDuration) + "|0|0|" + photoPath;
}
void Contractor::display() const {
    std::cout << "[Contractor] ";
    Employee::display();
    std::cout << "  Contract Duration: " << contractDuration << " months" << std::endl;
}
double Contractor::getContractDuration() const { return contractDuration; }
void Contractor::setContractDuration(double duration) { contractDuration = duration; }