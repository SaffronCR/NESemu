#include "CppUnitTest.h"

#include "SRAM.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace NESemuLibTests
{
    TEST_CLASS(SRAMTest)
    {
    public:

        TEST_METHOD(WriteReadSRAM)
        {
            SRAM sram;

            // If disabled, all reads should return 0
            sram.SetEnabled(false);
            for (int i = 0; i < SRAM::kSize; ++i)
            {
                Assert::AreEqual((int)sram.ReadMem(SRAM::kStartAddress + i), 0);
            }

            // Enable, write some example data and save
            sram.SetEnabled(true);
            for (int i = 0; i < SRAM::kSize; ++i)
            {
                uint8_t value = i % 256;
                sram.WriteMem(SRAM::kStartAddress + i, value);
            }
            sram.Save("test.bin");

            // Load saved file and check contents
            sram.PowerOn();
            for (int i = 0; i < SRAM::kSize; ++i)
            {
                Assert::AreEqual((int)sram.ReadMem(SRAM::kStartAddress + i), 0);
            }
            sram.Load("test.bin");
            for (int i = 0; i < SRAM::kSize; ++i)
            {
                uint8_t value = i % 256;
                Assert::AreEqual(sram.ReadMem(SRAM::kStartAddress + i), value);
            }

            // Disable and check it always returns 0 again
            sram.SetEnabled(false);
            for (int i = 0; i < SRAM::kSize; ++i)
            {
                Assert::AreEqual((int)sram.ReadMem(SRAM::kStartAddress + i), 0);
            }
        }

    };
}