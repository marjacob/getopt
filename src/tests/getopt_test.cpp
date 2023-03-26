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

#define GETOPT_EQ(Argv, Options, Expect)                                 \
	do {                                                             \
		auto const count_{_countof(Argv)};                       \
		auto const result_{::getopt(count_, (Argv), (Options))}; \
                                                                         \
		Assert::AreEqual(result_, static_cast<int>(Expect));     \
	} while (0)

#define OPTOPT_EQ(Expect)                                           \
	do {                                                        \
		Assert::AreEqual(optopt, static_cast<int>(Expect)); \
	} while (0)

namespace {
inline void
setup() noexcept
{
	getopt_reset();
}
} // namespace

namespace tests {
TEST_CLASS (getopt) {
public:
	TEST_METHOD (test_empty) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t *argv[] = {argv0};

		GETOPT_EQ(argv, L"abc", -1);
	}

	TEST_METHOD (test_simple) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a", L'a');
	}

	TEST_METHOD (test_unrecognized) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"cde", L'?');
	}

	TEST_METHOD (test_only_nonoptions) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"nonoption";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a", -1);
	}

	TEST_METHOD (test_consume_subset) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-f";
		wchar_t argv2[] = L"-g";
		wchar_t argv3[] = L"-h";
		wchar_t *argv[] = {argv0, argv1, argv2, argv3};

		GETOPT_EQ(argv, L"fgh", L'f');
		GETOPT_EQ(argv, L"fgh", L'g');
	}

	TEST_METHOD (test_multiple_options_separate_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t argv2[] = L"-b";
		wchar_t *argv[] = {argv0, argv1, argv2};

		GETOPT_EQ(argv, L"ab", L'a');
		GETOPT_EQ(argv, L"ab", L'b');
	}

	TEST_METHOD (test_multiple_options_same_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-ba";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"ab", L'b');
		GETOPT_EQ(argv, L"ab", L'a');
	}

	TEST_METHOD (test_embedded_nonoptions) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t argv2[] = L"nonoption1";
		wchar_t argv3[] = L"nonoption2";
		wchar_t argv4[] = L"-b";
		wchar_t *argv[] = {argv0, argv1, argv2, argv3, argv4};

		GETOPT_EQ(argv, L"ab", L'a');

		// The non-option argument "nonoption1" terminates the scan
		GETOPT_EQ(argv, L"ab", -1);
	}

	TEST_METHOD (test_argument_same_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-aargument";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a:", L'a');
		Assert::AreEqual(optarg, L"argument");
	}

	TEST_METHOD (test_argument_separate_argv) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t argv2[] = L"argument";
		wchar_t *argv[] = {argv0, argv1, argv2};

		GETOPT_EQ(argv, L"a:", L'a');
		Assert::AreEqual(optarg, L"argument");
	}

	TEST_METHOD (test_missing_required_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a:", L'?');
		OPTOPT_EQ(L'a');
		Assert::IsNull(optarg);

		// optind should be incremented past argc looking for args
		Assert::AreEqual(optind, 3);
	}

	TEST_METHOD (test_missing_required_argument_colon) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L":a:", L':');
		OPTOPT_EQ(L'a');
		Assert::IsNull(optarg);

		// optind should be incremented past argc looking for args
		Assert::AreEqual(optind, 3);
	}

	TEST_METHOD (test_optional_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-aargument";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a::", L'a');
		Assert::AreEqual(optarg, L"argument");
	}

	TEST_METHOD (test_missing_optional_argument) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t *argv[] = {argv0, argv1};

		GETOPT_EQ(argv, L"a::", L'a');
		Assert::IsNull(optarg);
	}

	TEST_METHOD (test_getopt_optopt) {
		setup();

		wchar_t argv0[] = L"foo.exe";
		wchar_t argv1[] = L"-a";
		wchar_t argv2[] = L"-b";
		wchar_t *argv[] = {argv0, argv1, argv2};

		GETOPT_EQ(argv, L"a", L'a');
		OPTOPT_EQ(L'a');

		GETOPT_EQ(argv, L"a", L'?');
		OPTOPT_EQ(L'b');
	}
};
} // namespace tests
