#include <iostream>
#include <fstream>
#include <cassert>
#include "markdown.h"
#include "boggle.h"
void test_basic_conversion() {
    MarkdownConverter converter;

    std::string markdown = "# Header 1\n## Header 2\n### Header 3";
    std::string expected_html = "<h1>Header 1</h1>\n<h2>Header 2</h2>\n<h3>Header 3</h3>\n";

    std::string result = converter.convert(markdown);
    std::cout << "Test Basic Conversion:\n" << result << "\n\n";

    assert(result == expected_html);
    std::cout << "Test Basic Conversion Passed!\n";
}

void test_text_formatting() {
    MarkdownConverter converter;

    std::string markdown = "**bold** _italic_ ***bold italic***";
    std::string expected_html = "<p><strong>bold</strong> <em>italic</em> <em><strong>bold italic</strong></em></p>";

    std::string result = converter.convert(markdown);
    std::cout << "Test Text Formatting:\n" << result << "\n\n";

    assert(result == expected_html);
    std::cout << "Test Text Formatting Passed!\n";
}

void test_list_conversion() {
    MarkdownConverter converter;

    std::string markdown = "- Item 1\n- Item 2\n1. First\n2. Second";
    std::string expected_html = "<ul>\n<li>Item 1</li>\n<li>Item 2</li>\n<ol>\n<li>First</li>\n<li>Second</li>\n</ul></ol>";

    std::string result = converter.convert(markdown);
    std::cout << "Test List Conversion:\n" << result << "\n\n";

    assert(result == expected_html);
    std::cout << "Test List Conversion Passed!\n";
}

void test_escape_characters() {
    MarkdownConverter converter;

    std::string markdown = "This is <b>bold</b> & copyright &copy;";
    std::string expected_html = "<p>This is &lt;b&gt;bold&lt;/b&gt; &amp; copyright &copy;</p>";

    std::string result = converter.convert(markdown);
    std::cout << "Test Escape Characters:\n" << result << "\n\n";

    assert(result == expected_html);
    std::cout << "Test Escape Characters Passed!\n";
}

void test_file_conversion() {
    std::string input_file = "test.md";
    std::string output_file = "test.html";

    std::ofstream test_input(input_file);
    test_input << "# Test Header\n- Item 1\n- Item 2\n";
    test_input.close();

    MarkdownConverter::markdown_to_html(input_file.c_str(), output_file.c_str());

    std::ifstream test_output(output_file);
    std::string result((std::istreambuf_iterator<char>(test_output)), std::istreambuf_iterator<char>());
    test_output.close();

    std::string expected_html = "<h1>Test Header</h1>\n<ul>\n<li>Item 1</li>\n<li>Item 2</li>\n</ul>";

    std::cout << "Test File Conversion:\n" << result << "\n\n";

    assert(result == expected_html);
    std::cout << "Test File Conversion Passed!\n";
}

int tester() {
    test_basic_conversion(); //html isn't my best language, so I built some general cases and made sure they made "sane" html, but advanced cases were avoided
    test_text_formatting();
    test_list_conversion(); //particularly here, it uses a nested list, but both nested and 2 lists, one unordered followed by an ordered list are both legal 
    test_escape_characters();
    test_file_conversion();

    std::cout << "All Tests Passed!\n";
    return 0;
}

// Test Function
void run_test_case(c_boggle& solver, int width, int height, const string& board_letters, const vector<string>& expected_words) {
    vector<string> found_words = solver.solve_board(width, height, board_letters.c_str());

    cout << "\n=== Test Case (" << width << "x" << height << ") ===" << endl;
    cout << "Board: " << board_letters << endl;
    cout << "Expected words: ";
    for (const auto& word : expected_words) cout << word << " ";
    cout << "\nFound words: ";
    for (const auto& word : found_words) cout << word << " ";

    cout << "\nTest " << (set<string>(found_words.begin(), found_words.end()) == set<string>(expected_words.begin(), expected_words.end()) ? "PASSED" : "FAILED") << endl;
}
int tests() {
    c_boggle solver;

    // 1. Basic 2x2 Board
    vector<string> dictionary1 = { "cat", "art", "rat", "tar" };
    solver.set_legal_words(dictionary1);
    run_test_case(solver, 2, 2, "cart", { "cat", "art", "rat", "tar" });

    // 2. Standard 4x4 Board
    vector<string> dictionary2 = { "tree", "free", "fire", "tire", "reel", "leet" };
    solver.set_legal_words(dictionary2);
    run_test_case(solver, 4, 4, "trfeireetilfrere", { "tree", "free", "tire", "reel" });

    // 3. Larger 5x5 Board
    vector<string> dictionary3 = { "apple", "plane", "plea", "ape", "pen" };
    solver.set_legal_words(dictionary3);
    run_test_case(solver, 5, 5, "aplepplanelepnpeeeeeeeeee", { "apple", "plane", "plea", "ape", "pen" });

    // 4. Edge Case: No Valid Words
    vector<string> empty_dictionary = {};
    solver.set_legal_words(empty_dictionary);
    run_test_case(solver, 3, 3, "abcdefghi", {});

    // 4. Edge Case: No Valid Words
    vector<string> smalll_dictionary = { "ab", };
    solver.set_legal_words(smalll_dictionary);
    run_test_case(solver, 3, 3, "abcdefgjhi", {});

    // 4. Edge Case: rec board
    vector<string> recdict = { "abc", };
    solver.set_legal_words(recdict);
    run_test_case(solver, 2, 3, "abcdef", { "abc" });

    // 4. Edge Case: 0x0 board
    vector<string> recdict2 = { "abc", };
    solver.set_legal_words(recdict);
    run_test_case(solver, 0, 0, "abcdef", { });

    // 5. Edge Case: Single Letter Board
    vector<string> single_letter_dict = { "a", "aa" };
    solver.set_legal_words(single_letter_dict);
    run_test_case(solver, 1, 1, "a", {}); // Now should return {} since "aa" is too short

    // 6. Edge Case: Repeating Letters
    vector<string> repeated_dict = { "aaa", "aa", "aaaa" };
    solver.set_legal_words(repeated_dict);
    run_test_case(solver, 2, 2, "aaaa", { "aaa", "aaaa" }); // "aa" is ignored now

    return 0;
}

void inputmarkdown()
{
	MarkdownConverter::markdown_to_html("input.txt", "output.html");
}
int main2()
{
    tests();
    inputmarkdown();
    tester();
    return 0;
}