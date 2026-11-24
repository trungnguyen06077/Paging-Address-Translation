# Data Format Documentation

## 1. page_table.csv

Purpose:
Contains page size and page table entries.

Format:
- First row: page_size
- Second row: column headers
- Remaining rows: page, frame, valid

Example:
page_size,16
page,frame,valid
0,5,1

## 2. test_cases.csv

Purpose:
Contains 11 test cases and manually calculated expected results.

Columns:
- test_id
- logical_address
- expected_status
- expected_page
- expected_offset
- expected_frame
- expected_physical_address

## 3. Expected Status

- OK: Translation successful
- NEGATIVE_ADDRESS: Logical address is negative
- PAGE_OUT_OF_TABLE: Page is outside page table
- INVALID_ENTRY: Valid bit = 0

For invalid translation:
Frame and physical address use -1
when they are not applicable.

## 4. Page Size

Page Size = 16 bytes.