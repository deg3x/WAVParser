# RIFF/WAV-Parser
A C++ library for parsing WAV files. \
Parsing should work for most generic RIFF types as well, however the code to extract the actual \
data section must be implemented for every specific file type.

For the time being file modification functionality is not provided. \
If you are using this library and need to modify a WAV file you will have to parse its data using the \
functionality provided, then work in raw bytes in order to create non-corrupt headers/data.

## Example Usage
```
#include "WAVData.h"

void some_function()
{
    ...
    
    wav::WAVData wavExample("path/to/your/file.wav");

    // You can also initialize through:
    // wav::WAVData wavExample();
    // wavExample.ParseFile("path/to/your/file.wav");

    if (wavExample.GetDataPointer() == nullptr)
    {
        // Parsing did not go as planned, check program output
    }

    std::cout << wavExample << std::endl;

    uint32_t* wavSamples = wavExample.GetSamples();

    // Mono audio data 
    for (int i = 0; i < wavExamples.GetNumberOfSamples(); i++)
    {
        // Sample operations
        // Access sample through *(wavSamples + i)
    }

    // Stereo audio data
    for (int i = 0; i < wavExamples.GetNumberOfSamples(); i++)
    {
        if (i % 2 == 0)
        {
            // Left channel data
            ...
        }
        else
        {
            // Right channel data
            ...
        }
    }

    ...
}
```

## TODO
1) Add WAV Header/Data modification functionality through the API.
2) Provide support for software specific metadata extraction (LIST chunks are currently saved as raw bytes)

## License
The MIT License (MIT)