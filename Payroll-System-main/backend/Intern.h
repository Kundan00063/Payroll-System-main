#ifndef INTERN_H
#define INTERN_H

#include "Employee.h"

/**
 * @class Intern
 * @brief Intern employee with 5% tax rate.
 *
 * Demonstrates: Inheritance, Polymorphism, Virtual Function Override.
 */
class Intern : public Employee {
private:
    std::string university;
    int internshipMonths;

public:
    Intern();
    Intern(const std::string& id, const std::string& name,
           const std::string& dept, const std::string& desig,
           double salary, double bonusAmt, double allowAmt,
           const std::string& uni, int months);

    double calculateTax() const override;
    std::string getEmployeeType() const override;
    std::string serialize() const override;
    void display() const override;

    std::string getUniversity() const;
    int getInternshipMonths() const;
    void setUniversity(const std::string& uni);
    void setInternshipMonths(int months);
};

#endif // INTERN_H
