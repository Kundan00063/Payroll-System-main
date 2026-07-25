#include "FileManager.h"
#include "FullTimeEmployee.h"
#include "Contractor.h"
#include "Intern.h"
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <filesystem>

template<typename T>
void FileManager::writeLine(std::ofstream& file, const T& value) {
    file << value << std::endl;
}

FileManager::FileManager(const std::string& empFile,
                         const std::string& payFile,
                         const std::string& logFile)
    : employeesFile(empFile), payrollFile(payFile), logsFile(logFile) {
    std::filesystem::create_directories("data");
}

std::vector<std::string> FileManager::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::unique_ptr<Employee> FileManager::parseEmployeeLine(const std::string& line) {
    if (line.empty()) return nullptr;

    auto parts = split(line, '|');
    if (parts.size() < 11) {
        throw std::runtime_error("Invalid employee record format");
    }

    std::string type = parts[0];
    std::string id = parts[1];
    std::string name = parts[2];
    std::string dept = parts[3];
    std::string desig = parts[4];
    double salary = std::stod(parts[5]);
    double bonus = std::stod(parts[6]);
    double allowance = std::stod(parts[7]);

    std::unique_ptr<Employee> emp;

    if (type == "FullTime") {
        double hra = std::stod(parts[8]);
        double da = std::stod(parts[9]);
        double pf = std::stod(parts[10]);
        std::string photo = parts.size() > 11 ? parts[11] : "";
        emp = std::make_unique<FullTimeEmployee>(id, name, dept, desig,
                                                  salary, bonus, allowance, hra, da, pf);
        if (!photo.empty()) emp->setPhotoPath(photo);
    } else if (type == "Contractor") {
        double duration = std::stod(parts[8]);
        std::string photo = parts.size() > 11 ? parts[11] : "";
        emp = std::make_unique<Contractor>(id, name, dept, desig,
                                            salary, bonus, allowance, duration);
        if (!photo.empty()) emp->setPhotoPath(photo);
    } else if (type == "Intern") {
        std::string uni = parts[8];
        int months = std::stoi(parts[9]);
        std::string photo = parts.size() > 11 ? parts[11] : "";
        emp = std::make_unique<Intern>(id, name, dept, desig,
                                        salary, bonus, allowance, uni, months);
        if (!photo.empty()) emp->setPhotoPath(photo);
    } else {
        throw std::runtime_error("Unknown employee type: " + type);
    }

    return emp;
}

void FileManager::saveEmployees(const std::vector<std::unique_ptr<Employee>>& employees) {
    std::ofstream file(employeesFile, std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open employees file for writing: " + employeesFile);
    }

    for (const auto& emp : employees) {
        if (emp) {
            writeLine(file, emp->serialize());
        }
    }
    file.close();
    logAction("Saved " + std::to_string(employees.size()) + " employees to file");
}

std::vector<std::unique_ptr<Employee>> FileManager::loadEmployees() {
    std::vector<std::unique_ptr<Employee>> employees;
    std::ifstream file(employeesFile);

    if (!file.is_open()) {
        return employees;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            auto emp = parseEmployeeLine(line);
            if (emp) employees.push_back(std::move(emp));
        } catch (const std::exception& e) {
            logAction("Error parsing line: " + std::string(e.what()));
        }
    }
    file.close();
    logAction("Loaded " + std::to_string(employees.size()) + " employees from file");
    return employees;
}

void FileManager::savePayrollRecord(const std::string& record) {
    std::ofstream file(payrollFile, std::ios::app);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open payroll file for writing");
    }
    writeLine(file, record);
    file.close();
}

std::vector<std::string> FileManager::readPayrollHistory() const{
    std::vector<std::string> records;
    std::ifstream file(payrollFile);
    if (!file.is_open()) return records;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) records.push_back(line);
    }
    file.close();
    return records;
}

void FileManager::logAction(const std::string& action) {
    std::ofstream file(logsFile, std::ios::app);
    if (!file.is_open()) return;

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf{};
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif

    file << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << " | " << action << std::endl;
    file.close();
}

std::vector<std::string> FileManager::readLogs() {
    std::vector<std::string> logs;
    std::ifstream file(logsFile);
    if (!file.is_open()) return logs;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) logs.push_back(line);
    }
    file.close();
    return logs;
}
