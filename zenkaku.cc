#include <algorithm>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <string>
#include <vector>

// Include the CLI11 single-header file
#include <CLI/CLI.hpp>

// --- Base Converter Interface ---
class DigitConverter {
public:
  virtual ~DigitConverter() = default;
  virtual void convert(const std::string &input, std::wostream &os) const = 0;
  virtual void reverse(const std::string &input, std::ostream &os) const = 0;
  virtual std::string getName() const = 0;
  virtual bool usesWideOutput() const = 0;
};

// --- Full-Width Converter ---
class FullWidthConverter : public DigitConverter {
public:
  void convert(const std::string &input, std::wostream &os) const override {
    for (char ch : input) {
      if (ch >= '0' && ch <= '9') {
        wchar_t fullWidthDigit = static_cast<wchar_t>(L'\uFF10' + (ch - '0'));
        os << fullWidthDigit;
      } else {
        os << static_cast<wchar_t>(ch);
      }
    }
  }

  void reverse(const std::string &input, std::ostream &os) const override {
    for (size_t i = 0; i < input.length();) {
      // Full-width digits U+FF10 to U+FF19 are encoded as 3 bytes in UTF-8
      if (i + 2 < input.length() &&
          static_cast<unsigned char>(input[i]) == 0xEF &&
          static_cast<unsigned char>(input[i + 1]) == 0xBC &&
          (static_cast<unsigned char>(input[i + 2]) >= 0x90 &&
           static_cast<unsigned char>(input[i + 2]) <= 0x99)) {

        char asciiDigit =
            '0' + (static_cast<unsigned char>(input[i + 2]) - 0x90);
        os << asciiDigit;
        i += 3;
      } else {
        os << input[i];
        i += 1;
      }
    }
  }

  std::string getName() const override { return "fullwidth"; }
  bool usesWideOutput() const override { return true; }
};

// --- Circle-Enclosed Converter ---
class CircleConverter : public DigitConverter {
public:
  void convert(const std::string &input, std::wostream &os) const override {
    for (char ch : input) {
      if (ch >= '0' && ch <= '9') {
        wchar_t circleDigit;
        if (ch == '0') {
          circleDigit = L'\u24EA'; // ⓪
        } else {
          circleDigit = static_cast<wchar_t>(L'\u2460' + (ch - '1')); // ①-⑨
        }
        os << circleDigit;
      } else {
        os << static_cast<wchar_t>(ch);
      }
    }
  }

  void reverse(const std::string &input, std::ostream &os) const override {
    for (size_t i = 0; i < input.length(); ++i) {
      // Circled Digits 1-9: U+2460 to U+2468
      if (i + 2 < input.length() &&
          static_cast<unsigned char>(input[i]) == 0xE2 &&
          static_cast<unsigned char>(input[i + 1]) == 0x91 &&
          (static_cast<unsigned char>(input[i + 2]) >= 0xA0 &&
           static_cast<unsigned char>(input[i + 2]) <= 0xA8)) {

        char asciiDigit = '1' + (input[i + 2] - 0xA0);
        os << asciiDigit;
        i += 2;
      }
      // Circled Digit Zero: U+24EA
      else if (i + 2 < input.length() &&
               static_cast<unsigned char>(input[i]) == 0xE2 &&
               static_cast<unsigned char>(input[i + 1]) == 0x92 &&
               static_cast<unsigned char>(input[i + 2]) == 0xAA) {

        os << '0';
        i += 2;
      } else {
        os << input[i];
      }
    }
  }

  std::string getName() const override { return "circle"; }
  bool usesWideOutput() const override { return true; }
};

// --- Roman Numeral Converter ---
class RomanConverter : public DigitConverter {
private:
  const std::vector<std::wstring> romanNumerals = {
      L"０", L"Ⅰ", L"Ⅱ", L"Ⅲ", L"Ⅳ", L"Ⅴ", L"Ⅵ", L"Ⅶ", L"Ⅷ", L"Ⅸ"};

public:
  void convert(const std::string &input, std::wostream &os) const override {
    for (char ch : input) {
      if (ch >= '0' && ch <= '9') {
        os << romanNumerals[ch - '0'];
      } else {
        os << static_cast<wchar_t>(ch);
      }
    }
  }

  void reverse(const std::string &input, std::ostream &os) const override {
    // Note: This is a simplified reverse implementation
    // A full implementation would need to handle UTF-8 decoding properly
    for (size_t i = 0; i < input.length();) {
      bool found = false;

      // Check for Roman numerals (simplified approach)
      if (i + 2 < input.length()) {
        // Check for specific UTF-8 sequences of Roman numerals
        // This is simplified - in practice, you'd want a more robust UTF-8
        // decoder

        // Full-width 0 (０): U+FF10
        if (static_cast<unsigned char>(input[i]) == 0xEF &&
            static_cast<unsigned char>(input[i + 1]) == 0xBC &&
            static_cast<unsigned char>(input[i + 2]) == 0x90) {
          os << '0';
          i += 3;
          found = true;
        }
        // Roman I (Ⅰ): U+2160
        else if (static_cast<unsigned char>(input[i]) == 0xE2 &&
                 static_cast<unsigned char>(input[i + 1]) == 0x85 &&
                 static_cast<unsigned char>(input[i + 2]) == 0xA0) {
          os << '1';
          i += 3;
          found = true;
        }
        // Add more Roman numeral reversals as needed...
      }

      if (!found) {
        os << input[i];
        i += 1;
      }
    }
  }

  std::string getName() const override { return "roman"; }
  bool usesWideOutput() const override { return true; }
};

// --- Chinese Numeral Converter ---
class ChineseConverter : public DigitConverter {
private:
  const std::vector<std::wstring> chineseNumerals = {
      L"〇", L"一", L"二", L"三", L"四", L"五", L"六", L"七", L"八", L"九"};

public:
  void convert(const std::string &input, std::wostream &os) const override {
    for (char ch : input) {
      if (ch >= '0' && ch <= '9') {
        os << chineseNumerals[ch - '0'];
      } else {
        os << static_cast<wchar_t>(ch);
      }
    }
  }

  void reverse(const std::string &input, std::ostream &os) const override {
    // Simplified reverse implementation
    // In practice, you'd implement proper UTF-8 decoding for Chinese characters
    for (size_t i = 0; i < input.length();) {
      bool found = false;

      if (i + 2 < input.length()) {
        // Check for specific Chinese numeral UTF-8 sequences
        // 〇 (U+3007): E3 80 87
        if (static_cast<unsigned char>(input[i]) == 0xE3 &&
            static_cast<unsigned char>(input[i + 1]) == 0x80 &&
            static_cast<unsigned char>(input[i + 2]) == 0x87) {
          os << '0';
          i += 3;
          found = true;
        }
        // 一 (U+4E00): E4 B8 80
        else if (static_cast<unsigned char>(input[i]) == 0xE4 &&
                 static_cast<unsigned char>(input[i + 1]) == 0xB8 &&
                 static_cast<unsigned char>(input[i + 2]) == 0x80) {
          os << '1';
          i += 3;
          found = true;
        }
        // Add more Chinese numeral reversals as needed...
      }

      if (!found) {
        os << input[i];
        i += 1;
      }
    }
  }

  std::string getName() const override { return "chinese"; }
  bool usesWideOutput() const override { return true; }
};

// --- Thai Numeral Converter ---
class ThaiConverter : public DigitConverter {
private:
  const std::vector<std::wstring> thaiNumerals = {L"๐", L"๑", L"๒", L"๓", L"๔",
                                                  L"๕", L"๖", L"๗", L"๘", L"๙"};

public:
  void convert(const std::string &input, std::wostream &os) const override {
    for (char ch : input) {
      if (ch >= '0' && ch <= '9') {
        os << thaiNumerals[ch - '0'];
      } else {
        os << static_cast<wchar_t>(ch);
      }
    }
  }

  void reverse(const std::string &input, std::ostream &os) const override {
    for (size_t i = 0; i < input.length();) {
      bool found = false;

      if (i + 2 < input.length()) {
        // Thai digits U+0E50 to U+0E59
        if (static_cast<unsigned char>(input[i]) == 0xE0 &&
            static_cast<unsigned char>(input[i + 1]) == 0xB9 &&
            (static_cast<unsigned char>(input[i + 2]) >= 0x90 &&
             static_cast<unsigned char>(input[i + 2]) <= 0x99)) {

          char asciiDigit =
              '0' + (static_cast<unsigned char>(input[i + 2]) - 0x90);
          os << asciiDigit;
          i += 3;
          found = true;
        }
      }

      if (!found) {
        os << input[i];
        i += 1;
      }
    }
  }

  std::string getName() const override { return "thai"; }
  bool usesWideOutput() const override { return true; }
};

// --- Converter Registry ---
class ConverterRegistry {
private:
  std::map<std::string, std::unique_ptr<DigitConverter>> converters;

public:
  ConverterRegistry() {
    // Register all available converters
    registerConverter(std::make_unique<FullWidthConverter>());
    registerConverter(std::make_unique<CircleConverter>());
    registerConverter(std::make_unique<RomanConverter>());
    registerConverter(std::make_unique<ChineseConverter>());
    registerConverter(std::make_unique<ThaiConverter>());
  }

  void registerConverter(std::unique_ptr<DigitConverter> converter) {
    std::string name = converter->getName();
    converters[name] = std::move(converter);
  }

  const DigitConverter *getConverter(const std::string &name) const {
    auto it = converters.find(name);
    return (it != converters.end()) ? it->second.get() : nullptr;
  }

  std::vector<std::string> getAvailableTypes() const {
    std::vector<std::string> types;
    for (const auto &pair : converters) {
      types.push_back(pair.first);
    }
    return types;
  }
};

int main(int argc, char **argv) {
  // Setup converter registry
  ConverterRegistry registry;

  // CLI11 Setup
  CLI::App app{"Convert digits in text to various Unicode formats or reverse."};

  std::string conversion_type = "fullwidth";
  auto available_types = registry.getAvailableTypes();
  std::string type_list = "Available types: ";
  for (size_t i = 0; i < available_types.size(); ++i) {
    if (i > 0)
      type_list += ", ";
    type_list += available_types[i];
  }

  app.add_option("-t,--type", conversion_type, "Conversion type. " + type_list)
      ->check(CLI::IsMember(available_types))
      ->group("Conversion Options");

  bool reverse_option = false;
  app.add_flag("-r,--reverse", reverse_option,
               "Reverse conversion from Unicode digits back to ASCII.")
      ->group("Conversion Options");

  std::vector<std::string> input_args;
  app.add_option("text", input_args,
                 "Text arguments to convert. If empty, reads from stdin.")
      ->allow_extra_args(false)
      ->multi_option_policy(CLI::MultiOptionPolicy::Join);

  CLI11_PARSE(app, argc, argv);

  // Get the selected converter
  const DigitConverter *converter = registry.getConverter(conversion_type);
  if (!converter) {
    std::cerr << "Error: Unknown conversion type '" << conversion_type << "'"
              << std::endl;
    return 1;
  }

  // Locale Setup
  std::locale::global(std::locale("en_US.utf8"));

  // Set up output stream based on conversion mode
  if (!reverse_option && converter->usesWideOutput()) {
    std::wcout.imbue(std::locale("en_US.utf8"));
  }

  // Process Input
  auto processText = [&](const std::string &text) {
    if (reverse_option) {
      converter->reverse(text, std::cout);
      std::cout << '\n';
    } else {
      if (converter->usesWideOutput()) {
        converter->convert(text, std::wcout);
        std::wcout << L'\n';
      } else {
        // For future converters that might use narrow output
        std::stringstream ss;
        std::wstringstream wss;
        converter->convert(text, wss);
        // Convert wide to narrow (simplified)
        std::wcout << wss.str() << L'\n';
      }
    }
  };

  if (input_args.empty()) {
    // Read from stdin
    std::string line;
    while (std::getline(std::cin, line)) {
      processText(line);
    }
  } else {
    // Process direct arguments
    for (const std::string &arg : input_args) {
      processText(arg);
    }
  }

  return 0;
}
