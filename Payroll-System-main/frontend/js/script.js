const API_BASE = '';
const ITEMS_PER_PAGE = 8;
let allEmployees = [];
let currentPage = 1;
let deleteTargetId = null;
let currentEmployeeData = null;
let chartInstances = {};

function checkAuth() {
    const token = localStorage.getItem('authToken');
    if (!token && !window.location.pathname.endsWith('index.html') && window.location.pathname !== '/') {
        window.location.href = 'index.html';
    }
}

function logout() {
    localStorage.removeItem('authToken');
    localStorage.removeItem('userRole');
    window.location.href = 'index.html';
}

function formatCurrency(amount) {
    return '₹' + Number(amount).toLocaleString('en-IN', { minimumFractionDigits: 0, maximumFractionDigits: 0 });
}

function showToast(message, type = 'info') {
    const container = document.getElementById('toastContainer');
    if (!container) return;

    const colors = { success: 'bg-success', danger: 'bg-danger', info: 'bg-primary', warning: 'bg-warning' };
    const toast = document.createElement('div');
    toast.className = `toast show align-items-center text-white ${colors[type] || colors.info} border-0`;
    toast.setAttribute('role', 'alert');
    toast.innerHTML = `
        <div class="d-flex">
            <div class="toast-body">${message}</div>
            <button type="button" class="btn-close btn-close-white me-2 m-auto" data-bs-dismiss="toast"></button>
        </div>`;
    container.appendChild(toast);
    setTimeout(() => toast.remove(), 4000);
}

function initTheme() {
    const theme = localStorage.getItem('theme') || 'light';
    document.documentElement.setAttribute('data-theme', theme);
    updateThemeIcon(theme);
}

function toggleTheme() {
    const current = document.documentElement.getAttribute('data-theme');
    const next = current === 'dark' ? 'light' : 'dark';
    document.documentElement.setAttribute('data-theme', next);
    localStorage.setItem('theme', next);
    updateThemeIcon(next);
}

function updateThemeIcon(theme) {
    document.querySelectorAll('#themeToggle i').forEach(icon => {
        icon.className = theme === 'dark' ? 'bi bi-sun' : 'bi bi-moon-stars';
    });
}

document.addEventListener('DOMContentLoaded', () => {
    initTheme();
    const themeBtn = document.getElementById('themeToggle');
    if (themeBtn) themeBtn.addEventListener('click', toggleTheme);

    const togglePwd = document.getElementById('togglePassword');
    if (togglePwd) {
        togglePwd.addEventListener('click', () => {
            const pwd = document.getElementById('password');
            const icon = togglePwd.querySelector('i');
            if (pwd.type === 'password') {
                pwd.type = 'text';
                icon.className = 'bi bi-eye-slash';
            } else {
                pwd.type = 'password';
                icon.className = 'bi bi-eye';
            }
        });
    }
});

function renderSidebar(activePage) {
    const container = document.getElementById('sidebarContainer');
    if (!container) return;

    const pages = [
        { id: 'dashboard', href: 'dashboard.html', icon: 'bi-speedometer2', label: 'Dashboard' },
        { id: 'employees', href: 'employees.html', icon: 'bi-people', label: 'Employees' },
        { id: 'addEmployee', href: 'addEmployee.html', icon: 'bi-person-plus', label: 'Add Employee' },
        { id: 'payroll', href: 'payroll.html', icon: 'bi-calculator', label: 'Payroll' },
        { id: 'analytics', href: 'analytics.html', icon: 'bi-bar-chart', label: 'Analytics' }
    ];

    container.innerHTML = `
        <aside class="sidebar">
            <div class="sidebar-brand">
                <i class="bi bi-building"></i>
                <h4>PayrollPro</h4>
            </div>
            <nav class="sidebar-nav">
                ${pages.map(p => `
                    <a href="${p.href}" class="${p.id === activePage ? 'active' : ''}">
                        <i class="bi ${p.icon}"></i> ${p.label}
                    </a>
                `).join('')}
            </nav>
            <div class="sidebar-footer">
                <button class="btn-logout" onclick="logout()">
                    <i class="bi bi-box-arrow-left me-1"></i> Logout
                </button>
            </div>
        </aside>`;
}

async function apiFetch(endpoint, options = {}) {
    const res = await fetch(API_BASE + endpoint, {
        headers: { 'Content-Type': 'application/json', ...options.headers },
        ...options
    });
    const data = await res.json().catch(() => ({}));
    if (!res.ok) throw new Error(data.error || `Request failed (${res.status})`);
    return data;
}

async function loadDashboard() {
    try {
        const data = await apiFetch('/api/dashboard');
        document.getElementById('totalEmployees').textContent = data.totalEmployees;
        document.getElementById('totalPayroll').textContent = formatCurrency(data.totalPayroll);
        document.getElementById('averageSalary').textContent = formatCurrency(data.averageSalary);
        document.getElementById('highestSalary').textContent = formatCurrency(data.highestSalary);
        document.getElementById('totalTax').textContent = formatCurrency(data.totalTax);

        const history = await apiFetch('/api/payroll-history');
        const activityEl = document.getElementById('recentActivity');
        if (history.length === 0) {
            activityEl.innerHTML = '<p class="text-muted text-center">No recent payroll activity</p>';
        } else {
            activityEl.innerHTML = history.slice(-5).reverse().map(h =>
                `<div class="d-flex justify-content-between align-items-center py-2 border-bottom">
                    <div>
                        <strong>${h.employeeId}</strong>
                        <small class="d-block text-muted">${h.date}</small>
                    </div>
                    <span class="badge bg-success">${formatCurrency(h.net)}</span>
                </div>`
            ).join('');
        }
    } catch (err) {
        showToast('Failed to load dashboard: ' + err.message, 'danger');
    }
}

function getTypeBadge(type) {
    const map = {
        FullTime: 'badge-fulltime',
        Contractor: 'badge-contractor',
        Intern: 'badge-intern'
    };
    return `<span class="badge-type ${map[type] || ''}">${type}</span>`;
}

async function initEmployeesPage() {
    try {
        allEmployees = await apiFetch('/api/employees');
        renderEmployeesTable();
    } catch (err) {
        showToast('Failed to load employees: ' + err.message, 'danger');
    }

    document.getElementById('searchInput').addEventListener('input', debounce(filterEmployees, 300));
    document.getElementById('filterDepartment').addEventListener('change', filterEmployees);
    document.getElementById('filterType').addEventListener('change', filterEmployees);
    document.getElementById('filterSalary').addEventListener('change', filterEmployees);
    document.getElementById('sortBy').addEventListener('change', filterEmployees);

    document.getElementById('confirmDelete').addEventListener('click', async () => {
        if (!deleteTargetId) return;
        try {
            await apiFetch(`/api/employees/delete/${deleteTargetId}`, { method: 'DELETE' });
            showToast('Employee deleted successfully', 'success');
            bootstrap.Modal.getInstance(document.getElementById('deleteModal')).hide();
            allEmployees = await apiFetch('/api/employees');
            filterEmployees();
        } catch (err) {
            showToast(err.message, 'danger');
        }
    });
}

function debounce(fn, delay) {
    let timer;
    return (...args) => {
        clearTimeout(timer);
        timer = setTimeout(() => fn(...args), delay);
    };
}

function filterEmployees() {
    const search = document.getElementById('searchInput').value.toLowerCase();
    const dept = document.getElementById('filterDepartment').value;
    const type = document.getElementById('filterType').value;
    const salaryRange = document.getElementById('filterSalary').value;
    const sort = document.getElementById('sortBy').value;

    let filtered = allEmployees.filter(emp => {
        const matchSearch = !search || emp.name.toLowerCase().includes(search) || emp.id.toLowerCase().includes(search);
        const matchDept = !dept || emp.department === dept;
        const matchType = !type || emp.type === type;
        let matchSalary = true;
        if (salaryRange) {
            const [min, max] = salaryRange.split('-').map(Number);
            matchSalary = emp.grossSalary >= min && emp.grossSalary <= max;
        }
        return matchSearch && matchDept && matchType && matchSalary;
    });

    filtered.sort((a, b) => {
        switch (sort) {
            case 'salary-desc': return b.grossSalary - a.grossSalary;
            case 'salary-asc': return a.grossSalary - b.grossSalary;
            case 'department': return a.department.localeCompare(b.department);
            default: return a.name.localeCompare(b.name);
        }
    });

    currentPage = 1;
    renderEmployeesTable(filtered);
}

function renderEmployeesTable(data) {
    const employees = data || allEmployees;
    const tbody = document.getElementById('employeesBody');
    const start = (currentPage - 1) * ITEMS_PER_PAGE;
    const pageData = employees.slice(start, start + ITEMS_PER_PAGE);

    if (pageData.length === 0) {
        tbody.innerHTML = '<tr><td colspan="8" class="text-center py-4 text-muted">No employees found</td></tr>';
    } else {
        tbody.innerHTML = pageData.map(emp => `
            <tr>
                <td><code>${emp.id}</code></td>
                <td><strong>${emp.name}</strong></td>
                <td>${emp.department}</td>
                <td>${emp.designation}</td>
                <td>${getTypeBadge(emp.type)}</td>
                <td>${formatCurrency(emp.grossSalary)}</td>
                <td>${formatCurrency(emp.netSalary)}</td>
                <td>
                    <div class="btn-group btn-group-sm">
                        <button class="btn btn-outline-primary" onclick="viewEmployee('${emp.id}')" title="View"><i class="bi bi-eye"></i></button>
                        <a href="addEmployee.html?edit=${emp.id}" class="btn btn-outline-warning" title="Edit"><i class="bi bi-pencil"></i></a>
                        <button class="btn btn-outline-danger" onclick="confirmDelete('${emp.id}', '${emp.name}')" title="Delete"><i class="bi bi-trash"></i></button>
                    </div>
                </td>
            </tr>`).join('');
    }

    document.getElementById('employeeCount').textContent =
        `Showing ${Math.min(start + 1, employees.length)}-${Math.min(start + ITEMS_PER_PAGE, employees.length)} of ${employees.length} employees`;

    renderPagination(employees.length);
}

function renderPagination(total) {
    const pages = Math.ceil(total / ITEMS_PER_PAGE);
    const pagination = document.getElementById('pagination');
    if (pages <= 1) { pagination.innerHTML = ''; return; }

    let html = '';
    for (let i = 1; i <= pages; i++) {
        html += `<li class="page-item ${i === currentPage ? 'active' : ''}">
            <a class="page-link" href="#" onclick="goToPage(${i}); return false;">${i}</a>
        </li>`;
    }
    pagination.innerHTML = html;
}

function goToPage(page) {
    currentPage = page;
    filterEmployees();
}

async function viewEmployee(id) {
    try {
        const emp = await apiFetch(`/api/employees/${id}`);
        document.getElementById('viewModalBody').innerHTML = `
            <div class="row g-3">
                <div class="col-md-6"><strong>ID:</strong> ${emp.id}</div>
                <div class="col-md-6"><strong>Name:</strong> ${emp.name}</div>
                <div class="col-md-6"><strong>Department:</strong> ${emp.department}</div>
                <div class="col-md-6"><strong>Designation:</strong> ${emp.designation}</div>
                <div class="col-md-6"><strong>Type:</strong> ${getTypeBadge(emp.type)}</div>
                <div class="col-md-6"><strong>Base Salary:</strong> ${formatCurrency(emp.baseSalary)}</div>
                <div class="col-md-6"><strong>Bonus:</strong> ${formatCurrency(emp.bonus)}</div>
                <div class="col-md-6"><strong>Allowance:</strong> ${formatCurrency(emp.allowance)}</div>
                <div class="col-md-6"><strong>Gross Salary:</strong> ${formatCurrency(emp.grossSalary)}</div>
                <div class="col-md-6"><strong>Tax:</strong> ${formatCurrency(emp.tax)}</div>
                <div class="col-md-6"><strong>PF:</strong> ${formatCurrency(emp.pf)}</div>
                <div class="col-md-6"><strong>Net Salary:</strong> <span class="text-primary fw-bold">${formatCurrency(emp.netSalary)}</span></div>
            </div>`;
        new bootstrap.Modal(document.getElementById('viewModal')).show();
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function confirmDelete(id, name) {
    deleteTargetId = id;
    document.getElementById('deleteEmpName').textContent = name;
    new bootstrap.Modal(document.getElementById('deleteModal')).show();
}

function initAddEmployeePage() {
    const typeSelect = document.getElementById('empType');
    typeSelect.addEventListener('change', toggleTypeFields);

    document.getElementById('empSalary').addEventListener('input', autoCalcHRA);

    document.getElementById('employeeForm').addEventListener('submit', saveEmployee);

    const params = new URLSearchParams(window.location.search);
    const editId = params.get('edit');
    if (editId) loadEmployeeForEdit(editId);
}

function toggleTypeFields() {
    const type = document.getElementById('empType').value;
    document.getElementById('fullTimeFields').classList.toggle('d-none', type !== 'FullTime');
    document.getElementById('contractorFields').classList.toggle('d-none', type !== 'Contractor');
    document.getElementById('internFields').classList.toggle('d-none', type !== 'Intern');
}

function autoCalcHRA() {
    const salary = parseFloat(document.getElementById('empSalary').value) || 0;
    if (document.getElementById('empType').value === 'FullTime') {
        document.getElementById('empHRA').value = Math.round(salary * 0.40);
        document.getElementById('empDA').value = Math.round(salary * 0.20);
    }
}

async function loadEmployeeForEdit(id) {
    try {
        const emp = await apiFetch(`/api/employees/${id}`);
        document.getElementById('formTitle').textContent = 'Edit Employee';
        document.getElementById('editMode').value = 'true';
        document.getElementById('originalId').value = id;
        document.getElementById('empId').value = emp.id;
        document.getElementById('empId').readOnly = true;
        document.getElementById('empName').value = emp.name;
        document.getElementById('empDepartment').value = emp.department;
        document.getElementById('empDesignation').value = emp.designation;
        document.getElementById('empType').value = emp.type;
        document.getElementById('empSalary').value = emp.baseSalary;
        document.getElementById('empBonus').value = emp.bonus;
        document.getElementById('empAllowance').value = emp.allowance;
        toggleTypeFields();

        if (emp.type === 'FullTime') {
            document.getElementById('empHRA').value = emp.hra || 0;
            document.getElementById('empDA').value = emp.da || 0;
            document.getElementById('empPF').value = emp.pfAmount || 0;
        } else if (emp.type === 'Contractor') {
            document.getElementById('empContractDuration').value = emp.contractDuration || 12;
        } else if (emp.type === 'Intern') {
            document.getElementById('empUniversity').value = emp.university || '';
            document.getElementById('empInternMonths').value = emp.internshipMonths || 6;
        }
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function saveEmployee(e) {
    e.preventDefault();

    const body = {
        id: document.getElementById('empId').value.trim(),
        name: document.getElementById('empName').value.trim(),
        department: document.getElementById('empDepartment').value,
        designation: document.getElementById('empDesignation').value.trim(),
        type: document.getElementById('empType').value,
        baseSalary: parseFloat(document.getElementById('empSalary').value),
        bonus: parseFloat(document.getElementById('empBonus').value) || 0,
        allowance: parseFloat(document.getElementById('empAllowance').value) || 0
    };

    if (!body.id || !body.name || !body.department || !body.designation || !body.type) {
        showToast('Please fill all required fields', 'warning');
        return;
    }

    if (body.baseSalary < 0) {
        showToast('Salary cannot be negative', 'warning');
        return;
    }

    if (body.type === 'FullTime') {
        body.hra = parseFloat(document.getElementById('empHRA').value) || 0;
        body.da = parseFloat(document.getElementById('empDA').value) || 0;
        body.pf = parseFloat(document.getElementById('empPF').value) || 0;
    } else if (body.type === 'Contractor') {
        body.contractDuration = parseFloat(document.getElementById('empContractDuration').value) || 12;
    } else if (body.type === 'Intern') {
        body.university = document.getElementById('empUniversity').value;
        body.internshipMonths = parseInt(document.getElementById('empInternMonths').value) || 6;
    }

    const isEdit = document.getElementById('editMode').value === 'true';
    const originalId = document.getElementById('originalId').value;

    try {
        if (isEdit) {
            await apiFetch(`/api/employees/update/${originalId}`, { method: 'PUT', body: JSON.stringify(body) });
            showToast('Employee updated successfully', 'success');
        } else {
            await apiFetch('/api/employees', { method: 'POST', body: JSON.stringify(body) });
            showToast('Employee added successfully', 'success');
        }

        const photoInput = document.getElementById('empPhoto');
        if (photoInput.files.length > 0) {
            const formData = await photoInput.files[0].arrayBuffer();
            await fetch(`/api/upload-photo/${body.id}`, { method: 'POST', body: formData });
        }

        setTimeout(() => window.location.href = 'employees.html', 1000);
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function initPayrollPage() {
    try {
        const employees = await apiFetch('/api/employees');
        const select = document.getElementById('employeeSelect');
        select.innerHTML = '<option value="">Choose an employee...</option>' +
            employees.map(e => `<option value="${e.id}">${e.name} (${e.id}) - ${e.type}</option>`).join('');

        select.addEventListener('change', () => {
            document.getElementById('calculateBtn').disabled = !select.value;
        });

        document.getElementById('calculateBtn').addEventListener('click', calculateSalary);
        document.getElementById('generatePayslipBtn').addEventListener('click', generatePayslip);
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function calculateSalary() {
    const id = document.getElementById('employeeSelect').value;
    if (!id) return;

    try {
        const emp = await apiFetch(`/api/payroll/${id}`);
        currentEmployeeData = emp;

        document.getElementById('noSelection').style.display = 'none';
        document.getElementById('salaryBreakdown').style.display = 'block';

        document.getElementById('breakdownContent').innerHTML = `
            <div class="col-md-4"><div class="breakdown-item earnings"><div class="label">Base Salary</div><div class="value">${formatCurrency(emp.baseSalary)}</div></div></div>
            <div class="col-md-4"><div class="breakdown-item earnings"><div class="label">Bonus</div><div class="value">${formatCurrency(emp.bonus)}</div></div></div>
            <div class="col-md-4"><div class="breakdown-item earnings"><div class="label">Allowance</div><div class="value">${formatCurrency(emp.allowance)}</div></div></div>
            <div class="col-md-4"><div class="breakdown-item earnings"><div class="label">Gross Salary</div><div class="value">${formatCurrency(emp.grossSalary)}</div></div></div>
            <div class="col-md-4"><div class="breakdown-item deductions"><div class="label">Tax</div><div class="value">${formatCurrency(emp.tax)}</div></div></div>
            <div class="col-md-4"><div class="breakdown-item deductions"><div class="label">PF</div><div class="value">${formatCurrency(emp.pf)}</div></div></div>
            <div class="col-12"><div class="breakdown-item net"><div class="label">Net Salary</div><div class="value">${formatCurrency(emp.netSalary)}</div></div></div>`;
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

async function generatePayslip() {
    const id = document.getElementById('employeeSelect').value;
    if (!id) return;

    try {
        const data = await apiFetch(`/api/payslip/${id}`);
        const emp = data.employee;
        const b = data.breakdown;

        document.getElementById('payslipContent').innerHTML = `
            <div class="payslip" id="payslipPrint">
                <div class="payslip-header">
                    <h3>${data.companyName}</h3>
                    <p class="text-muted mb-0">Salary Payslip - ${data.month}</p>
                    <small>Generated: ${data.generatedDate}</small>
                </div>
                <div class="row mb-3">
                    <div class="col-6"><strong>Employee:</strong> ${emp.name}<br><strong>ID:</strong> ${emp.id}</div>
                    <div class="col-6 text-end"><strong>Department:</strong> ${emp.department}<br><strong>Designation:</strong> ${emp.designation}</div>
                </div>
                <div class="payslip-row"><span>Base Salary</span><span>${formatCurrency(b.baseSalary)}</span></div>
                <div class="payslip-row"><span>Bonus</span><span>${formatCurrency(b.bonus)}</span></div>
                <div class="payslip-row"><span>Allowance</span><span>${formatCurrency(b.allowance)}</span></div>
                <div class="payslip-row"><span>Gross Salary</span><span>${formatCurrency(b.grossSalary)}</span></div>
                <div class="payslip-row"><span>Tax Deduction</span><span>- ${formatCurrency(b.tax)}</span></div>
                <div class="payslip-row"><span>PF Deduction</span><span>- ${formatCurrency(b.pf)}</span></div>
                <div class="payslip-row total"><span>Net Salary</span><span>${formatCurrency(b.netSalary)}</span></div>
            </div>`;

        new bootstrap.Modal(document.getElementById('payslipModal')).show();
        showToast('Payslip generated and saved', 'success');
    } catch (err) {
        showToast(err.message, 'danger');
    }
}

function printPayslip() {
    const content = document.getElementById('payslipPrint').innerHTML;
    const win = window.open('', '_blank');
    win.document.write(`<html><head><title>Payslip</title>
        <style>body{font-family:Arial,sans-serif;padding:2rem}
        .payslip-header{text-align:center;border-bottom:2px solid #4f46e5;padding-bottom:1rem;margin-bottom:1.5rem}
        .payslip-row{display:flex;justify-content:space-between;padding:0.5rem 0;border-bottom:1px dashed #ccc}
        .total{font-weight:bold;font-size:1.1rem;border-top:2px solid #4f46e5;margin-top:0.5rem;padding-top:0.75rem}</style>
        </head><body>${content}</body></html>`);
    win.document.close();
    win.print();
}

function downloadPayslipPDF() {
    const element = document.getElementById('payslipPrint');
    if (!element || typeof html2pdf === 'undefined') {
        showToast('PDF library not loaded', 'warning');
        return;
    }
    html2pdf().set({
        margin: 10,
        filename: 'payslip.pdf',
        html2canvas: { scale: 2 },
        jsPDF: { unit: 'mm', format: 'a4' }
    }).from(element).save();
    showToast('PDF download started', 'success');
}

async function initAnalyticsPage() {
    try {
        const data = await apiFetch('/api/analytics');
        const history = await apiFetch('/api/payroll-history');

        document.getElementById('anEmpCount').textContent = data.employeeCount;
        document.getElementById('anTotalPayroll').textContent = formatCurrency(data.totalPayroll);
        document.getElementById('anAvgSalary').textContent = formatCurrency(data.averageSalary);
        document.getElementById('anTotalTax').textContent = formatCurrency(data.totalTax);

        const isDark = document.documentElement.getAttribute('data-theme') === 'dark';
        const textColor = isDark ? '#f1f5f9' : '#1e293b';
        const gridColor = isDark ? 'rgba(255,255,255,0.1)' : 'rgba(0,0,0,0.1)';

        Object.values(chartInstances).forEach(c => c.destroy());
        chartInstances = {};

        const deptLabels = Object.keys(data.departmentSalary);
        const deptValues = Object.values(data.departmentSalary);

        chartInstances.deptBar = new Chart(document.getElementById('deptBarChart'), {
            type: 'bar',
            data: {
                labels: deptLabels,
                datasets: [{
                    label: 'Total Salary',
                    data: deptValues,
                    backgroundColor: ['#4f46e5', '#7c3aed', '#2563eb', '#0891b2', '#059669'],
                    borderRadius: 8
                }]
            },
            options: {
                responsive: true,
                plugins: { legend: { display: false } },
                scales: {
                    y: { ticks: { color: textColor, callback: v => '₹' + v.toLocaleString() }, grid: { color: gridColor } },
                    x: { ticks: { color: textColor }, grid: { display: false } }
                }
            }
        });

        const typeLabels = Object.keys(data.typeDistribution);
        const typeValues = Object.values(data.typeDistribution);

        chartInstances.typePie = new Chart(document.getElementById('typePieChart'), {
            type: 'pie',
            data: {
                labels: typeLabels,
                datasets: [{
                    data: typeValues,
                    backgroundColor: ['#4f46e5', '#f59e0b', '#22c55e']
                }]
            },
            options: {
                responsive: true,
                plugins: { legend: { labels: { color: textColor } } }
            }
        });

        const topNames = data.topPaid.map(t => t.name);
        const topSalaries = data.topPaid.map(t => t.salary);

        chartInstances.topPaid = new Chart(document.getElementById('topPaidChart'), {
            type: 'bar',
            data: {
                labels: topNames,
                datasets: [{
                    label: 'Gross Salary',
                    data: topSalaries,
                    backgroundColor: '#4f46e5',
                    borderRadius: 8
                }]
            },
            options: {
                indexAxis: 'y',
                responsive: true,
                plugins: { legend: { display: false } },
                scales: {
                    x: { ticks: { color: textColor, callback: v => '₹' + v.toLocaleString() }, grid: { color: gridColor } },
                    y: { ticks: { color: textColor }, grid: { display: false } }
                }
            }
        });

        const trendLabels = history.map(h => h.date);
        const trendGross = history.map(h => h.gross);
        const trendTax = history.map(h => h.tax);

        chartInstances.trend = new Chart(document.getElementById('trendLineChart'), {
            type: 'line',
            data: {
                labels: trendLabels.length ? trendLabels : ['No Data'],
                datasets: [
                    { label: 'Gross', data: trendGross.length ? trendGross : [0], borderColor: '#4f46e5', tension: 0.3, fill: false },
                    { label: 'Tax', data: trendTax.length ? trendTax : [0], borderColor: '#ef4444', tension: 0.3, fill: false }
                ]
            },
            options: {
                responsive: true,
                plugins: { legend: { labels: { color: textColor } } },
                scales: {
                    y: { ticks: { color: textColor }, grid: { color: gridColor } },
                    x: { ticks: { color: textColor }, grid: { color: gridColor } }
                }
            }
        });

        document.getElementById('exportAnalytics').addEventListener('click', () => {
            const blob = new Blob([JSON.stringify(data, null, 2)], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = 'payroll-analytics.json';
            a.click();
            showToast('Analytics exported', 'success');
        });
    } catch (err) {
        showToast('Failed to load analytics: ' + err.message, 'danger');
    }
}
