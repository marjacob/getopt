/*******************************************************************************
 * Copyright (c) 2012-2017, Kim Grasman <kim.grasman@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Kim Grasman nor the
 *     names of contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL KIM GRASMAN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#include "CppUnitTest.h"

#include <getopt/getopt.h>

#include <cstdlib>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define GETOPT_EQ(Argv, ShortOptions, LongOptions, Expect)                    \
	do {                                                                  \
		auto const count_{_countof(Argv)};                            \
		auto const result_{::getopt_long(                             \
		    count_, (Argv), (ShortOptions), (LongOptions), nullptr)}; \
                                                                              \
		Assert::AreEqual(result_, static_cast<int>(Expect));          \
	} while (0)

#define OPTOPT_EQ(Expect)                                           \
	do {                                                        \
		Assert::AreEqual(optopt, static_cast<int>(Expect)); \
	} while (0)

namespace {
option g_null_option = {0};

inline void
setup() noexcept
{
	getopt_reset();
}
} // namespace

namespace tests {
TEST_CLASS (getopt_long) {
public:
	TEST_METHOD (test_empty) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t *argv[] = {argv0};

		option const options[] = {g_null_option};

		GETOPT_EQ(argv, L"abc", options, -1);
	}

	TEST_METHOD (test_only_nonoptions) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"nonoption";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {g_null_option};

		GETOPT_EQ(argv, L"a", options, -1);
	}

	TEST_METHOD (test_only_shortopts) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t argv2[] = L"-b";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {g_null_option};

		GETOPT_EQ(argv, L"ab", options, L'a');
		GETOPT_EQ(argv, L"ab", options, L'b');
	}

	TEST_METHOD (test_one_longopt) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--first";
		wchar_t argv2[] = L"-a";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'f'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"ab", options, L'f');
		GETOPT_EQ(argv, L"ab", options, L'a');
	}

	TEST_METHOD (test_two_longopts) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--second";
		wchar_t argv2[] = L"--first";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'f'},
		    {L"second", no_argument, nullptr, L's'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"fs", options, L's');
		GETOPT_EQ(argv, L"fs", options, L'f');
	}

	TEST_METHOD (test_abbreviated) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--fir";
		wchar_t argv2[] = L"--fif";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'1'},
		    {L"fifth", no_argument, nullptr, L'5'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"15", options, L'1');
		GETOPT_EQ(argv, L"15", options, L'5');
	}

	TEST_METHOD (test_abbreviated_with_params) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--fir=100";
		wchar_t argv2[] = L"--fif=500";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", required_argument, nullptr, L'1'},
		    {L"fifth", required_argument, nullptr, L'5'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"15", options, L'1');
		Assert::AreEqual(optarg, L"100");

		GETOPT_EQ(argv, L"15", options, L'5');
		Assert::AreEqual(optarg, L"500");
	}

	TEST_METHOD (test_ambiguous_abbrev) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--fi";
		wchar_t argv2[] = L"--fi";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'1'},
		    {L"fifth", no_argument, nullptr, L'5'},
		    g_null_option,
		};

		// Ambiguous match
		GETOPT_EQ(argv, L"15", options, L'?');
	}

	TEST_METHOD (test_longindex) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--second";
		wchar_t argv2[] = L"--first";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'1'},
		    {L"second", no_argument, nullptr, L'2'},
		    g_null_option,
		};

		int longindex = -1;

		::getopt_long(_countof(argv), argv, L"", options, &longindex);
		Assert::AreEqual(longindex, 1);

		::getopt_long(_countof(argv), argv, L"", options, &longindex);
		Assert::AreEqual(longindex, 0);
	}

	TEST_METHOD (test_flag) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--second";
		wchar_t argv2[] = L"--first";
		wchar_t *argv[] = {argv0, argv1, argv2};

		int flag = 0;

		option const options[] = {
		    {L"first", no_argument, &flag, L'1'},
		    {L"second", no_argument, nullptr, L'2'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"12", options, L'2');

		GETOPT_EQ(argv, L"12", options, 0);
		Assert::AreEqual(flag, static_cast<int>(L'1'));
	}

	TEST_METHOD (test_unknown) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--unknown";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"first", no_argument, nullptr, L'1'},
		    {L"second", no_argument, nullptr, L'5'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"12", options, L'?');
		Assert::AreEqual(optind, 2); // optind should be incremented
	}

	TEST_METHOD (test_required_argument_same_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg=value";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", required_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a:", options, L'a');
		Assert::AreEqual(optarg, L"value");
	}

	TEST_METHOD (test_required_argument_separate_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg";
		wchar_t argv2[] = L"value";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"arg", required_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a:", options, L'a');
		Assert::AreEqual(optarg, L"value");
	}

	TEST_METHOD (test_missing_required_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", required_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a:", options, L':');
		Assert::IsNull(optarg);

		// optind should be incremented past argc looking for args
		Assert::AreEqual(optind, 3);
	}

	TEST_METHOD (test_extraneous_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg=bar";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", no_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a", options, L'?');
		Assert::IsNull(optarg);
	}

	TEST_METHOD (test_empty_required_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg=";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", required_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a:", options, L'a');
		Assert::AreEqual(optarg, L"");
	}

	TEST_METHOD (test_optional_argument_same_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg=value";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", optional_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a::", options, L'a');
		Assert::AreEqual(optarg, L"value");
	}

	TEST_METHOD (test_optional_argument_separate_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg";
		wchar_t argv2[] = L"value";
		wchar_t *argv[] = {argv0, argv1, argv2};

		option const options[] = {
		    {L"arg", optional_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a::", options, L'a');
		Assert::IsNull(optarg);
	}

	TEST_METHOD (test_missing_optional_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", optional_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a::", options, L'a');
		Assert::IsNull(optarg);
		Assert::AreEqual(optind, 2);
	}

	TEST_METHOD (test_empty_optional_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"--arg=";
		wchar_t *argv[] = {argv0, argv1};

		option const options[] = {
		    {L"arg", optional_argument, nullptr, L'a'},
		    g_null_option,
		};

		GETOPT_EQ(argv, L"a::", options, L'a');
		Assert::AreEqual(optarg, L"");
	}

	TEST_METHOD (test_resets_optarg) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-avalue";
		wchar_t *argv[] = {argv0, argv1};

		// First use getopt() to put something into optarg.
		Assert::AreEqual(getopt(_countof(argv), argv, L"a:"),
		                 static_cast<int>(L'a'));
		Assert::AreEqual(optarg, L"value");

		optind = 1;

		// Then make sure that getopt_long() doesn't get that value out
		// when a non-option is processed
		option const options[] = {
		    {L"arg", optional_argument, nullptr, L'a'},
		    g_null_option,
		};

		wchar_t argv1_[] = L"this";
		wchar_t *argv_[] = {argv0, argv1_};

		GETOPT_EQ(argv_, L"a::", options, -1);
		Assert::IsNull(optarg);
	}

	TEST_METHOD (test_resets_optopt) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-z";
		wchar_t *argv[] = {argv0, argv1};

		// First use getopt() to put something into optopt.
		Assert::AreEqual(getopt(_countof(argv), argv, L"a"),
		                 static_cast<int>(L'?'));
		Assert::AreEqual(optopt, static_cast<int>(L'z'));

		optind = 1;

		// Then make sure that getopt_long() doesn't get that value out
		// when a non-option is processed
		option const options[] = {
		    {L"arg", no_argument, nullptr, L'a'},
		    g_null_option,
		};

		wchar_t argv1_[] = L"--zarg";
		wchar_t *argv_[] = {argv0, argv1_};

		GETOPT_EQ(argv_, L"a", options, L'?');
		Assert::AreEqual(optopt, 0);
	}
};
} // namespace tests
