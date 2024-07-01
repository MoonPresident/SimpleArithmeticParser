#include <iostream>
#include <vector>

#include "SimpleArithmeticParser.h"

int main(int argc, char** argv) {
	//In a different world I would put this into a text file and read it in as
	//range of tests so they could be changed without recompiling but I assume
	//you have your own test suite so this is only here if anyone wants to
	//check that this code compiles and runs.
	std::vector<std::pair<int, const char*>> testsExpectingSuccess {
		{4, "1 + 3"},
		{8, "(1  + 3) * 2"},
		{8, "(4 / 2) + 6"},
		{8, "1 + ( 3 + 4)"},
		{1, "(1)"},
		{0, "1 / 3"},
		{0, "(((((0)))))"},
		{3, "1--2"},
		
		{-4, "1 + -5"},
		{-8, "(1  + 3) * -2"},
		{80, "(40 / 2) + 60"},
		{8, "1 + ( 3 + 4)"},
		{1000, "(1000)"},
		{0, "1000000 / 3000000"},
		{0, "(((((00000000)))))"},
		{-3, "4 - 7"},
		
		{2, "1 * -5 / -2"},
		{1, "(1 * 3) + -2"},
		{60, "(40 / 200) + 60"},
		{2, "14 / ( 3 + 4)"},
		{23, "(2000) - 1000 / 1000 * 23"},
		{3, "10000 / 3000"},
		{0, "(((((-00000000)))))"},
		{-111111, "-1 * 111111"},
		
		{50, "(-5 * ((5 * 6) - (5 * 8)))"},
		{70, "(40 - (3 * ((5 * 6) - (5 * 8))))"},
		{200, "4 - 3 * 5 * 6 - 5 * 8"},
	};
	
	std::vector<const char*> testsExpectingFailure {
		"",
		"(",
		")",
		"1 / 0",
		"--",
		"--1",
		"-(12)",
		"((5 * 6) - (5 * 8)",
	};
	
	int expectedSuccessCount = 0;
	int unexpectedSuccessCount = 0;
	int expectedFailCount = 0;
	int unexpectedFailCount = 0;
	
	std::cout << "Testing...\n";
	
	for(auto [expected, test]: testsExpectingSuccess) {
		int result;
		bool b = evaluate(test, result);
		if(!b) {
			std::cout << "EXPECTED SUCCESS - FAILED\n\t" << test << "\n\tExpected true\n\tReceived: false\n";
		} else if(result != expected) {
			std::cout << "EXPECTED SUCCESS - FAILED\n\t" << test << "\n\tExpected" << expected << "\n\tReceived:" << result << "\n";
			unexpectedFailCount++;
		} else {
			std::cout << "EXPECTED SUCCESS - PASSED: " << test << "\n";
			expectedSuccessCount++;
		}
	}
	
	for(auto test: testsExpectingFailure) {
		int result;
		bool b = evaluate(test, result);
		if(b) {
			std::cout << "EXPECTED FAILURE - FAILED\n\t" << test << "\n\tExpected false\n\tReceived: true\n";
			unexpectedSuccessCount++;
		} else {
			std::cout << "EXPECTED FAILURE - PASSED: " << test << "\n";
			expectedFailCount++;
		}
	}
	
	std::cout << "\n\n\tExpected passes: " << expectedSuccessCount << "\n\tUnexpected passes: " << unexpectedSuccessCount;
	std::cout << "\n\n\tExpected failures: " << expectedFailCount << "\n\tUnexpected failures: " << unexpectedFailCount << "\n\n";
	
	return 0;
}