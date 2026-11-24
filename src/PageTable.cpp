#include "PageTable.h"

#include <fstream>
#include <sstream>

// Tách "a, b, c" thành {"a", "b", "c"}
std::vector<std::string> splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, ',')) {
        // bỏ khoảng trắng / xuống dòng ở hai đầu
        size_t start = field.find_first_not_of(" \t\r\n");
        size_t end = field.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            fields.push_back("");
        } else {
            fields.push_back(field.substr(start, end - start + 1));
        }
    }
    return fields;
}
// Đọc file page table. Dạng file:
//   page_size,16
//   page,frame,valid
//   0,5,1        <- page 0 -> frame 5, hợp lệ
//   3,0,0        <- page 3 không hợp lệ
// Các dòng page phải viết theo thứ tự 0, 1, 2, ... (số dòng = page number).
bool PageTable::load(const std::string& filename, std::string& error) {
    std::ifstream file(filename);
    if (!file) {
        error = "Khong mo duoc file: " + filename;
        return false;
    }

    entries.clear();
    pageSize = 0;

    try {
        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> f = splitLine(line);

            // bỏ dòng trống và dòng chú thích (bắt đầu bằng #)
            if (f.empty() || f[0].empty() || f[0][0] == '#') continue;

            if (f[0] == "page_size") {
                pageSize = std::stoi(f.at(1));
            } else if (f[0] == "page") {
                // đây là dòng tiêu đề "page,frame,valid" -> bỏ qua
            } else {
                // dòng dữ liệu: page,frame,valid
                if (f.size() < 3 || std::stoi(f[0]) != (int)entries.size()) {
                    error = "Dong sai dinh dang hoac sai thu tu page: " + line;
                    return false;
                }
                PageTableEntry e;
                e.frame = std::stoi(f[1]);
                e.valid = (std::stoi(f[2]) == 1);
                entries.push_back(e);
            }
        }
    } catch (...) {
        error = "File page table co gia tri khong phai so";
        return false;
    }

    // page size phải > 0 và là lũy thừa của 2 (4, 8, 16, 1024, ...)
    if (pageSize <= 0 || (pageSize & (pageSize - 1)) != 0) {
        error = "page_size phai la luy thua cua 2";
        return false;
    }
    return true;
}
// Dịch địa chỉ - gồm 6 bước theo đề bài
TranslationResult PageTable::translate(long long logicalAddress) const {
    TranslationResult r;

    // (Trường hợp thêm) địa chỉ âm thì không có nghĩa
    if (logicalAddress < 0) {
        r.status = "NEGATIVE_ADDRESS";
        r.steps.push_back("Buoc 1: LA = " + std::to_string(logicalAddress) + " < 0 -> INVALID");
        return r;
    }

    // Bước 1: tách page number và offset
    r.page = (int)(logicalAddress / pageSize);    // phần nguyên của phép chia
    r.offset = (int)(logicalAddress % pageSize);  // phần dư
    r.steps.push_back("Buoc 1: page = " + std::to_string(logicalAddress) + " / " +
                      std::to_string(pageSize) + " = " + std::to_string(r.page) +
                      ", offset = " + std::to_string(logicalAddress) + " mod " +
                      std::to_string(pageSize) + " = " + std::to_string(r.offset));

    // Bước 2: page number có nằm trong page table không?
    if (r.page >= (int)entries.size()) {
        r.status = "PAGE_OUT_OF_TABLE";
        r.steps.push_back("Buoc 2: page " + std::to_string(r.page) + " >= so entry (" +
                          std::to_string(entries.size()) + ") -> INVALID (ngoai page table)");
        return r;
    }
    r.steps.push_back("Buoc 2: page " + std::to_string(r.page) + " < " +
                      std::to_string(entries.size()) + " -> co trong page table");

    // Bước 3: kiểm tra valid bit
    const PageTableEntry& entry = entries[r.page];
    if (!entry.valid) {
        r.status = "INVALID_ENTRY";
        r.steps.push_back("Buoc 3: valid bit cua page " + std::to_string(r.page) +
                          " = 0 -> INVALID_ENTRY");
        return r;
    }
    r.steps.push_back("Buoc 3: valid bit cua page " + std::to_string(r.page) + " = 1 -> hop le");

    // Bước 4: lấy frame number
    r.frame = entry.frame;
    r.steps.push_back("Buoc 4: frame = " + std::to_string(r.frame));

    // Bước 5: tính physical address
    r.physicalAddress = (long long)r.frame * pageSize + r.offset;
    r.steps.push_back("Buoc 5: PA = " + std::to_string(r.frame) + " * " +
                      std::to_string(pageSize) + " + " + std::to_string(r.offset) + " = " +
                      std::to_string(r.physicalAddress));

    // Bước 6: trả về kết quả
    r.status = "OK";
    r.steps.push_back("Buoc 6: ket qua PA = " + std::to_string(r.physicalAddress));
    return r;
}

// In page table ra màn hình / file
void PageTable::print(std::ostream& out) const {
    out << "Page size P = " << pageSize << " bytes\n";
    out << "page | frame | valid\n";
    for (size_t i = 0; i < entries.size(); i++) {
        out << i << "    | ";
        if (entries[i].valid) out << entries[i].frame; else out << "-";
        out << "     | " << (entries[i].valid ? 1 : 0) << "\n";
    }
}