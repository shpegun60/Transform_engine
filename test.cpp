#include "test.h"

#include "transform.h"
#include <iostream>
#include <array>
#include <vector>
//#include <span>

// Example transformation classes for testing
class Increment {
public:
    template<typename T>
    T apply(T input) const {
        return input + 1;
    }
};

struct Double {
    template<typename T>
    constexpr T apply(T value) const {
        return value * 2;
    }
};

class Square {
public:
    // Метод для застосування трансформації до елемента
    template<typename T>
    constexpr T apply(T value) const {
        return value * value;
    }
};


void testBasicUsage() {
    // Тест для базового використання з масивом
    Transform<5, int, true, Increment, Double> transform(Increment{}, Double{});
    std::array<int, 5> input = {1, 2, 3, 4, 5};

    bool result = transform.process(input);
    assert(result && "Basic usage failed");
    assert((transform.results() == std::array<int, 5>{{4, 6, 8, 10, 12}}) && "Basic result check failed");
    std::cout << "Basic usage passed.\n";
}

void testVectorInput() {
    // Тест для використання з вектором
    Transform<4, int, true, Increment, Double> transform(Increment{}, Double{});
    std::vector<int> input = {1, 2, 3, 4};

    bool result = transform.process(input);
    assert(result && "Vector input test failed");
    assert((transform.results() == std::array<int, 4>{{4, 6, 8, 10}}) && "Vector result check failed");
    std::cout << "Vector input test passed.\n";
}

void testMySpanInput() {
    // Тест для використання з Span
    Transform<4, int, true, Increment, Double> transform(Increment{}, Double{});
    std::vector<int> input = {1, 2, 3, 4};
    auto span = make_span(input);

    bool result = transform.process(span);
    assert(result && "Span input test failed");
    assert((transform.results() == std::array<int, 4>{{4, 6, 8, 10}}) && "Span result check failed");
    std::cout << "Span input test passed.\n";
}

void testPartialExecution() {
    // Тест для часткового виконання трансформацій
    Transform<3, int, true, Increment, Double> transform(Increment{}, Double{});
    std::array<int, 3> input = {1, 2, 3};

    transform.setFlags(1); // Виконати тільки Increment

    bool result = transform.process(input);
    assert(result && "Partial execution test failed");
    assert((transform.results() == std::array<int, 3>{{2, 3, 4}}) && "Partial result check failed");
    std::cout << "Partial execution passed.\n";
}

void testFlags() {
    // Тест для перевірки флагів
    Transform<3, int, true, Increment, Double> transform(Increment{}, Double{});
    std::array<int, 3> input = {1, 2, 3};

    transform.setFlags(2); // Виконати тільки Double

    bool result = transform.process(input);
    assert(result && "Flags test failed");
    assert((transform.results() == std::array<int, 3>{{2, 4, 6}}) && "Flags result check failed");
    std::cout << "Flags test passed.\n";
}

void testBreakBehavior() {
    // Тест для перевірки поведінки Break
    Transform<5, int, true, Increment, Break, Double> transform(Increment{}, Break{}, Double{});
    std::array<int, 5> input = {1, 2, 3, 4, 5};

    bool result = transform.process(input);
    assert(result && "Process should return true before break");

    // Очікуємо, що трансформації застосовуються тільки до Break
    assert((transform.get_array() == std::array<int, 5>{{2, 3, 4, 5, 6}}) && "Result before break check failed");

    // Виконуємо отримання результатів після Break, перевіряємо, що застосовується трансформація після Break (Double)
    auto finalResults = transform.results();
    assert((finalResults == std::array<int, 5>{{4, 6, 8, 10, 12}}) && "Results after break should be doubled");

    std::cout << "Break behavior test passed.\n";
}


void testFlagsBehavior() {
    // Тест 1: Застосування трансформацій з флагами, всі флаги активовані
    Transform<5, int, true, Increment, Double, Square> transform(Increment{}, Double{}, Square{});
    std::array<int, 5> input = {1, 2, 3, 4, 5};
    transform.setFlags(0xFFFFFFFF); // Усі флаги активовані

    bool result = transform.process(input);
    assert(result && "Process should return true with all flags active");

    // Перевірка результату після всіх трансформацій (до Break)
    assert((transform.get_array() == std::array<int, 5>{{16, 36, 64, 100, 144}}) && "All transformations should be applied");

    // Перевірка результатів після Break, має бути застосовано лише те, що до Break
    auto finalResults = transform.results();
    assert((finalResults == std::array<int, 5>{{16, 36, 64, 100, 144}}) && "Final results should remain as after all transformations before Break");

    // Тест 2: Використання Break та перевірка, що тільки застосування до Break відбулося
    Transform<5, int, true, Increment, Break, Double, Square> transformWithBreak(Increment{}, Break{}, Double{}, Square{});
    transformWithBreak.setFlags(0x07); // Тільки флаги для Increment, Break, і Double активні

    result = transformWithBreak.process(input);
    assert(result && "Process should return true with flags set before break");

    // Перевірка результату після виконання process (тільки Increment застосовується)
    assert((transformWithBreak.get_array() == std::array<int, 5>{{2, 3, 4, 5, 6}}) && "Only Increment transformation should be applied before Break");

    // Перевірка результатів після Break
    finalResults = transformWithBreak.results();
    assert((finalResults == std::array<int, 5>{{4, 6, 8, 10, 12}}) && "Results after Break should remain unchanged");

    // Тест 3: Застосування всіх трансформацій після Break (після Break нічого не відбувається)
    Transform<5, int, true, Increment, Break, Double, Square> transformWithBreakAllActive(Increment{}, Break{}, Double{}, Square{});
    transformWithBreakAllActive.setFlags(0x0F); // Усі флаги активовані

    result = transformWithBreakAllActive.process(input);
    assert(result && "Process should return true with all flags active before break");

    // Перевірка результатів після Break
    finalResults = transformWithBreakAllActive.results();
    assert((finalResults == std::array<int, 5>{{2, 3, 4, 5, 6}}) && "Results after Break should be the same as applied before Break");

    // Тест 4: Перевірка результатів після Break, коли активні тільки трансформації після Break
    Transform<5, int, true, Increment, Break, Double, Square> transformAfterBreakSquare(Increment{}, Break{}, Double{}, Square{});
    transformAfterBreakSquare.setFlags(0x08); // Тільки Square активний після Break

    result = transformAfterBreakSquare.process(input);
    assert(result && "Process should return true with Square flag active after break");
    assert((transformAfterBreakSquare.get_array() == std::array<int, 5>{{2, 3, 4, 5, 6}}) && "Only Increment and Break transformations should be applied before Break");

    // Перевірка результатів після Break (Square повинно бути застосоване після Break)
    finalResults = transformAfterBreakSquare.results();
    assert((finalResults == std::array<int, 5>{{1, 4, 9, 16, 25}}) && "Square transformation should be applied after Break");

    std::cout << "All tests passed!" << std::endl;
}



void test()
{
    std::cout << "Running tests..." << std::endl;

    testBasicUsage();
    testVectorInput();
    testPartialExecution();
    testFlags();
    testBreakBehavior();
    testMySpanInput();
    //testFlagsBehavior();
}
