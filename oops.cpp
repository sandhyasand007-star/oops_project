#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

struct Transaction {
    string id, date, timeStr, type, category, desc;
    double amount;
};

class FinanceTracker {
private:
    vector<Transaction> history;
    double monthlyBudget = 0;
    const string DATA_PATH = "data.txt";
    const string BUDGET_PATH = "budget.txt";

public:
    void loadData() {
        ifstream bFile(BUDGET_PATH);
        if (!(bFile >> monthlyBudget)) monthlyBudget = 0;
        bFile.close();

        history.clear();
        ifstream hFile(DATA_PATH);
        Transaction t;
        vector<Transaction> tempHistory;
        
        while (hFile >> t.id >> t.date >> t.timeStr >> t.type >> t.category >> t.desc >> t.amount) {
            tempHistory.push_back(t);
        }
        hFile.close();

        // --- AUTOMATIC MONTHLY RESET LOGIC ---
        time_t now = time(0);
        tm *ltm = localtime(&now);
        int currentMonth = ltm->tm_mon + 1;

        if (!tempHistory.empty()) {
            string lastDate = tempHistory.back().date;
            if (lastDate.length() >= 7) {
                int lastMonth = stoi(lastDate.substr(5, 2));
                if (currentMonth != lastMonth) {
                    ofstream clearFile(DATA_PATH, ios::trunc);
                    clearFile.close();
                    return;
                }
            }
        }
        history = tempHistory;
    }

    void saveData(string ty, string ca, string de, string dt, double am) {
        time_t now = time(0);
        string id = to_string(now);
        tm *ltm = localtime(&now);
        char timeBuf[10];
        strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", ltm);
        replace(de.begin(), de.end(), ' ', '_');
        if(de.empty()) de = "None";
        ofstream hFile(DATA_PATH, ios::app);
        hFile << id << " " << dt << " " << timeBuf << " " << ty << " " << ca << " " << de << " " << am << endl;
        hFile.close();
    }

    void deleteEntry(string delId) {
        loadData();
        ofstream hFile(DATA_PATH, ios::trunc);
        for (const auto& t : history) {
            if (t.id != delId) {
                hFile << t.id << " " << t.date << " " << t.timeStr << " " << t.type << " " << t.category << " " << t.desc << " " << t.amount << endl;
            }
        }
        hFile.close();
    }

    void saveBudget(double b) {
        ofstream bFile(BUDGET_PATH);
        bFile << b;
        bFile.close();
    }

    void displayDashboard() {
        double tInc = 0, tExp = 0, tSav = 0;
        double food = 0, transport = 0, bills = 0, salary = 0, savingsCat = 0;

        for (auto &tr : history) {
            if (tr.type == "Income") tInc += tr.amount;
            else if (tr.type == "Saving") tSav += tr.amount;
            else tExp += tr.amount;

            if (tr.category == "Food") food += tr.amount;
            else if (tr.category == "Transport") transport += tr.amount;
            else if (tr.category == "Bills") bills += tr.amount;
            else if (tr.category == "Salary") salary += tr.amount;
            else if (tr.category == "Saving") savingsCat += tr.amount;
        }

        cout << "Content-type:text/html\r\n\r\n";
        cout << "<html><head><title>Expense & Income Tracker</title>"
             << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
             << "<script src='https://cdn.jsdelivr.net/npm/chartjs-plugin-datalabels@2'></script>"
             << "<style>"
             << "body{font-family:'Segoe UI',Arial; margin:0; padding:20px; "
             << "background: linear-gradient(rgba(0, 0, 0, 0.5), rgba(0, 0, 0, 0.5)), url('https://images.unsplash.com/photo-1506744038136-46273834b3fb?ixlib=rb-4.0.1&auto=format&fit=crop&w=1600&q=80'); background-size: cover; background-attachment: fixed;}"
             << ".dashboard{max-width:1100px; margin:auto; background:rgba(255, 255, 255, 0.95); padding:30px; border-radius:20px; box-shadow:0 10px 30px rgba(0,0,0,0.5);}"
             << ".stat-grid{display:grid; grid-template-columns: repeat(4, 1fr); gap:15px; margin-bottom:30px;}"
             << ".stat-card{background:#fff; padding:20px; border-radius:15px; border:1px solid #eee; text-align:center;}"
             << ".budget-info{background:#2c3e50; color:white; padding:20px; border-radius:12px; margin-bottom:25px; display:flex; justify-content:space-between; align-items:center;}"
             << "table{width:100%; border-collapse:collapse; margin-top:20px;}"
             << "th{background:#f8f9fa; color:#666; padding:15px; text-align:left; border-bottom:2px solid #eee;}"
             << "td{padding:15px; border-bottom:1px solid #eee; font-size:14px;}"
             << ".del-btn{color:#dc3545; text-decoration:none; font-weight:bold; border:1px solid #dc3545; padding:4px 8px; border-radius:5px;}"
             << ".chart-container{width:400px; margin:40px auto; text-align:center;}"
             << "</style></head><body>"
             << "<div class='dashboard'>"
             << "<div style='text-align:center; margin-bottom:30px;'><h1 style='color:#2c3e50; border-bottom: 3px solid #28a745; display:inline-block; padding-bottom:5px;'>Expense & Income Tracker</h1></div>";

        cout << "<div class='budget-info'>"
             << "<span>Monthly Budget: <strong>" << monthlyBudget << "</strong></span>"
             << "<span>Spent: <strong>" << tExp << "</strong></span>"
             << "<span>Remaining: <strong>" << (monthlyBudget - tExp) << "</strong></span>"
             << "<span>Status: " << (tExp > monthlyBudget ? "<span style='color:#ff4d4d'>OVER LIMIT</span>" : "<span style='color:#4dff4d'>STAYING IN LIMIT</span>") << "</span>"
             << "</div>";

        cout << "<div class='stat-grid'>"
             << "<div class='stat-card' style='border-top:4px solid #28a745;'><h3>Total Income</h3><p>" << fixed << setprecision(2) << tInc << "</p></div>"
             << "<div class='stat-card' style='border-top:4px solid #007bff;'><h3>Savings</h3><p>" << tSav << "</p></div>"
             << "<div class='stat-card' style='border-top:4px solid #dc3545;'><h3>Expenses</h3><p>" << tExp << "</p></div>"
             << "<div class='stat-card' style='border-top:4px solid #FFD700;'><h3>Balance</h3><p>" << (tInc - tExp - tSav) << "</p></div>"
             << "</div>";

        cout << "<h3>Transaction History</h3><table><tr><th>Date (Time)</th><th>Type</th><th>Category</th><th>Description</th><th>Amount</th><th>Action</th></tr>";
        if (history.empty()) {
            cout << "<tr><td colspan='6' style='text-align:center;'>No records found.</td></tr>";
        } else {
            for (auto &tr : history) {
                string cleanDesc = tr.desc; replace(cleanDesc.begin(), cleanDesc.end(), '_', ' ');
                string color = (tr.type == "Income") ? "#28a745" : (tr.type == "Saving" ? "#007bff" : "#dc3545");
                cout << "<tr><td>" << tr.date << " <small>(" << tr.timeStr << ")</small></td><td>" << tr.type << "</td><td>" << tr.category << "</td><td>" << cleanDesc 
                     << "</td><td style='color:" << color << "; font-weight:bold;'>" << tr.amount << "</td>"
                     << "<td><a href='/cgi-bin/tracker.cgi?delete=" << tr.id << "' class='del-btn'>Delete</a></td></tr>";
            }
        }
        cout << "</table>";

        cout << "<div class='chart-container'><h3>Expense & Income Chart</h3><canvas id='myChart'></canvas></div>"
             << "<script>const ctx=document.getElementById('myChart').getContext('2d');"
             << "new Chart(ctx,{type:'pie',data:{labels:['Food','Transport','Bills','Salary','Saving'],"
             << "datasets:[{data:["<<food<<","<<transport<<","<<bills<<","<<salary<<","<<savingsCat<<"],"
             << "backgroundColor:['#ff6384','#36a2eb','#ffce56','#4bc0c0','#9966ff']}]},"
             << "plugins:[ChartDataLabels],options:{plugins:{datalabels:{formatter:(val,ctx)=>{"
             << "let sum=0;let data=ctx.chart.data.datasets[0].data;data.map(d=>sum+=d);"
             << "return sum>0?((val*100)/sum).toFixed(1)+'%':'0%';},color:'#fff',font:{weight:'bold'}}}}});</script>"
             << "<br><center><a href='/TRACK/oops.html' style='font-weight:bold; color:#28a745; text-decoration:none;'>+ Add New Entry / Set Budget</a></center></div></body></html>";
    }
};

string getParam(string q, string p) {
    size_t pos = q.find(p + "=");
    if (pos == string::npos) return "";
    size_t start = pos + p.length() + 1;
    size_t end = q.find("&", start);
    return q.substr(start, end - start);
}

int main() {
    char* qStr = getenv("QUERY_STRING");
    string query = qStr ? qStr : "";
    FinanceTracker myTracker;
    if (!query.empty()) {
        string delId = getParam(query, "delete");
        if (!delId.empty()) {
            myTracker.deleteEntry(delId);
            cout << "Status: 303 See Other\r\nLocation: /cgi-bin/oops.cgi\r\n\r\n";
            return 0;
        }
        string b = getParam(query, "budget");
        if (!b.empty() && b != "0") myTracker.saveBudget(atof(b.c_str()));
        string i = getParam(query, "item");
        if(!i.empty()) myTracker.saveData(getParam(query, "type"), getParam(query, "category"), i, getParam(query, "date"), atof(getParam(query, "price").c_str()));
        if (!b.empty() || !i.empty()) {
            cout << "Status: 303 See Other\r\nLocation: /cgi-bin/oops.cgi\r\n\r\n";
            return 0;
        }
    }
    myTracker.loadData();
    myTracker.displayDashboard();
    return 0;
}