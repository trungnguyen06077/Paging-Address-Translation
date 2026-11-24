#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "PageTable.h"

namespace {

// Một test case đọc từ test_cases.csv (7 cột, theo data/DATA_FORMAT.md)
struct Case {
    std::string id;              
    long long logicalAddress;    
    std::string expectedStatus;  
    int expectedPage;
    int expectedOffset;
    int expectedFrame;
    long long expectedPa;
};

// Đọc toàn bộ test case từ file CSV. Trả về false (kèm lý do) nếu lỗi.
bool loadCases(const std::string& filename, std::vector<Case>& cases, std::string& error) {
    std::ifstream file(filename);
    if (!file) {
        error = "khong mo duoc file test " + filename;
        return false;
    }

    std::string line;
    std::getline(file, line);  
    int lineNo = 1;
    while (std::getline(file, line)) {
        lineNo++;
        std::vector<std::string> f = splitLine(line);
        if (f.empty() || f[0].empty()) continue;  
        if (f.size() < 7) {
            error = "dong " + std::to_string(lineNo) + " thieu cot (can 7 cot): " + line;
            return false;
        }
        try {
            Case c;
            c.id = f[0];
            c.logicalAddress = std::stoll(f[1]);
            c.expectedStatus = f[2];
            c.expectedPage = std::stoi(f[3]);
            c.expectedOffset = std::stoi(f[4]);
            c.expectedFrame = std::stoi(f[5]);
            c.expectedPa = std::stoll(f[6]);
            cases.push_back(c);
        } catch (...) {
            error = "dong " + std::to_string(lineNo) + " co gia tri khong phai so: " + line;
            return false;
        }
    }
    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string pageTableFile = (argc > 1) ? argv[1] : "data/page_table.csv";
    std::string testFile = (argc > 2) ? argv[2] : "data/test_cases.csv";
    std::string resultDir = (argc > 3) ? argv[3] : "results";

    // 1. Đọc page table
    PageTable pt;
    std::string error;
    if (!pt.load(pageTableFile, error)) {
        std::cout << "Loi: " << error << "\n";
        return 1;
    }

    // 2. Đọc test case
    std::vector<Case> cases;
    if (!loadCases(testFile, cases, error)) {
        std::cout << "Loi: " << error << "\n";
        return 1;
    }

    // 3. Mở file kết quả
    std::ofstream trace(resultDir + "/trace.txt");
    std::ofstream actual(resultDir + "/actual_results.csv");
    if (!trace || !actual) {
        std::cout << "Loi: khong ghi duoc vao thu muc " << resultDir << "\n";
        return 1;
    }
    trace << "=== TRACE DICH DIA CHI ===\n";
    pt.print(trace);
    trace << "\n";
    actual << "test_id,logical_address,actual_status,actual_page,actual_offset,"
              "actual_frame,actual_physical_address,result\n";

    // 4. Chạy từng test case
    int passCount = 0;
    for (const Case& t : cases) {
        TranslationResult r = pt.translate(t.logicalAddress);

        // So sánh ĐỦ 5 trường: status, page, offset, frame, physical address
        bool pass = (r.status == t.expectedStatus) && (r.page == t.expectedPage) &&
                    (r.offset == t.expectedOffset) && (r.frame == t.expectedFrame) &&
                    (r.physicalAddress == t.expectedPa);
        if (pass) passCount++;
        std::string verdict = pass ? "PASS" : "FAIL";

        // Ghi trace
        trace << "--- " << t.id << " ---\n";
        trace << "Input: LA = " << t.logicalAddress << ", P = " << pt.getPageSize() << "\n";
        for (const std::string& step : r.steps) trace << "  " << step << "\n";
        trace << "  Expected: " << t.expectedStatus << " / page=" << t.expectedPage
              << " offset=" << t.expectedOffset << " frame=" << t.expectedFrame
              << " PA=" << t.expectedPa << "\n";
        trace << "  Actual  : " << r.status << " / page=" << r.page << " offset=" << r.offset
              << " frame=" << r.frame << " PA=" << r.physicalAddress << "\n";
        trace << "  => " << verdict << "\n\n";

        // Ghi actual_results.csv
        actual << t.id << "," << t.logicalAddress << "," << r.status << "," << r.page << ","
               << r.offset << "," << r.frame << "," << r.physicalAddress << "," << verdict
               << "\n";

        // In tóm tắt ra màn hình
        std::cout << t.id << ": LA=" << t.logicalAddress << " -> " << r.status
                  << " / PA=" << r.physicalAddress << "  [" << verdict << "]\n";
    }

    std::cout << "Tong ket: " << passCount << "/" << cases.size() << " test PASS\n";
    trace << "Tong ket: " << passCount << "/" << cases.size() << " test PASS\n";
    return (passCount == (int)cases.size()) ? 0 : 1;
}
