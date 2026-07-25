#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "Employee.h"

/**
 * @class FileManager
 * @brief Handles all file I/O operations for employee and payroll persistence.
 *
 * Demonstrates: Exception Handling, Templates (utility), Static utility methods.
 */
class FileManager {
private:
    std::string employeesFile;
    std::string payrollFile;
    std::string logsFile;

    template<typename T>
    static void writeLine(std::ofstream& file, const T& value);

public:
    FileManager(const std::string& empFile = "data/employees.txt",
                const std::string& payFile = "data/payroll.txt",
                const std::string& logFile = "data/logs.txt");

    void saveEmployees(const std::vector<std::unique_ptr<Employee>>& employees);
    std::vector<std::unique_ptr<Employee>> loadEmployees();
    void savePayrollRecord(const std::string& record);
    std::vector<std::string> readPayrollHistory() const;
    void logAction(const std::string& action);
    std::vector<std::string> readLogs();

    static std::unique_ptr<Employee> parseEmployeeLine(const std::string& line);
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

#endif // FILEMANAGER_H
