/*
 * FIFOBuff Comprehensive Test Suite
 * =================================
 * 
 * This test suite validates the FIFOBuff circular buffer implementation
 * with the following test cases:
 * 
 * 1. TestInitialState() - Verifies initial buffer state
 * 2. TestBasicWriteRead() - Tests basic write/read operations
 * 3. TestMultipleWriteRead() - Tests multiple byte operations
 * 4. TestBufferFull() - Tests buffer overflow handling
 * 5. TestBufferEmpty() - Tests buffer underflow handling
 * 6. TestCircularBuffer() - Tests circular buffer behavior
 * 7. TestPartialReadWrite() - Tests partial read/write operations
 * 8. TestMixedOperations() - Tests mixed sequential operations
 * 9. TestBoundaryConditions() - Tests zero-byte operations
 * 10. TestReturnValues() - Tests return value accuracy
 * 11. TestOverflowAndUnderflow() - Tests capacity limit handling
 * 
 * Key Features Tested:
 * - Circular buffer wraparound
 * - Capacity management (buffSize-1 usable slots)
 * - Return value accuracy for WriteBuff/ReadBuff
 * - Proper handling of full/empty states
 * - Position tracking (Read/Write positions)
 * - Room size calculation
 */

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

void PrintFIFOBuff(const FIFOBuff<char, 8>& buff)
{
	::printf("Read=%d, Write=%d, RoomLength=%d\n",
		buff.GetReadPos(), buff.GetWritePos(), buff.GetRoomLength());
}

// Test helper: Print test separator
void PrintTestSeparator(const char* testName)
{
	::printf("\n=== Test: %s ===\n", testName);
}

bool TestInitialState()
{
	PrintTestSeparator("Initial State");
	FIFOBuff<char, 8> buff;
	PrintFIFOBuff(buff);
		bool result = (buff.GetReadPos() == 0) && 
				  (buff.GetWritePos() == 0) && 
				  (buff.GetRoomLength() == 7); // For size 8, actual usable buffer size is 7
	::printf("Initial state test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestBasicWriteRead()
{
	::printf("\n=== Test: Basic Write/Read ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	
	// Write 1 byte
	int initial_room = buff.GetRoomLength();
	int written = buff.WriteBuff(src, 1);
	::printf("Written %d byte(s)\n", written);
	PrintFIFOBuff(buff);
	
	// Read 1 byte
	int read = buff.ReadBuff(dst, 1);
	::printf("Read %d byte(s), data='%c'\n", read, dst[0]);
	PrintFIFOBuff(buff);
	
	bool result = (initial_room == 7) && (written == 1) && (read == 1) && 
				(dst[0] == '0') && (buff.GetReadPos() == buff.GetWritePos());
	::printf("Basic write/read test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestMultipleWriteRead()
{
	::printf("\n=== Test: Multiple Write/Read ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	
	// Write 5 bytes
	int written = buff.WriteBuff(src, 5);
	::printf("Written %d byte(s)\n", written);
	PrintFIFOBuff(buff);
	
	// Read 3 bytes
	int read = buff.ReadBuff(dst, 3);
	::printf("Read %d byte(s), data='%.3s'\n", read, dst);
	PrintFIFOBuff(buff);
	
	bool result = (written == 5) && (read == 3) && (buff.GetRoomLength() == 5) && 
				(dst[0] == '0') && (dst[1] == '1') && (dst[2] == '2');
	::printf("Multiple write/read test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestBufferFull()
{
	::printf("\n=== Test: Buffer Full ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	
	// Fill the buffer to capacity
	int written = buff.WriteBuff(src, 7);
	::printf("Written %d byte(s) to fill buffer\n", written);
	PrintFIFOBuff(buff);
	
	// Try to write more data
	int room_before = buff.GetRoomLength();
	int overwritten = buff.WriteBuff(src + 7, 5);
	int room_after = buff.GetRoomLength();
	::printf("Attempted to write 5 more bytes, actually written: %d\n", overwritten);
	PrintFIFOBuff(buff);
	
	bool result = (written == 7) && (room_before == 0) && (overwritten == 0) && (room_after == 0);
	::printf("Buffer full test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestBufferEmpty()
{
	::printf("\n=== Test: Buffer Empty ===\n");
	FIFOBuff<char, 8> buff;
	char dst[16] = {0};
	
	// Try to read from empty buffer
	int read_pos_before = buff.GetReadPos();
	int write_pos_before = buff.GetWritePos();
	int read = buff.ReadBuff(dst, 5);
	int read_pos_after = buff.GetReadPos();
	int write_pos_after = buff.GetWritePos();
	::printf("Attempted to read 5 bytes from empty buffer, actually read: %d\n", read);
	PrintFIFOBuff(buff);
	
	bool result = (read == 0) && (read_pos_before == read_pos_after) && 
				(write_pos_before == write_pos_after);
	::printf("Buffer empty test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestCircularBuffer()
{
	::printf("\n=== Test: Circular Buffer ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	
	// Partial write
	int written1 = buff.WriteBuff(src, 4);
	::printf("Written %d byte(s)\n", written1);
	PrintFIFOBuff(buff);
	
	// Partial read
	int read1 = buff.ReadBuff(dst, 2);
	::printf("Read %d byte(s): '%.2s'\n", read1, dst);
	PrintFIFOBuff(buff);
	
	// Write more data (cause wraparound)
	int written2 = buff.WriteBuff(src + 4, 5);
	::printf("Written %d more byte(s)\n", written2);
	PrintFIFOBuff(buff);
	
	// Read all remaining data
	memset(dst, 0, sizeof(dst));
	int read2 = buff.ReadBuff(dst, 10);
	::printf("Read all remaining %d byte(s), data='%s'\n", read2, dst);
	PrintFIFOBuff(buff);
	
	bool result = (written1 == 4) && (read1 == 2) && (written2 == 5) && 
				(read2 == 7) && (strncmp(dst, "2345678", 7) == 0);
	::printf("Circular buffer test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestPartialReadWrite()
{
	::printf("\n=== Test: Partial Read/Write ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	
	// Write 3 bytes
	int written = buff.WriteBuff(src, 3);
	::printf("Written %d byte(s)\n", written);
	PrintFIFOBuff(buff);
	
	// Try to read 10 bytes (only 3 available)
	int read = buff.ReadBuff(dst, 10);
	::printf("Attempted to read 10 bytes, actually read: %d, data='%s'\n", read, dst);
	PrintFIFOBuff(buff);
	
	bool result = (written == 3) && (read == 3) && (strncmp(dst, "012", 3) == 0) && 
				(buff.GetReadPos() == buff.GetWritePos());
	::printf("Partial read/write test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestMixedOperations()
{
	::printf("\n=== Test: Mixed Operations ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	bool result = true;
	
	// Multiple write/read operations sequence
	int written1 = buff.WriteBuff(src, 2);
	::printf("Step 1: Written %d byte(s)\n", written1);
	PrintFIFOBuff(buff);
	result = result && (written1 == 2);
	
	int read1 = buff.ReadBuff(dst, 1);
	::printf("Step 2: Read %d byte(s): '%c'\n", read1, dst[0]);
	PrintFIFOBuff(buff);
	result = result && (read1 == 1) && (dst[0] == '0');
	
	int written2 = buff.WriteBuff(src + 2, 4);
	::printf("Step 3: Written %d more byte(s)\n", written2);
	PrintFIFOBuff(buff);
	result = result && (written2 == 4);
	
	memset(dst, 0, sizeof(dst));
	int read2 = buff.ReadBuff(dst, 3);
	::printf("Step 4: Read %d byte(s): '%s'\n", read2, dst);
	PrintFIFOBuff(buff);
	result = result && (read2 == 3) && (strncmp(dst, "123", 3) == 0);
	
	::printf("Mixed operations test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestBoundaryConditions()
{
	::printf("\n=== Test: Boundary Conditions ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";	char dst[16] = {0};
	bool result = true;
	
	// Write 0 bytes
	int room_before = buff.GetRoomLength();
	int written = buff.WriteBuff(src, 0);
	int room_after = buff.GetRoomLength();
	::printf("Written 0 bytes - requested: 0, actual: %d, room before: %d, after: %d\n", 
			 written, room_before, room_after);
	result = result && (written == 0) && (room_before == room_after);
	
	// Read 0 bytes
	buff.WriteBuff(src, 3);
	int read = buff.ReadBuff(dst, 0);
	::printf("Read 0 bytes from buffer with 3 bytes - requested: 0, actual: %d\n", read);
	PrintFIFOBuff(buff);
	result = result && (read == 0) && (buff.GetRoomLength() == 4); // 7 - 3 = 4
	
	::printf("Boundary conditions test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestReturnValues()
{
	::printf("\n=== Test: Return Values ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	bool result = true;
	
	// Normal write return value test
	int written1 = buff.WriteBuff(src, 3);
	::printf("Wrote 3 bytes: returned %d\n", written1);
	result = result && (written1 == 3);
	
	// Normal read return value test
	int read1 = buff.ReadBuff(dst, 2);
	::printf("Read 2 bytes: returned %d, data='%.2s'\n", read1, dst);
	result = result && (read1 == 2);
	
	// Write return value test when buffer is full
	buff.WriteBuff(src, 6); // Use remaining capacity
	int written2 = buff.WriteBuff(src, 5); // Capacity overflow
	::printf("Attempted to write 5 bytes to full buffer: returned %d\n", written2);
	result = result && (written2 == 0);
	
	// Read return value test when buffer is empty
	buff.ReadBuff(dst, 10); // Read all data
	int read2 = buff.ReadBuff(dst, 3); // Read from empty buffer
	::printf("Attempted to read 3 bytes from empty buffer: returned %d\n", read2);
	result = result && (read2 == 0);
	
	::printf("Return values test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

bool TestOverflowAndUnderflow()
{
	::printf("\n=== Test: Overflow and Underflow ===\n");
	FIFOBuff<char, 8> buff;
	const char* src = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char dst[16] = {0};
	bool result = true;
	
	// Overflow: Try to write more than buffer capacity
	int written = buff.WriteBuff(src, 10); // Try to write more than 7 bytes
	::printf("Attempted to write 10 bytes (capacity is 7): actually written %d\n", written);
	PrintFIFOBuff(buff);
	result = result && (written == 7); // Write up to maximum capacity
	
	// Underflow: Try to read more than buffer content
	int read = buff.ReadBuff(dst, 10); // Try to read more than 7 bytes
	::printf("Attempted to read 10 bytes (buffer has 7): actually read %d\n", read);
	::printf("Read data: '%.*s'\n", read, dst);
	PrintFIFOBuff(buff);
	result = result && (read == 7); // Read all content
	
	::printf("Overflow and underflow test: %s\n", result ? "PASS" : "FAIL");
	return result;
}

void RunAllTests()
{
	::printf("Starting FIFOBuff comprehensive test suite...\n\n");
	
	int passed = 0;
	int total = 0;
	
	total++; if (TestInitialState()) passed++;
	total++; if (TestBasicWriteRead()) passed++;
	total++; if (TestMultipleWriteRead()) passed++;
	total++; if (TestBufferFull()) passed++;
	total++; if (TestBufferEmpty()) passed++;
	total++; if (TestCircularBuffer()) passed++;
	total++; if (TestPartialReadWrite()) passed++;
	total++; if (TestMixedOperations()) passed++;
	total++; if (TestBoundaryConditions()) passed++;
	total++; if (TestReturnValues()) passed++;
	total++; if (TestOverflowAndUnderflow()) passed++;
	
	::printf("\n=== Test Results ===\n");
	::printf("Passed: %d/%d\n", passed, total);
	::printf("Success Rate: %.1f%%\n", (float)passed / total * 100.0f);
	
	if (passed == total) {
		::printf("🎉 All tests PASSED! ✓\n");
		::printf("The FIFOBuff implementation is working correctly.\n");
	} else {
		::printf("❌ Some tests FAILED! ✗\n");
		::printf("Please review the failed test cases above.\n");
	}
}

int main()
{
	::stdio_init_all();
	::printf("FIFOBuff Comprehensive Test Suite\n");
	::printf("==================================\n\n");
	
	RunAllTests();
	
	for (;;) ::tight_loop_contents();
}
