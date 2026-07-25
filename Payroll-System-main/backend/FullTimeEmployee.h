#ifndef FULLTIMEEMPLOYEE_H
#define FULLTIMEEMPLOYEE_H

#include "Employee.h"

/**
 * @class FullTimeEmployee
 * @brief Full-time employee with HRA, DA, PF and progressive tax slabs.
 *
 * Demonstrates: Inheritance, Polymorphism, Virtual Function Override.
 */
class FullTimeEmployee : public Employee {
private:
    double hra;
    double da;
    double pf;

public:
    FullTimeEmployee();
    FullTimeEmployee(const std::string& id, const std::string& name,
                     const std::string& dept, const std::string& desig,
                     double salary, double bonusAmt, double allowAmt,
                     double hraAmt, double daAmt, double pfAmt);

    double calculateTax() const override;
    double calculateGrossSalary() const override;
    double calculatePF() const override;
    std::string getEmployeeType() const override;
    std::string serialize() const override;
    void display() const override;

    double getHRA() const;
    double getDA() const;
    double getPF() const;
    void setHRA(double val);
    void setDA(double val);
    void setPF(double val);
};

#endif // FULLTIMEEMPLOYEE_H
