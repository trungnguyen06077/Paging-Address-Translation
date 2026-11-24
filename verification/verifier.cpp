#include "verifier.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

Verifier::Verifier() {}
Verifier::~Verifier() {}

bool Verifier::loadTestCases(const string& filename) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }
    testCases.clear();
    string line;
    if (getline(file, line)) {
        while (getline(file, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);
            if (f.size() < 7) continue;

            VerifierTestCase tc;
            tc.testId = f[0];
            tc.logicalAddress = atoll(f[1].c_str());
            tc.expectedStatus = f[2];
            tc.expectedPage = atoi(f[3].c_str());
            tc.expectedOffset = atoi(f[4].c_str());
            tc.expectedFrame = atoi(f[5].c_str());
            tc.expectedPhysicalAddress = atoll(f[6].c_str());

            testCases.push_back(tc);
        }
    }
    file.close();
    return true;
}

void Verifier::runVerification(const PageTable& pageTable) {
    reportRows.clear();
    for (const auto& tc : testCases) {
        TranslationResult res = pageTable.translate(tc.logicalAddress);
        VerificationRow row;
        row.testId = tc.testId;
        row.logicalAddress = tc.logicalAddress;
        row.actualStatus = res.status;
        row.actualPage = res.page;
        row.actualOffset = res.offset;
        row.actualFrame = res.frame;
        row.actualPhysicalAddress = res.physicalAddress;

        bool matches = true;
        if (row.actualStatus != tc.expectedStatus) matches = false;
        if (row.actualPage != tc.expectedPage) matches = false;
        if (row.actualOffset != tc.expectedOffset) matches = false;
        if (row.actualFrame != tc.expectedFrame) matches = false;
        if (row.actualPhysicalAddress != tc.expectedPhysicalAddress) matches = false;

        row.result = matches ? "PASS" : "FAIL";
        reportRows.push_back(row);
    }
}

bool Verifier::saveResults(const string& filename) const {
    ofstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }
    file << "test_id,logical_address,actual_status,actual_page,actual_offset,actual_frame,actual_physical_address,result\n";
    for (const auto& row : reportRows) {
        file << row.testId << ","
             << row.logicalAddress << ","
             << row.actualStatus << ","
             << row.actualPage << ","
             << row.actualOffset << ","
             << row.actualFrame << ","
             << row.actualPhysicalAddress << ","
             << row.result << "\n";
    }
    file.close();
    return true;
}

bool Verifier::saveDetailedTrace(const string& filename, const PageTable& pageTable) const {
    ofstream file(filename.c_str());
    if (!file.is_open()) {
        return false;
    }
    for (const auto& tc : testCases) {
        TranslationResult res = pageTable.translate(tc.logicalAddress);
        file << "TEST CASE: " << tc.testId << "\n";
        file << "Logical Address: " << tc.logicalAddress << "\n\n";
        for (const auto& step : res.steps) file << step << "\n";
        file << "\n";
        file << "Status: " << res.status << "\n";
        file << "Physical Address: " << res.physicalAddress << "\n";
        file << "----------------------------------------\n";
    }
    file.close();
    return true;
}
