Payroll & Tax Management System

A professional web-based payroll and tax management application built with C++17 (OOP) backend and a modern HTML/CSS/JavaScript frontend. Designed for enterprise HR workflows with file-based persistence and real-time analytics.

Tech Stack Crow Bootstrap

Features





Employee CRUD — Add, edit, delete, search, and filter employees



Multi-type employees — Full-Time, Contractor, Intern with distinct tax rules



Payroll calculator — Automatic gross, tax, PF, and net salary computation



Payslip generation — Printable and downloadable PDF payslips



Analytics dashboard — Chart.js visualizations (bar, pie, line charts)



File persistence — Auto-save to employees.txt, payroll.txt, logs.txt



Dark mode — Toggle between light and dark themes



Responsive UI — Bootstrap 5 with modern enterprise design



OOP Concepts Demonstrated







Concept



Implementation





Encapsulation



Private members with getters/setters in all classes





Abstraction



Employee abstract base class





Inheritance



FullTimeEmployee, Contractor, Intern extend Employee





Polymorphism



Runtime type-specific tax/salary calculations





Virtual Functions



calculateTax(), calculateGrossSalary(), display()





Pure Virtual Functions



calculateTax() = 0, getEmployeeType() = 0





Dynamic Binding



unique_ptr<Employee> with virtual dispatch





Constructor Overloading



Default and parameterized constructors





Destructor



Virtual destructor in Employee base class





Friend Function



operator<< for stream output





Static Members



Employee::totalEmployees counter





Composition



PayrollSystem owns FileManager





Aggregation



Analytics references employee collection





Exception Handling



try/catch with custom error messages





Templates



FileManager::writeLine<T>, Analytics::findExtremeSalary



Project Structure

Payroll-System/
├── frontend/
│   ├── index.html          # Login page
│   ├── dashboard.html      # Dashboard with stats
│   ├── employees.html      # Employee management
│   ├── addEmployee.html    # Add/Edit employee form
│   ├── payroll.html        # Payroll calculator & payslip
│   ├── analytics.html      # Analytics with Chart.js
│   ├── css/style.css       # Custom styles + dark mode
│   └── js/script.js        # Frontend logic
├── backend/
│   ├── Employee.h/.cpp     # Abstract base class
│   ├── FullTimeEmployee.h/.cpp
│   ├── Contractor.h/.cpp
│   ├── Intern.h/.cpp
│   ├── PayrollSystem.h/.cpp
│   ├── FileManager.h/.cpp
│   ├── Analytics.h/.cpp
│   └── Server.cpp          # Crow HTTP server
├── data/
│   ├── employees.txt       # Employee records
│   ├── payroll.txt         # Payroll history
│   └── logs.txt            # System logs
├── CMakeLists.txt
└── README.md



Tax Calculation Rules







Employee Type



Tax Rate





Full-Time



Progressive slabs: 5% (≤3L), 10% (3L–6L), 20% (6L–10L), 30% (>10L)





Contractor



Flat 10% on gross salary





Intern



Flat 5% on gross salary

Net Salary = Gross Salary − Tax − PF (12% of base for full-time)

Prerequisites





C++17 compiler — MSVC 2019+, GCC 8+, or Clang 7+



CMake 3.14+



Git (for fetching Crow and Asio dependencies)



Windows

Install Visual Studio 2022 with "Desktop development with C++" workload, plus CMake.

Linux

sudo apt update
sudo apt install build-essential cmake git



macOS

xcode-select --install
brew install cmake



Build & Run

# Clone or navigate to project
cd Payroll-System

# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run (from project root — server needs access to frontend/ and data/)
cd build
./payroll_server        # Linux/macOS
payroll_server.exe      # Windows

Open http://localhost:8080 in your browser.

Login Credentials







Username



Password





admin



admin123



API Endpoints







Method



Endpoint



Description





POST



/api/login



Admin authentication





GET



/api/dashboard



Dashboard statistics





GET



/api/employees



List all employees





GET



/api/employees/:id



Get employee by ID





POST



/api/employees



Add new employee





PUT



/api/employees/:id



Update employee





DELETE



/api/employees/:id



Delete employee





GET



/api/payroll/:id



Calculate salary





GET



/api/payslip/:id



Generate payslip





GET



/api/analytics



Analytics data





GET



/api/payroll-history



Payroll history



Sample Data

The project ships with 8 sample employees across Engineering, HR, Finance, Marketing, and Operations departments.

License

MIT License — Free for academic and portfolio use.



Built with ❤️ for Object-Oriented Programming coursework.