#ifndef VERIFIER_H
#define VERIFIER_H

#include <string>
#include <vector>
#include "PageTable.h"

struct VerifierTestCase {
    std::string testId;
    long long logicalAddress;
    std::string expectedStatus;
    int expectedPage;
    int expectedOffset;
    int expectedFrame;
    long long expectedPhysicalAddress;
};

struct VerificationRow {
    std::string testId;
    long long logicalAddress;
    std::string actualStatus;
    int actualPage;
    int actualOffset;
    int actualFrame;
    long long actualPhysicalAddress;
    std::string result;
};

class Verifier {
public:
    Verifier();
    ~Verifier();
    bool loadTestCases(const std::string& filename);
    void runVerification(const PageTable& pageTable);
    bool saveResults(const std::string& filename) const;
    bool saveDetailedTrace(const std::string& filename, const PageTable& pageTable) const;

private:
    std::vector<VerifierTestCase> testCases;
    std::vector<VerificationRow> reportRows;
};

#endif
