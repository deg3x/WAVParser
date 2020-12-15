#include <iostream>
#include "WAVData.h"

int main()
{
    wav::WAVData wavData("./sample_audio.wav");
    wav::WAVData copy = wavData;

    std::cout << copy << std::endl;

    copy.WriteToFile("./test_write.wav");

    return 0;
}
