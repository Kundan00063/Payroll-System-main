#include "crow.h"
#include "PayrollSystem.h"
#include "FullTimeEmployee.h"
#include "Contractor.h"
#include "Intern.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

static PayrollSystem payrollSystem;

crow::response jsonResponse(int code, const std::string& body) {
    crow::response res(code, body);
    res.add_header("Content-Type", "application/json");
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.add_header("Access-Control-Allow-Headers", "Content-Type");
    return res;
}

crow::response errorResponse(int code, const std::string& message) {
    return jsonResponse(code, "{\"error\":\"" + message + "\"}");
}

std::string readFileContent(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".svg") != std::string::npos) return "image/svg+xml";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain";
}

std::unique_ptr<Employee> createEmployeeFromJson(const crow::json::rvalue& body) {
    std::string type = body["type"].s();
    std::string id = body["id"].s();
    std::string name = body["name"].s();
    std::string dept = body["department"].s();
    std::string desig = body["designation"].s();
    double salary = body["baseSalary"].d();
    double bonus = body.has("bonus") ? body["bonus"].d() : 0.0;
    double allowance = body.has("allowance") ? body["allowance"].d() : 0.0;

    if (type == "FullTime") {
        double hra = body.has("hra") ? body["hra"].d() : salary * 0.40;
        double da = body.has("da") ? body["da"].d() : salary * 0.20;
        double pf = body.has("pf") ? body["pf"].d() : 0.0;
        return std::make_unique<FullTimeEmployee>(id, name, dept, desig,
                                                   salary, bonus, allowance, hra, da, pf);
    } else if (type == "Contractor") {
        double duration = body.has("contractDuration") ? body["contractDuration"].d() : 12.0;
        return std::make_unique<Contractor>(id, name, dept, desig,
                                             salary, bonus, allowance, duration);
    } else if (type == "Intern") {
        std::string uni = "N/A";

if (body.has("university"))
{
    uni = std::string(body["university"].s());
}
        int months = body.has("internshipMonths") ? static_cast<int>(body["internshipMonths"].d()) : 6;
        return std::make_unique<Intern>(id, name, dept, desig,
                                         salary, bonus, allowance, uni, months);
    }
    throw std::runtime_error("Invalid employee type: " + type);
}

int main() {
    crow::SimpleApp app;

    CROW_ROUTE(app, "/api/login").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req) {
        if (req.method == "OPTIONS"_method) {
            return jsonResponse(200, "{}");
        }
        try {
            auto body = crow::json::load(req.body);
            std::string username = body["username"].s();
            std::string password = body["password"].s();

            if (username == "admin" && password == "admin123") {
                return jsonResponse(200, "{\"success\":true,\"token\":\"payroll-admin-token\",\"role\":\"admin\"}");
            }
            return jsonResponse(401, "{\"success\":false,\"error\":\"Invalid credentials\"}");
        } catch (const std::exception& e) {
            return errorResponse(400, e.what());
        }
    });

    CROW_ROUTE(app, "/api/dashboard").methods("GET"_method)
    ([]() {
        try {
            return jsonResponse(200, payrollSystem.getDashboardJson());
        } catch (const std::exception& e) {
            return errorResponse(500, e.what());
        }
    });

    CROW_ROUTE(app, "/api/employees").methods("GET"_method)
    ([](const crow::request& req) {
        try {
            auto params = req.url_params;
            if (params.get("name")) {
                auto results = payrollSystem.searchByName(params.get("name"));
                std::ostringstream json;
                json << "[";
                for (size_t i = 0; i < results.size(); ++i) {
                    if (i > 0) json << ",";
                    Employee* e = results[i];
                    json << "{\"id\":\"" << e->getEmployeeId() << "\",";
                    json << "\"name\":\"" << e->getEmployeeName() << "\",";
                    json << "\"department\":\"" << e->getDepartment() << "\",";
                    json << "\"designation\":\"" << e->getDesignation() << "\",";
                    json << "\"type\":\"" << e->getEmployeeType() << "\",";
                    json << "\"baseSalary\":" << e->getBaseSalary() << ",";
                    json << "\"bonus\":" << e->getBonus() << ",";
                    json << "\"allowance\":" << e->getAllowance() << ",";
                    json << "\"grossSalary\":" << e->calculateGrossSalary() << ",";
                    json << "\"tax\":" << e->calculateTax() << ",";
                    json << "\"pf\":" << e->calculatePF() << ",";
                    json << "\"netSalary\":" << e->calculateNetSalary() << "}";
                }
                json << "]";
                return jsonResponse(200, json.str());
            }
            if (params.get("department")) {
                auto results = payrollSystem.filterByDepartment(params.get("department"));
                std::ostringstream json;
                json << "[";
                for (size_t i = 0; i < results.size(); ++i) {
                    if (i > 0) json << ",";
                    Employee* e = results[i];
                    json << "{\"id\":\"" << e->getEmployeeId() << "\",";
                    json << "\"name\":\"" << e->getEmployeeName() << "\",";
                    json << "\"department\":\"" << e->getDepartment() << "\",";
                    json << "\"type\":\"" << e->getEmployeeType() << "\",";
                    json << "\"grossSalary\":" << e->calculateGrossSalary() << "}";
                }
                json << "]";
                return jsonResponse(200, json.str());
            }
            if (params.get("type")) {
                auto results = payrollSystem.filterByType(params.get("type"));
                std::ostringstream json;
                json << "[";
                for (size_t i = 0; i < results.size(); ++i) {
                    if (i > 0) json << ",";
                    Employee* e = results[i];
                    json << "{\"id\":\"" << e->getEmployeeId() << "\",";
                    json << "\"name\":\"" << e->getEmployeeName() << "\",";
                    json << "\"type\":\"" << e->getEmployeeType() << "\",";
                    json << "\"grossSalary\":" << e->calculateGrossSalary() << "}";
                }
                json << "]";
                return jsonResponse(200, json.str());
            }
            return jsonResponse(200, payrollSystem.getAllEmployeesJson());
        } catch (const std::exception& e) {
            return errorResponse(500, e.what());
        }
    });

//     CROW_ROUTE(app, "/api/employees/<string>")
// .methods("GET"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
// ([&](const crow::request& req, const std::string& id)
// {
//     if (req.method == "OPTIONS"_method)
//         return jsonResponse(200, "{}");

//     try
//     {
//         if (req.method == "GET"_method)
//         {
//             return jsonResponse(200, payrollSystem.getEmployeeJson(id));
//         }

//         if (req.method == "PUT"_method)
//         {
//             auto body = crow::json::load(req.body);

//             if (!body)
//                 return errorResponse(400, "Invalid JSON");

//             auto emp = createEmployeeFromJson(body);

//             if (!payrollSystem.updateEmployee(id, std::move(emp)))
//                 return errorResponse(404, "Employee not found");

//             return jsonResponse(200,
//                 "{\"success\":true,\"message\":\"Employee updated successfully\"}");
//         }

//         if (req.method == "DELETE"_method)
//         {
//             if (!payrollSystem.removeEmployee(id))
//                 return errorResponse(404, "Employee not found");

//             return jsonResponse(200,
//                 "{\"success\":true,\"message\":\"Employee deleted successfully\"}");
//         }

//         return errorResponse(405, "Method Not Allowed");
//     }
//     catch(const std::exception& e)
//     {
//         return errorResponse(500, e.what());
//     }
// });
//------------------------
CROW_ROUTE(app, "/api/employees/<string>")
.methods("GET"_method)
([&](const std::string& id)
{
    try
    {
        return jsonResponse(200, payrollSystem.getEmployeeJson(id));
    }
    catch(const std::exception& e)
    {
        return errorResponse(404, e.what());
    }
});

CROW_ROUTE(app, "/api/employees/update/<string>")
.methods("PUT"_method, "OPTIONS"_method)
([&](const crow::request& req, const std::string& id)
{
    if(req.method == "OPTIONS"_method)
        return jsonResponse(200, "{}");

    try
    {
        auto body = crow::json::load(req.body);

        if(!body)
            return errorResponse(400, "Invalid JSON");

        auto emp = createEmployeeFromJson(body);

        if(!payrollSystem.updateEmployee(id, std::move(emp)))
            return errorResponse(404, "Employee not found");

        return jsonResponse(
            200,
            "{\"success\":true,\"message\":\"Employee updated successfully\"}"
        );
    }
    catch(const std::exception& e)
    {
        return errorResponse(500, e.what());
    }
});

CROW_ROUTE(app, "/api/employees/delete/<string>")
.methods("DELETE"_method, "OPTIONS"_method)
([&](const crow::request& req, const std::string& id)
{
    if(req.method == "OPTIONS"_method)
        return jsonResponse(200, "{}");

    try
    {
        if(!payrollSystem.removeEmployee(id))
            return errorResponse(404, "Employee not found");

        return jsonResponse(
            200,
            "{\"success\":true,\"message\":\"Employee deleted successfully\"}"
        );
    }
    catch(const std::exception& e)
    {
        return errorResponse(500, e.what());
    }
});
CROW_ROUTE(app, "/api/employees")
.methods("POST"_method, "OPTIONS"_method)
([&](const crow::request& req)
{
    if (req.method == "OPTIONS"_method)
        return jsonResponse(200, "{}");

    try
    {
        auto body = crow::json::load(req.body);

        if (!body)
            return errorResponse(400, "Invalid JSON");

        auto emp = createEmployeeFromJson(body);

        payrollSystem.addEmployee(std::move(emp));

        return jsonResponse(
            201,
            "{\"success\":true,\"message\":\"Employee added successfully\"}"
        );
    }
    catch (const std::exception& e)
    {
        return errorResponse(500, e.what());
    }
});
//-------------------------

    CROW_ROUTE(app, "/api/payroll/<string>").methods("GET"_method)
    ([](const std::string& id) {
        try {
            return jsonResponse(200, payrollSystem.calculateSalary(id));
        } catch (const std::exception& e) {
            return errorResponse(404, e.what());
        }
    });

    CROW_ROUTE(app, "/api/payslip/<string>").methods("GET"_method)
    ([](const std::string& id) {
        try {
            return jsonResponse(200, payrollSystem.generatePayslip(id));
        } catch (const std::exception& e) {
            return errorResponse(404, e.what());
        }
    });

    CROW_ROUTE(app, "/api/analytics").methods("GET"_method)
    ([]() {
        try {
            return jsonResponse(200, payrollSystem.getAnalyticsJson());
        } catch (const std::exception& e) {
            return errorResponse(500, e.what());
        }
    });

    CROW_ROUTE(app, "/api/payroll-history").methods("GET"_method)
    ([]() {
        try {
            return jsonResponse(200, payrollSystem.getPayrollHistoryJson());
        } catch (const std::exception& e) {
            return errorResponse(500, e.what());
        }
    });

    CROW_ROUTE(app, "/api/upload-photo/<string>").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req, const std::string& id) {
        if (req.method == "OPTIONS"_method) return jsonResponse(200, "{}");
        try {
            Employee* emp = payrollSystem.searchById(id);
            if (!emp) return errorResponse(404, "Employee not found");

            std::string filename = "data/photos/" + id + ".jpg";
            std::filesystem::create_directories("data/photos");
            std::ofstream file(filename, std::ios::binary);
            file.write(req.body.c_str(), req.body.size());
            file.close();

            emp->setPhotoPath(filename);
            return jsonResponse(200, "{\"success\":true,\"path\":\"" + filename + "\"}");
        } catch (const std::exception& e) {
            return errorResponse(500, e.what());
        }
    });
    CROW_ROUTE(app, "/")
([]()
{
    std::string content = readFileContent("frontend/index.html");

    if(content.empty())
        return crow::response(404, "index.html not found");

    crow::response res(200, content);
    res.add_header("Content-Type", "text/html");
    return res;
});

    CROW_ROUTE(app, "/<path>").methods("GET"_method)
    ([](const std::string& path) {
        std::string filePath = "frontend/" + path;
        if (path.empty() || path == "/") {
            filePath = "frontend/index.html";
        }

        if (!std::filesystem::exists(filePath)) {
            if (std::filesystem::exists("frontend/" + path + ".html")) {
                filePath = "frontend/" + path + ".html";
            } else {
                return crow::response(404, "Not Found");
            }
        }

        std::string content = readFileContent(filePath);
        crow::response res(200, content);
        res.add_header("Content-Type", getMimeType(filePath));
        return res;
    });

    CROW_ROUTE(app, "/frontend/<path>").methods("GET"_method)
    ([](const std::string& path) {
        std::string filePath = "frontend/" + path;
        if (!std::filesystem::exists(filePath)) {
            return crow::response(404, "Not Found");
        }
        std::string content = readFileContent(filePath);
        crow::response res(200, content);
        res.add_header("Content-Type", getMimeType(filePath));
        return res;
    });

    std::cout << "============================================" << std::endl;
    std::cout << "  Payroll & Tax Management System" << std::endl;
    std::cout << "  Server running at http://localhost:8080" << std::endl;
    std::cout << "  Login: admin / admin123" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Route registration completed!" << std::endl;
    app.port(8080).multithreaded().run();
    return 0;
}
