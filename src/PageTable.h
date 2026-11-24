#include <iostream>
#include <string>
#include <vector>

// Một dòng của page table
struct PageTableEntry {
    int  frame;  // số frame mà page này được ánh xạ tới
    bool valid;  // true = hợp lệ (valid), false = không hợp lệ (invalid)
};

// Kết quả của MỘT lần dịch địa chỉ
struct TranslationResult {
    std::string status;        // "OK", "NEGATIVE_ADDRESS", "PAGE_OUT_OF_TABLE" hoặc "INVALID_ENTRY"
    int page = -1;             // page number (-1 nếu địa chỉ âm)
    int offset = -1;           // offset trong page
    int frame = -1;            // frame number (chỉ có khi status = OK)
    long long physicalAddress = -1;  // physical address (chỉ có khi status = OK)
    std::vector<std::string> steps;  // ghi lại từng bước để in trace
};

// Hàm phụ: tách một dòng CSV thành các cột, đã bỏ khoảng trắng hai đầu
std::vector<std::string> splitLine(const std::string& line);

class PageTable {
public:
    // Đọc page size và các entry từ file CSV. Trả về false (kèm lý do trong error) nếu lỗi.
    bool load(const std::string& filename, std::string& error);

    // Dịch logical address -> physical address
    TranslationResult translate(long long logicalAddress) const;

    int getPageSize() const { return pageSize; }
    void print(std::ostream& out) const;

private:
    int pageSize = 0;                     // P (byte)
    std::vector<PageTableEntry> entries;  // entries[i] là entry của page i
};