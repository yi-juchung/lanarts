#include <cstdio>

#include <lcommon/unittest.h>
#include <lcommon/fatal_error.h>

#include "data/FilenameList.h"

SUITE(parse_unit_tests) {

	TEST(filenames_from_pattern_test) {
		{
			FilenameList testfiles, expectedfiles;
			filenames_from_pattern(testfiles, "test");
			expectedfiles.push_back("test");
			CHECK(testfiles == expectedfiles);
		}
		{
			FilenameList testfiles, expectedfiles;
			filenames_from_pattern(testfiles, "test(0-2)");
			expectedfiles.push_back("test0");
			expectedfiles.push_back("test1");
			expectedfiles.push_back("test2");
			CHECK(testfiles == expectedfiles);
		}
		{
			FilenameList testfiles, expectedfiles;
			filenames_from_pattern(testfiles, "test(0-2).png");
			expectedfiles.push_back("test0.png");
			expectedfiles.push_back("test1.png");
			expectedfiles.push_back("test2.png");
			CHECK(testfiles == expectedfiles);
		}

		{
			FilenameList testfiles, expectedfiles;
			filenames_from_pattern(testfiles, "test(2-2)");
			expectedfiles.push_back("test2");
			CHECK(testfiles == expectedfiles);
		}

		{
			bool failed = false;
			try {
				FilenameList testfiles;
				printf("NB: Next failure expected ...\n");
				filenames_from_pattern(testfiles, "test(2-2");
			} catch (const __FatalError& _) {
				failed = true;
			}
			unit_test_assert("Only start bracket should be invalid", failed);
		}

		{
			bool failed = false;
			try {
				FilenameList testfiles;
				printf("NB: Next failure expected ...\n");
				filenames_from_pattern(testfiles, "test2-2)");
			} catch (const __FatalError& _) {
				failed = true;
			}
			unit_test_assert("Only end bracket should be invalid", failed);
		}

	}
}
