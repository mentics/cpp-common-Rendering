#include "stdafx.h"
#include "CppUnitTest.h"

#include "MenticsCommon.h"
#include "MenticsCommonTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SomethingTest {

TEST_CLASS(SomethingTest) {
	const std::string name = "SomethingTest";

public:
	TEST_CLASS_INITIALIZE(BeforeClass) {
		setupLog();
	}

	TEST_METHOD(FirstTest) {
		Assert::Fail(L"TODO");
	}
};

}