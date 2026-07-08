# displays

Display driver libraries live here (OLED, TFT, e-paper, etc.).

No first-party display drivers have been extracted yet. The BeerCompass project
currently renders to an SSD1306 using the upstream `adafruit/Adafruit SSD1306`
library pulled in through `lib_deps`, so there is nothing to migrate here today.

Add new display libraries following the same PlatformIO layout used elsewhere in
this repo:

```
displays/
└── <DisplayName>/
    ├── src/
    │   ├── <DisplayName>.cpp
    │   └── <DisplayName>.h
    ├── examples/
    │   └── basic/
    │       └── main.cpp
    ├── library.json
    └── README.md
```
