# Paging Address Translation

Chương trình C++ mô phỏng **dịch địa chỉ logic → địa chỉ vật lý** bằng page table một cấp (page size cố định, có valid bit). Chương trình đọc page table và các test case từ file CSV, dịch từng địa chỉ, so sánh với kết quả mong đợi và ghi ra file kết quả cùng trace từng bước.

## Công thức

```
page   = LA / P
offset = LA mod P
PA     = frame * P + offset
```

Với `P = 16` byte (xem `data/page_table.csv`).

## Cấu trúc thư mục

```
Paging Address Translation/
├── src/
│   ├── main.cpp            
│   ├── PageTable.h        
│   └── PageTable.cpp      
├── data/
│   ├── page_table.csv     
│   ├── test_cases.csv   
│   └── DATA_FORMAT.md     
├── results/
│   ├── actual_results.csv  
│   └── trace.txt           
├── verification/
│   ├── verifier.h / .cpp  
└── paging.exe             
```

## Yêu cầu

- Trình biên dịch C++ hỗ trợ C++11 trở lên, ví dụ **g++** (Linux/macOS) hoặc **MinGW-w64 g++** (Windows).
- Không cần thư viện ngoài, không cần `make` hay CMake.

Kiểm tra đã cài g++:

```bash
g++ --version
```

## Build

Mở terminal **tại thư mục gốc của dự án** (thư mục chứa `src/`, `data/`, `results/`) rồi chạy:

```bash
g++ -std=c++17 -Wall -Wextra -o paging src/main.cpp src/PageTable.cpp
```

- Linux/macOS: tạo file `./paging`
- Windows (MinGW): tạo file `paging.exe`

## Chạy

> **Quan trọng:** luôn chạy từ thư mục gốc của dự án vì đường dẫn mặc định là tương đối (`data/...`, `results/...`). Thư mục `results/` phải tồn tại sẵn (chương trình không tự tạo thư mục).

**Dùng đường dẫn mặc định:**

```bash
./paging            # Linux/macOS
paging.exe          # Windows (cmd/PowerShell: .\paging.exe)
```

**Chỉ rõ file đầu vào và thư mục kết quả:**

```bash
./paging data/page_table.csv data/test_cases.csv results
```

| Tham số | Ý nghĩa | Mặc định |
|---|---|---|
| 1 | File page table | `data/page_table.csv` |
| 2 | File test case | `data/test_cases.csv` |
| 3 | Thư mục ghi kết quả | `results` |

### Mã thoát

| Mã | Ý nghĩa |
|---|---|
| `0` | Tất cả test PASS |
| `1` | Có test FAIL, hoặc lỗi đọc/ghi file |

Kiểm tra mã thoát: `echo $?` (Linux/macOS) hoặc `echo %ERRORLEVEL%` (Windows cmd).

## Tái tạo kết quả

Các bước để sinh lại `results/actual_results.csv` và `results/trace.txt` từ đầu:

```bash
# 1. Xoá kết quả cũ (đảm bảo thư mục results/ vẫn tồn tại)
rm -f results/actual_results.csv results/trace.txt      # Windows: del results\*.csv results\*.txt

# 2. Build
g++ -std=c++17 -Wall -Wextra -o paging src/main.cpp src/PageTable.cpp

# 3. Chạy
./paging
```

### Kết quả mong đợi trên màn hình

```
TC01: LA=0 -> OK / PA=80  [PASS]
TC02: LA=15 -> OK / PA=95  [PASS]
TC03: LA=16 -> OK / PA=32  [PASS]
TC04: LA=31 -> OK / PA=47  [PASS]
TC05: LA=37 -> OK / PA=117  [PASS]
TC06: LA=47 -> OK / PA=127  [PASS]
TC07: LA=48 -> INVALID_ENTRY / PA=-1  [PASS]
TC08: LA=64 -> OK / PA=16  [PASS]
TC09: LA=80 -> INVALID_ENTRY / PA=-1  [PASS]
TC10: LA=96 -> PAGE_OUT_OF_TABLE / PA=-1  [PASS]
TC11: LA=-1 -> NEGATIVE_ADDRESS / PA=-1  [PASS]
Tong ket: 11/11 test PASS
```

### Đối chiếu với kết quả gốc

Để so sánh với bản kết quả đã có, hãy sao lưu trước khi chạy lại:

```bash
cp results/actual_results.csv /tmp/actual_results.orig.csv
cp results/trace.txt          /tmp/trace.orig.txt
# ... chạy lại chương trình như trên ...
diff --strip-trailing-cr results/actual_results.csv /tmp/actual_results.orig.csv && echo "CSV khop"
diff --strip-trailing-cr results/trace.txt          /tmp/trace.orig.txt          && echo "Trace khop"
```

## Các test case

Page size `P = 16`, page table có 6 entry (page 0–5):

| page | frame | valid |
|---|---|---|
| 0 | 5 | 1 |
| 1 | 2 | 1 |
| 2 | 7 | 1 |
| 3 | - | 0 |
| 4 | 1 | 1 |
| 5 | - | 0 |

Giá trị `status` có thể trả về (chi tiết trong `data/DATA_FORMAT.md`):

| Status | Ý nghĩa |
|---|---|
| `OK` | Dịch thành công |
| `NEGATIVE_ADDRESS` | Địa chỉ logic âm |
| `PAGE_OUT_OF_TABLE` | Page number ≥ số entry của page table |
| `INVALID_ENTRY` | Entry tồn tại nhưng valid bit = 0 |

Với trường hợp không dịch được, `frame` và `physical address` đều là `-1`. Mỗi test so sánh đủ **5 trường**: status, page, offset, frame, physical address.

## Dùng dữ liệu riêng

**`page_table.csv`**

```
page_size,16
page,frame,valid
0,5,1
1,2,1
```

- Dòng 1: `page_size` (phải là luỹ thừa của 2).
- Dòng 2: tiêu đề cột.
- Các dòng sau: `page,frame,valid`, **phải viết theo thứ tự 0, 1, 2, …**
- Dòng trống và dòng bắt đầu bằng `#` được bỏ qua.

**`test_cases.csv`** gồm 7 cột:

```
test_id,logical_address,expected_status,expected_page,expected_offset,expected_frame,expected_physical_address
```

Sau khi sửa dữ liệu, chạy lại bằng:

```bash
./paging data/page_table.csv data/test_cases.csv results
```
