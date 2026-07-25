#include "Employee.h"

int Employee::totalEmployees = 0;

Employee::Employee(): employeeId(""), employeeName(""), department(""), designation(""),
      baseSalary(0.0), bonus(0.0), allowance(0.0), photoPath("") {
    ++totalEmployees;
}

Employee::Employee(const std::string& id, const std::string& name,
                   const std::string& dept, const std::string& desig,
                   double salary, double bonusAmt, double allowAmt)
    : employeeId(id), employeeName(name), department(dept),
      designation(desig), baseSalary(salary), bonus(bonusAmt),
      allowance(allowAmt), photoPath("") {
    if (salary < 0 || bonusAmt < 0 || allowAmt < 0) {
        throw std::invalid_argument("Salary components cannot be negative");
    }
    ++totalEmployees;
}

Employee::Employee(const Employee& other)
    : employeeId(other.employeeId), employeeName(other.employeeName),
      department(other.department), designation(other.designation),
      baseSalary(other.baseSalary), bonus(other.bonus),
      allowance(other.allowance), photoPath(other.photoPath) {
    ++totalEmployees;
}

Employee::~Employee() {
    --totalEmployees;
}

double Employee::calculateGrossSalary() const {
    return baseSalary + bonus + allowance;
}

double Employee::calculateNetSalary() const {
    return calculateGrossSalary() - calculateTax() - calculatePF();
}

double Employee::calculatePF() const {
    return 0.0;
}

void Employee::display() const {
    std::cout << "ID: " << employeeId
              << " | Name: " << employeeName
              << " | Dept: " << department
              << " | Type: " << getEmployeeType()
              << " | Gross: " << calculateGrossSalary()
              << " | Net: " << calculateNetSalary() << std::endl;
}

std::string Employee::getEmployeeId() const { return employeeId; }
std::string Employee::getEmployeeName() const { return employeeName; }
std::string Employee::getDepartment() const { return department; }
std::string Employee::getDesignation() const { return designation; }
double Employee::getBaseSalary() const { return baseSalary; }
double Employee::getBonus() const { return bonus; }
double Employee::getAllowance() const { return allowance; }
std::string Employee::getPhotoPath() const { return photoPath; }

void Employee::setEmployeeId(const std::string& id) { employeeId = id; }
void Employee::setEmployeeName(const std::string& name) { employeeName = name; }
void Employee::setDepartment(const std::string& dept) { department = dept; }
void Employee::setDesignation(const std::string& desig) { designation = desig; }

void Employee::setBaseSalary(double salary) {
    if (salary < 0) throw std::invalid_argument("Base salary cannot be negative");
    baseSalary = salary;
}

void Employee::setBonus(double bonusAmt) {
    if (bonusAmt < 0) throw std::invalid_argument("Bonus cannot be negative");
    bonus = bonusAmt;
}

void Employee::setAllowance(double allowAmt) {
    if (allowAmt < 0) throw std::invalid_argument("Allowance cannot be negative");
    allowance = allowAmt;
}

void Employee::setPhotoPath(const std::string& path) { photoPath = path; }

int Employee::getTotalEmployees() { return totalEmployees; }
void Employee::resetEmployeeCount() { totalEmployees = 0; }

std::ostream& operator<<(std::ostream& os, const Employee& emp) {
    os << emp.getEmployeeId() << " - " << emp.getEmployeeName()
       << " (" << emp.getEmployeeType() << ")";
    return os;
}
