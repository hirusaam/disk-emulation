This guide provides instructions for using the file system emulation and running the test cases (testcase1, testcase2, testcase3, testcase4) to generate reports. It includes compiling, running, and verifying the test case results.

Files Required
fs-disk.c: Handles disk operations for the emulated file system.
fs-disk.h: Header file with disk-related definitions and declarations.
fs-ops.c: Implements core file system operations such as create, open, read, write, delete, and seek.
fs-ops.h: Header file with file system operation declarations.
testcase.c: Contains test case logic.

Compiling and Running Test Cases
Test Case Compilation: Compile the testcase.c along with the file system implementation files using the gcc compiler.

gcc testcase.c fs-ops.c fs-disk.c -o testcase
This command compiles the code and produces an executable named testcase.

Running Test Cases: Execute the compiled program and redirect its output to a file.

./testcase > output.txt
The output of the test case will be stored in the output.txt file.

Test Case Descriptions
Test Case 1:

Objective: Verify basic file creation and writing.
Expected Behavior: File should be created and data should be written correctly.


Test Case 2:

Objective: Test reading data from a file.
Expected Behavior: Correct data should be read without errors.


Test Case 3:

Objective: Test deleting files and freeing resources.
Expected Behavior: Resources (inodes and blocks) should be reclaimed after deletion.


Test Case 4:

Objective: Test edge cases, such as seeking beyond file boundaries or writing exceeding maximum size.
Expected Behavior: Operations should fail gracefully without corrupting the file system.