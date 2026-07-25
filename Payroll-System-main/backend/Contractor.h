#ifndef CONTRACTOR_H
#define CONTRACTOR_H

#include "Employee.h"

/**
 * @class Contractor
 * @brief Contract-based employee with flat 10% tax rate.
 *
 * Demonstrates: Inheritance, Polymorphism, Virtual Function Override.
 */
class Contractor : public Employee {
private:
    double contractDuration;

public:
    Contractor();
    Contractor(const std::string& id, const std::string& name,
               const std::string& dept, const std::string& desig,
               double salary, double bonusAmt, double allowAmt,
               double duration);

    double calculateTax() const override;
    std::string getEmployeeType() const override;
    std::string serialize() const override;
    void display() const override;

    double getContractDuration() const;
    void setContractDuration(double duration);
};

#endif // CONTRACTOR_H
